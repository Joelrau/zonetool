#include "stdafx.hpp"

#include "RawFile.hpp"

#include <H1\Assets\RawFile.hpp>

namespace ZoneTool::S1
{
	void IRawFile::dump(RawFile* asset)
	{
		H1::IRawFile::dump(reinterpret_cast<H1::RawFile*>(asset));
	}
}
