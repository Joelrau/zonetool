#pragma once

namespace ZoneTool
{
	namespace T6
	{
		struct XAsset
		{
			XAssetType type;
			XAssetHeader header;
		};

		struct XAssetEntry
		{
			XAsset asset;
			unsigned int nextHash;
			unsigned int nextOverride;
			unsigned int nextPoolEntry;
		};

		union XAssetEntryPoolEntry
		{
			XAssetEntry entry;
			XAssetEntryPoolEntry* next;
		};

		struct ScriptStringList
		{
			int count;
			const char** strings;
		};

		struct XAssetListT6
		{
			ScriptStringList stringList;
			int dependCount;
			const char** depends;
			int assetCount;
			XAsset* assets;
		};


		extern bool is_dumping_complete;
		static Json localize_entries_buffer;

		class AssetHandler : public IPatch
		{
		private:
			static constexpr std::uint32_t AssetEntries = 86000;

			static XAssetEntry XAssetEntries[AssetEntries];
			static std::uint32_t db_hashmap[AssetEntries];

			static std::unordered_map<std::string, XAsset> StoredAssets;

			static std::unordered_map<std::int32_t, std::function<void*(void*)>> DuplicateAssetHandlers;

			static bool verify;
			static bool dump;

			typedef int (__cdecl * DB_GetXAssetSizeHandler_t)();
			static DB_GetXAssetSizeHandler_t* DB_GetXAssetSizeHandlers;

			static void** DB_XAssetPool;
			static unsigned int* g_poolSize;
			static DB_LoadData* g_load;
			static XAssetListT6** varXAssetList;
			static int current_loading_asset_num;
			static int max_loading_asset_num;
			static void StoreCurrentLoadingAssetNum();

			static void DB_LogLoadedAsset(XAssetEntry* assetEntry, std::int32_t type, bool isreferencedasset);
			static const char* DB_AddXAsset(XAssetEntry* newEntry);

			static void* ReallocateAssetPool(uint32_t type, unsigned int newSize);
			static void* ReallocateAssetPoolM(uint32_t type, int multiplier);

			static void FixupAssetLoading();

			static void* DuplicatePixelShader(void* ptr);
			static void* DuplicateVertexDecl(void* ptr);
			static void* DuplicateVertexShader(void* ptr);
			static void* DuplicateTechset(void* ptr);

		public:

			static bool IsCommonAsset(std::int32_t type, const std::string& name);

			static XAssetHeader FindXAsset(std::int32_t type, const char* name, std::uint32_t unk);
			static void AddXAsset(std::int32_t type, std::string name, void* pointer);
			static void SetVerify(bool state);
			static void SetDump(bool state);

			AssetHandler();
			~AssetHandler();
		};
	}
}
