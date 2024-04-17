#include "stdafx.hpp"

#include "Dumper/H1/Assets/XAnimParts.hpp"
#include "Dumper/IW6/Assets/XAnimParts.hpp"
#include "Dumper/IW7/Assets/XAnimParts.hpp"
#include "Dumper/S1/Assets/XAnimParts.hpp"

namespace ZoneTool::IW5
{
	void IXAnimParts::dump(XAnimParts* asset, const std::function<const char* (uint16_t)>& convertToString)
	{
		if (zonetool::dumping_target == zonetool::dump_target::h1)
		{
			return H1Dumper::dump(asset, convertToString);
		}
		else if (zonetool::dumping_target == zonetool::dump_target::iw6)
		{
			return IW6Dumper::dump(asset, convertToString);
		}
		else if (zonetool::dumping_target == zonetool::dump_target::iw7)
		{
			return IW7Dumper::dump(asset, convertToString);
		}
		else if (zonetool::dumping_target == zonetool::dump_target::s1)
		{
			return S1Dumper::dump(asset, convertToString);
		}
	}
}