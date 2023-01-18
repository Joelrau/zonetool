#include "stdafx.hpp"
#include "H1/Assets/FxWorld.hpp"
#include "IW5/Structs.hpp"

#include "IW5/Assets/FxWorld.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		void IFxWorld::dump(FxWorld* asset, ZoneMemory* mem)
		{
			IW5::IFxWorld::dump(reinterpret_cast<IW5::FxWorld*>(asset), mem);
		}
	}
}