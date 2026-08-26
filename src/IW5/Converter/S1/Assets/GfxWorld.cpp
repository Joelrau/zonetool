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
	namespace S1Converter
	{
		// which legacy cells become H1 sentinel-sun cells (routed to the shadow-mapped
		// sun env that loses near-wall corner votes, killing the unshadowed-sun flicker).
		// the encoding differs by source game (verified against each engine's
		// R_LightGridLookup corner-vote):
		//  - IW4/IW5 deprioritize primaryLightIndex in [256-lastSun .. 255]; low indices
		//    are real lights that win by weight (mp_rust: 10 DIR fill lights in [1..10],
		//    dominant terrain at pli=2). so match ONLY the high range.
		//  - IW3 only deprioritizes 0xFF in its own vote, but its sun is authored as raw
		//    index 1 (mp_test: 88% of cells) and shadowed via a SEPARATE global sun pass
		//    H1 doesn't have - so a raw env[1] would render as unshadowed sun near walls.
		//    route the low range [1..lastSun] to the sentinel too; lastSun==1 on IW3 maps
		//    keeps that to exactly the sun. the high term also catches IW3's 0xFF cells.
		static bool is_sun_light(unsigned int pli, unsigned int last_sun)
		{
			const bool low_range_is_sun = ZoneTool::get_linker_mode() == ZoneTool::linker_mode::iw3;
			return (low_range_is_sun && last_sun != 0 && pli >= 1 && pli <= last_sun)
				|| pli >= 256 - last_sun;
		}

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
				REINTERPRET_CAST_SAFE_TO_FROM(s1_asset->skies[i].skyStartSurfs, asset->skies[i].skyStartSurfs);
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
			REINTERPRET_CAST_SAFE_TO_FROM(s1_asset->dpvsPlanes.planes, asset->dpvsPlanes.planes);
			REINTERPRET_CAST_SAFE_TO_FROM(s1_asset->dpvsPlanes.nodes, asset->dpvsPlanes.nodes);

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
					REINTERPRET_CAST_SAFE_TO_FROM(s1_asset->aabbTrees[i].aabbTree[j].smodelIndexes, asset->aabbTrees[i].aabbTree[j].smodelIndexes);

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

			s1_asset->portalDistanceAnchorCount = 0;
			s1_asset->portalDistanceAnchorsAndCloseDistSquared = nullptr;

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

			s1_asset->draw.lightmapParameters.lightmapWidthPrimary = 1024;
			s1_asset->draw.lightmapParameters.lightmapHeightPrimary = 1024;
			s1_asset->draw.lightmapParameters.lightmapWidthSecondary = 512;
			s1_asset->draw.lightmapParameters.lightmapHeightSecondary = 512;
			s1_asset->draw.lightmapParameters.lightmapModelUnitsPerTexel = 8;

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
			REINTERPRET_CAST_SAFE_TO_FROM(s1_asset->draw.vld.data, asset->draw.vld.data);

			s1_asset->draw.indexCount = asset->draw.indexCount;
			REINTERPRET_CAST_SAFE_TO_FROM(s1_asset->draw.indices, asset->draw.indices);

			s1_asset->lightGrid.hasLightRegions = asset->lightGrid.hasLightRegions;
			s1_asset->lightGrid.useSkyForLowZ = 0;
			s1_asset->lightGrid.lastSunPrimaryLightIndex = asset->lightGrid.lastSunPrimaryLightIndex;
			memcpy(&s1_asset->lightGrid.mins, &asset->lightGrid.mins, sizeof(short[3]));
			memcpy(&s1_asset->lightGrid.maxs, &asset->lightGrid.maxs, sizeof(short[3]));
			s1_asset->lightGrid.rowAxis = asset->lightGrid.rowAxis;
			s1_asset->lightGrid.colAxis = asset->lightGrid.colAxis;
			REINTERPRET_CAST_SAFE_TO_FROM(s1_asset->lightGrid.rowDataStart, asset->lightGrid.rowDataStart);
			s1_asset->lightGrid.rawRowDataSize = asset->lightGrid.rawRowDataSize;
			REINTERPRET_CAST_SAFE_TO_FROM(s1_asset->lightGrid.rawRowData, asset->lightGrid.rawRowData);

			// always append a duplicate of color set 0 (the legacy default set) as
			// the LAST color/palette entry, matching original H1 layout (entry 0 =
			// empty sentinel, entry 1 = sky, last = default/missing). it serves two
			// purposes: missingGridColorIndex points at it for genuine lookup
			// misses, and legacy colorsIndex-0 references are remapped to it - the
			// tree treats voxel color_index 0 as "no data" (R_GetLightGrid
			// returns false), which would otherwise drop those cells' light env.
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

			s1_asset->lightGrid.entryCount = asset->lightGrid.entryCount;
			s1_asset->lightGrid.entries = mem.allocate<S1::GfxLightGridEntry>(s1_asset->lightGrid.entryCount);
			for (unsigned int i = 0; i < s1_asset->lightGrid.entryCount; i++)
			{
				s1_asset->lightGrid.entries[i].colorsIndex =
					(extend_colors && asset->lightGrid.entries[i].colorsIndex == 0)
					? zero_remap_index
					: asset->lightGrid.entries[i].colorsIndex;
				s1_asset->lightGrid.entries[i].primaryLightEnvIndex = asset->lightGrid.entries[i].primaryLightIndex;
				s1_asset->lightGrid.entries[i].unused = 0;
				s1_asset->lightGrid.entries[i].needsTrace = asset->lightGrid.entries[i].needsTrace;

				// sun cells -> sentinel sun env (see is_sun_light); mirrored in the
				// tree sample loop below
				if (is_sun_light(asset->lightGrid.entries[i].primaryLightIndex, asset->lastSunPrimaryLightIndex))
				{
					s1_asset->lightGrid.entries[i].primaryLightEnvIndex = static_cast<unsigned short>(asset->primaryLightCount);
				}
			}
			const unsigned int dest_color_count = extend_colors ? orig_color_count + 1 : orig_color_count;
			s1_asset->lightGrid.colorCount = dest_color_count;
			s1_asset->lightGrid.colors = mem.allocate<S1::GfxLightGridColors>(dest_color_count);
			for (unsigned int i = 0; i < dest_color_count; i++)
			{
				// the duplicated slot (index == orig_color_count) re-converts entry 0
				const unsigned int src = (extend_colors && i == orig_color_count) ? 0 : i;
				for (unsigned int j = 0; j < 56; j++)
				{
					auto& rgb = asset->lightGrid.colors[src].rgb[j];
					auto& dest_rgb = s1_asset->lightGrid.colors[i].rgb[j];
					dest_rgb[0] = float_to_half(rgb[0] / 255.f);
					dest_rgb[1] = float_to_half(rgb[1] / 255.f);
					dest_rgb[2] = float_to_half(rgb[2] / 255.f);
				}
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

				s1_asset->lightGrid.tableVersion = 1;
				s1_asset->lightGrid.paletteVersion = 1;

				s1_asset->lightGrid.rangeExponent8BitsEncoding = palette.config.range_exp_8bits;
				s1_asset->lightGrid.rangeExponent12BitsEncoding = palette.config.range_exp_12bits;
				s1_asset->lightGrid.rangeExponent16BitsEncoding = palette.config.range_exp_16bits;

				s1_asset->lightGrid.paletteEntryCount = static_cast<unsigned int>(palette.entry_address.size());
				s1_asset->lightGrid.paletteEntryAddress = mem.allocate<int>(s1_asset->lightGrid.paletteEntryCount);
				memcpy(s1_asset->lightGrid.paletteEntryAddress, palette.entry_address.data(),
					palette.entry_address.size() * sizeof(int));

				s1_asset->lightGrid.paletteBitstreamSize = static_cast<unsigned int>(palette.bitstream.size());
				s1_asset->lightGrid.paletteBitstream = mem.allocate<unsigned char>(s1_asset->lightGrid.paletteBitstreamSize);
				memcpy(s1_asset->lightGrid.paletteBitstream, palette.bitstream.data(), palette.bitstream.size());

				// point misses at the appended default entry (last palette index),
				// matching original H1 layout; entry 0 stays the empty sentinel
				s1_asset->lightGrid.missingGridColorIndex = extend_colors
					? zero_remap_index
					: (s1_asset->lightGrid.paletteEntryCount ? s1_asset->lightGrid.paletteEntryCount - 1 : 0);

				s1_asset->lightGrid.stageCount = asset->primaryLightCount;
				s1_asset->lightGrid.stageLightingContrastGain = mem.allocate<float>(s1_asset->lightGrid.stageCount);
				for (auto i = 0; i < s1_asset->lightGrid.stageCount; i++)
				{
					s1_asset->lightGrid.stageLightingContrastGain[i] = 0.3f;
				}

				// sky/default grid colors are linear HDR values stored as half floats
				float hdr_colors[56][3];
				if (asset->lightGrid.colorCount > 0)
				{
					lightgrid_sh::ldr_colors_to_hdr(asset->lightGrid.colors[0].rgb, hdr_colors);
					for (unsigned int j = 0; j < 56; j++)
					{
						s1_asset->lightGrid.defaultLightGridColors.rgb[j][0] = float_to_half(hdr_colors[j][0]);
						s1_asset->lightGrid.defaultLightGridColors.rgb[j][1] = float_to_half(hdr_colors[j][1]);
						s1_asset->lightGrid.defaultLightGridColors.rgb[j][2] = float_to_half(hdr_colors[j][2]);
					}
				}
				if (asset->lightGrid.colorCount > 1)
				{
					lightgrid_sh::ldr_colors_to_hdr(asset->lightGrid.colors[1].rgb, hdr_colors);
					for (unsigned int j = 0; j < 56; j++)
					{
						s1_asset->lightGrid.skyLightGridColors.rgb[j][0] = float_to_half(hdr_colors[j][0]);
						s1_asset->lightGrid.skyLightGridColors.rgb[j][1] = float_to_half(hdr_colors[j][1]);
						s1_asset->lightGrid.skyLightGridColors.rgb[j][2] = float_to_half(hdr_colors[j][2]);
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

				const auto tree = lightgrid_tree::build_tree(tree_samples.data(), tree_samples.size());

				// verify the round-trip through the game-exact mirror decoder.
				// tree_samples hold unique positions after dilation, so compare directly.
				size_t roundtrip_mismatches = 0;
				for (const auto& s : tree_samples)
				{
					lightgrid_tree::raw_result r{};
					const bool ok = lightgrid_tree::lookup(tree, s.pos, r);
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
				for (auto i = 0; i < 3; i++)
				{
					auto& s1_tree = s1_asset->lightGrid.tree[i];

					memset(&s1_tree, 0, sizeof(s1_tree));
					s1_tree.index = static_cast<unsigned char>(i);

					if (i > 0) continue;

					s1_tree.maxDepth = tree.max_depth;
					s1_tree.nodeCount = tree.node_count;
					s1_tree.leafCount = tree.leaf_count;
					memcpy(s1_tree.coordMinGridSpace, tree.coord_min_grid_space, sizeof(int[3]));
					memcpy(s1_tree.coordMaxGridSpace, tree.coord_max_grid_space, sizeof(int[3]));
					memcpy(s1_tree.coordHalfSizeGridSpace, tree.coord_half_size_grid_space, sizeof(int[3]));
					s1_tree.defaultColorIndexBitCount = tree.default_color_index_bit_count;
					s1_tree.defaultLightIndexBitCount = tree.default_light_index_bit_count;
					s1_tree.p_nodeTable = mem.allocate<unsigned int>(static_cast<std::uint32_t>(tree.node_table.size()));
					memcpy(s1_tree.p_nodeTable, tree.node_table.data(), tree.node_table.size() * sizeof(unsigned int));
					s1_tree.leafTableSize = static_cast<int>(tree.leaf_table.size());
					if (!tree.leaf_table.empty())
					{
						s1_tree.p_leafTable = mem.allocate<unsigned char>(s1_tree.leafTableSize);
						memcpy(s1_tree.p_leafTable, tree.leaf_table.data(), tree.leaf_table.size());
					}
				}
			}

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

			memcpy(s1_asset->bounds.midPoint, asset->bounds.midPoint, sizeof(float[3]));
			memcpy(s1_asset->bounds.halfSize, asset->bounds.halfSize, sizeof(float[3]));
			memcpy(s1_asset->shadowBounds.midPoint, asset->bounds.midPoint, sizeof(float[3]));
			memcpy(s1_asset->shadowBounds.halfSize, asset->bounds.halfSize, sizeof(float[3]));

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
			REINTERPRET_CAST_SAFE_TO_FROM(s1_asset->sceneDynBrush, asset->sceneDynBrush);

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
					REINTERPRET_CAST_SAFE_TO_FROM(s1_asset->shadowGeom[i].smodelIndex, asset->shadowGeom[i].smodelIndex);
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
					REINTERPRET_CAST_SAFE_TO_FROM(s1_asset->lightRegion[i].hulls[j].axis, asset->lightRegion[i].hulls[j].axis);
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

			REINTERPRET_CAST_SAFE_TO_FROM(s1_asset->dpvs.smodelInsts, asset->dpvs.smodelInsts);

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
				s1_asset->dpvs.smodelDrawInsts[i].reactiveMotionCullDist = s1_asset->dpvs.smodelDrawInsts[i].cullDist;
				s1_asset->dpvs.smodelDrawInsts[i].reactiveMotionLOD = 0;

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

			s1_asset->dpvs.smodelLightingInsts = mem.allocate<S1::GfxStaticModelLighting>(s1_asset->dpvs.smodelCount);
			for (unsigned int i = 0; i < s1_asset->dpvs.smodelCount; i++)
			{
				if ((s1_asset->dpvs.smodelDrawInsts[i].flags & S1::StaticModelFlag::STATIC_MODEL_FLAG_GROUND_LIGHTING) != 0)
				{
					//bgra -> rgba
					auto ground_lighting = asset->dpvs.smodelDrawInsts[i].groundLighting;
					auto bgra = ground_lighting.array;

					float rgba[4] = { bgra[2] / 255.0f, bgra[1] / 255.0f, bgra[0] / 255.0f, bgra[3] / 255.0f };

					s1_asset->dpvs.smodelLightingInsts[i].ambientLightingInfo.groundLighting.array[0] = float_to_half(rgba[0]); // r
					s1_asset->dpvs.smodelLightingInsts[i].ambientLightingInfo.groundLighting.array[1] = float_to_half(rgba[1]); // g
					s1_asset->dpvs.smodelLightingInsts[i].ambientLightingInfo.groundLighting.array[2] = float_to_half(rgba[2]); // b
					s1_asset->dpvs.smodelLightingInsts[i].ambientLightingInfo.groundLighting.array[3] = float_to_half(rgba[3]); // a
				}
				else if ((s1_asset->dpvs.smodelDrawInsts[i].flags & S1::StaticModelFlag::STATIC_MODEL_FLAG_LIGHTGRID_LIGHTING) != 0)
				{
					//s1_asset->dpvs.smodelDrawInsts[i].flags |= S1::StaticModelFlag::STATIC_MODEL_FLAG_ALLOW_FXMARK; // R_CalcModelLighting: 0x240
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

			REINTERPRET_CAST_SAFE_TO_FROM(s1_asset->dpvs.surfaceCastsSunShadow, asset->dpvs.surfaceCastsSunShadow);
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
			REINTERPRET_CAST_SAFE_TO_FROM(s1_asset->heroOnlyLights, asset->heroOnlyLights);

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

			s1_asset->buildInfo.bspCommandline = nullptr;
			s1_asset->buildInfo.lightCommandline = nullptr;
			s1_asset->buildInfo.bspTimestamp = nullptr;
			s1_asset->buildInfo.lightTimestamp = nullptr;

			return s1_asset;
		}

		S1::GfxWorld* convert(GfxWorld* asset, allocator& allocator)
		{
			// generate h1 gfxworld
			return GenerateS1GfxWorld(asset, allocator);
		}
	}
}
