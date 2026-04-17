#include "stdafx.hpp"

#include "ClipMap.hpp"

#include <H1\Assets\ClipMap.hpp>

namespace ZoneTool::S1
{
	void IClipMap::dump(clipMap_t* asset)
	{
		H1::IClipMap::dump(reinterpret_cast<H1::clipMap_t*>(asset));
	}
}
