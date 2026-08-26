#include "stdafx.hpp"
#include "../Include.hpp"

#include "GlassWorld.hpp"

namespace ZoneTool::IW5
{
	namespace IW7Converter
	{
		IW7::GlassWorld* GenerateIW7GlassWorld(GlassWorld* asset, allocator& mem)
		{
			// allocate IW7 GlassWorld structure
			const auto IW7_asset = mem.allocate<IW7::GlassWorld>();

			IW7_asset->name = asset->name;
			IW7_asset->g_glassData = mem.allocate<IW7::G_GlassData>();
			IW7_asset->g_glassData->pieceCount = asset->g_glassData->pieceCount;
			IW7_asset->g_glassData->glassPieces = mem.allocate<IW7::G_GlassPiece>(IW7_asset->g_glassData->pieceCount);
			for (unsigned int i = 0; i < IW7_asset->g_glassData->pieceCount; i++)
			{
				IW7_asset->g_glassData->glassPieces[i].damageTaken = asset->g_glassData->glassPieces[i].damageTaken;
				IW7_asset->g_glassData->glassPieces[i].collapseTime = asset->g_glassData->glassPieces[i].collapseTime;
				IW7_asset->g_glassData->glassPieces[i].lastStateChangeTime = asset->g_glassData->glassPieces[i].lastStateChangeTime;
				IW7_asset->g_glassData->glassPieces[i].impactDir = asset->g_glassData->glassPieces[i].impactDir;
				IW7_asset->g_glassData->glassPieces[i].impactPos[0] = asset->g_glassData->glassPieces[i].impactPos[0];
				IW7_asset->g_glassData->glassPieces[i].impactPos[1] = asset->g_glassData->glassPieces[i].impactPos[1];
			}
			IW7_asset->g_glassData->damageToWeaken = asset->g_glassData->damageToWeaken;
			IW7_asset->g_glassData->damageToDestroy = asset->g_glassData->damageToDestroy;
			IW7_asset->g_glassData->glassNameCount = asset->g_glassData->glassNameCount;
			IW7_asset->g_glassData->glassNames = mem.allocate<IW7::G_GlassName>(IW7_asset->g_glassData->glassNameCount);
			for (unsigned int i = 0; i < IW7_asset->g_glassData->glassNameCount; i++)
			{
				IW7_asset->g_glassData->glassNames[i].nameStr = asset->g_glassData->glassNames[i].nameStr;
				IW7_asset->g_glassData->glassNames[i].name = static_cast<IW7::scr_string_t>(asset->g_glassData->glassNames[i].name);
				IW7_asset->g_glassData->glassNames[i].pieceCount = asset->g_glassData->glassNames[i].pieceCount;
				IW7_asset->g_glassData->glassNames[i].pieceIndices = reinterpret_cast<unsigned __int16* __ptr64>(asset->g_glassData->glassNames[i].pieceIndices);
			}

			return IW7_asset;
		}

		IW7::GlassWorld* convert(GlassWorld* asset, allocator& allocator)
		{
			// generate IW7 glassworld
			return GenerateIW7GlassWorld(asset, allocator);
		}
	}
}