#include "stdafx.hpp"

#include <H1\Assets\GfxImage.hpp>
#include <H1\Structs.hpp>

namespace ZoneTool::S1
{
	void IGfxImage::dump(void* asset)
	{
		H1::IGfxImage::dump(reinterpret_cast<H1::GfxImage*>(asset));
	}
}
