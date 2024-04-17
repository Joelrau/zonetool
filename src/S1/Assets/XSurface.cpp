#include "stdafx.hpp"

#include "XSurface.hpp"

#include <H1\Assets\XSurface.hpp>

namespace ZoneTool::S1
{
	void IXSurface::dump(XModelSurfs* asset)
	{
		H1::IXSurface::dump(reinterpret_cast<H1::XModelSurfs*>(asset));
	}
}
