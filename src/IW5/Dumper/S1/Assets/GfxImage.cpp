#include "stdafx.hpp"

#include "GfxImage.hpp"

#include <IW5/Dumper/H1/Assets/GfxImage.hpp>

namespace ZoneTool::IW5::S1Dumper
{
	void dump(GfxImage* asset)
	{
		// 1:1 to H1
		ZoneTool::IW5::H1Dumper::dump(asset);
	}
}
