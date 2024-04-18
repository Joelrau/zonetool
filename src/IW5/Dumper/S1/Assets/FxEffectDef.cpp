#include "stdafx.hpp"

#include "FxEffectDef.hpp"
#include "Converter/S1/Assets/FxEffectDef.hpp"
#include "S1/Assets/FxEffectDef.hpp"

namespace ZoneTool::IW5::S1Dumper
{
	void dump(FxEffectDef* asset)
	{
		// generate h1 fx
		allocator allocator;
		auto* s1_asset = S1Converter::convert(asset, allocator);

		// dump fx
		S1::IFxEffectDef::dump(s1_asset);
	}
}
