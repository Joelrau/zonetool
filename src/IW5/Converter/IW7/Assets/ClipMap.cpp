#include "stdafx.hpp"
#include "../Include.hpp"

#include "ClipMap.hpp"

namespace ZoneTool::IW5
{
	namespace IW7Converter
	{
		IW7::MapEnts* generate_mapents(clipMap_t* clipmap, allocator& allocator)
		{
			const auto* asset = clipmap->mapEnts;
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

			COPY_VALUE(trigger.count);
			new_asset->trigger.models = allocator.allocate<IW7::TriggerModel>(asset->trigger.count);
			for (unsigned int i = 0; i < asset->trigger.count; i++)
			{
				new_asset->trigger.models[i].contents = asset->trigger.models[i].contents;
				new_asset->trigger.models[i].hullCount = asset->trigger.models[i].hullCount;
				new_asset->trigger.models[i].firstHull = asset->trigger.models[i].firstHull;
				new_asset->trigger.models[i].windingCount = 0;
				new_asset->trigger.models[i].firstWinding = 0;
				new_asset->trigger.models[i].flags = 0;
				new_asset->trigger.models[i].physicsAsset = nullptr; // fixme
				new_asset->trigger.models[i].physicsShapeOverrideIdx = 0xFFFF;
			}

			COPY_VALUE(trigger.hullCount);
			REINTERPRET_CAST_SAFE(trigger.hulls);
			COPY_VALUE(trigger.slabCount);
			REINTERPRET_CAST_SAFE(trigger.slabs);

			new_asset->clientTrigger.trigger.windingCount = 0;
			new_asset->clientTrigger.trigger.windings = nullptr;
			new_asset->clientTrigger.trigger.windingPointCount = 0;
			new_asset->clientTrigger.trigger.windingPoints = nullptr;

			COPY_VALUE(clientTrigger.trigger.count);
			new_asset->clientTrigger.trigger.models = allocator.allocate<IW7::TriggerModel>(asset->clientTrigger.trigger.count);
			for (unsigned int i = 0; i < asset->clientTrigger.trigger.count; i++)
			{
				new_asset->clientTrigger.trigger.models[i].contents = asset->clientTrigger.trigger.models[i].contents;
				new_asset->clientTrigger.trigger.models[i].hullCount = asset->clientTrigger.trigger.models[i].hullCount;
				new_asset->clientTrigger.trigger.models[i].firstHull = asset->clientTrigger.trigger.models[i].firstHull;
				new_asset->clientTrigger.trigger.models[i].windingCount = 0;
				new_asset->clientTrigger.trigger.models[i].firstWinding = 0;
				new_asset->clientTrigger.trigger.models[i].flags = 0;
				new_asset->clientTrigger.trigger.models[i].physicsAsset = nullptr; // fixme
				new_asset->clientTrigger.trigger.models[i].physicsShapeOverrideIdx = 0xFFFF;
			}

			COPY_VALUE(clientTrigger.trigger.hullCount);
			REINTERPRET_CAST_SAFE(clientTrigger.trigger.hulls);
			COPY_VALUE(clientTrigger.trigger.slabCount);
			REINTERPRET_CAST_SAFE(clientTrigger.trigger.slabs);

			new_asset->clientTrigger.trigger.windingCount = 0;
			new_asset->clientTrigger.trigger.windings = nullptr;
			new_asset->clientTrigger.trigger.windingPointCount = 0;
			new_asset->clientTrigger.trigger.windingPoints = nullptr;

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

			new_asset->havokEntsShapeDataSize = 0;
			new_asset->havokEntsShapeData = nullptr;

			new_asset->numSubModels = clipmap->numSubModels;
			new_asset->cmodels = allocator.allocate<IW7::cmodel_t>(clipmap->numSubModels);
			for (unsigned int i = 0; i < clipmap->numSubModels; i++)
			{
				memcpy(&new_asset->cmodels[i].bounds, &clipmap->cmodels[i].bounds, sizeof(Bounds));
				new_asset->cmodels[i].radius = clipmap->cmodels[i].radius;
				new_asset->cmodels[i].info = nullptr; //reinterpret_cast<zonetool::iw7::ClipInfo*>(clipmap->cmodels[i].info);
				new_asset->cmodels[i].physicsAsset = nullptr; // fixme
				new_asset->cmodels[i].physicsShapeOverrideIdx = 0xFFFF;
				new_asset->cmodels[i].navObstacleIdx = 0;
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

			IW7_asset->mapEnts = allocator.allocate<IW7::MapEnts>();
			IW7_asset->mapEnts->name = asset->mapEnts->name;

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
			IW7_asset->stageTrigger.count = 0;
			IW7_asset->stageTrigger.models = nullptr;
			IW7_asset->stageTrigger.hullCount = 0;
			IW7_asset->stageTrigger.hulls = nullptr;
			IW7_asset->stageTrigger.slabCount = 0;
			IW7_asset->stageTrigger.slabs = nullptr;

			// todo...
			IW7_asset->broadphaseMin[0] = -131072.f;
			IW7_asset->broadphaseMin[1] = -131072.f;
			IW7_asset->broadphaseMin[2] = -131072.f;
			IW7_asset->broadphaseMax[0] = 131072.f;
			IW7_asset->broadphaseMax[1] = 131072.f;
			IW7_asset->broadphaseMax[2] = 131072.f;
			
			IW7_asset->physicsCapacities; // todo...

			IW7_asset->havokWorldShapeDataSize = 0;
			IW7_asset->havokWorldShapeData = nullptr; // todo...

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