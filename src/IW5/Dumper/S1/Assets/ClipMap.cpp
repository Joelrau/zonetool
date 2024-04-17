#include "stdafx.hpp"

#include "ClipMap.hpp"

#include <IW5/Dumper/H1/Assets/ClipMap.hpp>

namespace ZoneTool::IW5::S1Dumper
{
	void dump(clipMap_t* asset)
	{
		// 1:1 to H1
		ZoneTool::IW5::H1Dumper::dump(asset);
	}
}
