#include "stdafx.hpp"
#include "H1/Assets/GlassWorld.hpp"

namespace ZoneTool
{
	namespace IW5
	{
		H1::GlassWorld* GenerateH1GlassWorld(GlassWorld* asset, ZoneMemory* mem)
		{
			// allocate H1 GlassWorld structure
			const auto h1_asset = mem->Alloc<H1::GlassWorld>();

			h1_asset->name = asset->name;
			h1_asset->g_glassData = mem->Alloc<H1::G_GlassData>();
			h1_asset->g_glassData->pieceCount = asset->g_glassData->pieceCount;
			h1_asset->g_glassData->glassPieces = mem->Alloc<H1::G_GlassPiece>(h1_asset->g_glassData->pieceCount);
			for (unsigned int i = 0; i < h1_asset->g_glassData->pieceCount; i++)
			{
				h1_asset->g_glassData->glassPieces[i].damageTaken = asset->g_glassData->glassPieces[i].damageTaken;
				h1_asset->g_glassData->glassPieces[i].collapseTime = asset->g_glassData->glassPieces[i].collapseTime;
				h1_asset->g_glassData->glassPieces[i].lastStateChangeTime = asset->g_glassData->glassPieces[i].lastStateChangeTime;
				h1_asset->g_glassData->glassPieces[i].impactDir = asset->g_glassData->glassPieces[i].impactDir;
				h1_asset->g_glassData->glassPieces[i].impactPos[0] = asset->g_glassData->glassPieces[i].impactPos[0];
				h1_asset->g_glassData->glassPieces[i].impactPos[1] = asset->g_glassData->glassPieces[i].impactPos[1];
			}
			h1_asset->g_glassData->damageToWeaken = asset->g_glassData->damageToWeaken;
			h1_asset->g_glassData->damageToDestroy = asset->g_glassData->damageToDestroy;
			h1_asset->g_glassData->glassNameCount = asset->g_glassData->glassNameCount;
			h1_asset->g_glassData->glassNames = mem->Alloc<H1::G_GlassName>(h1_asset->g_glassData->glassNameCount);
			for (unsigned int i = 0; i < h1_asset->g_glassData->glassNameCount; i++)
			{
				h1_asset->g_glassData->glassNames[i].nameStr = asset->g_glassData->glassNames[i].nameStr;
				h1_asset->g_glassData->glassNames[i].name = static_cast<H1::scr_string_t>(asset->g_glassData->glassNames[i].name);
				h1_asset->g_glassData->glassNames[i].pieceCount = asset->g_glassData->glassNames[i].pieceCount;
				h1_asset->g_glassData->glassNames[i].pieceIndices = reinterpret_cast<unsigned __int16* __ptr64>(asset->g_glassData->glassNames[i].pieceIndices);
			}
			memcpy(&h1_asset->g_glassData->pad, asset->g_glassData->pad, sizeof(asset->g_glassData->pad));

			return h1_asset;
		}

		void IGlassWorld::dump(GlassWorld* asset, ZoneMemory* mem)
		{
			// generate h1 glassworld
			auto h1_asset = GenerateH1GlassWorld(asset, mem);

			// dump h1 glassworld
			H1::IGlassWorld::dump(h1_asset);
		}
	}
}