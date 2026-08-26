#pragma once

namespace ZoneTool
{
	namespace IW4
	{
		class IMapEnts
		{
		public:
			static IW5::MapEnts* GenerateIW5MapEnts(MapEnts* asset, allocator& mem);
			static void dump(MapEnts* asset);
		};
	}
}