#pragma once

namespace ZoneTool
{
	namespace IW3
	{
		union XAssetHeader;

		static Function<XAssetHeader(std::int32_t, const char*)> DB_FindXAssetHeader = 0x489570;
		static Function<void(XZoneInfo*, std::uint32_t, std::uint32_t)> DB_LoadXAssets; //add offset

		typedef int (__cdecl * DB_GetXAssetSizeHandler_t)();
		static DB_GetXAssetSizeHandler_t* DB_GetXAssetSizeHandlers = (DB_GetXAssetSizeHandler_t*)0x726A10;

		static const char* SL_ConvertToString(std::uint16_t index)
		{
			return Memory::func<const char* (int)>(0x517E70)(index);
		}

		static short SL_AllocString(const std::string& string)
		{
			return Memory::func<short(const char*, std::uint32_t, std::size_t)>(0x518290)(
				string.data(), 1, string.size() + 1);
		}
	}
}
