#pragma once

namespace ZoneTool::S1
{
	class ISound
	{
	private:
		static void json_dump_snd_alias(ordered_json& sound, snd_alias_t* asset);
		static void json_dump(snd_alias_list_t* asset);

	public:
		static void dump(snd_alias_list_t* asset);
	};
}
