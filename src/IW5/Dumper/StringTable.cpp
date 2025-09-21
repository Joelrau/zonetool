#include "stdafx.hpp"

#include "StringTable.hpp"

#include <regex>

namespace ZoneTool::IW5::Dumper
{
	void dump(StringTable* asset)
	{
		auto file = filesystem::file(asset->name);
		file.open("wb");

		if (!file.get_fp())
		{
			file.close();
			return;
		}

		for (auto row = 0; row < asset->rowCount; row++)
		{
			for (auto column = 0; column < asset->columnCount; column++)
			{
				const auto index = (row * asset->columnCount) + column;
				const auto string_value = asset->values[index].string;
				const auto last_char = (column == asset->columnCount - 1) ? "\n" : ",";

				if (string_value == nullptr)
				{
					std::fprintf(file.get_fp(), last_char);
				}
				else
				{
					std::string str = string_value;
					auto added_quotes = false;
					if (str.contains(','))
					{
						added_quotes = true;
						str.insert(str.begin(), '"');
						str.insert(str.end(), '"');
					}

					if (str.contains('\"') && !added_quotes)
					{
						str = std::regex_replace(str, std::regex("\""), "\\\"");

						str.insert(str.begin(), '"');
						str.insert(str.end(), '"');
					}

					str = std::regex_replace(str, std::regex("\n"), "\\n");
					std::fprintf(file.get_fp(), "%s%s", str.data(), last_char);
				}
			}
		}

		file.close();
	}
}