#include "stdafx.hpp"

namespace ZoneTool::IW6
{
	void IMapEnts::dump_splineList(const std::string& name, SplineRecordList* splineList)
	{
		assetmanager::dumper dumper;
		const auto path = name + ".ents.splineList"s;
		if (dumper.open(path))
		{
			dumper.dump_short(splineList->splineCount);
			dumper.dump_array(splineList->splines, splineList->splineCount);
			for (unsigned short i = 0; i < splineList->splineCount; i++)
			{
				dumper.dump_array(splineList->splines[i].splinePoints, splineList->splines[i].splinePointCount);
				for (unsigned short j = 0; j < splineList->splines[i].splinePointCount; j++)
				{
					dumper.dump_array(splineList->splines[i].splinePoints[j].splineNodeLabel, 64);
					dumper.dump_array(splineList->splines[i].splinePoints[j].positionCubic, 4);
					dumper.dump_array(splineList->splines[i].splinePoints[j].tangentQuadratic, 3);
				}
			}
		}
	}

	void IMapEnts::dump_spawnList(const std::string& name, SpawnPointRecordList* spawnList)
	{
		const auto path = name + ".ents.spawnList.json"s;
		auto file = filesystem::file(path);
		file.open("wb");

		if (!file.get_fp())
		{
			return;
		}

		ordered_json data;

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

	void IMapEnts::dump_clientBlendTriggers(const std::string& name, ClientTriggerBlend* clientTriggerBlend)
	{
		assetmanager::dumper dumper;
		const auto path = name + ".ents.clientBlendTriggers"s;
		if (dumper.open(path))
		{
			dumper.dump_short(clientTriggerBlend->numClientTriggerBlendNodes);
			dumper.dump_array(clientTriggerBlend->blendNodes, clientTriggerBlend->numClientTriggerBlendNodes);
		}
	}

	void IMapEnts::dump_clientTriggers(const std::string& name, ClientTriggers* clientTrigger)
	{
		assetmanager::dumper dumper;
		const auto path = name + ".ents.clientTriggers";
		if (dumper.open(path))
		{
			dumper.dump_int(clientTrigger->trigger.count);
			dumper.dump_array<TriggerModel>(clientTrigger->trigger.models, clientTrigger->trigger.count);

			dumper.dump_int(clientTrigger->trigger.hullCount);
			dumper.dump_array<TriggerHull>(clientTrigger->trigger.hulls, clientTrigger->trigger.hullCount);

			dumper.dump_int(clientTrigger->trigger.slabCount);
			dumper.dump_array<TriggerSlab>(clientTrigger->trigger.slabs, clientTrigger->trigger.slabCount);

			dumper.dump_array(clientTrigger->clientTriggerAabbTree, clientTrigger->numClientTriggerNodes);

			dumper.dump_int(clientTrigger->triggerStringLength);
			dumper.dump_array(clientTrigger->triggerString, clientTrigger->triggerStringLength);

			dumper.dump_array(clientTrigger->visionSetTriggers, clientTrigger->trigger.count);
			dumper.dump_array(clientTrigger->triggerType, clientTrigger->trigger.count);
			dumper.dump_array(clientTrigger->origins, clientTrigger->trigger.count);
			dumper.dump_array(clientTrigger->scriptDelay, clientTrigger->trigger.count);
			dumper.dump_array(clientTrigger->audioTriggers, clientTrigger->trigger.count);
			dumper.dump_array(clientTrigger->blendLookup, clientTrigger->trigger.count);
			dumper.dump_array(clientTrigger->npcTriggers, clientTrigger->trigger.count);

			dumper.close();
		}
	}

	void IMapEnts::dump_triggers(const std::string& name, MapTriggers* trigger)
	{
		assetmanager::dumper dumper;
		const auto path = name + ".ents.triggers"s;
		if (dumper.open(path))
		{
			dumper.dump_int(trigger->count);
			dumper.dump_array<TriggerModel>(trigger->models, trigger->count);

			dumper.dump_int(trigger->hullCount);
			dumper.dump_array<TriggerHull>(trigger->hulls, trigger->hullCount);

			dumper.dump_int(trigger->slabCount);
			dumper.dump_array<TriggerSlab>(trigger->slabs, trigger->slabCount);

			dumper.close();
		}
	}

	void IMapEnts::dump_entityStrings(const std::string& name, char* entityString, int numEntityChars)
	{
		const auto path = name + ".ents"s;
		auto file = filesystem::file(path);
		file.open("wb");
		if (file.get_fp())
		{
			file.write(entityString, numEntityChars, 1);
			file.close();
		}
	}

	void IMapEnts::dump(MapEnts* asset)
	{
		dump_entityStrings(asset->name, asset->entityString, asset->numEntityChars);
		dump_triggers(asset->name, &asset->trigger);
		dump_clientTriggers(asset->name, &asset->clientTrigger);
		dump_clientBlendTriggers(asset->name, &asset->clientTriggerBlend);
		dump_spawnList(asset->name, &asset->spawnList);
		dump_splineList(asset->name, &asset->splineList);
	}
}