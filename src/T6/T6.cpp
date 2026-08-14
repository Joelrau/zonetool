#include "stdafx.hpp"

namespace ZoneTool
{
	namespace T6
	{
		bool is_dumping_complete = false;
		bool is_dumping = false;
		static Function<void(void*, std::uint32_t, std::uint32_t)> DB_LoadXAssets = SELECT(0x4174B0, 0x663090);
		static Function<void()> DB_ShutDownXAssets = SELECT(0x4FF8E0, 0x440BA0);
		int* g_zoneCount = (int*)0x132F2B4;
		XZone* g_zones = (XZone*)0x16D9F00;
		XAssetEntryPoolEntry* g_assetEntryPool = (XAssetEntryPoolEntry*)SELECT(0x14342E0, 0x1409058);
		XAssetEntryPoolEntry* g_freeAssetEntryHead = (XAssetEntryPoolEntry*)SELECT(0x12EAB34, 0x12BF9B4);

		struct CmdArgs
		{
			int nesting;
			int localClientNum[8];
			int controllerIndex[8];
			void* itemDef[8];
			int argshift[8];
			int argc[8];
			const char** argv[8];
			char textPool[8192];
			const char* argvPool[512];
			int usedTextPool[8];
			int totalUsedArgvPool;
			int totalUsedTextPool;
		};

		CmdArgs* get_cmd_args()
		{
			return reinterpret_cast<CmdArgs*>(reinterpret_cast<void* (*)(int)>(0x59A740)(4));
		}

		std::unordered_map<std::string, std::function<void(params&)>> handlers;

		void main_handler()
		{
			params params = {};

			const auto command = params[0];

			if (handlers.find(command) != handlers.end())
			{
				handlers[command](params);
			}
		}

		params::params()
			: nesting_(get_cmd_args()->nesting)
		{
		}

		int params::size() const
		{
			const auto cmd_args = get_cmd_args();
			return cmd_args->argc[cmd_args->nesting];
		}

		const char* params::get(int index) const
		{
			if (index >= this->size())
			{
				return "";
			}

			const auto cmd_args = get_cmd_args();
			return cmd_args->argv[this->nesting_][index];
		}

		std::string params::join(int index) const
		{
			std::string result = {};

			for (int i = index; i < this->size(); i++)
			{
				if (i > index) result.append(" ");
				result.append(this->get(i));
			}

			return result;
		}

		void add_raw(const char* name, void (*callback)())
		{
			const auto func = std::calloc(1, 24);
			reinterpret_cast<void(*)(const char*, void*, void*)>(0x4DC2A0)
				(name, callback, func);
		}

		void add(const char* command, std::function<void(params&)> callback)
		{
			if (handlers.find(command) == handlers.end())
			{
				add_raw(command, main_handler);
			}

			handlers[command] = callback;
		}

		void init()
		{
			add("dumpzone", [](const params& params)
			{
				// Check if enough arguments have been passed to the command
				if (params.size() == 1)
				{
					ZONETOOL_ERROR("usage: dumpzone <zone>");
					return;
				}
			});

			RegisterPatch<AssetHandler>();
		}
	}
}
