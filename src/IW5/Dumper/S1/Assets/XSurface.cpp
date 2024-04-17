#include "stdafx.hpp"

#include "XSurface.hpp"

#include <IW5/Dumper/H1/Assets/XSurface.hpp>

namespace ZoneTool::IW5::S1Dumper
{
	void dump(XModelSurfs* asset)
	{
		// 1:1 to H1
		ZoneTool::IW5::H1Dumper::dump(asset);
	}
}
