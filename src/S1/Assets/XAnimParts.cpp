#include "stdafx.hpp"

#include "XAnimParts.hpp"

#include <H1\Assets\XAnimParts.hpp>

namespace ZoneTool::S1
{
	void IXAnimParts::dump(XAnimParts* asset, const std::function<const char* (std::uint16_t)>& SL_ConvertToString)
	{
		H1::IXAnimParts::dump(reinterpret_cast<H1::XAnimParts*>(asset), SL_ConvertToString);
	}
}
