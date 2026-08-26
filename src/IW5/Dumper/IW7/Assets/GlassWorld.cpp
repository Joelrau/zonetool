#include "stdafx.hpp"

#include "GlassWorld.hpp"
#include "Converter/IW7/Assets/GlassWorld.hpp"
#include "IW7/Assets/GlassWorld.hpp"

namespace ZoneTool::IW5::IW7Dumper
{
	void dump(GlassWorld* asset)
	{
		// generate IW7 glassworld
		allocator allocator;
		auto* iw7_asset = IW7Converter::convert(asset, allocator);

		// dump IW7 glassworld
		IW7::IGlassWorld::dump(iw7_asset);
	}
}