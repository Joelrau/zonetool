#include "stdafx.hpp"

#include "FxWorld.hpp"

#include <H1\Assets\FxWorld.hpp>

namespace ZoneTool::S1
{
	void IFxWorld::dump(FxWorld* asset)
	{
		H1::IFxWorld::dump(reinterpret_cast<H1::FxWorld*>(asset));
	}
}
