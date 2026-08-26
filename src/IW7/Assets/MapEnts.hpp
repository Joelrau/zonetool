#pragma once

namespace ZoneTool::IW7
{
	class IMapEnts
	{
	private:
		static void dump_spawn_list(const std::string& name, SpawnPointRecordList* spawnList);
		static void dump_spawners(const std::string& name, SpawnerList* spawners);
		static void dump_entity_strings(const std::string& name, char* entityString, int numEntityChars);

	public:
		static void dump(MapEnts* asset);
	};
}