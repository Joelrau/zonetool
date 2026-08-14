#include "stdafx.hpp"

namespace ZoneTool
{
	namespace T6
	{
		void IStringTable::dump(StringTable* asset)
		{
			std::string path = asset->name;

			auto file = FileSystem::FileOpen(path, "w"s);

			for (int row = 0; row < asset->rows; row++)
			{
				for (int column = 0; column < asset->columns; column++)
				{
					fprintf(
						file,
						"%s%s",
						(asset->strings[(row * asset->columns) + column].string)
							? asset->strings[(row * asset->columns) + column].string
							: "",
						(column == asset->columns - 1) ? "\n" : ","
					);
				}
			}

			FileSystem::FileClose(file);
		}
	}
}
