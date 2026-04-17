#include "stdafx.hpp"

#include "PathData.hpp"
#include "Converter/H1/Assets/PathData.hpp"
#include "H1/Assets/PathData.hpp"

#include "IW3/Structs.hpp"
#include "IW3/Functions.hpp"

#include "IW4/Structs.hpp"
#include "IW4/Functions.hpp"

namespace ZoneTool::IW5::H1Dumper
{
	void dump(PathData* asset)
	{
		allocator allocator;
		auto* h1_asset = H1Converter::convert(asset, allocator);

		H1::IPathData::dump(h1_asset);
	}
}