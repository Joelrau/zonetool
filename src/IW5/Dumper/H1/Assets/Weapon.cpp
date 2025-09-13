#include "stdafx.hpp"

#include "Weapon.hpp"
#include "Converter/H1/Assets/Weapon.hpp"
#include "H1/Assets/WeaponDef.hpp"

namespace ZoneTool::IW5::H1Dumper
{
	void dump(WeaponCompleteDef* asset, const std::function<const char* (uint16_t)>& convertToString)
	{
		// generate h1
		allocator allocator;
		auto h1_asset = H1Converter::convert(asset, allocator);

		// dump h1
		H1::IWeaponDef::dump(h1_asset, convertToString);
	}
}