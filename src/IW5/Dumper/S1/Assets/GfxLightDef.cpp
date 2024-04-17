#include "stdafx.hpp"

#include "GfxLightDef.hpp"

#include <IW5/Dumper/H1/Assets/GfxLightDef.hpp>

namespace ZoneTool::IW5::S1Dumper
{
	void dump(GfxLightDef* asset)
	{
		// 1:1 to H1
		ZoneTool::IW5::H1Dumper::dump(asset);
	}
}
