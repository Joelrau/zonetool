#include "stdafx.hpp"

namespace ZoneTool::S1
{
	void IPhysPreset::dump(PhysPreset* asset)
	{
		const auto path = "physpreset\\"s + asset->name + ".pp";

		assetmanager::dumper write;
		if (!write.open(path))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);
		write.dump_string(asset->sndAliasPrefix);
		write.close();
	}
}
