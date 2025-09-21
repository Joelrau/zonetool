#include "stdafx.hpp"
#include "IW4/Assets/StringTable.hpp"

namespace ZoneTool
{
	namespace IW3
	{
		IW4::StringTable* convert(StringTable* asset, allocator& mem)
		{
			static const auto string_table_hash = [](const std::string& string) -> int
			{
				int hash = 0;
				const char* data = string.data();

				while (*data != 0)
				{
					hash = tolower(*data) + (31 * hash);
					data++;
				}

				return hash;
			};

			auto* iw4_asset = mem.allocate<IW4::StringTable>();
			iw4_asset->name = asset->name;
			iw4_asset->columns = asset->columnCount;
			iw4_asset->rows = asset->rowCount;
			iw4_asset->strings = mem.allocate<IW4::StringTableCell>(asset->rowCount * asset->columnCount);
			auto rows = asset->values;
			for (int row = 0; row < asset->rowCount; row++)
			{
				for (int col = 0; col < asset->columnCount; col++)
				{
					int entry = (row * asset->columnCount) + col;
					iw4_asset->strings[entry].string = const_cast<char*>(rows[entry]);
					iw4_asset->strings[entry].hash = string_table_hash(iw4_asset->strings[entry].string);
				}
			}
			return iw4_asset;
		}

		void IStringTable::dump(StringTable* asset)
		{
			allocator allocator;
			auto* iw4_asset = convert(asset, allocator);

			// dump stringtable
			IW4::IStringTable::dump(iw4_asset);
		}
	}
}