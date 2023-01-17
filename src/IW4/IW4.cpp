// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		bool is_dumping_complete = false;
		bool is_dumping = false;

		const char* Linker::version()
		{
			return "IW4";
		}

		bool Linker::is_used()
		{
			return !strncmp(reinterpret_cast<char*>(0x71B85C), this->version(), 3);
		}

		void Linker::startup()
		{
			// AssetHandler::SetDump(true);

			if (this->is_used())
			{
				// force compiling gsc
				Memory(0x427DB4).set<std::uint8_t>(0xEB);
				Memory(0x427D22).set<std::uint8_t>(0xEB);
				
				// 
				Memory(0x427DED).nop(6);
				
				// do nothing with online sessions
				Memory(0x441650).set<std::uint8_t>(0xC3);

				// Kill "missing asset" errors from the game to prevent confusion
				Memory(0x5BB380).set<std::uint8_t>(0xC3);

				// Tool init func
				Memory(0x4AA88B).call(printf);

				// r_loadForRenderer
				Memory(0x519DDF).set<BYTE>(0x0);

				// dirty disk breakpoint
				// Memory(0x4CF7F0).Set<BYTE>(0xCC);

				// delay loading of images, disable it
				Memory(0x51F450).set<BYTE>(0xC3);

				// don't remove the 'texture data' pointer from GfxImage	
				Memory(0x51F4FA).nop(6);

				// needed for the above to make Image_Release not misinterpret the texture data as a D3D texture
				Memory(0x51F03D).set<BYTE>(0xEB);

				// don't zero out pixel shaders
				Memory(0x505AFB).nop(7);

				// don't zero out vertex shaders
				Memory(0x505BDB).nop(7);

				// don't memset vertex declarations (not needed?)
				Memory(0x00431B91).nop(5);

				// allow loading of IWffu (unsigned) files
				Memory(0x4158D9).set<BYTE>(0xEB); //main function
				Memory(0x4A1D97).nop(2); //DB_AuthLoad_InflateInit

				// basic checks (hash jumps, both normal and playlist)
				Memory(0x5B97A3).nop(2);
				Memory(0x5BA493).nop(2);

				Memory(0x5B991C).nop(2);
				Memory(0x5BA60C).nop(2);

				Memory(0x5B97B4).nop(2);
				Memory(0x5BA4A4).nop(2);

				// some other, unknown, check
				Memory(0x5B9912).set<BYTE>(0xB8);
				Memory(0x5B9913).set<DWORD>(1);

				Memory(0x5BA602).set<BYTE>(0xB8);
				Memory(0x5BA603).set<DWORD>(1);

				// something related to image loading
				Memory(0x54ADB0).set<BYTE>(0xC3);

				// dvar setting function, unknown stuff related to server thread sync
				Memory(0x647781).set<BYTE>(0xEB);

				// fs_basegame
				Memory(0x6431D1).set("zonetool");

				// hunk size (was 300 MiB)
				Memory(0x64A029).set<DWORD>(0x3F000000);
				Memory(0x64A057).set<DWORD>(0x3F000000); // 0x1C200000

				// allow loading of IWffu (unsigned) files
				Memory(0x4158D9).set<BYTE>(0xEB); // main function
				Memory(0x4A1D97).nop(2); // DB_AuthLoad_InflateInit

				// basic checks (hash jumps, both normal and playlist)
				Memory(0x5B97A3).nop(2);
				Memory(0x5BA493).nop(2);

				Memory(0x5B991C).nop(2);
				Memory(0x5BA60C).nop(2);

				Memory(0x5B97B4).nop(2);
				Memory(0x5BA4A4).nop(2);

				// Disabling loadedsound touching
				Memory(0x492EFC).nop(5);

				// weaponfile patches
				Memory(0x408228).nop(5); // find asset header
				Memory(0x408230).nop(5); // is asset default
				Memory(0x40823A).nop(2); // jump

				// menu stuff
				// disable the 2 new tokens in ItemParse_rect
				Memory(0x640693).set<BYTE>(0xEB);

				// Dont load ASSET_TYPE_MENU anymore, we dont need it.
				Memory(0x453406).nop(5);
			}
		}

		std::shared_ptr<IZone> Linker::alloc_zone(const std::string& zone)
		{
			ZONETOOL_ERROR("AllocZone called but IW4 is not intended to compile zones!");
			return nullptr;
		}

		std::shared_ptr<ZoneBuffer> Linker::alloc_buffer()
		{
			ZONETOOL_ERROR("AllocBuffer called but IW4 is not intended to compile zones!");
			return nullptr;
		}

		void Linker::load_zone(const std::string& name)
		{
			ZONETOOL_INFO("Loading zone \"%s\"...", name.data());

			XZoneInfo zone = {name.data(), 20, 0};
			DB_LoadXAssets(&zone, 1, 0);

			ZONETOOL_INFO("Zone \"%s\" loaded.", name.data());
		}

		void Linker::unload_zones()
		{
			ZONETOOL_INFO("Unloading zones...");

			static XZoneInfo zone = {0, 0, 70};
			DB_LoadXAssets(&zone, 1, 1);
		}

		bool Linker::is_valid_asset_type(const std::string& type)
		{
			return this->type_to_int(type) >= 0;
		}

		std::int32_t Linker::type_to_int(std::string type)
		{
			auto xassettypes = reinterpret_cast<char**>(0x799278);

			for (std::int32_t i = 0; i < max; i++)
			{
				if (xassettypes[i] == type)
					return i;
			}

			return -1;
		}

		std::string Linker::type_to_string(std::int32_t type)
		{
			auto xassettypes = reinterpret_cast<char**>(0x799278);
			return xassettypes[type];
		}

        bool Linker::supports_building()
        {
            return false;
        }

		bool Linker::supports_version(const zone_target_version version)
		{
			return false;
		}

        void Linker::dump_zone(const std::string& name)
		{
			is_dumping_complete = false;
			is_dumping = true;

			FileSystem::SetFastFile(name);
			AssetHandler::SetDump(true);
			load_zone(name);

			while (!isDumpingComplete)
			{
				Sleep(1);
			}
		}

		void Linker::verify_zone(const std::string& name)
		{
			AssetHandler::SetVerify(true);
			load_zone(name);
			AssetHandler::SetVerify(false);
		}

		Linker::Linker()
		{
		}

		Linker::~Linker()
		{
		}
	}
}
