#pragma once

namespace ZoneTool::IW6
{
	class IAddonMapEnts
	{
	private:
		static void dump_triggers(const std::string& name, MapTriggers* trigger);
		static void dump_entityStrings(const std::string& name, char* entityString, int numEntityChars);

	public:
		static void dump(AddonMapEnts* asset);
	};
}