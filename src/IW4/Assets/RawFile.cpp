#include "stdafx.hpp"
#include "H1/Assets/RawFile.hpp"

namespace ZoneTool
{
	namespace IW5
	{
		H1::RawFile* GenerateH1RawFile(RawFile* asset, ZoneMemory* mem)
		{
			auto* h1_asset = mem->Alloc<H1::RawFile>();
			h1_asset->name = asset->name;
			h1_asset->compressedLen = asset->compressedLen;
			h1_asset->len = asset->len;
			h1_asset->buffer = asset->buffer;
			return h1_asset;
		}

		void IRawFile::dump(RawFile* asset, ZoneMemory* mem)
		{
			// generate h1 rawfile
			auto* h1_asset = GenerateH1RawFile(asset, mem);

			// dump rawfile
			H1::IRawFile::dump(h1_asset);
		}
	}
}