#include "stdafx.hpp"

namespace ZoneTool::IW7
{
	void IGfxWorldTr::dump(GfxWorldTransientZone* asset)
	{
		const auto path = "transient_zones\\"s + asset->name + ".gfxmap_tr"s;

		assetmanager::dumper write;
		if (!write.open(path))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);

		write.dump_array(asset->vd.vertices, asset->vertexCount);
		write.dump_array(asset->vld.data, asset->vertexLayerDataSize);

		write.dump_array(asset->aabbTreeCounts, asset->cellCount);
		write.dump_array(asset->aabbTrees, asset->cellCount);
		for (unsigned int i = 0; i < asset->cellCount; i++)
		{
			write.dump_array(asset->aabbTrees[i].aabbTree, asset->aabbTreeCounts[i].aabbTreeCount);
			for (int j = 0; j < asset->aabbTreeCounts[i].aabbTreeCount; j++)
			{
				write.dump_array(asset->aabbTrees[i].aabbTree[j].smodelIndexes,
					asset->aabbTrees[i].aabbTree[j].smodelIndexCount);
			}
		}

		write.close();
	}
}