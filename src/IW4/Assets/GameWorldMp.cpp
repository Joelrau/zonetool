#include "stdafx.hpp"
#include "H1/Assets/GlassWorld.hpp"
#include "IW5/Structs.hpp"

#include "IW5/Assets/GlassWorld.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		// IW4 GameWorldMp and IW5 GlassWorld are 1:1 (GameWorldSp has data between that's padded)
		void IGameWorldMp::dump(GameWorldMp* asset, ZoneMemory* mem)
		{
			IW5::IGlassWorld::dump(reinterpret_cast<IW5::GlassWorld*>(asset), mem);
		}
	}
}