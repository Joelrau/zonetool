#include "stdafx.hpp"

#include "ClipMap.hpp"

#include <H1\Assets\ClipMap.hpp>

namespace ZoneTool::S1
{
	void IClipMap::dump(clipMap_t* asset, const std::function<const char* (std::uint16_t)>& SL_ConvertToString)
	{
		H1::IClipMap::dump(reinterpret_cast<H1::clipMap_t*>(asset), SL_ConvertToString);
	}
}
