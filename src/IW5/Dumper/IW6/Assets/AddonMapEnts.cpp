#include "stdafx.hpp"

#include "AddonMapEnts.hpp"
#include "Converter/IW6/Assets/AddonMapEnts.hpp"
#include "IW6/Assets/AddonMapEnts.hpp"

namespace ZoneTool::IW5::IW6Dumper
{
	void dump(AddonMapEnts* asset)
	{
		allocator allocator;
		auto* iw6_asset = IW6Converter::convert(asset, allocator);

		IW6::IAddonMapEnts::dump(iw6_asset);
	}
}