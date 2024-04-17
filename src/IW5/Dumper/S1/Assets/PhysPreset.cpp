#include "stdafx.hpp"

#include "PhysPreset.hpp"

#include <IW5/Dumper/H1/Assets/PhysPreset.hpp>

namespace ZoneTool::IW5::S1Dumper
{
	void dump(PhysPreset* asset)
	{
		// 1:1 to H1
		ZoneTool::IW5::H1Dumper::dump(asset);
	}
}
