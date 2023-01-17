#include "stdafx.hpp"
#include "H1/Assets/PhysPreset.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		H1::PhysPreset* GenerateH1Asset(PhysPreset* asset, ZoneMemory* mem)
		{
			auto* h1_asset = mem->Alloc<H1::PhysPreset>();
			
			h1_asset->name = asset->name;
			h1_asset->type = asset->type; // convert?
			h1_asset->mass = asset->mass;
			h1_asset->bounce = asset->bounce;
			h1_asset->friction = asset->friction;
			h1_asset->bulletForceScale = asset->bulletForceScale;
			h1_asset->explosiveForceScale = asset->explosiveForceScale;
			//h1_asset->__pad0;
			h1_asset->sndAliasPrefix = asset->sndAliasPrefix;
			h1_asset->piecesSpreadFraction = asset->piecesSpreadFraction;
			h1_asset->piecesUpwardVelocity = asset->piecesUpwardVelocity;
			h1_asset->minMomentum = 0.0f; // does not exist on IW3/IW4
			h1_asset->maxMomentum = 0.0f; // ^
			h1_asset->minPitch = 0.0f; // ^
			h1_asset->maxPitch = 0.0f; // ^
			h1_asset->volumeType = static_cast<H1::PhysPresetScaling>(0); // ^
			h1_asset->pitchType = static_cast<H1::PhysPresetScaling>(0); // ^
			h1_asset->tempDefaultToCylinder = asset->tempDefaultToCylinder;
			h1_asset->perSurfaceSndAlias = 0; // ^

			return h1_asset;
		}

		void IPhysPreset::dump(PhysPreset* asset, ZoneMemory* mem)
		{
			// generate h1 asset
			auto* h1_asset = GenerateH1Asset(asset, mem);

			// dump asset
			H1::IPhysPreset::dump(h1_asset);
		}
	}
}