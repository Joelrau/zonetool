#include "stdafx.hpp"

#include "Sound.hpp"
#include "Converter/IW6/Assets/Sound.hpp"
#include "IW6/Assets/Sound.hpp"

namespace ZoneTool::IW5::IW6Dumper
{
	void dump(snd_alias_list_t* asset, const std::function<std::string(const char* filename)>& get_streamed_sound_data)
	{
		// generate IW6 asset
		allocator allocator;
		auto* iw6_asset = IW6Converter::convert(asset, allocator, get_streamed_sound_data);

		// dump sound
		IW6::ISound::dump(iw6_asset);
	}
}