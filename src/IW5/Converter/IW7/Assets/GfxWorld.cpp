#include "stdafx.hpp"
#include "../Include.hpp"

#include "GfxWorld.hpp"
#include "GfxImage.hpp"

#include "X64/Utils/Utils.hpp"
#include "X64/Utils/LightGrid/LightGridSH.hpp"
#include "X64/Utils/LightGrid/LightGridProbes.hpp"
#include "X64/Utils/LightGrid/LightGridTree.hpp"
#include <unordered_map>
#include <unordered_set>

#include "ComWorld.hpp"

#include <set>
#include <map>

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
			// Light grid sample points emitted per static model. IW7 gives every smodel a slice of
			// gfxWorld.lightGrid.probeData.gpuVisibleProbePositions and the engine resolves the
			// tetrahedral volume at each of those positions; unk2 = 0 is the shipped layout for a
			// two-point slice and covers 2444 of mp_dome_dusk's 3157 models.
			constexpr unsigned int smodel_probe_samples = 2;

			// Whether a light grid cell is lit by the sun rather than a local/indoor light. Same rule
			// as the H1 converter's is_sun_light: IW3-sourced maps put the sun in the low index range
			// [1..lastSun], everything else uses the high range.
			bool is_sun_light(const unsigned int pli, const unsigned int last_sun)
			{
				const bool low_range_is_sun = ZoneTool::get_linker_mode() == ZoneTool::linker_mode::iw3;
				return (low_range_is_sun && last_sun != 0 && pli >= 1 && pli <= last_sun)
					|| pli >= 256 - last_sun;
			}

			constexpr float light_proxy_pi = 3.14159265358979f;
			constexpr unsigned int light_proxy_segments = 8;
			constexpr unsigned int light_proxy_spot_rings = 3;
			constexpr unsigned int light_proxy_omni_rings = 5;

			constexpr unsigned char light_type_dir = 1;
			constexpr unsigned char light_type_spot = 2;
			constexpr unsigned char light_type_omni = 3;

			// Sun visibility bake (GfxProbeData coeffs[27]). Shipped maps carry a continuous
			// occlusion term - 10611 distinct values in mp_dome_dusk, ~30% of probes at exactly 0
			// and 5-11% at exactly 1 - so a classifier over primaryLightIndex can never reproduce
			// it. There is no ray casting here, but the legacy light grid is itself an occupancy
			// volume: cells inside solid geometry are simply never populated. Marching toward the
			// sun through that occupancy gives real, spatially varying shadowing from world geometry.
			constexpr unsigned int sun_trace_rays = 8;      // >1 so the terminator is soft
			constexpr float sun_trace_cone = 0.09f;         // ~5 degrees of jitter around the sun
			constexpr unsigned int sun_trace_max_steps = 384;

			// 80 degrees
			constexpr float light_proxy_wide_spot_cutoff = 1.3962634f;

			// A minimal circumscribing hull only needs ~1.09 * radius, but shipped hulls are far more
			// generous - measured over mp_dome_dusk, the 20-vertex unshadowed spots reach 1.26 R on
			// average (max 1.355) and the omni 1.369 R; mp_breakneck's omni is 1.415 R. This is a
			// *culling* volume feeding the clustered light bins, so a tight hull is the wrong trade:
			// clusters just outside it never receive the light and it cuts off on cluster boundaries,
			// which reads in game as the light stopping abruptly with blocky edges. Match the shipped
			// margin instead of the geometric minimum.
			constexpr float light_proxy_safety_margin = 1.20f;

			// Blunt fallback: ignore the cone entirely and wrap every light in a generous sphere.
			// The proxy is only a culling volume - the shader still applies the real cone falloff -
			// so an oversized hull cannot make a light spill, it can only stop it being culled out
			// of clusters near the edge of its reach. Use this to answer "is the proxy the thing
			// limiting where this light shows up"; if a big sphere fixes coverage, it is. The cost
			// is that more clusters process the light, and IW7 caps a cluster at
			// FRUSTUM_GRID_MAX_LIGHTS (256), so a dense map wants the fitted cone instead.
			constexpr bool light_proxy_use_sphere = false;
			constexpr float light_proxy_sphere_scale = 2.0f;

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

				// Emitted with the winding REVERSED, so the finished hull has inward-facing normals.
				// Every shipped proxy is wound that way - measured over mp_dome_dusk, all five hulls
				// checked (both the 20-vertex and 44-vertex kinds) have negative signed volume and
				// 0% of triangles facing outward, where ours were 100% outward. These meshes are
				// rasterised as light volumes rather than only used for culling, so the winding
				// decides which faces survive backface culling: inverted, the light renders over a
				// sliver of its real area, and enlarging the hull until the camera sits inside it
				// removes the light entirely.
				void add_triangle(const unsigned short a, const unsigned short b, const unsigned short c)
				{
					indices.push_back(a);
					indices.push_back(c);
					indices.push_back(b);
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
				const auto dist = range * proxy_circumscribe_scale(ring_step) * light_proxy_safety_margin;

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
				const auto dist = range * proxy_circumscribe_scale(ring_step) * light_proxy_safety_margin;

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

		unsigned int first_reflection_probe(unsigned int reflection_probe_count)
		{
			// Only when there is something left after dropping it: a world carrying nothing but
			// the sentinel keeps it, since IW7 still wants an array with at least one element.
			return reflection_probe_count > 1 ? 1 : 0;
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

			// Only the name: the pixels are assembled by the dumper, which has the converted
			// probes to hand. See GenerateReflectionProbeArray.
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

			// Drop IW5's invalid-probe sentinel - see first_reflection_probe. Nothing needs
			// re-indexing to follow it: every reflectionProbeIndex this converter writes is
			// already 0 (static models are forced to 0 below, world surfaceMaterials are
			// memset), so removing slot 0 repoints them all at the first real probe.
			const auto firstProbe = first_reflection_probe(asset->draw.reflectionProbeCount);
			const auto realProbeCount = asset->draw.reflectionProbeCount - firstProbe;

			new_asset->draw.reflectionProbeData.reflectionProbeCount = realProbeCount;
			new_asset->draw.reflectionProbeData.sharedReflectionProbeCount = 0;
			new_asset->draw.reflectionProbeData.reflectionProbes = allocator.allocate<IW7::GfxReflectionProbe>(realProbeCount);
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

				const unsigned int probeCount = realProbeCount;
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
					auto& srcProbe = asset->draw.reflectionProbeOrigins[firstProbe + i];
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

			new_asset->draw.iesLookupTexture = GenerateIesLookup(allocator);

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

			// Where each primary light is actually allowed to land, straight out of the source
			// bake. Every legacy grid cell names the one primary light that lights models in
			// it, and the IW3 compiler computed that with real visibility - so the union of a
			// light's cells is its true reach, walls already accounted for, at no cost to us.
			// Measured on mp_test_h1: the omni's box is (256 -288 64)..(480 -128 192) where an
			// unclipped sphere of its radius would span (232 -424 -40)..(552 -104 280), and the
			// spot through the wall falls outside the tight box on two axes.
			//
			// Indices are the ComWorld primaryLights indices - confirmed by index 1 being the
			// sun and covering the whole map while 2 and 3 sit on the spot's and omni's own
			// origins.
			struct light_cell_box
			{
				unsigned int cells = 0;
				float lo[3] = { 1e30f, 1e30f, 1e30f };
				float hi[3] = { -1e30f, -1e30f, -1e30f };
			};
			std::map<unsigned int, light_cell_box> light_boxes;

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
				// These two tables are engine constants in IW7, not per-map data. All five authentic
				// maps (mp_frontend, mp_dome_dusk, mp_paris, cp_zmb, mp_breakneck) ship them
				// byte-identical: skyLightGridColors all zero, and defaultLightGridColors the single
				// row (0, 0, 0.21875) repeated for all 56 bins.
				//
				// An earlier version converted them 1:1 from IW5's colors[0]/colors[1] the way the H1
				// converter does, which is wrong here: it wrote a non-zero sky table where every
				// shipped map writes zeros, and a default table 3.5x too bright with no zeros in it
				// (mean 0.2584 against the shipped 0.0729). That is a constant ambient floor added to
				// everything in the map, and it is what made converted maps read washed out even once
				// the probe volume itself was correctly calibrated.
				memset(&new_asset->lightGrid.skyLightGridColors, 0, sizeof(IW7::GfxLightGridColorsHDR));
				for (int i = 0; i < 56; i++)
				{
					new_asset->lightGrid.defaultLightGridColors.rgb[i][0] = 0.0f;
					new_asset->lightGrid.defaultLightGridColors.rgb[i][1] = 0.0f;
					new_asset->lightGrid.defaultLightGridColors.rgb[i][2] = 0.21875f;
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

				// Calibration against the four stock maps, over probes actually referenced by
				// tetrahedra (luminance p5/median/p95): mp_frontend 0.85/1.44/7.55, mp_dome_dusk
				// 0.00/0.84/3.65, mp_paris 0.02/0.68/12.93, mp_breakneck 0.00/2.79/2.79. project_sh
				// already carries the 2*sqrt(pi) = 3.5449 projection factor, so a scale of 1.0 is the
				// physically correct value - and it puts mp_test_h1's median at 0.74, right in that
				// range. Earlier builds used 32 and then 9, calibrated against mp_frontend's mean DC
				// alone; that map is an outlier bright lobby whose probes are all identical to its zone
				// fallback, and matching it made every converted map ~9x too bright with no dark areas
				// at all (p5 3.07 against 0.00-0.85 for stock). This is the knob if a map reads too
				// dark or too bright.
				constexpr float sh_ambient_scale = 1.0f;

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
				// one projected SH set per palette entry, keyed by grid cell, plus the cell's sun
				// visibility at [27]
				using probe_sh = std::array<float, 28>;
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

						// coeffs[27] is the probe's sun visibility, and it is per cell rather than per
						// palette entry, so it cannot live in the colour cache. The source only tells
						// us which primary light lit the cell, so this is binary where shipped maps
						// carry a continuous term - still far better than the constant 1.0 we used to
						// write, which claimed every probe in the map sees the full sun.
						auto cell = colour_cache[colors_index];
						cell[27] = is_sun_light(asset->lightGrid.entries[ref.entry_index].primaryLightIndex,
							asset->lastSunPrimaryLightIndex) ? 1.0f : 0.0f;
						grid_samples[key] = cell;
					}

					ZONETOOL_INFO("GfxWorld \"%s\": %zu populated light grid cells for probe sampling",
						asset->name, grid_samples.size());

					for (const auto& ref : refs)
					{
						if (ref.entry_index >= asset->lightGrid.entryCount
							|| !asset->lightGrid.entries)
						{
							continue;
						}
						const auto pli = static_cast<unsigned int>(
							asset->lightGrid.entries[ref.entry_index].primaryLightIndex);

						// legacy grid space: 32 units in x/y biased by 4096 cells, 64 in z
						// biased by 2048
						const float world[3] = {
							(static_cast<float>(ref.pos[0]) - 4096.0f) * 32.0f,
							(static_cast<float>(ref.pos[1]) - 4096.0f) * 32.0f,
							(static_cast<float>(ref.pos[2]) - 2048.0f) * 64.0f,
						};

						auto& box = light_boxes[pli];
						box.cells++;
						for (int k = 0; k < 3; k++)
						{
							box.lo[k] = std::min(box.lo[k], world[k]);
							box.hi[k] = std::max(box.hi[k], world[k]);
						}
					}

					for (const auto& kv : light_boxes)
					{
						const auto& b = kv.second;
						ZONETOOL_INFO("GfxWorld \"%s\": lightgrid primaryLightIndex %u -> %u "
							"cells, box (%.0f %.0f %.0f)..(%.0f %.0f %.0f)", asset->name,
							kv.first, b.cells, b.lo[0], b.lo[1], b.lo[2], b.hi[0], b.hi[1], b.hi[2]);
					}

				// ---- sun visibility, traced through the grid's own occupancy ----------------
				//
				// A cell the legacy grid never populated is either inside solid geometry or outside
				// the authored volume. Marching from each populated cell toward the sun and stopping
				// at the first unpopulated cell therefore reproduces world-geometry shadowing, while
				// a ray that leaves the populated bounds counts as open sky. Several jittered rays
				// per cell make the terminator a gradient rather than a hard edge, which is what the
				// continuous shipped term looks like.
				//
				// Limitation worth knowing: this only sees world geometry. Static models are not in
				// the legacy grid, so a probe under a car or a crate still reads as lit.
				if (!grid_samples.empty())
				{
					float sun_dir[3] = { 0.0f, 0.0f, 1.0f };
					bool have_sun = false;
					if (const auto sun_world = converter_com_world)
					{
						for (unsigned int i = 0; i < sun_world->primaryLightCount; i++)
						{
							const auto& sun_light = sun_world->primaryLights[i];
							if (sun_light.type != light_type_dir)
							{
								continue;
							}
							const auto len = std::sqrt((sun_light.dir[0] * sun_light.dir[0])
								+ (sun_light.dir[1] * sun_light.dir[1])
								+ (sun_light.dir[2] * sun_light.dir[2]));
							if (len <= 0.0f)
							{
								continue;
							}
							// dir points toward the sun, which is the way the ray has to travel
							sun_dir[0] = sun_light.dir[0] / len;
							sun_dir[1] = sun_light.dir[1] / len;
							sun_dir[2] = sun_light.dir[2] / len;
							have_sun = true;
							break;
						}
					}

					if (have_sun)
					{
						int lo[3] = { 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF };
						int hi[3] = { -0x7FFFFFFF, -0x7FFFFFFF, -0x7FFFFFFF };
						for (const auto& entry : grid_samples)
						{
							const int cell[3] = {
								static_cast<int>((entry.first >> 32) & 0xFFFF),
								static_cast<int>((entry.first >> 16) & 0xFFFF),
								static_cast<int>(entry.first & 0xFFFF),
							};
							for (int k = 0; k < 3; k++)
							{
								lo[k] = std::min(lo[k], cell[k]);
								hi[k] = std::max(hi[k], cell[k]);
							}
						}

						// an orthonormal pair perpendicular to the sun, for the jitter cone
						float jitter_u[3], jitter_v[3];
						{
							float helper[3] = { 0.0f, 0.0f, 1.0f };
							if (std::fabs(sun_dir[2]) > 0.9f)
							{
								helper[0] = 1.0f;
								helper[2] = 0.0f;
							}
							jitter_u[0] = (helper[1] * sun_dir[2]) - (helper[2] * sun_dir[1]);
							jitter_u[1] = (helper[2] * sun_dir[0]) - (helper[0] * sun_dir[2]);
							jitter_u[2] = (helper[0] * sun_dir[1]) - (helper[1] * sun_dir[0]);
							normalize_proxy_axis(jitter_u);

							jitter_v[0] = (sun_dir[1] * jitter_u[2]) - (sun_dir[2] * jitter_u[1]);
							jitter_v[1] = (sun_dir[2] * jitter_u[0]) - (sun_dir[0] * jitter_u[2]);
							jitter_v[2] = (sun_dir[0] * jitter_u[1]) - (sun_dir[1] * jitter_u[0]);
							normalize_proxy_axis(jitter_v);
						}

						// Static models as sun occluders.
						//
						// The march above can only stop on a cell the legacy grid never populated, so it
						// sees world geometry and nothing else. Measured on the shipped output, that left
						// coeffs[27] at a SINGLE distinct value - 1.0 for all 57,425 probes - against
						// 10,611-14,112 distinct values in every authentic map, 28-38% of whose probes sit
						// at exactly 0. The term carried no information: every probe claimed a completely
						// unoccluded view of the sun, including probes standing inside a structure built
						// out of static models.
						//
						// Rasterise each static model's world-space bounds into the same grid space and
						// treat those cells as blockers. It is an OBB-to-AABB approximation, so a thin or
						// hollow model over-occludes a little; far closer to the truth than ignoring
						// models entirely.
						std::unordered_set<unsigned long long> model_blocked;
						{
							size_t marked = 0;
							for (unsigned int i = 0; i < asset->dpvs.smodelCount; i++)
							{
								const auto& inst = asset->dpvs.smodelDrawInsts[i];
								if (!inst.model)
								{
									continue;
								}

								const auto* mid = inst.model->bounds.midPoint;
								const auto* half = inst.model->bounds.halfSize;

								float centre[3], extent[3];
								for (int r = 0; r < 3; r++)
								{
									centre[r] = inst.placement.origin[r] + inst.placement.scale
										* ((inst.placement.axis[0][r] * mid[0])
											+ (inst.placement.axis[1][r] * mid[1])
											+ (inst.placement.axis[2][r] * mid[2]));
									extent[r] = inst.placement.scale
										* ((std::fabs(inst.placement.axis[0][r]) * half[0])
											+ (std::fabs(inst.placement.axis[1][r]) * half[1])
											+ (std::fabs(inst.placement.axis[2][r]) * half[2]));
								}

								const int cell_lo[3] = {
									static_cast<int>(std::floor((centre[0] - extent[0]) / 32.0f)) + 4096,
									static_cast<int>(std::floor((centre[1] - extent[1]) / 32.0f)) + 4096,
									static_cast<int>(std::floor((centre[2] - extent[2]) / 64.0f)) + 2048,
								};
								const int cell_hi[3] = {
									static_cast<int>(std::floor((centre[0] + extent[0]) / 32.0f)) + 4096,
									static_cast<int>(std::floor((centre[1] + extent[1]) / 32.0f)) + 4096,
									static_cast<int>(std::floor((centre[2] + extent[2]) / 64.0f)) + 2048,
								};

								// one absurd model must not be able to blanket the whole map
								const unsigned long long span =
									static_cast<unsigned long long>(cell_hi[0] - cell_lo[0] + 1)
									* static_cast<unsigned long long>(cell_hi[1] - cell_lo[1] + 1)
									* static_cast<unsigned long long>(cell_hi[2] - cell_lo[2] + 1);
								if (span > 200000ull)
								{
									continue;
								}

								for (int z = cell_lo[2]; z <= cell_hi[2]; z++)
								{
									if (z < 0 || z > 0xFFFF) continue;
									for (int y = cell_lo[1]; y <= cell_hi[1]; y++)
									{
										if (y < 0 || y > 0xFFFF) continue;
										for (int x = cell_lo[0]; x <= cell_hi[0]; x++)
										{
											if (x < 0 || x > 0xFFFF) continue;
											model_blocked.insert(
												(static_cast<unsigned long long>(x) << 32)
												| (static_cast<unsigned long long>(y) << 16)
												| static_cast<unsigned long long>(z));
											marked++;
										}
									}
								}
							}
							ZONETOOL_INFO("GfxWorld \"%s\": %u static models occlude %zu grid cells for the "
								"sun visibility bake", asset->name, asset->dpvs.smodelCount, marked);
						}

						unsigned int fully_lit = 0;
						unsigned int fully_dark = 0;

						for (auto& entry : grid_samples)
						{
							const float start[3] = {
								static_cast<float>((entry.first >> 32) & 0xFFFF),
								static_cast<float>((entry.first >> 16) & 0xFFFF),
								static_cast<float>(entry.first & 0xFFFF),
							};

							unsigned int open_rays = 0;
							for (unsigned int r = 0; r < sun_trace_rays; r++)
							{
								const auto phi = (2.0f * light_proxy_pi * r) / sun_trace_rays;
								// ray 0 is the sun itself, the rest ring it
								const auto spread = (r == 0) ? 0.0f : sun_trace_cone;

								float dir[3];
								for (int k = 0; k < 3; k++)
								{
									dir[k] = sun_dir[k] + (((jitter_u[k] * std::cos(phi))
										+ (jitter_v[k] * std::sin(phi))) * spread);
								}
								normalize_proxy_axis(dir);

								// grid cells are 32 units in x/y and 64 in z, so a world direction has
								// to be rescaled before it can be walked in cell space
								float step[3] = { dir[0] / 32.0f, dir[1] / 32.0f, dir[2] / 64.0f };
								const auto longest = std::max(std::fabs(step[0]),
									std::max(std::fabs(step[1]), std::fabs(step[2])));
								if (longest <= 0.0f)
								{
									open_rays++;
									continue;
								}
								for (int k = 0; k < 3; k++)
								{
									step[k] /= (longest * 2.0f); // half a cell per step
								}

								float pos[3] = { start[0], start[1], start[2] };
								bool escaped = false;
								for (unsigned int walked = 0; walked < sun_trace_max_steps; walked++)
								{
									for (int k = 0; k < 3; k++)
									{
										pos[k] += step[k];
									}

									const int cell[3] = {
										static_cast<int>(std::lround(pos[0])),
										static_cast<int>(std::lround(pos[1])),
										static_cast<int>(std::lround(pos[2])),
									};
									if (cell[0] < lo[0] || cell[0] > hi[0]
										|| cell[1] < lo[1] || cell[1] > hi[1]
										|| cell[2] < lo[2] || cell[2] > hi[2])
									{
										escaped = true; // left the authored volume - open sky
										break;
									}

									const auto key = (static_cast<unsigned long long>(cell[0]) << 32)
										| (static_cast<unsigned long long>(cell[1]) << 16)
										| static_cast<unsigned long long>(cell[2]);
									if (grid_samples.find(key) == grid_samples.end())
									{
										break; // unpopulated cell - solid world geometry
									}
									if (model_blocked.find(key) != model_blocked.end())
									{
										break; // inside a static model's bounds
									}
								}

								if (escaped)
								{
									open_rays++;
								}
							}

							const auto visibility = static_cast<float>(open_rays)
								/ static_cast<float>(sun_trace_rays);
							entry.second[27] = visibility;

							if (visibility >= 1.0f)
							{
								fully_lit++;
							}
							else if (visibility <= 0.0f)
							{
								fully_dark++;
							}
						}

						ZONETOOL_INFO("GfxWorld \"%s\": sun visibility traced over %zu cells "
							"(%.1f%% fully lit, %.1f%% fully shadowed), sun dir (%.2f, %.2f, %.2f)",
							asset->name, grid_samples.size(),
							(100.0f * fully_lit) / grid_samples.size(),
							(100.0f * fully_dark) / grid_samples.size(),
							sun_dir[0], sun_dir[1], sun_dir[2]);
					}
				}

				// The per-cell sun flag above is binary, but shipped maps carry a continuous
					// visibility term (10611 distinct values in mp_dome_dusk, 14112 in mp_paris). A
					// hard 0/1 field snaps between adjacent probes and shows up as blown-white and
					// hard-black patches across a single model. Two 3x3x3 box passes over the
					// populated cells turn it into the soft gradient a real occlusion bake produces.
					for (int pass = 0; pass < 2; pass++)
					{
						std::unordered_map<unsigned long long, float> smoothed;
						smoothed.reserve(grid_samples.size());

						for (const auto& entry : grid_samples)
						{
							const auto cx = static_cast<int>((entry.first >> 32) & 0xFFFF);
							const auto cy = static_cast<int>((entry.first >> 16) & 0xFFFF);
							const auto cz = static_cast<int>(entry.first & 0xFFFF);

							float sum = 0.0f;
							int seen = 0;
							for (int dz = -1; dz <= 1; dz++)
							{
								for (int dy = -1; dy <= 1; dy++)
								{
									for (int dx = -1; dx <= 1; dx++)
									{
										const auto x = cx + dx, y = cy + dy, z = cz + dz;
										if (x < 0 || y < 0 || z < 0 || x > 0xFFFF || y > 0xFFFF || z > 0xFFFF)
										{
											continue;
										}
										const auto key = (static_cast<unsigned long long>(x) << 32)
											| (static_cast<unsigned long long>(y) << 16)
											| static_cast<unsigned long long>(z);
										const auto it = grid_samples.find(key);
										if (it != grid_samples.end())
										{
											sum += it->second[27];
											seen++;
										}
									}
								}
							}
							smoothed[entry.first] = seen ? (sum / static_cast<float>(seen)) : entry.second[27];
						}

						for (auto& entry : grid_samples)
						{
							entry.second[27] = smoothed[entry.first];
						}
					}
				}

				// Diagnostic, off by default. Bakes a position ramp into every probe instead of real
				// lighting: red rises with +x, green with +y, blue with +z across the world bounds.
				// Convert a map with this on and walk around - if the models change colour the
				// tetrahedral walk resolves and any remaining flatness is a sampling problem; if they
				// stay one flat colour the walk never resolves and the runtime is falling back to
				// zones[0].fallbackProbeData, which is what a uniformly lit map means.
				constexpr bool debug_position_ramp = false;

				// Diagnostics: how far the shell search has to reach, and how much range the source
				// palette actually has. Our probes span only 0.46x..1.26x of their median where stock
				// maps span 0..4x, and the two candidate explanations - a narrow source palette, or
				// the shell search smearing lit values over dark cells - need different fixes.
				unsigned int resolve_radius_hits[6] = {};

				const auto sample_sh = [&](const float* pos, float* out_sh)
					{
						if (debug_position_ramp)
						{
							float ramp[3];
							for (int c = 0; c < 3; c++)
							{
								const auto span = probe_params.bounds_max[c] - probe_params.bounds_min[c];
								const auto t = span > 0.0f ? (pos[c] - probe_params.bounds_min[c]) / span : 0.0f;
								// keep the ramp in the same brightness range as real probes
								ramp[c] = std::min(std::max(t, 0.0f), 1.0f) * 0.25f;
							}
							lightgrid_probes::constant_sh(ramp, sh_ambient_scale, out_sh);
							out_sh[27] = 1.0f;
							return;
						}

						const auto gx = static_cast<int>(std::floor(pos[0] / 32.0f)) + 4096;
						const auto gy = static_cast<int>(std::floor(pos[1] / 32.0f)) + 4096;
						const auto gz = static_cast<int>(std::floor(pos[2] / 64.0f)) + 2048;

						// Probes land on cell corners and plenty of cells are empty (walls, solid),
						// so widen the search rather than going black - but only so far. At the
						// old limit of 4 a probe could take its lighting from 128 units away in
						// x/y, which is straight through any wall in the map, and 28% of probes
						// were resolving off-cell (11883 at radius 1, 4250 at radius 2). That is a
						// light-through-wall mechanism in its own right: the omni on mp_test_h1
						// lights cells from y=-288 while the viewmodel at y=-347 is exactly two
						// cells outside, well inside the old reach.
						//
						// One cell is what a corner probe legitimately needs, since its own cell
						// centre may be unpopulated while the cell it borders is not. Anything
						// beyond that is a guess about a region the bake deliberately left empty.
						constexpr int probe_resolve_max_radius = 1;
						for (int radius = 0; radius <= probe_resolve_max_radius; radius++)
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
								resolve_radius_hits[radius]++;
								memcpy(out_sh, best, sizeof(float) * 28);
								return;
							}
						}

						// nothing within reach - fall back to the map average, DC only, and assume the
						// probe is lit like the zone fallback (which every shipped map sets to 1.0)
						resolve_radius_hits[5]++;
						lightgrid_probes::constant_sh(ambient, sh_ambient_scale, out_sh);
						out_sh[27] = 1.0f;
					};

				// Only emit probe cells where the source light grid actually has data. Shipped IW7
				// volumes are sparse and shaped like the map; ours filled the whole bounding box,
				// so every cell - including the ones inside walls and outside the play space - got
				// a plausible mid-grey from the nearest lit sample via the shell search below. That
				// is what left the volume a solid slab with no dark areas anywhere, and it reads in
				// game as "lit from every direction": right average colour, no contrast, no form.
				//
				// One grid cell of margin keeps a shell of probes around occupied space, since
				// probes sit on cell corners and a model at the edge of the play area still needs
				// all eight of them.
				if (!grid_samples.empty())
				{
					probe_params.cell_occupied = [&grid_samples](const float* lo, const float size)
					{
						const auto lo_x = static_cast<int>(std::floor(lo[0] / 32.0f)) + 4096 - 1;
						const auto hi_x = static_cast<int>(std::floor((lo[0] + size) / 32.0f)) + 4096 + 1;
						const auto lo_y = static_cast<int>(std::floor(lo[1] / 32.0f)) + 4096 - 1;
						const auto hi_y = static_cast<int>(std::floor((lo[1] + size) / 32.0f)) + 4096 + 1;
						const auto lo_z = static_cast<int>(std::floor(lo[2] / 64.0f)) + 2048 - 1;
						const auto hi_z = static_cast<int>(std::floor((lo[2] + size) / 64.0f)) + 2048 + 1;

						for (int z = lo_z; z <= hi_z; z++)
						{
							if (z < 0 || z > 0xFFFF) continue;
							for (int y = lo_y; y <= hi_y; y++)
							{
								if (y < 0 || y > 0xFFFF) continue;
								for (int x = lo_x; x <= hi_x; x++)
								{
									if (x < 0 || x > 0xFFFF) continue;
									const auto key = (static_cast<unsigned long long>(x) << 32)
										| (static_cast<unsigned long long>(y) << 16)
										| static_cast<unsigned long long>(z);
									if (grid_samples.find(key) != grid_samples.end())
									{
										return true;
									}
								}
							}
						}
						return false;
					};
				}

				const auto volume = lightgrid_probes::build(probe_params, sample_sh, sh_ambient_scale);

				{
					// what the shell search had to do
					ZONETOOL_INFO("GfxWorld \"%s\": probe resolve radius 0=%u 1=%u 2=%u 3=%u 4=%u, map-average fallback=%u",
						asset->name, resolve_radius_hits[0], resolve_radius_hits[1], resolve_radius_hits[2],
						resolve_radius_hits[3], resolve_radius_hits[4], resolve_radius_hits[5]);

					// how much dynamic range the source palette carries, as projected DC luminance
					std::vector<float> pal_lum;
					pal_lum.reserve(grid_samples.size());
					for (const auto& kv : grid_samples)
					{
						const auto& sh = kv.second;
						pal_lum.push_back((0.2126f * sh[0]) + (0.7152f * sh[9]) + (0.0722f * sh[18]));
					}
					if (!pal_lum.empty())
					{
						std::sort(pal_lum.begin(), pal_lum.end());
						const auto at = [&](const double f)
						{
							return pal_lum[static_cast<size_t>(f * (pal_lum.size() - 1))];
						};
						ZONETOOL_INFO("GfxWorld \"%s\": source cell luminance min=%.4f p5=%.4f p50=%.4f p95=%.4f max=%.4f over %zu cells",
							asset->name, pal_lum.front(), at(0.05), at(0.50), at(0.95), pal_lum.back(), pal_lum.size());
					}
				}

				// ---- per-voxel light lists -------------------------------------------------
				//
				// Leaving every leaf on one empty list is not "no data" - it is the assertion that
				// no light reaches any voxel in the map. IW7 keeps no other precomputed bound on a
				// local light's reach, so all that is left is the frustumLights proxy hull below,
				// a convex volume around the light that knows nothing about geometry. An omni
				// behind a wall therefore lights whatever is on the far side of it, viewmodels
				// included, and no shadow setting changes that because no shadow test is involved.
				//
				// Format, decoded from mp_frontend and mp_paris: a leaf's lightListAddress indexes
				// lightListArray, and the ushort there is (count << 7) - the low seven bits are
				// clear in every entry of both maps - followed by count raw indices into
				// ComWorld::primaryLights. Both maps open the array with the same two slots
				// (16384, 1) and put the empty list at address 2, which is what leaves with no
				// lights point at. That preamble decodes as a 128-entry list that would overrun
				// either array, so its meaning is not known; it is reproduced verbatim and nothing
				// is ever pointed at address 0 or 1.
				//
				// Spots only - see light_list_spot_only below for the measurement. A directional
				// light reaches every voxel equally so listing it would exclude nothing, and the
				// sun is bounded by its shadow cascades instead; why stock also leaves omnis out
				// is not established, only that it does, across 142 of them without exception.
				//
				// Occlusion reuses the light grid occupancy the sun bake marches, so it inherits
				// the same limitation: it sees world geometry only, and a leaf shadowed solely by
				// a static model will still list the light.
				std::vector<unsigned short> voxel_leaf_light_address;
				std::vector<unsigned short> voxel_light_list;
				if (volume.valid && volume.leaf_count && converter_com_world && !grid_samples.empty()
					&& volume.leaf_size > 0.0f
					&& volume.leaf_bounds_min.size() >= static_cast<size_t>(volume.leaf_count) * 3)
				{
					constexpr float light_trace_step = 16.0f;   // half the 32-unit grid cell
					constexpr float light_trace_slack = 48.0f;  // geometry hugging the bulb
					constexpr unsigned short light_list_empty_address = 2;
					constexpr size_t light_list_max_per_leaf = 16;

					// Spot-only, which is measured rather than assumed. Across mp_frontend,
					// mp_paris and cp_zmb the lists carry 6640 references and every single one is
					// a SPOT, while those maps hold 22 directional and 142 omni lights that are
					// never referenced once. cp_zmb is the decisive one: 142 omnis, 6541
					// references, no omni among them.
					//
					// It also does not gate what you might expect. Emitting light 3 of mp_test_h1
					// into no list at all left it still lighting the viewmodel through a wall, so
					// this structure does not feed dynamic model lighting - that comes from the
					// clustered frustumLights z-bins. These lists are still worth getting right
					// (one shared empty list is a wrong assertion, not missing data), but they are
					// not the light-through-wall mechanism.
					constexpr bool light_list_spot_only = true;

					struct local_light
					{
						unsigned short index;
						bool is_spot;
						float origin[3];
						float axis[3];   // down the cone, pointing away from the light
						float radius;
						float half_fov;
					};

					std::vector<local_light> locals;
					{
						const auto count = std::min<unsigned int>(new_asset->primaryLightCount,
							converter_com_world->primaryLightCount);
						for (unsigned int i = 0; i < count && i <= 0xFFFF; i++)
						{
							const auto& src = converter_com_world->primaryLights[i];
							const auto type = static_cast<unsigned char>(src.type);
							if (type != light_type_spot && type != light_type_omni)
							{
								continue;
							}
							if (light_list_spot_only && type != light_type_spot)
							{
								continue;
							}
							if (!(src.radius > 0.0f))
							{
								continue;
							}

							local_light light{};
							light.index = static_cast<unsigned short>(i);
							light.is_spot = (type == light_type_spot);
							memcpy(light.origin, src.origin, sizeof(light.origin));
							light.radius = src.radius;

							// dir points toward the light, so the cone runs the other way - the
							// same convention the frustum proxies are built on
							float axis[3] = { -src.dir[0], -src.dir[1], -src.dir[2] };
							const auto len = std::sqrt((axis[0] * axis[0]) + (axis[1] * axis[1])
								+ (axis[2] * axis[2]));
							if (len > 0.0f)
							{
								for (int k = 0; k < 3; k++)
								{
									light.axis[k] = axis[k] / len;
								}
							}
							else
							{
								// no axis to cone against, so treat it as omnidirectional
								light.axis[2] = -1.0f;
								light.is_spot = false;
							}

							light.half_fov = std::acos(std::max(-1.0f,
								std::min(1.0f, src.cosHalfFovOuter)));
							locals.push_back(light);
						}
					}

					// a cell carrying a light grid sample is open space; anything else is solid or
					// outside the authored volume
					const auto cell_open = [&grid_samples](const float p[3])
					{
						const auto x = static_cast<int>(std::floor(p[0] / 32.0f)) + 4096;
						const auto y = static_cast<int>(std::floor(p[1] / 32.0f)) + 4096;
						const auto z = static_cast<int>(std::floor(p[2] / 64.0f)) + 2048;
						if (x < 0 || x > 0xFFFF || y < 0 || y > 0xFFFF || z < 0 || z > 0xFFFF)
						{
							return false;
						}
						const auto key = (static_cast<unsigned long long>(x) << 32)
							| (static_cast<unsigned long long>(y) << 16)
							| static_cast<unsigned long long>(z);
						return grid_samples.find(key) != grid_samples.end();
					};

					// The last stretch before the bulb is skipped: lights are routinely mounted in
					// or against geometry, and marching all the way in would have every such light
					// occlude itself out of every list.
					const auto reaches = [&](const float from[3], const float to[3])
					{
						float dir[3] = { to[0] - from[0], to[1] - from[1], to[2] - from[2] };
						const auto dist = std::sqrt((dir[0] * dir[0]) + (dir[1] * dir[1])
							+ (dir[2] * dir[2]));
						if (dist <= light_trace_slack)
						{
							return true;
						}
						for (int k = 0; k < 3; k++)
						{
							dir[k] /= dist;
						}

						const auto stop = dist - light_trace_slack;
						for (auto t = light_trace_step; t < stop; t += light_trace_step)
						{
							const float p[3] = {
								from[0] + (dir[0] * t),
								from[1] + (dir[1] * t),
								from[2] + (dir[2] * t),
							};
							if (!cell_open(p))
							{
								return false;
							}
						}
						return true;
					};

					voxel_leaf_light_address.assign(volume.leaf_count, light_list_empty_address);
					voxel_light_list = { 16384, 1, 0 }; // preamble, then the shared empty list

					std::map<std::vector<unsigned short>, unsigned short> list_address;
					std::vector<std::pair<float, unsigned short>> ranked;
					std::vector<unsigned short> hits;
					unsigned int lit_leaves = 0;
					unsigned int clamped_leaves = 0;
					unsigned int dropped_leaves = 0;

					const auto half = volume.leaf_size * 0.5f;
					const auto half_diag = half * 1.7320508f;
					const auto inset = volume.leaf_size * 0.25f;

					for (unsigned int l = 0; l < volume.leaf_count; l++)
					{
						const auto* lo = &volume.leaf_bounds_min[static_cast<size_t>(l) * 3];
						const float centre[3] = { lo[0] + half, lo[1] + half, lo[2] + half };

						ranked.clear();
						for (const auto& light : locals)
						{
							const float delta[3] = {
								centre[0] - light.origin[0],
								centre[1] - light.origin[1],
								centre[2] - light.origin[2],
							};
							const auto dist = std::sqrt((delta[0] * delta[0])
								+ (delta[1] * delta[1]) + (delta[2] * delta[2]));

							// sphere against the leaf's bounding sphere, the conservative side of
							// a sphere-box test and a lot cheaper
							if (dist - half_diag > light.radius)
							{
								continue;
							}

							// cone, widened by the angle the leaf subtends from the light so a
							// narrow beam crossing a large voxel is not missed
							if (light.is_spot && dist > 0.001f)
							{
								const auto dot = ((delta[0] * light.axis[0])
									+ (delta[1] * light.axis[1])
									+ (delta[2] * light.axis[2])) / dist;
								const auto theta = std::acos(std::max(-1.0f, std::min(1.0f, dot)));
								const auto slack = std::asin(std::min(1.0f, half_diag / dist));
								if (theta > light.half_fov + slack)
								{
									continue;
								}
							}

							// One unoccluded sample anywhere in the leaf is enough to keep the
							// light: a voxel is far larger than the features that shadow it, and
							// dropping a light that should be there is a visible bug where keeping
							// a spare one only costs a little binning work.
							auto seen = false;
							for (int s = 0; s < 9 && !seen; s++)
							{
								float p[3];
								if (s == 0)
								{
									memcpy(p, centre, sizeof(p));
								}
								else
								{
									const auto c = s - 1;
									p[0] = lo[0] + ((c & 1) ? volume.leaf_size - inset : inset);
									p[1] = lo[1] + ((c & 2) ? volume.leaf_size - inset : inset);
									p[2] = lo[2] + ((c & 4) ? volume.leaf_size - inset : inset);
								}
								if (!cell_open(p))
								{
									continue; // sample sits in solid, so it sees nothing
								}
								seen = reaches(p, light.origin);
							}
							if (!seen)
							{
								continue;
							}

							ranked.emplace_back(dist, light.index);
						}

						if (ranked.empty())
						{
							continue;
						}

						// nearest first, so a clamp keeps the lights that matter most
						std::sort(ranked.begin(), ranked.end());
						if (ranked.size() > light_list_max_per_leaf)
						{
							ranked.resize(light_list_max_per_leaf);
							clamped_leaves++;
						}

						hits.clear();
						for (const auto& r : ranked)
						{
							hits.push_back(r.second);
						}
						std::sort(hits.begin(), hits.end()); // canonical, so equal sets share

						const auto found = list_address.find(hits);
						if (found != list_address.end())
						{
							voxel_leaf_light_address[l] = found->second;
							lit_leaves++;
							continue;
						}

						// addresses are ushorts, so the array cannot grow past 64K entries
						if (voxel_light_list.size() + hits.size() + 1 > 0xFFFF)
						{
							dropped_leaves++;
							continue;
						}

						const auto address = static_cast<unsigned short>(voxel_light_list.size());
						voxel_light_list.push_back(static_cast<unsigned short>(hits.size() << 7));
						voxel_light_list.insert(voxel_light_list.end(), hits.begin(), hits.end());
						list_address.emplace(hits, address);
						voxel_leaf_light_address[l] = address;
						lit_leaves++;
					}

					// The leaf boxes come from the generator's own cell coordinates, so this can
					// only fail if the two disagree about where the volume sits - worth knowing
					// before anything downstream trusts the lists.
					{
						float ext_lo[3] = { 1e30f, 1e30f, 1e30f };
						float ext_hi[3] = { -1e30f, -1e30f, -1e30f };
						for (unsigned int l = 0; l < volume.leaf_count; l++)
						{
							const auto* lo = &volume.leaf_bounds_min[static_cast<size_t>(l) * 3];
							for (int k = 0; k < 3; k++)
							{
								ext_lo[k] = std::min(ext_lo[k], lo[k]);
								ext_hi[k] = std::max(ext_hi[k], lo[k] + volume.leaf_size);
							}
						}
						for (int k = 0; k < 3; k++)
						{
							if (ext_lo[k] < volume.bound_min[k] - 0.5f
								|| ext_hi[k] > volume.bound_max[k] + 0.5f)
							{
								ZONETOOL_WARNING("GfxWorld \"%s\": leaf boxes run outside the voxel "
									"tree bounds on axis %d (%.1f..%.1f vs %.1f..%.1f)", asset->name,
									k, ext_lo[k], ext_hi[k], volume.bound_min[k], volume.bound_max[k]);
								break;
							}
						}

						ZONETOOL_INFO("GfxWorld \"%s\": %zu local lights over %u leaves of %.0f "
							"units, spanning %.0f %.0f %.0f .. %.0f %.0f %.0f", asset->name,
							locals.size(), volume.leaf_count, volume.leaf_size,
							ext_lo[0], ext_lo[1], ext_lo[2], ext_hi[0], ext_hi[1], ext_hi[2]);
					}

					ZONETOOL_INFO("GfxWorld \"%s\": voxel light lists - %u/%u leaves lit (%.1f%%), "
						"%zu distinct lists, %zu entries", asset->name, lit_leaves, volume.leaf_count,
						volume.leaf_count ? (100.0f * lit_leaves) / volume.leaf_count : 0.0f,
						list_address.size(), voxel_light_list.size());

					if (clamped_leaves || dropped_leaves)
					{
						ZONETOOL_WARNING("GfxWorld \"%s\": %u leaves clamped to %zu lights, %u left "
							"unlit because the light list array filled up", asset->name,
							clamped_leaves, light_list_max_per_leaf, dropped_leaves);
					}

					if (!locals.empty() && !lit_leaves)
					{
						ZONETOOL_WARNING("GfxWorld \"%s\": %zu local lights reach no voxel at all - "
							"every one will be culled everywhere", asset->name, locals.size());
					}
				}
				else if (volume.valid && volume.leaf_count)
				{
					// Falling through here writes the generator's placeholder - one empty list for
					// the whole map - which is the state that lets local lights through walls. It
					// is a silent failure otherwise, so say which input was missing.
					ZONETOOL_WARNING("GfxWorld \"%s\": no voxel light lists (com world %s, %zu grid "
						"cells, %u leaves, %.1f unit leaves) - local lights will not be culled by "
						"geometry", asset->name, converter_com_world ? "ok" : "MISSING",
						grid_samples.size(), volume.leaf_count, volume.leaf_size);
				}

				auto& zone = *new_asset->lightGrid.probeData.zones;
				if (volume.valid)
				{
					auto& pd = new_asset->lightGrid.probeData;

					// gpuVisibleProbes is NOT a second copy of the grid probes. R_LoadWorld calls
					// sub_1404BE920, which walks gpuVisibleProbePositions and uploads every origin into
					// the "light grid sampling requests" buffer; the GPU then resolves the tetrahedral
					// volume at those positions and writes the resulting SH into gpuVisibleProbesData,
					// which is where static models read their lighting from.
					//
					// The array is a concatenation of per-model slices: smodel i owns
					// [unk0, unk0 + unk3), where unk0/unk1 form a 32-bit first index and unk3 is the
					// count. Verified exactly on mp_dome_dusk: sum(unk3) == gpuVisibleProbesCount ==
					// 21152, and the sorted unk0 values equal the running sum of unk3 with no gaps or
					// overlaps. unk2 is a layout enum that pins the count - 0 -> 2 points (2444 of 3157
					// models), 1 -> 3, 2 and 3 -> larger sample grids for big models.
					//
					// Leaving these fields zero (what this converter did before) points every model at
					// slice 0, so every model is lit by one sample regardless of where it stands - which
					// is exactly the "one flat colour everywhere" symptom. Emit the simplest shipped
					// layout instead: unk2 = 0 and two sample points at the model's own origin.
					const auto smodel_count = asset->dpvs.smodelCount;
					const auto sample_probe_count = smodel_count * smodel_probe_samples;

					pd.gpuVisibleProbesCount = sample_probe_count;
					pd.gpuVisibleProbePositions = allocator.allocate<IW7::GfxGpuLightGridProbePosition>(
						sample_probe_count ? sample_probe_count : 1);

					// the trailing 0x2000 entries are GPU scratch and are zero in every shipped map
					pd.gpuVisibleProbesData =
						allocator.allocate<IW7::GfxSHProbeData>(sample_probe_count + 0x2000);

					// bake the resolved SH too, so the array is sane before the GPU first writes it -
					// shipped maps carry real values here, not zeros
					for (unsigned int i = 0; i < smodel_count; i++)
					{
						const auto& placement = asset->dpvs.smodelDrawInsts[i].placement;

						// Sample at the model's BOUNDS CENTRE, not its placement origin.
						//
						// Measured over every unk2 == 0 (two-sample) model in mp_dome_dusk, mp_paris
						// and mp_afghan - 12,914 models - not one of them samples exactly at its
						// placement origin. The offsets are small and centred (median 0.00 in x and
						// y, slightly positive in z) with a p5..p95 spread of a few units laterally
						// and up to +/-25 vertically, which is what a model-space bounds centre
						// rotated into world space looks like. Sampling at the origin puts a tall
						// prop's probe at its feet.
						float position[3] = { placement.origin[0], placement.origin[1],
							placement.origin[2] };
						if (const auto* model = asset->dpvs.smodelDrawInsts[i].model)
						{
							const auto* mid = model->bounds.midPoint;
							for (int r = 0; r < 3; r++)
							{
								position[r] += placement.scale
									* ((placement.axis[0][r] * mid[0])
										+ (placement.axis[1][r] * mid[1])
										+ (placement.axis[2][r] * mid[2]));
							}
						}

						float sh[28] = {};
						sample_sh(position, sh);
						unsigned short coeffs[32];
						lightgrid_probes::encode_probe_sh(sh, coeffs);

						// The two points of the unk2 == 0 layout are IDENTICAL in shipped data -
						// 100% of pairs in every authentic map - so emitting the same position
						// twice is correct, not a placeholder.
						for (unsigned int k = 0; k < smodel_probe_samples; k++)
						{
							const auto slot = (i * smodel_probe_samples) + k;
							memcpy(pd.gpuVisibleProbePositions[slot].origin, position, sizeof(position));
							memcpy(&pd.gpuVisibleProbesData[slot], coeffs, sizeof(coeffs));
						}
					}

					pd.probeCount = volume.probe_count;
					pd.probes = allocator.allocate<IW7::GfxSHProbeData>(volume.probe_count);
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

					// tetrahedronVisibility is a COMPACTED array, and bit 31 of indexFlags is what
					// selects into it.
					//
					// Measured across all six authentic maps, `tetrahedronCountVisible` is exactly
					// the number of tetrahedra with **at least one** corner carrying bit 31 - not
					// 41-47% by coincidence, but an exact match in every map:
					//
					//   mp_dome_dusk 103402   mp_paris 297161   mp_afghan 319972
					//   mp_breakneck 213259   cp_zmb   322466   mp_frontend 13786
					//
					// So bit 31 is not the "refinement level" marker it was assumed to be: it marks
					// a tetrahedron as having an entry, and the array is indexed by the running
					// count of flagged tetrahedra. The entries themselves are 64 **uint8 weights**
					// (four blocks of 16), not a 512-bit mask - shipped bytes take values like 0xDA,
					// 0x93, 0x5D, saturating at 0xFF, and no shipped entry is all-0xFF.
					//
					// This generator sets no corner flags, so under that rule no tetrahedron has an
					// entry and the honest emission is an empty array. Declaring
					// tetrahedronCountVisible = tetrahedronCount while flagging nothing was
					// internally inconsistent, and cost 64 bytes per tetrahedron of zone memory
					// that nothing could ever address.
					unsigned int visible_tets = 0;
					for (unsigned int t = 0; t < volume.tetrahedron_count; t++)
					{
						for (int k = 0; k < 4; k++)
						{
							if (volume.tetrahedrons[(t * 4) + k] & 0x80000000u)
							{
								visible_tets++;
								break;
							}
						}
					}

					pd.tetrahedronCountVisible = visible_tets;
					if (visible_tets)
					{
						pd.tetrahedronVisibility =
							allocator.allocate<IW7::GfxGpuLightGridTetrahedronVisibility>(visible_tets);
						// fully visible everywhere: we have no per-probe occlusion bake to put here
						memset(pd.tetrahedronVisibility, 0xFF,
							sizeof(IW7::GfxGpuLightGridTetrahedronVisibility) * visible_tets);
					}
					else
					{
						pd.tetrahedronVisibility = nullptr;
					}

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
					// zoneBound is what selects this tree at run time, and getting it wrong is
					// fatal rather than degraded: sub_140E3C1F0 walks voxelTree[], skips any entry
					// with voxelTopDownViewNodeCount == 0, takes the first whose box *strictly*
					// contains the camera (fabs(cam - midPoint) < halfSize on all three axes), and
					// calls Sys_Error("No valid voxel trees.  Are there empty skyboxes in your
					// map?") when none does.
					//
					// So it has to cover the volume this tree actually indexes, which is
					// voxelTreeHeader->boundMin..boundMax below - the generator snaps the origin
					// down to a root-cell boundary and rounds the extent up, making that box
					// strictly larger than IW5's GfxWorld::bounds. Copying the smaller box (what
					// this used to do) hard-errors for any camera in the margin between the two
					// and buys nothing.
					for (int i = 0; i < 3; i++)
					{
						tree.zoneBound.midPoint[i] = (volume.bound_min[i] + volume.bound_max[i]) * 0.5f;
						tree.zoneBound.halfSize[i] = (volume.bound_max[i] - volume.bound_min[i]) * 0.5f;
					}
					tree.voxelTopDownViewNodeCount = static_cast<int>(volume.top_down_view_nodes.size());
					tree.voxelInternalNodeCount = static_cast<int>(volume.internal_nodes.size());
					// The baked lists replace the generator's placeholder pair, which points every
					// leaf at one empty list. They are only used together - a leaf address means
					// nothing against a different array - so both fall back or neither does.
					const auto have_baked_lights =
						voxel_leaf_light_address.size() == volume.leaf_nodes.size()
						&& !voxel_light_list.empty();
					const auto& leaf_addresses =
						have_baked_lights ? voxel_leaf_light_address : volume.leaf_nodes;
					const auto& light_list =
						have_baked_lights ? voxel_light_list : volume.light_list;

					tree.voxelLeafNodeCount = static_cast<int>(leaf_addresses.size());
					tree.lightListArraySize = static_cast<int>(light_list.size());

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
					memcpy(tree.voxelLeafNodeArray, leaf_addresses.data(),
						sizeof(unsigned short) * leaf_addresses.size());
					tree.lightListArray = allocator.allocate<unsigned short>(tree.lightListArraySize);
					memcpy(tree.lightListArray, light_list.data(),
						sizeof(unsigned short) * light_list.size());
					tree.voxelInternalNodeDynamicLightList =
						allocator.allocate<unsigned int>(2 * tree.voxelInternalNodeCount); // runtime
				}

				// The zone fallback is used when a sample resolves to no tetrahedron; it is also all
				// a map gets if the volume could not be built - which is exactly when it must not
				// be zero. build() returns before it has any probes to average on its failure
				// paths, so its zone_fallback_coeffs are zeroed there, and copying those through
				// meant "volume failed" rendered as "every model black". Fall back to the map
				// average instead, which is what the samples would have averaged to anyway.
				if (volume.valid)
				{
					memcpy(zone.fallbackProbeData.coeffs, volume.zone_fallback_coeffs,
						sizeof(zone.fallbackProbeData.coeffs));
				}
				else
				{
					ZONETOOL_WARNING("GfxWorld \"%s\": light grid probe volume could not be built; "
						"falling back to a flat ambient probe", asset->name);

					float fallback_sh[28] = {};
					lightgrid_probes::constant_sh(ambient, sh_ambient_scale, fallback_sh);
					fallback_sh[27] = 1.0f; // every shipped map's zone fallback is fully lit
					unsigned short fallback[32];
					lightgrid_probes::encode_probe_sh(fallback_sh, fallback);
					memcpy(zone.fallbackProbeData.coeffs, fallback,
						sizeof(zone.fallbackProbeData.coeffs));
				}
				memset(zone.fallbackProbeData.pad, 0, sizeof(zone.fallbackProbeData.pad));

				// Diagnostic, off by default. Paints the two places a model can get a constant colour
				// from, in colours nothing else in a map produces, so one run says which it is:
				//   magenta -> the sample resolved to no tetrahedron and fell back to the zone probe,
				//              i.e. the GPU walk is not resolving (note r_lightGridDefaultColor does
				//              NOT cover this case - the engine uses this baked value, not the dvar);
				//   green   -> the model is reading gpuVisibleProbesData slot 0, i.e. its probe index
				//              is wrong rather than the walk;
				//   varies  -> the walk works and lighting is being sampled per position;
				//   unchanged -> the model is lit from neither, and the path is somewhere else.
				constexpr bool debug_probe_paint = false;
				if (debug_probe_paint)
				{
					// bright enough to be unmistakable next to a typical DC of ~26
					constexpr float paint_level = 0.35f;
					const float magenta[3] = { paint_level, 0.0f, paint_level };
					const float green[3] = { 0.0f, paint_level, 0.0f };

					float sh[27];
					unsigned short coeffs[32];

					lightgrid_probes::constant_sh(magenta, sh_ambient_scale, sh);
					lightgrid_probes::encode_probe_sh(sh, coeffs);
					memcpy(zone.fallbackProbeData.coeffs, coeffs,
						sizeof(zone.fallbackProbeData.coeffs));

					if (new_asset->lightGrid.probeData.gpuVisibleProbesData
						&& new_asset->lightGrid.probeData.gpuVisibleProbesCount)
					{
						lightgrid_probes::constant_sh(green, sh_ambient_scale, sh);
						lightgrid_probes::encode_probe_sh(sh, coeffs);
						memcpy(&new_asset->lightGrid.probeData.gpuVisibleProbesData[0], coeffs,
							sizeof(coeffs));
					}
				}
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
				// Resolve the ComWorld from the asset DB rather than through converter_com_world.
				// That global is only set once GenerateIW7ComWorld has run, and the GfxWorld is
				// converted first, so it was still null here and *every* light silently ended up
				// with an empty proxy hull - which is the "no local lights" state, since
				// R_IsCameraInsideLightMeshVolume leaves an inverted z range for vertexCount == 0.
				// The DB lookup has no ordering dependency. Fall back to the global so a caller
				// that has already converted the ComWorld still works.
				struct proxy_light
				{
					unsigned char type;
					float origin[3];
					float dir[3];
					float radius;
					float cos_half_fov_outer;
				};

				std::vector<proxy_light> lights;
				if (converter_com_world)
				{
					const auto count = std::min<unsigned int>(new_asset->primaryLightCount,
						converter_com_world->primaryLightCount);
					lights.reserve(count);
					for (unsigned int i = 0; i < count; i++)
					{
						const auto& src = converter_com_world->primaryLights[i];
						proxy_light light{};
						light.type = static_cast<unsigned char>(src.type);
						memcpy(light.origin, src.origin, sizeof(light.origin));
						memcpy(light.dir, src.dir, sizeof(light.dir));
						light.radius = src.radius;
						light.cos_half_fov_outer = src.cosHalfFovOuter;
						lights.push_back(light);
					}
				}
				else
				{
					ZONETOOL_WARNING("GfxWorld \"%s\": no ComWorld found, local lights will have no "
						"frustum proxy and may not be binned", asset->name);
				}

				{
					const auto light_count = static_cast<unsigned int>(lights.size());
					unsigned int proxy_count = 0;
					unsigned int clipped_count = 0;

					for (unsigned int i = 0; i < light_count; i++)
					{
						const auto& light = lights[i];
						if (light.radius <= 0.0f)
						{
							continue;
						}

						// dir points toward the light, so the volume runs the other way
						float axis[3] = { -light.dir[0], -light.dir[1], -light.dir[2] };
						normalize_proxy_axis(axis);

						proxy_mesh mesh{};
						const auto cos_outer = std::max(-1.0f, std::min(1.0f, light.cos_half_fov_outer));
						const auto half_angle = std::acos(cos_outer);

						// past ~80 degrees the expanded cone runs into tan(), and the spot is most of a
						// hemisphere anyway - the sphere hull contains it and stays well conditioned
						if (light.type != light_type_spot && light.type != light_type_omni)
						{
							// NONE and DIR (the sun) carry no proxy in shipped maps
							continue;
						}

						if (light_proxy_use_sphere)
						{
							build_omni_proxy(mesh, light.origin, light.radius * light_proxy_sphere_scale);
						}
						else if (light.type == light_type_spot && half_angle < light_proxy_wide_spot_cutoff)
						{
							build_spot_proxy(mesh, light.origin, axis, half_angle, light.radius);
						}
						else
						{
							build_omni_proxy(mesh, light.origin, light.radius);
						}
						// Clip the hull to where the source bake says this light lands.
						//
						// This mesh is rasterised as the light's volume, so its shape decides where
						// the light appears. An analytic cone or sphere runs straight through walls.
						// The legacy light grid already knows better: every cell names the one
						// primary light that lights models in it, computed by the IW3 compiler with
						// real visibility, so the union of a light's cells is its true reach.
						//
						// A light with no cells is left unclipped rather than deleted: an empty hull is
						// the "no local lights" state, and losing a light is worse than one that
						// over-reaches.
						//
						// Known limit: this is a box, so the volume keeps flat faces where it is cut
						// and the falloff terminates on cell boundaries rather than following the
						// room. Fitting each vertex by a ray against world collision would fix that
						// too, but the clipmap is converted after the GfxWorld, so the geometry is not
						// available at this point.
						{
							const auto found = light_boxes.find(i);
							if (found != light_boxes.end() && found->second.cells)
							{
								const auto& box = found->second;
								// A full cell on BOTH sides, which is what quaK settled on by eye: it reaches the
								// outer wall face and reads best in game. Tighter variants were tried and are
								// worse to look at - half a cell on the max side only lands the hull exactly on
								// the near surfaces, and no margin at all cuts the volume short of the room.
								//
								// None of this decides whether a light passes through a wall: the clustered
								// binning uses the light sphere, not this hull, so the shape is a look choice
								// rather than a cull. See the radius note in ComWorld.cpp.
								constexpr float cell_margin[3] = { 32.0f, 32.0f, 64.0f };

								for (size_t v = 0; v + 2 < mesh.vertices.size(); v += 3)
								{
									for (int k = 0; k < 3; k++)
									{
										mesh.vertices[v + k] = std::max(box.lo[k] - cell_margin[k],
											std::min(box.hi[k] + cell_margin[k], mesh.vertices[v + k]));
									}
								}
								clipped_count++;
							}
							else if (light.type == light_type_spot || light.type == light_type_omni)
							{
								ZONETOOL_WARNING("GfxWorld \"%s\": light %u has no light grid cells, "
									"so its proxy hull is unclipped and may light through walls",
									asset->name, i);
							}
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
						proxy_count++;
					}

					ZONETOOL_INFO("GfxWorld \"%s\": %u frustum light proxies over %u primary lights, "
						"%u clipped to their light grid cells",
						asset->name, proxy_count, new_asset->primaryLightCount, clipped_count);
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

			// ---- spot shadow casters ---------------------------------------------------
			//
			// shadowGeomOptimized is the caster list the spot shadow pass draws. A light with a
			// shadow map and no casters renders an empty shadow, which is indistinguishable in
			// game from having no shadow at all - and that is what IW5 hands over: mp_test_h1's
			// two local lights arrive with 1 surface and 0 static models each, on a map with 40
			// static models. Whether IW3 never filled it in or the IW3->IW5 step drops it, it is
			// not usable, so build the list here instead of copying it.
			//
			// The stored indices are world surface indices, not positions in dpvs.sortedSurfIndex:
			// R_AddBsp reads shadowGeom->sortedSurfIndex[i] straight into surfIndex and hands it
			// to R_ShouldDrawTransientZoneSurface. That is the same space dpvs.surfacesBounds is
			// indexed by. smodelIndex holds indices into dpvs.smodelDrawInsts.
			//
			// Sun lights (index <= lastSunPrimaryLightIndex) get no list: IW7 keeps sun casters in
			// GfxSurface::flags and GfxStaticModelDrawInst::sunShadowFlags instead, and every
			// shipped map zeroes exactly that range.
			{
				ZONETOOL_INFO("GfxWorld \"%s\": lastSunPrimaryLightIndex=%u, source shadowGeom=%s",
					asset->name, new_asset->lastSunPrimaryLightIndex,
					asset->shadowGeom ? "present" : "NULL");

				new_asset->shadowGeomOptimized =
					allocator.allocate<IW7::GfxShadowGeometry>(new_asset->primaryLightCount);

				const auto light_count = converter_com_world
					? std::min<unsigned int>(new_asset->primaryLightCount,
						converter_com_world->primaryLightCount)
					: 0u;

				std::vector<unsigned int> caster_surfaces;
				std::vector<unsigned short> caster_smodels;

				for (unsigned int i = 0; i < light_count; i++)
				{
					if (i <= new_asset->lastSunPrimaryLightIndex)
					{
						continue;
					}

					const auto& light = converter_com_world->primaryLights[i];
					const auto type = static_cast<unsigned char>(light.type);
					if (type != light_type_spot && type != light_type_omni)
					{
						continue;
					}
					if (!(light.radius > 0.0f))
					{
						continue;
					}

					// cone axis runs along -dir, the same convention as the frustum proxies
					float axis[3] = { -light.dir[0], -light.dir[1], -light.dir[2] };
					const auto axis_len = std::sqrt((axis[0] * axis[0]) + (axis[1] * axis[1])
						+ (axis[2] * axis[2]));
					const auto is_spot = (type == light_type_spot) && (axis_len > 0.0f);
					if (axis_len > 0.0f)
					{
						for (int k = 0; k < 3; k++)
						{
							axis[k] /= axis_len;
						}
					}
					const auto half_fov = std::acos(std::max(-1.0f,
						std::min(1.0f, light.cosHalfFovOuter)));

					// A box reaches the light if the light sphere touches it, and for a spot the
					// box also has to fall inside the cone widened by the angle the box subtends -
					// the same conservative pair of tests the voxel light lists use. Casters are
					// better over-included than missed: an extra one costs a little shadow map
					// fill, a missing one is a hole in the shadow.
					const auto reaches = [&](const Bounds& bounds)
					{
						const auto radius = std::sqrt(
							(bounds.halfSize[0] * bounds.halfSize[0])
							+ (bounds.halfSize[1] * bounds.halfSize[1])
							+ (bounds.halfSize[2] * bounds.halfSize[2]));

						const float delta[3] = {
							bounds.midPoint[0] - light.origin[0],
							bounds.midPoint[1] - light.origin[1],
							bounds.midPoint[2] - light.origin[2],
						};
						const auto dist = std::sqrt((delta[0] * delta[0]) + (delta[1] * delta[1])
							+ (delta[2] * delta[2]));

						if (dist - radius > light.radius)
						{
							return false;
						}

						if (is_spot && dist > 0.001f)
						{
							const auto dot = ((delta[0] * axis[0]) + (delta[1] * axis[1])
								+ (delta[2] * axis[2])) / dist;
							const auto theta = std::acos(std::max(-1.0f, std::min(1.0f, dot)));
							const auto slack = std::asin(std::min(1.0f, radius / dist));
							if (theta > half_fov + slack)
							{
								return false;
							}
						}
						return true;
					};

					caster_surfaces.clear();
					caster_smodels.clear();

					if (asset->dpvs.surfacesBounds)
					{
						for (unsigned int sf = 0; sf < asset->surfaceCount; sf++)
						{
							if (reaches(asset->dpvs.surfacesBounds[sf].bounds))
							{
								caster_surfaces.push_back(sf);
							}
						}
					}

					if (asset->dpvs.smodelInsts)
					{
						for (unsigned int sm = 0; sm < asset->dpvs.smodelCount && sm <= 0xFFFF; sm++)
						{
							if (reaches(asset->dpvs.smodelInsts[sm].bounds))
							{
								caster_smodels.push_back(static_cast<unsigned short>(sm));
							}
						}
					}

					// both counts are ushorts in the struct
					if (caster_surfaces.size() > 0xFFFF)
					{
						caster_surfaces.resize(0xFFFF);
					}
					if (caster_smodels.size() > 0xFFFF)
					{
						caster_smodels.resize(0xFFFF);
					}

					auto& dest = new_asset->shadowGeomOptimized[i];
					dest.surfaceCount = static_cast<unsigned short>(caster_surfaces.size());
					dest.smodelCount = static_cast<unsigned short>(caster_smodels.size());

					if (dest.surfaceCount)
					{
						dest.sortedSurfIndex = allocator.allocate<unsigned int>(dest.surfaceCount);
						memcpy(dest.sortedSurfIndex, caster_surfaces.data(),
							sizeof(unsigned int) * dest.surfaceCount);
					}
					if (dest.smodelCount)
					{
						dest.smodelIndex = allocator.allocate<unsigned short>(dest.smodelCount);
						memcpy(dest.smodelIndex, caster_smodels.data(),
							sizeof(unsigned short) * dest.smodelCount);
					}

					ZONETOOL_INFO("GfxWorld \"%s\": light %u shadow casters - %u surfaces, %u "
						"smodels (was %u / %u from the source)", asset->name, i,
						dest.surfaceCount, dest.smodelCount,
						asset->shadowGeom ? asset->shadowGeom[i].surfaceCount : 0,
						asset->shadowGeom ? asset->shadowGeom[i].smodelCount : 0);
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
					// Env 0, which is what every stock map does. IW5's primaryLightIndex selects a
					// primary light directly (1 = sun); IW7's primaryLightEnvIndex selects a
					// ComPrimaryLightEnv, a *set* of up to 4 lights. Different index spaces.
					//
					// This used to carry IW5's index across, on the reasoning that env 0 is empty in
					// our ComWorld so pointing at it would drop the sun from every static model. But
					// env 0 is empty in *stock* too - all nine dumped ComWorlds have env 0 with
					// numIndices == 0 and every static model pointing at it (see the note in
					// ComWorld.cpp). Stock static models simply are not lit through this path; they
					// take their lighting from the probe volume, which is why that volume has to work.
					//
					// Carrying the index across attached a specific local light to a whole model with
					// no geometry test anywhere in the path - not the frustum hull, not the voxel
					// light list, not the probes. That is the light-through-a-wall bug: it appeared
					// per model ("only on that material, nowhere around it"), and no amount of
					// clipping the light's volume touched it, because this path never consults it.
					new_asset->dpvs.smodelDrawInsts[i].primaryLightEnvIndex = 0;
					// Every static model in every stock map checked uses probe 0 (mp_dome_dusk 3157/3157,
					// mp_breakneck 13269/13269) even though those maps ship 31 and 55 probes, so IW7 picks
					// the reflection probe at runtime and the baked index is not per-model data - the same
					// pattern as primaryLightEnvIndex and the sky/default colour tables. Carrying IW5's
					// index across (1 and 4 on mp_test) points glossy models at a cubemap the engine never
					// intended, which adds a bright environment specular on top of the albedo - white car
					// paint over brown, while matte lightmapped world surfaces are unaffected.
					new_asset->dpvs.smodelDrawInsts[i].reflectionProbeIndex = 0;
					new_asset->dpvs.smodelDrawInsts[i].firstMtlSkinIndex = asset->dpvs.smodelDrawInsts[i].firstMtlSkinIndex;
					// which sun lights this model casts for; a model with no bit set is skipped outright by
					// R_AddAllStaticModelSurfacesRangeSunShadow once the opt path is live, so enrol every model.
					new_asset->dpvs.smodelDrawInsts[i].sunShadowFlags = sun_light_mask;
					new_asset->dpvs.smodelDrawInsts[i].transientZone = 0;

					// this model's slice of gpuVisibleProbePositions (see the light grid block above)
					const auto probe_slice_first = i * smodel_probe_samples;
					new_asset->dpvs.smodelDrawInsts[i].unk0 =
						static_cast<unsigned short>(probe_slice_first & 0xFFFF);
					new_asset->dpvs.smodelDrawInsts[i].unk1 =
						static_cast<unsigned short>(probe_slice_first >> 16);
					new_asset->dpvs.smodelDrawInsts[i].unk2 = 0;
					new_asset->dpvs.smodelDrawInsts[i].unk3 =
						static_cast<unsigned short>(smodel_probe_samples);

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

				// The optimised sun shadow caster set, one bit array per sun light.
				//
				// Leaving this null (what this did before) does not disable sun shadows - the
				// dynamic cascade still draws them - but it does cost the cached static path
				// that R_AddAllStaticModelSurfacesRangeSunShadow feeds, which is what carries
				// distant shadows. The symptom is exactly that: crisp shadows near the camera
				// and coarse blobs further out.
				//
				// Layout is sunShadowOptCount rows of sunSurfVisDataCount 32-bit words, one
				// row per sun light, one bit per surface. Shipped maps cap the row count at 5
				// because GfxSurface::flags only spares bits 3..7 for the per-sun-light mask,
				// and otherwise it equals lastSunPrimaryLightIndex. Every surface that casts a
				// sun shadow joins every row, which is the same all-lights mask already given
				// to GfxStaticModelDrawInst::sunShadowFlags above.
				if (sun_light_count && new_asset->dpvs.surfaceCastsSunShadow)
				{
					new_asset->dpvs.sunShadowOptCount = sun_light_count;
					new_asset->dpvs.sunSurfVisDataCount = new_asset->dpvs.surfaceVisDataCount;

					const auto words = static_cast<size_t>(new_asset->dpvs.sunShadowOptCount)
						* new_asset->dpvs.sunSurfVisDataCount;
					new_asset->dpvs.surfaceCastsSunShadowOpt =
						allocator.allocate<unsigned int>(words);

					for (unsigned int row = 0; row < new_asset->dpvs.sunShadowOptCount; row++)
					{
						memcpy(&new_asset->dpvs.surfaceCastsSunShadowOpt[
								static_cast<size_t>(row) * new_asset->dpvs.sunSurfVisDataCount],
							new_asset->dpvs.surfaceCastsSunShadow,
							sizeof(unsigned int) * new_asset->dpvs.sunSurfVisDataCount);
					}

					ZONETOOL_INFO("GfxWorld \"%s\": sun shadow opt - %u rows x %u words over %u "
						"surfaces", asset->name, new_asset->dpvs.sunShadowOptCount,
						new_asset->dpvs.sunSurfVisDataCount, new_asset->surfaceCount);
				}
				else
				{
					new_asset->dpvs.sunShadowOptCount = 0;
					new_asset->dpvs.sunSurfVisDataCount = 0;
					new_asset->dpvs.surfaceCastsSunShadowOpt = nullptr;
					ZONETOOL_WARNING("GfxWorld \"%s\": no sun shadow opt data (sun lights %u, "
						"casts array %s) - distant sun shadows will stay coarse", asset->name,
						sun_light_count,
						new_asset->dpvs.surfaceCastsSunShadow ? "present" : "NULL");
				}

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