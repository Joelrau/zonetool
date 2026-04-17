#include "stdafx.hpp"

#include "ClipMap.hpp"
#include "Converter/IW6/Assets/ClipMap.hpp"
#include "IW6/Assets/ClipMap.hpp"

namespace ZoneTool::IW5::IW6Dumper
{
	void dump(clipMap_t* asset)
	{
		// generate iw6 clipmap
		allocator allocator;
		auto* iw6_asset = IW6Converter::convert(asset, allocator);

		// dump iw6 clipmap
		IW6::IClipMap::dump(iw6_asset);
	}
}