#include "stdafx.hpp"

#include "PathData.hpp"
#include "Converter/S1/Assets/PathData.hpp"
#include "S1/Assets/PathData.hpp"

namespace ZoneTool::IW5::S1Dumper
{
	void dump(PathData* asset)
	{
		allocator allocator;
		auto* s1_asset = S1Converter::convert(asset, allocator);

		S1::IPathData::dump(s1_asset);
	}
}
