#include "stdafx.hpp"

#include "ClipMap.hpp"
#include "../Common/havok.hpp"

namespace ZoneTool::IW7
{
	void IMapEnts::dump_spawn_list(const std::string& name, SpawnPointRecordList* spawnList)
	{
		const auto path = name + ".ents.spawnList.json"s;
		auto file = filesystem::file(path);
		file.open("wb");

		if (!file.get_fp())
		{
			return;
		}

		ordered_json data{};

		for (unsigned short i = 0; i < spawnList->spawnsCount; i++)
		{
			data[i]["name"] = SL_ConvertToString(spawnList->spawns[i].name);
			data[i]["target"] = SL_ConvertToString(spawnList->spawns[i].target);
			data[i]["script_noteworthy"] = SL_ConvertToString(spawnList->spawns[i].script_noteworthy);
			for (auto j = 0; j < 3; j++)
			{
				data[i]["origin"][j] = spawnList->spawns[i].origin[j];
				data[i]["angles"][j] = spawnList->spawns[i].angles[j];
			}
		}

		const auto json = data.dump(4);
		file.write(json.data(), json.size(), 1);

		file.close();
	}

	void IMapEnts::dump_spawners(const std::string& name, SpawnerList* spawners)
	{
		const auto path = name + ".ents.spawners"s;
		auto file = filesystem::file(path);
		file.open("wb");

		if (!file.get_fp())
		{
			return;
		}

		ordered_json data{};

		for (unsigned int i = 0; i < spawners->spawnerCount; i++)
		{
			data[i]["targetname"] = SL_ConvertToString(spawners->spawnerList[i].targetname);
			data[i]["classname"] = SL_ConvertToString(spawners->spawnerList[i].classname);
			data[i]["count"] = spawners->spawnerList[i].count;

			for (auto j = 0; j < 3; j++)
			{
				data[i]["origin"][j] = spawners->spawnerList[i].origin[j];
				data[i]["angles"][j] = spawners->spawnerList[i].angles[j];
			}

			data[i]["fields"] = {};
			for (unsigned int j = 0; j < spawners->spawnerList[i].numFields; j++)
			{
				data[i]["fields"][j]["key"] = SL_ConvertToString(spawners->spawnerList[i].fields[j].key);
				data[i]["fields"][j]["value"] = SL_ConvertToString(spawners->spawnerList[i].fields[j].value);
				data[i]["fields"][j]["type"] = spawners->spawnerList[i].fields[j].type;
			}
		}

		const auto json = data.dump(4);
		file.write(json.data(), json.size(), 1);

		file.close();
	}

	void IMapEnts::dump_entity_strings(const std::string& name, char* entityString, int numEntityChars)
	{
		const auto path = name + ".ents"s;
		auto file = filesystem::file(path);
		file.open("wb");

		if (!file.get_fp())
		{
			return;
		}

		file.write(entityString, numEntityChars, 1);
		file.close();
	}

	void IMapEnts::dump(MapEnts* asset)
	{
		assetmanager::dumper dumper;
		const auto path = asset->name + ".ents.data"s;
		if (!dumper.open(path))
		{
			return;
		}

		dumper.dump_single(asset);

		dump_entity_strings(asset->name, asset->entityString, asset->numEntityChars);
		dump_spawn_list(asset->name, &asset->spawnList);
		dump_spawners(asset->name, &asset->spawners);

		dumper.dump_array<TriggerModel>(asset->trigger.models, asset->trigger.count);
		for (unsigned int i = 0; i < asset->trigger.count; i++)
		{
			dumper.dump_asset(asset->trigger.models[i].physicsAsset);
		}
		dumper.dump_array<TriggerHull>(asset->trigger.hulls, asset->trigger.hullCount);
		dumper.dump_array<TriggerSlab>(asset->trigger.slabs, asset->trigger.slabCount);
		dumper.dump_array<TriggerWinding>(asset->trigger.windings, asset->trigger.windingCount);
		dumper.dump_array<TriggerWindingPoint>(asset->trigger.windingPoints, asset->trigger.windingPointCount);

		dumper.dump_array<TriggerModel>(asset->clientTrigger.trigger.models, asset->clientTrigger.trigger.count);
		for (unsigned int i = 0; i < asset->clientTrigger.trigger.count; i++)
		{
			dumper.dump_asset(asset->clientTrigger.trigger.models[i].physicsAsset);
		}
		dumper.dump_array<TriggerHull>(asset->clientTrigger.trigger.hulls, asset->clientTrigger.trigger.hullCount);
		dumper.dump_array<TriggerSlab>(asset->clientTrigger.trigger.slabs, asset->clientTrigger.trigger.slabCount);
		dumper.dump_array<TriggerWinding>(asset->clientTrigger.trigger.windings, asset->clientTrigger.trigger.windingCount);
		dumper.dump_array<TriggerWindingPoint>(asset->clientTrigger.trigger.windingPoints, asset->clientTrigger.trigger.windingPointCount);
		dumper.dump_array(asset->clientTrigger.triggerString, asset->clientTrigger.triggerStringLength);
		dumper.dump_array(asset->clientTrigger.visionSetTriggers, asset->clientTrigger.trigger.count);
		dumper.dump_array(asset->clientTrigger.triggerType, asset->clientTrigger.trigger.count);
		dumper.dump_array(asset->clientTrigger.origins, asset->clientTrigger.trigger.count);
		dumper.dump_array(asset->clientTrigger.scriptDelay, asset->clientTrigger.trigger.count);
		dumper.dump_array(asset->clientTrigger.audioTriggers, asset->clientTrigger.trigger.count);
		dumper.dump_array(asset->clientTrigger.blendLookup, asset->clientTrigger.trigger.count);
		dumper.dump_array(asset->clientTrigger.npcTriggers, asset->clientTrigger.trigger.count);
		dumper.dump_array(asset->clientTrigger.audioStateIds, asset->clientTrigger.trigger.count);
		dumper.dump_array(asset->clientTrigger.audioRvbPanInfo, asset->clientTrigger.trigger.count);
		dumper.dump_array(asset->clientTrigger.transientIndex, asset->clientTrigger.trigger.count);
		if (asset->clientTrigger.linkTo)
		{
			dumper.dump_array(asset->clientTrigger.linkTo, asset->clientTrigger.trigger.count);
			for (unsigned int i = 0; i < asset->clientTrigger.trigger.count; i++)
			{
				dumper.dump_single(asset->clientTrigger.linkTo[i]);
			}
		}
		dumper.dump_array(asset->clientTriggerBlend.blendNodes, asset->clientTriggerBlend.numClientTriggerBlendNodes);

		dumper.dump_array(asset->splineList.splines, asset->splineList.splineCount);
		for (unsigned short i = 0; i < asset->splineList.splineCount; i++)
		{
			dumper.dump_array(asset->splineList.splines[i].splinePoints, asset->splineList.splines[i].splinePointCount);
			for (unsigned short j = 0; j < asset->splineList.splines[i].splinePointCount; j++)
			{
				dumper.dump_string(SL_ConvertToString(asset->splineList.splines[i].splinePoints[j].splineNodeLabel));
				dumper.dump_string(SL_ConvertToString(asset->splineList.splines[i].splinePoints[j].targetname));
				dumper.dump_string(SL_ConvertToString(asset->splineList.splines[i].splinePoints[j].target));
				dumper.dump_string(SL_ConvertToString(asset->splineList.splines[i].splinePoints[j].string));
			}
		}

		const auto havok_data_path = path;
		havok::binary::dump_havok_data(havok_data_path, asset->havokEntsShapeData, asset->havokEntsShapeDataSize);

		dumper.dump_array(asset->cmodels, asset->numSubModels);
		for (unsigned int i = 0; i < asset->numSubModels; i++)
		{
			dumper.dump_single(asset->cmodels[i].info);
			if (asset->cmodels[i].info)
			{
				IClipMap::dump_info(asset->cmodels[i].info, dumper);
			}
			dumper.dump_asset(asset->cmodels[i].physicsAsset);
		}

		dumper.dump_array(asset->dynEntDefList[0], asset->dynEntCount[0]);
		for (unsigned short i = 0; i < asset->dynEntCount[0]; i++)
		{
			dumper.dump_asset(asset->dynEntDefList[0][i].baseModel);
			dumper.dump_single(asset->dynEntDefList[0][i].linkTo);
		}

		dumper.dump_array(asset->dynEntDefList[1], asset->dynEntCount[1]);
		for (unsigned short i = 0; i < asset->dynEntCount[1]; i++)
		{
			dumper.dump_asset(asset->dynEntDefList[1][i].baseModel);
			dumper.dump_single(asset->dynEntDefList[1][i].linkTo);
		}

		dumper.dump_array(asset->dynEntPoseList[0][0], asset->dynEntCount[0]);
		for (unsigned short i = 0; i < asset->dynEntCount[0]; i++)
		{
			dumper.dump_array(asset->dynEntPoseList[0][0][i].poses, asset->dynEntPoseList[0][0][i].numPoses);
			dumper.dump_array(asset->dynEntPoseList[0][0][i].unk, asset->dynEntPoseList[0][0][i].numPoses);
		}

		dumper.dump_array(asset->dynEntPoseList[1][0], asset->dynEntCount[0]);
		for (unsigned short i = 0; i < asset->dynEntCount[0]; i++)
		{
			dumper.dump_array(asset->dynEntPoseList[1][0][i].poses, asset->dynEntPoseList[1][0][i].numPoses);
			dumper.dump_array(asset->dynEntPoseList[1][0][i].unk, asset->dynEntPoseList[1][0][i].numPoses);
		}

		dumper.dump_array(asset->dynEntPoseList[0][1], asset->dynEntCount[1]);
		for (unsigned short i = 0; i < asset->dynEntCount[1]; i++)
		{
			dumper.dump_array(asset->dynEntPoseList[0][1][i].poses, asset->dynEntPoseList[0][1][i].numPoses);
			dumper.dump_array(asset->dynEntPoseList[0][1][i].unk, asset->dynEntPoseList[0][1][i].numPoses);
		}

		dumper.dump_array(asset->dynEntPoseList[1][1], asset->dynEntCount[1]);
		for (unsigned short i = 0; i < asset->dynEntCount[1]; i++)
		{
			dumper.dump_array(asset->dynEntPoseList[1][1][i].poses, asset->dynEntPoseList[1][1][i].numPoses);
			dumper.dump_array(asset->dynEntPoseList[1][1][i].unk, asset->dynEntPoseList[1][1][i].numPoses);
		}

		dumper.dump_array(asset->dynEntGlobalIdList[0], asset->dynEntCountTotal);
		dumper.dump_array(asset->dynEntGlobalIdList[1], asset->dynEntCountTotal);

		dumper.dump_array(asset->unk2, asset->unk2Count);
		for (unsigned int i = 0; i < asset->unk2Count; i++)
		{
			dumper.dump_array(asset->unk2[i].unk01, asset->unk2[i].unk01Count);
		}
		dumper.dump_array(asset->unk2_1[0], asset->unk2Count);
		dumper.dump_array(asset->unk2_1[1], asset->unk2Count);
		dumper.dump_array(asset->unk2_2[0], asset->unk2Count);
		dumper.dump_array(asset->unk2_2[1], asset->unk2Count);

		dumper.dump_array(asset->unk3, asset->unk3Count);

		dumper.dump_array(asset->clientEntAnchors, asset->clientEntAnchorCount);
		for (unsigned int i = 0; i < asset->clientEntAnchorCount; i++)
		{
			dumper.dump_string(SL_ConvertToString(asset->clientEntAnchors[i].name));
		}

		dumper.dump_array(asset->scriptableMapEnts.instances, asset->scriptableMapEnts.totalInstanceCount);
		for (unsigned int i = 0; i < asset->scriptableMapEnts.totalInstanceCount; i++)
		{
			dumper.dump_asset(asset->scriptableMapEnts.instances[i].unk01.unk01.def);
			dumper.dump_asset(asset->scriptableMapEnts.instances[i].unk01.unk01.unk01.model);
			dumper.dump_array(asset->scriptableMapEnts.instances[i].unk01.unk01.eventStreamBuffer, asset->scriptableMapEnts.instances[i].unk01.unk01.eventStreamBufferSize);

			dumper.dump_asset(asset->scriptableMapEnts.instances[i].unk02[0].unk01.def);
			dumper.dump_asset(asset->scriptableMapEnts.instances[i].unk02[0].unk01.unk01.model);
			dumper.dump_array(asset->scriptableMapEnts.instances[i].unk02[0].unk01.eventStreamBuffer, asset->scriptableMapEnts.instances[i].unk02[0].unk01.eventStreamBufferSize);

			dumper.dump_asset(asset->scriptableMapEnts.instances[i].unk02[1].unk01.def);
			dumper.dump_asset(asset->scriptableMapEnts.instances[i].unk02[1].unk01.unk01.model);
			dumper.dump_array(asset->scriptableMapEnts.instances[i].unk02[1].unk01.eventStreamBuffer, asset->scriptableMapEnts.instances[i].unk02[1].unk01.eventStreamBufferSize);

			dumper.dump_string(SL_ConvertToString(asset->scriptableMapEnts.instances[i].unk03));
			dumper.dump_string(asset->scriptableMapEnts.instances[i].unk04);
			dumper.dump_string(SL_ConvertToString(asset->scriptableMapEnts.instances[i].targetname));
		}

		dumper.dump_array(asset->scriptableMapEnts.unk.unk01, asset->scriptableMapEnts.unk.unk01Count);
		dumper.dump_array(asset->scriptableMapEnts.unk.unk02_1, asset->scriptableMapEnts.unk.unk02Count);
		dumper.dump_array(asset->scriptableMapEnts.unk.unk02_2, asset->scriptableMapEnts.unk.unk02Count);

		dumper.dump_array(asset->scriptableMapEnts.reservedDynents[0].reservedDynents, asset->scriptableMapEnts.reservedDynents[0].numReservedDynents);
		dumper.dump_array(asset->scriptableMapEnts.reservedDynents[1].reservedDynents, asset->scriptableMapEnts.reservedDynents[1].numReservedDynents);

		dumper.dump_array(asset->mayhemScenes, asset->numMayhemScenes);
		for (unsigned int i = 0; i < asset->numMayhemScenes; i++)
		{
			dumper.dump_asset(asset->mayhemScenes[i].mayhem);
			dumper.dump_single(asset->mayhemScenes[i].linkTo);
			dumper.dump_string(SL_ConvertToString(asset->mayhemScenes[i].scriptName));
		}

		dumper.dump_array(asset->audioPASpeakers, asset->audioPASpeakerCount);

		dumper.close();
	}
}