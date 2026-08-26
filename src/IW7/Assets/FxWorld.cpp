#include "stdafx.hpp"

namespace ZoneTool::IW7
{
#define DUMP_VFX(__NAME__) \
	if (__NAME__.type == FX_COMBINED_VFX) \
	{ \
		write.dump_asset(__NAME__.u.vfx); \
	} \
	else \
	{ \
		write.dump_asset(__NAME__.u.fx); \
	}

	void IFxWorld::dump(FxWorld* asset)
	{
		const auto path = asset->name + ".fxmap"s;

		assetmanager::dumper write;
		if (!write.open(path))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);

		write.dump_array(asset->glassSys.defs, asset->glassSys.defCount);
		for (unsigned int i = 0; i < asset->glassSys.defCount; i++)
		{
			write.dump_asset(asset->glassSys.defs[i].material);
			write.dump_asset(asset->glassSys.defs[i].materialShattered);

			write.dump_asset(asset->glassSys.defs[i].physicsAsset);

			DUMP_VFX(asset->glassSys.defs[i].pieceBreakEffect);
			DUMP_VFX(asset->glassSys.defs[i].shatterEffect);
			DUMP_VFX(asset->glassSys.defs[i].shatterSmallEffect);
			DUMP_VFX(asset->glassSys.defs[i].crackDecalEffect);

			write.dump_string(asset->glassSys.defs[i].damagedSound);
			write.dump_string(asset->glassSys.defs[i].destroyedSound);
			write.dump_string(asset->glassSys.defs[i].destroyedQuietSound);
		}

		write.dump_array(asset->glassSys.lightingHandles, asset->glassSys.initPieceCount);
		write.dump_array(asset->glassSys.initGeoData, asset->glassSys.initGeoDataCount);
		write.dump_array(asset->glassSys.initPieceStates, asset->glassSys.initPieceCount);

		write.close();
	}

#undef DUMP_VFX
}