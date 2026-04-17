#include "stdafx.hpp"

#include "AddonMapEnts.hpp"
#include "Converter/H1/Assets/AddonMapEnts.hpp"
#include "H1/Assets/AddonMapEnts.hpp"

namespace ZoneTool::IW5::H1Dumper
{
	void dump(AddonMapEnts* asset)
	{
		allocator allocator;
		auto* h1_asset = H1Converter::convert(asset, allocator);

		H1::IAddonMapEnts::dump(h1_asset);
	}
}