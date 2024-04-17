#include "stdafx.hpp"

#include "GfxWorld.hpp"
#include "Converter/S1/Assets/GfxWorld.hpp"
#include "S1/Assets/GfxWorld.hpp"

namespace ZoneTool::IW5::S1Dumper
{
	void dump(GfxWorld* asset)
	{
		// generate h1 gfxworld
		allocator allocator;
		auto* h1_asset = S1Converter::convert(asset, allocator);

		// dump h1 gfxworld
		S1::IGfxWorld::dump(h1_asset);
	}
}
