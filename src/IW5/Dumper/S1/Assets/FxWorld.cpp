#include "stdafx.hpp"

#include "FxWorld.hpp"

#include <IW5/Dumper/H1/Assets/FxWorld.hpp>

namespace ZoneTool::IW5::S1Dumper
{
	void dump(FxWorld* asset)
	{
		// 1:1 to H1
		ZoneTool::IW5::H1Dumper::dump(asset);
	}
}
