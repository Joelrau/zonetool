#include "stdafx.hpp"

#include "GfxImage.hpp"

#include <H1\Assets\GfxImage.hpp>

namespace ZoneTool::S1
{
	void IGfxImage::dump(GfxImage* asset)
	{
		H1::IGfxImage::dump(reinterpret_cast<H1::GfxImage*>(asset));
	}
}
