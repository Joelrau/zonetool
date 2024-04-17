#pragma once

#include <H1\Structs.hpp>

namespace ZoneTool::S1
{
	class IClipMap
	{
	public:
		static void dump(clipMap_t* asset, const std::function<const char* (std::uint16_t)>& SL_ConvertToString);
	};
}
