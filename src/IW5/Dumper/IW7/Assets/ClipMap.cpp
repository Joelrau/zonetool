#include "stdafx.hpp"

#include "ClipMap.hpp"
#include "Converter/IW7/Assets/ClipMap.hpp"
#include "IW7/Assets/ClipMap.hpp"
#include "IW7/Assets/MapEnts.hpp"
#include "IW7/Assets/PhysicsAsset.hpp"

#include <algorithm>

namespace ZoneTool::IW5::IW7Dumper
{
	void dump(clipMap_t* asset)
	{
		allocator allocator;

		// generate iw7 clipmap -- this builds the mapents too, since the clipmap points at
		// them and the trigger volumes have to agree between the two
		auto* iw7_asset = IW7Converter::convert(asset, allocator);

		// dump iw7 clipmap
		IW7::IClipMap::dump(iw7_asset);

		auto* iw7_mapents = iw7_asset->mapEnts;
		if (!iw7_mapents)
		{
			return;
		}

		// dump iw7 mapents
		IW7::IMapEnts::dump(iw7_mapents);

		// dump spawns
		mapents2spawns::dump_spawns(filesystem::get_dump_path() + asset->name + ".ents.spawnList.json"s, 
			iw7_mapents->entityString);

		// Brush models point at a generated dummy PhysicsAsset; without it in the zone the
		// runtime builds no body for them at all. They all share one asset, so dump each
		// distinct one once.
		std::vector<IW7::PhysicsAsset*> dumped;
		const auto dump_physics = [&](IW7::PhysicsAsset* physics)
		{
			if (!physics || std::find(dumped.begin(), dumped.end(), physics) != dumped.end())
			{
				return;
			}

			dumped.emplace_back(physics);
			IW7::IPhysicsAsset::dump(physics);
		};

		for (unsigned int i = 0; i < iw7_mapents->numSubModels; i++)
		{
			dump_physics(iw7_mapents->cmodels[i].physicsAsset);
		}

		// Triggers carry their own dummy when ZT_HAVOK_TRIGGER_SHAPES is on.
		for (unsigned int i = 0; i < iw7_mapents->trigger.count; i++)
		{
			dump_physics(iw7_mapents->trigger.models[i].physicsAsset);
		}
	}
}