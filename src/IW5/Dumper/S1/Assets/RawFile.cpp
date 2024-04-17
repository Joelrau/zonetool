#include "stdafx.hpp"

#include "RawFile.hpp"

#include <IW5/Dumper/H1/Assets/RawFile.hpp>

namespace ZoneTool::IW5::S1Dumper
{
	void dump(RawFile* asset)
	{
		// 1:1 to H1
		ZoneTool::IW5::H1Dumper::dump(asset);
	}
}
