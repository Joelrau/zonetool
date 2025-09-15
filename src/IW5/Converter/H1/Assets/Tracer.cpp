#include "stdafx.hpp"
#include "../Include.hpp"

#include "Tracer.hpp"

namespace ZoneTool::IW5
{
	namespace H1Converter
	{
		H1::TracerDef* GenerateH1TracerDef(TracerDef* asset, allocator& mem)
		{
			auto* h1_asset = mem.allocate<H1::TracerDef>();

			h1_asset->name = asset->name;
			h1_asset->material = reinterpret_cast<H1::Material*>(asset->material);
			h1_asset->effectDef = nullptr;
			h1_asset->drawInterval = asset->drawInterval;
			h1_asset->speed = asset->speed;
			h1_asset->beamLength = asset->beamLength;
			h1_asset->beamWidth = asset->beamWidth;
			h1_asset->screwRadius = asset->screwRadius;
			h1_asset->screwDist = asset->screwDist;
			memcpy(h1_asset->colors, asset->colors, sizeof(asset->colors));

			return h1_asset;
		}

		H1::TracerDef* convert(TracerDef* asset, allocator& allocator)
		{
			// generate h1 asset
			return GenerateH1TracerDef(asset, allocator);
		}
	}
}