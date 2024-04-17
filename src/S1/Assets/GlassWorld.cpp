#include "stdafx.hpp"

#include "GlassWorld.hpp"

#include <H1\Assets\GlassWorld.hpp>

namespace ZoneTool::S1
{
	void IGlassWorld::dump(GlassWorld* asset)
	{
		H1::IGlassWorld::dump(reinterpret_cast<H1::GlassWorld*>(asset));
	}
}
