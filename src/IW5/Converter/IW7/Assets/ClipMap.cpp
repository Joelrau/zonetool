#include "stdafx.hpp"
#include "../Include.hpp"

#include "ClipMap.hpp"
#include "ClipMapCollision.hpp"

#include "Common/havok_builder.hpp"

#include <algorithm>
#include <cstdlib>
#include <string>

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
		}

		IW7::MapEnts* generate_mapents(clipMap_t* clipmap, allocator& allocator)
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

			if (ZoneTool::currentlinkermode == ZoneTool::linker_mode::iw5)
			{
				const auto str = ::mapents::converter::iw5::convert_mapents_ids(
					std::string{ asset->entityString, static_cast<size_t>(asset->numEntityChars) });
				new_asset->entityString = const_cast<char*>(allocator.duplicate_string(str));
				new_asset->numEntityChars = static_cast<int>(str.size());
			}
			else
			{
				new_asset->entityString = asset->entityString;
				new_asset->numEntityChars = asset->numEntityChars;
			}

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

				const auto blob = ents_shapes_enabled()
					? ZoneTool::IW7::havok::builder::build_ents_shape_list(ents)
					: std::vector<std::uint8_t>{};

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

			for (auto i = 0; i < reserved_dynents; i++)
			{
				auto* dyn = &new_asset->dynEntDefList[0][i];
				dyn->type = IW7::DYNENT_TYPE_SCRIPTABLEINST;
				dyn->scriptableMapIndex = 500;
				dyn->unk2 = true;
			}

			const auto copy_dynents = [&](const auto index)
			{
				for (auto i = reserved_dynents; i < new_asset->dynEntCount[index]; i++)
				{
					const auto idx = i - reserved_dynents;

					{
						auto* new_dynent_def = &new_asset->dynEntDefList[index][i];
						auto* dynent_def = &clipmap->dynEntDefList[index][idx];

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
								return IW7::DYNENT_TYPE_INVALID;
								break;
							case DYNENT_TYPE_HINGE:
								return IW7::DYNENT_TYPE_HINGE;
								break;
							}
							return IW7::DYNENT_TYPE_INVALID;
						};

						new_dynent_def->type = convert_type(dynent_def->type);
						memcpy(&new_dynent_def->pose, &dynent_def->pose, sizeof(GfxPlacement));
						new_dynent_def->baseModel = reinterpret_cast<IW7::XModel*>(dynent_def->xModel);
						new_dynent_def->brushModel = dynent_def->brushModel;
						new_dynent_def->linkTo = nullptr;
						new_dynent_def->scriptableMapIndex = 0;
						new_dynent_def->unk2 = true;
					}

					{
						auto* dynent_pose_model =
							&new_asset->dynEntPoseList[IW7::DynEntityBasis::DYNENT_BASIS_MODEL][index][i];
						auto* dynent_pose_brush =
							&new_asset->dynEntPoseList[IW7::DynEntityBasis::DYNENT_BASIS_BRUSH][index][i];
						auto* dynent_pose = &clipmap->dynEntPoseList[index][idx];

						// model
						memcpy(&dynent_pose_model->pose, &dynent_pose->pose, sizeof(IW7::GfxPlacement));
						dynent_pose_model->numPoses = 1;
						dynent_pose_model->poses = allocator.allocate<IW7::GfxPlacement>(1);
						memcpy(&dynent_pose_model->poses[0], &dynent_pose_model->pose, sizeof(IW7::GfxPlacement));
						dynent_pose_model->radius = dynent_pose->radius;

						// brush
						memcpy(&dynent_pose_brush->pose, &dynent_pose->pose, sizeof(IW7::GfxPlacement));
						dynent_pose_brush->numPoses = 1;
						dynent_pose_brush->poses = allocator.allocate<IW7::GfxPlacement>(1);
						memcpy(&dynent_pose_brush->poses[0], &dynent_pose_brush->pose, sizeof(IW7::GfxPlacement));
						dynent_pose_brush->radius = dynent_pose->radius;
					}
				}
			};
			copy_dynents(0);
			copy_dynents(1);

			for (auto i = 0; i < new_asset->dynEntCountTotal; i++)
			{
				new_asset->dynEntGlobalIdList[0][i].basis = 0;
				new_asset->dynEntGlobalIdList[0][i].id = i;

				new_asset->dynEntGlobalIdList[1][i].basis = 0;
				new_asset->dynEntGlobalIdList[1][i].id = i;
			}

			for (auto i = 0; i < 8; i++)
			{
				new_asset->unkIndexes[i] = -1;
			}

			new_asset->unk2Count = 0;
			new_asset->unk2 = nullptr;
			new_asset->unk2_1[0] = nullptr;
			new_asset->unk2_1[1] = nullptr;
			new_asset->unk2_2[0] = nullptr;
			new_asset->unk2_2[1] = nullptr;
			new_asset->unk3Count = 0;
			new_asset->unk3 = nullptr;

			new_asset->clientEntAnchorCount = 0;
			new_asset->clientEntAnchors = nullptr;

			new_asset->scriptableMapEnts.totalInstanceCount = 500;
			new_asset->scriptableMapEnts.runtimeInstanceCount = 500;
			new_asset->scriptableMapEnts.reservedInstanceCount = 500;

			new_asset->scriptableMapEnts.instances = allocator.allocate<IW7::ScriptableInstance>(new_asset->scriptableMapEnts.totalInstanceCount);

			// these are runtime data
			new_asset->scriptableMapEnts.reservedDynents[0].numReservedDynents = reserved_dynents;
			new_asset->scriptableMapEnts.reservedDynents[0].reservedDynents =
				allocator.allocate<IW7::ScriptableReservedDynent>(new_asset->scriptableMapEnts.reservedDynents[0].numReservedDynents);

			// these are runtime data
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

			// The mapents carry every trigger volume in the map, so this is not just a name
			// reference -- it is where the converted triggers live. The clipmap is dumped
			// with only the name (IClipMap::dump calls dump_asset on it), but the object has
			// to be real so the dumper can write it out as the MapEnts asset it points at.
			IW7_asset->mapEnts = generate_mapents(asset, allocator);

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

			// World collision. IW7 has no brush/BSP collision at all -- clipMap_t::info is
			// just planes -- so the entire collidable world has to be generated as a single
			// hknpCompressedMeshShape. See docs/iw7-havok-collision.md.
			IW7_asset->havokWorldShapeDataSize = 0;
			IW7_asset->havokWorldShapeData = nullptr;
			{
				const auto triangles = collision::extract(asset);
				if (!triangles.empty())
				{
					ZoneTool::IW7::havok::builder::mesh_input input{};
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
						input.triangles.emplace_back(out);
					}

					const auto blob = ZoneTool::IW7::havok::builder::build_world_shape(input);

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
