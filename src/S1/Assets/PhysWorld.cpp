#include "stdafx.hpp"

#include "PhysWorld.hpp"

#include <H1\Assets\PhysWorld.hpp>

namespace ZoneTool::S1
{
	void IPhysWorld::dump(PhysWorld* asset)
	{
		H1::IPhysWorld::dump(reinterpret_cast<H1::PhysWorld*>(asset));
	}
}
