#include "stdafx.hpp"

#include <H1\Assets\MapEnts.hpp>
#include <H1\Structs.hpp>

namespace ZoneTool::S1
{
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
			dumper.dump_array(clientTrigger->unk1, clientTrigger->trigger.count);
			dumper.dump_array(clientTrigger->unk2, clientTrigger->trigger.count);
			dumper.dump_array(clientTrigger->triggerType, clientTrigger->trigger.count);
			dumper.dump_array(clientTrigger->origins, clientTrigger->trigger.count);
			dumper.dump_array(clientTrigger->scriptDelay, clientTrigger->trigger.count);
			dumper.dump_array(clientTrigger->audioTriggers, clientTrigger->trigger.count);
			dumper.dump_array(clientTrigger->blendLookup, clientTrigger->trigger.count);
			dumper.dump_array(clientTrigger->unk3, clientTrigger->trigger.count);
			dumper.dump_array(clientTrigger->unk4, clientTrigger->trigger.count);
			dumper.dump_array(clientTrigger->unk5, clientTrigger->trigger.count);

			dumper.close();
		}
	}

	void IMapEnts::dump(MapEnts* asset, const std::function<const char* (std::uint16_t)>& SL_ConvertToString)
	{
		H1::IMapEnts::dump_entityStrings(asset->name, asset->entityString, asset->numEntityChars);
		H1::IMapEnts::dump_triggers(asset->name, reinterpret_cast<H1::MapTriggers*>(&asset->trigger));
		dump_clientTriggers(asset->name, &asset->clientTrigger);
		H1::IMapEnts::dump_clientBlendTriggers(asset->name, reinterpret_cast<H1::ClientTriggerBlend*>(&asset->clientTriggerBlend));
		H1::IMapEnts::dump_spawnList(asset->name, reinterpret_cast<H1::SpawnPointRecordList*>(&asset->spawnList), SL_ConvertToString);
		H1::IMapEnts::dump_splineList(asset->name, reinterpret_cast<H1::SplineRecordList*>(&asset->splineList));
	}
}
