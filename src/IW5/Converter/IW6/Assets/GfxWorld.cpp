#include "stdafx.hpp"
#include "../Include.hpp"

#include "GfxWorld.hpp"

#include "X64/Utils/Umbra/umbra.hpp"
#include "X64/Utils/Utils.hpp"

namespace ZoneTool::IW5
{
	namespace IW6Converter
	{
		IW6::GfxWorld* GenerateIW6GfxWorld(GfxWorld* asset, allocator& mem)
		{
			// allocate IW6 GfxWorld structure
			const auto iw6_asset = mem.allocate<IW6::GfxWorld>();

			iw6_asset->name = asset->name;
			iw6_asset->baseName = asset->baseName;

			iw6_asset->bspVersion = 74;

			iw6_asset->planeCount = asset->planeCount;
			iw6_asset->nodeCount = asset->nodeCount;
			iw6_asset->surfaceCount = asset->surfaceCount;
			iw6_asset->skyCount = asset->skyCount;

			iw6_asset->skies = mem.allocate<IW6::GfxSky>(iw6_asset->skyCount);
			for (int i = 0; i < iw6_asset->skyCount; i++)
			{
				iw6_asset->skies[i].skySurfCount = asset->skies[i].skySurfCount;
				iw6_asset->skies[i].skyStartSurfs = reinterpret_cast<int*>(asset->skies[i].skyStartSurfs);
				if (iw6_asset->skies[i].skyImage)
				{
					iw6_asset->skies[i].skyImage = mem.allocate<IW6::GfxImage>();
					iw6_asset->skies[i].skyImage->name = asset->skies->skyImage->name;
				}
				else
				{
					iw6_asset->skies[i].skyImage = nullptr;
				}
				iw6_asset->skies[i].skySamplerState = asset->skies[i].skySamplerState;
			}

			iw6_asset->lastSunPrimaryLightIndex = asset->lastSunPrimaryLightIndex;
			iw6_asset->primaryLightCount = asset->primaryLightCount;
			iw6_asset->primaryLightEnvCount = asset->primaryLightCount + 1;
			iw6_asset->sortKeyLitDecal = 6;
			iw6_asset->sortKeyEffectDecal = 39;
			iw6_asset->sortKeyTopDecal = 16;
			iw6_asset->sortKeyEffectAuto = 48;
			iw6_asset->sortKeyDistortion = 43;

			iw6_asset->dpvsPlanes.cellCount = asset->dpvsPlanes.cellCount;
			REINTERPRET_CAST_SAFE(iw6_asset->dpvsPlanes.planes, asset->dpvsPlanes.planes);
			REINTERPRET_CAST_SAFE(iw6_asset->dpvsPlanes.nodes, asset->dpvsPlanes.nodes);

			iw6_asset->dpvsPlanes.sceneEntCellBits = mem.allocate<unsigned int>(asset->dpvsPlanes.cellCount << 9);
			for (int i = 0; i < asset->dpvsPlanes.cellCount << 9; i++)
			{
				iw6_asset->dpvsPlanes.sceneEntCellBits[i] = asset->dpvsPlanes.sceneEntCellBits[i];
			}

			iw6_asset->aabbTreeCounts = mem.allocate<IW6::GfxCellTreeCount>(iw6_asset->dpvsPlanes.cellCount);
			iw6_asset->aabbTrees = mem.allocate<IW6::GfxCellTree>(iw6_asset->dpvsPlanes.cellCount);
			for (int i = 0; i < iw6_asset->dpvsPlanes.cellCount; i++)
			{
				iw6_asset->aabbTreeCounts[i].aabbTreeCount = asset->aabbTreeCounts[i].aabbTreeCount;
				iw6_asset->aabbTrees[i].aabbTree = mem.allocate<IW6::GfxAabbTree>(iw6_asset->aabbTreeCounts[i].aabbTreeCount);
				for (int j = 0; j < iw6_asset->aabbTreeCounts[i].aabbTreeCount; j++)
				{
					memcpy(&iw6_asset->aabbTrees[i].aabbTree[j].bounds, &asset->aabbTrees[i].aabbTree[j].bounds, sizeof(float[2][3]));

					iw6_asset->aabbTrees[i].aabbTree[j].startSurfIndex = asset->aabbTrees[i].aabbTree[j].startSurfIndex;
					iw6_asset->aabbTrees[i].aabbTree[j].surfaceCount = asset->aabbTrees[i].aabbTree[j].surfaceCount;

					iw6_asset->aabbTrees[i].aabbTree[j].smodelIndexCount = asset->aabbTrees[i].aabbTree[j].smodelIndexCount;
					REINTERPRET_CAST_SAFE(iw6_asset->aabbTrees[i].aabbTree[j].smodelIndexes, asset->aabbTrees[i].aabbTree[j].smodelIndexes);

					// has some problems?
					//iw6_asset->aabbTrees[i].aabbTree[j].childCount = asset->aabbTrees[i].aabbTree[j].childCount;
					iw6_asset->aabbTrees[i].aabbTree[j].childCount = 0;

					// re-calculate childrenOffset
					auto offset = asset->aabbTrees[i].aabbTree[j].childrenOffset;
					int childrenIndex = offset / sizeof(IW5::GfxAabbTree);
					int childrenOffset = childrenIndex * sizeof(IW6::GfxAabbTree);
					iw6_asset->aabbTrees[i].aabbTree[j].childrenOffset = childrenOffset;
				}
			}

			iw6_asset->cells = mem.allocate<IW6::GfxCell>(iw6_asset->dpvsPlanes.cellCount);
			for (int i = 0; i < iw6_asset->dpvsPlanes.cellCount; i++)
			{
				memcpy(&iw6_asset->cells[i].bounds, &asset->cells[i].bounds, sizeof(float[2][3]));
				iw6_asset->cells[i].portalCount = asset->cells[i].portalCount;

				auto add_portal = [](IW6::GfxPortal* iw6_portal, IW5::GfxPortal* iw5_portal)
				{
					//iw6_portal->writable.isQueued = iw5_portal->writable.isQueued;
					//iw6_portal->writable.isAncestor = iw5_portal->writable.isAncestor;
					//iw6_portal->writable.recursionDepth = iw5_portal->writable.recursionDepth;
					//iw6_portal->writable.hullPointCount = iw5_portal->writable.hullPointCount;
					//iw6_portal->writable.hullPoints = reinterpret_cast<float(*)[2]>(iw5_portal->writable.hullPoints);
					//iw6_portal->writable.queuedParent = add_portal(iw5_portal->writable.queuedParent); // mapped at runtime

					memcpy(&iw6_portal->plane, &iw5_portal->plane, sizeof(float[4]));
					iw6_portal->vertices = reinterpret_cast<float(*)[3]>(iw5_portal->vertices);
					iw6_portal->cellIndex = iw5_portal->cellIndex;
					iw6_portal->closeDistance = 0;
					iw6_portal->vertexCount = iw5_portal->vertexCount;
					memcpy(&iw6_portal->hullAxis, &iw5_portal->hullAxis, sizeof(float[2][3]));
				};
				iw6_asset->cells[i].portals = mem.allocate<IW6::GfxPortal>(iw6_asset->cells[i].portalCount);
				for (int j = 0; j < iw6_asset->cells[i].portalCount; j++)
				{
					add_portal(&iw6_asset->cells[i].portals[j], &asset->cells[i].portals[j]);
				}

				iw6_asset->cells[i].reflectionProbeCount = asset->cells[i].reflectionProbeCount;
				iw6_asset->cells[i].reflectionProbes = reinterpret_cast<unsigned __int8*>(asset->cells[i].reflectionProbes);
				iw6_asset->cells[i].reflectionProbeReferenceCount = asset->cells[i].reflectionProbeReferenceCount;
				iw6_asset->cells[i].reflectionProbeReferences = reinterpret_cast<unsigned __int8*>(asset->cells[i].reflectionProbeReferences);
			}

			iw6_asset->draw.reflectionProbeCount = asset->draw.reflectionProbeCount;
			iw6_asset->draw.reflectionProbes = mem.allocate<IW6::GfxImage* __ptr64>(iw6_asset->draw.reflectionProbeCount);
			iw6_asset->draw.reflectionProbeOrigins = mem.allocate<IW6::GfxReflectionProbe>(iw6_asset->draw.reflectionProbeCount);
			iw6_asset->draw.reflectionProbeTextures = mem.allocate<IW6::GfxTexture>(iw6_asset->draw.reflectionProbeCount);
			for (unsigned int i = 0; i < iw6_asset->draw.reflectionProbeCount; i++)
			{
				iw6_asset->draw.reflectionProbes[i] = mem.allocate<IW6::GfxImage>();
				iw6_asset->draw.reflectionProbes[i]->name = asset->draw.reflectionProbes[i]->name;
				memcpy(&iw6_asset->draw.reflectionProbeOrigins[i].origin, &asset->draw.reflectionProbeOrigins[i].origin, sizeof(float[3]));
				iw6_asset->draw.reflectionProbeOrigins[i].probeVolumeCount = 0;
				iw6_asset->draw.reflectionProbeOrigins[i].probeVolumes = nullptr;
				//memcpy(&iw6_asset->draw.reflectionProbeTextures[i], &asset->draw.reflectionProbeTextures[i].loadDef, 20);
			}
			iw6_asset->draw.reflectionProbeReferenceCount = asset->draw.reflectionProbeReferenceCount;
			iw6_asset->draw.reflectionProbeReferenceOrigins = reinterpret_cast<IW6::GfxReflectionProbeReferenceOrigin*>(
				asset->draw.reflectionProbeReferenceOrigins);
			iw6_asset->draw.reflectionProbeReferences = reinterpret_cast<IW6::GfxReflectionProbeReference*>(
				asset->draw.reflectionProbeReferences);

			iw6_asset->draw.lightmapCount = asset->draw.lightmapCount;
			iw6_asset->draw.lightmaps = mem.allocate<IW6::GfxLightmapArray>(iw6_asset->draw.lightmapCount);
			iw6_asset->draw.lightmapPrimaryTextures = mem.allocate<IW6::GfxTexture>(iw6_asset->draw.lightmapCount);
			iw6_asset->draw.lightmapSecondaryTextures = mem.allocate<IW6::GfxTexture>(iw6_asset->draw.lightmapCount);
			for (int i = 0; i < iw6_asset->draw.lightmapCount; i++)
			{
				iw6_asset->draw.lightmaps[i].primary = mem.allocate<IW6::GfxImage>();
				iw6_asset->draw.lightmaps[i].primary->name = asset->draw.lightmaps[i].primary->name;
				iw6_asset->draw.lightmaps[i].secondary = mem.allocate<IW6::GfxImage>();
				iw6_asset->draw.lightmaps[i].secondary->name = asset->draw.lightmaps[i].secondary->name;

				//memcpy(&iw6_asset->draw.lightmapPrimaryTextures[i], &asset->draw.lightmapPrimaryTextures[i].loadDef, 20);
				//memcpy(&iw6_asset->draw.lightmapSecondaryTextures[i], &asset->draw.lightmapSecondaryTextures[i].loadDef, 20);
			}
			if (asset->draw.lightmapOverridePrimary)
			{
				iw6_asset->draw.lightmapOverridePrimary = mem.allocate<IW6::GfxImage>();
				iw6_asset->draw.lightmapOverridePrimary->name = asset->draw.lightmapOverridePrimary->name;
			}
			else
			{
				iw6_asset->draw.lightmapOverridePrimary = nullptr;
			}

			if (asset->draw.lightmapOverrideSecondary)
			{
				iw6_asset->draw.lightmapOverrideSecondary = mem.allocate<IW6::GfxImage>();
				iw6_asset->draw.lightmapOverrideSecondary->name = asset->draw.lightmapOverrideSecondary->name;
			}
			else
			{
				iw6_asset->draw.lightmapOverrideSecondary = nullptr;
			}

			iw6_asset->draw.trisType = 0; // dunno

			iw6_asset->draw.vertexCount = asset->draw.vertexCount;
			iw6_asset->draw.vd.vertices = mem.allocate<IW6::GfxWorldVertex>(iw6_asset->draw.vertexCount);
			for (unsigned int i = 0; i < iw6_asset->draw.vertexCount; i++)
			{
				memcpy(&iw6_asset->draw.vd.vertices[i], &asset->draw.vd.vertices[i], sizeof(IW5::GfxWorldVertex));

				// re-calculate these...
				float normal_unpacked[3]{ 0.0f, 0.0f, 0.0f };
				PackedVec::Vec3UnpackUnitVec(asset->draw.vd.vertices[i].normal.array, normal_unpacked);

				float tangent_unpacked[3]{ 0.0f, 0.0f, 0.0f };
				PackedVec::Vec3UnpackUnitVec(asset->draw.vd.vertices[i].tangent.array, tangent_unpacked);

				float normal[3] = { normal_unpacked[0], normal_unpacked[1], normal_unpacked[2] };
				float tangent[3] = { tangent_unpacked[0], tangent_unpacked[1], tangent_unpacked[2] };

				float sign = 0.0f;
				if (asset->draw.vd.vertices[i].binormalSign == -1.0f)
				{
					sign = 1.0f;
				}

				iw6_asset->draw.vd.vertices[i].normal.packed = PackedVec::Vec3PackUnitVec(normal);
				iw6_asset->draw.vd.vertices[i].tangent.packed = PackedVec::Vec3PackUnitVec(tangent);

				// correct color : bgra->rgba
				iw6_asset->draw.vd.vertices[i].color.array[0] = asset->draw.vd.vertices[i].color.array[2];
				iw6_asset->draw.vd.vertices[i].color.array[1] = asset->draw.vd.vertices[i].color.array[1];
				iw6_asset->draw.vd.vertices[i].color.array[2] = asset->draw.vd.vertices[i].color.array[0];
				iw6_asset->draw.vd.vertices[i].color.array[3] = asset->draw.vd.vertices[i].color.array[3];
			}
			iw6_asset->draw.vd.worldVb = nullptr;

			iw6_asset->draw.vertexLayerDataSize = asset->draw.vertexLayerDataSize;
			REINTERPRET_CAST_SAFE(iw6_asset->draw.vld.data, asset->draw.vld.data);

			iw6_asset->draw.indexCount = asset->draw.indexCount;
			REINTERPRET_CAST_SAFE(iw6_asset->draw.indices, asset->draw.indices);

			iw6_asset->lightGrid.hasLightRegions = asset->lightGrid.hasLightRegions;
			iw6_asset->lightGrid.useSkyForLowZ = 0;
			iw6_asset->lightGrid.lastSunPrimaryLightIndex = asset->lightGrid.lastSunPrimaryLightIndex;
			memcpy(&iw6_asset->lightGrid.mins, &asset->lightGrid.mins, sizeof(short[3]));
			memcpy(&iw6_asset->lightGrid.maxs, &asset->lightGrid.maxs, sizeof(short[3]));
			iw6_asset->lightGrid.rowAxis = asset->lightGrid.rowAxis;
			iw6_asset->lightGrid.colAxis = asset->lightGrid.colAxis;
			REINTERPRET_CAST_SAFE(iw6_asset->lightGrid.rowDataStart, asset->lightGrid.rowDataStart);
			iw6_asset->lightGrid.rawRowDataSize = asset->lightGrid.rawRowDataSize;
			REINTERPRET_CAST_SAFE(iw6_asset->lightGrid.rawRowData, asset->lightGrid.rawRowData);
			iw6_asset->lightGrid.entryCount = asset->lightGrid.entryCount;
			iw6_asset->lightGrid.entries = mem.allocate<IW6::GfxLightGridEntry>(iw6_asset->lightGrid.entryCount);
			for (unsigned int i = 0; i < iw6_asset->lightGrid.entryCount; i++)
			{
				iw6_asset->lightGrid.entries[i].colorsIndex = asset->lightGrid.entries[i].colorsIndex;
				iw6_asset->lightGrid.entries[i].primaryLightEnvIndex = asset->lightGrid.entries[i].primaryLightIndex;
				iw6_asset->lightGrid.entries[i].unused = 0;
				iw6_asset->lightGrid.entries[i].needsTrace = asset->lightGrid.entries[i].needsTrace;
			}
			iw6_asset->lightGrid.colorCount = asset->lightGrid.colorCount;
			iw6_asset->lightGrid.colors = reinterpret_cast<IW6::GfxLightGridColors*>(asset->lightGrid.colors);

#define EXPERIMENTAL_LIGHTGRID_COLORS
#ifdef EXPERIMENTAL_LIGHTGRID_COLORS
			// --experimental--
			{
				// iw6 mp_character_room
				[[maybe_unused]] const std::uint8_t skyLightGridColors_bytes[] = { 0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63 };
				[[maybe_unused]] const std::uint8_t defaultLightGridColors_bytes[] = { 0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63 };
				[[maybe_unused]] const std::uint8_t p_nodeTable_bytes[] = { 1,0,0,255,9,0,0,136,11,0,0,68,13,0,0,34,15,0,0,17,17,0,0,136,19,0,0,68,21,0,0,34,23,0,0,17,0,0,0,240,47,0,0,255,134,0,0,240,172,0,0,255,0,1,0,240,41,1,0,255,136,1,0,240,186,1,0,255,26,2,0,255,95,2,0,15,124,2,0,255,193,2,0,15,222,2,0,255,35,3,0,15,64,3,0,255,129,3,0,15 };
				[[maybe_unused]] const std::uint8_t p_leafTable_bytes[] = { 43,200,178,32,26,91,212,113,63,110,254,190,224,239,3,89,15,100,0,252,15,224,43,211,47,48,207,0,100,192,195,15,30,34,219,254,131,150,22,48,178,254,36,217,86,210,6,59,202,210,28,202,178,8,26,71,69,29,63,254,254,190,192,223,7,178,30,200,122,236,0,252,7,0,252,195,60,3,243,12,244,192,195,99,240,48,25,255,65,75,11,45,45,32,2,117,151,182,137,5,254,254,190,192,223,7,178,30,200,122,236,0,252,7,0,252,195,60,3,243,12,236,192,195,7,192,195,163,165,133,150,22,8,1,59,73,66,32,90,199,196,45,108,0,252,7,0,252,195,60,3,126,0,248,10,0,240,5,211,22,32,30,247,2,108,63,79,240,243,160,165,5,59,203,66,40,203,66,40,26,75,69,25,63,236,0,252,23,0,252,195,60,3,243,12,254,0,248,42,0,240,5,211,22,76,91,32,110,205,22,236,63,79,241,243,160,165,133,150,22,252,0,252,7,1,248,135,121,6,230,25,254,0,248,10,0,240,5,211,22,76,91,48,241,152,17,252,63,79,240,231,65,75,11,45,45,59,137,145,36,122,199,196,37,108,192,195,7,192,195,163,165,5,48,42,158,160,255,0,126,128,135,9,0,15,131,182,12,100,31,143,96,152,21,0,230,25,59,203,177,52,14,177,48,26,11,197,245,3,236,192,195,31,192,195,163,165,133,150,22,48,211,121,128,36,146,2,254,128,135,57,0,15,131,182,12,218,50,228,31,143,100,152,183,1,0,204,51,48,207,0,244,192,195,99,128,240,90,42,11,180,180,208,210,2,32,219,7,8,254,128,135,57,0,15,131,182,12,218,50,228,31,143,64,216,7,4,128,121,6,230,25,43,7,195,36,26,95,148,221,246,3,48,23,40,123,107,215,3,116,63,143,68,218,94,160,9,2,180,180,0,116,31,15,53,202,110,39,64,0,152,103,0,124,124,77,4,249,154,192,107,2,59,135,227,44,132,194,44,26,199,69,217,109,63,48,23,56,187,42,187,27,244,63,143,4,250,15,36,75,16,160,165,133,150,22,228,31,143,249,219,230,70,2,0,204,51,48,207,0,236,124,77,4,124,77,224,53,129,215,4,32,70,119,14,236,63,79,240,243,160,165,133,150,22,228,31,143,4,152,7,0,48,207,192,60,3,236,124,77,4,124,77,224,53,129,215,4,27,138,162,0,138,162,0,26,4,246,254,62,127,31,200,122,32,235,1,228,0,252,3,240,15,243,12,204,51,228,192,195,3,15,143,150,22,90,90,246,254,62,127,31,200,122,32,235,1,228,0,252,3,240,15,243,12,204,51,228,192,195,3,15,143,150,22,90,90,27,7,130,0,26,4,182,254,62,127,31,200,122,164,0,252,3,240,15,243,12,164,192,195,3,15,143,150,22,27,202,2,36,202,2,36,26,4,228,0,252,3,240,15,243,12,204,51,246,0,248,2,192,23,76,91,48,109,1,228,63,207,207,131,150,22,90,90,228,0,252,3,240,15,243,12,204,51,246,0,248,2,192,23,76,91,48,109,1,228,63,207,207,131,150,22,90,90,27,8,2,28,26,4,164,0,252,3,240,15,243,12,182,0,248,2,192,23,76,91,164,63,207,207,131,150,22,27,10,176,36,10,176,36,26,4,228,192,195,3,15,143,150,22,90,90,246,128,135,1,60,12,218,50,104,203,0,228,31,207,199,3,243,12,204,51,228,192,195,3,15,143,150,22,90,90,246,128,135,1,60,12,218,50,104,203,0,228,31,207,199,3,243,12,204,51,27,8,128,28,26,4,164,192,195,3,15,143,150,22,182,128,135,1,60,12,218,50,164,31,207,199,3,243,12,27,64,146,40,64,146,40,26,4,228,63,207,207,131,150,22,90,90,228,31,207,199,3,243,12,204,51,228,124,77,240,53,129,215,4,94,19,228,63,207,207,131,150,22,90,90,228,31,207,199,3,243,12,204,51,228,124,77,240,53,129,215,4,94,19,27,192,113,32,26,4,164,63,207,207,131,150,22,164,31,207,199,3,243,12,164,124,77,240,53,129,215,4 };
				[[maybe_unused]] const std::uint8_t paletteBitStream_bytes[] = { 8, 33, 195, 128, 128, 124, 128, 128, 128, 129, 128, 199, 128, 128, 124, 128, 128, 128, 129, 128, 216, 128, 128, 124, 128, 128, 128, 129, 128, 0, 8, 33, 187, 128, 128, 130, 128, 128, 128, 128, 128, 191, 128, 128, 130, 128, 128, 128, 128, 128, 209, 128, 128, 130, 128, 128, 128, 128, 128, 0, 0, 0, 82, 80, 18, 64, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 214, 106, 18, 64, 1, 0, 0, 0, 8, 33, 195, 128, 128, 124, 128, 128, 128, 129, 128, 199, 128, 128, 124, 128, 128, 128, 129, 128, 216, 128, 128, 124, 128, 128, 128, 129, 128, 0, 198, 24, 241, 248, 8, 128, 128, 128, 128, 90, 128, 241, 8, 128, 248, 128, 128, 128, 147, 184, 241, 128, 248, 8, 128, 128, 128, 147, 72, 0 };

				iw6_asset->lightGrid.tableVersion = 1;
				iw6_asset->lightGrid.paletteVersion = 1;
				iw6_asset->lightGrid.paletteEntryCount = asset->lightGrid.colorCount;
				iw6_asset->lightGrid.paletteEntryAddress = mem.allocate<int>(iw6_asset->lightGrid.paletteEntryCount);
				for (unsigned int i = 0; i < iw6_asset->lightGrid.paletteEntryCount; i++)
				{
					iw6_asset->lightGrid.paletteEntryAddress[i] = 30; // 0, 30, 86, 116 //asset->lightGrid.entries[i].colorsIndex;
				}

				iw6_asset->lightGrid.paletteBitstreamSize = sizeof(paletteBitStream_bytes);
				iw6_asset->lightGrid.paletteBitstream = mem.allocate<unsigned char>(iw6_asset->lightGrid.paletteBitstreamSize);
				memcpy(iw6_asset->lightGrid.paletteBitstream, paletteBitStream_bytes, sizeof(paletteBitStream_bytes));

				iw6_asset->lightGrid.missingGridColorIndex = iw6_asset->lightGrid.paletteEntryCount - 1;

				iw6_asset->lightGrid.rangeExponent8BitsEncoding = 0;
				iw6_asset->lightGrid.rangeExponent12BitsEncoding = 4;
				iw6_asset->lightGrid.rangeExponent16BitsEncoding = 23;

				iw6_asset->lightGrid.stageCount = asset->primaryLightCount;
				iw6_asset->lightGrid.stageLightingContrastGain = mem.allocate<float>(iw6_asset->lightGrid.stageCount);
				for (auto i = 0; i < iw6_asset->lightGrid.stageCount; i++)
				{
					iw6_asset->lightGrid.stageLightingContrastGain[i] = 0.3f;
				}

				//memcpy(&iw6_asset->lightGrid.skyLightGridColors, skyLightGridColors_bytes, sizeof(skyLightGridColors_bytes));
				//memcpy(&iw6_asset->lightGrid.defaultLightGridColors, defaultLightGridColors_bytes, sizeof(defaultLightGridColors_bytes));
			}
			// ----
#endif

			iw6_asset->modelCount = asset->modelCount;
			iw6_asset->models = mem.allocate<IW6::GfxBrushModel>(iw6_asset->modelCount);
			for (int i = 0; i < iw6_asset->modelCount; i++)
			{
				int decals = asset->models[i].surfaceCount - asset->models[i].surfaceCountNoDecal;

				//memcpy(&iw6_asset->models[i].writable.bounds, &asset->models[i].writable.bounds, sizeof(float[2][3])); // Irrevelant
				memcpy(&iw6_asset->models[i].bounds, &asset->models[i].bounds, sizeof(float[2][3]));

				iw6_asset->models[i].radius = asset->models[i].radius;
				iw6_asset->models[i].startSurfIndex = asset->models[i].startSurfIndex;
				iw6_asset->models[i].surfaceCount = asset->models[i].surfaceCountNoDecal + decals;
			}

			memcpy(&iw6_asset->bounds, &asset->bounds, sizeof(float[2][3]));

			iw6_asset->checksum = asset->checksum;

			iw6_asset->materialMemoryCount = asset->materialMemoryCount;
			iw6_asset->materialMemory = mem.allocate<IW6::MaterialMemory>(iw6_asset->materialMemoryCount);
			for (int i = 0; i < iw6_asset->materialMemoryCount; i++)
			{
				iw6_asset->materialMemory[i].material = reinterpret_cast<IW6::Material*>(asset->materialMemory[i].material);
				iw6_asset->materialMemory[i].memory = asset->materialMemory[i].memory;
			}

			iw6_asset->sun.hasValidData = asset->sun.hasValidData;
			iw6_asset->sun.spriteMaterial = reinterpret_cast<IW6::Material*>(asset->sun.spriteMaterial);
			iw6_asset->sun.flareMaterial = reinterpret_cast<IW6::Material*>(asset->sun.flareMaterial);
			memcpy(&iw6_asset->sun.spriteSize, &asset->sun.spriteSize, Difference(&asset->sun.sunFxPosition, &asset->sun.spriteSize) + sizeof(float[3]));

			memcpy(&iw6_asset->outdoorLookupMatrix, &asset->outdoorLookupMatrix, sizeof(float[4][4]));

			iw6_asset->outdoorImage = mem.allocate<IW6::GfxImage>();
			iw6_asset->outdoorImage->name = asset->outdoorImage->name;

			iw6_asset->cellCasterBits = mem.allocate<unsigned int>(iw6_asset->dpvsPlanes.cellCount * ((iw6_asset->dpvsPlanes.cellCount + 31) >> 5));
			for (int i = 0; i < asset->dpvsPlanes.cellCount * ((asset->dpvsPlanes.cellCount + 31) >> 5); i++)
			{
				iw6_asset->cellCasterBits[i] = asset->cellCasterBits[i];
			}
			iw6_asset->cellHasSunLitSurfsBits = mem.allocate<unsigned int>((iw6_asset->dpvsPlanes.cellCount + 31) >> 5); // todo?

			iw6_asset->sceneDynModel = mem.allocate<IW6::GfxSceneDynModel>(asset->dpvsDyn.dynEntClientCount[0]);
			for (unsigned int i = 0; i < asset->dpvsDyn.dynEntClientCount[0]; i++)
			{
				iw6_asset->sceneDynModel[i].info.hasGfxEntIndex = asset->sceneDynModel[i].info.hasGfxEntIndex;
				iw6_asset->sceneDynModel[i].info.lod = asset->sceneDynModel[i].info.lod;
				iw6_asset->sceneDynModel[i].info.surfId = asset->sceneDynModel[i].info.surfId;
				iw6_asset->sceneDynModel[i].dynEntId = asset->sceneDynModel[i].dynEntId;
			}
			REINTERPRET_CAST_SAFE(iw6_asset->sceneDynBrush, asset->sceneDynBrush);

			//iw6_asset->primaryLightEntityShadowVis = reinterpret_cast<unsigned int* __ptr64>(asset->primaryLightEntityShadowVis);
			int count = ((iw6_asset->primaryLightCount - iw6_asset->lastSunPrimaryLightIndex) << 13) - 0x2000;
			iw6_asset->primaryLightEntityShadowVis = mem.allocate<unsigned int>(count);
			for (unsigned int i = 0; i < count; i++)
			{
				iw6_asset->primaryLightEntityShadowVis[i] = asset->primaryLightEntityShadowVis[i];
			}

			iw6_asset->primaryLightDynEntShadowVis[0] = reinterpret_cast<unsigned int* __ptr64>(asset->primaryLightDynEntShadowVis[0]);
			iw6_asset->primaryLightDynEntShadowVis[1] = reinterpret_cast<unsigned int* __ptr64>(asset->primaryLightDynEntShadowVis[1]);

			//iw6_asset->nonSunPrimaryLightForModelDynEnt = reinterpret_cast<unsigned __int16* __ptr64>(asset->primaryLightForModelDynEnt);
			iw6_asset->nonSunPrimaryLightForModelDynEnt = mem.allocate<unsigned short>(asset->dpvsDyn.dynEntClientCount[0]);
			for (unsigned int i = 0; i < asset->dpvsDyn.dynEntClientCount[0]; i++)
			{
				iw6_asset->nonSunPrimaryLightForModelDynEnt[i] = asset->nonSunPrimaryLightForModelDynEnt[i];
			}

			if (asset->shadowGeom)
			{
				iw6_asset->shadowGeom = mem.allocate<IW6::GfxShadowGeometry>(iw6_asset->primaryLightCount);
				for (unsigned int i = 0; i < iw6_asset->primaryLightCount; i++)
				{
					iw6_asset->shadowGeom[i].surfaceCount = asset->shadowGeom[i].surfaceCount;
					iw6_asset->shadowGeom[i].smodelCount = asset->shadowGeom[i].smodelCount;

					iw6_asset->shadowGeom[i].sortedSurfIndex = mem.allocate<unsigned int>(iw6_asset->shadowGeom[i].surfaceCount);
					for (unsigned int j = 0; j < iw6_asset->shadowGeom[i].surfaceCount; j++)
					{
						iw6_asset->shadowGeom[i].sortedSurfIndex[j] = asset->shadowGeom[i].sortedSurfIndex[j];
					}
					REINTERPRET_CAST_SAFE(iw6_asset->shadowGeom[i].smodelIndex, asset->shadowGeom[i].smodelIndex);
				}
			}
			iw6_asset->shadowGeomOptimized = nullptr;

			iw6_asset->lightRegion = mem.allocate<IW6::GfxLightRegion>(iw6_asset->primaryLightCount);
			for (unsigned int i = 0; i < iw6_asset->primaryLightCount; i++)
			{
				iw6_asset->lightRegion[i].hullCount = asset->lightRegion[i].hullCount;
				iw6_asset->lightRegion[i].hulls = mem.allocate<IW6::GfxLightRegionHull>(iw6_asset->lightRegion[i].hullCount);
				for (unsigned int j = 0; j < iw6_asset->lightRegion[i].hullCount; j++)
				{
					memcpy(&iw6_asset->lightRegion[i].hulls[j].kdopMidPoint, &asset->lightRegion[i].hulls[j].kdopMidPoint, sizeof(float[9]));
					memcpy(&iw6_asset->lightRegion[i].hulls[j].kdopHalfSize, &asset->lightRegion[i].hulls[j].kdopHalfSize, sizeof(float[9]));

					iw6_asset->lightRegion[i].hulls[j].axisCount = asset->lightRegion[i].hulls[j].axisCount;
					REINTERPRET_CAST_SAFE(iw6_asset->lightRegion[i].hulls[j].axis, asset->lightRegion[i].hulls[j].axis);
				}
			}

			unsigned int lit_decal_count = asset->dpvs.staticSurfaceCount - asset->dpvs.staticSurfaceCountNoDecal;

			iw6_asset->dpvs.smodelCount = asset->dpvs.smodelCount;
			iw6_asset->dpvs.staticSurfaceCount = asset->dpvs.staticSurfaceCountNoDecal + lit_decal_count;
			iw6_asset->dpvs.litOpaqueSurfsBegin = asset->dpvs.litOpaqueSurfsBegin;
			iw6_asset->dpvs.litOpaqueSurfsEnd = asset->dpvs.litOpaqueSurfsEnd;
			iw6_asset->dpvs.litDecalSurfsBegin = asset->dpvs.litOpaqueSurfsEnd; // skip
			iw6_asset->dpvs.litDecalSurfsEnd = asset->dpvs.litOpaqueSurfsEnd; // skip
			iw6_asset->dpvs.litTransSurfsBegin = asset->dpvs.litTransSurfsBegin;
			iw6_asset->dpvs.litTransSurfsEnd = asset->dpvs.litTransSurfsEnd;
			iw6_asset->dpvs.shadowCasterSurfsBegin = asset->dpvs.shadowCasterSurfsBegin;
			iw6_asset->dpvs.shadowCasterSurfsEnd = asset->dpvs.shadowCasterSurfsEnd;
			iw6_asset->dpvs.emissiveSurfsBegin = asset->dpvs.emissiveSurfsBegin;
			iw6_asset->dpvs.emissiveSurfsEnd = asset->dpvs.emissiveSurfsEnd;
			iw6_asset->dpvs.smodelVisDataCount = asset->dpvs.smodelVisDataCount;
			iw6_asset->dpvs.surfaceVisDataCount = asset->dpvs.surfaceVisDataCount;

			iw6_asset->dpvs.smodelVisData[0] = mem.allocate<unsigned int>(iw6_asset->dpvs.smodelVisDataCount);
			iw6_asset->dpvs.smodelVisData[1] = mem.allocate<unsigned int>(iw6_asset->dpvs.smodelVisDataCount);
			iw6_asset->dpvs.smodelVisData[2] = mem.allocate<unsigned int>(iw6_asset->dpvs.smodelVisDataCount);
			for (unsigned int i = 0; i < iw6_asset->dpvs.smodelVisDataCount; i++)
			{
				//iw6_asset->dpvs.smodelVisData[0][i] = asset->dpvs.smodelVisData[0][i];
				//iw6_asset->dpvs.smodelVisData[1][i] = asset->dpvs.smodelVisData[1][i];
				//iw6_asset->dpvs.smodelVisData[2][i] = asset->dpvs.smodelVisData[2][i];
			}

			iw6_asset->dpvs.surfaceVisData[0] = mem.allocate<unsigned int>(iw6_asset->dpvs.surfaceVisDataCount);
			iw6_asset->dpvs.surfaceVisData[1] = mem.allocate<unsigned int>(iw6_asset->dpvs.surfaceVisDataCount);
			iw6_asset->dpvs.surfaceVisData[2] = mem.allocate<unsigned int>(iw6_asset->dpvs.surfaceVisDataCount);
			for (unsigned int i = 0; i < iw6_asset->dpvs.surfaceVisDataCount; i++)
			{
				//iw6_asset->dpvs.surfaceVisData[0][i] = asset->dpvs.surfaceVisData[0][i];
				//iw6_asset->dpvs.surfaceVisData[1][i] = asset->dpvs.surfaceVisData[1][i];
				//iw6_asset->dpvs.surfaceVisData[2][i] = asset->dpvs.surfaceVisData[2][i];
			}

			iw6_asset->dpvs.unknownData01[0] = mem.allocate<unsigned int>(iw6_asset->dpvs.smodelVisDataCount + 1); // idk?
			iw6_asset->dpvs.unknownData01[1] = mem.allocate<unsigned int>(iw6_asset->dpvs.smodelVisDataCount + 1);
			iw6_asset->dpvs.unknownData01[2] = mem.allocate<unsigned int>(iw6_asset->dpvs.smodelVisDataCount + 1);

			iw6_asset->dpvs.unknownData02[0] = mem.allocate<unsigned int>(iw6_asset->dpvs.surfaceVisDataCount); // tesselationData?
			iw6_asset->dpvs.unknownData02[1] = mem.allocate<unsigned int>(iw6_asset->dpvs.surfaceVisDataCount);
			iw6_asset->dpvs.unknownData02[2] = mem.allocate<unsigned int>(iw6_asset->dpvs.surfaceVisDataCount);

			iw6_asset->dpvs.lodData = mem.allocate<unsigned int>(iw6_asset->dpvs.smodelCount + 1); // idk?

			iw6_asset->dpvs.tessellationCutoffVisData[0] = mem.allocate<unsigned int>(iw6_asset->dpvs.surfaceVisDataCount); // idk if correct?
			iw6_asset->dpvs.tessellationCutoffVisData[1] = mem.allocate<unsigned int>(iw6_asset->dpvs.surfaceVisDataCount);
			iw6_asset->dpvs.tessellationCutoffVisData[2] = mem.allocate<unsigned int>(iw6_asset->dpvs.surfaceVisDataCount);

			iw6_asset->dpvs.sortedSurfIndex = mem.allocate<unsigned int>(iw6_asset->dpvs.staticSurfaceCount);
			for (unsigned int i = 0; i < iw6_asset->dpvs.staticSurfaceCount; i++)
			{
				iw6_asset->dpvs.sortedSurfIndex[i] = asset->dpvs.sortedSurfIndex[i];
			}

			REINTERPRET_CAST_SAFE(iw6_asset->dpvs.smodelInsts, asset->dpvs.smodelInsts);

			iw6_asset->dpvs.surfaces = mem.allocate<IW6::GfxSurface>(iw6_asset->surfaceCount);
			for (unsigned int i = 0; i < iw6_asset->surfaceCount; i++)
			{
				iw6_asset->dpvs.surfaces[i].tris.vertexLayerData = asset->dpvs.surfaces[i].tris.vertexLayerData;
				iw6_asset->dpvs.surfaces[i].tris.firstVertex = asset->dpvs.surfaces[i].tris.firstVertex;
				iw6_asset->dpvs.surfaces[i].tris.maxEdgeLength = 0;
				iw6_asset->dpvs.surfaces[i].tris.vertexCount = asset->dpvs.surfaces[i].tris.vertexCount;
				iw6_asset->dpvs.surfaces[i].tris.triCount = asset->dpvs.surfaces[i].tris.triCount;
				iw6_asset->dpvs.surfaces[i].tris.baseIndex = asset->dpvs.surfaces[i].tris.baseIndex;
				iw6_asset->dpvs.surfaces[i].material = reinterpret_cast<IW6::Material*>(asset->dpvs.surfaces[i].material);
				iw6_asset->dpvs.surfaces[i].laf.fields.lightmapIndex = asset->dpvs.surfaces[i].laf.fields.lightmapIndex;
				iw6_asset->dpvs.surfaces[i].laf.fields.reflectionProbeIndex = asset->dpvs.surfaces[i].laf.fields.reflectionProbeIndex;
				iw6_asset->dpvs.surfaces[i].laf.fields.primaryLightEnvIndex = asset->dpvs.surfaces[i].laf.fields.primaryLightIndex;
				iw6_asset->dpvs.surfaces[i].laf.fields.flags = asset->dpvs.surfaces[i].laf.fields.flags;
			}

			iw6_asset->dpvs.surfacesBounds = mem.allocate<IW6::GfxSurfaceBounds>(iw6_asset->surfaceCount);
			for (unsigned int i = 0; i < iw6_asset->surfaceCount; i++)
			{
				memcpy(&iw6_asset->dpvs.surfacesBounds[i].bounds, &asset->dpvs.surfacesBounds[i].bounds, sizeof(IW5::Bounds));
			}

			iw6_asset->dpvs.smodelDrawInsts = mem.allocate<IW6::GfxStaticModelDrawInst>(iw6_asset->dpvs.smodelCount);
			for (unsigned int i = 0; i < iw6_asset->dpvs.smodelCount; i++)
			{
				memcpy(&iw6_asset->dpvs.smodelDrawInsts[i].placement, &asset->dpvs.smodelDrawInsts[i].placement, sizeof(IW5::GfxPackedPlacement));
				iw6_asset->dpvs.smodelDrawInsts[i].model = reinterpret_cast<IW6::XModel*>(asset->dpvs.smodelDrawInsts[i].model);
				memset(&iw6_asset->dpvs.smodelDrawInsts[i].vertexLightingInfo, 0, sizeof(IW6::GfxStaticModelVertexLightingInfo));
				iw6_asset->dpvs.smodelDrawInsts[i].modelLightmapInfo.offset[0] = 0;
				iw6_asset->dpvs.smodelDrawInsts[i].modelLightmapInfo.offset[1] = 0;
				iw6_asset->dpvs.smodelDrawInsts[i].modelLightmapInfo.scale[0] = 0;
				iw6_asset->dpvs.smodelDrawInsts[i].modelLightmapInfo.scale[1] = 0;
				iw6_asset->dpvs.smodelDrawInsts[i].modelLightmapInfo.lightmapIndex = -1;
				iw6_asset->dpvs.smodelDrawInsts[i].lightingHandle = asset->dpvs.smodelDrawInsts[i].lightingHandle;
				iw6_asset->dpvs.smodelDrawInsts[i].cullDist = asset->dpvs.smodelDrawInsts[i].cullDist;
				iw6_asset->dpvs.smodelDrawInsts[i].flags = 0;
				iw6_asset->dpvs.smodelDrawInsts[i].staticModelId = 0;
				iw6_asset->dpvs.smodelDrawInsts[i].primaryLightEnvIndex = asset->dpvs.smodelDrawInsts[i].primaryLightIndex;
				iw6_asset->dpvs.smodelDrawInsts[i].reflectionProbeIndex = asset->dpvs.smodelDrawInsts[i].reflectionProbeIndex;
				iw6_asset->dpvs.smodelDrawInsts[i].firstMtlSkinIndex = asset->dpvs.smodelDrawInsts[i].firstMtlSkinIndex;
				iw6_asset->dpvs.smodelDrawInsts[i].sunShadowFlags = 1;

				// casts no shadows
				auto no_shadows = (asset->dpvs.smodelDrawInsts[i].flags & 0x10) != 0;
				if (no_shadows)
				{
					iw6_asset->dpvs.smodelDrawInsts[i].flags |= IW6::StaticModelFlag::STATIC_MODEL_FLAG_NO_CAST_SHADOW;
				}

				// ground lighting
				auto ground_lighting = (asset->dpvs.smodelDrawInsts[i].flags & 0x20) != 0 || asset->dpvs.smodelDrawInsts[i].groundLighting.packed != 0;
				if (ground_lighting)
				{
					iw6_asset->dpvs.smodelDrawInsts[i].flags |= IW6::StaticModelFlag::STATIC_MODEL_FLAG_GROUND_LIGHTING;
				}
				// regular lighting
				else
				{
					iw6_asset->dpvs.smodelDrawInsts[i].flags |= IW6::StaticModelFlag::STATIC_MODEL_FLAG_LIGHTGRID_LIGHTING;
				}
			}
			for (unsigned int i = 0; i < iw6_asset->dpvs.smodelCount; i++)
			{
				if ((iw6_asset->dpvs.smodelDrawInsts[i].flags & IW6::StaticModelFlag::STATIC_MODEL_FLAG_GROUND_LIGHTING) != 0)
				{
					//bgra -> rgba
					auto ground_lighting = asset->dpvs.smodelDrawInsts[i].groundLighting;
					auto bgra = ground_lighting.array;

					float rgba[4] = { bgra[2] / 255.0f, bgra[1] / 255.0f, bgra[0] / 255.0f, bgra[3] / 255.0f };

					iw6_asset->dpvs.smodelDrawInsts[i].groundLighting[0] = (rgba[0]); // r
					iw6_asset->dpvs.smodelDrawInsts[i].groundLighting[1] = (rgba[1]); // g
					iw6_asset->dpvs.smodelDrawInsts[i].groundLighting[2] = (rgba[2]); // b
					iw6_asset->dpvs.smodelDrawInsts[i].groundLighting[3] = (rgba[3]); // a
				}
				else if ((iw6_asset->dpvs.smodelDrawInsts[i].flags & IW6::StaticModelFlag::STATIC_MODEL_FLAG_LIGHTGRID_LIGHTING) != 0)
				{
					// runtime calculated
				}
				else if ((iw6_asset->dpvs.smodelDrawInsts[i].flags & IW6::StaticModelFlag::STATIC_MODEL_FLAG_LIGHTMAP_LIGHTING) != 0)
				{
					// todo?
				}
				else if ((iw6_asset->dpvs.smodelDrawInsts[i].flags & IW6::StaticModelFlag::STATIC_MODEL_FLAG_VERTEXLIT_LIGHTING) != 0)
				{
					// todo?
				}
			}

			iw6_asset->dpvs.surfaceMaterials = mem.allocate<IW6::GfxDrawSurf>(iw6_asset->surfaceCount);
			for (unsigned int i = 0; i < iw6_asset->surfaceCount; i++) // runtime data
			{
				iw6_asset->dpvs.surfaceMaterials[i].fields.objectId = asset->dpvs.surfaceMaterials[i].fields.objectId;
				iw6_asset->dpvs.surfaceMaterials[i].fields.reflectionProbeIndex = asset->dpvs.surfaceMaterials[i].fields.reflectionProbeIndex;
				iw6_asset->dpvs.surfaceMaterials[i].fields.hasGfxEntIndex = asset->dpvs.surfaceMaterials[i].fields.hasGfxEntIndex;
				iw6_asset->dpvs.surfaceMaterials[i].fields.customIndex = asset->dpvs.surfaceMaterials[i].fields.customIndex;
				iw6_asset->dpvs.surfaceMaterials[i].fields.materialSortedIndex = asset->dpvs.surfaceMaterials[i].fields.materialSortedIndex;
				iw6_asset->dpvs.surfaceMaterials[i].fields.tessellation = 0;
				iw6_asset->dpvs.surfaceMaterials[i].fields.prepass = asset->dpvs.surfaceMaterials[i].fields.prepass;
				iw6_asset->dpvs.surfaceMaterials[i].fields.useHeroLighting = asset->dpvs.surfaceMaterials[i].fields.useHeroLighting;
				iw6_asset->dpvs.surfaceMaterials[i].fields.sceneLightEnvIndex = asset->dpvs.surfaceMaterials[i].fields.sceneLightIndex;
				iw6_asset->dpvs.surfaceMaterials[i].fields.viewModelRender = asset->dpvs.surfaceMaterials[i].fields.viewModelRender;
				iw6_asset->dpvs.surfaceMaterials[i].fields.surfType = asset->dpvs.surfaceMaterials[i].fields.surfType;
				iw6_asset->dpvs.surfaceMaterials[i].fields.primarySortKey = asset->dpvs.surfaceMaterials[i].fields.primarySortKey;
				iw6_asset->dpvs.surfaceMaterials[i].fields.unused = asset->dpvs.surfaceMaterials[i].fields.unused;
			}

			REINTERPRET_CAST_SAFE(iw6_asset->dpvs.surfaceCastsSunShadow, asset->dpvs.surfaceCastsSunShadow);
			iw6_asset->dpvs.sunShadowOptCount = 0;
			iw6_asset->dpvs.sunSurfVisDataCount = 0;
			iw6_asset->dpvs.surfaceCastsSunShadowOpt = nullptr;
			iw6_asset->dpvs.constantBuffersLit = mem.allocate<char* __ptr64>(iw6_asset->dpvs.smodelCount); //nullptr;
			iw6_asset->dpvs.constantBuffersAmbient = mem.allocate<char* __ptr64>(iw6_asset->dpvs.smodelCount); //nullptr;
			iw6_asset->dpvs.usageCount = asset->dpvs.usageCount;

			iw6_asset->dpvsDyn.dynEntClientWordCount[0] = asset->dpvsDyn.dynEntClientWordCount[0];
			iw6_asset->dpvsDyn.dynEntClientWordCount[1] = asset->dpvsDyn.dynEntClientWordCount[1];
			iw6_asset->dpvsDyn.dynEntClientCount[0] = asset->dpvsDyn.dynEntClientCount[0];
			iw6_asset->dpvsDyn.dynEntClientCount[1] = asset->dpvsDyn.dynEntClientCount[1];
			iw6_asset->dpvsDyn.dynEntCellBits[0] = reinterpret_cast<unsigned int*>(asset->dpvsDyn.dynEntCellBits[0]);
			iw6_asset->dpvsDyn.dynEntCellBits[1] = reinterpret_cast<unsigned int*>(asset->dpvsDyn.dynEntCellBits[1]);
			iw6_asset->dpvsDyn.dynEntVisData[0][0] = reinterpret_cast<unsigned __int8*>(asset->dpvsDyn.dynEntVisData[0][0]);
			iw6_asset->dpvsDyn.dynEntVisData[0][1] = reinterpret_cast<unsigned __int8*>(asset->dpvsDyn.dynEntVisData[0][1]);
			iw6_asset->dpvsDyn.dynEntVisData[0][2] = reinterpret_cast<unsigned __int8*>(asset->dpvsDyn.dynEntVisData[0][2]);
			iw6_asset->dpvsDyn.dynEntVisData[1][0] = reinterpret_cast<unsigned __int8*>(asset->dpvsDyn.dynEntVisData[1][0]);
			iw6_asset->dpvsDyn.dynEntVisData[1][1] = reinterpret_cast<unsigned __int8*>(asset->dpvsDyn.dynEntVisData[1][1]);
			iw6_asset->dpvsDyn.dynEntVisData[1][2] = reinterpret_cast<unsigned __int8*>(asset->dpvsDyn.dynEntVisData[1][2]);

			iw6_asset->mapVtxChecksum = asset->mapVtxChecksum;

			iw6_asset->heroOnlyLightCount = asset->heroOnlyLightCount;
			REINTERPRET_CAST_SAFE(iw6_asset->heroOnlyLights, asset->heroOnlyLights);

			iw6_asset->fogTypesAllowed = asset->fogTypesAllowed;

			iw6_asset->umbraTomeSize = 0;
			iw6_asset->umbraTomeData = nullptr;
			iw6_asset->umbraTomePtr = nullptr;
			/*
			{
				float gfx_mins[3]
				{
					asset->bounds.midPoint[0] - asset->bounds.halfSize[0],
					asset->bounds.midPoint[1] - asset->bounds.halfSize[1],
					asset->bounds.midPoint[2] - asset->bounds.halfSize[2]
				};
				float gfx_maxs[3]
				{
					asset->bounds.midPoint[0] + asset->bounds.halfSize[0],
					asset->bounds.midPoint[1] + asset->bounds.halfSize[1],
					asset->bounds.midPoint[2] + asset->bounds.halfSize[2]
				};

				static char buffer[sizeof(Umbra::ImpTome)];
				memset(buffer, 0, sizeof(buffer));
				auto* new_tome = reinterpret_cast<Umbra::ImpTome*>(buffer);
				new_tome->m_versionMagic = 0xD6000012;
				new_tome->m_crc32 = 0xD15AB1ED;
				new_tome->m_size = sizeof(buffer);
				new_tome->m_lodBaseDistance = 512.0f;
				memcpy(&new_tome->m_treeMin, gfx_mins, sizeof(float[3]));
				memcpy(&new_tome->m_treeMax, gfx_maxs, sizeof(float[3]));

				new_tome->m_crc32 = Umbra::ImpTome::computeCRC32(new_tome);

				iw6_asset->umbraTomeSize = new_tome->m_size;
				iw6_asset->umbraTomeData = mem->ManualAlloc<char>(iw6_asset->umbraTomeSize);
				memcpy(iw6_asset->umbraTomeData, buffer, iw6_asset->umbraTomeSize);
				iw6_asset->umbraTomePtr = reinterpret_cast<void*>(iw6_asset->umbraTomeData);
			}
			*/

			return iw6_asset;
		}

		IW6::GfxWorld* convert(GfxWorld* asset, allocator& allocator)
		{
			// generate IW6 gfxworld
			return GenerateIW6GfxWorld(asset, allocator);
		}
	}
}