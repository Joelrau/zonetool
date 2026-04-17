#include "stdafx.hpp"

namespace ZoneTool::IW6
{
	void IClipMap::dump_info(ClipInfo* info, assetmanager::dumper& write)
	{
		write.dump_array(info->planes, info->planeCount);
		write.dump_array(info->materials, info->numMaterials);
		for (unsigned int i = 0; i < info->numMaterials; i++)
		{
			write.dump_string(info->materials[i].name);
		}

		write.dump_array(info->brushsides, info->numBrushSides);
		for (unsigned int i = 0; i < info->numBrushSides; i++)
		{
			write.dump_array(info->brushsides[i].plane, 1);
		}

		write.dump_array(info->brushEdges, info->numBrushEdges);
		write.dump_array(info->leafbrushNodes, info->leafbrushNodesCount);
		for (unsigned int i = 0; i < info->leafbrushNodesCount; i++)
		{
			if (info->leafbrushNodes[i].leafBrushCount > 0)
			{
				write.dump_array(info->leafbrushNodes[i].data.leaf.brushes, info->leafbrushNodes[i].leafBrushCount);
			}
		}

		write.dump_array(info->leafbrushes, info->numLeafBrushes);
		write.dump_array(info->brushes, info->numBrushes);
		for (unsigned int i = 0; i < info->numBrushes; i++)
		{
			if (info->brushes[i].sides)
			{
				write.dump_array(info->brushes[i].sides, 1);
				if (info->brushes[i].sides->plane)
				{
					write.dump_array(info->brushes[i].sides->plane, 1);
				}
			}
			if (info->brushes[i].baseAdjacentSide)
			{
				write.dump_array(info->brushes[i].baseAdjacentSide, 1);
			}
		}

		write.dump_array(info->brushBounds, info->numBrushes);
		write.dump_array(info->brushContents, info->numBrushes);
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

		//write.dump_single(asset->pInfo);
		//dump_info(asset->pInfo, write);

		write.dump_array(asset->staticModelList, asset->numStaticModels);
		for (unsigned int i = 0; i < asset->numStaticModels; i++)
		{
			if (asset->staticModelList[i].xmodel)
			{
				write.dump_asset(asset->staticModelList[i].xmodel);
			}
		}

		write.dump_array(asset->nodes, asset->numNodes);
		for (unsigned int i = 0; i < asset->numNodes; i++)
		{
			if (asset->nodes[i].plane)
			{
				write.dump_array(asset->nodes[i].plane, 1);
			}
		}

		write.dump_array(asset->leafs, asset->numLeafs);
		write.dump_array(asset->verts, asset->vertCount);
		write.dump_array(asset->triIndices, asset->triCount * 3);
		write.dump_array(asset->triEdgeIsWalkable, ((asset->triCount + 2 * asset->triCount + 31) >> 3) & 0xFFFFFFFFFFFFFFFCui64);
		write.dump_array(asset->borders, asset->borderCount);

		write.dump_array(asset->partitions, asset->partitionCount);
		for (int i = 0; i < asset->partitionCount; i++)
		{
			if (asset->partitions[i].borders)
			{
				write.dump_array(asset->partitions[i].borders, 1);
			}
		}

		write.dump_array(asset->aabbTrees, asset->aabbTreeCount);

		write.dump_array(asset->cmodels, asset->numSubModels);
		//for (unsigned int i = 0; i < asset->numSubModels; i++)
		//{
		//	if (asset->cmodels[i].info)
		//	{
		//		write.dump_single(asset->cmodels[i].info);
		//		dump_info(asset->cmodels[i].info, write);
		//	}
		//}

		write.dump_array(asset->smodelNodes, asset->smodelNodeCount);
		write.dump_asset(asset->mapEnts);

		write.dump_array(asset->stages, asset->stageCount);
		for (unsigned char i = 0; i < asset->stageCount; i++)
		{
			write.dump_string(asset->stages[i].name);
		}
		//write.dump_array(asset->stageTrigger.models, asset->stageTrigger.count);
		//write.dump_array(asset->stageTrigger.hulls, asset->stageTrigger.hullCount);
		//write.dump_array(asset->stageTrigger.slabs, asset->stageTrigger.slabCount);

		for (int i = 0; i < 2; i++)
		{
			write.dump_int(asset->dynEntCount[i]);

			if (asset->dynEntCount[i] <= 0)
			{
				continue;
			}

			write.dump_array(asset->dynEntDefList[i], asset->dynEntCount[i]);
			write.dump_array(asset->dynEntPoseList[i], asset->dynEntCount[i]);
			write.dump_array(asset->dynEntClientList[i], asset->dynEntCount[i]);
			write.dump_array(asset->dynEntCollList[i], asset->dynEntCount[i]);
			for (int j = 0; j < asset->dynEntCount[i]; j++)
			{
				write.dump_asset(asset->dynEntDefList[i][j].baseModel);
				write.dump_asset(asset->dynEntDefList[i][j].destroyFx);
				write.dump_asset(asset->dynEntDefList[i][j].physPreset);
				write.dump_single(asset->dynEntDefList[i][j].hinge);
				write.dump_single(asset->dynEntDefList[i][j].linkTo);
			}
		}

		for (unsigned int i = 0; i < asset->dynEntAnchorCount; i++)
		{
			write.dump_string(SL_ConvertToString(asset->dynEntAnchorNames[i]));
		}

		write.dump_array(asset->scriptableMapEnts.instances, asset->scriptableMapEnts.instanceCount);
		if (asset->scriptableMapEnts.instances)
		{
			for (unsigned int i = 0; i < asset->scriptableMapEnts.instanceCount; i++)
			{
				if (asset->scriptableMapEnts.instances[i].def)
				{
					write.dump_asset(asset->scriptableMapEnts.instances[i].def);
				}
				if (asset->scriptableMapEnts.instances[i].eventConstantsBuf && asset->scriptableMapEnts.instances[i].def)
				{
					write.dump_array(asset->scriptableMapEnts.instances[i].eventConstantsBuf,
						asset->scriptableMapEnts.instances[i].def->eventConstantsSize);
				}
				auto str = SL_ConvertToString(asset->scriptableMapEnts.instances[i].targetname);
				write.dump_string(str ? str : "");
				if (asset->scriptableMapEnts.instances[i].currentModel)
				{
					write.dump_asset(asset->scriptableMapEnts.instances[i].currentModel);
				}
				if (asset->scriptableMapEnts.instances[i].partStates && asset->scriptableMapEnts.instances[i].def)
				{
					write.dump_array(asset->scriptableMapEnts.instances[i].partStates,
						asset->scriptableMapEnts.instances[i].def->partCount);
				}
				if (asset->scriptableMapEnts.instances[i].eventStreamBuf && asset->scriptableMapEnts.instances[i].def)
				{
					write.dump_array(asset->scriptableMapEnts.instances[i].eventStreamBuf,
						asset->scriptableMapEnts.instances[i].def->eventStreamSize);
				}
			}
		}

		write.dump_array(asset->scriptableMapEnts.animEntries, asset->scriptableMapEnts.animEntryCount);
		if (asset->scriptableMapEnts.animEntries)
		{
			for (unsigned int i = 0; i < asset->scriptableMapEnts.animEntryCount; i++)
			{
				if (asset->scriptableMapEnts.animEntries[i].animName)
				{
					write.dump_string(asset->scriptableMapEnts.animEntries[i].animName);
				}
			}
		}

		write.close();
	}
}