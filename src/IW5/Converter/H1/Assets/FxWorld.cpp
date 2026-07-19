#include "stdafx.hpp"
#include "../Include.hpp"

#include "FxWorld.hpp"

namespace ZoneTool::IW5
{
	namespace H1Converter
	{
		H1::FxWorld* GenerateH1FxWorld(FxWorld* asset, allocator& mem)
		{
			// allocate H1 FxWorld structure
			const auto h1_asset = mem.allocate<H1::FxWorld>();

			h1_asset->name = asset->name;

			h1_asset->glassSys.time = asset->glassSys.time;
			h1_asset->glassSys.prevTime = asset->glassSys.prevTime;
			h1_asset->glassSys.defCount = asset->glassSys.defCount;
			h1_asset->glassSys.pieceLimit = asset->glassSys.pieceLimit;
			h1_asset->glassSys.pieceWordCount = asset->glassSys.pieceWordCount;
			h1_asset->glassSys.initPieceCount = asset->glassSys.initPieceCount;
			h1_asset->glassSys.cellCount = asset->glassSys.cellCount;
			h1_asset->glassSys.activePieceCount = asset->glassSys.activePieceCount;
			h1_asset->glassSys.firstFreePiece = asset->glassSys.firstFreePiece;
			h1_asset->glassSys.geoDataLimit = asset->glassSys.geoDataLimit;
			h1_asset->glassSys.geoDataCount = asset->glassSys.geoDataCount;
			h1_asset->glassSys.initGeoDataCount = asset->glassSys.initGeoDataCount;

			h1_asset->glassSys.defs = mem.allocate<H1::FxGlassDef>(h1_asset->glassSys.defCount);
			for (unsigned int i = 0; i < h1_asset->glassSys.defCount; i++)
			{
				h1_asset->glassSys.defs[i].halfThickness = asset->glassSys.defs[i].halfThickness;
				memcpy(&h1_asset->glassSys.defs[i].texVecs, &asset->glassSys.defs[i].texVecs, sizeof(float[2][2]));
				
				// correct color : bgra->rgba
				h1_asset->glassSys.defs[i].color.array[0] = asset->glassSys.defs[i].color.array[2];
				h1_asset->glassSys.defs[i].color.array[1] = asset->glassSys.defs[i].color.array[1];
				h1_asset->glassSys.defs[i].color.array[2] = asset->glassSys.defs[i].color.array[0];
				h1_asset->glassSys.defs[i].color.array[3] = asset->glassSys.defs[i].color.array[3];

				h1_asset->glassSys.defs[i].material = reinterpret_cast<H1::Material*>(asset->glassSys.defs[i].material);
				h1_asset->glassSys.defs[i].materialShattered = reinterpret_cast<H1::Material*>(asset->glassSys.defs[i].materialShattered);
				h1_asset->glassSys.defs[i].physPreset = reinterpret_cast<H1::PhysPreset*>(asset->glassSys.defs[i].physPreset);

				h1_asset->glassSys.defs[i].pieceBreakEffect = mem.allocate<H1::FxEffectDef>();
				h1_asset->glassSys.defs[i].pieceBreakEffect->name = mem.duplicate_string("code/glass_shatter_piece");

				h1_asset->glassSys.defs[i].shatterEffect = mem.allocate<H1::FxEffectDef>();
				h1_asset->glassSys.defs[i].shatterEffect->name = mem.duplicate_string("code/glass_shatter_64x64");

				h1_asset->glassSys.defs[i].shatterSmallEffect = mem.allocate<H1::FxEffectDef>();
				h1_asset->glassSys.defs[i].shatterSmallEffect->name = mem.duplicate_string("code/glass_shatter_32x32");

				h1_asset->glassSys.defs[i].crackDecalEffect = nullptr;

				h1_asset->glassSys.defs[i].damagedSound = mem.allocate<H1::snd_alias_list_t>();
				h1_asset->glassSys.defs[i].damagedSound->name = mem.duplicate_string("glass_pane_shatter");

				h1_asset->glassSys.defs[i].destroyedSound = mem.allocate<H1::snd_alias_list_t>();
				h1_asset->glassSys.defs[i].destroyedSound->name = mem.duplicate_string("glass_pane_blowout");

				h1_asset->glassSys.defs[i].destroyedQuietSound = mem.allocate<H1::snd_alias_list_t>();
				h1_asset->glassSys.defs[i].destroyedQuietSound->name = mem.duplicate_string("glass_pane_breakout");

				h1_asset->glassSys.defs[i].invHighMipRadius = 0.0652364343f;
				h1_asset->glassSys.defs[i].shatteredInvHighMipRadius = 0.815455437f;
				h1_asset->glassSys.defs[i].numCrackRings = -1;
				h1_asset->glassSys.defs[i].isOpaque = 0;
			}

			h1_asset->glassSys.piecePlaces = mem.allocate<H1::FxGlassPiecePlace>(h1_asset->glassSys.pieceLimit);
			for (unsigned int i = 0; i < h1_asset->glassSys.pieceLimit; i++)
			{
				memcpy(&h1_asset->glassSys.piecePlaces[i], &asset->glassSys.piecePlaces[i], sizeof(IW5::FxGlassPiecePlace));
			}

			h1_asset->glassSys.pieceStates = mem.allocate<H1::FxGlassPieceState>(h1_asset->glassSys.pieceLimit);
			for (unsigned int i = 0; i < h1_asset->glassSys.pieceLimit; i++)
			{
				memcpy(&h1_asset->glassSys.pieceStates[i].texCoordOrigin, &asset->glassSys.pieceStates[i].texCoordOrigin, sizeof(float[2]));
				h1_asset->glassSys.pieceStates[i].supportMask = asset->glassSys.pieceStates[i].supportMask;
				h1_asset->glassSys.pieceStates[i].initIndex = asset->glassSys.pieceStates[i].initIndex;
				h1_asset->glassSys.pieceStates[i].geoDataStart = asset->glassSys.pieceStates[i].geoDataStart;
				h1_asset->glassSys.pieceStates[i].lightingIndex = asset->glassSys.pieceStates[i].initIndex;
				h1_asset->glassSys.pieceStates[i].defIndex = asset->glassSys.pieceStates[i].defIndex;
				h1_asset->glassSys.pieceStates[i].vertCount = asset->glassSys.pieceStates[i].vertCount;
				h1_asset->glassSys.pieceStates[i].holeDataCount = asset->glassSys.pieceStates[i].holeDataCount;
				h1_asset->glassSys.pieceStates[i].crackDataCount = asset->glassSys.pieceStates[i].crackDataCount;
				h1_asset->glassSys.pieceStates[i].fanDataCount = asset->glassSys.pieceStates[i].fanDataCount;
				h1_asset->glassSys.pieceStates[i].flags = asset->glassSys.pieceStates[i].flags;
				h1_asset->glassSys.pieceStates[i].areaX2 = asset->glassSys.pieceStates[i].areaX2;
			}

			h1_asset->glassSys.pieceDynamics = mem.allocate<H1::FxGlassPieceDynamics>(h1_asset->glassSys.pieceLimit);
			for (unsigned int i = 0; i < h1_asset->glassSys.pieceLimit; i++)
			{
				h1_asset->glassSys.pieceDynamics[i].fallTime = asset->glassSys.pieceDynamics[i].fallTime;
				h1_asset->glassSys.pieceDynamics[i].physObjId = asset->glassSys.pieceDynamics[i].physObjId;
				h1_asset->glassSys.pieceDynamics[i].physJointId = asset->glassSys.pieceDynamics[i].physJointId;
				memcpy(&h1_asset->glassSys.pieceDynamics[i].vel, &asset->glassSys.pieceDynamics[i].vel, sizeof(float[3]));
				memcpy(&h1_asset->glassSys.pieceDynamics[i].avel, &asset->glassSys.pieceDynamics[i].avel, sizeof(float[3]));
			}

			h1_asset->glassSys.geoData = mem.allocate<H1::FxGlassGeometryData>(h1_asset->glassSys.geoDataLimit);
			for (unsigned int i = 0; i < h1_asset->glassSys.geoDataLimit; i++)
			{
				memcpy(&h1_asset->glassSys.geoData[i], &asset->glassSys.geoData[i], sizeof(IW5::FxGlassGeometryData));
			}

			h1_asset->glassSys.isInUse = mem.allocate<unsigned int>(h1_asset->glassSys.pieceWordCount);
			for (unsigned int i = 0; i < h1_asset->glassSys.pieceWordCount; i++)
			{
				h1_asset->glassSys.isInUse[i] = asset->glassSys.isInUse[i];
			}

			h1_asset->glassSys.cellBits = mem.allocate<unsigned int>(h1_asset->glassSys.pieceWordCount * h1_asset->glassSys.cellCount);
			for (unsigned int i = 0; i < h1_asset->glassSys.pieceWordCount * h1_asset->glassSys.cellCount; i++)
			{
				h1_asset->glassSys.cellBits[i] = asset->glassSys.cellBits[i];
			}

			h1_asset->glassSys.visData = mem.allocate<unsigned char>((h1_asset->glassSys.pieceLimit + 15) & 0xFFFFFFF0);
			for (unsigned int i = 0; i < ((h1_asset->glassSys.pieceLimit + 15) & 0xFFFFFFF0); i++)
			{
				h1_asset->glassSys.visData[i] = asset->glassSys.visData[i];
			}

			h1_asset->glassSys.linkOrg = reinterpret_cast<float(*)[3]>(asset->glassSys.linkOrg);

			h1_asset->glassSys.halfThickness = mem.allocate<float>((h1_asset->glassSys.pieceLimit + 3) & 0xFFFFFFFC);
			for (unsigned int i = 0; i < ((h1_asset->glassSys.pieceLimit + 3) & 0xFFFFFFFC); i++)
			{
				h1_asset->glassSys.halfThickness[i] = asset->glassSys.halfThickness[i];
			}

			h1_asset->glassSys.lightingHandles = mem.allocate<unsigned short>(h1_asset->glassSys.initPieceCount);
			for (unsigned int i = 0; i < h1_asset->glassSys.initPieceCount; i++)
			{
				h1_asset->glassSys.lightingHandles[i] = asset->glassSys.lightingHandles[i];
			}

			h1_asset->glassSys.initPieceStates = mem.allocate<H1::FxGlassInitPieceState>(h1_asset->glassSys.initPieceCount);
			for (unsigned int i = 0; i < h1_asset->glassSys.initPieceCount; i++)
			{
				memcpy(&h1_asset->glassSys.initPieceStates[i].frame, &asset->glassSys.initPieceStates[i].frame, sizeof(IW5::FxSpatialFrame));
				h1_asset->glassSys.initPieceStates[i].radius = asset->glassSys.initPieceStates[i].radius;
				memcpy(&h1_asset->glassSys.initPieceStates[i].texCoordOrigin, &asset->glassSys.initPieceStates[i].texCoordOrigin, sizeof(float[2]));
				h1_asset->glassSys.initPieceStates[i].supportMask = asset->glassSys.initPieceStates[i].supportMask;
				h1_asset->glassSys.initPieceStates[i].areaX2 = asset->glassSys.initPieceStates[i].areaX2;
				h1_asset->glassSys.initPieceStates[i].lightingIndex = 0;
				h1_asset->glassSys.initPieceStates[i].defIndex = asset->glassSys.initPieceStates[i].defIndex;
				h1_asset->glassSys.initPieceStates[i].vertCount = asset->glassSys.initPieceStates[i].vertCount;
				h1_asset->glassSys.initPieceStates[i].fanDataCount = asset->glassSys.initPieceStates[i].fanDataCount;
			}

			h1_asset->glassSys.initGeoData = mem.allocate<H1::FxGlassGeometryData>(h1_asset->glassSys.initGeoDataCount);
			for (unsigned int i = 0; i < h1_asset->glassSys.initGeoDataCount; i++)
			{
				memcpy(&h1_asset->glassSys.initGeoData[i], &asset->glassSys.initGeoData[i], sizeof(IW5::FxGlassGeometryData));
			}

			h1_asset->glassSys.needToCompactData = asset->glassSys.needToCompactData;
			h1_asset->glassSys.initCount = asset->glassSys.initCount;
			h1_asset->glassSys.effectChanceAccum = asset->glassSys.effectChanceAccum;
			h1_asset->glassSys.lastPieceDeletionTime = asset->glassSys.lastPieceDeletionTime;

			return h1_asset;
		}

		H1::FxWorld* convert(FxWorld* asset, allocator& allocator)
		{
			// generate h1 fxworld
			return GenerateH1FxWorld(asset, allocator);
		}
	}
}