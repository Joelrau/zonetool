#include "stdafx.hpp"

#include <H1\Assets\ComWorld.hpp>
#include <H1\Structs.hpp>

namespace ZoneTool::S1
{
	void IComWorld::dump(ComWorld* asset)
	{
		H1::IComWorld::dump(reinterpret_cast<H1::ComWorld*>(asset));
	}
}
