#include "stdafx.hpp"

namespace ZoneTool
{
	namespace T6
	{
		extern bool is_dumping;
		
		extern std::string lastfastfile = "none";

		XAssetEntry AssetHandler::XAssetEntries[AssetEntries];
		std::uint32_t AssetHandler::db_hashmap[AssetEntries];

		bool AssetHandler::verify = false;
		bool AssetHandler::dump = false;

		//Updated for T6
		void** AssetHandler::DB_XAssetPool = (void**)SELECT(0xD4B390,0xD40230);
		unsigned int* AssetHandler::g_poolSize = (unsigned int*)SELECT(0xD4AFD0,0xD3FE70);
		AssetHandler::DB_GetXAssetSizeHandler_t* AssetHandler::DB_GetXAssetSizeHandlers = (DB_GetXAssetSizeHandler_t*)SELECT(0xD4ABD0,0xD3FA70);
		DB_LoadData* AssetHandler::g_load = (DB_LoadData*)SELECT(0x12C6F80,0x129BE00);
		std::unordered_map<std::string, XAsset> AssetHandler::StoredAssets;

		static Function<const char* (XAsset*)> DB_GetXAssetName = SELECT(0xD4A9F0,0x534520);

		int AssetHandler::current_loading_asset_num;
		int AssetHandler::max_loading_asset_num;
		XAssetListT6** AssetHandler::varXAssetList = (XAssetListT6**)SELECT(0x12E7804,0x12BC684);

		//static Function < XAssetHeader(XAssetType, const char*, bool, int)> DB_FindXAssetHeader = 0x43F460;

		XAssetHeader AssetHandler::FindXAsset(std::int32_t type, const char* name, std::uint32_t unk)
		{
			if (StoredAssets.find(name) != StoredAssets.end())
			{
				if (StoredAssets[name].type == type)
				{
					return StoredAssets[name].header;
				}
			}

			return XAssetHeader(); // DB_FindXAssetHeader(type, name, flags);
		}

		void AssetHandler::AddXAsset(std::int32_t type, std::string name, void* pointer)
		{
			if (StoredAssets.find(name) == StoredAssets.end())
			{
				//StoredAssets[name] = {static_cast<XAssetType>(type), reinterpret_cast<RawFile*>(pointer)};
			}
		}

		void AssetHandler::SetVerify(bool state)
		{
			verify = state;
		}

		void AssetHandler::SetDump(bool state)
		{
			dump = state;
		}

		const char* GetAssetName(XAssetEntry* assetEntry)
		{
			auto xAsset = &assetEntry->asset;
			return DB_GetXAssetName(xAsset);
		}

		static std::vector<std::pair<std::int32_t, std::string>> CommonAssets;

		void AssetHandler::DB_LogLoadedAsset(XAssetEntry* assetEntry, std::int32_t type, bool isreferencedasset)
		{
			std::string fastfile = g_load->filename;

			FileSystem::SetFastFile(fastfile);
			zonetool::filesystem::set_fastfile(fastfile);

			static FILE* csvFile = nullptr;

			if (!csvFile)
			{
				csvFile = FileSystem::FileOpen(fastfile + ".csv", "wb");
			}

			std::string assetName = GetAssetName(assetEntry);

			auto xassettypes = reinterpret_cast<char**>SELECT(0xD4A8F0, 0xD3F790);

			// dump assets to disk
			if (csvFile)
			{
				fprintf(csvFile, "%s,%s\n", xassettypes[type], assetName.data());
				//printf("%s,%s\n", xassettypes[type], assetName.data());
			}

			if (type == rawfile && assetName == fastfile)
			{
				ZONETOOL_INFO("Zone \"%s\" dumped.", &fastfile[0]);

				FileSystem::FileClose(csvFile);
				csvFile = nullptr;

				is_dumping_complete = true;
			}

			if (!assetName.empty() && assetName[0] != ',')
			{
				auto assetHeader = assetEntry->asset.header;

#define DUMPCASE(__type__,__interface__,__struct__) \
				if (type == __type__) \
				{ \
					printf("Dumping assset %s, of type %s\n", assetName.data(), xassettypes[type]); \
					auto asset = assetHeader.__struct__; \
					__interface__::dump(asset); \
				} \

				DUMPCASE(stringtable, IStringTable, XAssetHeader::stringTable);
				DUMPCASE(rawfile, IRawFile, XAssetHeader::rawfile);
			}
		}

		bool AssetHandler::IsCommonAsset(std::int32_t type, const std::string& name)
		{
			for (auto& commonAsset : CommonAssets)
			{
				if (commonAsset.first == type && commonAsset.second == name)
				{
					return true;
				}
			}

			return false;
		}

		const char* AssetHandler::DB_AddXAsset(XAssetEntry* newEntry)
		{
			try
			{
				AssetHandler::DB_LogLoadedAsset(newEntry, newEntry->asset.type, false);
			}
			catch (const std::exception& e)
			{
				printf("error: %s\n", e.what());
			}

			return Function<const char* (XAssetEntry*)>SELECT(0xD4A9F0, 0x534520)(newEntry); //DB_GetXAssetName
		}

		void* AssetHandler::ReallocateAssetPool(uint32_t type, unsigned int newSize)
		{
			int elSize = DB_GetXAssetSizeHandlers[type]();

			void* poolEntry = malloc(newSize * elSize);
			DB_XAssetPool[type] = poolEntry;
			g_poolSize[type] = newSize;

			return poolEntry;
		}

		void* AssetHandler::ReallocateAssetPoolM(uint32_t type, int multiplier)
		{
			int elSize = DB_GetXAssetSizeHandlers[type]();
			int newSize = multiplier * g_poolSize[type];

			void* poolEntry = malloc(newSize * elSize);
			DB_XAssetPool[type] = poolEntry;
			g_poolSize[type] = newSize;

			return poolEntry;
		}

		AssetHandler::AssetHandler()
		{
			Memory(SELECT(0x7FD8CA, 0x7FCCCA)).call(DB_AddXAsset);
		}

		AssetHandler::~AssetHandler()
		{
		}
	}
}
