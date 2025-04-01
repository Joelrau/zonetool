#include "stdafx.hpp"
#include "IW5/Assets/Material.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		IW5::Material* convert(Material* asset, allocator& mem)
		{
			auto* iw5_asset = mem.allocate<IW5::Material>();
			iw5_asset->name = asset->name;

			iw5_asset->info.gameFlags = asset->gameFlags;
			iw5_asset->info.sortKey = asset->sortKey;
			iw5_asset->info.textureAtlasRowCount = asset->textureAtlasRowCount;
			iw5_asset->info.textureAtlasColumnCount = asset->textureAtlasColumnCount;
			//iw5_asset->info.drawSurf = asset->drawSurf;
			iw5_asset->info.surfaceTypeBits = asset->surfaceTypeBits;

			std::memset(iw5_asset->stateBitsEntry, 0xFF, sizeof(iw5_asset->stateBitsEntry)); // convert...

			iw5_asset->textureCount = asset->textureCount;
			iw5_asset->constantCount = asset->constantCount;
			iw5_asset->stateBitsCount = asset->stateBitsCount;
			iw5_asset->stateFlags = asset->stateFlags;

			iw5_asset->cameraRegion = asset->cameraRegion;
			if (iw5_asset->cameraRegion > CAMERA_REGION_NONE)
			{
				iw5_asset->cameraRegion = CAMERA_REGION_NONE;
			}

			iw5_asset->techniqueSet = reinterpret_cast<IW5::MaterialTechniqueSet*>(asset->techniqueSet);

			static_assert(sizeof(MaterialTextureDef) == sizeof(IW5::MaterialTextureDef));
			iw5_asset->textureTable = reinterpret_cast<IW5::MaterialTextureDef*>(asset->textureTable);

			iw5_asset->constantTable = reinterpret_cast<IW5::MaterialConstantDef*>(asset->constantTable); // convert?
			iw5_asset->stateBitsTable = reinterpret_cast<IW5::GfxStateBits*>(asset->stateBitsTable); // convert?

			iw5_asset->subMaterials = nullptr;

			return iw5_asset;
		}

		void IMaterial::dump(Material* asset)
		{
			allocator allocator;
			auto* iw5_asset = convert(asset, allocator);

			// dump asset
			IW5::IMaterial::dump(iw5_asset);
		}
	}
}