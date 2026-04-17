#pragma once

namespace ZoneTool::H1
{
	class IAddonMapEnts
	{
	public:
		static void dump_triggers(const std::string& name, MapTriggers* trigger);
		static void dump_entityStrings(const std::string& name, char* entityString, int numEntityChars);

		static void dump(AddonMapEnts* asset);
	};
}