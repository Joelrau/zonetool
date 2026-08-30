#include "stdafx.hpp"
#include "../Include.hpp"

#include "GfxWorld.hpp"

#include "X64/Utils/Utils.hpp"
#include "X64/Utils/LightGrid/LightGridSH.hpp"
#include "X64/Utils/LightGrid/LightGridProbes.hpp"
#include "X64/Utils/LightGrid/LightGridTree.hpp"
#include <unordered_map>

#include "ComWorld.hpp"

#include <set>

namespace ZoneTool::IW5
{
	namespace IW7Converter
	{
		// ---- primary light proxy hulls ------------------------------------------------------
		//
		// IW7 uses GfxWorld::frustumLights for exactly two things: R_IsCameraInsideLightMeshVolume
		// walks the vertices to get the light's view-space z extent, which is what places the light
		// in the z-binned frustum grid, and an optional per-face camera test behind
		// r_frustumLightProxyUseMeshCheck. Both only need a convex volume that *contains* the light,
		// so we circumscribe it rather than reproduce IW7's own tessellation - too large costs a few
		// extra bins, too small loses the light. A light with vertexCount 0 keeps the inverted range
		// (FLT_MAX, 0) that function starts from and drops out of the grid entirely.
		//
		// Shipped IW7 proxies give the conventions: vertex 0 is the light origin, the hull extends
		// along -dir (dir points toward the light), and only SPOT/OMNI carry one.
		namespace
		{
			constexpr float light_proxy_pi = 3.14159265358979f;
			constexpr unsigned int light_proxy_segments = 8;
			constexpr unsigned int light_proxy_spot_rings = 3;
			constexpr unsigned int light_proxy_omni_rings = 5;

			constexpr unsigned char light_type_spot = 2;
			constexpr unsigned char light_type_omni = 3;

			// 80 degrees
			constexpr float light_proxy_wide_spot_cutoff = 1.3962634f;

			struct proxy_mesh
			{
				std::vector<float> vertices; // xyz triplets
				std::vector<unsigned short> indices;

				unsigned short add_vertex(const float* p)
				{
					vertices.push_back(p[0]);
					vertices.push_back(p[1]);
					vertices.push_back(p[2]);
					return static_cast<unsigned short>((vertices.size() / 3) - 1);
				}

				void add_triangle(const unsigned short a, const unsigned short b, const unsigned short c)
				{
					indices.push_back(a);
					indices.push_back(b);
					indices.push_back(c);
				}
			};

			void normalize_proxy_axis(float v[3])
			{
				const auto len = std::sqrt((v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]));
				if (len > 0.0f)
				{
					v[0] /= len;
					v[1] /= len;
					v[2] /= len;
				}
			}

			// any orthonormal pair perpendicular to axis
			void build_proxy_basis(const float axis[3], float u[3], float v[3])
			{
				float helper[3] = { 0.0f, 0.0f, 1.0f };
				if (std::fabs(axis[2]) > 0.9f)
				{
					helper[0] = 1.0f;
					helper[2] = 0.0f;
				}

				u[0] = (helper[1] * axis[2]) - (helper[2] * axis[1]);
				u[1] = (helper[2] * axis[0]) - (helper[0] * axis[2]);
				u[2] = (helper[0] * axis[1]) - (helper[1] * axis[0]);
				normalize_proxy_axis(u);

				v[0] = (axis[1] * u[2]) - (axis[2] * u[1]);
				v[1] = (axis[2] * u[0]) - (axis[0] * u[2]);
				v[2] = (axis[0] * u[1]) - (axis[1] * u[0]);
				normalize_proxy_axis(v);
			}

			// every vertex sits on a sphere of radius `dist`, so a face of the hull sits at
			// dist * cos(half the angular gap between its vertices). Scaling the sample distance by
			// 1 / cos(gap) keeps every face outside the true light volume.
			float proxy_circumscribe_scale(const float polar_gap)
			{
				return 1.0f / (std::cos(light_proxy_pi / light_proxy_segments) * std::cos(polar_gap * 0.5f));
			}

			// The distance scale above only covers the spherical cap. It does nothing for the cone's
			// side faces, because pushing the ring further from the apex widens the hull without
			// widening its aperture - a rim point half way between two ring vertices still ends up
			// outside by 1 / cos(pi / segments). Widening the cone angle instead is what makes the
			// pyramid circumscribe the cone: a face at `expanded` has half-angle `half_angle` at its
			// mid-azimuth. Verified against the rim circle, which is the worst case.
			float expand_spot_cone(const float half_angle)
			{
				return std::atan(std::tan(half_angle) / std::cos(light_proxy_pi / light_proxy_segments));
			}

			void add_proxy_ring(proxy_mesh& mesh, const float origin[3], const float axis[3],
				const float u[3], const float v[3], const float theta, const float dist,
				std::vector<unsigned short>& out)
			{
				const auto sin_theta = std::sin(theta);
				const auto cos_theta = std::cos(theta);

				for (unsigned int s = 0; s < light_proxy_segments; s++)
				{
					const auto phi = (2.0f * light_proxy_pi * s) / light_proxy_segments;
					const auto cos_phi = std::cos(phi);
					const auto sin_phi = std::sin(phi);

					float p[3];
					for (int c = 0; c < 3; c++)
					{
						p[c] = origin[c] + (((axis[c] * cos_theta)
							+ (((u[c] * cos_phi) + (v[c] * sin_phi)) * sin_theta)) * dist);
					}
					out.push_back(mesh.add_vertex(p));
				}
			}

			void bridge_proxy_rings(proxy_mesh& mesh, const std::vector<unsigned short>& inner,
				const std::vector<unsigned short>& outer)
			{
				for (unsigned int s = 0; s < light_proxy_segments; s++)
				{
					const auto n = (s + 1) % light_proxy_segments;
					mesh.add_triangle(inner[s], outer[s], outer[n]);
					mesh.add_triangle(inner[s], outer[n], inner[n]);
				}
			}

			void cap_proxy_ring(proxy_mesh& mesh, const unsigned short pole,
				const std::vector<unsigned short>& ring, const bool flip)
			{
				for (unsigned int s = 0; s < light_proxy_segments; s++)
				{
					const auto n = (s + 1) % light_proxy_segments;
					if (flip)
					{
						mesh.add_triangle(pole, ring[n], ring[s]);
					}
					else
					{
						mesh.add_triangle(pole, ring[s], ring[n]);
					}
				}
			}

			// apex at the light origin, a tip on the axis and light_proxy_spot_rings rings out to the
			// outer cone angle - the same apex + axial + rings topology the shipped hulls use.
			void build_spot_proxy(proxy_mesh& mesh, const float origin[3], const float axis[3],
				const float half_angle, const float range)
			{
				float u[3], v[3];
				build_proxy_basis(axis, u, v);

				const auto ring_step = expand_spot_cone(half_angle) / light_proxy_spot_rings;
				const auto dist = range * proxy_circumscribe_scale(ring_step);

				float tip[3];
				for (int c = 0; c < 3; c++)
				{
					tip[c] = origin[c] + (axis[c] * dist);
				}

				const auto apex = mesh.add_vertex(origin);
				const auto axial = mesh.add_vertex(tip);

				std::vector<std::vector<unsigned short>> rings;
				for (unsigned int k = 1; k <= light_proxy_spot_rings; k++)
				{
					std::vector<unsigned short> ring;
					add_proxy_ring(mesh, origin, axis, u, v, ring_step * k, dist, ring);
					rings.push_back(ring);
				}

				cap_proxy_ring(mesh, axial, rings.front(), false);
				for (std::size_t k = 0; k + 1 < rings.size(); k++)
				{
					bridge_proxy_rings(mesh, rings[k], rings[k + 1]);
				}
				cap_proxy_ring(mesh, apex, rings.back(), true);
			}

			void build_omni_proxy(proxy_mesh& mesh, const float origin[3], const float range)
			{
				constexpr float axis[3] = { 0.0f, 0.0f, 1.0f };
				float u[3], v[3];
				build_proxy_basis(axis, u, v);

				const auto ring_step = light_proxy_pi / (light_proxy_omni_rings + 1);
				const auto dist = range * proxy_circumscribe_scale(ring_step);

				float pole[3];
				for (int c = 0; c < 3; c++)
				{
					pole[c] = origin[c] + (axis[c] * dist);
				}
				const auto north = mesh.add_vertex(pole);

				for (int c = 0; c < 3; c++)
				{
					pole[c] = origin[c] - (axis[c] * dist);
				}
				const auto south = mesh.add_vertex(pole);

				std::vector<std::vector<unsigned short>> rings;
				for (unsigned int k = 1; k <= light_proxy_omni_rings; k++)
				{
					std::vector<unsigned short> ring;
					add_proxy_ring(mesh, origin, axis, u, v, ring_step * k, dist, ring);
					rings.push_back(ring);
				}

				cap_proxy_ring(mesh, north, rings.front(), false);
				for (std::size_t k = 0; k + 1 < rings.size(); k++)
				{
					bridge_proxy_rings(mesh, rings[k], rings[k + 1]);
				}
				cap_proxy_ring(mesh, south, rings.back(), true);
			}
		}

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
					iw7_portal->vertices = reinterpret_cast<float(PTR64)[3]>(iw5_portal->vertices);
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
				// every authentic IW7 map ships these, probe-based ones included
				new_asset->lightGrid.tableVersion = 1;
				new_asset->lightGrid.paletteVersion = 1;
				new_asset->lightGrid.rangeExponent8BitsEncoding = 0;
				new_asset->lightGrid.rangeExponent12BitsEncoding = 4;
				new_asset->lightGrid.rangeExponent16BitsEncoding = 23;
				new_asset->lightGrid.stageCount = 0;
				new_asset->lightGrid.stageLightingContrastGain = 0;
				// IW7's own compiler never emits a real octree light grid - every authentic map
				// (mp_paris, mp_afghan, mp_breakneck, cp_zmb, mp_dome_dusk, mp_frontend) ships this
				// exact 3-entry palette and 2-node tree stub alongside a full probe volume. Emit it
				// verbatim rather than zeros, so anything that expects a light grid to exist finds one.
				static const int stub_palette_addresses[3] = { 0, 30, 86 };
				static const unsigned char stub_palette_bitstream[116] = {
					0xE7,0x1C,0x00,0xF8,0x08,0x80,0x80,0x80,0x80,0x80,0xF1,0x00,0x08,0x80,0xF8,0x80,
					0x80,0x80,0xB8,0x48,0x00,0x80,0xF8,0x08,0x80,0x80,0x80,0x48,0x48,0x00,0x00,0x00,
					0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x80,0x80,0x80,0x80,0x80,0x80,
					0x80,0x80,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x5C,0x5E,0x4A,0x3F,
					0xFF,0xFF,0x7F,0x7F,0xFF,0xFF,0x7F,0x7F,0xFF,0xFF,0x7F,0xFF,0xFF,0xFF,0x7F,0xFF,
					0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
					0x80,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x80,0x80,0x80,0x80,0x80,
					0x80,0x80,0x80,0x00,
				};

				new_asset->lightGrid.paletteEntryCount = 3;
				new_asset->lightGrid.paletteEntryAddress = allocator.allocate<int>(3);
				memcpy(new_asset->lightGrid.paletteEntryAddress, stub_palette_addresses,
					sizeof(stub_palette_addresses));
				new_asset->lightGrid.paletteBitstreamSize = sizeof(stub_palette_bitstream);
				new_asset->lightGrid.paletteBitstream =
					allocator.allocate<unsigned char>(sizeof(stub_palette_bitstream));
				memcpy(new_asset->lightGrid.paletteBitstream, stub_palette_bitstream,
					sizeof(stub_palette_bitstream));
				// IW5's GfxLightGridColors (unsigned char rgb[56][3]) and IW7's GfxLightGridColorsHDR
				// (float rgb[56][3]) are the same 56 directional bins, so these tables convert 1:1 -
				// only the storage changes, LDR bytes to linear floats. Palette entry 0 is the default
				// set and entry 1 the sky set, the same mapping the H1 converter uses. Anything the
				// source map does not provide stays zeroed.
				memset(&new_asset->lightGrid.skyLightGridColors, 0, sizeof(IW7::GfxLightGridColorsHDR));
				memset(&new_asset->lightGrid.defaultLightGridColors, 0, sizeof(IW7::GfxLightGridColorsHDR));

				if (asset->lightGrid.colors)
				{
					float hdr_colors[56][3];
					if (asset->lightGrid.colorCount > 0)
					{
						lightgrid_sh::ldr_colors_to_hdr(asset->lightGrid.colors[0].rgb, hdr_colors);
						memcpy(new_asset->lightGrid.defaultLightGridColors.rgb, hdr_colors, sizeof(hdr_colors));
					}
					if (asset->lightGrid.colorCount > 1)
					{
						lightgrid_sh::ldr_colors_to_hdr(asset->lightGrid.colors[1].rgb, hdr_colors);
						memcpy(new_asset->lightGrid.skyLightGridColors.rgb, hdr_colors, sizeof(hdr_colors));
					}
				}
				// the matching one-leaf tree stub, also verbatim from shipped maps
				static const unsigned int stub_node_table[2] = { 16777217u, 2147483648u };
				static const unsigned char stub_leaf_table[6] = { 0x01, 0x83, 0x00, 0x04, 0x06, 0x11 };

				new_asset->lightGrid.tree.maxDepth = 1;
				new_asset->lightGrid.tree.nodeCount = 2;
				new_asset->lightGrid.tree.leafCount = 1;
				new_asset->lightGrid.tree.coordMinGridSpace[0] = 4092;
				new_asset->lightGrid.tree.coordMinGridSpace[1] = 4092;
				new_asset->lightGrid.tree.coordMinGridSpace[2] = 2047;
				new_asset->lightGrid.tree.coordMaxGridSpace[0] = 4100;
				new_asset->lightGrid.tree.coordMaxGridSpace[1] = 4100;
				new_asset->lightGrid.tree.coordMaxGridSpace[2] = 2049;
				new_asset->lightGrid.tree.coordHalfSizeGridSpace[0] = 4;
				new_asset->lightGrid.tree.coordHalfSizeGridSpace[1] = 4;
				new_asset->lightGrid.tree.coordHalfSizeGridSpace[2] = 1;
				new_asset->lightGrid.tree.defaultColorIndexBitCount = 2;
				new_asset->lightGrid.tree.defaultLightIndexBitCount = 32;
				new_asset->lightGrid.tree.p_nodeTable = allocator.allocate<unsigned int>(2);
				memcpy(new_asset->lightGrid.tree.p_nodeTable, stub_node_table, sizeof(stub_node_table));
				new_asset->lightGrid.tree.leafTableSize = sizeof(stub_leaf_table);
				new_asset->lightGrid.tree.p_leafTable =
					allocator.allocate<unsigned char>(sizeof(stub_leaf_table));
				memcpy(new_asset->lightGrid.tree.p_leafTable, stub_leaf_table, sizeof(stub_leaf_table));

				memset(&new_asset->lightGrid.probeData, 0, sizeof(IW7::GfxLightGridProbeData));
				new_asset->lightGrid.probeData.zoneCount = 1;
				new_asset->lightGrid.probeData.zones = allocator.allocate<IW7::GfxGpuLightGridZone>(1);
				// probeData is IW7's only working static model lighting path: a GPU tetrahedral volume
				// of L2 SH probes, walked in the shader from a per-voxel seed tetrahedron. The
				// octree/palette light grid IW7 inherited from the IW6/H1 lineage is vestigial - every
				// authentic map ships a 3-entry stub palette and a 2-node stub tree - so without a real
				// volume here every XModel renders black, viewhands included.
				//
				// See X64/Utils/LightGrid/LightGridProbes.hpp for the format notes this builds against.
				float ambient[3] = { 0.0f, 0.0f, 0.0f };
				if (asset->lightGrid.colorCount && asset->lightGrid.colors)
				{
					// weight each palette entry by how many grid cells actually use it - averaging colors[]
					// flat would give a rarely referenced entry the same say as the dominant one.
					std::vector<double> usage(asset->lightGrid.colorCount, 0.0);
					double total_usage = 0.0;
					for (unsigned int i = 0; asset->lightGrid.entries && i < asset->lightGrid.entryCount; i++)
					{
						const auto colors_index = asset->lightGrid.entries[i].colorsIndex;
						if (colors_index < asset->lightGrid.colorCount)
						{
							usage[colors_index] += 1.0;
							total_usage += 1.0;
						}
					}

					if (total_usage == 0.0)
					{
						// octree-only grid (no legacy entries): fall back to a flat palette average
						std::fill(usage.begin(), usage.end(), 1.0);
						total_usage = static_cast<double>(asset->lightGrid.colorCount);
					}

					double accum[3] = { 0.0, 0.0, 0.0 };
					float hdr_colors[56][3];
					for (unsigned int i = 0; i < asset->lightGrid.colorCount; i++)
					{
						if (usage[i] == 0.0)
						{
							continue;
						}

						// the 56 directional bins are shared between IW5 and IW7, so the mean over the
						// sphere is the entry's ambient radiance.
						lightgrid_sh::ldr_colors_to_hdr(asset->lightGrid.colors[i].rgb, hdr_colors);
						double entry[3] = { 0.0, 0.0, 0.0 };
						for (unsigned int j = 0; j < 56; j++)
						{
							entry[0] += hdr_colors[j][0];
							entry[1] += hdr_colors[j][1];
							entry[2] += hdr_colors[j][2];
						}

						for (int c = 0; c < 3; c++)
						{
							accum[c] += (entry[c] / 56.0) * usage[i];
						}
					}

					for (int c = 0; c < 3; c++)
					{
						ambient[c] = static_cast<float>(accum[c] / total_usage);
					}
				}

				// Constant radiance projects onto the DC basis function scaled by 2*sqrt(pi) = 3.545,
				// but that lands roughly 9x below the only shipped reference: mp_frontend's probes are
				// DC (6.03, 7.82, 9.29) while the same derivation over mp_test_h1 gives (0.70, 0.74,
				// 0.84). Either IW7 evaluates SH on a different convention or the shipped maps are far
				// brighter, so this is calibrated empirically to land a typical map in the shipped
				// range. This is the one knob if converted maps read too dark or too bright.
				constexpr float sh_ambient_scale = 32.0f;

				lightgrid_probes::build_params probe_params{};
				for (int i = 0; i < 3; i++)
				{
					probe_params.bounds_min[i] = asset->bounds.midPoint[i] - asset->bounds.halfSize[i];
					probe_params.bounds_max[i] = asset->bounds.midPoint[i] + asset->bounds.halfSize[i];
				}

				// Per-probe lighting out of the IW5 grid. The legacy row data gives every populated
				// grid position and the entry it points at; each entry's colorsIndex selects one of
				// the 56-bin colour tables, whose mean over the sphere is that cell's ambient
				// radiance. Grid space is the usual legacy one: 32-unit cells in x/y, 64 in z,
				// biased by 131072 (4096 cells in x/y, 2048 in z).
				// one projected SH set per palette entry, keyed by grid cell
				using probe_sh = std::array<float, 27>;
				std::unordered_map<unsigned long long, probe_sh> grid_samples;
				{
					std::vector<probe_sh> colour_cache;
					std::vector<char> colour_cached;
					if (asset->lightGrid.colorCount)
					{
						colour_cache.resize(asset->lightGrid.colorCount, probe_sh{});
						colour_cached.resize(asset->lightGrid.colorCount, 0);
					}

					std::vector<lightgrid_tree::grid_entry_ref> refs;
					if (asset->lightGrid.rowDataStart && asset->lightGrid.rawRowData)
					{
						refs = lightgrid_tree::enumerate_row_data(
							asset->lightGrid.mins, asset->lightGrid.maxs,
							asset->lightGrid.rowAxis, asset->lightGrid.colAxis,
							asset->lightGrid.rowDataStart, asset->lightGrid.rawRowData);
					}

					float bins[56][3];
					for (const auto& ref : refs)
					{
						if (ref.entry_index >= asset->lightGrid.entryCount || !asset->lightGrid.entries)
						{
							continue;
						}
						const auto colors_index = asset->lightGrid.entries[ref.entry_index].colorsIndex;
						if (colors_index >= asset->lightGrid.colorCount || !asset->lightGrid.colors)
						{
							continue;
						}

						if (!colour_cached[colors_index])
						{
							// project the 56 directional bins onto IW7's SH basis rather than
							// averaging them away - the average is what made every model flat
							lightgrid_sh::ldr_colors_to_hdr(asset->lightGrid.colors[colors_index].rgb, bins);
							lightgrid_probes::project_sh(bins, lightgrid_sh::grid_basis_dirs, 56,
								sh_ambient_scale, colour_cache[colors_index].data());
							colour_cached[colors_index] = 1;
						}

						const auto key = (static_cast<unsigned long long>(ref.pos[0]) << 32)
							| (static_cast<unsigned long long>(ref.pos[1]) << 16)
							| static_cast<unsigned long long>(ref.pos[2]);
						grid_samples[key] = colour_cache[colors_index];
					}

					ZONETOOL_INFO("GfxWorld \"%s\": %zu populated light grid cells for probe sampling",
						asset->name, grid_samples.size());
				}

				const auto volume = lightgrid_probes::build(probe_params,
					[&](const float* pos, float* out_sh)
					{
						const auto gx = static_cast<int>(std::floor(pos[0] / 32.0f)) + 4096;
						const auto gy = static_cast<int>(std::floor(pos[1] / 32.0f)) + 4096;
						const auto gz = static_cast<int>(std::floor(pos[2] / 64.0f)) + 2048;

						// probes land on cell corners and plenty of cells are empty (walls, solid),
						// so widen the search until something is found rather than going black
						for (int radius = 0; radius <= 4; radius++)
						{
							const float* best = nullptr;
							int best_dist = 0x7FFFFFFF;
							for (int dz = -radius; dz <= radius; dz++)
							{
								for (int dy = -radius; dy <= radius; dy++)
								{
									for (int dx = -radius; dx <= radius; dx++)
									{
										if (std::max(std::max(std::abs(dx), std::abs(dy)), std::abs(dz)) != radius)
										{
											continue; // only the new shell
										}
										const auto x = gx + dx, y = gy + dy, z = gz + dz;
										if (x < 0 || y < 0 || z < 0 || x > 0xFFFF || y > 0xFFFF || z > 0xFFFF)
										{
											continue;
										}
										const auto key = (static_cast<unsigned long long>(x) << 32)
											| (static_cast<unsigned long long>(y) << 16)
											| static_cast<unsigned long long>(z);
										const auto it = grid_samples.find(key);
										if (it == grid_samples.end())
										{
											continue;
										}
										// weight z harder, matching how the legacy sampler treats
										// vertical distance
										const auto dist = dx * dx + dy * dy + 4 * dz * dz;
										if (dist < best_dist)
										{
											best_dist = dist;
											best = it->second.data();
										}
									}
								}
							}
							if (best)
							{
								memcpy(out_sh, best, sizeof(float) * 27);
								return;
							}
						}

						// nothing within reach - fall back to the map average, DC only
						lightgrid_probes::constant_sh(ambient, sh_ambient_scale, out_sh);
					}, sh_ambient_scale);

				auto& zone = *new_asset->lightGrid.probeData.zones;
				if (volume.valid)
				{
					auto& pd = new_asset->lightGrid.probeData;

					// gpuVisibleProbes is load-bearing: zeroing it on a stock map makes everything go
					// black (tested in game). It is a second, independently populated probe set - in
					// mp_dome_dusk its 21152 positions share nothing with the 44092 grid probes and sit
					// at irregular coordinates - and its data array is sized (count + 0x2000), so a zero
					// count also under-sizes the GPU buffer the runtime streams through. Mirror our grid
					// probes into it so the buffer is the right size and starts with sane contents.
					pd.gpuVisibleProbesCount = volume.probe_count;
					pd.gpuVisibleProbePositions =
						allocator.allocate<IW7::GfxGpuLightGridProbePosition>(volume.probe_count);
					memcpy(pd.gpuVisibleProbePositions, volume.probe_positions.data(),
						sizeof(float) * volume.probe_positions.size());

					// the trailing 0x2000 entries are scratch and are zero in every shipped map
					pd.gpuVisibleProbesData =
						allocator.allocate<IW7::GfxProbeData>(volume.probe_count + 0x2000);
					memcpy(pd.gpuVisibleProbesData, volume.probes.data(),
						sizeof(unsigned short) * volume.probes.size());

					pd.probeCount = volume.probe_count;
					pd.probes = allocator.allocate<IW7::GfxProbeData>(volume.probe_count);
					memcpy(pd.probes, volume.probes.data(), sizeof(unsigned short) * volume.probes.size());
					pd.probePositions = allocator.allocate<IW7::GfxGpuLightGridProbePosition>(volume.probe_count);
					memcpy(pd.probePositions, volume.probe_positions.data(),
						sizeof(float) * volume.probe_positions.size());

					pd.tetrahedronCount = volume.tetrahedron_count;
					pd.tetrahedrons = allocator.allocate<IW7::GfxGpuLightGridTetrahedron>(volume.tetrahedron_count);
					memcpy(pd.tetrahedrons, volume.tetrahedrons.data(),
						sizeof(unsigned int) * volume.tetrahedrons.size());
					pd.tetrahedronNeighbors =
						allocator.allocate<IW7::GfxGpuLightGridTetrahedronNeighbors>(volume.tetrahedron_count);
					memcpy(pd.tetrahedronNeighbors, volume.tetrahedron_neighbors.data(),
						sizeof(unsigned int) * volume.tetrahedron_neighbors.size());

					// Shipped maps carry a visibility entry for ~41% of their tetrahedra: 64 bytes each,
					// overwhelmingly 0xFF with occasional smaller values, so 0xFF reads as "fully
					// visible" whether the engine treats it as 512 bits or 64 byte weights. Leaving it
					// absent is what a walk that silently falls back to the zone probe looks like, so
					// emit a permissive entry for *every* tetrahedron - that stays valid whether the
					// table is indexed by tetrahedron index or by a compacted visible-only index.
					pd.tetrahedronCountVisible = volume.tetrahedron_count;
					pd.tetrahedronVisibility =
						allocator.allocate<IW7::GfxGpuLightGridTetrahedronVisibility>(volume.tetrahedron_count);
					memset(pd.tetrahedronVisibility, 0xFF,
						sizeof(IW7::GfxGpuLightGridTetrahedronVisibility) * volume.tetrahedron_count);

					pd.voxelStartTetrahedronCount = static_cast<unsigned int>(volume.voxel_start_tetrahedron.size());
					pd.voxelStartTetrahedron = allocator.allocate<IW7::GfxGpuLightGridVoxelStartTetrahedron>(
						pd.voxelStartTetrahedronCount);
					memcpy(pd.voxelStartTetrahedron, volume.voxel_start_tetrahedron.data(),
						sizeof(unsigned int) * volume.voxel_start_tetrahedron.size());

					zone.numProbes = volume.zone_num_probes;
					zone.firstProbe = volume.zone_first_probe;
					zone.numTetrahedrons = volume.zone_num_tetrahedrons;
					zone.firstTetrahedron = volume.zone_first_tetrahedron;
					zone.firstVoxelTetrahedronIndex = volume.zone_first_voxel_tetrahedron_index;
					zone.numVoxelTetrahedronIndices = volume.zone_num_voxel_tetrahedron_indices;

					// the probe volume is indexed by the voxel tree's leaves, so the tree has to be the
					// one the volume was built against - this replaces the per-sky stub built earlier
					new_asset->voxelTreeCount = 1;
					new_asset->voxelTree = allocator.allocate<IW7::GfxVoxelTree>(1);
					auto& tree = new_asset->voxelTree[0];
					memcpy(&tree.zoneBound, &asset->bounds, sizeof(Bounds));
					tree.voxelTopDownViewNodeCount = static_cast<int>(volume.top_down_view_nodes.size());
					tree.voxelInternalNodeCount = static_cast<int>(volume.internal_nodes.size());
					tree.voxelLeafNodeCount = static_cast<int>(volume.leaf_nodes.size());
					tree.lightListArraySize = static_cast<int>(volume.light_list.size());

					tree.voxelTreeHeader = allocator.allocate<IW7::GfxVoxelTreeHeader>();
					memcpy(tree.voxelTreeHeader->rootNodeDimension, volume.root_node_dimension, sizeof(int[4]));
					memcpy(tree.voxelTreeHeader->nodeCoordBitShift, volume.node_coord_bit_shift, sizeof(int[4]));
					memcpy(&tree.voxelTreeHeader->boundMin, volume.bound_min, sizeof(float[4]));
					memcpy(&tree.voxelTreeHeader->boundMax, volume.bound_max, sizeof(float[4]));

					tree.voxelTopDownViewNodeArray = allocator.allocate<IW7::GfxVoxelTopDownViewNode>(
						tree.voxelTopDownViewNodeCount);
					memcpy(tree.voxelTopDownViewNodeArray, volume.top_down_view_nodes.data(),
						sizeof(IW7::GfxVoxelTopDownViewNode) * tree.voxelTopDownViewNodeCount);
					tree.voxelInternalNodeArray = allocator.allocate<IW7::GfxVoxelInternalNode>(
						tree.voxelInternalNodeCount);
					memcpy(tree.voxelInternalNodeArray, volume.internal_nodes.data(),
						sizeof(IW7::GfxVoxelInternalNode) * tree.voxelInternalNodeCount);
					tree.voxelLeafNodeArray = allocator.allocate<IW7::GfxVoxelLeafNode>(tree.voxelLeafNodeCount);
					memcpy(tree.voxelLeafNodeArray, volume.leaf_nodes.data(),
						sizeof(unsigned short) * volume.leaf_nodes.size());
					tree.lightListArray = allocator.allocate<unsigned short>(tree.lightListArraySize);
					memcpy(tree.lightListArray, volume.light_list.data(),
						sizeof(unsigned short) * volume.light_list.size());
					tree.voxelInternalNodeDynamicLightList =
						allocator.allocate<unsigned int>(2 * tree.voxelInternalNodeCount); // runtime
				}

				// the zone fallback is used when a sample resolves to no tetrahedron; it is also all a
				// map gets if the volume could not be built
				memcpy(zone.fallbackProbeData.coeffs, volume.zone_fallback_coeffs,
					sizeof(zone.fallbackProbeData.coeffs));
				memset(zone.fallbackProbeData.pad, 0, sizeof(zone.fallbackProbeData.pad));
			}

			new_asset->frustumLights = allocator.allocate<IW7::GfxFrustumLights>(new_asset->primaryLightCount);

			// lightViewFrustums stays zeroed: both consumers (sub_140E1E2A0 / sub_140E1E510) early
			// out on planeCount == 0, so an absent frustum is a supported state, and they cull
			// against these planes - a guessed volume would silently drop shadow casters. The
			// shipped shapes are not a plain light frustum either (mp_dome_dusk light 7 is an
			// axis-aligned box that does not match its cone's AABB), so leave it off until the
			// volume is actually identified.
			new_asset->lightViewFrustums = allocator.allocate<IW7::GfxLightViewFrustum>(new_asset->primaryLightCount);

			// the light shapes live in the ComWorld, which is loaded alongside this GfxWorld and
			// shares its asset name
			{
				const auto com_world = converter_com_world;
				if (com_world)
				{
					const auto light_count = com_world
						? std::min<unsigned int>(new_asset->primaryLightCount, com_world->primaryLightCount)
						: 0u;

					for (unsigned int i = 0; i < light_count; i++)
					{
						const auto& light = com_world->primaryLights[i];
						if (light.radius <= 0.0f)
						{
							continue;
						}

						// dir points toward the light, so the volume runs the other way
						float axis[3] = { -light.dir[0], -light.dir[1], -light.dir[2] };
						normalize_proxy_axis(axis);

						proxy_mesh mesh{};
						const auto cos_outer = std::max(-1.0f, std::min(1.0f, light.cosHalfFovOuter));
						const auto half_angle = std::acos(cos_outer);

						// past ~80 degrees the expanded cone runs into tan(), and the spot is most of a
						// hemisphere anyway - the sphere hull contains it and stays well conditioned
						if (light.type == light_type_spot && half_angle < light_proxy_wide_spot_cutoff)
						{
							build_spot_proxy(mesh, light.origin, axis, half_angle, light.radius);
						}
						else if (light.type == light_type_spot || light.type == light_type_omni)
						{
							build_omni_proxy(mesh, light.origin, light.radius);
						}
						else
						{
							// NONE and DIR (the sun) carry no proxy in shipped maps
							continue;
						}

						auto& dest = new_asset->frustumLights[i];

						// 32 bytes per vertex, of which only the leading xyz is ever read
						dest.vertexCount = static_cast<unsigned int>(mesh.vertices.size() / 3);
						dest.vertices = allocator.allocate<char>(32 * dest.vertexCount);
						for (unsigned int v = 0; v < dest.vertexCount; v++)
						{
							memcpy(&dest.vertices[32 * v], &mesh.vertices[3ull * v], sizeof(float[3]));
						}

						dest.indexCount = static_cast<unsigned int>(mesh.indices.size());
						dest.indices = allocator.allocate<unsigned short>(dest.indexCount);
						memcpy(dest.indices, mesh.indices.data(), sizeof(unsigned short) * dest.indexCount);
					}
				}
			}

			// The probe volume builds its own voxel tree and is indexed by that tree's leaves, so
			// only fall back to the per-sky stub when no volume was generated.
			if (!new_asset->voxelTree)
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

			COPY_VALUE(sun.hasValidData);
			COPY_ASSET(sun.spriteMaterial);
			COPY_ASSET(sun.flareMaterial);
			COPY_VALUE(sun.spriteSize);
			COPY_VALUE(sun.flareMinSize);
			COPY_VALUE(sun.flareMinDot);
			COPY_VALUE(sun.flareMaxSize);
			COPY_VALUE(sun.flareMaxDot);
			COPY_VALUE(sun.flareMaxAlpha);
			COPY_VALUE(sun.flareFadeInTime);
			COPY_VALUE(sun.flareFadeOutTime);
			COPY_VALUE(sun.blindMinDot);
			COPY_VALUE(sun.blindMaxDot);
			COPY_VALUE(sun.blindMaxDarken);
			COPY_VALUE(sun.blindFadeInTime);
			COPY_VALUE(sun.blindFadeOutTime);
			COPY_VALUE(sun.glareMinDot);
			COPY_VALUE(sun.glareMaxDot);
			COPY_VALUE(sun.glareMaxLighten);
			COPY_VALUE(sun.glareFadeInTime);
			COPY_VALUE(sun.glareFadeOutTime);
			COPY_ARR(sun.sunFxPosition);

			COPY_ARR(outdoorLookupMatrix);
			COPY_ASSET(outdoorImage);
			new_asset->dustMaterial = nullptr;
			new_asset->materialLod0SizeThreshold = 0.5f;

			if (asset->shadowGeom)
			{
				new_asset->shadowGeomOptimized = allocator.allocate<IW7::GfxShadowGeometry>(new_asset->primaryLightCount);
				for (unsigned int i = 0; i < new_asset->primaryLightCount; i++)
				{
					// primary lights 0..lastSunPrimaryLightIndex are the sun lights, and IW7 keeps no caster
					// list for them - shadowGeomOptimized only feeds the spot shadow passes. Every shipped map
					// zeroes exactly that range (mp_bog 0-1, mp_dome_dusk 0-2, mp_frontend 0-20), and sun
					// casters are expressed through GfxSurface::flags / GfxStaticModelDrawInst::sunShadowFlags
					// instead. IW5 does populate its sun light entry, so drop it rather than copying it over.
					if (i <= new_asset->lastSunPrimaryLightIndex)
					{
						continue;
					}

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
				// IW7 rebuilds dpvs.surfaceCastsSunShadow and dpvs.surfaceCastsSunShadowOpt every
				// R_SortWorldSurfacesSetSurfaces out of GfxSurface::flags: bit 0 is "casts sun shadow" and
				// bits 3..7 are a per-sun-light mask picking which surfaceCastsSunShadowOpt row the surface
				// joins. Only 5 bits are available, which is why shipped maps cap dpvs.sunShadowOptCount at 5
				// (mp_frontend has 20 sun lights and still stores 5); otherwise it equals
				// lastSunPrimaryLightIndex exactly. GfxStaticModelDrawInst::sunShadowFlags is the same mask
				// for static models.
				const auto sun_light_count = std::min<unsigned int>(new_asset->lastSunPrimaryLightIndex, 5);
				const auto sun_light_mask = static_cast<unsigned char>((1 << sun_light_count) - 1);

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
				// umbra smodel object index -> smodel index: the object-ID decoder marks
				// smodelVisData at lodData[objIndex] for tag 0x10000000. The object index is 1-based,
				// which is what the trailing +1 entry is for - in every shipped IW7 map (mp_bog,
				// mp_dome_dusk, mp_shipment) lodData[0] is 0 and lodData[1..smodelCount] is a
				// permutation of 0..smodelCount-1. We keep the smodel order, so write that identity.
				// Only matters if a tome ever resolves objects; ours takes the draw-everything path.
				new_asset->dpvs.lodData = allocator.allocate<unsigned int>(new_asset->dpvs.smodelCount + 1);
				for (unsigned int i = 0; i < new_asset->dpvs.smodelCount; i++)
				{
					new_asset->dpvs.lodData[i + 1] = i;
				}
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
					new_asset->dpvs.surfaces[i].material = reinterpret_cast<IW7::Material PTR64>(asset->dpvs.surfaces[i].material);
					new_asset->dpvs.surfaces[i].lightmapIndex = asset->dpvs.surfaces[i].laf.fields.lightmapIndex;

					// bit 0 means the same thing in both engines - r_drawsurf.cpp tests laf.fields.flags & 1
					// before setting the surfaceCastsSunShadow bit in IW5 and in IW7 alike - and it is the only
					// bit IW5 ever sets. The remaining IW5 bits would be read as IW7's sun light mask, so mask
					// them off and enrol every caster in all of the sun light sets.
					const auto casts_sun_shadow = (asset->dpvs.surfaces[i].laf.fields.flags & 1) != 0;
					new_asset->dpvs.surfaces[i].flags = casts_sun_shadow
						? static_cast<unsigned char>(1 | (sun_light_mask << 3))
						: 0;

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
					COPY_ARR(dpvs.smodelDrawInsts[i].placement);

					new_asset->dpvs.smodelDrawInsts[i].model =
						reinterpret_cast<IW7::XModel PTR64>(asset->dpvs.smodelDrawInsts[i].model);

					auto& src_draw_inst = asset->dpvs.smodelDrawInsts[i];

					new_asset->dpvs.smodelDrawInsts[i].modelLightmapInfo.lightmapIndex = -1;

					new_asset->dpvs.smodelDrawInsts[i].lightingHandle = asset->dpvs.smodelDrawInsts[i].lightingHandle;
					new_asset->dpvs.smodelDrawInsts[i].cullDist = asset->dpvs.smodelDrawInsts[i].cullDist;
					new_asset->dpvs.smodelDrawInsts[i].flags = asset->dpvs.smodelDrawInsts[i].flags;
					new_asset->dpvs.smodelDrawInsts[i].primaryLightEnvIndex = asset->dpvs.smodelDrawInsts[i].primaryLightIndex;
					new_asset->dpvs.smodelDrawInsts[i].reflectionProbeIndex = asset->dpvs.smodelDrawInsts[i].reflectionProbeIndex;
					new_asset->dpvs.smodelDrawInsts[i].firstMtlSkinIndex = asset->dpvs.smodelDrawInsts[i].firstMtlSkinIndex;
					// which sun lights this model casts for; a model with no bit set is skipped outright by
					// R_AddAllStaticModelSurfacesRangeSunShadow once the opt path is live, so enrol every model.
					new_asset->dpvs.smodelDrawInsts[i].sunShadowFlags = sun_light_mask;
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

				// old smodel index -> index after the map compiler's static model sort, streamed as
				// 2 * smodelCount bytes. IW7 never reads it (the only code touching dpvs+0x370 is
				// Load/Preload_GfxWorldDpvsStatic; the umbra path remaps smodels through lodData
				// instead) and shipped IW7 zones such as mp_bog and mp_shipment carry it fully zeroed.
				// We do not reorder anything, so write the identity map.
				new_asset->dpvs.sortedSmodelIndices = allocator.allocate<unsigned short>(asset->dpvs.smodelCount);
				for (unsigned int i = 0; i < asset->dpvs.smodelCount; i++)
				{
					new_asset->dpvs.sortedSmodelIndices[i] = static_cast<unsigned short>(i);
				}

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