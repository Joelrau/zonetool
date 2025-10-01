#include "stdafx.hpp"

#include "RawFile.hpp"

#include <zlib.h>

namespace ZoneTool::IW5::Dumper
{
	void dump(RawFile* asset)
	{
		std::string name = asset->name == "*"s ? "RadiantFields.bin" : asset->name;
		auto file = filesystem::file(name);
		file.open("wb");

		if (asset->compressedLen > 0)
		{
			std::vector<std::uint8_t> uncompressed;
			uncompressed.resize(asset->len);

			uncompress(uncompressed.data(), (uLongf*)&asset->len, (Bytef*)asset->buffer, asset->compressedLen);

			file.write(uncompressed.data(), uncompressed.size(), 1);
		}
		else if (asset->len > 0)
		{
			file.write(asset->buffer, asset->len, 1);
		}

		file.close();
	}
}