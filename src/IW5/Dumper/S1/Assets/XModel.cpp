#include "stdafx.hpp"

#include "XModel.hpp"

#include <IW5/Dumper/H1/Assets/XModel.hpp>

namespace ZoneTool::IW5::S1Dumper
{
	void dump(XModel* asset, const std::function<const char* (uint16_t)>& convertToString)
	{
		// 1:1 to H1
		ZoneTool::IW5::H1Dumper::dump(asset, convertToString);
	}
}
