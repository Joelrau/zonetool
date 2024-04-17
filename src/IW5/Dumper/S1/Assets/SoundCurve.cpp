#include "stdafx.hpp"

#include "SoundCurve.hpp"

#include <IW5/Dumper/H1/Assets/SoundCurve.hpp>

namespace ZoneTool::IW5::S1Dumper
{
	void dump(SndCurve* asset)
	{
		// 1:1 to H1
		ZoneTool::IW5::H1Dumper::dump(asset);
	}
}
