#pragma once

namespace ZoneTool::H1
{
	class IMapEnts
	{
	public:
		static void dump_splineList(const std::string& name, SplineRecordList* splineList);
		static void dump_spawnList(const std::string& name, SpawnPointRecordList* spawnList, const std::function<const char* (std::uint16_t)>& SL_ConvertToString);
		static void dump_clientBlendTriggers(const std::string& name, ClientTriggerBlend* clientTriggerBlend);
		static void dump_clientTriggers(const std::string& name, ClientTriggers* clientTrigger);
		static void dump_triggers(const std::string& name, MapTriggers* trigger);
		static void dump_entityStrings(const std::string& name, char* entityString, int numEntityChars);

		static void dump(MapEnts* asset, const std::function<const char* (std::uint16_t)>& SL_ConvertToString);
	};
}