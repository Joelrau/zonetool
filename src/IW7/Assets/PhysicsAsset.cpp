#include "stdafx.hpp"
#include "PhysicsAsset.hpp"

#include "../Common/havok.hpp"

namespace ZoneTool::IW7
{
	void IPhysicsAsset::dump(PhysicsAsset* asset)
	{
		const auto path = "physicsasset\\"s + asset->name;

		assetmanager::dumper write;
		if (!write.open(path))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);

		for (auto i = 0; i < asset->numSFXEventAssets; i++)
		{
			write.dump_asset(asset->sfxEventAssets[i]);
		}

		for (auto i = 0; i < asset->numVFXEventAssets; i++)
		{
			write.dump_asset(asset->vfxEventAssets[i]);
		}

		write.close();

		havok::binary::dump_havok_data(path, asset->havokData, asset->havokDataSize);
	}
}
