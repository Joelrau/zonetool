#include "stdafx.hpp"

#include "FxWorld.hpp"
#include "Converter/IW7/Assets/FxWorld.hpp"
#include "IW7/Assets/FxWorld.hpp"

namespace ZoneTool::IW5::IW7Dumper
{
	void dump(FxWorld* asset)
	{
		// generate IW7 fxworld
		allocator allocator;
		auto* iw7_asset = IW7Converter::convert(asset, allocator);

		// dump IW7 fxworld
		IW7::IFxWorld::dump(iw7_asset);
	}
}