// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"
#include "../IW4/Assets/GfxWorld.hpp"

namespace ZoneTool
{
	namespace IW3
	{
		void IGfxWorld::dump(GfxWorld* world)
		{
			//ZoneMemory mem(20 * 1024 * 1024);

			//if (!world) return;

			//IW4::GfxSky sky;
			//IW4::GfxWorld map;
			//ZeroMemory(&sky, sizeof(sky));
			//ZeroMemory(&map, sizeof(map));

			//map.name = world->name;
			//map.baseName = world->baseName;
			//map.planeCount = world->planeCount;
			//map.nodeCount = world->nodeCount;
			//map.indexCount = world->surfaceCount;

			//map.skyCount = 1;
			//map.skies = &sky;

			//sky.skyImage = (IW4::GfxImage*)world->skyImage;
			//sky.skySamplerState = world->skySamplerState & 0xFF;
			//sky.skyStartSurfs = (uint32_t*)world->skyStartSurfs;
			//sky.skySurfCount = world->skySurfCount;

			//map.sunPrimaryLightIndex = world->sunPrimaryLightIndex;
			//map.primaryLightCount = world->primaryLightCount;

			//// map.dpvsPlanes = world->dpvsPlanes;
			//memcpy(&map.dpvsPlanes, &world->dpvsPlanes, sizeof world->dpvsPlanes);

			//// AABBTree data is stored as part of the cells.
			//// However, in IW4 it's not, so we have to extract the data
			//if (world->cells)
			//{
			//	map.aabbTreeCounts = mem.Alloc<IW4::GfxCellTreeCount>(world->dpvsPlanes.cellCount);
			//	map.aabbTree = mem.Alloc<IW4::GfxCellTree>(world->dpvsPlanes.cellCount);
			//	map.cells = mem.Alloc<IW4::GfxCell>(world->dpvsPlanes.cellCount);

			//	for (int i = 0; i < world->dpvsPlanes.cellCount; ++i)
			//	{
			//		map.aabbTreeCounts[i].aabbTreeCount = world->cells[i].aabbTreeCount;

			//		map.cells[i].bounds.compute(world->cells[i].mins, world->cells[i].maxs); // Verified
			//		map.cells[i].portalCount = world->cells[i].portalCount;
			//		map.cells[i].reflectionProbeCount = world->cells[i].reflectionProbeCount;
			//		map.cells[i].reflectionProbes = world->cells[i].reflectionProbes;

			//		if (world->cells[i].aabbTree)
			//		{
			//			map.aabbTree[i].aabbtree = mem.Alloc<IW4::GfxAabbTree>(world->cells[i].aabbTreeCount);
			//			std::memcpy(map.aabbTree[i].aabbtree, world->cells[i].aabbTree, sizeof(IW4::GfxAabbTree) * world->cells[i].aabbTreeCount);

			//			for (int j = 0; j < world->cells[i].aabbTreeCount; ++j)
			//			{
			//				static_assert(sizeof IW4::GfxAabbTree == sizeof IW3::GfxAabbTree, "Size mismatch");
			//				map.aabbTree[i].aabbtree[j].bounds.compute(world->cells[i].aabbTree[j].mins, world->cells[i].aabbTree[j].maxs); // Verified
			//			}
			//		}

			//		if (world->cells[i].portals)
			//		{
			//			map.cells[i].portals = mem.Alloc<IW4::GfxPortal>(world->cells[i].portalCount);

			//			// Map all portals, so we have them ready for the next loop (might be unnecessary, as they are mapped at runtime)
			//			std::unordered_map<IW3::GfxPortal*, IW4::GfxPortal*> portalMap = { { nullptr, nullptr } };
			//			for (int j = 0; j < world->cells[i].portalCount; ++j)
			//			{
			//				portalMap[&world->cells[i].portals[j]] = &map.cells[i].portals[j];
			//			}

			//			for (int j = 0; j < world->cells[i].portalCount; ++j)
			//			{
			//				IW3::GfxPortal* portal = &world->cells[i].portals[j];
			//				IW4::GfxPortal* destPortal = &map.cells[i].portals[j];

			//				destPortal->cellIndex = static_cast<unsigned short>(portal->cell - world->cells);
			//				if (destPortal->cellIndex >= static_cast<unsigned short>(world->dpvsPlanes.cellCount))
			//				{
			//					ZONETOOL_FATAL("Unable to calculate cell index. This should not happen!\n");
			//					destPortal->cellIndex = 0;
			//				}

			//				destPortal->vertices = portal->vertices;
			//				destPortal->vertexCount = portal->vertexCount;

			//				destPortal->writable.isQueued = portal->writable.isQueued;
			//				destPortal->writable.isAncestor = portal->writable.isAncestor;
			//				destPortal->writable.recursionDepth = portal->writable.recursionDepth;
			//				destPortal->writable.hullPointCount = portal->writable.hullPointCount;
			//				destPortal->writable.hullPoints = portal->writable.hullPoints;

			//				if (portalMap.find(portal->writable.queuedParent) != portalMap.end())
			//				{
			//					destPortal->writable.queuedParent = portalMap[portal->writable.queuedParent];
			//				}
			//				else
			//				{
			//					if (portal->writable.queuedParent) ZONETOOL_ERROR("Unmapped portal. This shouldn't happen. Nulling it...\n");
			//					destPortal->writable.queuedParent = nullptr;
			//				}

			//				std::memcpy(destPortal->plane.coeffs, portal->plane.coeffs, sizeof(destPortal->plane.coeffs));
			//				std::memcpy(destPortal->hullAxis, portal->hullAxis, sizeof(destPortal->hullAxis));
			//			}
			//		}
			//	}
			//}

			//map.worldDraw.reflectionProbeCount = world->reflectionProbeCount;
			//map.worldDraw.reflectionProbeTextures = (IW4::GfxTexture*)world->reflectionProbeTextures;
			//map.worldDraw.lightmapCount = world->lightmapCount;
			//map.worldDraw.lightmaps = (IW4::GfxLightmapArray*)world->lightmaps;
			//map.worldDraw.lightmapPrimaryTextures = (IW4::GfxTexture*)world->lightmapPrimaryTextures;
			//map.worldDraw.lightmapSecondaryTextures = (IW4::GfxTexture*)world->lightmapSecondaryTextures;
			//map.worldDraw.skyImage = (IW4::GfxImage*)world->skyImage;
			//map.worldDraw.outdoorImage = (IW4::GfxImage*)world->outdoorImage;
			//map.worldDraw.vertexCount = world->vertexCount;
			//memcpy(&map.worldDraw.vd, &world->vd, sizeof world->vd);
			//map.worldDraw.vertexLayerDataSize = world->vertexLayerDataSize;
			//memcpy(&map.worldDraw.vld, &world->vld, sizeof world->vld);
			//map.worldDraw.indexCount = world->indexCount;
			//map.worldDraw.indices = world->indices;

			//// Split reflection images and probes
			//if (world->reflectionProbes)
			//{
			//	map.worldDraw.reflectionImages = mem.Alloc<IW4::GfxImage*>(world->reflectionProbeCount);
			//	map.worldDraw.reflectionProbes = mem.Alloc<IW4::GfxReflectionProbe>(world->reflectionProbeCount);

			//	for (unsigned int i = 0; i < world->reflectionProbeCount; ++i)
			//	{
			//		map.worldDraw.reflectionImages[i] = (IW4::GfxImage*)world->reflectionProbes[i].reflectionImage;

			//		std::memcpy(map.worldDraw.reflectionProbes[i].offset, world->reflectionProbes[i].origin, sizeof(map.worldDraw.reflectionProbes[i].offset));
			//	}
			//}

			//memcpy(&map.lightGrid, &world->lightGrid, sizeof world->lightGrid);
			//map.modelCount = world->modelCount;

			//if (world->models)
			//{
			//	map.models = mem.Alloc<IW4::GfxBrushModel>(world->modelCount);

			//	for (int i = 0; i < world->modelCount; ++i)
			//	{
			//		map.models[i].writable.bounds.compute(world->models[i].writable.mins, world->models[i].writable.maxs); // Irrelevant, runtime data
			//		map.models[i].bounds.compute(world->models[i].bounds[0], world->models[i].bounds[1]); // Verified

			//		float* halfSize = map.models[i].bounds.halfSize;
			//		map.models[i].radius = std::sqrt(std::pow(halfSize[0], 2) + std::pow(halfSize[1], 2) + std::pow(halfSize[2], 2));

			//		map.models[i].surfaceCount = world->models[i].surfaceCount;
			//		map.models[i].startSurfIndex = world->models[i].startSurfIndex;
			//		map.models[i].surfaceCountNoDecal = world->models[i].surfaceCountNoDecal;
			//	}
			//}

			//map.bounds.compute(world->mins, world->maxs);

			//map.checksum = world->checksum;
			//map.materialMemoryCount = world->materialMemoryCount;
			//map.materialMemory = (IW4::MaterialMemory*)world->materialMemory;
			//memcpy(&map.sun, &world->sun, sizeof world->sun);

			//std::memcpy(map.outdoorLookupMatrix, world->outdoorLookupMatrix, sizeof(map.outdoorLookupMatrix));
			//map.outdoorImage = (IW4::GfxImage*)world->outdoorImage;

			//map.cellCasterBits = world->cellCasterBits;
			//map.cellHasSunLitSurfsBits = reinterpret_cast<unsigned int*>(1); // This mustn't be null!

			//map.sceneDynModel = world->sceneDynModel;
			//map.sceneDynBrush = world->sceneDynBrush;

			//map.primaryLightEntityShadowVis = world->primaryLightEntityShadowVis;
			//map.primaryLightDynEntShadowVis[0] = world->primaryLightDynEntShadowVis[0];
			//map.primaryLightDynEntShadowVis[1] = world->primaryLightDynEntShadowVis[1];
			//map.nonSunPrimaryLightForModelDynEnt = world->nonSunPrimaryLightForModelDynEnt;

			//map.shadowGeom = world->shadowGeom;
			//map.lightRegion = world->lightRegion;

			//map.dpvs.smodelCount = world->dpvs.smodelCount;
			//map.dpvs.staticSurfaceCount = world->dpvs.staticSurfaceCount;
			//map.dpvs.staticSurfaceCountNoDecal = world->dpvs.staticSurfaceCountNoDecal;

			//// Not sure if correct
			//// update: slightly more sure but not much lol
			//map.dpvs.litOpaqueSurfsBegin = world->dpvs.litSurfsBegin;
			//map.dpvs.litOpaqueSurfsEnd = world->dpvs.decalSurfsEnd;

			//// these don't exist in iw3 so skip
			//map.dpvs.litTransSurfsBegin = world->dpvs.decalSurfsEnd;
			//map.dpvs.litTransSurfsEnd = world->dpvs.decalSurfsEnd;

			//// Skip those as well
			//map.dpvs.shadowCasterSurfsBegin = world->dpvs.decalSurfsEnd;
			//map.dpvs.shadowCasterSurfsEnd = world->dpvs.decalSurfsEnd;

			//map.dpvs.emissiveSurfsBegin = world->dpvs.emissiveSurfsBegin;
			//map.dpvs.emissiveSurfsEnd = world->dpvs.emissiveSurfsEnd;
			//map.dpvs.smodelVisDataCount = world->dpvs.smodelVisDataCount;
			//map.dpvs.surfaceVisDataCount = world->dpvs.surfaceVisDataCount;

			//std::memcpy(map.dpvs.smodelVisData, world->dpvs.smodelVisData, sizeof(map.dpvs.smodelVisData));
			//std::memcpy(map.dpvs.surfaceVisData, world->dpvs.surfaceVisData, sizeof(map.dpvs.surfaceVisData));

			//map.dpvs.sortedSurfIndex = world->dpvs.sortedSurfIndex;

			//if (world->dpvs.smodelInsts)
			//{
			//	map.dpvs.smodelInsts = mem.Alloc<IW4::GfxStaticModelInst>(world->dpvs.smodelCount);

			//	for (unsigned int i = 0; i < world->dpvs.smodelCount; ++i)
			//	{
			//		map.dpvs.smodelInsts[i].bounds.compute(world->dpvs.smodelInsts[i].mins, world->dpvs.smodelInsts[i].maxs); // Verified

			//		// I guess the sun is always a good lighting source ;)
			//		map.dpvs.smodelInsts[i].lightingOrigin[0] = world->sun.sunFxPosition[0];
			//		map.dpvs.smodelInsts[i].lightingOrigin[1] = world->sun.sunFxPosition[1];
			//		map.dpvs.smodelInsts[i].lightingOrigin[2] = world->sun.sunFxPosition[2];
			//	}
			//}

			//if (world->dpvs.surfaces)
			//{
			//	map.dpvs.surfaces = mem.Alloc<IW4::GfxSurface>(world->surfaceCount);
			//	map.dpvs.surfacesBounds = mem.Alloc<IW4::GfxSurfaceBounds>(world->surfaceCount);

			//	for (int i = 0; i < world->surfaceCount; ++i)
			//	{
			//		map.dpvs.surfaces[i].tris = world->dpvs.surfaces[i].tris;
			//		map.dpvs.surfaces[i].material = world->dpvs.surfaces[i].material;
			//		map.dpvs.surfaces[i].lightmapIndex = world->dpvs.surfaces[i].lightmapIndex;
			//		map.dpvs.surfaces[i].reflectionProbeIndex = world->dpvs.surfaces[i].reflectionProbeIndex;
			//		map.dpvs.surfaces[i].primaryLightIndex = world->dpvs.surfaces[i].primaryLightIndex;
			//		map.dpvs.surfaces[i].flags = world->dpvs.surfaces[i].flags;

			//		map.dpvs.surfacesBounds[i].bounds.compute(world->dpvs.surfaces[i].bounds[0], world->dpvs.surfaces[i].bounds[1]); // Verified
			//	}
			//}

			//if (world->dpvs.smodelDrawInsts)
			//{
			//	map.dpvs.smodelDrawInsts = mem.Alloc<IW4::GfxStaticModelDrawInst>(world->dpvs.smodelCount);

			//	for (unsigned int i = 0; i < world->dpvs.smodelCount; ++i)
			//	{
			//		std::memcpy(map.dpvs.smodelDrawInsts[i].placement.origin, world->dpvs.smodelDrawInsts[i].placement.origin, sizeof(map.dpvs.smodelDrawInsts[i].placement.origin));
			//		std::memcpy(map.dpvs.smodelDrawInsts[i].placement.axis, world->dpvs.smodelDrawInsts[i].placement.axis, sizeof(map.dpvs.smodelDrawInsts[i].placement.axis));

			//		std::memcpy(map.dpvs.smodelDrawInsts[i].cacheId, world->dpvs.smodelDrawInsts[i].smodelCacheIndex, sizeof(map.dpvs.smodelDrawInsts[i].cacheId));

			//		map.dpvs.smodelDrawInsts[i].placement.scale = world->dpvs.smodelDrawInsts[i].placement.scale;
			//		map.dpvs.smodelDrawInsts[i].model = world->dpvs.smodelDrawInsts[i].model;
			//		map.dpvs.smodelDrawInsts[i].cullDist = static_cast<unsigned short>(world->dpvs.smodelDrawInsts[i].cullDist);
			//		map.dpvs.smodelDrawInsts[i].reflectionProbeIndex = world->dpvs.smodelDrawInsts[i].reflectionProbeIndex;
			//		map.dpvs.smodelDrawInsts[i].primaryLightIndex = world->dpvs.smodelDrawInsts[i].primaryLightIndex;
			//		map.dpvs.smodelDrawInsts[i].lightingHandle = world->dpvs.smodelDrawInsts[i].lightingHandle;
			//		map.dpvs.smodelDrawInsts[i].flags = world->dpvs.smodelDrawInsts[i].flags;

			//		// This has been moved
			//		if (world->dpvs.smodelInsts) map.dpvs.smodelDrawInsts[i].groundLighting = world->dpvs.smodelInsts[i].groundLighting;
			//	}
			//}

			//map.dpvs.surfaceMaterials = (IW4::GfxDrawSurf*)world->dpvs.surfaceMaterials;
			//map.dpvs.surfaceCastsSunShadow = world->dpvs.surfaceCastsSunShadow;
			//map.dpvs.usageCount = world->dpvs.usageCount;

			//memcpy(&map.dpvsDyn, &world->dpvsDyn, sizeof world->dpvsDyn);

			//// Should we set that to true? :O
			//map.fogTypesAllowed = 3; // iw4_credits has 0x3

			//map.sortKeyLitDecal = 0x6;
			//map.sortKeyEffectDecal = 0x27;
			//map.sortKeyEffectAuto = 0x30;
			//map.sortKeyDistortion = 0x2b;

			//int baseIndex = 0;
			//map.worldDraw.indices = mem.Alloc<unsigned short>(map.worldDraw.indexCount);
			//for (unsigned int i = 0; i < map.indexCount; ++i)
			//{
			//	std::memcpy(&map.worldDraw.indices[baseIndex], &world->indices[map.dpvs.surfaces[i].tris.baseIndex], map.dpvs.surfaces[i].tris.triCount * 6);
			//	map.dpvs.surfaces[i].tris.baseIndex = baseIndex;
			//	baseIndex += map.dpvs.surfaces[i].tris.triCount * 3;
			//}

			//if (baseIndex != map.worldDraw.indexCount)
			//{
			//	ZONETOOL_WARNING("Warning: Didn't sort all indicies for worldDraw");
			//}

			//// Specify that it's a custom map
			//map.checksum = 0xDEADBEEF;

			//IW4::IGfxWorld::dump(&map);
		}
	}
}
