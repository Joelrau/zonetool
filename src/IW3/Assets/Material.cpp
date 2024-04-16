#include "stdafx.hpp"

#include "Dumper/H1/Assets/Material.hpp"
#include "Dumper/IW6/Assets/Material.hpp"
#include "Dumper/IW7/Assets/Material.hpp"
#include "Dumper/S1/Assets/Material.hpp"

namespace ZoneTool::IW3
{
	void IMaterial::dump(Material* asset)
	{
		if (zonetool::dumping_target == zonetool::dump_target::h1)
		{
			return H1Dumper::dump(asset);
		}
		else if (zonetool::dumping_target == zonetool::dump_target::iw6)
		{
			return IW6Dumper::dump(asset);
		}
		else if (zonetool::dumping_target == zonetool::dump_target::iw7)
		{
			return IW7Dumper::dump(asset);
		}
		else if (zonetool::dumping_target == zonetool::dump_target::s1)
		{
			return S1Dumper::dump(asset);
		}
	}
}