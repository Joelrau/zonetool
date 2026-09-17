#include "stdafx.hpp"
#include "Utils/Math.hpp"
#include "../Include.hpp"

#include "ClipMap.hpp"
#include "ClipMapCollision.hpp"
#include "ParticleSystem.hpp"

#include "Common/havok_builder.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <regex>
#include <string>
#include <numbers>

namespace ZoneTool::IW5
{
	namespace IW7Converter
	{
		namespace
		{
			// The dummy PhysicsAsset IW7 attaches to script brush models. It supplies the
			// body/motion properties; physicsShapeOverrideIdx replaces the shape it carries
			// with one from MapEnts::havokEntsShapeData. Dumped stock IW7 maps ship it under
			// physicsasset/, so a converted zone can reference it by name -- but the zone has
			// to actually contain it, or the brush model silently gets no body.
			constexpr auto BRUSHMODEL_PHYSICS_ASSET = "scriptbrushmodeldummydefault";

			// The trigger equivalent. Stock gives every trigger a Havok compound as well, for
			// physics bodies overlapping the volume; the script-facing trigger path does not
			// need it and runs off the slab hulls alone (docs/iw7-triggers.md).
			constexpr auto TRIGGER_PHYSICS_ASSET = "triggermodeldummydefault";

			// The engine's "all valid contents" mask -- the union of every collisionFilterInfo
			// bit across all shipped blobs. Stock stores it in shapeContents for triggers.
			constexpr auto ENTS_TRIGGER_CONTENTS = 0xC7FFBFFFu;

			// ShapeTagData::userData bit 48: this surface came from a brush. IW7's player cast
			// sweeps its separate non-brush shape against anything without it.
			constexpr auto ENTS_BRUSH_BASIS = 1ull << 48;

			// Attaching Havok bodies to triggers is off by default. Script triggers already
			// work without them, and a body with the wrong quality would make a trigger
			// solid -- a worse regression than not having one at all. Set
			// ZT_HAVOK_TRIGGER_SHAPES=1 to generate them and test.
			bool trigger_shapes_enabled()
			{
				const auto* env = std::getenv("ZT_HAVOK_TRIGGER_SHAPES");
				return env && env[0] == '1';
			}

			// Brush-model Havok shapes are ON by default -- without them "model" "*N"
			// entities get no physics body at all. Set ZT_HAVOK_ENTS_SHAPES=0 to emit a null
			// havokEntsShapeData instead, which is what shipped before this list existed.
			//
			// This exists purely to bisect. WorldCollision_AddMapEnts skips a zero-sized blob
			// entirely, so turning it off restores exactly the previous runtime behaviour and
			// isolates whether a crash comes from this blob or from somewhere else.
			bool ents_shapes_enabled()
			{
				const auto* env = std::getenv("ZT_HAVOK_ENTS_SHAPES");
				return !(env && env[0] == '0');
			}

			// IW7 keeps trigger volumes exactly where IW5 does: MapTriggers, a flat array of
			// TriggerModels indexed straight from the entity string by "model" "?N", each
			// owning a run of TriggerHulls (an entity-local AABB plus optional slab planes).
			// It is not Havok data -- the runtime trigger path (SV_SetTriggerModel ->
			// CM_TriggerModelBounds / CM_ContentsOfTriggerModel -> SV_LinkEntity) reads these
			// arrays and never touches TriggerModel::physicsAsset. See docs/iw7-triggers.md.
			//
			// TriggerHull and TriggerSlab are byte-identical between the two games, so they
			// pass through by pointer (REINTERPRET_CAST_SAFE static_asserts that). Only
			// TriggerModel grew -- 8 bytes to 32 -- so it needs a real copy.
			void convert_map_triggers(const MapTriggers& src, IW7::MapTriggers& dst,
				allocator& allocator)
			{
				dst.count = src.count;
				dst.models = allocator.allocate<IW7::TriggerModel>(src.count);
				for (unsigned int i = 0; i < src.count; i++)
				{
					// contents needs no remapping: IW5 and IW7 agree bit for bit here
					// (every shipped map of either game uses 0x28000001 / 0x28000000 /
					// 0x28004000 and nothing else).
					dst.models[i].contents = src.models[i].contents;
					dst.models[i].hullCount = src.models[i].hullCount;
					dst.models[i].firstHull = src.models[i].firstHull;

					// Windings are an IW7 addition that no shipped IW7 map uses -- all five
					// stock maps checked have windingCount == 0 -- and IW5 has no source for
					// them, so they stay empty.
					dst.models[i].windingCount = 0;
					dst.models[i].firstWinding = 0;
					dst.models[i].flags = 0;

					// Both of these are the Havok side of a trigger, used when a physics body
					// rather than the player overlaps the volume. Stock maps point physicsAsset
					// at a dummy PhysicsAsset and index a real shape in
					// MapEnts::havokEntsShapeData; we generate that list empty, and the runtime
					// only reads the override when physicsAsset is non-null anyway. The
					// script-facing trigger path reads neither, so this costs script triggers
					// nothing. 0xFFFF is IW7's "no shape override".
					dst.models[i].physicsAsset = nullptr;
					dst.models[i].physicsShapeOverrideIdx = 0xFFFF;
				}

				dst.hullCount = src.hullCount;
				REINTERPRET_CAST_SAFE_TO_FROM(dst.hulls, src.hulls);
				dst.slabCount = src.slabCount;
				REINTERPRET_CAST_SAFE_TO_FROM(dst.slabs, src.slabs);

				dst.windingCount = 0;
				dst.windings = nullptr;
				dst.windingPointCount = 0;
				dst.windingPoints = nullptr;
			}

			// Entities reach a trigger volume through "model" "?N", N being a zero-based index
			// into trigger.models -- same convention in both games (checked against 18 stock
			// IW5 maps and 7 stock IW7 ones). A reference past the end of the array is a
			// silent out-of-bounds read at map load, so count them rather than trusting it.
			void audit_trigger_references(const char* entity_string, const int num_chars,
				const unsigned int model_count)
			{
				if (!entity_string || num_chars <= 0)
				{
					return;
				}

				const std::string ents{entity_string, static_cast<size_t>(num_chars)};

				auto refs = 0;
				auto out_of_range = 0;
				auto highest = -1;

				for (size_t pos = ents.find("\"?"); pos != std::string::npos;
					pos = ents.find("\"?", pos + 2))
				{
					auto digits = pos + 2;
					auto end = digits;
					while (end < ents.size() && ents[end] >= '0' && ents[end] <= '9')
					{
						end++;
					}

					// "?" followed by anything but digits and a closing quote is not a
					// brushmodel reference.
					if (end == digits || end >= ents.size() || ents[end] != '"')
					{
						continue;
					}

					const auto index = std::atoi(ents.substr(digits, end - digits).data());
					refs++;
					highest = std::max(highest, index);
					if (index < 0 || static_cast<unsigned int>(index) >= model_count)
					{
						out_of_range++;
					}
				}

				ZONETOOL_INFO("mapents: %u trigger models, %d referenced by entities "
					"(highest \"?%d\")", model_count, refs, highest);

				if (out_of_range > 0)
				{
					ZONETOOL_WARNING("mapents: %d entity trigger reference(s) point past the "
						"end of the %u-model trigger array", out_of_range, model_count);
				}
			}

			bool same_bounds(const Bounds& left, const Bounds& right)
			{
				constexpr auto epsilon = 0.01f;
				for (auto axis = 0; axis < 3; axis++)
				{
					if (std::fabs(left.midPoint[axis] - right.midPoint[axis]) > epsilon ||
						std::fabs(left.halfSize[axis] - right.halfSize[axis]) > epsilon)
					{
						return false;
					}
				}
				return true;
			}

			// IW5 map sources use "*N" for both brush models and trigger volumes. IW7
			// keeps brush models at "*N", but script triggers must use "?N" to index
			// MapTriggers. Only make that substitution when the source cmodel is an exact
			// geometric match for exactly one TriggerModel; this prevents an incidental
			// matching index from turning an ordinary brush model into a trigger.
			int trigger_for_cmodel(const clipMap_t* clipmap, const unsigned int cmodel)
			{
				if (!clipmap || cmodel >= clipmap->numSubModels)
				{
					return -1;
				}

				const auto& bounds = clipmap->cmodels[cmodel].bounds;
				auto match = -1;
				for (unsigned int model = 0; model < clipmap->mapEnts->trigger.count; model++)
				{
					const auto& trigger = clipmap->mapEnts->trigger.models[model];
					for (unsigned int hull = trigger.firstHull;
						hull < trigger.firstHull + trigger.hullCount; hull++)
					{
						if (hull >= clipmap->mapEnts->trigger.hullCount ||
							!same_bounds(bounds, clipmap->mapEnts->trigger.hulls[hull].bounds))
						{
							continue;
						}

						if (match != -1 && match != static_cast<int>(model))
						{
							return -1;
						}
						match = static_cast<int>(model);
					}
				}
				return match;
			}

			std::string fix_entity_model_references(const clipMap_t* clipmap,
				const std::string& source)
			{
				static const std::regex classname_expr(
					R"entity("classname"\s+"([^"]*)")entity");
				static const std::regex model_expr(
					R"entity("model"\s+"([*?])(\d+)")entity");

				std::string result;
				result.reserve(source.size() + 128);
				size_t cursor = 0;
				auto rewritten_triggers = 0;
				auto wired_brushmodels = 0;

				while (cursor < source.size())
				{
					const auto open = source.find('{', cursor);
					if (open == std::string::npos)
					{
						result.append(source, cursor, std::string::npos);
						break;
					}
					const auto close = source.find('}', open + 1);
					if (close == std::string::npos)
					{
						// Do not attempt to repair malformed entity text here.
						result.append(source, cursor, std::string::npos);
						break;
					}

					result.append(source, cursor, open - cursor);
					auto entity = source.substr(open, close - open + 1);
					std::smatch classname;
					std::smatch model;
					if (std::regex_search(entity, classname, classname_expr) &&
						std::regex_search(entity, model, model_expr))
					{
						const auto entity_class = classname[1].str();
						const auto model_kind = model[1].str()[0];
						const auto model_index = static_cast<unsigned int>(
							std::strtoul(model[2].str().c_str(), nullptr, 10));

						if (entity_class.starts_with("trigger_") && model_kind == '*')
						{
							const auto trigger = trigger_for_cmodel(clipmap, model_index);
							if (trigger >= 0)
							{
								const auto value_pos = static_cast<size_t>(model.position(2));
								const auto value_len = static_cast<size_t>(model.length(2));
								entity.replace(value_pos, value_len, std::to_string(trigger));
								entity[static_cast<size_t>(model.position(1))] = '?';
								rewritten_triggers++;
							}
						}
						else if (entity_class == "script_brushmodel" && model_kind == '*' &&
							entity.find("\"physicsasset\"") == std::string::npos)
						{
							// This is the spelling used by the target entity parser. It selects the
							// dummy's motion/body configuration; cmodel::physicsAsset supplies the
							// actual asset and the shape override supplies its geometry.
							entity.insert(entity.size() - 1,
								"\"physicsasset\" \"scriptbrushmodeldummydefault\"\n");
							wired_brushmodels++;
						}
					}

					result.append(entity);
					cursor = close + 1;
				}

				if (rewritten_triggers || wired_brushmodels)
				{
					ZONETOOL_INFO("mapents: rewrote %d trigger model reference(s), wired %d "
						"script brush model physics asset(s)", rewritten_triggers, wired_brushmodels);
				}
				return result;
			}
		}

		IW7::ScriptableDef* generate_scriptable_def_from_dynent(const DynEntityDef* dynent, allocator& allocator)
		{
			auto generate_name = [](const DynEntityDef* dynent) -> std::string
			{
				std::string name;
				name.reserve(96);
				name.append(dynent->xModel ? dynent->xModel->name : "dynent");
				name.append("_destruct_");
				name.append(std::to_string(dynent->health));
				return name;
			};

			auto* new_def = allocator.allocate<IW7::ScriptableDef>();
			new_def->name = allocator.duplicate_string(generate_name(dynent));
			// Health-state scriptables carry HAS_HEALTH in the root flags.  The
			// stock mp_fallen watermelon is 0x81 (0x80 | 0x1), not just 0x80.
			new_def->flags = IW7::SCRIPTABLE_DEFFLAG_HAS_HEALTH | 0x80;
			new_def->type = 0;
			new_def->nextScriptableDef = nullptr;
			new_def->numParts = 1;
			new_def->parts = allocator.allocate<IW7::ScriptablePartDef>(1);
			new_def->maxNumDynEntsRequired = 0;
			new_def->partCount = 1;
			// This is a client-instanced map prop; stock watermelon uses zero
			// server-instanced parts even though its first state is Health.
			new_def->serverInstancedPartCount = 0;
			new_def->serverControlledPartCount = 0;
			new_def->maxNumDynEntPartsBase = 1;
			new_def->maxNumDynEntPartsForSpawning = 0;
			new_def->eventStreamSizeRequiredServer = 0;
			new_def->eventStreamSizeRequiredClient = 0;
			new_def->eventStreamSize = 4;
			new_def->ffMemCost = 0;
			new_def->animationTreeName = 0;
			new_def->animationTreeDef[0] = nullptr;
			new_def->animationTreeDef[1] = nullptr;
			new_def->numXModels = dynent->xModel ? 1 : 0;
			new_def->models = new_def->numXModels
				? allocator.allocate<IW7::XModel PTR64>(new_def->numXModels)
				: nullptr;

			if (new_def->models)
			{
				new_def->models[0] = reinterpret_cast<IW7::XModel*>(dynent->xModel);
			}

			auto* part = new_def->parts;
			part->name = "";
			// Stock map scriptable parts use 0x180 for an active part.  The
			// 0x100 bit is present even on the simple one-part definitions that
			// back ordinary map props; omitting it leaves the part inactive.
			part->flags = 0x180;
			part->flatId = 0;
			part->serverInstanceFlatId = 0;
			part->serverControlledFlatId = 0;
			part->eventStreamBufferOffsetServer = 0;
			part->eventStreamBufferOffsetClient = 0;
			// The root reserves the four-byte health stream, but this one-part
			// definition does not assign a per-part stream range.  Stock watermelon
			// has a zero part eventStreamSize; using four here shifts the runtime
			// event buffer layout.
			part->eventStreamSize = 0;
			part->numStates = 2;
			part->states = allocator.allocate<IW7::ScriptableStateDef>(2);

			auto* healthy_state = &part->states[0];

			healthy_state->base.name = allocator.duplicate_string("healthy");
			healthy_state->base.flags = 0x80;
			healthy_state->base.numEvents = 1;
			healthy_state->base.events = allocator.allocate<IW7::ScriptableEventDef>(1);
			healthy_state->type = IW7::Scriptable_StateType_Health;

			// Stock compiled health states point the specialized base at the state
			// base itself.  Keep that identity instead of allocating a duplicate.
			healthy_state->data.health.base = &healthy_state->base;

			healthy_state->data.health.health = std::max(dynent->health, 1);
			healthy_state->data.health.minimumDamage = 0;
			healthy_state->data.health.damagePropagationFromParent = 1.0f;
			healthy_state->data.health.damagePropagationFromChild = 1.0f;
			// The stock map definition leaves the optional script identifier null;
			// a zero scrScript_id means no health callback is registered.
			healthy_state->data.health.script_id = nullptr;
			healthy_state->data.health.scrScript_id = 0;

			auto* model_event = &healthy_state->base.events[0];
			model_event->base.name = "";
			model_event->type = IW7::Scriptable_EventType_Model;
			model_event->data.model.base = &model_event->base;
			model_event->data.model.model = reinterpret_cast<IW7::XModel*>(dynent->xModel);
			model_event->data.model.dynamicSimulation = false;
			model_event->data.model.activatePhysics = false;
			model_event->data.model.hudOutlineColor = 0;
			model_event->data.model.hudOutlineActive = true;
			model_event->data.model.hudOutlineFill = false;
			model_event->data.model.neverMoves = false;

			auto* dead_state = &part->states[1];

			dead_state->base.name = allocator.duplicate_string("dead");
			dead_state->base.flags = 0;
			// stock p7_food_fruit_watermelon's dead state starts with a Model event
			// whose model is null; that is what removes the healthy model.  Without
			// it the prop stays visible after the destroy fx plays.
			dead_state->base.numEvents = dynent->destroyFx ? 2 : 1;
			dead_state->base.events = allocator.allocate<IW7::ScriptableEventDef>(dead_state->base.numEvents);
			dead_state->type = IW7::Scriptable_StateType_Simple;

			dead_state->data.simple.base = allocator.allocate<IW7::ScriptableStateBaseDef>();
			dead_state->data.simple.base->name = dead_state->base.name;
			dead_state->data.simple.base->flags = dead_state->base.flags;
			dead_state->data.simple.base->numEvents = dead_state->base.numEvents;
			dead_state->data.simple.base->events = dead_state->base.events;

			auto* hide_event = &dead_state->base.events[0];
			hide_event->base.name = "";
			hide_event->type = IW7::Scriptable_EventType_Model;
			hide_event->data.model.base = &hide_event->base;
			hide_event->data.model.model = nullptr;
			// byte pattern copied from the stock dead-state event (01 00 00 00 00 00)
			hide_event->data.model.hudOutlineColor = 1;
			hide_event->data.model.hudOutlineActive = false;
			hide_event->data.model.hudOutlineFill = false;
			hide_event->data.model.neverMoves = false;
			hide_event->data.model.dynamicSimulation = false;
			hide_event->data.model.activatePhysics = false;

			if (dynent->destroyFx)
			{
				auto* destroy_event = &dead_state->base.events[1];
				destroy_event->base.name = "";
				destroy_event->type = IW7::Scriptable_EventType_PFX;
				destroy_event->data.particleFX.base = &destroy_event->base;
				destroy_event->data.particleFX.stateful = false;
				// IW7 scriptable PFX events are serialized as ParticleSystemDef/VFX
				// references.  The IW5 source field is an FxEffectDef, but the IW7
				// dumper/converter emits that asset as a ParticleSystemDef.  Marking
				// this as FX_COMBINED_FX makes the game interpret the VFX pointer as
				// an FxEffectDef and crash while walking its elemDefs (0x140A1BDF4).
				// FxEffectDef and ParticleSystemDef are different IW7 asset
				// layouts.  Reinterpreting the IW5 pointer makes the particle
				// renderer read FxElemDef data as ParticleEmitterDef data, which
				// is the crash seen in the emitter draw path.  The effect itself is
				// converted when its own asset is dumped; the scriptable only
				// serializes the name.  Don't convert it here: on the IW3 path
				// destroyFx is the IW3 FxEffectDef cast straight to the IW5 type
				// (IW3/IW4 ClipMap), so everything past the counts is garbage.
				auto* vfx = allocator.manual_allocate<IW7::ParticleSystemDef>(sizeof(const char*));
				vfx->name = allocator.duplicate_string(dynent->destroyFx->name);
				destroy_event->data.particleFX.effectDef.u.vfx = vfx;
				destroy_event->data.particleFX.effectDef.type = IW7::FX_COMBINED_VFX;
				destroy_event->data.particleFX.eventStreamBufferOffsetClient = 0;
			}

			return new_def;
		}

		// `world_tags` is the shapeTagData table the map's world blob emitted, and it is
		// handed straight to the ents shape list as its required prefix. IW7 keeps ONE
		// shape-tag decoder for the whole map, so the ents list's table is the table the
		// world mesh's own tags are decoded against -- see the note on
		// havok::builder::shape_tag. Empty means no world blob was built.
		IW7::MapEnts* generate_mapents(clipMap_t* clipmap, allocator& allocator,
			const std::vector<ZoneTool::IW7::havok::builder::shape_tag>& world_tags)
		{
			const auto* asset = clipmap->mapEnts;
			if (!asset)
			{
				ZONETOOL_ERROR("clipmap \"%s\" has no mapents -- entities and triggers cannot "
					"be converted", clipmap->name);
				return nullptr;
			}

			auto* new_asset = allocator.allocate<IW7::MapEnts>();
			REINTERPRET_CAST_SAFE(name);

			std::string entity_string;
			if (ZoneTool::currentlinkermode == ZoneTool::linker_mode::iw5)
			{
				entity_string = ::mapents::converter::iw5::convert_mapents_ids(
					std::string{ asset->entityString, static_cast<size_t>(asset->numEntityChars) });
			}
			else
			{
				entity_string.assign(asset->entityString, static_cast<size_t>(asset->numEntityChars));
			}
			// An IW3 source reaches this IW5->IW7 converter with linker_mode::iw3, so this
			// target-specific rewrite must not be gated on the IW5 numeric-token conversion.
			entity_string = fix_entity_model_references(clipmap, entity_string);
			new_asset->entityString = const_cast<char*>(allocator.duplicate_string(entity_string));
			new_asset->numEntityChars = static_cast<int>(entity_string.size());

			// Script triggers (trigger_multiple, trigger_use, trigger_hurt, ...) and the
			// client-side ones (vision sets, reverb zones) both live here.
			convert_map_triggers(asset->trigger, new_asset->trigger, allocator);
			convert_map_triggers(asset->clientTrigger.trigger, new_asset->clientTrigger.trigger,
				allocator);

			audit_trigger_references(new_asset->entityString, new_asset->numEntityChars,
				new_asset->trigger.count);

			COPY_VALUE(clientTrigger.triggerStringLength);
			REINTERPRET_CAST_SAFE(clientTrigger.triggerString);

			auto allocate_uchar = [&](const unsigned char default_value = -1)
			{
				const auto count = asset->clientTrigger.trigger.count;
				auto memory = allocator.allocate<unsigned char>(asset->clientTrigger.trigger.count);
				std::fill(memory, memory + count, default_value);
				return memory;
			};

			auto allocate = [&](const short default_value = -1)
			{
				const auto count = asset->clientTrigger.trigger.count;
				auto memory = allocator.allocate<short>(asset->clientTrigger.trigger.count);
				std::fill(memory, memory + count, default_value);
				return memory;
			};

			new_asset->clientTrigger.triggerType = allocate_uchar(0);
			new_asset->clientTrigger.visionSetTriggers = allocate();

			for (auto i = 0; i < asset->clientTrigger.trigger.count; i++)
			{
				if ((asset->clientTrigger.triggerType[i] & CLIENT_TRIGGER_VISIONSET) != 0)
				{
					new_asset->clientTrigger.triggerType[i] |= IW7::CLIENT_TRIGGER_VISIONSET;

					new_asset->clientTrigger.visionSetTriggers[i] = asset->clientTrigger.triggerStringOffsets[i];
				}
			}

			new_asset->clientTrigger.origins = reinterpret_cast<float(*__ptr64)[3]>(asset->clientTrigger.origins);
			new_asset->clientTrigger.scriptDelay = asset->clientTrigger.scriptDelay;
			new_asset->clientTrigger.audioTriggers = asset->clientTrigger.audioTriggers;
			new_asset->clientTrigger.blendLookup = allocate(); // todo?
			new_asset->clientTrigger.npcTriggers = allocate(); // todo?

			new_asset->clientTrigger.audioStateIds = allocator.allocate<short>(asset->clientTrigger.trigger.count);
			new_asset->clientTrigger.audioRvbPanInfo = allocator.allocate<IW7::CTAudRvbPanInfo>(asset->clientTrigger.trigger.count);
			new_asset->clientTrigger.transientIndex = allocator.allocate<short>(asset->clientTrigger.trigger.count);
			for (unsigned int i = 0; i < asset->clientTrigger.trigger.count; i++)
			{
				new_asset->clientTrigger.audioStateIds[i] = -1;

				new_asset->clientTrigger.audioRvbPanInfo[i].hasCustomPosition = false;
				// has some more data..

				new_asset->clientTrigger.transientIndex[i] = 0;
			}

			new_asset->clientTrigger.linkTo = allocator.allocate<IW7::ClientEntityLinkToDef PTR64>(asset->clientTrigger.trigger.count);
			for (unsigned int i = 0; i < asset->clientTrigger.trigger.count; i++)
			{
				new_asset->clientTrigger.linkTo[i] = nullptr;
			}

			new_asset->clientTriggerBlend.numClientTriggerBlendNodes = 0;
			new_asset->clientTriggerBlend.blendNodes = nullptr;

			new_asset->spawnList.spawnsCount = 0;
			new_asset->spawnList.spawns = nullptr; // later: generate spawnents with mapents2spawns

			new_asset->splineList.splineCount = 0;
			new_asset->splineList.splines = nullptr;

			// The per-entity Havok shape list: one convex compound per IW5 brush model, so
			// that "model" "*N" entities get a physics body instead of nothing. It has to
			// exist even when empty -- CM_ContentsOfBrushModel reads shapeContents off this
			// list for every brush-model entity without checking the list pointer, and the
			// pointer is only ever set by WorldCollision_AddMapEnts, which skips a zero-sized
			// blob entirely. See docs/iw7-ents-shapes.md.
			new_asset->havokEntsShapeDataSize = 0;
			new_asset->havokEntsShapeData = nullptr;

			// cmodel index -> slot in the shape list, filled in below.
			std::vector<unsigned short> cmodel_shape_index(clipmap->numSubModels, 0xFFFF);
			{
				ZoneTool::IW7::havok::builder::ents_input ents{};
				ents.world_tags = world_tags;
				if (world_tags.empty())
				{
					// The world blob is built first precisely so this cannot happen. Without
					// it the ents table is ordered by whatever the brush models need, and
					// since the runtime decodes EVERY shape against this one table, the world
					// mesh's tags resolve against the wrong records -- floors and walls pick
					// up someone else's collision filter and userData, or run off the end of
					// the table entirely, and the player walks through them while rays and
					// bullets still hit.
					ZONETOOL_WARNING("mapents: no world shape tag table for \"%s\" -- the "
						"world mesh's tags will resolve against the ents table instead of "
						"its own, giving world surfaces the wrong filters", asset->name);
				}

				// Collected only when ZT_HAVOK_OBJ_DIR is set; see the note on the dump
				// helpers in ClipMapCollision.hpp.
				const auto dump_obj = collision::obj_dump_enabled();
				std::vector<collision::hull_group> ents_obj;

				const auto brush_models = collision::extract_brush_models(clipmap);
				for (const auto& model : brush_models)
				{
					if (dump_obj)
					{
						ents_obj.emplace_back(collision::hull_group{
							va("brushmodel_%u", model.index), model.hulls});
					}

					ZoneTool::IW7::havok::builder::ents_shape shape{};
					// A brush model keeps its own mask in both fields -- stock breakneck stores
					// 0x30200 and 0x30000 on 36 of its shapes, and its tags carry real contents
					// (0x1, 0x2080, 0x30200) rather than a wildcard. The compile-only bits come
					// off in the builder: ours used to carry CONTENTS_DETAIL (0x08000001,
					// 0x08031640), which IW7's collision filter rejects outright.
					// Same rule as the world mesh, and the same correction: a solid brush model
					// keeps CONTENTS_SOLID. ZT_HAVOK_SOLID_AS_CLIP=1 enables the experimental clip remap on
					// both paths at once -- they have to agree or a brush model and the world
					// shell around it filter differently.
					// ZT_HAVOK_SOLID_CONTENTS overrides the mask on both paths too, for the same
					// reason: they have to agree.
					auto contents = model.contents;
					const auto* solid_as_clip = std::getenv("ZT_HAVOK_SOLID_AS_CLIP");
					// Keep the entity shape's contents aligned with the world mesh.  A bare
					// CONTENTS_SOLID is normal in stock palettes, so the experimental clip
					// substitution is opt-in only.
					if ((contents & 0x1) && solid_as_clip && solid_as_clip[0] == '1')
					{
						auto mask = 0x00031640u;
						const auto* env = std::getenv("ZT_HAVOK_SOLID_CONTENTS");
						if (env && env[0])
						{
							char* end = nullptr;
							const auto value = std::strtoul(env, &end, 0);
							if (end != env && value)
							{
								mask = static_cast<unsigned int>(value);
							}
						}
						contents = (contents & ~0x1) | static_cast<int>(mask);
					}
					shape.contents = contents;
					shape.entity_contents = static_cast<unsigned int>(contents);
					// These hulls are brushes, so they carry the brush basis, and the material
					// and surface flags come from the ClipMaterial their planes carry -- the
					// same source the world mesh uses. Leaving those at a constant made every
					// brush model untyped concrete and dropped its LADDER / SLICK /
					// NOPENETRATE / STAIRS / MANTLEON bits; stock ents blobs carry the full
					// spread (mp_afghan 12 distinct CRCs over 136 tags, flags set on 135).
					shape.material_crc = model.material_crc;
					shape.user_data = ENTS_BRUSH_BASIS | model.surface_flags;
					shape.name = va("%s:brushmodel %u", asset->name, model.index);

					for (const auto& hull : model.hulls)
					{
						ZoneTool::IW7::havok::builder::polytope convex{};
						convex.verts = hull.verts;
						for (const auto& face : hull.faces)
						{
							ZoneTool::IW7::havok::builder::polytope_face out{};
							std::memcpy(out.plane, face.plane, sizeof(float[4]));
							out.indices = face.indices;
							convex.faces.emplace_back(std::move(out));
						}
						shape.convexes.emplace_back(std::move(convex));
					}

					if (model.index < cmodel_shape_index.size())
					{
						cmodel_shape_index[model.index] =
							static_cast<unsigned short>(ents.shapes.size());
					}
					ents.shapes.emplace_back(std::move(shape));
				}

				// Triggers, appended after the brush models so the slot indices carry on.
				std::vector<unsigned short> trigger_shape_index(asset->trigger.count, 0xFFFF);
				if (trigger_shapes_enabled())
				{
					for (unsigned int t = 0; t < asset->trigger.count; t++)
					{
						auto hulls = collision::extract_trigger_hulls(asset->trigger, t);
						if (hulls.empty())
						{
							continue;
						}

						if (dump_obj)
						{
							ents_obj.emplace_back(collision::hull_group{
								va("trigger_%u", t), hulls});
						}

						ZoneTool::IW7::havok::builder::ents_shape shape{};
						shape.contents = asset->trigger.models[t].contents;
						// A trigger is the one case stock does use the wildcard for: all 78
						// trigger shapes in mp_dome_dusk and 63 of breakneck's 106 carry it.
						shape.entity_contents = ENTS_TRIGGER_CONTENTS;
						// A trigger is a volume, not a surface: nothing walks on it or shoots
						// it, so it keeps the default material and carries no surface flags.
						// Only the brush basis, as its hulls are brush-derived.
						shape.user_data = ENTS_BRUSH_BASIS;
						shape.name = va("%s:trigger %u", asset->name, t);

						for (const auto& hull : hulls)
						{
							ZoneTool::IW7::havok::builder::polytope convex{};
							convex.verts = hull.verts;
							for (const auto& face : hull.faces)
							{
								ZoneTool::IW7::havok::builder::polytope_face out{};
								std::memcpy(out.plane, face.plane, sizeof(float[4]));
								out.indices = face.indices;
								convex.faces.emplace_back(std::move(out));
							}
							shape.convexes.emplace_back(std::move(convex));
						}

						trigger_shape_index[t] = static_cast<unsigned short>(ents.shapes.size());
						ents.shapes.emplace_back(std::move(shape));
					}
				}

				if (dump_obj && !ents_obj.empty())
				{
					collision::write_hulls_obj(
						collision::obj_dump_path(asset->name, ".ents.obj"), ents_obj,
						ents.scale);
				}

				ZoneTool::IW7::havok::builder::ents_tag_merge tag_merge{};
				const auto blob = ents_shapes_enabled()
					? ZoneTool::IW7::havok::builder::build_ents_shape_list(ents, &tag_merge)
					: std::vector<std::uint8_t>{};

				if (!blob.empty())
				{
					// The one number that matters is `prefix`: it has to equal the world
					// table's size, or the world mesh is decoding against the wrong records.
					ZONETOOL_INFO("mapents: havok tag table -- world %zu entries, ents %zu "
						"(%zu reused from the world table, %zu appended)",
						tag_merge.prefix, tag_merge.total, tag_merge.reused,
						tag_merge.appended);
				}

				if (dump_obj)
				{
					collision::write_blob(
						collision::obj_dump_path(asset->name, ".ents.hkx"),
						blob.data(), blob.size());
				}

				if (!ents_shapes_enabled())
				{
					ZONETOOL_WARNING("mapents: ZT_HAVOK_ENTS_SHAPES=0 -- emitting a null havok "
						"ents shape list; brush model entities will have no physics body");
				}

				if (!blob.empty())
				{
					auto* memory = allocator.allocate<char>(blob.size());
					std::memcpy(memory, blob.data(), blob.size());

					new_asset->havokEntsShapeData = memory;
					new_asset->havokEntsShapeDataSize = static_cast<unsigned int>(blob.size());
				}
				else
				{
					// Without the list the indices below would point into nothing, and
					// CM_ContentsOfBrushModel would read through a null pointer.
					ZONETOOL_WARNING("mapents: no havok ents shape list generated for \"%s\" -- "
						"brush model entities may fault on link", asset->name);
					std::fill(cmodel_shape_index.begin(), cmodel_shape_index.end(),
						static_cast<unsigned short>(0xFFFF));
					std::fill(trigger_shape_index.begin(), trigger_shape_index.end(),
						static_cast<unsigned short>(0xFFFF));
				}

				// Point the trigger models at their shapes. Both fields are needed or the
				// runtime builds no body -- see docs/iw7-ents-shapes.md section 3.
				auto shaped_triggers = 0;
				IW7::PhysicsAsset* trigger_physics = nullptr;
				for (unsigned int t = 0; t < new_asset->trigger.count; t++)
				{
					if (t >= trigger_shape_index.size() || trigger_shape_index[t] == 0xFFFF)
					{
						continue;
					}

					if (!trigger_physics)
					{
						trigger_physics = allocator.allocate<IW7::PhysicsAsset>();
						trigger_physics->name = TRIGGER_PHYSICS_ASSET;

						ZoneTool::IW7::havok::builder::physics_asset_input physics{};
						physics.body_name = "triggermodeldummy";

						const auto physics_blob =
							ZoneTool::IW7::havok::builder::build_physics_asset(physics);
						if (!physics_blob.empty())
						{
							auto* memory = allocator.allocate<char>(physics_blob.size());
							std::memcpy(memory, physics_blob.data(), physics_blob.size());
							trigger_physics->havokData = memory;
							trigger_physics->havokDataSize =
								static_cast<unsigned int>(physics_blob.size());
						}

						trigger_physics->numRigidBodies = 1;
						trigger_physics->numSFXEventAssets = 1;
						trigger_physics->sfxEventAssets =
							allocator.allocate<IW7::PhysicsSFXEventAsset PTR64>(1);
						trigger_physics->numVFXEventAssets = 1;
						trigger_physics->vfxEventAssets =
							allocator.allocate<IW7::PhysicsVFXEventAsset PTR64>(1);
					}

					new_asset->trigger.models[t].physicsShapeOverrideIdx = trigger_shape_index[t];
					new_asset->trigger.models[t].physicsAsset = trigger_physics;
					shaped_triggers++;
				}

				if (shaped_triggers)
				{
					ZONETOOL_INFO("mapents: %d triggers given havok shapes "
						"(ZT_HAVOK_TRIGGER_SHAPES), backed by \"%s\"", shaped_triggers,
						TRIGGER_PHYSICS_ASSET);
				}
			}

			IW7::ClipInfo* info = allocator.allocate<IW7::ClipInfo>();
			info->planeCount = clipmap->info.planeCount;
			info->planes = allocator.allocate<IW7::cplane_s>(info->planeCount);
			for (auto i = 0; i < info->planeCount; i++)
			{
				memcpy(&info->planes[i], &clipmap->info.planes[i], sizeof(cplane_s));
			}

			// A brush model only gets a Havok body if BOTH physicsAsset and
			// physicsShapeOverrideIdx are set -- sub_140146DA0 reads the override only when
			// the asset is non-null, and skips body creation entirely when it is null. Stock
			// maps point this at a dummy PhysicsAsset named in the entity string (key 51961);
			// IW5 entities have no such key, so name the same default IW7 uses and let the
			// zone supply it.
			IW7::PhysicsAsset* brushmodel_physics = nullptr;
			{
				auto shaped = 0;
				for (const auto index : cmodel_shape_index)
				{
					shaped += index != 0xFFFF ? 1 : 0;
				}

				if (shaped > 0)
				{
					brushmodel_physics = allocator.allocate<IW7::PhysicsAsset>();
					brushmodel_physics->name = BRUSHMODEL_PHYSICS_ASSET;

					// The asset is generated rather than copied: build_physics_asset
					// reproduces the shipped dummy byte for byte.
					ZoneTool::IW7::havok::builder::physics_asset_input physics{};
					physics.body_name = "scriptbrushmodeldummy";

					const auto blob = ZoneTool::IW7::havok::builder::build_physics_asset(physics);
					if (!blob.empty())
					{
						auto* memory = allocator.allocate<char>(blob.size());
						std::memcpy(memory, blob.data(), blob.size());
						brushmodel_physics->havokData = memory;
						brushmodel_physics->havokDataSize =
							static_cast<unsigned int>(blob.size());
					}

					// One static body, no constraints, and one empty SFX/VFX event slot each
					// -- matching the shipped dummies.
					brushmodel_physics->numRigidBodies = 1;
					brushmodel_physics->numConstraints = 0;
					brushmodel_physics->numSFXEventAssets = 1;
					brushmodel_physics->sfxEventAssets =
						allocator.allocate<IW7::PhysicsSFXEventAsset PTR64>(1);
					brushmodel_physics->numVFXEventAssets = 1;
					brushmodel_physics->vfxEventAssets =
						allocator.allocate<IW7::PhysicsVFXEventAsset PTR64>(1);

					ZONETOOL_INFO("mapents: %d brush models given havok shapes, backed by a "
						"generated \"%s\" (%zu byte blob)", shaped, BRUSHMODEL_PHYSICS_ASSET,
						blob.size());
				}
			}

			new_asset->numSubModels = clipmap->numSubModels;
			new_asset->cmodels = allocator.allocate<IW7::cmodel_t>(clipmap->numSubModels);
			for (unsigned int i = 0; i < clipmap->numSubModels; i++)
			{
				memcpy(&new_asset->cmodels[i].bounds, &clipmap->cmodels[i].bounds, sizeof(Bounds));
				new_asset->cmodels[i].radius = clipmap->cmodels[i].radius;
				new_asset->cmodels[i].info = info;

				// cmodels[0] is the world submodel and never gets a body: stock leaves its
				// physicsAsset null, which makes its shape index a don't-care.
				const auto shape_index = i == 0 ? 0xFFFF : cmodel_shape_index[i];
				new_asset->cmodels[i].physicsShapeOverrideIdx = shape_index;
				new_asset->cmodels[i].physicsAsset =
					shape_index != 0xFFFF ? brushmodel_physics : nullptr;

				new_asset->cmodels[i].navObstacleIdx = i == 0 ? 0 : 0xFFFF;
				//new_asset->cmodels[i].edgeFirstIndex = 0;
			}

			new_asset->dynEntCount[0] = clipmap->dynEntCount[0];
			new_asset->dynEntCount[1] = clipmap->dynEntCount[1];

			unsigned short reserved_dynents = 64;
			new_asset->dynEntCount[0] += reserved_dynents;

			unsigned short total_dynents = new_asset->dynEntCount[0] + new_asset->dynEntCount[1];
			new_asset->dynEntCountTotal = total_dynents;

			new_asset->dynEntDefList[0] = allocator.allocate<IW7::DynEntityDef>(new_asset->dynEntCount[0]);
			new_asset->dynEntPoseList[0][0] = allocator.allocate<IW7::DynEntityPose>(new_asset->dynEntCount[0]);
			new_asset->dynEntPoseList[1][0] = allocator.allocate<IW7::DynEntityPose>(new_asset->dynEntCount[0]);
			new_asset->dynEntClientList[0][0] = allocator.allocate<IW7::DynEntityClient>(new_asset->dynEntCount[0]);
			new_asset->dynEntClientList[1][0] = allocator.allocate<IW7::DynEntityClient>(new_asset->dynEntCount[0]);

			new_asset->dynEntDefList[1] = allocator.allocate<IW7::DynEntityDef>(new_asset->dynEntCount[1]);
			new_asset->dynEntPoseList[0][1] = allocator.allocate<IW7::DynEntityPose>(new_asset->dynEntCount[1]);
			new_asset->dynEntPoseList[1][1] = allocator.allocate<IW7::DynEntityPose>(new_asset->dynEntCount[1]);
			new_asset->dynEntClientList[0][1] = allocator.allocate<IW7::DynEntityClient>(new_asset->dynEntCount[1]);
			new_asset->dynEntClientList[1][1] = allocator.allocate<IW7::DynEntityClient>(new_asset->dynEntCount[1]);

			new_asset->dynEntGlobalIdList[0] = allocator.allocate<IW7::DynEntityGlobalId>(new_asset->dynEntCountTotal);
			new_asset->dynEntGlobalIdList[1] = allocator.allocate<IW7::DynEntityGlobalId>(new_asset->dynEntCountTotal);

			struct generated_scriptable
			{
				IW7::ScriptableDef* def;
				GfxPlacement pose;
			};
			std::vector<generated_scriptable> scriptable_defs;
			std::array<int, IW7::DYNENT_TYPE_COUNT> dynent_type_count{};

			const auto copy_dynents = [&](const auto index)
			{
				for (auto i = 0; i < new_asset->dynEntCount[index] - (index == 0 ? reserved_dynents : 0); i++)
				{
					{
						auto* new_dynent_def = &new_asset->dynEntDefList[index][i];
						auto* dynent_def = &clipmap->dynEntDefList[index][i];

						const auto convert_type = [](DynEntityType type) -> IW7::DynEntityType
						{
							switch (type)
							{
							case DYNENT_TYPE_INVALID:
								return IW7::DYNENT_TYPE_INVALID;
								break;
							case DYNENT_TYPE_CLUTTER:
								return IW7::DYNENT_TYPE_CLUTTER;
								break;
							case DYNENT_TYPE_DESTRUCT:
								return IW7::DYNENT_TYPE_CLUTTER;
								break;
							case DYNENT_TYPE_HINGE:
								return IW7::DYNENT_TYPE_HINGE;
								break;
							}
							return IW7::DYNENT_TYPE_INVALID;
						};

						// scriptable dynent does not want to spawn in???
						if (dynent_def->type == DYNENT_TYPE_DESTRUCT)
						{
							ZONETOOL_INFO("converting dynent destruct into scriptable");
							auto* scriptable_def = generate_scriptable_def_from_dynent(dynent_def, allocator);
							scriptable_defs.push_back({ scriptable_def, dynent_def->pose });

							new_dynent_def->instanceIndex = static_cast<unsigned int>(500 + scriptable_defs.size() - 1);
							new_dynent_def->type = IW7::DYNENT_TYPE_HINGE;
							// The type-3 dynent is the bridge between the map and the
							// scriptable instance.  It still needs its base model: the
							// client dynent registration seeds activeModel from
							// DynEntityDef::baseModel before the scriptable state events
							// are evaluated.  Leaving this null makes the association
							// exist, but leaves the prop invisible because the initial
							// client registration has no model to activate.
							//new_dynent_def->baseModel =
							//	reinterpret_cast<IW7::XModel*>(dynent_def->xModel);
							//new_dynent_def->baseModel =
							//	reinterpret_cast<IW7::XModel*>(dynent_def->xModel);
							// Stock type-3 scriptable associations retain the source dynent
							// pose.  The scriptable instance also stores this placement, but
							// the dynent association uses its own pose during registration.
							// Leaving it zeroed associates the scriptable at the origin.
							memcpy(&new_dynent_def->pose, &dynent_def->pose, sizeof(GfxPlacement));
							new_dynent_def->linkTo = nullptr;
							// Type 3 is IW7's map-scriptable association.  Stock entries //retain the
							// source dynent contents in this 16-bit field; leaving it zero //changes
							// the association's physics/contents classification.
							// Every stock type-3 association has a nonzero class here; 1 is //the
							// ordinary static scriptable association used by the majority of //them.
							new_dynent_def->unk5 = 1;
							new_dynent_def->unk6 = static_cast<short>(dynent_def->contents);
							// DynEntCl_InitEntities tests this flag before calling
							// DynEntCL_AddEntity.  Without it the type-3 association is
							// left inactive and the scriptable can never spawn.
							new_dynent_def->spawnEnabled = true;
						}
						else
						{
							new_dynent_def->type = convert_type(dynent_def->type);
							if (new_dynent_def->type == IW7::DYNENT_TYPE_CLUTTER)
							{
								memcpy(&new_dynent_def->initialPose, &dynent_def->pose, sizeof(GfxPlacement));
							}
							memcpy(&new_dynent_def->pose, &dynent_def->pose, sizeof(GfxPlacement));
							new_dynent_def->baseModel = reinterpret_cast<IW7::XModel*>(dynent_def->xModel);
							new_dynent_def->brushModel = dynent_def->brushModel;
							new_dynent_def->linkTo = nullptr;
							new_dynent_def->instanceIndex = 0;
							new_dynent_def->spawnEnabled = true;
						}

						dynent_type_count[new_dynent_def->type]++;
					}

					{
						auto* dynent_pose_model =
							&new_asset->dynEntPoseList[IW7::DynEntityBasis::DYNENT_BASIS_MODEL][index][i];
						auto* dynent_pose_brush =
							&new_asset->dynEntPoseList[IW7::DynEntityBasis::DYNENT_BASIS_BRUSH][index][i];
						auto* dynent_pose = &clipmap->dynEntPoseList[index][i];

						// model
						memcpy(&dynent_pose_model->pose, &dynent_pose->pose, sizeof(IW7::GfxPlacement));
						dynent_pose_model->numPoses = 1;
						dynent_pose_model->poses = allocator.allocate<IW7::GfxPlacement>(1);
						memcpy(&dynent_pose_model->poses[0], &dynent_pose_model->pose, sizeof(IW7::GfxPlacement));
						dynent_pose_model->radius = dynent_pose->radius;
						dynent_pose_model->detailBodyToBoneMap = allocator.allocate<char>(dynent_pose_model->numPoses);

						// brush
						memcpy(&dynent_pose_brush->pose, &dynent_pose->pose, sizeof(IW7::GfxPlacement));
						dynent_pose_brush->numPoses = 1;
						dynent_pose_brush->poses = allocator.allocate<IW7::GfxPlacement>(1);
						memcpy(&dynent_pose_brush->poses[0], &dynent_pose_brush->pose, sizeof(IW7::GfxPlacement));
						dynent_pose_brush->radius = dynent_pose->radius;
						dynent_pose_brush->detailBodyToBoneMap = nullptr;
					}
				}
			};
			copy_dynents(0);
			copy_dynents(1);

			for (auto i = 0; i < reserved_dynents; i++)
			{
				auto base_index = new_asset->dynEntCount[0] - reserved_dynents;
				auto* dyn = &new_asset->dynEntDefList[0][i + base_index];
				dyn->type = IW7::DYNENT_TYPE_SCRIPTABLEINST;
				dyn->instanceIndex = static_cast<unsigned int>(500 + scriptable_defs.size());
				dyn->unk4 = i; // reserved index
				dyn->spawnActive = true;
				dyn->unk5 = 4;
				dyn->unk6 = 0x666;
				dyn->spawnEnabled = true;
			}

			for (auto i = 0; i < new_asset->dynEntCountTotal; i++)
			{
				new_asset->dynEntGlobalIdList[0][i].basis = 0;
				new_asset->dynEntGlobalIdList[0][i].id = i;

				new_asset->dynEntGlobalIdList[1][i].basis = 1;
				new_asset->dynEntGlobalIdList[1][i].id = i;
			}

			std::fill_n(&new_asset->dynEntPhysicsSetupHead[0][0], 4,
				static_cast<unsigned short>(0xFFFF));
			std::fill_n(&new_asset->dynEntPhysicsSetupTail[0][0], 4,
				static_cast<unsigned short>(0xFFFF));

			// IW5 has no transient dynent-zone metadata.  Converted dynents are all
			// base-world entities (isTransient == false), so do not fabricate a
			// transient group containing them.  The transient loader walks this list
			// and a synthetic base group causes it to repeatedly process the same
			// physics/scriptable entities.  Stock base-world maps leave these fields
			// empty when there are no transient dynents.
			new_asset->dynEntTransientGroupCount = 0;
			new_asset->dynEntTransientGroups = nullptr;
			new_asset->dynEntTransientGroupRuntime[0] = nullptr;
			new_asset->dynEntTransientGroupRuntime[1] = nullptr;
			new_asset->dynEntTransientGroupState[0] = nullptr;
			new_asset->dynEntTransientGroupState[1] = nullptr;

			new_asset->unk3Count = 0;
			new_asset->unk3 = nullptr;

			new_asset->clientEntAnchorCount = 0;
			new_asset->clientEntAnchors = nullptr;

			new_asset->scriptableMapEnts.totalInstanceCount = static_cast<unsigned int>(500 + scriptable_defs.size());
			new_asset->scriptableMapEnts.runtimeInstanceCount = 500;
			new_asset->scriptableMapEnts.reservedInstanceCount = 500;

			new_asset->scriptableMapEnts.instances = allocator.allocate<IW7::ScriptableInstance>(new_asset->scriptableMapEnts.totalInstanceCount);
			std::memset(new_asset->scriptableMapEnts.instances, 0,
				sizeof(IW7::ScriptableInstance) *
				new_asset->scriptableMapEnts.totalInstanceCount);

			// IW7 does not rebuild these part-runtime pools until after the map has
			// been loaded.  They nevertheless must be present in the serialized
			// ScriptableMapEnts: the client initialization path passes the pool
			// count to Scriptable_GetPartRuntime and dereferences its result.  A
			// zero count/null pointer therefore crashes while loading the map
			// (sub_140BEC6A0 at 0x140BEC6C1).  Stock dumps contain zeroed stateId
			// entries, so allocate zero-initialized entries for the complete
			// instance range; the runtime builder can compact/rebuild them later.
			const auto part_runtime_capacity = new_asset->scriptableMapEnts.totalInstanceCount;
			new_asset->scriptableMapEnts.runtimeData.partRuntimeCount =
				static_cast<int>(part_runtime_capacity);
			new_asset->scriptableMapEnts.runtimeData.partRuntime =
				allocator.allocate<IW7::ScriptablePartRuntime>(part_runtime_capacity);
			new_asset->scriptableMapEnts.runtimeData.partRuntimeLocalClientCount =
				static_cast<int>(part_runtime_capacity);
			new_asset->scriptableMapEnts.runtimeData.partRuntimeLocalClient[0] =
				allocator.allocate<IW7::ScriptablePartRuntime>(part_runtime_capacity);
			new_asset->scriptableMapEnts.runtimeData.partRuntimeLocalClient[1] =
				allocator.allocate<IW7::ScriptablePartRuntime>(part_runtime_capacity);
			std::memset(new_asset->scriptableMapEnts.runtimeData.partRuntime, 0,
				sizeof(IW7::ScriptablePartRuntime) * part_runtime_capacity);
			std::memset(new_asset->scriptableMapEnts.runtimeData.partRuntimeLocalClient[0], 0,
				sizeof(IW7::ScriptablePartRuntime) * part_runtime_capacity);
			std::memset(new_asset->scriptableMapEnts.runtimeData.partRuntimeLocalClient[1], 0,
				sizeof(IW7::ScriptablePartRuntime) * part_runtime_capacity);

			for (unsigned int i = 0; i < part_runtime_capacity; ++i)
			{
				new_asset->scriptableMapEnts.runtimeData.partRuntime[i].stateId = 0;
				new_asset->scriptableMapEnts.runtimeData.partRuntimeLocalClient[0][i].stateId = 0;
				new_asset->scriptableMapEnts.runtimeData.partRuntimeLocalClient[1][i].stateId = 0;
			}

			const auto write_scriptable_placement = [](IW7::ScriptableInstanceContext& context,
				const GfxPlacement& pose)
			{
				// engine convention ([pitch, yaw, roll] degrees, positive pitch down) - see Utils/Math.hpp
				context.origin[0] = pose.origin[0];
				context.origin[1] = pose.origin[1];
				context.origin[2] = pose.origin[2];
				math::UnitQuatToAngles(pose.quat, context.angles);

				std::memcpy(context.initialOrigin, pose.origin, sizeof(pose.origin));
				std::memcpy(context.initialAngles, context.angles, sizeof(context.angles));
			};

			for (unsigned int i = 0; i < scriptable_defs.size(); i++)
			{
				auto& instance = new_asset->scriptableMapEnts.instances[500 + i];
				instance.contextHeader.context.def = scriptable_defs[i].def;
				instance.contextHeaderLocalClient[0].context.def = scriptable_defs[i].def; // localclient 0
				instance.contextHeaderLocalClient[1].context.def = scriptable_defs[i].def; // localclient 1
				write_scriptable_placement(instance.contextHeader.context, scriptable_defs[i].pose);
				write_scriptable_placement(instance.contextHeaderLocalClient[0].context, scriptable_defs[i].pose);
				write_scriptable_placement(instance.contextHeaderLocalClient[1].context, scriptable_defs[i].pose);

				// These are serialized runtime event-stream buffers, not optional
				// pointers.  IW7 adds an 8-byte server header and a 16-byte
				// local-client header to the definition stream.  Thus the generated
				// four-byte stream becomes the stock 12/20-byte buffers, while a
				// definition with a larger stream scales automatically.
				const auto definition_event_stream_size =
					static_cast<unsigned int>(scriptable_defs[i].def->eventStreamSize);
				const auto server_event_stream_size = definition_event_stream_size + 8u;
				const auto local_client_event_stream_size = definition_event_stream_size + 16u;
				instance.contextHeader.context.eventStreamBufferSize = server_event_stream_size;
				instance.contextHeader.context.eventStreamBuffer =
					allocator.allocate<char>(server_event_stream_size);
				instance.contextHeaderLocalClient[0].context.eventStreamBufferSize =
					local_client_event_stream_size;
				instance.contextHeaderLocalClient[0].context.eventStreamBuffer =
					allocator.allocate<char>(local_client_event_stream_size);
				instance.contextHeaderLocalClient[1].context.eventStreamBufferSize =
					local_client_event_stream_size;
				instance.contextHeaderLocalClient[1].context.eventStreamBuffer =
					allocator.allocate<char>(local_client_event_stream_size);
				std::fill_n(instance.contextHeader.context.eventStreamBuffer,
					server_event_stream_size, static_cast<char>(0));
				std::fill_n(instance.contextHeaderLocalClient[0].context.eventStreamBuffer,
					local_client_event_stream_size, static_cast<char>(0));
				std::fill_n(instance.contextHeaderLocalClient[1].context.eventStreamBuffer,
					local_client_event_stream_size, static_cast<char>(0));

				// stock map does this
				std::fill_n(reinterpret_cast<int*>(instance.contextHeader.unk02), 2, -1);
				std::fill_n(reinterpret_cast<int*>(instance.contextHeaderLocalClient[0].unk02), 3, -1);
				std::fill_n(reinterpret_cast<int*>(instance.contextHeaderLocalClient[1].unk02), 3, -1);
			}

			// Reserved dynent pools are initialized by IW7 after the zone is loaded.
			new_asset->scriptableMapEnts.reservedDynents[0].numReservedDynents = reserved_dynents;
			new_asset->scriptableMapEnts.reservedDynents[0].reservedDynents =
				allocator.allocate<IW7::ScriptableReservedDynent>(new_asset->scriptableMapEnts.reservedDynents[0].numReservedDynents);

			new_asset->scriptableMapEnts.reservedDynents[1].numReservedDynents = reserved_dynents;
			new_asset->scriptableMapEnts.reservedDynents[1].reservedDynents =
				allocator.allocate<IW7::ScriptableReservedDynent>(new_asset->scriptableMapEnts.reservedDynents[1].numReservedDynents);

			new_asset->numMayhemScenes = 0;
			new_asset->mayhemScenes = nullptr;

			new_asset->spawners.spawnerCount = 0;
			new_asset->spawners.spawnerList = nullptr;

			new_asset->audioPASpeakerCount = 0;
			new_asset->audioPASpeakers = nullptr;

			return new_asset;
		}

		void GenerateIW7ClipInfo(IW7::ClipInfo* info, IW5::ClipInfo* dinfo, allocator& mem)
		{
			if (!dinfo)
			{
				info = nullptr;
				return;
			}

			info->planeCount = dinfo->planeCount;
			info->planes = reinterpret_cast<IW7::cplane_s*>(dinfo->planes);
		}

		IW7::clipMap_t* GenerateIW7ClipMap(clipMap_t* asset, allocator& allocator)
		{
			// allocate IW7 clipMap_t structure
			const auto IW7_asset = allocator.allocate<IW7::clipMap_t>();

			IW7_asset->name = asset->name;
			IW7_asset->isInUse = asset->isInUse;
			GenerateIW7ClipInfo(&IW7_asset->info, &asset->info, allocator);
			IW7_asset->pInfo = &IW7_asset->info;

			IW7_asset->numStaticModels = asset->numStaticModels;
			IW7_asset->staticModelList = allocator.allocate<IW7::cStaticModel_s>(IW7_asset->numStaticModels);
			for (unsigned int i = 0; i < IW7_asset->numStaticModels; i++)
			{
				IW7_asset->staticModelList[i].xmodel = reinterpret_cast<IW7::XModel*>(asset->staticModelList[i].xmodel);
				std::memcpy(&IW7_asset->staticModelList[i].origin, &asset->staticModelList[i].origin, sizeof(float[3]));
				std::memcpy(&IW7_asset->staticModelList[i].invScaledAxis, &asset->staticModelList[i].invScaledAxis, 
					sizeof(float[3][3]));
				IW7_asset->staticModelList[i].unk1 = false;
				IW7_asset->staticModelList[i].unk2 = false;
				IW7_asset->staticModelList[i].hasTransientModel = false;
				IW7_asset->staticModelList[i].hasTransientPhysicsAsset = false;
			}
			
			IW7_asset->staticModelCollisionModelList.numModels = asset->numStaticModels;
			IW7_asset->staticModelCollisionModelList.staticModelIndex = allocator.allocate<int>(asset->numStaticModels);
			for (unsigned int i = 0; i < asset->numStaticModels; i++)
			{
				IW7_asset->staticModelCollisionModelList.staticModelIndex[i] = i;
			}
			IW7_asset->numStaticModelCollisionModelLists = 0;
			IW7_asset->staticModelCollisionModelLists = nullptr;

			// World collision. IW7 has no brush/BSP collision at all -- clipMap_t::info is
			// just planes -- so the entire collidable world has to be generated as a single
			// hknpCompressedMeshShape. See docs/iw7-havok-collision.md.
			//
			// This runs BEFORE the mapents are generated, which is a hard ordering and not a
			// preference: the ents shape list has to be built on top of this blob's tag
			// table, because IW7 decodes every shape in the map -- world mesh included --
			// against the single table registered from the main shape list.
			IW7_asset->havokWorldShapeDataSize = 0;
			IW7_asset->havokWorldShapeData = nullptr;
			std::vector<ZoneTool::IW7::havok::builder::shape_tag> world_shape_tags;
			{
				const auto world = collision::extract_world(asset);
				const auto& triangles = world.triangles;
				if (!triangles.empty() || !world.convexes.empty())
				{
					ZoneTool::IW7::havok::builder::mesh_input input{};
					// Brushes arrive as convexes (ZT_HAVOK_BRUSH_CONVEX, default on) and become
					// convex custom primitives; the builder counts what it emits for the shape
					// list's convexCounts itself.
					input.convexes.reserve(world.convexes.size());
					for (const auto& cvx : world.convexes)
					{
						ZoneTool::IW7::havok::builder::convex out{};
						out.verts = cvx.verts;
						out.surface_tag = cvx.surface_tag;
						out.contents = cvx.contents;
						out.material_crc = cvx.material_crc;
						out.user_data = cvx.user_data;
						input.convexes.emplace_back(std::move(out));
					}

					// Triangles only: a convex custom primitive is a point set with no winding.
					const auto* flip_winding = std::getenv("ZT_HAVOK_FLIP_WINDING");
					const auto reverse_winding = flip_winding && flip_winding[0] == '1';
					if (reverse_winding)
					{
						ZONETOOL_WARNING("clipmap: reversing world collision winding "
							"(ZT_HAVOK_FLIP_WINDING diagnostic, triangles only)");
					}
					input.triangles.reserve(triangles.size());
					for (const auto& tri : triangles)
					{
						ZoneTool::IW7::havok::builder::triangle out{};
						std::memcpy(out.verts, tri.verts, sizeof(out.verts));
						out.surface_tag = tri.surface_tag;
						out.contents = tri.contents;
						out.material_crc = tri.material_crc;
						out.user_data = tri.user_data;
						out.is_quad = tri.is_quad;
						std::memcpy(out.vert3, tri.vert3, sizeof(out.vert3));
						if (reverse_winding)
						{
							if (out.is_quad)
							{
								// Preserve the quad's ring while reversing its normal:
								// (v0,v1,v2,v3) becomes (v0,v3,v2,v1).
								std::swap(out.verts[1], out.vert3);
							}
							else
							{
								std::swap(out.verts[1], out.verts[2]);
							}
						}
						input.triangles.emplace_back(out);
					}

					const auto blob = ZoneTool::IW7::havok::builder::build_world_shape(
						input, &world_shape_tags);

					if (collision::obj_dump_enabled())
					{
						collision::write_blob(
							collision::obj_dump_path(asset->name, ".world.hkx"),
							blob.data(), blob.size());
					}

					if (!blob.empty())
					{
						auto* memory = allocator.allocate<char>(blob.size());
						std::memcpy(memory, blob.data(), blob.size());

						IW7_asset->havokWorldShapeData = memory;
						IW7_asset->havokWorldShapeDataSize = static_cast<unsigned int>(blob.size());
					}
					else
					{
						// build_world_shape logs why. A null blob means no world collision,
						// which is survivable for loading but the map will have nothing to
						// stand on.
						ZONETOOL_WARNING("clipmap: no havok world shape generated for \"%s\"",
							asset->name);
					}
				}
			}

			// The mapents carry every trigger volume in the map, so this is not just a name
			// reference -- it is where the converted triggers live. The clipmap is dumped
			// with only the name (IClipMap::dump calls dump_asset on it), but the object has
			// to be real so the dumper can write it out as the MapEnts asset it points at.
			IW7_asset->mapEnts = generate_mapents(asset, allocator, world_shape_tags);

			IW7_asset->stageCount = asset->stageCount;
			IW7_asset->stages = allocator.allocate<IW7::Stage>(IW7_asset->stageCount);
			for (unsigned int i = 0; i < IW7_asset->stageCount; i++)
			{
				IW7_asset->stages[i].name = asset->stages[i].name;
				memcpy(&IW7_asset->stages[i].origin, &asset->stages[i].origin, sizeof(float[3]));
				IW7_asset->stages[i].triggerIndex = asset->stages[i].triggerIndex;
				IW7_asset->stages[i].sunPrimaryLightIndex = asset->stages[i].sunPrimaryLightIndex;
				IW7_asset->stages[i].entityUID = 0x3A83126F;
			}

			// stageTrigger is a second trigger set, indexed by Stage::triggerIndex instead of
			// by an entity, and in every stock IW7 map it holds exactly the same models, hulls
			// and slabs as MapEnts::trigger. It stays empty here because IClipMap::dump does
			// not write these three arrays to the .colmap stream -- populating them would put
			// non-zero counts in the struct with no array data behind them and desync the
			// linker's parse. Carrying them across needs a dumper change on both sides.
			IW7_asset->stageTrigger.count = 0;
			IW7_asset->stageTrigger.models = nullptr;
			IW7_asset->stageTrigger.hullCount = 0;
			IW7_asset->stageTrigger.hulls = nullptr;
			IW7_asset->stageTrigger.slabCount = 0;
			IW7_asset->stageTrigger.slabs = nullptr;
			IW7_asset->stageTrigger.windingCount = 0;
			IW7_asset->stageTrigger.windings = nullptr;
			IW7_asset->stageTrigger.windingPointCount = 0;
			IW7_asset->stageTrigger.windingPoints = nullptr;

			// With stageTrigger empty the stage list has nothing to index. CM_GetStageFromPoint
			// walks stages[1..stageCount) and feeds each stage's triggerIndex straight into the
			// hull/slab test with no bounds check, but bails out entirely at stageCount <= 1 --
			// so that is the safe ceiling until stage triggers actually survive the dump.
			if (IW7_asset->stageCount > 1)
			{
				ZONETOOL_WARNING("clipmap: %u stages but no stage triggers survive the dump -- "
					"clamping stageCount to 1", IW7_asset->stageCount);
				IW7_asset->stageCount = 1;
			}

			// this should be fine.
			IW7_asset->broadphaseMin[0] = -131072.f;
			IW7_asset->broadphaseMin[1] = -131072.f;
			IW7_asset->broadphaseMin[2] = -131072.f;
			IW7_asset->broadphaseMax[0] = 131072.f;
			IW7_asset->broadphaseMax[1] = 131072.f;
			IW7_asset->broadphaseMax[2] = 131072.f;
			
			IW7_asset->physicsCapacities; // these might get set during runtime

			IW7_asset->numCollisionHeatmapEntries = 0;
			IW7_asset->collisionHeatmap = nullptr; // todo...

			IW7_asset->topDownMapData = nullptr; // todo...

			IW7_asset->checksum = asset->checksum;

			return IW7_asset;
		}

		IW7::clipMap_t* convert(clipMap_t* asset, allocator& allocator)
		{
			return GenerateIW7ClipMap(asset, allocator);
		}
	}
}
