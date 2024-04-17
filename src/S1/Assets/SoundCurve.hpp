#pragma once

#include <H1\Structs.hpp>

namespace ZoneTool::S1
{
	class ISoundCurve
	{
	public:
		static void dump(SndCurve* asset, const std::string& type);
		static void dump(SndCurve* asset);
	};
}