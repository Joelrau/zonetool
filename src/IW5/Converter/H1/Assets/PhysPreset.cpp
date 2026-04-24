#include "stdafx.hpp"
#include "../Include.hpp"

#include "PhysPreset.hpp"

namespace ZoneTool::IW5
{
	namespace H1Converter
	{
		H1::PhysPreset* GenerateH1Asset(PhysPreset* asset, allocator& mem)
		{
			auto* h1_asset = mem.allocate<H1::PhysPreset>();
			
			h1_asset->name = asset->name;
			h1_asset->type = asset->type; // convert?
			h1_asset->mass = asset->mass;
			h1_asset->bounce = asset->bounce;
			h1_asset->friction = asset->friction;
			h1_asset->bulletForceScale = asset->bulletForceScale;
			h1_asset->explosiveForceScale = asset->explosiveForceScale;
			h1_asset->explosiveSpinScale = 0.0f;
			h1_asset->contents = 42001553;
			h1_asset->sndAliasPrefix = asset->sndAliasPrefix;
			h1_asset->piecesSpreadFraction = asset->piecesSpreadFraction;
			h1_asset->piecesUpwardVelocity = asset->piecesUpwardVelocity;
			h1_asset->minMomentum = asset->minMomentum;
			h1_asset->maxMomentum = asset->maxMomentum;
			h1_asset->minPitch = asset->minPitch;
			h1_asset->maxPitch = asset->maxPitch;
			h1_asset->volumeType = static_cast<H1::PhysPresetScaling>(asset->volumeType);
			h1_asset->pitchType = static_cast<H1::PhysPresetScaling>(asset->pitchType);
			h1_asset->tempDefaultToCylinder = asset->tempDefaultToCylinder;
			h1_asset->perSurfaceSndAlias = asset->perSurfaceSndAlias;

			return h1_asset;
		}

		H1::PhysPreset* convert(PhysPreset* asset, allocator& allocator)
		{
			// generate h1 asset
			return GenerateH1Asset(asset, allocator);
		}
	}
}