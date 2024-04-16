#include "stdafx.hpp"

namespace ZoneTool::S1
{
	void IGfxWorld::dump(GfxWorld* asset)
	{
		const auto path = asset->name + ".gfxmap"s;

		assetmanager::dumper write;
		if (!write.open(path))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);
		write.dump_string(asset->baseName);

		write.dump_array(asset->skies, asset->skyCount);
		for (int i = 0; i < asset->skyCount; i++)
		{
			write.dump_array(asset->skies[i].skyStartSurfs, asset->skies[i].skySurfCount);
			write.dump_asset(asset->skies[i].skyImage);
		}

		write.dump_array(asset->dpvsPlanes.planes, asset->planeCount);
		write.dump_array(asset->dpvsPlanes.nodes, asset->nodeCount);
		write.dump_array(asset->dpvsPlanes.sceneEntCellBits, asset->dpvsPlanes.cellCount << 9);

		write.dump_array(asset->aabbTreeCounts, asset->dpvsPlanes.cellCount);
		write.dump_array(asset->aabbTrees, asset->dpvsPlanes.cellCount);
		for (int i = 0; i < asset->dpvsPlanes.cellCount; i++)
		{
			write.dump_array(asset->aabbTrees[i].aabbTree, asset->aabbTreeCounts[i].aabbTreeCount);
			for (int j = 0; j < asset->aabbTreeCounts[i].aabbTreeCount; j++)
			{
				write.dump_array(asset->aabbTrees[i].aabbTree[j].smodelIndexes,
					asset->aabbTrees[i].aabbTree[j].smodelIndexCount);
			}
		}

		write.dump_array(asset->cells, asset->dpvsPlanes.cellCount);
		for (int i = 0; i < asset->dpvsPlanes.cellCount; i++)
		{
			write.dump_array(asset->cells[i].portals, asset->cells[i].portalCount);
			for (int j = 0; j < asset->cells[i].portalCount; j++)
			{
				write.dump_array(asset->cells[i].portals[j].vertices, asset->cells[i].portals[j].vertexCount);
			}
			write.dump_array(asset->cells[i].reflectionProbes, asset->cells[i].reflectionProbeCount);
			write.dump_array(asset->cells[i].reflectionProbeReferences, asset->cells[i].reflectionProbeReferenceCount);
		}

		write.dump_array(asset->portalGroup, asset->portalGroupCount);
		for (unsigned int i = 0; i < asset->portalGroupCount; i++)
		{
			if (asset->portalGroup[i].group)
			{
				write.dump_string(asset->portalGroup[i].group);
			}

			write.dump_array(asset->portalGroup[i].info, asset->portalGroup[i].infoCount);
		}

		write.dump_array(asset->unk_vec4_0, asset->unk_vec4_count_0);

		write.dump_array(asset->draw.reflectionProbes, asset->draw.reflectionProbeCount);
		for (unsigned int i = 0; i < asset->draw.reflectionProbeCount; i++)
		{
			write.dump_asset(asset->draw.reflectionProbes[i]);
		}

		write.dump_array(asset->draw.reflectionProbeOrigins, asset->draw.reflectionProbeCount);
		for (unsigned int i = 0; i < asset->draw.reflectionProbeCount; i++)
		{
			write.dump_array(asset->draw.reflectionProbeOrigins[i].probeVolumes, asset->draw.reflectionProbeOrigins[i].probeVolumeCount);
			for (unsigned int j = 0; j < asset->draw.reflectionProbeOrigins[i].probeVolumeCount; j++)
			{
				write.dump_array(asset->draw.reflectionProbeOrigins[i].probeVolumes[j].data, asset->draw.reflectionProbeOrigins[i].probeVolumes[j].count);
			}
		}

		write.dump_array(asset->draw.reflectionProbeTextures, asset->draw.reflectionProbeCount);

		write.dump_array(asset->draw.reflectionProbeReferenceOrigins, asset->draw.reflectionProbeReferenceCount);
		write.dump_array(asset->draw.reflectionProbeReferences, asset->draw.reflectionProbeReferenceCount);

		write.dump_array(asset->draw.lightmaps, asset->draw.lightmapCount);
		for (int i = 0; i < asset->draw.lightmapCount; i++)
		{
			write.dump_asset(asset->draw.lightmaps[i].primary);
			write.dump_asset(asset->draw.lightmaps[i].secondary);
		}
		write.dump_array(asset->draw.lightmapPrimaryTextures, asset->draw.lightmapCount);
		write.dump_array(asset->draw.lightmapSecondaryTextures, asset->draw.lightmapCount);
		write.dump_asset(asset->draw.lightmapOverridePrimary);
		write.dump_asset(asset->draw.lightmapOverrideSecondary);

		write.dump_array(asset->draw.vd.vertices, asset->draw.vertexCount);
		write.dump_array(asset->draw.vld.data, asset->draw.vertexLayerDataSize);
		write.dump_array(asset->draw.indices, asset->draw.indexCount);

		write.dump_array(asset->lightGrid.rowDataStart,
			(asset->lightGrid.maxs[asset->lightGrid.rowAxis] - asset->lightGrid.mins[asset->lightGrid.rowAxis] + 1));
		write.dump_array(asset->lightGrid.rawRowData, asset->lightGrid.rawRowDataSize);
		write.dump_array(asset->lightGrid.entries, asset->lightGrid.entryCount);
		write.dump_array(asset->lightGrid.colors, asset->lightGrid.colorCount);

		write.dump_array(asset->lightGrid.stageLightingContrastGain, asset->lightGrid.stageCount);
		write.dump_array(asset->lightGrid.paletteEntryAddress, asset->lightGrid.paletteEntryCount);
		write.dump_array(asset->lightGrid.paletteBitstream, asset->lightGrid.paletteBitstreamSize);

		for (auto i = 0; i < 3; i++)
		{
			write.dump_array(asset->lightGrid.tree[i].p_nodeTable, asset->lightGrid.tree[i].nodeCount);
			write.dump_array(asset->lightGrid.tree[i].p_leafTable, asset->lightGrid.tree[i].leafTableSize);
		}

		write.dump_array(asset->models, asset->modelCount);

		write.dump_array(asset->materialMemory, asset->materialMemoryCount);
		for (int i = 0; i < asset->materialMemoryCount; i++)
		{
			write.dump_asset(asset->materialMemory[i].material);
		}

		write.dump_asset(asset->sun.spriteMaterial);
		write.dump_asset(asset->sun.flareMaterial);

		write.dump_asset(asset->outdoorImage);

		write.dump_array(asset->cellCasterBits,
			asset->dpvsPlanes.cellCount * ((asset->dpvsPlanes.cellCount + 31) >> 5));
		write.dump_array(asset->cellHasSunLitSurfsBits, (asset->dpvsPlanes.cellCount + 31) >> 5);

		write.dump_array(asset->sceneDynModel, asset->dpvsDyn.dynEntClientCount[0]);
		write.dump_array(asset->sceneDynBrush, asset->dpvsDyn.dynEntClientCount[1]);

		write.dump_array(asset->primaryLightEntityShadowVis,
			((asset->primaryLightCount - asset->lastSunPrimaryLightIndex) << 13) - 0x2000);

		write.dump_array(asset->primaryLightDynEntShadowVis[0],
			asset->dpvsDyn.dynEntClientCount[0] * (asset->primaryLightCount - asset->lastSunPrimaryLightIndex - 1));
		write.dump_array(asset->primaryLightDynEntShadowVis[1],
			asset->dpvsDyn.dynEntClientCount[1] * (asset->primaryLightCount - asset->lastSunPrimaryLightIndex - 1));

		write.dump_array(asset->nonSunPrimaryLightForModelDynEnt, asset->dpvsDyn.dynEntClientCount[0]);

		write.dump_array(asset->shadowGeom, asset->primaryLightCount);
		if (asset->shadowGeom)
		{
			for (unsigned int i = 0; i < asset->primaryLightCount; i++)
			{
				write.dump_array(asset->shadowGeom[i].sortedSurfIndex, asset->shadowGeom[i].surfaceCount);
				write.dump_array(asset->shadowGeom[i].smodelIndex, asset->shadowGeom[i].smodelCount);
			}
		}
		write.dump_array(asset->shadowGeomOptimized, asset->primaryLightCount);
		if (asset->shadowGeomOptimized)
		{
			for (unsigned int i = 0; i < asset->primaryLightCount; i++)
			{
				write.dump_array(asset->shadowGeomOptimized[i].sortedSurfIndex, asset->shadowGeomOptimized[i].surfaceCount);
				write.dump_array(asset->shadowGeomOptimized[i].smodelIndex, asset->shadowGeomOptimized[i].smodelCount);
			}
		}

		write.dump_array(asset->lightRegion, asset->primaryLightCount);
		for (unsigned int i = 0; i < asset->primaryLightCount; i++)
		{
			write.dump_array(asset->lightRegion[i].hulls, asset->lightRegion[i].hullCount);
			for (unsigned int j = 0; j < asset->lightRegion[i].hullCount; j++)
			{
				write.dump_array(asset->lightRegion[i].hulls[j].axis, asset->lightRegion[i].hulls[j].axisCount);
			}
		}

		for (auto i = 0; i < 4; i++)
		{
			write.dump_array(asset->dpvs.smodelVisData[i], asset->dpvs.smodelVisDataCount);
		}

		for (auto i = 0; i < 27; i++)
		{
			write.dump_array(asset->dpvs.smodelUnknownVisData[i], asset->dpvs.smodelVisDataCount);
		}

		for (auto i = 0; i < 4; i++)
		{
			write.dump_array(asset->dpvs.surfaceVisData[i], asset->dpvs.surfaceVisDataCount);
		}

		for (auto i = 0; i < 27; i++)
		{
			write.dump_array(asset->dpvs.surfaceUnknownVisData[i], asset->dpvs.surfaceVisDataCount);
		}

		for (auto i = 0; i < 4; i++)
		{
			write.dump_array(asset->dpvs.smodelUmbraVisData[i], asset->dpvs.smodelVisDataCount);
		}

		for (auto i = 0; i < 4; i++)
		{
			write.dump_array(asset->dpvs.surfaceUmbraVisData[i], asset->dpvs.surfaceVisDataCount);
		}

		write.dump_array(asset->dpvs.unknownSModelVisData1, asset->dpvs.smodelVisDataCount);
		write.dump_array(asset->dpvs.unknownSModelVisData2, 2 * asset->dpvs.smodelVisDataCount);

		write.dump_array(asset->dpvs.lodData, asset->dpvs.smodelCount + 1);
		write.dump_array(asset->dpvs.sortedSurfIndex, asset->dpvs.staticSurfaceCount);

		write.dump_array(asset->dpvs.smodelInsts, asset->dpvs.smodelCount);
		write.dump_array(asset->dpvs.surfaces, asset->surfaceCount);
		for (unsigned int i = 0; i < asset->surfaceCount; i++)
		{
			write.dump_asset(asset->dpvs.surfaces[i].material);
		}
		write.dump_array(asset->dpvs.surfacesBounds, asset->surfaceCount);
		write.dump_array(asset->dpvs.smodelDrawInsts, asset->dpvs.smodelCount);
		for (unsigned int i = 0; i < asset->dpvs.smodelCount; i++)
		{
			write.dump_asset(asset->dpvs.smodelDrawInsts[i].model);
		}
		write.dump_array(asset->dpvs.smodelLighting, asset->dpvs.smodelCount);
		for (unsigned int i = 0; i < asset->dpvs.smodelCount; i++)
		{
			auto flags = asset->dpvs.smodelDrawInsts[i].flags;
			if ((flags & 0x180) != 0 && (flags & 0x80) != 0 && asset->dpvs.smodelLighting[i].vertexLightingInfo.lightingValues)
			{
				write.dump_array(asset->dpvs.smodelLighting[i].vertexLightingInfo.lightingValues, asset->dpvs.smodelLighting[i].vertexLightingInfo.numLightingValues);
			}
		}

		write.dump_array(asset->dpvs.subdivVertexLighting, asset->dpvs.subdivVertexLightingInfoCount);

		write.dump_array(asset->dpvs.surfaceMaterials, asset->surfaceCount);
		write.dump_array(asset->dpvs.surfaceCastsSunShadow, asset->dpvs.surfaceVisDataCount);
		write.dump_array(asset->dpvs.surfaceCastsSunShadowOpt, asset->dpvs.sunShadowOptCount * asset->dpvs.sunSurfVisDataCount);

		write.dump_array(asset->dpvs.surfaceDeptAndSurf, asset->dpvs.staticSurfaceCount);

		write.dump_array(asset->dpvs.constantBuffersLit, asset->dpvs.smodelCount);
		for (unsigned int i = 0; i < asset->dpvs.smodelCount; i++)
		{
			write.dump_raw(asset->dpvs.constantBuffersLit[i], 1);
		}
		write.dump_array(asset->dpvs.constantBuffersAmbient, asset->dpvs.smodelCount);
		for (unsigned int i = 0; i < asset->dpvs.smodelCount; i++)
		{
			write.dump_raw(asset->dpvs.constantBuffersAmbient[i], 1);
		}

		write.dump_array(asset->dpvsDyn.dynEntCellBits[0],
			asset->dpvsDyn.dynEntClientWordCount[0] * asset->dpvsPlanes.cellCount);
		write.dump_array(asset->dpvsDyn.dynEntCellBits[1],
			asset->dpvsDyn.dynEntClientWordCount[1] * asset->dpvsPlanes.cellCount);

		write.dump_array(asset->dpvsDyn.dynEntVisData[0][0], asset->dpvsDyn.dynEntClientWordCount[0] * 32);
		write.dump_array(asset->dpvsDyn.dynEntVisData[0][1], asset->dpvsDyn.dynEntClientWordCount[0] * 32);
		write.dump_array(asset->dpvsDyn.dynEntVisData[0][2], asset->dpvsDyn.dynEntClientWordCount[0] * 32);

		write.dump_array(asset->dpvsDyn.dynEntVisData[1][0], asset->dpvsDyn.dynEntClientWordCount[1] * 32);
		write.dump_array(asset->dpvsDyn.dynEntVisData[1][1], asset->dpvsDyn.dynEntClientWordCount[1] * 32);
		write.dump_array(asset->dpvsDyn.dynEntVisData[1][2], asset->dpvsDyn.dynEntClientWordCount[1] * 32);

		write.dump_array(asset->dpvsDyn.dynEntVisData[0][3], asset->dpvsDyn.dynEntClientWordCount[0] * 32);
		write.dump_array(asset->dpvsDyn.dynEntVisData[1][3], asset->dpvsDyn.dynEntClientWordCount[1] * 32);

		write.dump_array(asset->heroOnlyLights, asset->heroOnlyLightCount);

		write.dump_array(asset->umbraTomeData, asset->umbraTomeSize);

		write.dump_array(asset->mdaoVolumes, asset->mdaoVolumesCount);
		for (unsigned int i = 0; i < asset->mdaoVolumesCount; i++)
		{
			write.dump_asset(asset->mdaoVolumes[i].volumeData);
		}

		write.dump_string(asset->buildInfo.args0);
		write.dump_string(asset->buildInfo.args1);
		write.dump_string(asset->buildInfo.buildStartTime);
		write.dump_string(asset->buildInfo.buildEndTime);

		write.close();
	}
}
