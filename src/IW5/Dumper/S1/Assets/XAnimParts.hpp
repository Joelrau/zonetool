#pragma once

namespace ZoneTool::IW5
{
	namespace S1Dumper
	{
		void dump(XAnimParts* asset, const std::function<const char* (uint16_t)>& convertToString);
	}
}