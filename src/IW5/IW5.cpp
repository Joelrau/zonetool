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
	namespace IW5
	{
		bool is_dumping_complete = false;
		bool is_dumping = false;

		const char* Linker::version()
		{
			return "IW5";
		}

		bool Linker::is_used()
		{
			return !strncmp(reinterpret_cast<char*>(0x770C6C), this->version(), 3);
		}

		__declspec(naked) void Linker::MessageLoop()
		{
			while (true)
			{
			}
		}

		FS_Read_t FS_Read;

		void fs_file_close(int handle)
		{
			static DWORD func = 0x524E10;

			__asm
			{
				mov eax, handle
				call func
			}
		}

		int fs_fopen_file_read_for_thread(const char* filename, int* file, int thread)
		{
			static DWORD func = 0x525330;
			int result{};

			__asm
			{
				pushad

				mov edx, filename
				push thread
				push file
				call func
				add esp, 0x8
				mov result, eax

				popad
			}

			return result;
		}

		int FS_FOpenFileReadForThread(const char* filename, int* file, int thread)
		{
			return fs_fopen_file_read_for_thread(filename, file, thread);
		}

		void FS_FileClose(int h)
		{
			fs_file_close(h);
		}

		std::string filesystem_read_big_file(const char* filename)
		{
			std::string file_buffer{};

			int handle = -1;
			FS_FOpenFileReadForThread(filename, &handle, 0);

			if (handle > 0)
			{
				constexpr unsigned int BUFF_SIZE = 1024;

				while (true)
				{
					char buffer[BUFF_SIZE];
					auto size_read = FS_Read(buffer, BUFF_SIZE, handle);

					file_buffer.append(buffer, size_read);

					if (size_read < BUFF_SIZE)
					{
						// We're done!
						break;
					}
				}

				// shi
				FS_FileClose(handle);
			}

			return file_buffer;
		}

		char* Sys_GetMapZoneDir_stub(int a1, const char* filename)
		{
			char dest[64]{};
			strncpy(dest, filename, 0x3F);
			dest[63] = 0;

			auto game_va = reinterpret_cast<char* (*)(const char*, const char*)>(0x535580);
			auto lang = *reinterpret_cast<char**>(0x594242C);

			if (filesystem::file(va("zone\\%s\\%s.ff", lang, filename)).exists())
				return game_va("zone\\%s\\", lang);
			return game_va("%s", "zone\\dlc\\");
		}

		void Linker::startup()
		{
			if (this->is_used())
			{
				set_linker_mode(linker_mode::iw5);

				// AssetHandler::SetDump(true);

				FS_Read = FS_Read_t(0x525940);

				// Kill original console window
				Memory(0x53CB60).set<std::uint8_t>(0xC3);
				Memory(0x53C850).set<std::uint8_t>(0xC3);

				// Message loop
				Memory(0x53A2E0).jump(MessageLoop);

				// Load patches
				RegisterPatch<FFCompression>();
				RegisterPatch<AssetHandler>();

				// Disable DW
				Memory(0x53821C).nop(5);

				// Do not read server config, we don't need it anyways.
				Memory(0x4DAEFE).nop(5);

				// Do not interact with the steamapi functions.
				Memory(0x599EAE).nop(5);

				// Sys_Error -> printf
				Memory(0x539A50).jump(printf);

				// Patch hunk size (Allows us to load more assets)
				Memory(0x5340D9).set<DWORD>(0x3F000000);
				Memory(0x534110).set<DWORD>(0x3F000000);

				// Replace the default attachment name (the specified default does not exist)
				Memory(0x74B0BC).write_string("reflex");

				// Fix some dlc zones not loading
				Memory(0x44FD9A).call(Sys_GetMapZoneDir_stub);
			}
		}

		std::shared_ptr<IZone> Linker::alloc_zone(const std::string& zone)
		{
			ZONETOOL_ERROR("AllocZone called but IW5 is not intended to compile zones!");
			return nullptr;
		}

		std::shared_ptr<ZoneBuffer> Linker::alloc_buffer()
		{
			ZONETOOL_ERROR("AllocBuffer called but IW5 is not intended to compile zones!");
			return nullptr;
		}

		void Linker::load_zone(const std::string& name)
		{
			ZONETOOL_INFO("Loading zone \"%s\"...", name.data());

			XZoneInfo zone = {name.data(), 20, 0};
			DB_LoadXAssets(&zone, 1, 0);

			auto timeout = 3000u;
			ZONETOOL_INFO("Waiting %u sec for zone \"%s\" to be loaded.", timeout / 1000, name.data());
			Sleep(timeout);
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
			auto xassettypes = reinterpret_cast<char**>(0x7C6208);

			for (std::int32_t i = 0; i < ASSET_TYPE_COUNT; i++)
			{
				if (xassettypes[i] == type)
					return i;
			}

			return -1;
		}

		std::string Linker::type_to_string(std::int32_t type)
		{
			auto xassettypes = reinterpret_cast<char**>(0x7C6208);
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

        void Linker::dump_zone(const std::string& name, zonetool::dump_target target)
		{
			zonetool::dumping_target = target;
			zonetool::dumping_source = zonetool::iw5;

			is_dumping_complete = false;
			is_dumping = true;

			FileSystem::SetFastFile(name);
			AssetHandler::SetDump(true);
			load_zone(name);

			while (!is_dumping_complete)
			{
				// TODO: find
				//if (!*reinterpret_cast<bool*>(0x1294A00))
				//{
				//	AssetHandler::StopDump();
				//}
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
