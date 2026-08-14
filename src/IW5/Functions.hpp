#pragma once

namespace ZoneTool
{
	namespace IW5
	{
		union XAssetHeader;

		static Function<XAssetHeader(std::int32_t, const char*, std::uint32_t)> DB_FindXAssetHeader = 0x44E770;
		//static Function<bool(std::int32_t, const char*)> DB_IsXAssetDefault = 0x4CA800;
		static Function<void(XZoneInfo*, std::uint32_t, std::uint32_t)> DB_LoadXAssets = 0x44F740;

		XAssetHeader DB_FindXAssetHeader_(std::int32_t type, const char* name, bool createDefault);

		static const char* SL_ConvertToString(std::uint16_t index)
		{
			return Memory::func<const char* (int)>(0x4E72D0)(index);
		}

		static short SL_AllocString(const std::string& string)
		{
			return Memory::func<short(const char*, std::uint32_t, std::size_t, std::uint32_t)>(0x4E75A0)(
				string.data(), 1, string.size() + 1, 7);
		}
	}
}
