#include "stdafx.hpp"

namespace ZoneTool
{
	namespace T6
	{
		void IRawFile::dump(RawFile* asset)
		{
			auto fp = FileSystem::FileOpen(asset->name, "wb");
			if (fp)
			{
				if (asset->len > 0)
				{
					fwrite(asset->buffer, asset->len, 1, fp);
				}
			}
			FileSystem::FileClose(fp);
		}
	}
}
