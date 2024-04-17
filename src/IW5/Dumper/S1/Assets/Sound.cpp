#include "stdafx.hpp"

#include "Sound.hpp"

#include <IW5/Dumper/H1/Assets/Sound.hpp>

namespace ZoneTool::IW5::S1Dumper
{
	void dump(snd_alias_list_t* asset, const std::function<std::string(const char* filename)>& get_streamed_sound_data)
	{
		// 1:1 to H1
		ZoneTool::IW5::H1Dumper::dump(asset, get_streamed_sound_data);
	}
}
