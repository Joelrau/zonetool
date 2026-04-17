#include "stdafx.hpp"

#include "XAnimParts.hpp"
#include "Converter/IW6/Assets/XAnimParts.hpp"
#include "IW6/Assets/XAnimParts.hpp"

namespace ZoneTool::IW5::IW6Dumper
{
	void dump(XAnimParts* asset)
	{
		// generate IW6 anims
		allocator allocator;
		auto iw6_asset = IW6Converter::convert(asset, allocator);

		// dump IW6 anims
		IW6::IXAnimParts::dump(iw6_asset);
	}
}