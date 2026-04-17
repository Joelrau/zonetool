#include "stdafx.hpp"

#include "XAnimParts.hpp"
#include "Converter/IW7/Assets/XAnimParts.hpp"
#include "IW7/Assets/XAnimParts.hpp"

namespace ZoneTool::IW5::IW7Dumper
{
	void dump(XAnimParts* asset)
	{
		// generate IW7 anims
		allocator allocator;
		auto iw7_asset = IW7Converter::convert(asset, allocator);

		// dump IW7 anims
		IW7::IXAnimParts::dump(iw7_asset);
	}
}