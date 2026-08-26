#include "stdafx.hpp"

#include "ComWorld.hpp"
#include "Converter/IW7/Assets/ComWorld.hpp"
#include "IW7/Assets/ComWorld.hpp"

namespace ZoneTool::IW5::IW7Dumper
{
	void dump(ComWorld* asset)
	{
		// generate iw7 comworld
		allocator allocator;
		auto* iw7_asset = IW7Converter::convert(asset, allocator);

		// dump iw7 comworld
		IW7::IComWorld::dump(iw7_asset);
	}
}