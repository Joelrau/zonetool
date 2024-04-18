#include "stdafx.hpp"
#include "../Include.hpp"

#include "GfxWorld.hpp"

#include "X64/Utils/Umbra/umbra.hpp"
#include "X64/Utils/Utils.hpp"

namespace ZoneTool::IW5
{
	namespace S1Converter
	{
		S1::GfxWorld* GenerateS1GfxWorld(GfxWorld* asset, allocator& mem)
		{
			// allocate S1 GfxWorld structure
			const auto s1_asset = mem.allocate<S1::GfxWorld>();

			s1_asset->name = asset->name;
			s1_asset->baseName = asset->baseName;

			s1_asset->bspVersion = 109;

			s1_asset->planeCount = asset->planeCount;
			s1_asset->nodeCount = asset->nodeCount;
			s1_asset->surfaceCount = asset->surfaceCount;
			s1_asset->skyCount = asset->skyCount;

			s1_asset->skies = mem.allocate<S1::GfxSky>(s1_asset->skyCount);
			for (int i = 0; i < s1_asset->skyCount; i++)
			{
				s1_asset->skies[i].skySurfCount = asset->skies[i].skySurfCount;
				REINTERPRET_CAST_SAFE(s1_asset->skies[i].skyStartSurfs, asset->skies[i].skyStartSurfs);
				if (asset->skies[i].skyImage)
				{
					s1_asset->skies[i].skyImage = mem.allocate<S1::GfxImage>();
					s1_asset->skies[i].skyImage->name = asset->skies[i].skyImage->name;
				}
				else
				{
					s1_asset->skies[i].skyImage = nullptr;
				}
				s1_asset->skies[i].skySamplerState = asset->skies[i].skySamplerState;

				// add bounds
				//assert(asset->skies[i].skySurfCount == 1);
				for (auto j = 0; j < asset->skies[i].skySurfCount; j++)
				{
					auto index = asset->dpvs.sortedSurfIndex[asset->skies[i].skyStartSurfs[j]];
					auto* surface_bounds = &asset->dpvs.surfacesBounds[index];
					memcpy(&s1_asset->skies[i].bounds, &surface_bounds->bounds, sizeof(surface_bounds->bounds));

					//
					break;
				}
			}

			s1_asset->portalGroupCount = 0;
			s1_asset->lastSunPrimaryLightIndex = asset->lastSunPrimaryLightIndex;
			s1_asset->primaryLightCount = asset->primaryLightCount;
			s1_asset->primaryLightEnvCount = asset->primaryLightCount + 1;
			s1_asset->sortKeyLitDecal = 7; // s1_asset->sortKeyLitDecal = asset->sortKeyLitDecal;
			s1_asset->sortKeyEffectDecal = 43; // s1_asset->sortKeyEffectDecal = asset->sortKeyEffectDecal;
			s1_asset->sortKeyTopDecal = 17;
			s1_asset->sortKeyEffectAuto = 53; // s1_asset->sortKeyEffectAuto = asset->sortKeyEffectAuto;
			s1_asset->sortKeyDistortion = 48; // s1_asset->sortKeyDistortion = asset->sortKeyDistortion;
			s1_asset->sortKeyHair = 18;
			s1_asset->sortKeyEffectBlend = 33;

			s1_asset->dpvsPlanes.cellCount = asset->dpvsPlanes.cellCount;
			REINTERPRET_CAST_SAFE(s1_asset->dpvsPlanes.planes, asset->dpvsPlanes.planes);
			REINTERPRET_CAST_SAFE(s1_asset->dpvsPlanes.nodes, asset->dpvsPlanes.nodes);

			s1_asset->dpvsPlanes.sceneEntCellBits = mem.allocate<unsigned int>(asset->dpvsPlanes.cellCount << 9);
			for (int i = 0; i < asset->dpvsPlanes.cellCount << 9; i++)
			{
				s1_asset->dpvsPlanes.sceneEntCellBits[i] = asset->dpvsPlanes.sceneEntCellBits[i];
			}

			s1_asset->aabbTreeCounts = mem.allocate<S1::GfxCellTreeCount>(s1_asset->dpvsPlanes.cellCount); //reinterpret_cast<IW6::GfxCellTreeCount* __ptr64>(asset->aabbTreeCounts);
			s1_asset->aabbTrees = mem.allocate<S1::GfxCellTree>(s1_asset->dpvsPlanes.cellCount);
			for (int i = 0; i < s1_asset->dpvsPlanes.cellCount; i++)
			{
				s1_asset->aabbTreeCounts[i].aabbTreeCount = asset->aabbTreeCounts[i].aabbTreeCount;
				s1_asset->aabbTrees[i].aabbTree = mem.allocate<S1::GfxAabbTree>(s1_asset->aabbTreeCounts[i].aabbTreeCount);
				for (int j = 0; j < s1_asset->aabbTreeCounts[i].aabbTreeCount; j++)
				{
					memcpy(&s1_asset->aabbTrees[i].aabbTree[j].bounds, &asset->aabbTrees[i].aabbTree[j].bounds, sizeof(float[2][3]));

					s1_asset->aabbTrees[i].aabbTree[j].startSurfIndex = asset->aabbTrees[i].aabbTree[j].startSurfIndex;
					s1_asset->aabbTrees[i].aabbTree[j].surfaceCount = asset->aabbTrees[i].aabbTree[j].surfaceCount;

					s1_asset->aabbTrees[i].aabbTree[j].smodelIndexCount = asset->aabbTrees[i].aabbTree[j].smodelIndexCount;
					REINTERPRET_CAST_SAFE(s1_asset->aabbTrees[i].aabbTree[j].smodelIndexes, asset->aabbTrees[i].aabbTree[j].smodelIndexes);

					s1_asset->aabbTrees[i].aabbTree[j].childCount = asset->aabbTrees[i].aabbTree[j].childCount;
					// re-calculate childrenOffset
					auto offset = asset->aabbTrees[i].aabbTree[j].childrenOffset;
					int childrenIndex = offset / sizeof(IW5::GfxAabbTree);
					int childrenOffset = childrenIndex * sizeof(S1::GfxAabbTree);
					s1_asset->aabbTrees[i].aabbTree[j].childrenOffset = childrenOffset;
				}
			}

			s1_asset->cells = mem.allocate<S1::GfxCell>(s1_asset->dpvsPlanes.cellCount);
			for (int i = 0; i < s1_asset->dpvsPlanes.cellCount; i++)
			{
				memcpy(&s1_asset->cells[i].bounds, &asset->cells[i].bounds, sizeof(float[2][3]));
				s1_asset->cells[i].portalCount = asset->cells[i].portalCount;

				auto add_portal = [](S1::GfxPortal* h1_portal, IW5::GfxPortal* iw5_portal)
				{
					//h1_portal->writable.isQueued = iw5_portal->writable.isQueued;
					//h1_portal->writable.isAncestor = iw5_portal->writable.isAncestor;
					//h1_portal->writable.recursionDepth = iw5_portal->writable.recursionDepth;
					//h1_portal->writable.hullPointCount = iw5_portal->writable.hullPointCount;
					//h1_portal->writable.hullPoints = reinterpret_cast<float(*__ptr64)[2]>(iw5_portal->writable.hullPoints);
					//h1_portal->writable.queuedParent = add_portal(iw5_portal->writable.queuedParent); // mapped at runtime

					memcpy(&h1_portal->plane, &iw5_portal->plane, sizeof(float[4]));
					h1_portal->vertices = reinterpret_cast<float(*__ptr64)[3]>(iw5_portal->vertices);
					h1_portal->cellIndex = iw5_portal->cellIndex;
					h1_portal->closeDistance = 0;
					h1_portal->vertexCount = iw5_portal->vertexCount;
					memcpy(&h1_portal->hullAxis, &iw5_portal->hullAxis, sizeof(float[2][3]));
				};
				s1_asset->cells[i].portals = mem.allocate<S1::GfxPortal>(s1_asset->cells[i].portalCount);
				for (int j = 0; j < s1_asset->cells[i].portalCount; j++)
				{
					add_portal(&s1_asset->cells[i].portals[j], &asset->cells[i].portals[j]);
				}

				s1_asset->cells[i].reflectionProbeCount = asset->cells[i].reflectionProbeCount;
				s1_asset->cells[i].reflectionProbes = reinterpret_cast<unsigned __int8* __ptr64>(asset->cells[i].reflectionProbes);
				s1_asset->cells[i].reflectionProbeReferenceCount = asset->cells[i].reflectionProbeReferenceCount;
				s1_asset->cells[i].reflectionProbeReferences = reinterpret_cast<unsigned __int8* __ptr64>(asset->cells[i].reflectionProbeReferences);
			}

			s1_asset->portalGroup = nullptr;

			s1_asset->unk_vec4_count_0 = 0;
			s1_asset->unk_vec4_0 = nullptr;

			s1_asset->draw.reflectionProbeCount = asset->draw.reflectionProbeCount;
			s1_asset->draw.reflectionProbes = mem.allocate<S1::GfxImage* __ptr64>(s1_asset->draw.reflectionProbeCount);
			s1_asset->draw.reflectionProbeOrigins = mem.allocate<S1::GfxReflectionProbe>(s1_asset->draw.reflectionProbeCount);
			s1_asset->draw.reflectionProbeTextures = mem.allocate<S1::GfxRawTexture>(s1_asset->draw.reflectionProbeCount);
			for (unsigned int i = 0; i < s1_asset->draw.reflectionProbeCount; i++)
			{
				s1_asset->draw.reflectionProbes[i] = mem.allocate<S1::GfxImage>();
				s1_asset->draw.reflectionProbes[i]->name = asset->draw.reflectionProbes[i]->name;
				memcpy(&s1_asset->draw.reflectionProbeOrigins[i].origin, &asset->draw.reflectionProbeOrigins[i].origin, sizeof(float[3]));
				s1_asset->draw.reflectionProbeOrigins[i].probeVolumeCount = 0;
				s1_asset->draw.reflectionProbeOrigins[i].probeVolumes = nullptr;
				//memcpy(&s1_asset->draw.reflectionProbeTextures[i], &asset->draw.reflectionProbeTextures[i].loadDef, 20);
			}
			s1_asset->draw.reflectionProbeReferenceCount = asset->draw.reflectionProbeReferenceCount;
			s1_asset->draw.reflectionProbeReferenceOrigins = reinterpret_cast<S1::GfxReflectionProbeReferenceOrigin * __ptr64>(
				asset->draw.reflectionProbeReferenceOrigins);
			s1_asset->draw.reflectionProbeReferences = reinterpret_cast<S1::GfxReflectionProbeReference * __ptr64>(
				asset->draw.reflectionProbeReferences);

			s1_asset->draw.lightmapCount = asset->draw.lightmapCount;
			s1_asset->draw.lightmaps = mem.allocate<S1::GfxLightmapArray>(s1_asset->draw.lightmapCount);
			s1_asset->draw.lightmapPrimaryTextures = mem.allocate<S1::GfxRawTexture>(s1_asset->draw.lightmapCount);
			s1_asset->draw.lightmapSecondaryTextures = mem.allocate<S1::GfxRawTexture>(s1_asset->draw.lightmapCount);
			for (int i = 0; i < s1_asset->draw.lightmapCount; i++)
			{
				s1_asset->draw.lightmaps[i].primary = mem.allocate<S1::GfxImage>();
				s1_asset->draw.lightmaps[i].primary->name = asset->draw.lightmaps[i].primary->name;
				s1_asset->draw.lightmaps[i].secondary = mem.allocate<S1::GfxImage>();
				s1_asset->draw.lightmaps[i].secondary->name = asset->draw.lightmaps[i].secondary->name;

				//memcpy(&s1_asset->draw.lightmapPrimaryTextures[i], &asset->draw.lightmapPrimaryTextures[i].loadDef, 20);
				//memcpy(&s1_asset->draw.lightmapSecondaryTextures[i], &asset->draw.lightmapSecondaryTextures[i].loadDef, 20);
			}
			if (asset->draw.lightmapOverridePrimary)
			{
				s1_asset->draw.lightmapOverridePrimary = mem.allocate<S1::GfxImage>();
				s1_asset->draw.lightmapOverridePrimary->name = asset->draw.lightmapOverridePrimary->name;
			}
			else
			{
				s1_asset->draw.lightmapOverridePrimary = nullptr;
			}

			if (asset->draw.lightmapOverrideSecondary)
			{
				s1_asset->draw.lightmapOverrideSecondary = mem.allocate<S1::GfxImage>();
				s1_asset->draw.lightmapOverrideSecondary->name = asset->draw.lightmapOverrideSecondary->name;
			}
			else
			{
				s1_asset->draw.lightmapOverrideSecondary = nullptr;
			}

			s1_asset->draw.u1[0] = 1024; s1_asset->draw.u1[1] = 1024; // u1
			s1_asset->draw.u2[0] = 512; s1_asset->draw.u2[1] = 512; // u2
			s1_asset->draw.u3 = 8; // u3

			s1_asset->draw.trisType = 0; // dunno

			s1_asset->draw.vertexCount = asset->draw.vertexCount;
			s1_asset->draw.vd.vertices = mem.allocate<S1::GfxWorldVertex>(s1_asset->draw.vertexCount);
			for (unsigned int i = 0; i < s1_asset->draw.vertexCount; i++)
			{
				memcpy(&s1_asset->draw.vd.vertices[i], &asset->draw.vd.vertices[i], sizeof(IW5::GfxWorldVertex));

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

				s1_asset->draw.vd.vertices[i].normal.packed = PackedVec::Vec3PackUnitVecWithAlpha(normal, 1.0f);
				s1_asset->draw.vd.vertices[i].tangent.packed = PackedVec::Vec3PackUnitVecWithAlpha(tangent, sign);

				// correct color : bgra->rgba
				s1_asset->draw.vd.vertices[i].color.array[0] = asset->draw.vd.vertices[i].color.array[2];
				s1_asset->draw.vd.vertices[i].color.array[1] = asset->draw.vd.vertices[i].color.array[1];
				s1_asset->draw.vd.vertices[i].color.array[2] = asset->draw.vd.vertices[i].color.array[0];
				s1_asset->draw.vd.vertices[i].color.array[3] = asset->draw.vd.vertices[i].color.array[3];
			}

			s1_asset->draw.vertexLayerDataSize = asset->draw.vertexLayerDataSize;
			REINTERPRET_CAST_SAFE(s1_asset->draw.vld.data, asset->draw.vld.data);

			s1_asset->draw.indexCount = asset->draw.indexCount;
			REINTERPRET_CAST_SAFE(s1_asset->draw.indices, asset->draw.indices);

			s1_asset->lightGrid.hasLightRegions = asset->lightGrid.hasLightRegions;
			s1_asset->lightGrid.useSkyForLowZ = 0;
			s1_asset->lightGrid.lastSunPrimaryLightIndex = asset->lightGrid.lastSunPrimaryLightIndex;
			memcpy(&s1_asset->lightGrid.mins, &asset->lightGrid.mins, sizeof(short[3]));
			memcpy(&s1_asset->lightGrid.maxs, &asset->lightGrid.maxs, sizeof(short[3]));
			s1_asset->lightGrid.rowAxis = asset->lightGrid.rowAxis;
			s1_asset->lightGrid.colAxis = asset->lightGrid.colAxis;
			REINTERPRET_CAST_SAFE(s1_asset->lightGrid.rowDataStart, asset->lightGrid.rowDataStart);
			s1_asset->lightGrid.rawRowDataSize = asset->lightGrid.rawRowDataSize;
			REINTERPRET_CAST_SAFE(s1_asset->lightGrid.rawRowData, asset->lightGrid.rawRowData);
			s1_asset->lightGrid.entryCount = asset->lightGrid.entryCount;
			s1_asset->lightGrid.entries = mem.allocate<S1::GfxLightGridEntry>(s1_asset->lightGrid.entryCount);
			for (unsigned int i = 0; i < s1_asset->lightGrid.entryCount; i++)
			{
				s1_asset->lightGrid.entries[i].colorsIndex = asset->lightGrid.entries[i].colorsIndex;
				s1_asset->lightGrid.entries[i].primaryLightEnvIndex = asset->lightGrid.entries[i].primaryLightIndex;
				s1_asset->lightGrid.entries[i].unused = 0;
				s1_asset->lightGrid.entries[i].needsTrace = asset->lightGrid.entries[i].needsTrace;
			}
			s1_asset->lightGrid.colorCount = asset->lightGrid.colorCount;
			s1_asset->lightGrid.colors = mem.allocate<S1::GfxLightGridColors>(s1_asset->lightGrid.colorCount);
			for (unsigned int i = 0; i < s1_asset->lightGrid.colorCount; i++)
			{
				for (unsigned int j = 0; j < 56; j++)
				{
					auto& rgb = asset->lightGrid.colors[i].rgb[j];
					auto& dest_rgb = s1_asset->lightGrid.colors[i].rgb[j];
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

				s1_asset->lightGrid.tableVersion = 1;
				s1_asset->lightGrid.paletteVersion = 1;
				s1_asset->lightGrid.paletteEntryCount = asset->lightGrid.entryCount;
				s1_asset->lightGrid.paletteEntryAddress = mem.allocate<int>(s1_asset->lightGrid.paletteEntryCount);
				for (unsigned int i = 0; i < s1_asset->lightGrid.paletteEntryCount; i++)
				{
					s1_asset->lightGrid.paletteEntryAddress[i] = 30; // 0, 30, 86, 116 //asset->lightGrid.entries[i].colorsIndex;
				}

				s1_asset->lightGrid.paletteBitstreamSize = sizeof(paletteBitStream_bytes);
				s1_asset->lightGrid.paletteBitstream = mem.allocate<unsigned char>(s1_asset->lightGrid.paletteBitstreamSize);
				memcpy(s1_asset->lightGrid.paletteBitstream, paletteBitStream_bytes, sizeof(paletteBitStream_bytes));

				s1_asset->lightGrid.missingGridColorIndex = s1_asset->lightGrid.paletteEntryCount - 1;

				s1_asset->lightGrid.rangeExponent8BitsEncoding = 0;
				s1_asset->lightGrid.rangeExponent12BitsEncoding = 4;
				s1_asset->lightGrid.rangeExponent16BitsEncoding = 23;

				s1_asset->lightGrid.stageCount = 1;
				s1_asset->lightGrid.stageLightingContrastGain = mem.allocate<float>(1);
				s1_asset->lightGrid.stageLightingContrastGain[0] = 0.3f;

				//memcpy(&s1_asset->lightGrid.skyLightGridColors, skyLightGridColors_bytes, sizeof(skyLightGridColors_bytes));
				//memcpy(&s1_asset->lightGrid.defaultLightGridColors, defaultLightGridColors_bytes, sizeof(defaultLightGridColors_bytes));

				for (auto i = 0; i < 3; i++)
				{
					s1_asset->lightGrid.tree[i].index = i;
				}
			}
			// ----

			s1_asset->modelCount = asset->modelCount;
			s1_asset->models = mem.allocate<S1::GfxBrushModel>(s1_asset->modelCount);
			for (int i = 0; i < s1_asset->modelCount; i++)
			{
				int decals = asset->models[i].surfaceCount - asset->models[i].surfaceCountNoDecal;

				//memcpy(&s1_asset->models[i].writable.bounds, &asset->models[i].writable.bounds, sizeof(float[2][3])); // Irrevelant
				memcpy(&s1_asset->models[i].bounds, &asset->models[i].bounds, sizeof(float[2][3]));

				s1_asset->models[i].radius = asset->models[i].radius;
				s1_asset->models[i].startSurfIndex = asset->models[i].startSurfIndex;
				s1_asset->models[i].surfaceCount = asset->models[i].surfaceCountNoDecal + decals;
				s1_asset->models[i].mdaoVolumeIndex = -1;
			}

			memcpy(s1_asset->shadowBounds.midPoint, asset->bounds.midPoint, sizeof(float[3]));
			memcpy(s1_asset->shadowBounds.halfSize, asset->bounds.halfSize, sizeof(float[3]));
			memcpy(s1_asset->unkBounds.midPoint, asset->bounds.midPoint, sizeof(float[3]));
			memcpy(s1_asset->unkBounds.halfSize, asset->bounds.halfSize, sizeof(float[3]));

			// bounds1 = ?
			// bounds2 = shadowBounds;

			s1_asset->checksum = asset->checksum;

			s1_asset->materialMemoryCount = asset->materialMemoryCount;
			s1_asset->materialMemory = mem.allocate<S1::MaterialMemory>(s1_asset->materialMemoryCount);
			for (int i = 0; i < s1_asset->materialMemoryCount; i++)
			{
				s1_asset->materialMemory[i].material = reinterpret_cast<S1::Material * __ptr64>(asset->materialMemory[i].material);
				s1_asset->materialMemory[i].memory = asset->materialMemory[i].memory;
			}

			s1_asset->sun.hasValidData = asset->sun.hasValidData;
			s1_asset->sun.spriteMaterial = reinterpret_cast<S1::Material * __ptr64>(asset->sun.spriteMaterial);
			s1_asset->sun.flareMaterial = reinterpret_cast<S1::Material * __ptr64>(asset->sun.flareMaterial);
			memcpy(&s1_asset->sun.spriteSize, &asset->sun.spriteSize, Difference(&asset->sun.sunFxPosition, &asset->sun.spriteSize) + sizeof(float[3]));

			memcpy(&s1_asset->outdoorLookupMatrix, &asset->outdoorLookupMatrix, sizeof(float[4][4]));

			s1_asset->outdoorImage = mem.allocate<S1::GfxImage>();
			s1_asset->outdoorImage->name = asset->outdoorImage->name;

			s1_asset->cellCasterBits = mem.allocate<unsigned int>(s1_asset->dpvsPlanes.cellCount * ((s1_asset->dpvsPlanes.cellCount + 31) >> 5));
			for (int i = 0; i < asset->dpvsPlanes.cellCount * ((asset->dpvsPlanes.cellCount + 31) >> 5); i++)
			{
				s1_asset->cellCasterBits[i] = asset->cellCasterBits[i];
			}
			s1_asset->cellHasSunLitSurfsBits = mem.allocate<unsigned int>((s1_asset->dpvsPlanes.cellCount + 31) >> 5); // todo?

			s1_asset->sceneDynModel = mem.allocate<S1::GfxSceneDynModel>(asset->dpvsDyn.dynEntClientCount[0]);
			for (unsigned int i = 0; i < asset->dpvsDyn.dynEntClientCount[0]; i++)
			{
				s1_asset->sceneDynModel[i].info.hasGfxEntIndex = asset->sceneDynModel[i].info.hasGfxEntIndex;
				s1_asset->sceneDynModel[i].info.lod = asset->sceneDynModel[i].info.lod;
				s1_asset->sceneDynModel[i].info.surfId = asset->sceneDynModel[i].info.surfId;
				s1_asset->sceneDynModel[i].dynEntId = asset->sceneDynModel[i].dynEntId;
			}
			REINTERPRET_CAST_SAFE(s1_asset->sceneDynBrush, asset->sceneDynBrush);

			//s1_asset->primaryLightEntityShadowVis = reinterpret_cast<unsigned int* __ptr64>(asset->primaryLightEntityShadowVis);
			int count = ((s1_asset->primaryLightCount - s1_asset->lastSunPrimaryLightIndex) << 13) - 0x2000;
			s1_asset->primaryLightEntityShadowVis = mem.allocate<unsigned int>(count);
			for (unsigned int i = 0; i < count; i++)
			{
				s1_asset->primaryLightEntityShadowVis[i] = asset->primaryLightEntityShadowVis[i];
			}

			s1_asset->primaryLightDynEntShadowVis[0] = reinterpret_cast<unsigned int* __ptr64>(asset->primaryLightDynEntShadowVis[0]);
			s1_asset->primaryLightDynEntShadowVis[1] = reinterpret_cast<unsigned int* __ptr64>(asset->primaryLightDynEntShadowVis[1]);

			//s1_asset->nonSunPrimaryLightForModelDynEnt = reinterpret_cast<unsigned __int16* __ptr64>(asset->primaryLightForModelDynEnt);
			s1_asset->nonSunPrimaryLightForModelDynEnt = mem.allocate<unsigned short>(asset->dpvsDyn.dynEntClientCount[0]);
			for (unsigned int i = 0; i < asset->dpvsDyn.dynEntClientCount[0]; i++)
			{
				s1_asset->nonSunPrimaryLightForModelDynEnt[i] = asset->nonSunPrimaryLightForModelDynEnt[i];
			}

			if (asset->shadowGeom)
			{
				s1_asset->shadowGeom = mem.allocate<S1::GfxShadowGeometry>(s1_asset->primaryLightCount);
				for (unsigned int i = 0; i < s1_asset->primaryLightCount; i++)
				{
					s1_asset->shadowGeom[i].surfaceCount = asset->shadowGeom[i].surfaceCount;
					s1_asset->shadowGeom[i].smodelCount = asset->shadowGeom[i].smodelCount;

					s1_asset->shadowGeom[i].sortedSurfIndex = mem.allocate<unsigned int>(s1_asset->shadowGeom[i].surfaceCount);
					for (unsigned int j = 0; j < s1_asset->shadowGeom[i].surfaceCount; j++)
					{
						s1_asset->shadowGeom[i].sortedSurfIndex[j] = asset->shadowGeom[i].sortedSurfIndex[j];
					}
					REINTERPRET_CAST_SAFE(s1_asset->shadowGeom[i].smodelIndex, asset->shadowGeom[i].smodelIndex);
				}
			}
			s1_asset->shadowGeomOptimized = nullptr;

			s1_asset->lightRegion = mem.allocate<S1::GfxLightRegion>(s1_asset->primaryLightCount);
			for (unsigned int i = 0; i < s1_asset->primaryLightCount; i++)
			{
				s1_asset->lightRegion[i].hullCount = asset->lightRegion[i].hullCount;
				s1_asset->lightRegion[i].hulls = mem.allocate<S1::GfxLightRegionHull>(s1_asset->lightRegion[i].hullCount);
				for (unsigned int j = 0; j < s1_asset->lightRegion[i].hullCount; j++)
				{
					memcpy(&s1_asset->lightRegion[i].hulls[j].kdopMidPoint, &asset->lightRegion[i].hulls[j].kdopMidPoint, sizeof(float[9]));
					memcpy(&s1_asset->lightRegion[i].hulls[j].kdopHalfSize, &asset->lightRegion[i].hulls[j].kdopHalfSize, sizeof(float[9]));

					s1_asset->lightRegion[i].hulls[j].axisCount = asset->lightRegion[i].hulls[j].axisCount;
					REINTERPRET_CAST_SAFE(s1_asset->lightRegion[i].hulls[j].axis, asset->lightRegion[i].hulls[j].axis);
				}
			}

			unsigned int lit_decal_count = asset->dpvs.staticSurfaceCount - asset->dpvs.staticSurfaceCountNoDecal;

			s1_asset->dpvs.smodelCount = asset->dpvs.smodelCount;
			s1_asset->dpvs.subdivVertexLightingInfoCount = 0;
			s1_asset->dpvs.staticSurfaceCount = asset->dpvs.staticSurfaceCountNoDecal + lit_decal_count;
			s1_asset->dpvs.litOpaqueSurfsBegin = asset->dpvs.litOpaqueSurfsBegin;
			s1_asset->dpvs.litOpaqueSurfsEnd = asset->dpvs.litOpaqueSurfsEnd;
			s1_asset->dpvs.unkSurfsBegin = 0;
			s1_asset->dpvs.unkSurfsEnd = 0;
			s1_asset->dpvs.litDecalSurfsBegin = asset->dpvs.litOpaqueSurfsEnd; // skip
			s1_asset->dpvs.litDecalSurfsEnd = asset->dpvs.litOpaqueSurfsEnd; // skip
			s1_asset->dpvs.litTransSurfsBegin = asset->dpvs.litTransSurfsBegin;
			s1_asset->dpvs.litTransSurfsEnd = asset->dpvs.litTransSurfsEnd;
			s1_asset->dpvs.shadowCasterSurfsBegin = asset->dpvs.shadowCasterSurfsBegin;
			s1_asset->dpvs.shadowCasterSurfsEnd = asset->dpvs.shadowCasterSurfsEnd;
			s1_asset->dpvs.emissiveSurfsBegin = asset->dpvs.emissiveSurfsBegin;
			s1_asset->dpvs.emissiveSurfsEnd = asset->dpvs.emissiveSurfsEnd;
			s1_asset->dpvs.smodelVisDataCount = asset->dpvs.smodelVisDataCount;
			s1_asset->dpvs.surfaceVisDataCount = asset->dpvs.surfaceVisDataCount;

			for (auto i = 0; i < 4; i++)
			{
				s1_asset->dpvs.smodelVisData[i] = mem.allocate<unsigned int>(s1_asset->dpvs.smodelVisDataCount);
			}

			for (auto i = 0; i < 4; i++)
			{
				s1_asset->dpvs.surfaceVisData[i] = mem.allocate<unsigned int>(s1_asset->dpvs.surfaceVisDataCount);
			}

			for (auto i = 0; i < 3; i++)
			{
				//memcpy(s1_asset->dpvs.smodelVisData[i], asset->dpvs.smodelVisData[i], sizeof(int) * s1_asset->dpvs.smodelVisDataCount);
				//memcpy(s1_asset->dpvs.surfaceVisData[i], asset->dpvs.surfaceVisData[i], sizeof(int) * s1_asset->dpvs.surfaceVisDataCount);
			}

			for (auto i = 0; i < 27; i++)
			{
				s1_asset->dpvs.smodelUnknownVisData[i] = mem.allocate<unsigned int>(s1_asset->dpvs.smodelVisDataCount);
			}

			for (auto i = 0; i < 27; i++)
			{
				s1_asset->dpvs.surfaceUnknownVisData[i] = mem.allocate<unsigned int>(s1_asset->dpvs.surfaceVisDataCount);
			}

			for (auto i = 0; i < 4; i++)
			{
				s1_asset->dpvs.smodelUmbraVisData[i] = mem.allocate<unsigned int>(s1_asset->dpvs.smodelVisDataCount);
			}

			for (auto i = 0; i < 4; i++)
			{
				s1_asset->dpvs.surfaceUmbraVisData[i] = mem.allocate<unsigned int>(s1_asset->dpvs.surfaceVisDataCount);
			}

			s1_asset->dpvs.unknownSModelVisData1 = mem.allocate<unsigned int>(s1_asset->dpvs.smodelVisDataCount);
			s1_asset->dpvs.unknownSModelVisData2 = mem.allocate<unsigned int>(s1_asset->dpvs.smodelVisDataCount * 2);

			s1_asset->dpvs.lodData = mem.allocate<unsigned int>(s1_asset->dpvs.smodelCount + 1);
			//s1_asset->dpvs.tessellationCutoffVisData = mem.allocate<unsigned int>(s1_asset->dpvs.surfaceVisDataCount);

			s1_asset->dpvs.sortedSurfIndex = mem.allocate<unsigned int>(s1_asset->dpvs.staticSurfaceCount);
			for (unsigned int i = 0; i < s1_asset->dpvs.staticSurfaceCount; i++)
			{
				s1_asset->dpvs.sortedSurfIndex[i] = asset->dpvs.sortedSurfIndex[i];
			}

			REINTERPRET_CAST_SAFE(s1_asset->dpvs.smodelInsts, asset->dpvs.smodelInsts);

			s1_asset->dpvs.surfaces = mem.allocate<S1::GfxSurface>(s1_asset->surfaceCount);
			for (unsigned int i = 0; i < s1_asset->surfaceCount; i++)
			{
				s1_asset->dpvs.surfaces[i].tris.vertexLayerData = asset->dpvs.surfaces[i].tris.vertexLayerData;
				s1_asset->dpvs.surfaces[i].tris.firstVertex = asset->dpvs.surfaces[i].tris.firstVertex;
				s1_asset->dpvs.surfaces[i].tris.maxEdgeLength = 0;
				s1_asset->dpvs.surfaces[i].tris.vertexCount = asset->dpvs.surfaces[i].tris.vertexCount;
				s1_asset->dpvs.surfaces[i].tris.triCount = asset->dpvs.surfaces[i].tris.triCount;
				s1_asset->dpvs.surfaces[i].tris.baseIndex = asset->dpvs.surfaces[i].tris.baseIndex;
				s1_asset->dpvs.surfaces[i].material = reinterpret_cast<S1::Material * __ptr64>(asset->dpvs.surfaces[i].material);
				s1_asset->dpvs.surfaces[i].laf.fields.lightmapIndex = asset->dpvs.surfaces[i].laf.fields.lightmapIndex;
				s1_asset->dpvs.surfaces[i].laf.fields.reflectionProbeIndex = asset->dpvs.surfaces[i].laf.fields.reflectionProbeIndex;
				s1_asset->dpvs.surfaces[i].laf.fields.primaryLightEnvIndex = asset->dpvs.surfaces[i].laf.fields.primaryLightIndex;
				s1_asset->dpvs.surfaces[i].laf.fields.flags = asset->dpvs.surfaces[i].laf.fields.flags;
			}

			s1_asset->dpvs.surfacesBounds = mem.allocate<S1::GfxSurfaceBounds>(s1_asset->surfaceCount);
			for (unsigned int i = 0; i < s1_asset->surfaceCount; i++)
			{
				memcpy(&s1_asset->dpvs.surfacesBounds[i].bounds, &asset->dpvs.surfacesBounds[i].bounds, sizeof(IW5::Bounds));
				s1_asset->dpvs.surfacesBounds[i].__pad0; // idk
			}

			s1_asset->dpvs.smodelDrawInsts = mem.allocate<S1::GfxStaticModelDrawInst>(s1_asset->dpvs.smodelCount);
			for (unsigned int i = 0; i < s1_asset->dpvs.smodelCount; i++)
			{
				memcpy(&s1_asset->dpvs.smodelDrawInsts[i].placement, &asset->dpvs.smodelDrawInsts[i].placement, sizeof(IW5::GfxPackedPlacement));
				s1_asset->dpvs.smodelDrawInsts[i].model = reinterpret_cast<S1::XModel * __ptr64>(asset->dpvs.smodelDrawInsts[i].model);
				s1_asset->dpvs.smodelDrawInsts[i].lightingHandle = asset->dpvs.smodelDrawInsts[i].lightingHandle;
				s1_asset->dpvs.smodelDrawInsts[i].staticModelId = 0;
				s1_asset->dpvs.smodelDrawInsts[i].primaryLightEnvIndex = asset->dpvs.smodelDrawInsts[i].primaryLightIndex;
				s1_asset->dpvs.smodelDrawInsts[i].reflectionProbeIndex = asset->dpvs.smodelDrawInsts[i].reflectionProbeIndex;
				s1_asset->dpvs.smodelDrawInsts[i].firstMtlSkinIndex = asset->dpvs.smodelDrawInsts[i].firstMtlSkinIndex;
				s1_asset->dpvs.smodelDrawInsts[i].sunShadowFlags = 1;

				s1_asset->dpvs.smodelDrawInsts[i].cullDist = asset->dpvs.smodelDrawInsts[i].cullDist;
				s1_asset->dpvs.smodelDrawInsts[i].unk0 = s1_asset->dpvs.smodelDrawInsts[i].cullDist;
				s1_asset->dpvs.smodelDrawInsts[i].unk1 = 0;

				// casts no shadows
				auto no_shadows = (asset->dpvs.smodelDrawInsts[i].flags & 0x10) != 0;
				if (no_shadows)
				{
					s1_asset->dpvs.smodelDrawInsts[i].flags |= S1::StaticModelFlag::STATIC_MODEL_FLAG_NO_CAST_SHADOW;
				}

				// ground lighting
				auto ground_lighting = (asset->dpvs.smodelDrawInsts[i].flags & 0x20) != 0 || asset->dpvs.smodelDrawInsts[i].groundLighting.packed != 0;
				if (ground_lighting)
				{
					s1_asset->dpvs.smodelDrawInsts[i].flags |= S1::StaticModelFlag::STATIC_MODEL_FLAG_GROUND_LIGHTING;
				}
				// regular lighting
				else
				{
					s1_asset->dpvs.smodelDrawInsts[i].flags |= S1::StaticModelFlag::STATIC_MODEL_FLAG_LIGHTGRID_LIGHTING;
				}
			}

			s1_asset->dpvs.smodelLighting = mem.allocate<S1::GfxStaticModelLighting>(s1_asset->dpvs.smodelCount);
			for (unsigned int i = 0; i < s1_asset->dpvs.smodelCount; i++)
			{
				if ((s1_asset->dpvs.smodelDrawInsts[i].flags & S1::StaticModelFlag::STATIC_MODEL_FLAG_GROUND_LIGHTING) != 0)
				{
					//bgra -> rgba
					auto ground_lighting = asset->dpvs.smodelDrawInsts[i].groundLighting;
					auto bgra = ground_lighting.array;

					float rgba[4] = { bgra[2] / 255.0f, bgra[1] / 255.0f, bgra[0] / 255.0f, bgra[3] / 255.0f };

					s1_asset->dpvs.smodelLighting[i].modelGroundLightingInfo.groundLighting[0] = float_to_half(rgba[0]); // r
					s1_asset->dpvs.smodelLighting[i].modelGroundLightingInfo.groundLighting[1] = float_to_half(rgba[1]); // g
					s1_asset->dpvs.smodelLighting[i].modelGroundLightingInfo.groundLighting[2] = float_to_half(rgba[2]); // b
					s1_asset->dpvs.smodelLighting[i].modelGroundLightingInfo.groundLighting[3] = float_to_half(rgba[3]); // a
				}
				else if ((s1_asset->dpvs.smodelDrawInsts[i].flags & S1::StaticModelFlag::STATIC_MODEL_FLAG_LIGHTGRID_LIGHTING) != 0)
				{
					//s1_asset->dpvs.smodelDrawInsts[i].flags |= S1::StaticModelFlag::STATIC_MODEL_FLAG_ALLOW_FXMARK; // R_CalcModelLighting: 0x240
					s1_asset->dpvs.smodelLighting[i].modelLightGridLightingInfo.colorFloat16[0] = float_to_half(0.5f); // r
					s1_asset->dpvs.smodelLighting[i].modelLightGridLightingInfo.colorFloat16[1] = float_to_half(0.5f); // g
					s1_asset->dpvs.smodelLighting[i].modelLightGridLightingInfo.colorFloat16[2] = float_to_half(0.5f); // b
					s1_asset->dpvs.smodelLighting[i].modelLightGridLightingInfo.colorFloat16[3] = float_to_half(0.5f); // a
				}
				else if ((s1_asset->dpvs.smodelDrawInsts[i].flags & S1::StaticModelFlag::STATIC_MODEL_FLAG_LIGHTMAP_LIGHTING) != 0)
				{
					// todo?
				}
				else if ((s1_asset->dpvs.smodelDrawInsts[i].flags & S1::StaticModelFlag::STATIC_MODEL_FLAG_VERTEXLIT_LIGHTING) != 0)
				{
					// todo?
				}
			}

			s1_asset->dpvs.subdivVertexLighting = nullptr;

			s1_asset->dpvs.surfaceMaterials = mem.allocate<S1::GfxDrawSurf>(s1_asset->surfaceCount);
			for (unsigned int i = 0; i < s1_asset->surfaceCount; i++) // these are probably wrong
			{
				s1_asset->dpvs.surfaceMaterials[i].fields.objectId = asset->dpvs.surfaceMaterials[i].fields.objectId;
				s1_asset->dpvs.surfaceMaterials[i].fields.reflectionProbeIndex = asset->dpvs.surfaceMaterials[i].fields.reflectionProbeIndex;
				s1_asset->dpvs.surfaceMaterials[i].fields.hasGfxEntIndex = asset->dpvs.surfaceMaterials[i].fields.hasGfxEntIndex;
				s1_asset->dpvs.surfaceMaterials[i].fields.customIndex = asset->dpvs.surfaceMaterials[i].fields.customIndex;
				s1_asset->dpvs.surfaceMaterials[i].fields.materialSortedIndex = asset->dpvs.surfaceMaterials[i].fields.materialSortedIndex;
				s1_asset->dpvs.surfaceMaterials[i].fields.tessellation = 0;
				s1_asset->dpvs.surfaceMaterials[i].fields.prepass = asset->dpvs.surfaceMaterials[i].fields.prepass;
				s1_asset->dpvs.surfaceMaterials[i].fields.useHeroLighting = asset->dpvs.surfaceMaterials[i].fields.useHeroLighting;
				s1_asset->dpvs.surfaceMaterials[i].fields.sceneLightEnvIndex = asset->dpvs.surfaceMaterials[i].fields.sceneLightIndex;
				s1_asset->dpvs.surfaceMaterials[i].fields.viewModelRender = asset->dpvs.surfaceMaterials[i].fields.viewModelRender;
				s1_asset->dpvs.surfaceMaterials[i].fields.surfType = asset->dpvs.surfaceMaterials[i].fields.surfType;
				s1_asset->dpvs.surfaceMaterials[i].fields.primarySortKey = asset->dpvs.surfaceMaterials[i].fields.primarySortKey;
				s1_asset->dpvs.surfaceMaterials[i].fields.unused = asset->dpvs.surfaceMaterials[i].fields.unused;
			}

			REINTERPRET_CAST_SAFE(s1_asset->dpvs.surfaceCastsSunShadow, asset->dpvs.surfaceCastsSunShadow);
			//s1_asset->dpvs.sunShadowOptCount = 1;
			//s1_asset->dpvs.sunSurfVisDataCount = s1_asset->dpvs.surfaceVisDataCount * 8;
			//s1_asset->dpvs.surfaceCastsSunShadowOpt = mem.allocate<unsigned int>(s1_asset->dpvs.sunShadowOptCount * s1_asset->dpvs.sunSurfVisDataCount);
			//memcpy(s1_asset->dpvs.surfaceCastsSunShadowOpt, s1_asset->dpvs.surfaceCastsSunShadow, sizeof(int) * (s1_asset->dpvs.sunShadowOptCount * s1_asset->dpvs.sunSurfVisDataCount));
			s1_asset->dpvs.surfaceDeptAndSurf = mem.allocate<S1::GfxDepthAndSurf>(s1_asset->dpvs.staticSurfaceCount); // todo?
			s1_asset->dpvs.constantBuffersLit = mem.allocate<char* __ptr64>(s1_asset->dpvs.smodelCount); //nullptr;
			s1_asset->dpvs.constantBuffersAmbient = mem.allocate<char* __ptr64>(s1_asset->dpvs.smodelCount); //nullptr;
			s1_asset->dpvs.usageCount = asset->dpvs.usageCount;

			s1_asset->dpvsDyn.dynEntClientWordCount[0] = asset->dpvsDyn.dynEntClientWordCount[0];
			s1_asset->dpvsDyn.dynEntClientWordCount[1] = asset->dpvsDyn.dynEntClientWordCount[1];
			s1_asset->dpvsDyn.dynEntClientCount[0] = asset->dpvsDyn.dynEntClientCount[0];
			s1_asset->dpvsDyn.dynEntClientCount[1] = asset->dpvsDyn.dynEntClientCount[1];
			s1_asset->dpvsDyn.dynEntCellBits[0] = reinterpret_cast<unsigned int* __ptr64>(asset->dpvsDyn.dynEntCellBits[0]);
			s1_asset->dpvsDyn.dynEntCellBits[1] = reinterpret_cast<unsigned int* __ptr64>(asset->dpvsDyn.dynEntCellBits[1]);
			s1_asset->dpvsDyn.dynEntVisData[0][0] = reinterpret_cast<unsigned char* __ptr64>(asset->dpvsDyn.dynEntVisData[0][0]);
			s1_asset->dpvsDyn.dynEntVisData[0][1] = reinterpret_cast<unsigned char* __ptr64>(asset->dpvsDyn.dynEntVisData[0][1]);
			s1_asset->dpvsDyn.dynEntVisData[0][2] = reinterpret_cast<unsigned char* __ptr64>(asset->dpvsDyn.dynEntVisData[0][2]);
			s1_asset->dpvsDyn.dynEntVisData[0][3] = mem.allocate<unsigned char>(s1_asset->dpvsDyn.dynEntClientWordCount[0] * 32);
			s1_asset->dpvsDyn.dynEntVisData[1][0] = reinterpret_cast<unsigned char* __ptr64>(asset->dpvsDyn.dynEntVisData[1][0]);
			s1_asset->dpvsDyn.dynEntVisData[1][1] = reinterpret_cast<unsigned char* __ptr64>(asset->dpvsDyn.dynEntVisData[1][1]);
			s1_asset->dpvsDyn.dynEntVisData[1][2] = reinterpret_cast<unsigned char* __ptr64>(asset->dpvsDyn.dynEntVisData[1][2]);
			s1_asset->dpvsDyn.dynEntVisData[1][3] = mem.allocate<unsigned char>(s1_asset->dpvsDyn.dynEntClientWordCount[1] * 32);

			s1_asset->mapVtxChecksum = asset->mapVtxChecksum;

			s1_asset->heroOnlyLightCount = asset->heroOnlyLightCount;
			REINTERPRET_CAST_SAFE(s1_asset->heroOnlyLights, asset->heroOnlyLights);

			s1_asset->fogTypesAllowed = asset->fogTypesAllowed;

			s1_asset->umbraTomeSize = 0;
			s1_asset->umbraTomeData = nullptr;
			s1_asset->umbraTomePtr = nullptr;
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

				s1_asset->umbraTomeSize = new_tome->m_size;
				s1_asset->umbraTomeData = mem->ManualAlloc<char>(s1_asset->umbraTomeSize);
				memcpy(s1_asset->umbraTomeData, buffer, s1_asset->umbraTomeSize);
				s1_asset->umbraTomePtr = reinterpret_cast<void*>(s1_asset->umbraTomeData);
			}
			*/

			s1_asset->mdaoVolumesCount = 0;
			s1_asset->mdaoVolumes = nullptr;

			// pad3 unknown data

			s1_asset->buildInfo.args0 = nullptr;
			s1_asset->buildInfo.args1 = nullptr;
			s1_asset->buildInfo.buildStartTime = nullptr;
			s1_asset->buildInfo.buildEndTime = nullptr;

			return s1_asset;
		}

		S1::GfxWorld* convert(GfxWorld* asset, allocator& allocator)
		{
			// generate h1 gfxworld
			return GenerateS1GfxWorld(asset, allocator);
		}
	}
}
