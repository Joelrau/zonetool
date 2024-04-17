#include "stdafx.hpp"

#include "GlassWorld.hpp"

#include <IW5/Dumper/H1/Assets/GlassWorld.hpp>

namespace ZoneTool::IW5::S1Dumper
{
	void dump(GlassWorld* asset)
	{
		// 1:1 to H1
		ZoneTool::IW5::H1Dumper::dump(asset);
	}
}
