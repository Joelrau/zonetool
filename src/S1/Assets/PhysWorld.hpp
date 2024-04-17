#pragma once

#include <H1\Structs.hpp>

namespace ZoneTool::S1
{
	class IPhysWorld
	{
	public:
		static void dump(PhysWorld* asset, const std::function<const char* (std::uint16_t)>& SL_ConvertToString);
	};
}
