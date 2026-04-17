#include "stdafx.hpp"

#include "ClipMap.hpp"
#include "Converter/H1/Assets/ClipMap.hpp"
#include "H1/Assets/ClipMap.hpp"
#include "H1/Assets/PhysWorld.hpp"
#include "H1/Utils/PhysWorld/generate.hpp"

namespace ZoneTool::IW5::H1Dumper
{
	void dump(clipMap_t* asset)
	{
		// generate h1 clipmap
		allocator allocator;
		auto* h1_asset = H1Converter::convert(asset, allocator);

		// dump h1 clipmap
		H1::IClipMap::dump(h1_asset);

		// dump physmap here too i guess, since it's needed.
		{
			/* {
				const auto path = filesystem::get_dump_path() + asset->name + ".physmap"s;
				filesystem::file file(path);
				if (file.exists())
				{
					ZONETOOL_INFO("Not generating physmap since it already exists...");
					return;
				}
			}*/

			try
			{
				ZONETOOL_INFO("Generating PhysWorld...");
				auto* physmap = ZoneTool::H1::physworld_gen::generate_physworld(h1_asset, &allocator);
				ZONETOOL_INFO("Generated PhysWorld.");

				H1::IPhysWorld::dump(physmap);
			}
			catch (std::exception& err)
			{
				ZONETOOL_ERROR("Failed to generate and dump physicsworld asset.\nError: %s", err.what());
			}
		}
	}
}