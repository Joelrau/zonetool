#pragma once

namespace ZoneTool::H1
{
	class IWeaponAttachment : public IAsset
	{
	public:
		static void dump(WeaponAttachment* asset, const std::function<const char* (std::uint16_t)>& SL_ConvertToString);
	};
}