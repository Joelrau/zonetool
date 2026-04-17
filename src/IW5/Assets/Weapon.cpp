#include "stdafx.hpp"

#include "Dumper/H1/Assets/Weapon.hpp"

namespace ZoneTool::IW5
{
	void IWeapon::dump(WeaponCompleteDef* asset)
	{
		if (zonetool::dumping_target == zonetool::dump_target::h1)
		{
			return H1Dumper::dump(asset);
		}
	}
}