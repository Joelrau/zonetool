#pragma once

#include <H1\Structs.hpp>

namespace ZoneTool::S1
{
	class IMapEnts
	{
	private:
		static void dump_clientTriggers(const std::string& name, ClientTriggers* clientTrigger);

	public:
		static void dump(MapEnts* asset, const std::function<const char* (std::uint16_t)>& SL_ConvertToString);
	};
}
