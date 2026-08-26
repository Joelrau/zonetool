#include "stdafx.hpp"

namespace ZoneTool::IW7
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

		write.dump_array(asset->cellTransientInfos, asset->dpvsPlanes.cellCount);

		write.dump_array(asset->cells, asset->dpvsPlanes.cellCount);
		for (int i = 0; i < asset->dpvsPlanes.cellCount; i++)
		{
			write.dump_array(asset->cells[i].portals, asset->cells[i].portalCount);
			for (int j = 0; j < asset->cells[i].portalCount; j++)
			{
				write.dump_array(asset->cells[i].portals[j].vertices, asset->cells[i].portals[j].vertexCount);
			}
		}

		write.dump_array(asset->draw.reflectionProbeData.reflectionProbes, asset->draw.reflectionProbeData.reflectionProbeCount);
		for (unsigned int i = 0; i < asset->draw.reflectionProbeData.reflectionProbeCount; i++)
		{
			write.dump_string(asset->draw.reflectionProbeData.reflectionProbes[i].livePath);
			write.dump_array(asset->draw.reflectionProbeData.reflectionProbes[i].probeInstances, asset->draw.reflectionProbeData.reflectionProbes[i].probeInstanceCount);
		}

		write.dump_asset(asset->draw.reflectionProbeData.reflectionProbeArrayImage);
		write.dump_array(asset->draw.reflectionProbeData.probeRelightingData, asset->draw.reflectionProbeData.probeRelightingCount);
		write.dump_array(asset->draw.reflectionProbeData.reflectionProbeGBufferImages, asset->draw.reflectionProbeData.reflectionProbeGBufferImageCount);
		for (unsigned int i = 0; i < asset->draw.reflectionProbeData.reflectionProbeGBufferImageCount; i++)
		{
			write.dump_asset(asset->draw.reflectionProbeData.reflectionProbeGBufferImages[i]);
		}
		write.dump_array(asset->draw.reflectionProbeData.reflectionProbeInstances, asset->draw.reflectionProbeData.reflectionProbeInstanceCount);
		for (unsigned int i = 0; i < asset->draw.reflectionProbeData.reflectionProbeInstanceCount; i++)
		{
			write.dump_string(asset->draw.reflectionProbeData.reflectionProbeInstances[i].livePath);
			write.dump_string(asset->draw.reflectionProbeData.reflectionProbeInstances[i].livePath2);
		}
		write.dump_array(asset->draw.reflectionProbeData.reflectionProbeLightgridSampleData, asset->draw.reflectionProbeData.reflectionProbeCount);

		write.dump_array(asset->draw.lightmapReindexData.reindexElement, asset->draw.lightmapReindexData.reindexCount);
		write.dump_array(asset->draw.lightmapReindexData.packedLightmap, asset->draw.lightmapReindexData.packedLightmapCount);

		write.dump_asset(asset->draw.iesLookupTexture);
		write.dump_array(asset->draw.decalVolumeCollections, asset->draw.decalVolumeCollectionCount);
		write.dump_asset(asset->draw.lightmapOverridePrimary);
		write.dump_asset(asset->draw.lightmapOverrideSecondary);
		write.dump_array(asset->draw.lightMaps, asset->draw.lightMapCount);
		for (unsigned int i = 0; i < asset->draw.lightMapCount; i++)
		{
			write.dump_asset(asset->draw.lightMaps[i]);
		}

		for (auto i = 0; i < 32; i++)
		{
			write.dump_asset(asset->draw.transientZones[i]);
		}

		write.dump_array(asset->draw.indices, asset->draw.indexCount);

		write.dump_array(asset->draw.volumetrics.volumetrics, asset->draw.volumetrics.volumetricCount);
		for (unsigned int i = 0; i < asset->draw.volumetrics.volumetricCount; i++)
		{
			write.dump_string(asset->draw.volumetrics.volumetrics[i].livePath);
			for (auto m = 0; m < 4; m++)
			{
				write.dump_asset(asset->draw.volumetrics.volumetrics[i].masks[m].image);
			}
		}

		write.dump_array(asset->lightGrid.stageLightingContrastGain, asset->lightGrid.stageCount);
		write.dump_array(asset->lightGrid.paletteEntryAddress, asset->lightGrid.paletteEntryCount);
		write.dump_array(asset->lightGrid.paletteBitstream, asset->lightGrid.paletteBitstreamSize);

		write.dump_array(asset->lightGrid.tree.p_nodeTable, asset->lightGrid.tree.nodeCount);
		write.dump_array(asset->lightGrid.tree.p_leafTable, asset->lightGrid.tree.leafTableSize);

		write.dump_array(asset->lightGrid.probeData.gpuVisibleProbePositions, asset->lightGrid.probeData.gpuVisibleProbesCount);
		write.dump_array(asset->lightGrid.probeData.gpuVisibleProbesData, (asset->lightGrid.probeData.gpuVisibleProbesCount + 0x2000));

		write.dump_array(asset->lightGrid.probeData.probes, asset->lightGrid.probeData.probeCount);

		write.dump_array(asset->lightGrid.probeData.probePositions, asset->lightGrid.probeData.probeCount);

		write.dump_array(asset->lightGrid.probeData.zones, asset->lightGrid.probeData.zoneCount);

		write.dump_array(asset->lightGrid.probeData.tetrahedrons, asset->lightGrid.probeData.tetrahedronCount);

		write.dump_array(asset->lightGrid.probeData.tetrahedronNeighbors, asset->lightGrid.probeData.tetrahedronCount);

		write.dump_array(asset->lightGrid.probeData.tetrahedronVisibility, asset->lightGrid.probeData.tetrahedronCountVisible);

		write.dump_array(asset->lightGrid.probeData.voxelStartTetrahedron, asset->lightGrid.probeData.voxelStartTetrahedronCount);

		write.dump_array(asset->frustumLights, asset->primaryLightCount);
		if (asset->frustumLights)
		{
			for (unsigned int i = 0; i < asset->primaryLightCount; i++)
			{
				write.dump_array(asset->frustumLights[i].indices, asset->frustumLights[i].indexCount);
				write.dump_array(asset->frustumLights[i].vertices, 32 * asset->frustumLights[i].vertexCount);
			}
		}

		write.dump_array(asset->lightViewFrustums, asset->primaryLightCount);
		if (asset->lightViewFrustums)
		{
			for (unsigned int i = 0; i < asset->primaryLightCount; i++)
			{
				write.dump_array(asset->lightViewFrustums[i].planes, asset->lightViewFrustums[i].planeCount);
				write.dump_array(asset->lightViewFrustums[i].indices, asset->lightViewFrustums[i].indexCount);
				write.dump_array(asset->lightViewFrustums[i].vertices, asset->lightViewFrustums[i].vertexCount);
			}
		}

		write.dump_array(asset->voxelTree, asset->voxelTreeCount);
		for (int i = 0; i < asset->voxelTreeCount; i++)
		{
			write.dump_single(asset->voxelTree[i].voxelTreeHeader);
			write.dump_array(asset->voxelTree[i].voxelTopDownViewNodeArray, asset->voxelTree[i].voxelTopDownViewNodeCount);
			write.dump_array(asset->voxelTree[i].voxelInternalNodeArray, asset->voxelTree[i].voxelInternalNodeCount);
			write.dump_array(asset->voxelTree[i].voxelLeafNodeArray, asset->voxelTree[i].voxelLeafNodeCount);
			write.dump_array(asset->voxelTree[i].lightListArray, asset->voxelTree[i].lightListArraySize);
		}

		write.dump_array(asset->heightfields, asset->heightfieldCount);
		for (int i = 0; i < asset->heightfieldCount; i++)
		{
			write.dump_asset(asset->heightfields[i].image);
		}

		write.dump_array(asset->unk01.unk03, asset->unk01.unk03Count);

		write.dump_array(asset->models, asset->modelCount);

		write.dump_array(asset->materialMemory, asset->materialMemoryCount);
		for (int i = 0; i < asset->materialMemoryCount; i++)
		{
			write.dump_asset(asset->materialMemory[i].material);
		}

		write.dump_asset(asset->sun.spriteMaterial);
		write.dump_asset(asset->sun.flareMaterial);

		write.dump_asset(asset->outdoorImage);
		write.dump_asset(asset->dustMaterial);

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

		write.dump_array(asset->lightAABB.nodeArray, asset->lightAABB.nodeCount);
		write.dump_array(asset->lightAABB.lightArray, asset->lightAABB.lightCount);

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
			write.dump_array(asset->dpvs.smodelDrawInsts[i].vertexLightingInfo.lightingValues,
				asset->dpvs.smodelDrawInsts[i].vertexLightingInfo.numLightingValues);
		}

		write.dump_array(asset->dpvs.surfaceCastsSunShadow, asset->dpvs.surfaceVisDataCount);

		write.dump_array(asset->dpvs.sortedSmodelIndices, asset->dpvs.smodelCount);

		write.dump_array(asset->heroOnlyLights, asset->heroOnlyLightCount);

		write.dump_array(asset->umbraGates, asset->numUmbraGates);
		write.dump_array(asset->umbraTomeData, asset->umbraTomeSize);

		write.dump_array(asset->umbraGates2, asset->numUmbraGates2);
		write.dump_array(asset->umbraTomeData2, asset->umbraTomeSize2);

		write.dump_array(asset->umbraUnkData, asset->umbraUnkSize);

		write.close();
	}
}