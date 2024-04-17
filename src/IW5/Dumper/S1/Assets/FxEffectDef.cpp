#include "stdafx.hpp"

#include "FxEffectDef.hpp"

#include <IW5/Dumper/H1/Assets/FxEffectDef.hpp>

namespace ZoneTool::IW5::S1Dumper
{
	void dump(FxEffectDef* asset)
	{
		// 1:1 to H1
		ZoneTool::IW5::H1Dumper::dump(asset);
	}
}
