#include "stdafx.hpp"

#include <zlib.h>

#include <H1\Assets\RawFile.hpp>
#include <H1\Structs.hpp>

namespace ZoneTool::S1
{
	void IRawFile::dump(RawFile* asset)
	{
		H1::IRawFile::dump(reinterpret_cast<H1::RawFile*>(asset));
	}
}
