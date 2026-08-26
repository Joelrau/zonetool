#include "stdafx.hpp"

#include "GfxWorld.hpp"
#include "Converter/IW7/Assets/GfxWorld.hpp"
#include "IW7/Assets/GfxWorld.hpp"
#include "IW7/Assets/GfxWorldTr.hpp"

namespace ZoneTool::IW5::IW7Dumper
{
	void dump(GfxWorld* asset)
	{
		// generate IW7 gfxworld
		allocator allocator;
		auto* iw7_asset = IW7Converter::convert(asset, allocator);

		// dump IW7 gfxworld
		IW7::IGfxWorld::dump(iw7_asset);

		// dump IW/ gfxworld_tr
		IW7::IGfxWorldTr::dump(iw7_asset->draw.transientZones[0]);
	}
}