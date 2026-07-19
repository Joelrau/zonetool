#include "stdafx.hpp"
#include "IW5/Assets/PhysCollmap.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		void IPhysCollmap::dump(PhysCollmap* asset)
		{
			// dump asset
			IW5::IPhysCollmap::dump(reinterpret_cast<IW5::PhysCollmap*>(asset));
		}
	}
}