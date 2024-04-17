#include "stdafx.hpp"

#include "MapEnts.hpp"
#include "Converter/S1/Assets/MapEnts.hpp"
#include "S1/Assets/MapEnts.hpp"

namespace ZoneTool::IW5::S1Dumper
{
	void dump(MapEnts* asset)
	{
		// generate s1 mapents
		allocator allocator;
		auto* s1_asset = S1Converter::convert(asset, allocator);

		// dump s1 mapents
		S1::IMapEnts::dump(s1_asset, SL_ConvertToString);

		// dump spawns
		mapents2spawns::dump_spawns(filesystem::get_dump_path() + asset->name + ".ents.spawnList"s, asset->entityString);
	}
}
