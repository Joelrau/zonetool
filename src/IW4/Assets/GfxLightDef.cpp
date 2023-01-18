#include "stdafx.hpp"
#include "H1/Assets/GfxLightDef.hpp"
#include "IW5/Structs.hpp"

#include "IW5/Assets/GfxLightDef.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		void IGfxLightDef::dump(GfxLightDef* asset, ZoneMemory* mem)
		{
			auto iw5_asset = mem->Alloc<IW5::GfxLightDef>();

			// copy data
			memcpy(iw5_asset, asset, sizeof GfxLightDef);
			memset(&iw5_asset->cucoloris, 0, sizeof GfxLightImage);
			iw5_asset->lmapLookupStart = asset->lmapLookupStart;

			// dump data
			IW5::IGfxLightDef::dump(iw5_asset, mem);
		}
	}
}