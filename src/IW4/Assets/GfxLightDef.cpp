#include "stdafx.hpp"
#include "H1/Assets/GfxLightDef.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		H1::GfxLightDef* GenerateH1GfxLightDef(GfxLightDef* asset, ZoneMemory* mem)
		{
			auto* h1_asset = mem->Alloc<H1::GfxLightDef>();
			h1_asset->name = asset->name;
			if (asset->attenuation.image)
			{
				h1_asset->attenuation.image = mem->Alloc<H1::GfxImage>();
				h1_asset->attenuation.image->name = asset->attenuation.image->name;
			}
			h1_asset->attenuation.samplerState = asset->attenuation.samplerState;
			//if (asset->cucoloris.image)
			//{
			//	h1_asset->cucoloris.image = mem->Alloc<H1::GfxImage>();
			//	h1_asset->cucoloris.image->name = "";
			//}
			h1_asset->cucoloris.samplerState = 10;
			h1_asset->lmapLookupStart = asset->lmapLookupStart;
			return h1_asset;
		}

		void IGfxLightDef::dump(GfxLightDef* asset, ZoneMemory* mem)
		{
			// generate h1 lightdef
			auto* h1_asset = GenerateH1GfxLightDef(asset, mem);

			// dump lightdef
			H1::IGfxLightDef::dump(h1_asset);
		}
	}
}