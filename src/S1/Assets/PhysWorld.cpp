#include "stdafx.hpp"

#include "PhysWorld.hpp"

#include <H1\Assets\PhysWorld.hpp>

namespace ZoneTool::S1
{
	void IPhysWorld::dump(PhysWorld* asset, const std::function<const char* (std::uint16_t)>& SL_ConvertToString)
	{
		H1::IPhysWorld::dump(reinterpret_cast<H1::PhysWorld*>(asset), SL_ConvertToString);
	}
}
