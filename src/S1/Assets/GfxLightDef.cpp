#include "stdafx.hpp"

#include <H1\Assets\GfxLightDef.hpp>
#include <H1\Structs.hpp>

namespace ZoneTool::S1
{
	void IGfxLightDef::dump(void* asset)
	{
		H1::IGfxLightDef::dump(reinterpret_cast<H1::GfxLightDef*>(asset));
	}
}