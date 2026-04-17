#include "stdafx.hpp"

#include "XAnimParts.hpp"

#include <H1\Assets\XAnimParts.hpp>

namespace ZoneTool::S1
{
	void IXAnimParts::dump(XAnimParts* asset)
	{
		H1::IXAnimParts::dump(reinterpret_cast<H1::XAnimParts*>(asset));
	}
}
