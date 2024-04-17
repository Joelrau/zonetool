#include "stdafx.hpp"

#include "Dumper/H1/Assets/Sound.hpp"
#include "Dumper/IW6/Assets/Sound.hpp"
#include "Dumper/S1/Assets/Sound.hpp"

namespace ZoneTool::IW5
{
	void ISound::dump(snd_alias_list_t* asset, const std::function<std::string(const char* filename)>& get_streamed_sound_data)
	{
		if (zonetool::dumping_target == zonetool::dump_target::h1)
		{
			return H1Dumper::dump(asset, get_streamed_sound_data);
		}
		else if (zonetool::dumping_target == zonetool::dump_target::iw6)
		{
			return IW6Dumper::dump(asset, get_streamed_sound_data);
		}
		else if (zonetool::dumping_target == zonetool::dump_target::s1)
		{
			return S1Dumper::dump(asset, get_streamed_sound_data);
		}
	}
}