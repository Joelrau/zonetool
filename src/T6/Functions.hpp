#pragma once

namespace ZoneTool
{
	namespace T6
	{
		union XAssetHeader;

		static Function<XAssetHeader(std::int32_t, const char*, bool, std::uint32_t)> DB_FindXAssetHeader = SELECT(0x6F9030, 0x43F460);
		static Function<bool(std::int32_t, const char*)> DB_IsXAssetDefault = SELECT(0x4F3F70, 0x45BE90);
		//static Function<void(int, XZoneInfo*, std::uint32_t, std::uint32_t)> DB_LoadXAssets = 0x663090;
		static Function<bool(const char*)> DB_IsZoneLoaded = SELECT(0x80510, 0x430DA0);
		static Function<void(int, bool)> DB_UnloadXZoneInternal = SELECT(0x7FE4D0, 0x7FD8D0);
		static Function<char* (unsigned int)> SL_ConvertToString_game = SELECT(0x4560B0, 0x532230);
		static Function<void(XZone*)> DB_UnloadXAssetsMemory = SELECT(0x7FE810, 0x7FDC10);
	
		static const char* SL_ConvertToString(std::uint16_t index)
		{
			return SL_ConvertToString_game(index);
		}

		static short SL_AllocString(const std::string& string)
		{
			return Memory::func<short(const char*, std::uint32_t, std::size_t)>(0x4E75A0)(
				string.data(), 1, string.size() + 1);
		}
	}
}
