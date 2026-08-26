#include "stdafx.hpp"
#include "IW5/Assets/MapEnts.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		IW5::MapEnts* IMapEnts::GenerateIW5MapEnts(MapEnts* asset, allocator& mem)
		{
			auto* new_asset = mem.allocate<IW5::MapEnts>();
			new_asset->name = asset->name;
			new_asset->entityString = asset->entityString;
			new_asset->numEntityChars = asset->numEntityChars;
			memcpy(&new_asset->trigger, &asset->trigger, sizeof(MapTriggers)); 
			static_assert(sizeof(IW4::MapTriggers) == sizeof(IW5::MapTriggers));
			memset(&new_asset->clientTrigger, 0, sizeof(IW5::ClientTriggers));

			return new_asset;
		}

		void IMapEnts::dump(MapEnts* asset)
		{
			allocator allocator;
			auto new_asset = GenerateIW5MapEnts(asset, allocator);

			IW5::IMapEnts::dump(new_asset);
		}
	}
}
