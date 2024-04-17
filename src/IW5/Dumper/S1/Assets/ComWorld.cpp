#include "stdafx.hpp"

#include "ComWorld.hpp"

#include <IW5/Dumper/H1/Assets/ComWorld.hpp>

namespace ZoneTool::IW5::S1Dumper
{
	void dump(ComWorld* asset)
	{
		// 1:1 to H1
		ZoneTool::IW5::H1Dumper::dump(asset);
	}
}
