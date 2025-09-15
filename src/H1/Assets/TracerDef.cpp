#include "stdafx.hpp"

namespace ZoneTool::H1
{
	void ITracerDef::dump(TracerDef* asset)
	{
		const auto path = "tracer\\"s + asset->name;

		assetmanager::dumper dump;
		if (!dump.open(path))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);

		dump.dump_asset(asset->material);
		dump.dump_asset(asset->effectDef);

		dump.close();
	}
}