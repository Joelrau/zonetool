#pragma once

namespace ZoneTool::IW5
{
	namespace IW7Converter
	{
		IW7::MapEnts* generate_mapents(clipMap_t* clipmap, allocator& allocator);
		IW7::clipMap_t* convert(clipMap_t* asset, allocator& allocator);
	}
}