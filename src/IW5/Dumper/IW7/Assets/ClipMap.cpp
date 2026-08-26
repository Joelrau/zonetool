#include "stdafx.hpp"

#include "ClipMap.hpp"
#include "Converter/IW7/Assets/ClipMap.hpp"
#include "IW7/Assets/ClipMap.hpp"
#include "IW7/Assets/MapEnts.hpp"

namespace ZoneTool::IW5::IW7Dumper
{
	void dump(clipMap_t* asset)
	{
		allocator allocator;

		// generate iw7 clipmap
		auto* iw7_asset = IW7Converter::convert(asset, allocator);

		// dump iw7 clipmap
		IW7::IClipMap::dump(iw7_asset);

		// generate iw7 mapents
		auto* iw7_mapents = IW7Converter::generate_mapents(asset, allocator);

		// dump iw7 mapents
		IW7::IMapEnts::dump(iw7_mapents);

		// dump spawns
		mapents2spawns::dump_spawns(filesystem::get_dump_path() + asset->name + ".ents.spawnList.json"s, 
			iw7_mapents->entityString);
	}
}