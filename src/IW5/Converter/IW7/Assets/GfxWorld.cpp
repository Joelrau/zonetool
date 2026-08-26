#include "stdafx.hpp"
#include "../Include.hpp"

#include "GfxWorld.hpp"

#include "X64/Utils/Utils.hpp"

#include <set>

namespace ZoneTool::IW5
{
	namespace IW7Converter
	{
		IW7::GfxImage* generate_reflection_probe_array_image(GfxWorldDraw* draw, allocator& allocator)
		{
			const std::string image_name = "*reflection_probe_array";
			/*const std::string image_name_clean = "_reflection_probe_array";

			std::uint32_t width = 0, height = 0, mip_levels = 0;
			std::uint16_t depth = 0;
			std::int32_t format = 0;
			bool once = false;

			std::vector<DirectX::Image> images{};

			for (unsigned int image_index = 1; image_index < draw->reflectionProbeCount; image_index++)
			{
				GfxImage* probe_image = draw->reflectionProbes[image_index];
				std::uint8_t* data = probe_image->pixelData;

				if (once) {
					assert(width == probe_image->width && height == probe_image->height && format == probe_image->imageFormat);
				}

				width = probe_image->width;
				height = probe_image->height;
				depth = probe_image->depth;
				mip_levels = probe_image->levelCount;
				format = probe_image->imageFormat;
				once = true;

				for (auto a = 0; a < 6; a++)
				{
					unsigned int divider = 1;
					for (auto i = 0; i < (int)probe_image->levelCount; i++)
					{
						DirectX::Image srcImg{};
						srcImg.width = std::max(1u, probe_image->width / divider);
						srcImg.height = std::max(1u, probe_image->height / divider);
						srcImg.format = DXGI_FORMAT(probe_image->imageFormat);
						srcImg.pixels = data;

						DirectX::ComputePitch(srcImg.format, srcImg.width, srcImg.height, srcImg.rowPitch, srcImg.slicePitch);

						DirectX::ScratchImage hdrTemp;
						auto hr = DirectX::Convert(srcImg, DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::TEX_FILTER_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, hdrTemp);

						if (FAILED(hr)) return nullptr;

						auto* persistentPixels = allocator.allocate_array<uint8_t>(hdrTemp.GetPixelsSize());
						memcpy(persistentPixels, hdrTemp.GetPixels(), hdrTemp.GetPixelsSize());

						DirectX::Image finalImg{};
						finalImg.width = srcImg.width;
						finalImg.height = srcImg.height;
						finalImg.format = DXGI_FORMAT_R16G16B16A16_FLOAT;
						finalImg.pixels = persistentPixels;
						DirectX::ComputePitch(finalImg.format, finalImg.width, finalImg.height, finalImg.rowPitch, finalImg.slicePitch);

						images.push_back(finalImg);

						data += srcImg.slicePitch;
						divider *= 2;
					}
				}
			}

			DirectX::TexMetadata mdata{};
			mdata.width = width;
			mdata.height = height;
			mdata.depth = depth;
			mdata.arraySize = (draw->reflectionProbeCount - 1) * 6;
			mdata.mipLevels = mip_levels;
			mdata.format = DXGI_FORMAT_R16G16B16A16_FLOAT;
			mdata.dimension = DirectX::TEX_DIMENSION_TEXTURE2D;
			mdata.miscFlags |= DirectX::TEX_MISC_TEXTURECUBE;

			DirectX::ScratchImage compressed;
			auto hr = DirectX::Compress(images.data(), images.size(), mdata, DXGI_FORMAT_BC6H_UF16, DirectX::TEX_COMPRESS_PARALLEL, DirectX::TEX_THRESHOLD_DEFAULT, compressed);

			if (FAILED(hr)) return nullptr;

			std::string spath = filesystem::get_dump_path() + "images\\" + image_name_clean + ".dds";
			std::wstring wpath(spath.begin(), spath.end());
			std::filesystem::create_directories(filesystem::get_dump_path() + "images\\");

			hr = DirectX::SaveToDDSFile(compressed.GetImages(), compressed.GetImageCount(), compressed.GetMetadata(), DirectX::DDS_FLAGS_NONE, wpath.data());
			if (FAILED(hr)) return nullptr;*/

			auto* image = allocator.allocate<IW7::GfxImage>();
			image->name = allocator.duplicate_string(image_name);
			return image;
		}

		// Umbra 3 tome version accepted by IW7. Load_UmbraTome -> Umbra::Tome::init
		// (iw7_ship_dump.exe 0x140E92DB0) validates only four things: the magic's high
		// word must be 0xD600, its low word must be in [0x12, 0x14], the tome must be
		// 16-byte aligned, and umbraTomeSize must be >= m_size. Shipped IW7 maps use
		// 0x14, which is also the version the 368-byte ImpTome layout belongs to.
		constexpr unsigned int UMBRA_TOME_VERSION_MAGIC = 0xD6000014;

		// The tome's view volume is deliberately parked outside anything reachable.
		// IW5/IW7 BSP coordinates are bounded to +/-131072, so a 64-unit cube at
		// +200000 on every axis can never contain the camera, while still sitting well
		// inside the +/-262144 range Umbra itself uses for tree bounds.
		constexpr float UMBRA_DEAD_VOLUME_ORIGIN = 200000.0f;
		constexpr float UMBRA_DEAD_VOLUME_SIZE = 64.0f;

		// CRC-32C (Castagnoli, reflected polynomial 0x82F63B78), init 0xFFFFFFFF with a
		// final complement. The tome stores it over the bytes from m_size onwards, i.e.
		// the whole blob minus its own m_versionMagic and m_crc32 fields. Verified
		// against both shipped mp_paris tomes (0xB9729593 and 0xC24D4414).
		//
		// IW7 does not actually verify this at load time, but it is cheap and something
		// else may well check it.
		unsigned int compute_umbra_tome_crc32(const void* data, std::size_t size)
		{
			const auto* bytes = static_cast<const unsigned char*>(data);
			unsigned int crc = 0xFFFFFFFF;

			for (std::size_t i = 0; i < size; i++)
			{
				crc ^= bytes[i];
				for (int bit = 0; bit < 8; bit++)
				{
					crc = (crc & 1u) ? ((crc >> 1) ^ 0x82F63B78u) : (crc >> 1);
				}
			}

			return ~crc;
		}

		// IW7 cannot draw a map that has no umbra tome. The static visibility worker
		// (iw7_ship_dump.exe 0x1405FB6D0) only ever fills the dpvs vis-data buffers from
		// inside its `if (g_world->umbraTomePtr)` block; with a null tome that block is
		// skipped entirely, nothing is ever marked visible, and the world draws empty.
		//
		// Inside that block, *any* non-zero Umbra query error falls through to
		// R_SetAllVisDataForScene (0x140DE3680), which memsets every vis buffer to 0xFF -
		// "draw everything, cull nothing". The follow-up pass that expands umbra object
		// IDs into vis bits (0x1405FAA30) only ORs bits in and returns immediately when
		// m_numObjects is 0, so it cannot take that visibility away again.
		//
		// So a tome that loads cleanly but whose query always fails gives us a fully
		// rendered, completely unculled map. That is the correct stopgap until a real
		// occlusion tome can be generated: IW5-sized maps on IW7-era hardware can afford
		// to draw everything, but they cannot afford to draw nothing.
		//
		// The failure is arranged by handing Umbra a structurally empty tome whose view
		// volume sits outside any reachable position, so every query reports "Camera
		// outside Umbra view volume".
		IW7::Umbra::ImpTome* generate_umbra_tome(allocator& allocator)
		{
			// the allocator zero-fills, which is what we want for every count and every
			// DataPtr offset in the tome: no tiles, no clusters, no objects, no gates.
			auto* tome = allocator.allocate<IW7::Umbra::ImpTome>();

			tome->m_versionMagic = UMBRA_TOME_VERSION_MAGIC;
			tome->m_size = sizeof(IW7::Umbra::ImpTome);

			// read by R_Umbra_QueryStaticCamera (0x1405FAFD0) to scale the LOD distance,
			// so it has to be a sane positive value. 128 is what shipped maps use.
			tome->m_lodBaseDistance = 128.0f;
			tome->m_flags = 0;

			tome->m_treeMin.x = UMBRA_DEAD_VOLUME_ORIGIN;
			tome->m_treeMin.y = UMBRA_DEAD_VOLUME_ORIGIN;
			tome->m_treeMin.z = UMBRA_DEAD_VOLUME_ORIGIN;
			tome->m_treeMax.x = UMBRA_DEAD_VOLUME_ORIGIN + UMBRA_DEAD_VOLUME_SIZE;
			tome->m_treeMax.y = UMBRA_DEAD_VOLUME_ORIGIN + UMBRA_DEAD_VOLUME_SIZE;
			tome->m_treeMax.z = UMBRA_DEAD_VOLUME_ORIGIN + UMBRA_DEAD_VOLUME_SIZE;

			tome->m_boundsMin = tome->m_treeMin;
			tome->m_boundsMax = tome->m_treeMax;

			tome->m_clusterCoordScale = 1.0f;

			tome->m_crc32 = compute_umbra_tome_crc32(
				&tome->m_size, tome->m_size - offsetof(IW7::Umbra::ImpTome, m_size));

			return tome;
		}

		IW7::GfxWorld* GenerateIW7GfxWorld(GfxWorld* asset, allocator& allocator)
		{
			const auto new_asset = allocator.allocate<IW7::GfxWorld>();

			new_asset->name = asset->name;
			new_asset->baseName = asset->baseName;
			new_asset->bspVersion = 159;

			COPY_VALUE(planeCount);
			COPY_VALUE(nodeCount);
			COPY_VALUE(surfaceCount);

			COPY_VALUE(skyCount);
			new_asset->skies = allocator.allocate<IW7::GfxSky>(asset->skyCount);
			for (int i = 0; i < asset->skyCount; i++)
			{
				COPY_VALUE(skies[i].skySurfCount);
				REINTERPRET_CAST_SAFE(skies[i].skyStartSurfs);
				COPY_ASSET(skies[i].skyImage);
				COPY_VALUE(skies[i].skySamplerState);
			}

			COPY_VALUE(lastSunPrimaryLightIndex);
			COPY_VALUE(primaryLightCount);
			new_asset->movingScriptablePrimaryLightCount = 0;

			new_asset->sortKeyLitDecal = 7;
			new_asset->sortKeyEffectDecal = 14;
			new_asset->sortKeyTopDecal = 17;
			new_asset->sortKeyEffectAuto = 35;
			new_asset->sortKeyDistortion = 24;
			new_asset->sortKeyEffectDistortion = 36;
			new_asset->sortKey2D = 41;
			new_asset->sortKeyOpaqueBegin = 1;
			new_asset->sortKeyOpaqueEnd = 6;
			new_asset->sortKeyDecalBegin = 7;
			new_asset->sortKeyDecalEnd = 17;
			new_asset->sortKeyTransBegin = 18;
			new_asset->sortKeyTransEnd = 34;
			new_asset->sortKeyEmissiveBegin = 35;
			new_asset->sortKeyEmissiveEnd = 40;

			COPY_VALUE(dpvsPlanes.cellCount);
			REINTERPRET_CAST_SAFE(dpvsPlanes.planes);
			REINTERPRET_CAST_SAFE(dpvsPlanes.nodes);
			REINTERPRET_CAST_SAFE(dpvsPlanes.sceneEntCellBits);
			new_asset->cells = allocator.allocate<IW7::GfxCell>(asset->dpvsPlanes.cellCount);
			for (int i = 0; i < new_asset->dpvsPlanes.cellCount; i++)
			{
				memcpy(&new_asset->cells[i].bounds, &asset->cells[i].bounds, sizeof(float[2][3]));
				new_asset->cells[i].portalCount = asset->cells[i].portalCount;

				auto add_portal = [](IW7::GfxPortal* iw7_portal, IW5::GfxPortal* iw5_portal)
				{
					memcpy(&iw7_portal->plane, &iw5_portal->plane, sizeof(float[4]));
					iw7_portal->vertices = reinterpret_cast<float(*__ptr64)[3]>(iw5_portal->vertices);
					iw7_portal->cellIndex = iw5_portal->cellIndex;
					iw7_portal->closeDistance = 0;
					iw7_portal->vertexCount = iw5_portal->vertexCount;
					memcpy(&iw7_portal->hullAxis, &iw5_portal->hullAxis, sizeof(float[2][3]));
				};
				new_asset->cells[i].portals = allocator.allocate<IW7::GfxPortal>(new_asset->cells[i].portalCount);
				for (int j = 0; j < new_asset->cells[i].portalCount; j++)
				{
					add_portal(&new_asset->cells[i].portals[j], &asset->cells[i].portals[j]);
				}
			}

			new_asset->cellTransientInfos = allocator.allocate<IW7::GfxCellTransientInfo>(asset->dpvsPlanes.cellCount);
			for (unsigned short i = 0; i < asset->dpvsPlanes.cellCount; i++)
			{
				new_asset->cellTransientInfos[i].aabbTreeIndex = i;
				new_asset->cellTransientInfos[i].transientZone = 0;
			}

			assert(asset->draw.reflectionProbeCount);
			new_asset->draw.reflectionProbeData.reflectionProbeCount = asset->draw.reflectionProbeCount;
			new_asset->draw.reflectionProbeData.sharedReflectionProbeCount = 0;
			new_asset->draw.reflectionProbeData.reflectionProbes = allocator.allocate<IW7::GfxReflectionProbe>(asset->draw.reflectionProbeCount);
			new_asset->draw.reflectionProbeData.reflectionProbeArrayImage = generate_reflection_probe_array_image(&asset->draw, allocator);

			new_asset->draw.reflectionProbeData.probeRelightingCount = 0;
			new_asset->draw.reflectionProbeData.probeRelightingData = nullptr;

			new_asset->draw.reflectionProbeData.reflectionProbeGBufferImageCount = 0;
			new_asset->draw.reflectionProbeData.reflectionProbeGBufferImages = nullptr;
			new_asset->draw.reflectionProbeData.reflectionProbeGBufferTextures = nullptr;

			new_asset->draw.reflectionProbeData.reflectionProbeLightgridSampleData =
				allocator.allocate<IW7::GfxReflectionProbeSampleData>(new_asset->draw.reflectionProbeData.reflectionProbeCount);
			new_asset->draw.reflectionProbeData.reflectionProbeLightgridSampleDataBuffer = nullptr;
			new_asset->draw.reflectionProbeData.reflectionProbeLightgridSampleDataBufferView = nullptr;
			new_asset->draw.reflectionProbeData.reflectionProbeLightgridSampleDataBufferRWView = nullptr;

			{
				constexpr float kFallbackVolumeHalfExtent = 262144.0f; // "infinite" bounding volume
				constexpr float kFallbackFeather = 8.0f;
				constexpr unsigned short kIdentityQuat[4] = {}; // placeholder, see below

				const unsigned int probeCount = asset->draw.reflectionProbeCount;
				const unsigned int totalInstanceCount = probeCount + 1; // +1 for the null/fallback probe

				// 1. Allocation
				new_asset->draw.reflectionProbeData.reflectionProbeInstanceCount = totalInstanceCount;

				auto* instances = allocator.allocate<IW7::GfxReflectionProbeInstance>(totalInstanceCount);
				new_asset->draw.reflectionProbeData.reflectionProbeInstances = instances;

				// Index buffer size must match instance count
				auto* globalProbeInstanceIndices = allocator.allocate<unsigned int>(totalInstanceCount);

				// 2. Build pass � one instance per source probe
				for (unsigned int i = 0; i < probeCount; i++)
				{
					auto& srcProbe = asset->draw.reflectionProbeOrigins[i];
					auto& dstProbe = new_asset->draw.reflectionProbeData.reflectionProbes[i];
					auto& inst = instances[i];

					// --- probe entry ---
					dstProbe.livePath = nullptr;
					memcpy(dstProbe.origin, srcProbe.origin, sizeof(vec3_t));
					memset(dstProbe.angles, 0, sizeof(vec3_t));
					dstProbe.probeRelightingIndex = static_cast<unsigned int>(-1);
					dstProbe.probeInstanceCount = 1;
					dstProbe.probeInstances = &globalProbeInstanceIndices[i];
					dstProbe.probeInstances[0] = i;

					// --- instance entry ---
					memset(&inst, 0, sizeof(inst));
					memcpy(inst.probePosition, srcProbe.origin, sizeof(vec3_t));
					inst.probeImageIndex = static_cast<unsigned short>(i);
					inst.priority = -1.0f;
					inst.probeRotation[0] = 0.0f;
					inst.probeRotation[1] = 0.0f;
					inst.probeRotation[2] = 0.0f;
					inst.probeRotation[3] = 1.0f; // identity quat

					if (i == 0)
					{
						// First probe doubles as the world-fallback volume: lowest
						// priority, huge bounds, axis-aligned.
						inst.priority = -FLT_MAX;
						memcpy(inst.volumeObb.center, dstProbe.origin, sizeof(vec3_t));

						inst.volumeObb.halfSize[0] = kFallbackVolumeHalfExtent;
						inst.volumeObb.halfSize[1] = kFallbackVolumeHalfExtent;
						inst.volumeObb.halfSize[2] = kFallbackVolumeHalfExtent;

						inst.volumeObb.xAxis[0] = 1.0f; inst.volumeObb.xAxis[1] = 0.0f; inst.volumeObb.xAxis[2] = 0.0f;
						inst.volumeObb.yAxis[0] = 0.0f; inst.volumeObb.yAxis[1] = 1.0f; inst.volumeObb.yAxis[2] = 0.0f;
						inst.volumeObb.zAxis[0] = 0.0f; inst.volumeObb.zAxis[1] = 0.0f; inst.volumeObb.zAxis[2] = 1.0f;

						inst.feather[0] = inst.feather[1] = inst.feather[2] = kFallbackFeather;
					}
				}

				// NOTE: instances[probeCount] (the reserved "+1" slot) is allocated but
				// never initialized here � currently left as raw allocator memory.
			}

			// todo...
			//new_asset->draw.lightmapReindexData;

			new_asset->draw.iesLookupTexture = allocator.allocate<IW7::GfxImage>();
			new_asset->draw.iesLookupTexture->name = allocator.duplicate_string("*ieslookup");

			new_asset->draw.decalVolumeCollectionCount = 0;
			new_asset->draw.decalVolumeCollections = nullptr;

			COPY_ASSET(draw.lightmapOverridePrimary);
			COPY_ASSET(draw.lightmapOverrideSecondary);

			new_asset->draw.lightMapCount = asset->draw.lightmapCount;
			new_asset->draw.lightMaps = allocator.allocate<IW7::GfxLightMap PTR64>(asset->draw.lightmapCount);
			for (int i = 0; i < asset->draw.lightmapCount; i++)
			{
				new_asset->draw.lightMaps[i] = allocator.allocate<IW7::GfxLightMap>();
				new_asset->draw.lightMaps[i]->name = allocator.duplicate_string(va("*lightmap%d", i));
				if (asset->draw.lightmaps[i].primary) // primary
				{
					new_asset->draw.lightMaps[i]->textures[0] = allocator.allocate<IW7::GfxImage>();
					new_asset->draw.lightMaps[i]->textures[0]->name = asset->draw.lightmaps[i].primary->name;
				}
				if (asset->draw.lightmaps[i].secondary) // secondary
				{
					new_asset->draw.lightMaps[i]->textures[1] = allocator.allocate<IW7::GfxImage>();
					new_asset->draw.lightMaps[i]->textures[1]->name = asset->draw.lightmaps[i].secondary->name;
				}
				new_asset->draw.lightMaps[i]->textures[2] = allocator.allocate<IW7::GfxImage>();
				new_asset->draw.lightMaps[i]->textures[2]->name = allocator.duplicate_string(va("*lightmap%d_secondunorm", i));
			}
			new_asset->draw.lightmapTextures = nullptr; // runtime data, allocated elsewhere

			new_asset->draw.unused1 = nullptr;
			new_asset->draw.unused2 = nullptr;
			new_asset->draw.unused3 = nullptr;

			new_asset->draw.transientZoneCount = 1;
			new_asset->draw.transientZones[0] = allocator.allocate<IW7::GfxWorldTransientZone>();
			new_asset->draw.transientZones[0]->name = allocator.duplicate_string(filesystem::get_fastfile());
			new_asset->draw.transientZones[0]->transientZoneIndex = 0;

			new_asset->draw.transientZones[0]->vertexCount = asset->draw.vertexCount;
			new_asset->draw.transientZones[0]->vd.vertices = allocator.allocate<IW7::GfxWorldVertex>(asset->draw.vertexCount);
			for (unsigned int i = 0; i < asset->draw.vertexCount; i++)
			{
				static_assert(sizeof(GfxWorldVertex) == sizeof(IW7::GfxWorldVertex));
				memcpy(&new_asset->draw.transientZones[0]->vd.vertices[i], &asset->draw.vd.vertices[i], sizeof(GfxWorldVertex));

				// re-calculate these...
				float normal_unpacked[3]{ 0.0f, 0.0f, 0.0f };
				PackedVec::Vec3UnpackUnitVec(asset->draw.vd.vertices[i].normal.array, normal_unpacked);

				float tangent_unpacked[3]{ 0.0f, 0.0f, 0.0f };
				PackedVec::Vec3UnpackUnitVec(asset->draw.vd.vertices[i].tangent.array, tangent_unpacked);

				float normal[3] = { normal_unpacked[0], normal_unpacked[1], normal_unpacked[2] };
				float tangent[3] = { tangent_unpacked[0], tangent_unpacked[1], tangent_unpacked[2] };

				new_asset->draw.transientZones[0]->vd.vertices[i].normal.packed = PackedVec::Vec3PackUnitVec(normal);
				new_asset->draw.transientZones[0]->vd.vertices[i].tangent.packed = PackedVec::Vec3PackUnitVec(tangent);

				// correct color : bgra->rgba
				new_asset->draw.transientZones[0]->vd.vertices[i].color.array[0] = asset->draw.vd.vertices[i].color.array[2];
				new_asset->draw.transientZones[0]->vd.vertices[i].color.array[1] = asset->draw.vd.vertices[i].color.array[1];
				new_asset->draw.transientZones[0]->vd.vertices[i].color.array[2] = asset->draw.vd.vertices[i].color.array[0];
				new_asset->draw.transientZones[0]->vd.vertices[i].color.array[3] = asset->draw.vd.vertices[i].color.array[3];
			}

			new_asset->draw.transientZones[0]->vertexLayerDataSize = asset->draw.vertexLayerDataSize;
			new_asset->draw.transientZones[0]->vld.data = asset->draw.vld.data;

			new_asset->draw.transientZones[0]->cellCount = asset->dpvsPlanes.cellCount;

			new_asset->draw.transientZones[0]->aabbTreeCounts = allocator.allocate<IW7::GfxCellTreeCount>(asset->dpvsPlanes.cellCount);
			new_asset->draw.transientZones[0]->aabbTrees = allocator.allocate<IW7::GfxCellTree>(asset->dpvsPlanes.cellCount);
			for (int i = 0; i < asset->dpvsPlanes.cellCount; i++)
			{
				new_asset->draw.transientZones[0]->aabbTreeCounts[i].aabbTreeCount = asset->aabbTreeCounts[i].aabbTreeCount;
				new_asset->draw.transientZones[0]->aabbTrees[i].aabbTree = allocator.allocate<IW7::GfxAabbTree>(asset->aabbTreeCounts[i].aabbTreeCount);
				for (int j = 0; j < asset->aabbTreeCounts[i].aabbTreeCount; j++)
				{
					memcpy(&new_asset->draw.transientZones[0]->aabbTrees[i].aabbTree[j].bounds, &asset->aabbTrees[i].aabbTree[j].bounds, sizeof(float[2][3]));

					new_asset->draw.transientZones[0]->aabbTrees[i].aabbTree[j].startSurfIndex = asset->aabbTrees[i].aabbTree[j].startSurfIndex;
					new_asset->draw.transientZones[0]->aabbTrees[i].aabbTree[j].surfaceCount = asset->aabbTrees[i].aabbTree[j].surfaceCount;

					new_asset->draw.transientZones[0]->aabbTrees[i].aabbTree[j].smodelIndexCount = asset->aabbTrees[i].aabbTree[j].smodelIndexCount;
					new_asset->draw.transientZones[0]->aabbTrees[i].aabbTree[j].smodelIndexes = asset->aabbTrees[i].aabbTree[j].smodelIndexes;

					new_asset->draw.transientZones[0]->aabbTrees[i].aabbTree[j].childCount = asset->aabbTrees[i].aabbTree[j].childCount;

					// re-calculate childrenOffset
					auto offset = asset->aabbTrees[i].aabbTree[j].childrenOffset;
					int childrenIndex = offset / sizeof(GfxAabbTree);
					int childrenOffset = childrenIndex * sizeof(IW7::GfxAabbTree);
					new_asset->draw.transientZones[0]->aabbTrees[i].aabbTree[j].childrenOffset = childrenOffset;
				}
			}

			new_asset->draw.indexCount = asset->draw.indexCount;
			new_asset->draw.indices = asset->draw.indices;

			// todo...
			{
				new_asset->draw.volumetrics.volumetricCount = 0;
				new_asset->draw.volumetrics.volumetrics = nullptr;
				constexpr int unk_values[] = { 0, 0, 5, 5, 6, 32, 32, 64, 0 };
				memcpy(new_asset->lightGrid.unk, unk_values, sizeof(unk_values));
				new_asset->lightGrid.tableVersion = 0;
				new_asset->lightGrid.paletteVersion = 0;
				new_asset->lightGrid.rangeExponent8BitsEncoding = 0;
				new_asset->lightGrid.rangeExponent12BitsEncoding = 0;
				new_asset->lightGrid.rangeExponent16BitsEncoding = 0;
				new_asset->lightGrid.stageCount = 0;
				new_asset->lightGrid.stageLightingContrastGain = 0;
				new_asset->lightGrid.paletteEntryCount = 0;
				new_asset->lightGrid.paletteEntryAddress = 0;
				new_asset->lightGrid.paletteBitstreamSize = 0;
				new_asset->lightGrid.paletteBitstream = 0;
				for (unsigned int j = 0; j < 56; j++)
				{
					auto& dest_rgb = new_asset->lightGrid.skyLightGridColors.rgb[j];
					dest_rgb[0] = 0;
					dest_rgb[1] = 0;
					dest_rgb[2] = 0;
				}
				for (unsigned int j = 0; j < 56; j++)
				{
					auto& dest_rgb = new_asset->lightGrid.defaultLightGridColors.rgb[j];
					dest_rgb[0] = 0;
					dest_rgb[1] = 0;
					dest_rgb[2] = 0;
				}
				new_asset->lightGrid.tree.maxDepth = 0;
				new_asset->lightGrid.tree.nodeCount = 0;
				new_asset->lightGrid.tree.leafCount = 0;
				memset(&new_asset->lightGrid.tree.coordMinGridSpace, 0, sizeof(int[3]));
				memset(&new_asset->lightGrid.tree.coordMaxGridSpace, 0, sizeof(int[3]));
				memset(&new_asset->lightGrid.tree.coordHalfSizeGridSpace, 0, sizeof(int[3]));
				new_asset->lightGrid.tree.defaultColorIndexBitCount = 0;
				new_asset->lightGrid.tree.defaultLightIndexBitCount = 0;
				new_asset->lightGrid.tree.p_nodeTable = nullptr;
				new_asset->lightGrid.tree.leafTableSize = 0;
				new_asset->lightGrid.tree.p_leafTable = nullptr;

				memset(&new_asset->lightGrid.probeData, 0, sizeof(IW7::GfxLightGridProbeData));
				// fixme somehow...
				new_asset->lightGrid.probeData.zoneCount = 1;
				new_asset->lightGrid.probeData.zones = allocator.allocate<IW7::GfxGpuLightGridZone>(1);
				new_asset->lightGrid.probeData.zones->numProbes = 0;
				new_asset->lightGrid.probeData.zones->firstProbe = 0;
				new_asset->lightGrid.probeData.zones->numTetrahedrons = 0;
				new_asset->lightGrid.probeData.zones->firstTetrahedron = 0;
				new_asset->lightGrid.probeData.zones->firstVoxelTetrahedronIndex = 0;
				new_asset->lightGrid.probeData.zones->numVoxelTetrahedronIndices = 0;
				memset(new_asset->lightGrid.probeData.zones->fallbackProbeData.coeffs, 0, sizeof(new_asset->lightGrid.probeData.zones->fallbackProbeData.coeffs));
				memset(new_asset->lightGrid.probeData.zones->fallbackProbeData.pad, 0, sizeof(new_asset->lightGrid.probeData.zones->fallbackProbeData.pad));
			}

			// todo...
			new_asset->frustumLights = allocator.allocate<IW7::GfxFrustumLights>(new_asset->primaryLightCount);
			new_asset->lightViewFrustums = allocator.allocate<IW7::GfxLightViewFrustum>(new_asset->primaryLightCount);

			// todo...
			{
				new_asset->voxelTreeCount = new_asset->skyCount;
				new_asset->voxelTree = allocator.allocate<IW7::GfxVoxelTree>(new_asset->voxelTreeCount);
				for (auto i = 0; i < new_asset->skyCount; i++)
				{
					const auto get_sky_bounds = [](const GfxSky& sky, const GfxWorld* world) -> Bounds
					{
						Bounds bounds{};
						bounds.midPoint[0] = 0.0f;
						bounds.midPoint[1] = 0.0f;
						bounds.midPoint[2] = 0.0f;
						bounds.halfSize[0] = 0.0f;
						bounds.halfSize[1] = 0.0f;
						bounds.halfSize[2] = 0.0f;
						for (int j = 0; j < sky.skySurfCount; j++)
						{
							auto index = world->dpvs.sortedSurfIndex[sky.skyStartSurfs[j]];
							auto surface_bounds = &world->dpvs.surfacesBounds[index];
							bounds.midPoint[0] += surface_bounds->bounds.midPoint[0];
							bounds.midPoint[1] += surface_bounds->bounds.midPoint[1];
							bounds.midPoint[2] += surface_bounds->bounds.midPoint[2];
							bounds.halfSize[0] += surface_bounds->bounds.halfSize[0];
							bounds.halfSize[1] += surface_bounds->bounds.halfSize[1];
							bounds.halfSize[2] += surface_bounds->bounds.halfSize[2];
						}
						if (sky.skySurfCount > 0)
						{
							float inv_count = 1.0f / static_cast<float>(sky.skySurfCount);
							bounds.midPoint[0] *= inv_count;
							bounds.midPoint[1] *= inv_count;
							bounds.midPoint[2] *= inv_count;
							bounds.halfSize[0] *= inv_count;
							bounds.halfSize[1] *= inv_count;
							bounds.halfSize[2] *= inv_count;
						}
						return bounds;
					};

					auto sky_bounds = get_sky_bounds(asset->skies[i], asset);
					memcpy(&new_asset->voxelTree[i].zoneBound, &sky_bounds, sizeof(Bounds));

					new_asset->voxelTree[i].voxelTopDownViewNodeCount = 1;
					new_asset->voxelTree[i].voxelInternalNodeCount = 1;
					new_asset->voxelTree[i].voxelLeafNodeCount = 1;
					new_asset->voxelTree[i].lightListArraySize = 1;

					new_asset->voxelTree[i].voxelTreeHeader = allocator.allocate<IW7::GfxVoxelTreeHeader>();
					memset(&new_asset->voxelTree[i].voxelTreeHeader->rootNodeDimension, 0, sizeof(int[4]));
					memset(&new_asset->voxelTree[i].voxelTreeHeader->nodeCoordBitShift, 0, sizeof(int[4]));
					memset(&new_asset->voxelTree[i].voxelTreeHeader->boundMin, 0, sizeof(float[4]));
					memset(&new_asset->voxelTree[i].voxelTreeHeader->boundMax, 0, sizeof(float[4]));

					new_asset->voxelTree[i].voxelTopDownViewNodeArray = allocator.allocate<IW7::GfxVoxelTopDownViewNode>(new_asset->voxelTree[i].voxelTopDownViewNodeCount);
					new_asset->voxelTree[i].voxelTopDownViewNodeArray->firstNodeIndex = -1;
					new_asset->voxelTree[i].voxelTopDownViewNodeArray->zMin = 2147483647;
					new_asset->voxelTree[i].voxelTopDownViewNodeArray->zMax = -2147483648;

					new_asset->voxelTree[i].voxelInternalNodeArray = allocator.allocate<IW7::GfxVoxelInternalNode>(new_asset->voxelTree[i].voxelInternalNodeCount);
					new_asset->voxelTree[i].voxelInternalNodeArray->firstNodeIndex[0] = 0;
					new_asset->voxelTree[i].voxelInternalNodeArray->firstNodeIndex[1] = 0;
					new_asset->voxelTree[i].voxelInternalNodeArray->childNodeMask[0] = 0;
					new_asset->voxelTree[i].voxelInternalNodeArray->childNodeMask[1] = 0;

					new_asset->voxelTree[i].voxelLeafNodeArray = allocator.allocate<IW7::GfxVoxelLeafNode>(new_asset->voxelTree[i].voxelLeafNodeCount);
					new_asset->voxelTree[i].voxelLeafNodeArray->lightListAddress = 0;

					new_asset->voxelTree[i].lightListArray = allocator.allocate<unsigned short>(new_asset->voxelTree[i].lightListArraySize);
					new_asset->voxelTree[i].lightListArray[0] = 0;

					new_asset->voxelTree[i].voxelInternalNodeDynamicLightList = allocator.allocate<unsigned int>(2 * new_asset->voxelTree[i].voxelInternalNodeCount);
					new_asset->voxelTree[i].voxelInternalNodeDynamicLightList[0] = 0;
					new_asset->voxelTree[i].voxelInternalNodeDynamicLightList[1] = 0;
				}
			}

			// todo...
			new_asset->heightfieldCount = 0;
			new_asset->heightfields = nullptr;

			// irrelevant
			new_asset->unk01.unk01Count = 0;
			new_asset->unk01.unk01 = nullptr;
			new_asset->unk01.unk02Count = 0;
			new_asset->unk01.unk02 = nullptr;
			new_asset->unk01.unk03Count = 0;
			new_asset->unk01.unk03 = nullptr;

			COPY_VALUE(modelCount);
			new_asset->models = allocator.allocate<IW7::GfxBrushModel>(asset->modelCount);
			for (int i = 0; i < asset->modelCount; i++)
			{
				COPY_ARR(models[i].bounds);
				COPY_VALUE(models[i].radius);
				COPY_VALUE_CAST(models[i].startSurfIndex);
				COPY_VALUE(models[i].surfaceCount);
			}

			std::memcpy(&new_asset->bounds, &asset->bounds, sizeof(Bounds));
			
			COPY_VALUE(checksum);

			COPY_VALUE(materialMemoryCount);
			new_asset->materialMemory = allocator.allocate<IW7::MaterialMemory>(new_asset->materialMemoryCount);
			for (int i = 0; i < new_asset->materialMemoryCount; i++)
			{
				new_asset->materialMemory[i].material = reinterpret_cast<IW7::Material PTR64>(asset->materialMemory[i].material);
				new_asset->materialMemory[i].memory = asset->materialMemory[i].memory;
			}

			COPY_VALUE_CAST(sun);
			COPY_ARR(outdoorLookupMatrix);
			COPY_ASSET(outdoorImage);
			new_asset->dustMaterial = nullptr;
			new_asset->materialLod0SizeThreshold = 0.5f;

			if (asset->shadowGeom)
			{
				new_asset->shadowGeomOptimized = allocator.allocate<IW7::GfxShadowGeometry>(new_asset->primaryLightCount);
				for (unsigned int i = 0; i < new_asset->primaryLightCount; i++)
				{
					new_asset->shadowGeomOptimized[i].surfaceCount = asset->shadowGeom[i].surfaceCount;
					new_asset->shadowGeomOptimized[i].smodelCount = asset->shadowGeom[i].smodelCount;
					new_asset->shadowGeomOptimized[i].sortedSurfIndex = allocator.allocate<unsigned int>(new_asset->shadowGeomOptimized[i].surfaceCount);
					for (unsigned int j = 0; j < new_asset->shadowGeomOptimized[i].surfaceCount; j++)
					{
						new_asset->shadowGeomOptimized[i].sortedSurfIndex[j] = asset->shadowGeom[i].sortedSurfIndex[j];
					}
					REINTERPRET_CAST_SAFE_TO_FROM(new_asset->shadowGeomOptimized[i].smodelIndex, asset->shadowGeom[i].smodelIndex);
				}
			}

			new_asset->lightRegion = allocator.allocate<IW7::GfxLightRegion>(new_asset->primaryLightCount);
			for (unsigned int i = 0; i < new_asset->primaryLightCount; i++)
			{
				new_asset->lightRegion[i].hullCount = asset->lightRegion[i].hullCount;
				new_asset->lightRegion[i].hulls = allocator.allocate<IW7::GfxLightRegionHull>(new_asset->lightRegion[i].hullCount);
				for (unsigned int j = 0; j < new_asset->lightRegion[i].hullCount; j++)
				{
					memcpy(&new_asset->lightRegion[i].hulls[j].kdopMidPoint, &asset->lightRegion[i].hulls[j].kdopMidPoint, sizeof(float[9]));
					memcpy(&new_asset->lightRegion[i].hulls[j].kdopHalfSize, &asset->lightRegion[i].hulls[j].kdopHalfSize, sizeof(float[9]));

					new_asset->lightRegion[i].hulls[j].axisCount = asset->lightRegion[i].hulls[j].axisCount;
					REINTERPRET_CAST_SAFE_TO_FROM(new_asset->lightRegion[i].hulls[j].axis, asset->lightRegion[i].hulls[j].axis);
				}
			}

			// todo?...
			new_asset->lightAABB.nodeCount = 0;
			new_asset->lightAABB.lightCount = 0;
			new_asset->lightAABB.nodeArray = nullptr;
			new_asset->lightAABB.lightArray = nullptr;

			// dpvs
			{
				COPY_VALUE(dpvs.smodelCount);
				COPY_VALUE(dpvs.staticSurfaceCount);
				COPY_VALUE(dpvs.litOpaqueSurfsBegin);
				COPY_VALUE(dpvs.litOpaqueSurfsEnd);
				new_asset->dpvs.litDecalSurfsBegin = new_asset->dpvs.litOpaqueSurfsEnd; // skip
				new_asset->dpvs.litDecalSurfsEnd = new_asset->dpvs.litOpaqueSurfsEnd; // skip
				COPY_VALUE(dpvs.litTransSurfsBegin);
				COPY_VALUE(dpvs.litTransSurfsEnd);
				COPY_VALUE(dpvs.emissiveSurfsBegin);
				COPY_VALUE(dpvs.emissiveSurfsEnd);
				new_asset->dpvs.smodelVisDataCount = (new_asset->dpvs.smodelCount + 0x1F) >> 5;
				new_asset->dpvs.surfaceVisDataCount = (new_asset->surfaceCount + 0x1F) >> 5;
				new_asset->dpvs.primaryLightVisDataCount = (new_asset->primaryLightCount + 0x1F) >> 5;
				new_asset->dpvs.reflectionProbeVisDataCount = (new_asset->draw.reflectionProbeData.reflectionProbeInstanceCount + 0x1F) >> 5;
				new_asset->dpvs.volumetricVisDataCount = (new_asset->draw.volumetrics.volumetricCount + 0x1F) >> 5;
				new_asset->dpvs.decalVisDataCount = (new_asset->draw.decalVolumeCollectionCount + 0x1F) >> 5;
				new_asset->dpvs.lodData = allocator.allocate<unsigned int>(new_asset->dpvs.smodelCount + 1);
				new_asset->dpvs.sortedSurfIndex = allocator.allocate<unsigned int>(new_asset->dpvs.staticSurfaceCount);
				for (unsigned int i = 0; i < new_asset->dpvs.staticSurfaceCount; i++)
				{
					new_asset->dpvs.sortedSurfIndex[i] = asset->dpvs.sortedSurfIndex[i];
				}
				REINTERPRET_CAST_SAFE(dpvs.smodelInsts);

				new_asset->dpvs.surfaces = allocator.allocate<IW7::GfxSurface>(asset->surfaceCount);
				for (unsigned int i = 0; i < asset->surfaceCount; i++)
				{
					COPY_VALUE(dpvs.surfaces[i].tris.vertexLayerData);
					COPY_VALUE(dpvs.surfaces[i].tris.firstVertex);
					new_asset->dpvs.surfaces[i].tris.maxEdgeLength = 0;
					COPY_VALUE(dpvs.surfaces[i].tris.vertexCount);
					COPY_VALUE(dpvs.surfaces[i].tris.triCount);
					COPY_VALUE(dpvs.surfaces[i].tris.baseIndex);
					new_asset->dpvs.surfaces[i].material = reinterpret_cast<IW7::Material*>(asset->dpvs.surfaces[i].material);
					new_asset->dpvs.surfaces[i].lightmapIndex = asset->dpvs.surfaces[i].laf.fields.lightmapIndex;
					new_asset->dpvs.surfaces[i].flags = asset->dpvs.surfaces[i].laf.fields.flags;

					new_asset->dpvs.surfaces[i].unk1 = 0;
					new_asset->dpvs.surfaces[i].unk2 = 0;
					new_asset->dpvs.surfaces[i].unk3 = 0;
					new_asset->dpvs.surfaces[i].unk4 = 0;

					new_asset->dpvs.surfaces[i].transientZone = 0;
				}

				new_asset->dpvs.surfacesBounds = allocator.allocate<IW7::GfxSurfaceBounds>(asset->surfaceCount);
				for (unsigned int i = 0; i < asset->surfaceCount; i++)
				{
					COPY_ARR(dpvs.surfacesBounds[i].bounds);
				}

				new_asset->dpvs.smodelDrawInsts = allocator.allocate<IW7::GfxStaticModelDrawInst>(asset->dpvs.smodelCount);
				for (unsigned int i = 0; i < asset->dpvs.smodelCount; i++)
				{
					COPY_VALUE_CAST(dpvs.smodelDrawInsts[i].placement);

					new_asset->dpvs.smodelDrawInsts[i].model =
						reinterpret_cast<IW7::XModel*>(asset->dpvs.smodelDrawInsts[i].model);

					auto& src_draw_inst = asset->dpvs.smodelDrawInsts[i];

					new_asset->dpvs.smodelDrawInsts[i].modelLightmapInfo.lightmapIndex = -1;

					new_asset->dpvs.smodelDrawInsts[i].lightingHandle = asset->dpvs.smodelDrawInsts[i].lightingHandle;
					new_asset->dpvs.smodelDrawInsts[i].cullDist = asset->dpvs.smodelDrawInsts[i].cullDist;
					new_asset->dpvs.smodelDrawInsts[i].flags = asset->dpvs.smodelDrawInsts[i].flags;
					new_asset->dpvs.smodelDrawInsts[i].primaryLightEnvIndex = asset->dpvs.smodelDrawInsts[i].primaryLightIndex;
					new_asset->dpvs.smodelDrawInsts[i].reflectionProbeIndex = asset->dpvs.smodelDrawInsts[i].reflectionProbeIndex;
					new_asset->dpvs.smodelDrawInsts[i].firstMtlSkinIndex = asset->dpvs.smodelDrawInsts[i].firstMtlSkinIndex;
					new_asset->dpvs.smodelDrawInsts[i].sunShadowFlags = 0;
					new_asset->dpvs.smodelDrawInsts[i].transientZone = 0;

					auto& iw7_draw_inst = new_asset->dpvs.smodelDrawInsts[i];
					auto& draw_inst = asset->dpvs.smodelDrawInsts[i];

					// g_lodDistIndexToScale
					iw7_draw_inst.flags |= IW7::StaticModelFlag::STATIC_MODEL_FLAG_SCALE_9; // 1.0f

					// casts no shadows
					auto no_shadows = (draw_inst.flags & 0x10) != 0;
					if (no_shadows)
					{
						iw7_draw_inst.flags |= IW7::StaticModelFlag::STATIC_MODEL_FLAG_NO_CAST_SHADOW;
					}

					// ground lighting
					auto ground_lighting = (draw_inst.flags & 0x20) != 0;
					if (ground_lighting)
					{
						iw7_draw_inst.flags |= IW7::StaticModelFlag::STATIC_MODEL_FLAG_GROUND_LIGHTING;
					}

					// regular lighting
					iw7_draw_inst.flags |= IW7::StaticModelFlag::STATIC_MODEL_FLAG_LIGHTGRID_LIGHTING;
				}

				new_asset->dpvs.surfaceMaterials = allocator.allocate<IW7::GfxDrawSurf>(new_asset->surfaceCount);
				memset(new_asset->dpvs.surfaceMaterials, 0, 
					sizeof(IW7::GfxDrawSurf) * new_asset->surfaceCount); // zero data, runtime

				REINTERPRET_CAST_SAFE(dpvs.surfaceCastsSunShadow);

				// todo...
				new_asset->dpvs.sunShadowOptCount = 0;
				new_asset->dpvs.sunSurfVisDataCount = 0;
				new_asset->dpvs.surfaceCastsSunShadowOpt = nullptr; // fixme

				// todo...
				new_asset->dpvs.sortedSmodelIndices = allocator.allocate<unsigned short>(asset->dpvs.smodelCount);

				// todo...
				new_asset->dpvs.constantBuffers = nullptr;

				COPY_VALUE(dpvs.usageCount);
			}

			COPY_ARR(dpvsDyn.dynEntClientWordCount);
			COPY_ARR(dpvsDyn.dynEntClientCount);

			{
				new_asset->dpvsDyn.dynEntClientCount[0] += 64; // reserve_dynents
				new_asset->dpvsDyn.dynEntClientWordCount[0] += 2; // reserve_dynents ( 64 >> 5 )

				new_asset->dpvsDyn.dynEntCellBits[0] = allocator.allocate<unsigned int>(new_asset->dpvsDyn.dynEntClientCount[0] * new_asset->dpvsPlanes.cellCount); // runtime
				new_asset->dpvsDyn.dynEntCellBits[1] = allocator.allocate<unsigned int>(new_asset->dpvsDyn.dynEntClientCount[1] * new_asset->dpvsPlanes.cellCount); // runtime

				// 0 - 3 are valid.
				new_asset->dpvsDyn.dynEntVisData[0][0] = allocator.allocate<unsigned char>(32 * new_asset->dpvsDyn.dynEntClientWordCount[0]); // runtime
				new_asset->dpvsDyn.dynEntVisData[0][1] = allocator.allocate<unsigned char>(32 * new_asset->dpvsDyn.dynEntClientWordCount[0]); // runtime
				new_asset->dpvsDyn.dynEntVisData[0][2] = allocator.allocate<unsigned char>(32 * new_asset->dpvsDyn.dynEntClientWordCount[0]); // runtime

				new_asset->dpvsDyn.dynEntVisData[1][0] = allocator.allocate<unsigned char>(32 * new_asset->dpvsDyn.dynEntClientWordCount[1]); // runtime
				new_asset->dpvsDyn.dynEntVisData[1][1] = allocator.allocate<unsigned char>(32 * new_asset->dpvsDyn.dynEntClientWordCount[1]); // runtime
				new_asset->dpvsDyn.dynEntVisData[1][2] = allocator.allocate<unsigned char>(32 * new_asset->dpvsDyn.dynEntClientWordCount[1]); // runtime
			}

			COPY_VALUE(mapVtxChecksum);
			COPY_VALUE(heroOnlyLightCount);
			REINTERPRET_CAST_SAFE(heroOnlyLights);

			// IW7 renders nothing at all without a tome here - see generate_umbra_tome.
			{
				auto* tome = generate_umbra_tome(allocator);

				new_asset->numUmbraGates = 0;
				new_asset->umbraGates = nullptr;
				new_asset->umbraTomeSize = tome->m_size;
				new_asset->umbraTomeData = reinterpret_cast<char*>(tome);

				// runtime pointer, filled in by Load_UmbraTome once the zone is streamed.
				new_asset->umbraTomePtr = nullptr;
			}

			// the second tome is the gate tome, and gates are a T7/IW7 authoring concept
			// with no IW5 equivalent. It is only consulted by the gate-state queries, not
			// by the static visibility path that decides what gets drawn, so a converted
			// map does not need one.
			new_asset->numUmbraGates2 = 0;
			new_asset->umbraGates2 = nullptr;
			new_asset->umbraTomeSize2 = 0;
			new_asset->umbraTomeData2 = nullptr;
			new_asset->umbraTomePtr2 = nullptr;

			// 4 bytes holding a float (2400.0 in shipped maps). Consumer not identified,
			// and shipped content is happy to have none of it.
			new_asset->umbraUnkSize = 0;
			new_asset->umbraUnkData = nullptr;

			{
				// re-calculate values
				auto AlignUp = [](auto value, auto alignment)
				{
					return (value + (alignment - 1)) & ~(alignment - 1);
				};

				const auto lights = new_asset->primaryLightCount
					- new_asset->lastSunPrimaryLightIndex
					- new_asset->movingScriptablePrimaryLightCount
					- 1;

				new_asset->staticSpotOmniPrimaryLightCountAligned = AlignUp(lights, 32);

				new_asset->primaryLightMotionDetectBitsEntries = new_asset->staticSpotOmniPrimaryLightCountAligned >> 4;;
				new_asset->primaryLightMotionDetectBits = allocator.allocate<unsigned int>(new_asset->primaryLightMotionDetectBitsEntries); // runtime

				new_asset->entityMotionBitsEntries = 134; // idk (seems to always be 134)
				new_asset->entityMotionBits = allocator.allocate<unsigned int>(new_asset->entityMotionBitsEntries); // runtime

				new_asset->numPrimaryLightEntityShadowVisEntries = new_asset->staticSpotOmniPrimaryLightCountAligned * 0x86;
				new_asset->primaryLightEntityShadowVis = allocator.allocate<unsigned int>(new_asset->numPrimaryLightEntityShadowVisEntries); // runtime

				new_asset->dynEntMotionBitsEntries[0] =
					((new_asset->dpvsDyn.dynEntClientCount[0] + 31) >> 5) * 2;
				new_asset->dynEntMotionBits[0] = allocator.allocate<unsigned int>(new_asset->dynEntMotionBitsEntries[0]); // runtime
				new_asset->dynEntMotionBitsEntries[1] =
					((new_asset->dpvsDyn.dynEntClientCount[1] + 31) >> 5) * 2;
				new_asset->dynEntMotionBits[1] = allocator.allocate<unsigned int>(new_asset->dynEntMotionBitsEntries[1]); // runtime

				new_asset->numPrimaryLightDynEntShadowVisEntries[0] =
					(new_asset->staticSpotOmniPrimaryLightCountAligned * new_asset->dpvsDyn.dynEntClientCount[0]) >> 4;
				new_asset->primaryLightDynEntShadowVis[0] = allocator.allocate<unsigned int>(new_asset->numPrimaryLightDynEntShadowVisEntries[0]); // runtime
				new_asset->numPrimaryLightDynEntShadowVisEntries[1] =
					(new_asset->staticSpotOmniPrimaryLightCountAligned * new_asset->dpvsDyn.dynEntClientCount[1]) >> 4;
				new_asset->primaryLightDynEntShadowVis[1] = allocator.allocate<unsigned int>(new_asset->numPrimaryLightDynEntShadowVisEntries[1]); // runtime
			}
			

			return new_asset;
		}

		IW7::GfxWorld* convert(GfxWorld* asset, allocator& allocator)
		{
			// generate IW7 gfxworld
			return GenerateIW7GfxWorld(asset, allocator);
		}
	}
}