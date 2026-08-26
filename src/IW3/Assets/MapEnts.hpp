#pragma once

namespace ZoneTool
{
	namespace IW3
	{
		class IMapEnts
		{
		public:
			static IW4::MapEnts* GenerateIW4MapEnts(MapEnts* asset, allocator& mem);
			static void dump(MapEnts* asset);
		};
	}
}