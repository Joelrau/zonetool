#include "stdafx.hpp"

#include "Dumper/H1/Assets/FxWorld.hpp"
#include "Dumper/IW6/Assets/FxWorld.hpp"
#include "Dumper/S1/Assets/FxWorld.hpp"

namespace ZoneTool::IW5
{
	void IFxWorld::dump(FxWorld* asset)
	{
		if (zonetool::dumping_target == zonetool::dump_target::h1)
		{
			return H1Dumper::dump(asset);
		}
		else if (zonetool::dumping_target == zonetool::dump_target::iw6)
		{
			return IW6Dumper::dump(asset);
		}
		else if (zonetool::dumping_target == zonetool::dump_target::s1)
		{
			return S1Dumper::dump(asset);
		}
	}
}