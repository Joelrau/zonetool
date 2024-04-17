#pragma once

#include <H1\Structs.hpp>

namespace ZoneTool::S1
{
	class IXModel
	{
	public:
		static void dump(XModel* asset, const std::function<const char* (std::uint16_t)>& SL_ConvertToString);
	};
}
