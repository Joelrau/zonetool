#include "stdafx.hpp"

#include "WeaponAttachment.hpp"
#include "Converter/H1/Assets/WeaponAttachment.hpp"
#include "H1/Assets/WeaponAttachment.hpp"

namespace ZoneTool::IW5::H1Dumper
{
	void dump(WeaponAttachment* asset)
	{
		// generate h1 anims
		allocator allocator;
		auto h1_asset = H1Converter::convert(asset, allocator);

		// dump h1 anims
		H1::IWeaponAttachment::dump(h1_asset);
	}
}