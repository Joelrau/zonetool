#include "stdafx.hpp"
#include "../Include.hpp"

#include "GfxWorld.hpp"

#include "X64/Utils/Umbra/umbra.hpp"
#include "X64/Utils/Utils.hpp"
#include "X64/Utils/LightGrid/LightGridSH.hpp"
#include "X64/Utils/LightGrid/LightGridTree.hpp"

#include <set>

namespace ZoneTool::IW5
{
	namespace IW6Converter
	{
		// a legacy entry is "sun" when its raw primaryLightIndex uses either sun
		// encoding: the low range [1 .. lastSun] or the high range
		// [256-lastSun .. 255] (lastSun == 0 leaves the low range empty). sun
		// cells are remapped to the appended sentinel sun env: the engine treats
		// only envs whose light index is >= 2048 - lastSunPrimaryLightIndex as sun
		// (the shadow-mapped sun path), and those always lose the light-grid corner
		// vote to real indoor envs (R_LightGridLookup) - so near-wall lookups never
		// flicker to an unshadowed sun.
		static bool is_sun_light(unsigned int pli, unsigned int last_sun)
		{
			return (last_sun != 0 && pli >= 1 && pli <= last_sun)
				|| pli >= 256 - last_sun;
		}

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

			// always append a duplicate of color set 0 (the legacy default set) as
			// the LAST color/palette entry, matching original layout (entry 0 =
			// empty sentinel, entry 1 = sky, last = default/missing). it serves two
			// purposes: missingGridColorIndex points at it for genuine lookup
			// misses, and legacy colorsIndex-0 references are remapped to it - the
			// tree treats voxel color_index 0 as "no data" (R_GetLightGrid returns
			// false), which would otherwise drop those cells' light env.
			const auto lightgrid_refs = lightgrid_tree::enumerate_row_data(
				asset->lightGrid.mins, asset->lightGrid.maxs,
				asset->lightGrid.rowAxis, asset->lightGrid.colAxis,
				asset->lightGrid.rowDataStart, asset->lightGrid.rawRowData);

			const unsigned int orig_color_count = asset->lightGrid.colorCount;
			bool extend_colors = orig_color_count > 0;
			if (extend_colors && orig_color_count == 0xFFFF)
			{
				ZONETOOL_WARNING("GfxWorld \"%s\": light grid colorCount is 0xFFFF, "
					"skipping default color entry append to avoid index overflow", asset->name);
				extend_colors = false;
			}
			const unsigned int zero_remap_index = orig_color_count; // only valid when extend_colors

			iw6_asset->lightGrid.entryCount = asset->lightGrid.entryCount;
			iw6_asset->lightGrid.entries = mem.allocate<IW6::GfxLightGridEntry>(iw6_asset->lightGrid.entryCount);
			for (unsigned int i = 0; i < iw6_asset->lightGrid.entryCount; i++)
			{
				iw6_asset->lightGrid.entries[i].colorsIndex =
					(extend_colors && asset->lightGrid.entries[i].colorsIndex == 0)
					? zero_remap_index
					: asset->lightGrid.entries[i].colorsIndex;
				iw6_asset->lightGrid.entries[i].primaryLightEnvIndex = asset->lightGrid.entries[i].primaryLightIndex;
				iw6_asset->lightGrid.entries[i].unused = 0;
				iw6_asset->lightGrid.entries[i].needsTrace = asset->lightGrid.entries[i].needsTrace;

				// sun cells -> sentinel sun env (see is_sun_light); mirrored in the
				// tree sample loop below
				if (is_sun_light(asset->lightGrid.entries[i].primaryLightIndex, asset->lastSunPrimaryLightIndex))
				{
					iw6_asset->lightGrid.entries[i].primaryLightEnvIndex = static_cast<unsigned short>(asset->primaryLightCount);
				}
			}
			const unsigned int dest_color_count = extend_colors ? orig_color_count + 1 : orig_color_count;
			iw6_asset->lightGrid.colorCount = dest_color_count;
			if (extend_colors)
			{
				// allocate a copy (the shared reinterpret-cast pointer can't grow) and
				// duplicate legacy entry 0 into the fresh slot. IW5/IW6 GfxLightGridColors
				// share the same unsigned char rgb[56][3] layout (168 bytes).
				iw6_asset->lightGrid.colors = mem.allocate<IW6::GfxLightGridColors>(dest_color_count);
				memcpy(iw6_asset->lightGrid.colors, asset->lightGrid.colors,
					static_cast<size_t>(orig_color_count) * sizeof(IW6::GfxLightGridColors));
				memcpy(&iw6_asset->lightGrid.colors[orig_color_count], &asset->lightGrid.colors[0],
					sizeof(IW6::GfxLightGridColors));
			}
			else
			{
				iw6_asset->lightGrid.colors = reinterpret_cast<IW6::GfxLightGridColors*>(asset->lightGrid.colors);
			}

			// build the SH color palette + lightgrid tree from the legacy LDR data.
			{
				// palette: one SH entry per legacy color set, colorsIndex maps 1:1.
				// build from the (optionally extended) LDR colors so palette entry i
				// stays aligned with colors[i] and colorsIndex i. build_palette_from_ldr
				// sizes paletteEntryCount from the count we pass here.
				std::vector<unsigned char> extended_ldr;
				const unsigned char* palette_colors =
					reinterpret_cast<const unsigned char*>(asset->lightGrid.colors);
				unsigned int palette_color_count = orig_color_count;
				if (extend_colors)
				{
					const size_t stride = sizeof(asset->lightGrid.colors[0]); // 168 bytes
					extended_ldr.resize(static_cast<size_t>(orig_color_count + 1) * stride);
					memcpy(extended_ldr.data(), palette_colors,
						static_cast<size_t>(orig_color_count) * stride);
					memcpy(extended_ldr.data() + static_cast<size_t>(orig_color_count) * stride,
						palette_colors, stride); // duplicate entry 0 into the fresh slot
					palette_colors = extended_ldr.data();
					palette_color_count = orig_color_count + 1;
				}
				const auto palette = lightgrid_sh::build_palette_from_ldr(
					palette_colors, palette_color_count);

				iw6_asset->lightGrid.tableVersion = 1;
				iw6_asset->lightGrid.paletteVersion = 1;

				iw6_asset->lightGrid.rangeExponent8BitsEncoding = palette.config.range_exp_8bits;
				iw6_asset->lightGrid.rangeExponent12BitsEncoding = palette.config.range_exp_12bits;
				iw6_asset->lightGrid.rangeExponent16BitsEncoding = palette.config.range_exp_16bits;

				iw6_asset->lightGrid.paletteEntryCount = static_cast<unsigned int>(palette.entry_address.size());
				iw6_asset->lightGrid.paletteEntryAddress = mem.allocate<int>(iw6_asset->lightGrid.paletteEntryCount);
				memcpy(iw6_asset->lightGrid.paletteEntryAddress, palette.entry_address.data(),
					palette.entry_address.size() * sizeof(int));

				iw6_asset->lightGrid.paletteBitstreamSize = static_cast<unsigned int>(palette.bitstream.size());
				iw6_asset->lightGrid.paletteBitstream = mem.allocate<unsigned char>(iw6_asset->lightGrid.paletteBitstreamSize);
				memcpy(iw6_asset->lightGrid.paletteBitstream, palette.bitstream.data(), palette.bitstream.size());

				// point misses at the appended default entry (last palette index),
				// matching original layout; entry 0 stays the empty sentinel
				iw6_asset->lightGrid.missingGridColorIndex = extend_colors
					? zero_remap_index
					: (iw6_asset->lightGrid.paletteEntryCount ? iw6_asset->lightGrid.paletteEntryCount - 1 : 0);

				iw6_asset->lightGrid.stageCount = asset->primaryLightCount;
				iw6_asset->lightGrid.stageLightingContrastGain = mem.allocate<float>(iw6_asset->lightGrid.stageCount);
				for (auto i = 0; i < iw6_asset->lightGrid.stageCount; i++)
				{
					iw6_asset->lightGrid.stageLightingContrastGain[i] = 0.3f;
				}

				// sky/default grid colors are linear HDR values stored as half floats
				float hdr_colors[56][3];
				if (asset->lightGrid.colorCount > 0)
				{
					lightgrid_sh::ldr_colors_to_hdr(asset->lightGrid.colors[0].rgb, hdr_colors);
					for (unsigned int j = 0; j < 56; j++)
					{
						iw6_asset->lightGrid.defaultLightGridColors.rgb[j][0] = hdr_colors[j][0];
						iw6_asset->lightGrid.defaultLightGridColors.rgb[j][1] = hdr_colors[j][1];
						iw6_asset->lightGrid.defaultLightGridColors.rgb[j][2] = hdr_colors[j][2];
					}
				}
				if (asset->lightGrid.colorCount > 1)
				{
					lightgrid_sh::ldr_colors_to_hdr(asset->lightGrid.colors[1].rgb, hdr_colors);
					for (unsigned int j = 0; j < 56; j++)
					{
						iw6_asset->lightGrid.skyLightGridColors.rgb[j][0] = hdr_colors[j][0];
						iw6_asset->lightGrid.skyLightGridColors.rgb[j][1] = hdr_colors[j][1];
						iw6_asset->lightGrid.skyLightGridColors.rgb[j][2] = hdr_colors[j][2];
					}
				}

				// tree: rebuild the compressed octree that the engine walks in
				// R_LightGridLookup from the populated grid positions enumerated above.
				std::vector<lightgrid_tree::grid_sample> tree_samples;
				std::vector<unsigned char> sample_classes; // parallel: 0 = indoor, 1 = sun
				tree_samples.reserve(lightgrid_refs.size());
				sample_classes.reserve(lightgrid_refs.size());
				for (const auto& ref : lightgrid_refs)
				{
					if (ref.entry_index >= asset->lightGrid.entryCount)
					{
						continue;
					}
					const auto& entry = asset->lightGrid.entries[ref.entry_index];

					lightgrid_tree::grid_sample sample{};
					memcpy(sample.pos, ref.pos, sizeof(sample.pos));
					// remap colorsIndex 0 -> duplicate slot so the tree never stores
					// real data at color_index 0 (see extend_colors above)
					sample.color_index = (extend_colors && entry.colorsIndex == 0)
						? zero_remap_index
						: entry.colorsIndex;
					sample.light_index = entry.primaryLightIndex;
					// the old per-corner needsTrace mask becomes two z-half trace bits
					sample.trace_lo = (entry.needsTrace & 0x55) != 0;
					sample.trace_hi = (entry.needsTrace & 0xAA) != 0;

					// classify from the ORIGINAL primaryLightIndex (see is_sun_light):
					// drives both the dilation donor preference and the sentinel-env
					// remap that must match the entries loop above
					const bool is_sun = is_sun_light(entry.primaryLightIndex, asset->lastSunPrimaryLightIndex);
					sample_classes.push_back(is_sun ? 1 : 0);
					if (is_sun)
					{
						sample.light_index = static_cast<unsigned short>(asset->primaryLightCount);
					}
					tree_samples.push_back(sample);
				}

				// count unique populated cells before dilation (dilate_samples and
				// build_tree both collapse duplicate positions to the last sample)
				std::set<unsigned long long> unique_before;
				for (const auto& s : tree_samples)
				{
					unique_before.insert(
						(static_cast<unsigned long long>(s.pos[2]) << 42)
						| (static_cast<unsigned long long>(s.pos[1]) << 21)
						| static_cast<unsigned long long>(s.pos[0]));
				}
				const size_t populated_cell_count = unique_before.size();

				// dilate populated cells into empty neighbors so near-wall / in-solid
				// lookups clamp to real lighting instead of the sun fallback. run after
				// the colorsIndex-0 remap so dilated copies never carry color_index 0.
				// pass the class array so empty wall cells prefer an indoor donor over
				// a laterally-closer sun donor (avoids indoor sun flicker).
				lightgrid_tree::dilate_samples(tree_samples, 2, sample_classes.data());
				const size_t dilated_cell_count = tree_samples.size() - populated_cell_count;

				const auto tree = lightgrid_tree::build_tree(tree_samples.data(), tree_samples.size(), lightgrid_tree::leaf_size_bits::iw6);

				// verify the round-trip through the game-exact mirror decoder using an
				// iw6-sized view (the tree_data overload assumes h1 leaf-size bits).
				{
					lightgrid_tree::tree_view view{};
					view.max_depth = tree.max_depth;
					view.node_count = tree.node_count;
					view.coord_min_grid_space = tree.coord_min_grid_space;
					view.coord_max_grid_space = tree.coord_max_grid_space;
					view.default_color_index_bit_count = tree.default_color_index_bit_count;
					view.default_light_index_bit_count = tree.default_light_index_bit_count;
					view.node_table = tree.node_table.data();
					view.leaf_table_size = static_cast<int>(tree.leaf_table.size());
					view.leaf_table = tree.leaf_table.data();
					view.size_bits = lightgrid_tree::leaf_size_bits::iw6;

					size_t roundtrip_mismatches = 0;
					for (const auto& s : tree_samples)
					{
						lightgrid_tree::raw_result r{};
						const bool ok = lightgrid_tree::lookup(view, s.pos, r);
						if (!ok || r.color_index != s.color_index || r.light_index != s.light_index
							|| r.trace_lo != s.trace_lo || r.trace_hi != s.trace_hi)
						{
							roundtrip_mismatches++;
						}
					}
					if (roundtrip_mismatches)
					{
						ZONETOOL_ERROR("GfxWorld \"%s\": light grid tree %zu samples (%zu dilated), "
							"%zu round-trip mismatches", asset->name, populated_cell_count,
							dilated_cell_count, roundtrip_mismatches);
					}
					else
					{
						ZONETOOL_INFO("GfxWorld \"%s\": light grid tree %zu samples (%zu dilated), round-trip OK",
							asset->name, populated_cell_count, dilated_cell_count);
					}
				}
				auto& iw6_tree = iw6_asset->lightGrid.tree;
				memset(&iw6_tree, 0, sizeof(iw6_tree));
				iw6_tree.maxDepth = tree.max_depth;
				iw6_tree.nodeCount = tree.node_count;
				iw6_tree.leafCount = tree.leaf_count;
				memcpy(iw6_tree.coordMinGridSpace, tree.coord_min_grid_space, sizeof(int[3]));
				memcpy(iw6_tree.coordMaxGridSpace, tree.coord_max_grid_space, sizeof(int[3]));
				memcpy(iw6_tree.coordHalfSizeGridSpace, tree.coord_half_size_grid_space, sizeof(int[3]));
				iw6_tree.defaultColorIndexBitCount = tree.default_color_index_bit_count;
				iw6_tree.defaultLightIndexBitCount = tree.default_light_index_bit_count;
				iw6_tree.p_nodeTable = mem.allocate<unsigned int>(static_cast<std::uint32_t>(tree.node_table.size()));
				memcpy(iw6_tree.p_nodeTable, tree.node_table.data(), tree.node_table.size() * sizeof(unsigned int));
				iw6_tree.leafTableSize = static_cast<int>(tree.leaf_table.size());
				if (!tree.leaf_table.empty())
				{
					iw6_tree.p_leafTable = mem.allocate<unsigned char>(iw6_tree.leafTableSize);
					memcpy(iw6_tree.p_leafTable, tree.leaf_table.data(), tree.leaf_table.size());
				}
			}

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