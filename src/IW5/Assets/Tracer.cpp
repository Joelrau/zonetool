#include "stdafx.hpp"

#include "Dumper/H1/Assets/Tracer.hpp"

namespace ZoneTool::IW5
{
	void ITracerDef::dump(TracerDef* asset)
	{
		if (zonetool::dumping_target == zonetool::dump_target::h1)
		{
			return H1Dumper::dump(asset);
		}
	}
}