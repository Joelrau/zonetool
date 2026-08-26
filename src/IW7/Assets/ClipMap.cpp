#include "stdafx.hpp"

#include "../Common/havok.hpp"

namespace ZoneTool::IW7
{
	void IClipMap::dump_info(ClipInfo* info, assetmanager::dumper& write)
	{
		write.dump_array(info->planes, info->planeCount);
	}

	void IClipMap::dump(clipMap_t* asset)
	{
		const auto path = asset->name + ".colmap"s;

		assetmanager::dumper write;
		if (!write.open(path))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);

		dump_info(&asset->info, write);

		write.dump_array(asset->staticModelList, asset->numStaticModels);
		for (unsigned int i = 0; i < asset->numStaticModels; i++)
		{
			write.dump_asset(asset->staticModelList[i].xmodel);
		}

		write.dump_array(asset->staticModelCollisionModelList.staticModelIndex, asset->staticModelCollisionModelList.numModels);
		write.dump_array(asset->staticModelCollisionModelLists, asset->numStaticModelCollisionModelLists);
		for (unsigned int i = 0; i < asset->numStaticModelCollisionModelLists; i++)
		{
			write.dump_array(asset->staticModelCollisionModelLists[i].staticModelIndex, asset->staticModelCollisionModelLists[i].numModels);
		}

		write.dump_asset(asset->mapEnts);

		write.dump_array(asset->stages, asset->stageCount);
		for (unsigned char i = 0; i < asset->stageCount; i++)
		{
			write.dump_string(asset->stages[i].name);
		}

		const auto havok_data_path = path;
		havok::binary::dump_havok_data(havok_data_path, asset->havokWorldShapeData, asset->havokWorldShapeDataSize);

		write.dump_array(asset->collisionHeatmap, asset->numCollisionHeatmapEntries);

		write.dump_single(asset->topDownMapData);

		write.close();
	}
}