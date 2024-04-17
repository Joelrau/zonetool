#include "stdafx.hpp"

#include "SoundCurve.hpp"

#include <H1\Assets\SoundCurve.hpp>

namespace ZoneTool::S1
{
	void ISoundCurve::dump(SndCurve* asset, const std::string& type)
	{
		H1::ISoundCurve::dump(reinterpret_cast<H1::SndCurve*>(asset), type);
	}

	void ISoundCurve::dump(SndCurve* asset)
	{
		dump(asset, "sndcurve");
	}
}
