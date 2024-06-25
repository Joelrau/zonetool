#include "stdafx.hpp"
#include "../Include.hpp"

#include "GfxWorld.hpp"

#include "X64/Utils/Umbra/umbra.hpp"
#include "X64/Utils/Utils.hpp"

namespace ZoneTool::IW5
{
	namespace H1Converter
	{
		H1::GfxWorld* GenerateH1GfxWorld(GfxWorld* asset, allocator& mem)
		{
			// allocate H1 GfxWorld structure
			const auto h1_asset = mem.allocate<H1::GfxWorld>();

			h1_asset->name = asset->name;
			h1_asset->baseName = asset->baseName;

			h1_asset->bspVersion = 111;

			h1_asset->planeCount = asset->planeCount;
			h1_asset->nodeCount = asset->nodeCount;
			h1_asset->surfaceCount = asset->surfaceCount;
			h1_asset->skyCount = asset->skyCount;

			h1_asset->skies = mem.allocate<H1::GfxSky>(h1_asset->skyCount);
			for (int i = 0; i < h1_asset->skyCount; i++)
			{
				h1_asset->skies[i].skySurfCount = asset->skies[i].skySurfCount;
				REINTERPRET_CAST_SAFE(h1_asset->skies[i].skyStartSurfs, asset->skies[i].skyStartSurfs);
				if (asset->skies[i].skyImage)
				{
					h1_asset->skies[i].skyImage = mem.allocate<H1::GfxImage>();
					h1_asset->skies[i].skyImage->name = asset->skies[i].skyImage->name;
				}
				else
				{
					h1_asset->skies[i].skyImage = nullptr;
				}
				h1_asset->skies[i].skySamplerState = asset->skies[i].skySamplerState;

				// add bounds
				//assert(asset->skies[i].skySurfCount == 1);
				for (auto j = 0; j < asset->skies[i].skySurfCount; j++)
				{
					auto index = asset->dpvs.sortedSurfIndex[asset->skies[i].skyStartSurfs[j]];
					auto* surface_bounds = &asset->dpvs.surfacesBounds[index];
					memcpy(&h1_asset->skies[i].bounds, &surface_bounds->bounds, sizeof(surface_bounds->bounds));

					//
					break;
				}
			}

			h1_asset->portalGroupCount = 0;
			h1_asset->lastSunPrimaryLightIndex = asset->lastSunPrimaryLightIndex;
			h1_asset->primaryLightCount = asset->primaryLightCount;
			h1_asset->primaryLightEnvCount = asset->primaryLightCount + 1;
			h1_asset->sortKeyLitDecal = 7;
			h1_asset->sortKeyEffectDecal = 43;
			h1_asset->sortKeyTopDecal = 17;
			h1_asset->sortKeyEffectAuto = 53;
			h1_asset->sortKeyDistortion = 48;
			h1_asset->sortKeyHair = 18;
			h1_asset->sortKeyEffectBlend = 33;

			h1_asset->dpvsPlanes.cellCount = asset->dpvsPlanes.cellCount;
			REINTERPRET_CAST_SAFE(h1_asset->dpvsPlanes.planes, asset->dpvsPlanes.planes);
			REINTERPRET_CAST_SAFE(h1_asset->dpvsPlanes.nodes, asset->dpvsPlanes.nodes);

			h1_asset->dpvsPlanes.sceneEntCellBits = mem.allocate<unsigned int>(asset->dpvsPlanes.cellCount << 9);
			for (int i = 0; i < asset->dpvsPlanes.cellCount << 9; i++)
			{
				h1_asset->dpvsPlanes.sceneEntCellBits[i] = asset->dpvsPlanes.sceneEntCellBits[i];
			}

			h1_asset->aabbTreeCounts = mem.allocate<H1::GfxCellTreeCount>(h1_asset->dpvsPlanes.cellCount);
			h1_asset->aabbTrees = mem.allocate<H1::GfxCellTree>(h1_asset->dpvsPlanes.cellCount);
			for (int i = 0; i < h1_asset->dpvsPlanes.cellCount; i++)
			{
				h1_asset->aabbTreeCounts[i].aabbTreeCount = asset->aabbTreeCounts[i].aabbTreeCount;
				h1_asset->aabbTrees[i].aabbTree = mem.allocate<H1::GfxAabbTree>(h1_asset->aabbTreeCounts[i].aabbTreeCount);
				for (int j = 0; j < h1_asset->aabbTreeCounts[i].aabbTreeCount; j++)
				{
					memcpy(&h1_asset->aabbTrees[i].aabbTree[j].bounds, &asset->aabbTrees[i].aabbTree[j].bounds, sizeof(float[2][3]));

					h1_asset->aabbTrees[i].aabbTree[j].startSurfIndex = asset->aabbTrees[i].aabbTree[j].startSurfIndex;
					h1_asset->aabbTrees[i].aabbTree[j].surfaceCount = asset->aabbTrees[i].aabbTree[j].surfaceCount;

					h1_asset->aabbTrees[i].aabbTree[j].smodelIndexCount = asset->aabbTrees[i].aabbTree[j].smodelIndexCount;
					REINTERPRET_CAST_SAFE(h1_asset->aabbTrees[i].aabbTree[j].smodelIndexes, asset->aabbTrees[i].aabbTree[j].smodelIndexes);

					h1_asset->aabbTrees[i].aabbTree[j].childCount = asset->aabbTrees[i].aabbTree[j].childCount;
					// re-calculate childrenOffset
					auto offset = asset->aabbTrees[i].aabbTree[j].childrenOffset;
					int childrenIndex = offset / sizeof(IW5::GfxAabbTree);
					int childrenOffset = childrenIndex * sizeof(H1::GfxAabbTree);
					h1_asset->aabbTrees[i].aabbTree[j].childrenOffset = childrenOffset;
				}
			}

			h1_asset->cells = mem.allocate<H1::GfxCell>(h1_asset->dpvsPlanes.cellCount);
			for (int i = 0; i < h1_asset->dpvsPlanes.cellCount; i++)
			{
				memcpy(&h1_asset->cells[i].bounds, &asset->cells[i].bounds, sizeof(float[2][3]));
				h1_asset->cells[i].portalCount = asset->cells[i].portalCount;

				auto add_portal = [](H1::GfxPortal* h1_portal, IW5::GfxPortal* iw5_portal)
				{
					memcpy(&h1_portal->plane, &iw5_portal->plane, sizeof(float[4]));
					h1_portal->vertices = reinterpret_cast<float(*__ptr64)[3]>(iw5_portal->vertices);
					h1_portal->cellIndex = iw5_portal->cellIndex;
					h1_portal->closeDistance = 0;
					h1_portal->vertexCount = iw5_portal->vertexCount;
					memcpy(&h1_portal->hullAxis, &iw5_portal->hullAxis, sizeof(float[2][3]));
				};
				h1_asset->cells[i].portals = mem.allocate<H1::GfxPortal>(h1_asset->cells[i].portalCount);
				for (int j = 0; j < h1_asset->cells[i].portalCount; j++)
				{
					add_portal(&h1_asset->cells[i].portals[j], &asset->cells[i].portals[j]);
				}

				h1_asset->cells[i].reflectionProbeCount = asset->cells[i].reflectionProbeCount;
				h1_asset->cells[i].reflectionProbes = reinterpret_cast<unsigned __int8* __ptr64>(asset->cells[i].reflectionProbes);
				h1_asset->cells[i].reflectionProbeReferenceCount = asset->cells[i].reflectionProbeReferenceCount;
				h1_asset->cells[i].reflectionProbeReferences = reinterpret_cast<unsigned __int8* __ptr64>(asset->cells[i].reflectionProbeReferences);
			}

			h1_asset->portalGroup = nullptr;

			h1_asset->unk_vec4_count_0 = 0;
			h1_asset->unk_vec4_0 = nullptr;

			h1_asset->draw.reflectionProbeCount = asset->draw.reflectionProbeCount;
			h1_asset->draw.reflectionProbes = mem.allocate<H1::GfxImage* __ptr64>(h1_asset->draw.reflectionProbeCount);
			h1_asset->draw.reflectionProbeOrigins = mem.allocate<H1::GfxReflectionProbe>(h1_asset->draw.reflectionProbeCount);
			h1_asset->draw.reflectionProbeTextures = mem.allocate<H1::GfxRawTexture>(h1_asset->draw.reflectionProbeCount);
			for (unsigned int i = 0; i < h1_asset->draw.reflectionProbeCount; i++)
			{
				h1_asset->draw.reflectionProbes[i] = mem.allocate<H1::GfxImage>();
				h1_asset->draw.reflectionProbes[i]->name = asset->draw.reflectionProbes[i]->name;
				memcpy(&h1_asset->draw.reflectionProbeOrigins[i].origin, &asset->draw.reflectionProbeOrigins[i].origin, sizeof(float[3]));
				h1_asset->draw.reflectionProbeOrigins[i].probeVolumeCount = 0;
				h1_asset->draw.reflectionProbeOrigins[i].probeVolumes = nullptr;
				//memcpy(&h1_asset->draw.reflectionProbeTextures[i], &asset->draw.reflectionProbeTextures[i].loadDef, 20);
			}
			h1_asset->draw.reflectionProbeReferenceCount = asset->draw.reflectionProbeReferenceCount;
			h1_asset->draw.reflectionProbeReferenceOrigins = reinterpret_cast<H1::GfxReflectionProbeReferenceOrigin * __ptr64>(
				asset->draw.reflectionProbeReferenceOrigins);
			h1_asset->draw.reflectionProbeReferences = reinterpret_cast<H1::GfxReflectionProbeReference * __ptr64>(
				asset->draw.reflectionProbeReferences);

			h1_asset->draw.lightmapCount = asset->draw.lightmapCount;
			h1_asset->draw.lightmaps = mem.allocate<H1::GfxLightmapArray>(h1_asset->draw.lightmapCount);
			h1_asset->draw.lightmapPrimaryTextures = mem.allocate<H1::GfxRawTexture>(h1_asset->draw.lightmapCount);
			h1_asset->draw.lightmapSecondaryTextures = mem.allocate<H1::GfxRawTexture>(h1_asset->draw.lightmapCount);
			for (int i = 0; i < h1_asset->draw.lightmapCount; i++)
			{
				h1_asset->draw.lightmaps[i].primary = mem.allocate<H1::GfxImage>();
				h1_asset->draw.lightmaps[i].primary->name = asset->draw.lightmaps[i].primary->name;
				h1_asset->draw.lightmaps[i].secondary = mem.allocate<H1::GfxImage>();
				h1_asset->draw.lightmaps[i].secondary->name = asset->draw.lightmaps[i].secondary->name;

				//memcpy(&h1_asset->draw.lightmapPrimaryTextures[i], &asset->draw.lightmapPrimaryTextures[i].loadDef, 20);
				//memcpy(&h1_asset->draw.lightmapSecondaryTextures[i], &asset->draw.lightmapSecondaryTextures[i].loadDef, 20);
			}
			if (asset->draw.lightmapOverridePrimary)
			{
				h1_asset->draw.lightmapOverridePrimary = mem.allocate<H1::GfxImage>();
				h1_asset->draw.lightmapOverridePrimary->name = asset->draw.lightmapOverridePrimary->name;
			}
			else
			{
				h1_asset->draw.lightmapOverridePrimary = nullptr;
			}

			if (asset->draw.lightmapOverrideSecondary)
			{
				h1_asset->draw.lightmapOverrideSecondary = mem.allocate<H1::GfxImage>();
				h1_asset->draw.lightmapOverrideSecondary->name = asset->draw.lightmapOverrideSecondary->name;
			}
			else
			{
				h1_asset->draw.lightmapOverrideSecondary = nullptr;
			}

			h1_asset->draw.u1[0] = 1024; h1_asset->draw.u1[1] = 1024; // u1
			h1_asset->draw.u2[0] = 512; h1_asset->draw.u2[1] = 512; // u2
			h1_asset->draw.u3 = 8; // u3

			h1_asset->draw.trisType = 0; // dunno

			h1_asset->draw.vertexCount = asset->draw.vertexCount;
			h1_asset->draw.vd.vertices = mem.allocate<H1::GfxWorldVertex>(h1_asset->draw.vertexCount);
			for (unsigned int i = 0; i < h1_asset->draw.vertexCount; i++)
			{
				memcpy(&h1_asset->draw.vd.vertices[i], &asset->draw.vd.vertices[i], sizeof(IW5::GfxWorldVertex));

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

				h1_asset->draw.vd.vertices[i].normal.packed = PackedVec::Vec3PackUnitVecWithAlpha(normal, 1.0f);
				h1_asset->draw.vd.vertices[i].tangent.packed = PackedVec::Vec3PackUnitVecWithAlpha(tangent, sign);

				// correct color : bgra->rgba
				h1_asset->draw.vd.vertices[i].color.array[0] = asset->draw.vd.vertices[i].color.array[2];
				h1_asset->draw.vd.vertices[i].color.array[1] = asset->draw.vd.vertices[i].color.array[1];
				h1_asset->draw.vd.vertices[i].color.array[2] = asset->draw.vd.vertices[i].color.array[0];
				h1_asset->draw.vd.vertices[i].color.array[3] = asset->draw.vd.vertices[i].color.array[3];
			}

			h1_asset->draw.vertexLayerDataSize = asset->draw.vertexLayerDataSize;
			REINTERPRET_CAST_SAFE(h1_asset->draw.vld.data, asset->draw.vld.data);

			h1_asset->draw.indexCount = asset->draw.indexCount;
			REINTERPRET_CAST_SAFE(h1_asset->draw.indices, asset->draw.indices);

			h1_asset->draw.displacementParmsCount = 0;
			h1_asset->draw.displacementParms = nullptr;

			h1_asset->lightGrid.hasLightRegions = asset->lightGrid.hasLightRegions;
			h1_asset->lightGrid.useSkyForLowZ = 0;
			h1_asset->lightGrid.lastSunPrimaryLightIndex = asset->lightGrid.lastSunPrimaryLightIndex;
			memcpy(&h1_asset->lightGrid.mins, &asset->lightGrid.mins, sizeof(short[3]));
			memcpy(&h1_asset->lightGrid.maxs, &asset->lightGrid.maxs, sizeof(short[3]));
			h1_asset->lightGrid.rowAxis = asset->lightGrid.rowAxis;
			h1_asset->lightGrid.colAxis = asset->lightGrid.colAxis;
			REINTERPRET_CAST_SAFE(h1_asset->lightGrid.rowDataStart, asset->lightGrid.rowDataStart);
			h1_asset->lightGrid.rawRowDataSize = asset->lightGrid.rawRowDataSize;
			REINTERPRET_CAST_SAFE(h1_asset->lightGrid.rawRowData, asset->lightGrid.rawRowData);
			h1_asset->lightGrid.entryCount = asset->lightGrid.entryCount;
			h1_asset->lightGrid.entries = mem.allocate<H1::GfxLightGridEntry>(h1_asset->lightGrid.entryCount);
			for (unsigned int i = 0; i < h1_asset->lightGrid.entryCount; i++)
			{
				h1_asset->lightGrid.entries[i].colorsIndex = asset->lightGrid.entries[i].colorsIndex;
				h1_asset->lightGrid.entries[i].primaryLightEnvIndex = asset->lightGrid.entries[i].primaryLightIndex;
				h1_asset->lightGrid.entries[i].unused = 0;
				h1_asset->lightGrid.entries[i].needsTrace = asset->lightGrid.entries[i].needsTrace;
			}
			h1_asset->lightGrid.colorCount = asset->lightGrid.colorCount;
			h1_asset->lightGrid.colors = mem.allocate<H1::GfxLightGridColors>(h1_asset->lightGrid.colorCount);
			for (unsigned int i = 0; i < h1_asset->lightGrid.colorCount; i++)
			{
				for (unsigned int j = 0; j < 56; j++)
				{
					auto& rgb = asset->lightGrid.colors[i].rgb[j];
					auto& dest_rgb = h1_asset->lightGrid.colors[i].rgb[j];
					dest_rgb[0] = float_to_half(rgb[0] / 255.f);
					dest_rgb[1] = float_to_half(rgb[1] / 255.f);
					dest_rgb[2] = float_to_half(rgb[2] / 255.f);
				}
			}

			// --experimental--
			{
				// iw6 mp_character_room
				[[maybe_unused]] const std::uint8_t skyLightGridColors_bytes[] = { 0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63,0,0,59,63,0,0,63,63,0,0,81,63 };
				[[maybe_unused]] const std::uint8_t defaultLightGridColors_bytes[] = { 0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63,0,0,65,63,0,0,69,63,0,0,86,63 };
				[[maybe_unused]] const std::uint8_t p_nodeTable_bytes[] = { 1,0,0,255,9,0,0,136,11,0,0,68,13,0,0,34,15,0,0,17,17,0,0,136,19,0,0,68,21,0,0,34,23,0,0,17,0,0,0,240,47,0,0,255,134,0,0,240,172,0,0,255,0,1,0,240,41,1,0,255,136,1,0,240,186,1,0,255,26,2,0,255,95,2,0,15,124,2,0,255,193,2,0,15,222,2,0,255,35,3,0,15,64,3,0,255,129,3,0,15 };
				[[maybe_unused]] const std::uint8_t p_leafTable_bytes[] = { 43,200,178,32,26,91,212,113,63,110,254,190,224,239,3,89,15,100,0,252,15,224,43,211,47,48,207,0,100,192,195,15,30,34,219,254,131,150,22,48,178,254,36,217,86,210,6,59,202,210,28,202,178,8,26,71,69,29,63,254,254,190,192,223,7,178,30,200,122,236,0,252,7,0,252,195,60,3,243,12,244,192,195,99,240,48,25,255,65,75,11,45,45,32,2,117,151,182,137,5,254,254,190,192,223,7,178,30,200,122,236,0,252,7,0,252,195,60,3,243,12,236,192,195,7,192,195,163,165,133,150,22,8,1,59,73,66,32,90,199,196,45,108,0,252,7,0,252,195,60,3,126,0,248,10,0,240,5,211,22,32,30,247,2,108,63,79,240,243,160,165,5,59,203,66,40,203,66,40,26,75,69,25,63,236,0,252,23,0,252,195,60,3,243,12,254,0,248,42,0,240,5,211,22,76,91,32,110,205,22,236,63,79,241,243,160,165,133,150,22,252,0,252,7,1,248,135,121,6,230,25,254,0,248,10,0,240,5,211,22,76,91,48,241,152,17,252,63,79,240,231,65,75,11,45,45,59,137,145,36,122,199,196,37,108,192,195,7,192,195,163,165,5,48,42,158,160,255,0,126,128,135,9,0,15,131,182,12,100,31,143,96,152,21,0,230,25,59,203,177,52,14,177,48,26,11,197,245,3,236,192,195,31,192,195,163,165,133,150,22,48,211,121,128,36,146,2,254,128,135,57,0,15,131,182,12,218,50,228,31,143,100,152,183,1,0,204,51,48,207,0,244,192,195,99,128,240,90,42,11,180,180,208,210,2,32,219,7,8,254,128,135,57,0,15,131,182,12,218,50,228,31,143,64,216,7,4,128,121,6,230,25,43,7,195,36,26,95,148,221,246,3,48,23,40,123,107,215,3,116,63,143,68,218,94,160,9,2,180,180,0,116,31,15,53,202,110,39,64,0,152,103,0,124,124,77,4,249,154,192,107,2,59,135,227,44,132,194,44,26,199,69,217,109,63,48,23,56,187,42,187,27,244,63,143,4,250,15,36,75,16,160,165,133,150,22,228,31,143,249,219,230,70,2,0,204,51,48,207,0,236,124,77,4,124,77,224,53,129,215,4,32,70,119,14,236,63,79,240,243,160,165,133,150,22,228,31,143,4,152,7,0,48,207,192,60,3,236,124,77,4,124,77,224,53,129,215,4,27,138,162,0,138,162,0,26,4,246,254,62,127,31,200,122,32,235,1,228,0,252,3,240,15,243,12,204,51,228,192,195,3,15,143,150,22,90,90,246,254,62,127,31,200,122,32,235,1,228,0,252,3,240,15,243,12,204,51,228,192,195,3,15,143,150,22,90,90,27,7,130,0,26,4,182,254,62,127,31,200,122,164,0,252,3,240,15,243,12,164,192,195,3,15,143,150,22,27,202,2,36,202,2,36,26,4,228,0,252,3,240,15,243,12,204,51,246,0,248,2,192,23,76,91,48,109,1,228,63,207,207,131,150,22,90,90,228,0,252,3,240,15,243,12,204,51,246,0,248,2,192,23,76,91,48,109,1,228,63,207,207,131,150,22,90,90,27,8,2,28,26,4,164,0,252,3,240,15,243,12,182,0,248,2,192,23,76,91,164,63,207,207,131,150,22,27,10,176,36,10,176,36,26,4,228,192,195,3,15,143,150,22,90,90,246,128,135,1,60,12,218,50,104,203,0,228,31,207,199,3,243,12,204,51,228,192,195,3,15,143,150,22,90,90,246,128,135,1,60,12,218,50,104,203,0,228,31,207,199,3,243,12,204,51,27,8,128,28,26,4,164,192,195,3,15,143,150,22,182,128,135,1,60,12,218,50,164,31,207,199,3,243,12,27,64,146,40,64,146,40,26,4,228,63,207,207,131,150,22,90,90,228,31,207,199,3,243,12,204,51,228,124,77,240,53,129,215,4,94,19,228,63,207,207,131,150,22,90,90,228,31,207,199,3,243,12,204,51,228,124,77,240,53,129,215,4,94,19,27,192,113,32,26,4,164,63,207,207,131,150,22,164,31,207,199,3,243,12,164,124,77,240,53,129,215,4 };
				[[maybe_unused]] const std::uint8_t paletteBitStream_bytes[] ={ 8, 33, 195, 128, 128, 124, 128, 128, 128, 129, 128, 199, 128, 128, 124, 128, 128, 128, 129, 128, 216, 128, 128, 124, 128, 128, 128, 129, 128, 0, 8, 33, 187, 128, 128, 130, 128, 128, 128, 128, 128, 191, 128, 128, 130, 128, 128, 128, 128, 128, 209, 128, 128, 130, 128, 128, 128, 128, 128, 0, 0, 0, 82, 80, 18, 64, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 214, 106, 18, 64, 1, 0, 0, 0, 8, 33, 195, 128, 128, 124, 128, 128, 128, 129, 128, 199, 128, 128, 124, 128, 128, 128, 129, 128, 216, 128, 128, 124, 128, 128, 128, 129, 128, 0, 198, 24, 241, 248, 8, 128, 128, 128, 128, 90, 128, 241, 8, 128, 248, 128, 128, 128, 147, 184, 241, 128, 248, 8, 128, 128, 128, 147, 72, 0 };

				h1_asset->lightGrid.tableVersion = 1;
				h1_asset->lightGrid.paletteVersion = 1;
				h1_asset->lightGrid.paletteEntryCount = asset->lightGrid.entryCount;
				h1_asset->lightGrid.paletteEntryAddress = mem.allocate<int>(h1_asset->lightGrid.paletteEntryCount);
				for (unsigned int i = 0; i < h1_asset->lightGrid.paletteEntryCount; i++)
				{
					h1_asset->lightGrid.paletteEntryAddress[i] = 30; // 0, 30, 86, 116 //asset->lightGrid.entries[i].colorsIndex;
				}

				h1_asset->lightGrid.paletteBitstreamSize = sizeof(paletteBitStream_bytes);
				h1_asset->lightGrid.paletteBitstream = mem.allocate<unsigned char>(h1_asset->lightGrid.paletteBitstreamSize);
				memcpy(h1_asset->lightGrid.paletteBitstream, paletteBitStream_bytes, sizeof(paletteBitStream_bytes));

				h1_asset->lightGrid.missingGridColorIndex = h1_asset->lightGrid.paletteEntryCount - 1;

				h1_asset->lightGrid.rangeExponent8BitsEncoding = 0;
				h1_asset->lightGrid.rangeExponent12BitsEncoding = 4;
				h1_asset->lightGrid.rangeExponent16BitsEncoding = 23;

				h1_asset->lightGrid.stageCount = 1;
				h1_asset->lightGrid.stageLightingContrastGain = mem.allocate<float>(1);
				h1_asset->lightGrid.stageLightingContrastGain[0] = 0.3f;

				//memcpy(&h1_asset->lightGrid.skyLightGridColors, skyLightGridColors_bytes, sizeof(skyLightGridColors_bytes));
				//memcpy(&h1_asset->lightGrid.defaultLightGridColors, defaultLightGridColors_bytes, sizeof(defaultLightGridColors_bytes));

				for (auto i = 0; i < 3; i++)
				{
					h1_asset->lightGrid.tree[i].index = i;
				}
			}
			// ----

			h1_asset->modelCount = asset->modelCount;
			h1_asset->models = mem.allocate<H1::GfxBrushModel>(h1_asset->modelCount);
			for (int i = 0; i < h1_asset->modelCount; i++)
			{
				int decals = asset->models[i].surfaceCount - asset->models[i].surfaceCountNoDecal;

				//memcpy(&h1_asset->models[i].writable.bounds, &asset->models[i].writable.bounds, sizeof(float[2][3])); // Irrevelant
				memcpy(&h1_asset->models[i].bounds, &asset->models[i].bounds, sizeof(float[2][3]));

				h1_asset->models[i].radius = asset->models[i].radius;
				h1_asset->models[i].startSurfIndex = asset->models[i].startSurfIndex;
				h1_asset->models[i].surfaceCount = asset->models[i].surfaceCountNoDecal + decals;
				h1_asset->models[i].mdaoVolumeIndex = -1;
			}

			memcpy(h1_asset->shadowBounds.midPoint, asset->bounds.midPoint, sizeof(float[3]));
			memcpy(h1_asset->shadowBounds.halfSize, asset->bounds.halfSize, sizeof(float[3]));
			memcpy(h1_asset->unkBounds.midPoint, asset->bounds.midPoint, sizeof(float[3]));
			memcpy(h1_asset->unkBounds.halfSize, asset->bounds.halfSize, sizeof(float[3]));

			// bounds1 = ?
			// bounds2 = shadowBounds;

			h1_asset->checksum = asset->checksum;

			h1_asset->materialMemoryCount = asset->materialMemoryCount;
			h1_asset->materialMemory = mem.allocate<H1::MaterialMemory>(h1_asset->materialMemoryCount);
			for (int i = 0; i < h1_asset->materialMemoryCount; i++)
			{
				h1_asset->materialMemory[i].material = reinterpret_cast<H1::Material * __ptr64>(asset->materialMemory[i].material);
				h1_asset->materialMemory[i].memory = asset->materialMemory[i].memory;
			}

			h1_asset->sun.hasValidData = asset->sun.hasValidData;
			h1_asset->sun.spriteMaterial = reinterpret_cast<H1::Material * __ptr64>(asset->sun.spriteMaterial);
			h1_asset->sun.flareMaterial = reinterpret_cast<H1::Material * __ptr64>(asset->sun.flareMaterial);
			memcpy(&h1_asset->sun.spriteSize, &asset->sun.spriteSize, Difference(&asset->sun.sunFxPosition, &asset->sun.spriteSize) + sizeof(float[3]));

			memcpy(&h1_asset->outdoorLookupMatrix, &asset->outdoorLookupMatrix, sizeof(float[4][4]));

			h1_asset->outdoorImage = mem.allocate<H1::GfxImage>();
			h1_asset->outdoorImage->name = asset->outdoorImage->name;

			h1_asset->cellCasterBits = mem.allocate<unsigned int>(h1_asset->dpvsPlanes.cellCount * ((h1_asset->dpvsPlanes.cellCount + 31) >> 5));
			for (int i = 0; i < asset->dpvsPlanes.cellCount * ((asset->dpvsPlanes.cellCount + 31) >> 5); i++)
			{
				h1_asset->cellCasterBits[i] = asset->cellCasterBits[i];
			}
			h1_asset->cellHasSunLitSurfsBits = mem.allocate<unsigned int>((h1_asset->dpvsPlanes.cellCount + 31) >> 5); // todo?

			h1_asset->sceneDynModel = mem.allocate<H1::GfxSceneDynModel>(asset->dpvsDyn.dynEntClientCount[0]);
			for (unsigned int i = 0; i < asset->dpvsDyn.dynEntClientCount[0]; i++)
			{
				h1_asset->sceneDynModel[i].info.hasGfxEntIndex = asset->sceneDynModel[i].info.hasGfxEntIndex;
				h1_asset->sceneDynModel[i].info.lod = asset->sceneDynModel[i].info.lod;
				h1_asset->sceneDynModel[i].info.surfId = asset->sceneDynModel[i].info.surfId;
				h1_asset->sceneDynModel[i].dynEntId = asset->sceneDynModel[i].dynEntId;
			}
			REINTERPRET_CAST_SAFE(h1_asset->sceneDynBrush, asset->sceneDynBrush);

			//h1_asset->primaryLightEntityShadowVis = reinterpret_cast<unsigned int* __ptr64>(asset->primaryLightEntityShadowVis);
			int count = ((h1_asset->primaryLightCount - h1_asset->lastSunPrimaryLightIndex) << 13) - 0x2000;
			h1_asset->primaryLightEntityShadowVis = mem.allocate<unsigned int>(count);
			for (unsigned int i = 0; i < count; i++)
			{
				h1_asset->primaryLightEntityShadowVis[i] = asset->primaryLightEntityShadowVis[i];
			}

			h1_asset->primaryLightDynEntShadowVis[0] = reinterpret_cast<unsigned int* __ptr64>(asset->primaryLightDynEntShadowVis[0]);
			h1_asset->primaryLightDynEntShadowVis[1] = reinterpret_cast<unsigned int* __ptr64>(asset->primaryLightDynEntShadowVis[1]);

			//h1_asset->nonSunPrimaryLightForModelDynEnt = reinterpret_cast<unsigned __int16* __ptr64>(asset->primaryLightForModelDynEnt);
			h1_asset->nonSunPrimaryLightForModelDynEnt = mem.allocate<unsigned short>(asset->dpvsDyn.dynEntClientCount[0]);
			for (unsigned int i = 0; i < asset->dpvsDyn.dynEntClientCount[0]; i++)
			{
				h1_asset->nonSunPrimaryLightForModelDynEnt[i] = asset->nonSunPrimaryLightForModelDynEnt[i];
			}

			if (asset->shadowGeom)
			{
				h1_asset->shadowGeom = mem.allocate<H1::GfxShadowGeometry>(h1_asset->primaryLightCount);
				for (unsigned int i = 0; i < h1_asset->primaryLightCount; i++)
				{
					h1_asset->shadowGeom[i].surfaceCount = asset->shadowGeom[i].surfaceCount;
					h1_asset->shadowGeom[i].smodelCount = asset->shadowGeom[i].smodelCount;

					h1_asset->shadowGeom[i].sortedSurfIndex = mem.allocate<unsigned int>(h1_asset->shadowGeom[i].surfaceCount);
					for (unsigned int j = 0; j < h1_asset->shadowGeom[i].surfaceCount; j++)
					{
						h1_asset->shadowGeom[i].sortedSurfIndex[j] = asset->shadowGeom[i].sortedSurfIndex[j];
					}
					REINTERPRET_CAST_SAFE(h1_asset->shadowGeom[i].smodelIndex, asset->shadowGeom[i].smodelIndex);
				}
			}
			h1_asset->shadowGeomOptimized = h1_asset->shadowGeom;

			h1_asset->lightRegion = mem.allocate<H1::GfxLightRegion>(h1_asset->primaryLightCount);
			for (unsigned int i = 0; i < h1_asset->primaryLightCount; i++)
			{
				h1_asset->lightRegion[i].hullCount = asset->lightRegion[i].hullCount;
				h1_asset->lightRegion[i].hulls = mem.allocate<H1::GfxLightRegionHull>(h1_asset->lightRegion[i].hullCount);
				for (unsigned int j = 0; j < h1_asset->lightRegion[i].hullCount; j++)
				{
					memcpy(&h1_asset->lightRegion[i].hulls[j].kdopMidPoint, &asset->lightRegion[i].hulls[j].kdopMidPoint, sizeof(float[9]));
					memcpy(&h1_asset->lightRegion[i].hulls[j].kdopHalfSize, &asset->lightRegion[i].hulls[j].kdopHalfSize, sizeof(float[9]));

					h1_asset->lightRegion[i].hulls[j].axisCount = asset->lightRegion[i].hulls[j].axisCount;
					REINTERPRET_CAST_SAFE(h1_asset->lightRegion[i].hulls[j].axis, asset->lightRegion[i].hulls[j].axis);
				}
			}

			unsigned int lit_decal_count = asset->dpvs.staticSurfaceCount - asset->dpvs.staticSurfaceCountNoDecal;

			h1_asset->dpvs.smodelCount = asset->dpvs.smodelCount;
			h1_asset->dpvs.subdivVertexLightingInfoCount = 0;
			h1_asset->dpvs.staticSurfaceCount = asset->dpvs.staticSurfaceCountNoDecal + lit_decal_count;
			h1_asset->dpvs.litOpaqueSurfsBegin = asset->dpvs.litOpaqueSurfsBegin;
			h1_asset->dpvs.litOpaqueSurfsEnd = asset->dpvs.litOpaqueSurfsEnd;
			h1_asset->dpvs.unkSurfsBegin = 0;
			h1_asset->dpvs.unkSurfsEnd = 0;
			h1_asset->dpvs.litDecalSurfsBegin = asset->dpvs.litOpaqueSurfsEnd; // skip
			h1_asset->dpvs.litDecalSurfsEnd = asset->dpvs.litOpaqueSurfsEnd; // skip
			h1_asset->dpvs.litTransSurfsBegin = asset->dpvs.litTransSurfsBegin;
			h1_asset->dpvs.litTransSurfsEnd = asset->dpvs.litTransSurfsEnd;
			h1_asset->dpvs.shadowCasterSurfsBegin = asset->dpvs.shadowCasterSurfsBegin;
			h1_asset->dpvs.shadowCasterSurfsEnd = asset->dpvs.shadowCasterSurfsEnd;
			h1_asset->dpvs.emissiveSurfsBegin = asset->dpvs.emissiveSurfsBegin;
			h1_asset->dpvs.emissiveSurfsEnd = asset->dpvs.emissiveSurfsEnd;
			h1_asset->dpvs.smodelVisDataCount = asset->dpvs.smodelVisDataCount;
			h1_asset->dpvs.surfaceVisDataCount = asset->dpvs.surfaceVisDataCount;

			for (auto i = 0; i < 4; i++)
			{
				h1_asset->dpvs.smodelVisData[i] = mem.allocate<unsigned int>(h1_asset->dpvs.smodelVisDataCount);
			}

			for (auto i = 0; i < 4; i++)
			{
				h1_asset->dpvs.surfaceVisData[i] = mem.allocate<unsigned int>(h1_asset->dpvs.surfaceVisDataCount);
			}

			for (auto i = 0; i < 3; i++)
			{
				//memcpy(h1_asset->dpvs.smodelVisData[i], asset->dpvs.smodelVisData[i], sizeof(int) * h1_asset->dpvs.smodelVisDataCount);
				//memcpy(h1_asset->dpvs.surfaceVisData[i], asset->dpvs.surfaceVisData[i], sizeof(int) * h1_asset->dpvs.surfaceVisDataCount);
			}

			for (auto i = 0; i < 27; i++)
			{
				h1_asset->dpvs.smodelUnknownVisData[i] = mem.allocate<unsigned int>(h1_asset->dpvs.smodelVisDataCount);
			}

			for (auto i = 0; i < 27; i++)
			{
				h1_asset->dpvs.surfaceUnknownVisData[i] = mem.allocate<unsigned int>(h1_asset->dpvs.surfaceVisDataCount);
			}

			for (auto i = 0; i < 4; i++)
			{
				h1_asset->dpvs.smodelUmbraVisData[i] = mem.allocate<unsigned int>(h1_asset->dpvs.smodelVisDataCount);
			}

			for (auto i = 0; i < 4; i++)
			{
				h1_asset->dpvs.surfaceUmbraVisData[i] = mem.allocate<unsigned int>(h1_asset->dpvs.surfaceVisDataCount);
			}

			h1_asset->dpvs.unknownSModelVisData1 = mem.allocate<unsigned int>(h1_asset->dpvs.smodelVisDataCount);
			h1_asset->dpvs.unknownSModelVisData2 = mem.allocate<unsigned int>(h1_asset->dpvs.smodelVisDataCount * 2);

			h1_asset->dpvs.lodData = mem.allocate<unsigned int>(h1_asset->dpvs.smodelCount + 1);
			h1_asset->dpvs.tessellationCutoffVisData = mem.allocate<unsigned int>(h1_asset->dpvs.surfaceVisDataCount);

			h1_asset->dpvs.sortedSurfIndex = mem.allocate<unsigned int>(h1_asset->dpvs.staticSurfaceCount);
			for (unsigned int i = 0; i < h1_asset->dpvs.staticSurfaceCount; i++)
			{
				h1_asset->dpvs.sortedSurfIndex[i] = asset->dpvs.sortedSurfIndex[i];
			}

			REINTERPRET_CAST_SAFE(h1_asset->dpvs.smodelInsts, asset->dpvs.smodelInsts);

			h1_asset->dpvs.surfaces = mem.allocate<H1::GfxSurface>(h1_asset->surfaceCount);
			for (unsigned int i = 0; i < h1_asset->surfaceCount; i++)
			{
				h1_asset->dpvs.surfaces[i].tris.vertexLayerData = asset->dpvs.surfaces[i].tris.vertexLayerData;
				h1_asset->dpvs.surfaces[i].tris.firstVertex = asset->dpvs.surfaces[i].tris.firstVertex;
				h1_asset->dpvs.surfaces[i].tris.maxEdgeLength = 0;
				h1_asset->dpvs.surfaces[i].tris.unk = -1;
				h1_asset->dpvs.surfaces[i].tris.vertexCount = asset->dpvs.surfaces[i].tris.vertexCount;
				h1_asset->dpvs.surfaces[i].tris.triCount = asset->dpvs.surfaces[i].tris.triCount;
				h1_asset->dpvs.surfaces[i].tris.baseIndex = asset->dpvs.surfaces[i].tris.baseIndex;
				h1_asset->dpvs.surfaces[i].material = reinterpret_cast<H1::Material * __ptr64>(asset->dpvs.surfaces[i].material);
				h1_asset->dpvs.surfaces[i].laf.fields.lightmapIndex = asset->dpvs.surfaces[i].laf.fields.lightmapIndex;
				h1_asset->dpvs.surfaces[i].laf.fields.reflectionProbeIndex = asset->dpvs.surfaces[i].laf.fields.reflectionProbeIndex;
				h1_asset->dpvs.surfaces[i].laf.fields.primaryLightEnvIndex = asset->dpvs.surfaces[i].laf.fields.primaryLightIndex;
				h1_asset->dpvs.surfaces[i].laf.fields.flags = asset->dpvs.surfaces[i].laf.fields.flags;
			}

			h1_asset->dpvs.surfacesBounds = mem.allocate<H1::GfxSurfaceBounds>(h1_asset->surfaceCount);
			for (unsigned int i = 0; i < h1_asset->surfaceCount; i++)
			{
				memcpy(&h1_asset->dpvs.surfacesBounds[i].bounds, &asset->dpvs.surfacesBounds[i].bounds, sizeof(IW5::Bounds));
				h1_asset->dpvs.surfacesBounds[i].__pad0; // idk
			}

			h1_asset->dpvs.smodelDrawInsts = mem.allocate<H1::GfxStaticModelDrawInst>(h1_asset->dpvs.smodelCount);
			for (unsigned int i = 0; i < h1_asset->dpvs.smodelCount; i++)
			{
				memcpy(&h1_asset->dpvs.smodelDrawInsts[i].placement, &asset->dpvs.smodelDrawInsts[i].placement, sizeof(IW5::GfxPackedPlacement));
				h1_asset->dpvs.smodelDrawInsts[i].model = reinterpret_cast<H1::XModel * __ptr64>(asset->dpvs.smodelDrawInsts[i].model);
				h1_asset->dpvs.smodelDrawInsts[i].lightingHandle = asset->dpvs.smodelDrawInsts[i].lightingHandle;
				h1_asset->dpvs.smodelDrawInsts[i].staticModelId = 0;
				h1_asset->dpvs.smodelDrawInsts[i].primaryLightEnvIndex = asset->dpvs.smodelDrawInsts[i].primaryLightIndex;
				h1_asset->dpvs.smodelDrawInsts[i].reflectionProbeIndex = asset->dpvs.smodelDrawInsts[i].reflectionProbeIndex;
				h1_asset->dpvs.smodelDrawInsts[i].firstMtlSkinIndex = asset->dpvs.smodelDrawInsts[i].firstMtlSkinIndex;
				h1_asset->dpvs.smodelDrawInsts[i].sunShadowFlags = 1;

				h1_asset->dpvs.smodelDrawInsts[i].cullDist = asset->dpvs.smodelDrawInsts[i].cullDist;
				h1_asset->dpvs.smodelDrawInsts[i].unk0 = h1_asset->dpvs.smodelDrawInsts[i].cullDist;
				h1_asset->dpvs.smodelDrawInsts[i].unk1 = 0;

				// casts no shadows
				auto no_shadows = (asset->dpvs.smodelDrawInsts[i].flags & 0x10) != 0;
				if (no_shadows)
				{
					h1_asset->dpvs.smodelDrawInsts[i].flags |= H1::StaticModelFlag::STATIC_MODEL_FLAG_NO_CAST_SHADOW;
				}

				// ground lighting
				auto ground_lighting = (asset->dpvs.smodelDrawInsts[i].flags & 0x20) != 0 || asset->dpvs.smodelDrawInsts[i].groundLighting.packed != 0;
				if (ground_lighting)
				{
					h1_asset->dpvs.smodelDrawInsts[i].flags |= H1::StaticModelFlag::STATIC_MODEL_FLAG_GROUND_LIGHTING;
				}
				// regular lighting
				else
				{
					h1_asset->dpvs.smodelDrawInsts[i].flags |= H1::StaticModelFlag::STATIC_MODEL_FLAG_LIGHTGRID_LIGHTING;
				}
			}

			h1_asset->dpvs.smodelLighting = mem.allocate<H1::GfxStaticModelLighting>(h1_asset->dpvs.smodelCount);
			for (unsigned int i = 0; i < h1_asset->dpvs.smodelCount; i++)
			{
				if ((h1_asset->dpvs.smodelDrawInsts[i].flags & H1::StaticModelFlag::STATIC_MODEL_FLAG_GROUND_LIGHTING) != 0)
				{
					//bgra -> rgba
					auto ground_lighting = asset->dpvs.smodelDrawInsts[i].groundLighting;
					auto bgra = ground_lighting.array;

					float rgba[4] = { bgra[2] / 255.0f, bgra[1] / 255.0f, bgra[0] / 255.0f, bgra[3] / 255.0f };

					h1_asset->dpvs.smodelLighting[i].modelGroundLightingInfo.groundLighting[0] = float_to_half(rgba[0]); // r
					h1_asset->dpvs.smodelLighting[i].modelGroundLightingInfo.groundLighting[1] = float_to_half(rgba[1]); // g
					h1_asset->dpvs.smodelLighting[i].modelGroundLightingInfo.groundLighting[2] = float_to_half(rgba[2]); // b
					h1_asset->dpvs.smodelLighting[i].modelGroundLightingInfo.groundLighting[3] = float_to_half(rgba[3]); // a
				}
				else if ((h1_asset->dpvs.smodelDrawInsts[i].flags & H1::StaticModelFlag::STATIC_MODEL_FLAG_LIGHTGRID_LIGHTING) != 0)
				{
					//h1_asset->dpvs.smodelDrawInsts[i].flags |= H1::StaticModelFlag::STATIC_MODEL_FLAG_ALLOW_FXMARK; // R_CalcModelLighting: 0x240
					h1_asset->dpvs.smodelLighting[i].modelLightGridLightingInfo.lighting[0] = float_to_half(0.5f); // r
					h1_asset->dpvs.smodelLighting[i].modelLightGridLightingInfo.lighting[1] = float_to_half(0.5f); // g
					h1_asset->dpvs.smodelLighting[i].modelLightGridLightingInfo.lighting[2] = float_to_half(0.5f); // b
					h1_asset->dpvs.smodelLighting[i].modelLightGridLightingInfo.lighting[3] = float_to_half(0.5f); // a
				}
				else if ((h1_asset->dpvs.smodelDrawInsts[i].flags & H1::StaticModelFlag::STATIC_MODEL_FLAG_LIGHTMAP_LIGHTING) != 0)
				{
					// todo?
				}
				else if ((h1_asset->dpvs.smodelDrawInsts[i].flags & H1::StaticModelFlag::STATIC_MODEL_FLAG_VERTEXLIT_LIGHTING) != 0)
				{
					// todo?
				}
			}

			h1_asset->dpvs.subdivVertexLighting = nullptr;

			h1_asset->dpvs.surfaceMaterials = mem.allocate<H1::GfxDrawSurf>(h1_asset->surfaceCount);
			for (unsigned int i = 0; i < h1_asset->surfaceCount; i++) // these are probably wrong
			{
				h1_asset->dpvs.surfaceMaterials[i].fields.objectId = asset->dpvs.surfaceMaterials[i].fields.objectId;
				h1_asset->dpvs.surfaceMaterials[i].fields.reflectionProbeIndex = asset->dpvs.surfaceMaterials[i].fields.reflectionProbeIndex;
				h1_asset->dpvs.surfaceMaterials[i].fields.hasGfxEntIndex = asset->dpvs.surfaceMaterials[i].fields.hasGfxEntIndex;
				h1_asset->dpvs.surfaceMaterials[i].fields.customIndex = asset->dpvs.surfaceMaterials[i].fields.customIndex;
				h1_asset->dpvs.surfaceMaterials[i].fields.materialSortedIndex = asset->dpvs.surfaceMaterials[i].fields.materialSortedIndex;
				h1_asset->dpvs.surfaceMaterials[i].fields.tessellation = 0;
				h1_asset->dpvs.surfaceMaterials[i].fields.prepass = asset->dpvs.surfaceMaterials[i].fields.prepass;
				h1_asset->dpvs.surfaceMaterials[i].fields.useHeroLighting = asset->dpvs.surfaceMaterials[i].fields.useHeroLighting;
				h1_asset->dpvs.surfaceMaterials[i].fields.sceneLightEnvIndex = asset->dpvs.surfaceMaterials[i].fields.sceneLightIndex;
				h1_asset->dpvs.surfaceMaterials[i].fields.viewModelRender = asset->dpvs.surfaceMaterials[i].fields.viewModelRender;
				h1_asset->dpvs.surfaceMaterials[i].fields.surfType = asset->dpvs.surfaceMaterials[i].fields.surfType;
				h1_asset->dpvs.surfaceMaterials[i].fields.primarySortKey = asset->dpvs.surfaceMaterials[i].fields.primarySortKey;
				h1_asset->dpvs.surfaceMaterials[i].fields.unused = asset->dpvs.surfaceMaterials[i].fields.unused;
			}

			REINTERPRET_CAST_SAFE(h1_asset->dpvs.surfaceCastsSunShadow, asset->dpvs.surfaceCastsSunShadow);
			//h1_asset->dpvs.sunShadowOptCount = 1;
			//h1_asset->dpvs.sunSurfVisDataCount = h1_asset->dpvs.surfaceVisDataCount * 8;
			//h1_asset->dpvs.surfaceCastsSunShadowOpt = mem.allocate<unsigned int>(h1_asset->dpvs.sunShadowOptCount * h1_asset->dpvs.sunSurfVisDataCount);
			//memcpy(h1_asset->dpvs.surfaceCastsSunShadowOpt, h1_asset->dpvs.surfaceCastsSunShadow, sizeof(int) * (h1_asset->dpvs.sunShadowOptCount * h1_asset->dpvs.sunSurfVisDataCount));
			h1_asset->dpvs.surfaceDeptAndSurf = mem.allocate<H1::GfxDepthAndSurf>(h1_asset->dpvs.staticSurfaceCount); // todo?
			h1_asset->dpvs.constantBuffersLit = mem.allocate<char* __ptr64>(h1_asset->dpvs.smodelCount); //nullptr;
			h1_asset->dpvs.constantBuffersAmbient = mem.allocate<char* __ptr64>(h1_asset->dpvs.smodelCount); //nullptr;
			h1_asset->dpvs.usageCount = asset->dpvs.usageCount;

			h1_asset->dpvsDyn.dynEntClientWordCount[0] = asset->dpvsDyn.dynEntClientWordCount[0];
			h1_asset->dpvsDyn.dynEntClientWordCount[1] = asset->dpvsDyn.dynEntClientWordCount[1];
			h1_asset->dpvsDyn.dynEntClientCount[0] = asset->dpvsDyn.dynEntClientCount[0];
			h1_asset->dpvsDyn.dynEntClientCount[1] = asset->dpvsDyn.dynEntClientCount[1];
			h1_asset->dpvsDyn.dynEntCellBits[0] = reinterpret_cast<unsigned int* __ptr64>(asset->dpvsDyn.dynEntCellBits[0]);
			h1_asset->dpvsDyn.dynEntCellBits[1] = reinterpret_cast<unsigned int* __ptr64>(asset->dpvsDyn.dynEntCellBits[1]);
			h1_asset->dpvsDyn.dynEntVisData[0][0] = reinterpret_cast<unsigned char* __ptr64>(asset->dpvsDyn.dynEntVisData[0][0]);
			h1_asset->dpvsDyn.dynEntVisData[0][1] = reinterpret_cast<unsigned char* __ptr64>(asset->dpvsDyn.dynEntVisData[0][1]);
			h1_asset->dpvsDyn.dynEntVisData[0][2] = reinterpret_cast<unsigned char* __ptr64>(asset->dpvsDyn.dynEntVisData[0][2]);
			h1_asset->dpvsDyn.dynEntVisData[0][3] = mem.allocate<unsigned char>(h1_asset->dpvsDyn.dynEntClientWordCount[0] * 32);
			h1_asset->dpvsDyn.dynEntVisData[1][0] = reinterpret_cast<unsigned char* __ptr64>(asset->dpvsDyn.dynEntVisData[1][0]);
			h1_asset->dpvsDyn.dynEntVisData[1][1] = reinterpret_cast<unsigned char* __ptr64>(asset->dpvsDyn.dynEntVisData[1][1]);
			h1_asset->dpvsDyn.dynEntVisData[1][2] = reinterpret_cast<unsigned char* __ptr64>(asset->dpvsDyn.dynEntVisData[1][2]);
			h1_asset->dpvsDyn.dynEntVisData[1][3] = mem.allocate<unsigned char>(h1_asset->dpvsDyn.dynEntClientWordCount[1] * 32);

			h1_asset->mapVtxChecksum = asset->mapVtxChecksum;

			h1_asset->heroOnlyLightCount = asset->heroOnlyLightCount;
			REINTERPRET_CAST_SAFE(h1_asset->heroOnlyLights, asset->heroOnlyLights);

			h1_asset->fogTypesAllowed = asset->fogTypesAllowed;

			h1_asset->umbraTomeSize = 0;
			h1_asset->umbraTomeData = nullptr;
			h1_asset->umbraTomePtr = nullptr;
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

				h1_asset->umbraTomeSize = new_tome->m_size;
				h1_asset->umbraTomeData = mem->ManualAlloc<char>(h1_asset->umbraTomeSize);
				memcpy(h1_asset->umbraTomeData, buffer, h1_asset->umbraTomeSize);
				h1_asset->umbraTomePtr = reinterpret_cast<void*>(h1_asset->umbraTomeData);
			}
			*/

			h1_asset->mdaoVolumesCount = 0;
			h1_asset->mdaoVolumes = nullptr;

			// pad3 unknown data

			h1_asset->buildInfo.args0 = nullptr;
			h1_asset->buildInfo.args1 = nullptr;
			h1_asset->buildInfo.buildStartTime = nullptr;
			h1_asset->buildInfo.buildEndTime = nullptr;

			return h1_asset;
		}

		H1::GfxWorld* convert(GfxWorld* asset, allocator& allocator)
		{
			// generate h1 gfxworld
			return GenerateH1GfxWorld(asset, allocator);
		}
	}
}