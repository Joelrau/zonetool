#include "stdafx.hpp"

#include "GfxLightDef.hpp"

#include <H1\Assets\GfxLightDef.hpp>

namespace ZoneTool::S1
{
	void IGfxLightDef::dump(GfxLightDef* asset)
	{
		H1::IGfxLightDef::dump(reinterpret_cast<H1::GfxLightDef*>(asset));
	}
}