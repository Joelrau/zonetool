#include "stdafx.hpp"

#include <H1\Assets\FxWorld.hpp>
#include <H1\Structs.hpp>

namespace ZoneTool::S1
{
	void IFxWorld::dump(void* asset)
	{
		H1::IFxWorld::dump(reinterpret_cast<H1::FxWorld*>(asset));
	}
}
