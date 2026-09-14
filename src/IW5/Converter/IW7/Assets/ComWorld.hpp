#pragma once

namespace ZoneTool::IW5
{
	namespace IW7Converter
	{
		extern IW7::ComWorld* converter_com_world;
		IW7::ComWorld* convert(ComWorld* asset, allocator& allocator);
	}
}