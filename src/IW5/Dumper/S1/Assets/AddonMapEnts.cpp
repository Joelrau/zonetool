#include "stdafx.hpp"

#include "AddonMapEnts.hpp"
#include "Converter/S1/Assets/AddonMapEnts.hpp"
#include "S1/Assets/AddonMapEnts.hpp"

namespace ZoneTool::IW5::S1Dumper
{
	void dump(AddonMapEnts* asset)
	{
		allocator allocator;
		auto* s1_asset = S1Converter::convert(asset, allocator);

		S1::IAddonMapEnts::dump(s1_asset);
	}
}
