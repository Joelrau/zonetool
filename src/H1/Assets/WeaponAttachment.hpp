#pragma once

namespace ZoneTool::H1
{
	class IWeaponAttachment : public IAsset
	{
	public:
		static void dump(WeaponAttachment* asset);
	};
}