#include "stdafx.hpp"

#include "Dumper/H1/Assets/Weapon.hpp"

namespace ZoneTool::IW5
{
	void IWeapon::dump(WeaponCompleteDef* asset, const std::function<const char* (uint16_t)>& convertToString)
	{
		if (zonetool::dumping_target == zonetool::dump_target::h1)
		{
			return H1Dumper::dump(asset, convertToString);
		}
	}
}