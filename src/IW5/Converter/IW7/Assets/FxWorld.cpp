#include "stdafx.hpp"
#include "../Include.hpp"

#include "FxWorld.hpp"

namespace ZoneTool::IW5
{
	namespace IW7Converter
	{
#define COPY_SOUND(__name__) \
			if(asset->__name__) \
			{ \
				new_asset->__name__ = asset->__name__->name; \
			}

#define COPY_EFFECT(__name__) \
			new_asset->__name__.type = IW7::FX_COMBINED_VFX; \
			if (asset->__name__) \
			{ \
				new_asset->__name__.u.vfx = allocator.allocate<IW7::ParticleSystemDef>(); \
				new_asset->__name__.u.vfx->name = asset->__name__->name; \
			}

#define CREATE_EFFECT(__name__, __effect_name__) \
			new_asset->__name__.type = IW7::FX_COMBINED_VFX; \
			new_asset->__name__.u.vfx = allocator.allocate<IW7::ParticleSystemDef>(); \
			new_asset->__name__.u.vfx->name = allocator.duplicate_string(__effect_name__);

#define CREATE_SOUND(__name__, __sound_name__) \
			new_asset->__name__ = allocator.duplicate_string(__sound_name__);

		IW7::FxWorld* GenerateIW7FxWorld(FxWorld* asset, allocator& allocator)
		{
			const auto new_asset = allocator.allocate<IW7::FxWorld>();

			REINTERPRET_CAST_SAFE(name);

			COPY_VALUE(glassSys.time);
			COPY_VALUE(glassSys.prevTime);
			COPY_VALUE(glassSys.defCount);
			COPY_VALUE(glassSys.pieceLimit);
			COPY_VALUE(glassSys.pieceWordCount);
			COPY_VALUE(glassSys.cellCount);
			COPY_VALUE(glassSys.activePieceCount);
			COPY_VALUE(glassSys.firstFreePiece);
			COPY_VALUE(glassSys.geoDataLimit);
			COPY_VALUE(glassSys.geoDataCount);
			COPY_VALUE(glassSys.initGeoDataCount);
			COPY_VALUE(glassSys.needToCompactData);
			COPY_VALUE(glassSys.initCount);
			COPY_VALUE(glassSys.effectChanceAccum);
			COPY_VALUE(glassSys.lastPieceDeletionTime);
			COPY_VALUE(glassSys.initPieceCount);

			new_asset->glassSys.defs = allocator.allocate<IW7::FxGlassDef>(new_asset->glassSys.defCount);
			for (unsigned int i = 0; i < new_asset->glassSys.defCount; i++)
			{
				COPY_VALUE(glassSys.defs[i].halfThickness);
				COPY_ARR(glassSys.defs[i].texVecs);
				
				// correct color : bgra->rgba
				new_asset->glassSys.defs[i].color.array[0] = asset->glassSys.defs[i].color.array[2];
				new_asset->glassSys.defs[i].color.array[1] = asset->glassSys.defs[i].color.array[1];
				new_asset->glassSys.defs[i].color.array[2] = asset->glassSys.defs[i].color.array[0];
				new_asset->glassSys.defs[i].color.array[3] = asset->glassSys.defs[i].color.array[3];

				COPY_ASSET(glassSys.defs[i].material);
				COPY_ASSET(glassSys.defs[i].materialShattered);
				new_asset->glassSys.defs[i].physicsAsset = nullptr; // fixme

				CREATE_EFFECT(glassSys.defs[i].pieceBreakEffect, "code/glass_shatter_piece");
				CREATE_EFFECT(glassSys.defs[i].shatterEffect, "code/glass_shatter_64x64");
				CREATE_EFFECT(glassSys.defs[i].shatterSmallEffect, "code/glass_shatter_32x32");

				new_asset->glassSys.defs[i].crackDecalEffect.u.vfx = nullptr;

				CREATE_SOUND(glassSys.defs[i].damagedSound, "glass_pane_shatter");
				CREATE_SOUND(glassSys.defs[i].destroyedSound, "glass_pane_blowout");
				CREATE_SOUND(glassSys.defs[i].destroyedQuietSound, "glass_pane_breakout");

				//new_asset->glassSys.defs[i].highMipRadiusInvSq = 0.0652364343f;
				//new_asset->glassSys.defs[i].shatteredHighMipRadiusInvSq = 0.815455437f;

				new_asset->glassSys.defs[i].numCrackRings = -1;
				new_asset->glassSys.defs[i].isOpaque = 0;
			}

			new_asset->glassSys.piecePlaces = allocator.allocate<IW7::FxGlassPiecePlace>(new_asset->glassSys.pieceLimit);
			for (unsigned int i = 0; i < new_asset->glassSys.pieceLimit; i++)
			{
				memcpy(&new_asset->glassSys.piecePlaces[i], &asset->glassSys.piecePlaces[i], sizeof(IW5::FxGlassPiecePlace));
			}

			new_asset->glassSys.pieceStates = allocator.allocate<IW7::FxGlassPieceState>(new_asset->glassSys.pieceLimit);
			for (unsigned int i = 0; i < new_asset->glassSys.pieceLimit; i++)
			{
				memcpy(&new_asset->glassSys.pieceStates[i].texCoordOrigin, &asset->glassSys.pieceStates[i].texCoordOrigin, sizeof(float[2]));
				new_asset->glassSys.pieceStates[i].supportMask = asset->glassSys.pieceStates[i].supportMask;
				new_asset->glassSys.pieceStates[i].initIndex = asset->glassSys.pieceStates[i].initIndex;
				new_asset->glassSys.pieceStates[i].geoDataStart = asset->glassSys.pieceStates[i].geoDataStart;
				new_asset->glassSys.pieceStates[i].lightingIndex = asset->glassSys.pieceStates[i].initIndex;
				new_asset->glassSys.pieceStates[i].defIndex = asset->glassSys.pieceStates[i].defIndex;
				new_asset->glassSys.pieceStates[i].vertCount = asset->glassSys.pieceStates[i].vertCount;
				new_asset->glassSys.pieceStates[i].holeDataCount = asset->glassSys.pieceStates[i].holeDataCount;
				new_asset->glassSys.pieceStates[i].crackDataCount = asset->glassSys.pieceStates[i].crackDataCount;
				new_asset->glassSys.pieceStates[i].fanDataCount = asset->glassSys.pieceStates[i].fanDataCount;
				new_asset->glassSys.pieceStates[i].flags = asset->glassSys.pieceStates[i].flags;
				new_asset->glassSys.pieceStates[i].areaX2 = asset->glassSys.pieceStates[i].areaX2;
			}

			new_asset->glassSys.pieceDynamics = allocator.allocate<IW7::FxGlassPieceDynamics>(new_asset->glassSys.pieceLimit);
			for (unsigned int i = 0; i < new_asset->glassSys.pieceLimit; i++) // dynamic data
			{
				new_asset->glassSys.pieceDynamics[i].fallTime = asset->glassSys.pieceDynamics[i].fallTime;
				//new_asset->glassSys.pieceDynamics[i].physObjId = asset->glassSys.pieceDynamics[i].physObjId;
				//new_asset->glassSys.pieceDynamics[i].physJointId = asset->glassSys.pieceDynamics[i].physJointId;
				memcpy(&new_asset->glassSys.pieceDynamics[i].vel, &asset->glassSys.pieceDynamics[i].vel, sizeof(float[3]));
				memcpy(&new_asset->glassSys.pieceDynamics[i].avel, &asset->glassSys.pieceDynamics[i].avel, sizeof(float[3]));
			}

			new_asset->glassSys.geoData = allocator.allocate<IW7::FxGlassGeometryData>(new_asset->glassSys.geoDataLimit);
			for (unsigned int i = 0; i < new_asset->glassSys.geoDataLimit; i++)
			{
				memcpy(&new_asset->glassSys.geoData[i], &asset->glassSys.geoData[i], sizeof(IW5::FxGlassGeometryData));
			}

			new_asset->glassSys.isInUse = allocator.allocate<unsigned int>(new_asset->glassSys.pieceWordCount);
			for (unsigned int i = 0; i < new_asset->glassSys.pieceWordCount; i++)
			{
				new_asset->glassSys.isInUse[i] = asset->glassSys.isInUse[i];
			}

			new_asset->glassSys.cellBits = allocator.allocate<unsigned int>(new_asset->glassSys.pieceWordCount * new_asset->glassSys.cellCount);
			for (unsigned int i = 0; i < new_asset->glassSys.pieceWordCount * new_asset->glassSys.cellCount; i++)
			{
				new_asset->glassSys.cellBits[i] = asset->glassSys.cellBits[i];
			}

			new_asset->glassSys.visData = allocator.allocate<unsigned char>((new_asset->glassSys.pieceLimit + 15) & 0xFFFFFFF0);
			for (unsigned int i = 0; i < ((new_asset->glassSys.pieceLimit + 15) & 0xFFFFFFF0); i++)
			{
				new_asset->glassSys.visData[i] = asset->glassSys.visData[i];
			}

			new_asset->glassSys.linkOrg = reinterpret_cast<float(*)[3]>(asset->glassSys.linkOrg);

			new_asset->glassSys.halfThickness = allocator.allocate<float>((new_asset->glassSys.pieceLimit + 3) & 0xFFFFFFFC);
			for (unsigned int i = 0; i < ((new_asset->glassSys.pieceLimit + 3) & 0xFFFFFFFC); i++)
			{
				new_asset->glassSys.halfThickness[i] = asset->glassSys.halfThickness[i];
			}

			new_asset->glassSys.lightingHandles = allocator.allocate<unsigned short>(new_asset->glassSys.initPieceCount);
			for (unsigned int i = 0; i < new_asset->glassSys.initPieceCount; i++)
			{
				new_asset->glassSys.lightingHandles[i] = asset->glassSys.lightingHandles[i];
			}

			new_asset->glassSys.initPieceStates = allocator.allocate<IW7::FxGlassInitPieceState>(new_asset->glassSys.initPieceCount);
			for (unsigned int i = 0; i < new_asset->glassSys.initPieceCount; i++)
			{
				memcpy(&new_asset->glassSys.initPieceStates[i].frame, &asset->glassSys.initPieceStates[i].frame, sizeof(IW5::FxSpatialFrame));
				new_asset->glassSys.initPieceStates[i].radius = asset->glassSys.initPieceStates[i].radius;
				memcpy(&new_asset->glassSys.initPieceStates[i].texCoordOrigin, &asset->glassSys.initPieceStates[i].texCoordOrigin, sizeof(float[2]));
				new_asset->glassSys.initPieceStates[i].supportMask = asset->glassSys.initPieceStates[i].supportMask;
				new_asset->glassSys.initPieceStates[i].areaX2 = asset->glassSys.initPieceStates[i].areaX2;
				new_asset->glassSys.initPieceStates[i].lightingIndex = 0;
				new_asset->glassSys.initPieceStates[i].defIndex = asset->glassSys.initPieceStates[i].defIndex;
				new_asset->glassSys.initPieceStates[i].vertCount = asset->glassSys.initPieceStates[i].vertCount;
				new_asset->glassSys.initPieceStates[i].fanDataCount = asset->glassSys.initPieceStates[i].fanDataCount;
			}

			new_asset->glassSys.initGeoData = allocator.allocate<IW7::FxGlassGeometryData>(new_asset->glassSys.initGeoDataCount);
			for (unsigned int i = 0; i < new_asset->glassSys.initGeoDataCount; i++)
			{
				memcpy(&new_asset->glassSys.initGeoData[i], &asset->glassSys.initGeoData[i], sizeof(IW5::FxGlassGeometryData));
			}

			return new_asset;
		}

		IW7::FxWorld* convert(FxWorld* asset, allocator& allocator)
		{
			// generate IW7 fxworld
			return GenerateIW7FxWorld(asset, allocator);
		}
	}
}