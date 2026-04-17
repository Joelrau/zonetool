#include "stdafx.hpp"

#include "Dumper/H1/Assets/WeaponAttachment.hpp"

namespace ZoneTool::IW5
{
	void IWeaponAttachment::dump(WeaponAttachment* asset)
	{
		if (zonetool::dumping_target == zonetool::dump_target::h1)
		{
			return H1Dumper::dump(asset);
		}
	}
}