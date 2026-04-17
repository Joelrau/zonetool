#pragma once

namespace ZoneTool::S1
{
	class IMapEnts
	{
	private:
		static void dump_clientTriggers(const std::string& name, ClientTriggers* clientTrigger);

	public:
		static void dump(MapEnts* asset);
	};
}
