#include "stdafx.hpp"
#include "../Include.hpp"

#include "Common/havok_builder.hpp"
#include "ClipMapCollision.hpp"

#include "XModel.hpp"
#include "Assets/Material.hpp"

namespace ZoneTool::IW5
{
	namespace IW7Converter
	{
		namespace
		{
			IW7::SurfaceFlags surf_flags_conversion_table[31]
			{
				IW7::SurfaceFlags::SURFACE_FLAG_NONE,
				IW7::SurfaceFlags::SURFACE_FLAG_BARK,
				IW7::SurfaceFlags::SURFACE_FLAG_BRICK,
				IW7::SurfaceFlags::SURFACE_FLAG_CARPET_SOLID,
				IW7::SurfaceFlags::SURFACE_FLAG_CLOTH,
				IW7::SurfaceFlags::SURFACE_FLAG_CONCRETE_DRY,
				IW7::SurfaceFlags::SURFACE_FLAG_DIRT,
				IW7::SurfaceFlags::SURFACE_FLAG_FLESH,
				IW7::SurfaceFlags::SURFACE_FLAG_FOLIAGE_DRY,
				IW7::SurfaceFlags::SURFACE_FLAG_GLASS_PANE,
				IW7::SurfaceFlags::SURFACE_FLAG_GRASS_SHORT,
				IW7::SurfaceFlags::SURFACE_FLAG_GRAVEL,
				IW7::SurfaceFlags::SURFACE_FLAG_ICE_SOLID,
				IW7::SurfaceFlags::SURFACE_FLAG_METAL_THICK,
				IW7::SurfaceFlags::SURFACE_FLAG_MUD,
				IW7::SurfaceFlags::SURFACE_FLAG_PAPER,
				IW7::SurfaceFlags::SURFACE_FLAG_PLASTER,
				IW7::SurfaceFlags::SURFACE_FLAG_ROCK,
				IW7::SurfaceFlags::SURFACE_FLAG_SAND,
				IW7::SurfaceFlags::SURFACE_FLAG_SNOW,
				IW7::SurfaceFlags::SURFACE_FLAG_WATER,
				IW7::SurfaceFlags::SURFACE_FLAG_WOOD_SOLID,
				IW7::SurfaceFlags::SURFACE_FLAG_ASPHALT_DRY,
				IW7::SurfaceFlags::SURFACE_FLAG_CERAMIC,
				IW7::SurfaceFlags::SURFACE_FLAG_PLASTIC,
				IW7::SurfaceFlags::SURFACE_FLAG_RUBBER,
				IW7::SurfaceFlags::SURFACE_FLAG_CUSHION,
				IW7::SurfaceFlags::SURFACE_FLAG_FRUIT,
				IW7::SurfaceFlags::SURFACE_FLAG_METAL_PAINTED,
				IW7::SurfaceFlags::SURFACE_FLAG_RIOTSHIELD,
				IW7::SurfaceFlags::SURFACE_FLAG_SLUSH,
			}; IW5::CSurfaceFlags;
		}

		int convert_surf_flags(int flags)
		{
				// The IW5 surface type is a 5-bit FIELD at bits 20..24 (SURF_FLAG_DEFAULT ..
				// SURF_FLAG_SLUSH, step 0x00100000, so values 0..30). Bits above it are
				// ordinary flags -- SURF_FLAG_MANTLEON 0x02000000, SURF_FLAG_MANTLEOVER
				// 0x04000000, SURF_FLAG_PORTAL 0x80000000 -- and each is converted separately
				// further down.
				//
				// A bare `flags >> 20` folded those into the index: a mantle surface indexed
				// the 31-entry table at type+32, a mantle-over one at type+64, and because
				// `flags` is signed, a portal surface shifted to a NEGATIVE index and read
				// memory in front of the table. Mantle and portal surfaces are ordinary in a
				// CoD4 map, and this feeds ShapeTagData::userData for world collision, so the
				// out-of-bounds read landed in the shipped zone.
				const auto surf_type = (static_cast<unsigned int>(flags) >> 20) & 0x1Fu;
				int IW7_flags = surf_flags_conversion_table[
					surf_type < ARRAYSIZE(surf_flags_conversion_table) ? surf_type : 0];
				auto convert = [&](IW5::CSurfaceFlags a, IW7::SurfaceFlags b)
				{
					IW7_flags |= ((flags & a) == a) ? b : 0;
				};
				convert(IW5::CSurfaceFlags::SURF_FLAG_OPAQUEGLASS, IW7::SurfaceFlags::SURFACE_FLAG_OPAQUEGLASS);
				convert(IW5::CSurfaceFlags::SURF_FLAG_CLIPMISSILE, IW7::SurfaceFlags::SURFACE_FLAG_CLIPMISSILE);
				convert(IW5::CSurfaceFlags::SURF_FLAG_AI_NOSIGHT, IW7::SurfaceFlags::SURFACE_FLAG_AI_NOSIGHT);
				convert(IW5::CSurfaceFlags::SURF_FLAG_CLIPSHOT, IW7::SurfaceFlags::SURFACE_FLAG_CLIPSHOT);
				convert(IW5::CSurfaceFlags::SURF_FLAG_PLAYERCLIP, IW7::SurfaceFlags::SURFACE_FLAG_PLAYERCLIP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_MONSTERCLIP, IW7::SurfaceFlags::SURFACE_FLAG_MONSTERCLIP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_AICLIPALLOWDEATH, IW7::SurfaceFlags::SURFACE_FLAG_AICLIPALLOWDEATH);
				convert(IW5::CSurfaceFlags::SURF_FLAG_VEHICLECLIP, IW7::SurfaceFlags::SURFACE_FLAG_VEHICLECLIP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_ITEMCLIP, IW7::SurfaceFlags::SURFACE_FLAG_ITEMCLIP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NODROP, IW7::SurfaceFlags::SURFACE_FLAG_NODROP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NONSOLID, IW7::SurfaceFlags::SURFACE_FLAG_NONSOLID);
				convert(IW5::CSurfaceFlags::SURF_FLAG_DETAIL, IW7::SurfaceFlags::SURFACE_FLAG_DETAIL);
				convert(IW5::CSurfaceFlags::SURF_FLAG_STRUCTURAL, IW7::SurfaceFlags::SURFACE_FLAG_STRUCTURAL);
				convert(IW5::CSurfaceFlags::SURF_FLAG_PORTAL, IW7::SurfaceFlags::SURFACE_FLAG_PORTAL);
				convert(IW5::CSurfaceFlags::SURF_FLAG_CANSHOOTCLIP, IW7::SurfaceFlags::SURFACE_FLAG_CANSHOOTCLIP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_ORIGIN, IW7::SurfaceFlags::SURFACE_FLAG_ORIGIN);
				convert(IW5::CSurfaceFlags::SURF_FLAG_SKY, IW7::SurfaceFlags::SURFACE_FLAG_SKY);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NOCASTSHADOW, IW7::SurfaceFlags::SURFACE_FLAG_NOCASTSHADOW);
				convert(IW5::CSurfaceFlags::SURF_FLAG_PHYSICSGEOM, IW7::SurfaceFlags::SURFACE_FLAG_PHYSICSGEOM);
				convert(IW5::CSurfaceFlags::SURF_FLAG_LIGHTPORTAL, IW7::SurfaceFlags::SURFACE_FLAG_LIGHTPORTAL);
				convert(IW5::CSurfaceFlags::SURF_FLAG_OUTDOORBOUNDS, IW7::SurfaceFlags::SURFACE_FLAG_OUTDOORBOUNDS);
				convert(IW5::CSurfaceFlags::SURF_FLAG_SLICK, IW7::SurfaceFlags::SURFACE_FLAG_SLICK);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NOIMPACT, IW7::SurfaceFlags::SURFACE_FLAG_NOIMPACT);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NOMARKS, IW7::SurfaceFlags::SURFACE_FLAG_NOMARKS);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NOPENETRATE, IW7::SurfaceFlags::SURFACE_FLAG_NOPENETRATE);
				convert(IW5::CSurfaceFlags::SURF_FLAG_LADDER, IW7::SurfaceFlags::SURFACE_FLAG_LADDER);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NODAMAGE, IW7::SurfaceFlags::SURFACE_FLAG_NODAMAGE);
				convert(IW5::CSurfaceFlags::SURF_FLAG_MANTLEON, IW7::SurfaceFlags::SURFACE_FLAG_MANTLEON);
				convert(IW5::CSurfaceFlags::SURF_FLAG_MANTLEOVER, IW7::SurfaceFlags::SURFACE_FLAG_MANTLEOVER);
				convert(IW5::CSurfaceFlags::SURF_FLAG_STAIRS, IW7::SurfaceFlags::SURFACE_FLAG_STAIRS);
				convert(IW5::CSurfaceFlags::SURF_FLAG_SOFT, IW7::SurfaceFlags::SURFACE_FLAG_SOFT);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NOSTEPS, IW7::SurfaceFlags::SURFACE_FLAG_NOSTEPS);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NODRAW, IW7::SurfaceFlags::SURFACE_FLAG_NODRAW);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NOLIGHTMAP, IW7::SurfaceFlags::SURFACE_FLAG_NOLIGHTMAP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NODLIGHT, IW7::SurfaceFlags::SURFACE_FLAG_NODLIGHT);
				return IW7_flags;
		}

		IW7::XModel* GenerateIW7Model(XModel* asset, allocator& mem)
		{
			// allocate IW7 XModel structure
			auto* iw7_asset = mem.allocate<IW7::XModel>();

			iw7_asset->name = asset->name;
			iw7_asset->numBones = asset->numBones;
			iw7_asset->numRootBones = asset->numRootBones;
			iw7_asset->numsurfs = asset->numsurfs;
			iw7_asset->numReactiveMotionParts = 0;
			iw7_asset->scale = asset->scale;
			memcpy(&iw7_asset->noScalePartBits, &asset->noScalePartBits, sizeof(asset->noScalePartBits));

			iw7_asset->boneNames = mem.allocate<IW7::scr_string_t>(asset->numBones);
			for (auto i = 0; i < asset->numBones; i++)
			{
				iw7_asset->boneNames[i] = static_cast<IW7::scr_string_t>(asset->boneNames[i]);
			}

			REINTERPRET_CAST_SAFE_TO_FROM(iw7_asset->parentList, asset->parentList);
			REINTERPRET_CAST_SAFE_TO_FROM(iw7_asset->tagAngles, asset->quats);
			REINTERPRET_CAST_SAFE_TO_FROM(iw7_asset->tagPositions, asset->trans);
			REINTERPRET_CAST_SAFE_TO_FROM(iw7_asset->partClassification, asset->partClassification);
			REINTERPRET_CAST_SAFE_TO_FROM(iw7_asset->baseMat, asset->baseMat);
			iw7_asset->reactiveMotionParts = nullptr;

			iw7_asset->materialHandles = mem.allocate<IW7::Material* __ptr64>(asset->numsurfs);
			for (auto i = 0; i < asset->numsurfs; i++)
			{
				if (asset->materialHandles[i])
				{
					iw7_asset->materialHandles[i] = mem.allocate<IW7::Material>();
					iw7_asset->materialHandles[i]->name = mem.duplicate_string(IW7::resolve_material_name(asset->materialHandles[i]->info.name));
				}
			}

			for (auto i = 0; i < 6; i++)
			{
				iw7_asset->lodInfo[i].dist = 1000000.0f;
			}

			// level of detail data
			for (auto i = 0; i < asset->numLods; i++)
			{
				iw7_asset->lodInfo[i].dist = asset->lodInfo[i].dist;
				iw7_asset->lodInfo[i].numsurfs = asset->lodInfo[i].numsurfs;
				iw7_asset->lodInfo[i].surfIndex = asset->lodInfo[i].surfIndex;
				iw7_asset->lodInfo[i].modelSurfs = mem.allocate<IW7::XModelSurfs>();
				iw7_asset->lodInfo[i].modelSurfs->name = mem.duplicate_string(asset->lodInfo[i].modelSurfs->name);
				memcpy(&iw7_asset->lodInfo[i].partBits, &asset->lodInfo[i].partBits, sizeof(asset->lodInfo[i].partBits));
			}

			iw7_asset->maxLoadedLod = asset->maxLoadedLod;
			iw7_asset->numLods = asset->numLods;
			iw7_asset->collLod = asset->collLod;
			iw7_asset->flags = asset->flags;

			iw7_asset->numCollSurfs = asset->numCollSurfs;
			iw7_asset->collSurfs = mem.allocate<IW7::XModelCollSurf_s>(asset->numCollSurfs);
			for (auto i = 0; i < asset->numCollSurfs; i++)
			{
				memcpy(&iw7_asset->collSurfs[i].bounds, &asset->collSurfs[i].bounds, sizeof(float[2][3]));

				iw7_asset->collSurfs[i].boneIdx = asset->collSurfs[i].boneIdx;
				iw7_asset->collSurfs[i].contents = asset->collSurfs[i].contents;
				iw7_asset->collSurfs[i].surfFlags = convert_surf_flags(asset->collSurfs[i].surfFlags);
			}

			iw7_asset->contents = asset->contents;

			REINTERPRET_CAST_SAFE_TO_FROM(iw7_asset->boneInfo, asset->boneInfo);

			iw7_asset->radius = asset->radius;
			memcpy(&iw7_asset->bounds, &asset->bounds, sizeof(asset->bounds));
			iw7_asset->memUsage = asset->memUsage;

			// Static model collision.
			//
			// IW7 takes it from XModel::physicsAsset, a per-model HavokPhysicsAsset wrapping a
			// static body. Without one the model collides with nothing: cStaticModel_s carries
			// only the XModel pointer, so there is nowhere else for the shape to come from.
			//
			// The source is what IW5 itself traces static models against: the collision LOD,
			// which the linker bakes into XModelCollSurf_s::collTris in model space. It is
			// authored rather than derived: grass clumps and other decoration have no collision
			// LOD (collLod is -1), so they get no physics asset at all. Vehicles carry their
			// collision only this way, with no PhysCollmap. A model without a collision LOD falls
			// back to its PhysCollmap, the volumes the physics system collides it with.
			//
			// Static models are shot-only. CoD only runs point traces -- bullets, sight, the
			// crosshair -- against static models; player movement is a box trace and never tests
			// them. That is why mappers clip their props: mp_test_h1 wraps a table that has its
			// own 124-triangle collision LOD in 20 clip brushes, and every palm trunk likewise.
			// IW7 works the same way. A model mesh carries no shape tags, so what it collides as
			// is the body's collisionFilterInfo, and 262 of the 263 stock static-body mesh assets
			// set it to 0x3180 -- CLIPSHOT | AI_NOSIGHT | ITEM | MISSILECLIP, no solid or player clip.
			// The few that do block players add PLAYERCLIP | MONSTERCLIP (0x32180: crates,
			// bumper cars). Solid here is what made the pail and cars block the player.
			//
			// Per-model blobs are CoD units / 32; see docs/iw7-havok-collision.md.
			iw7_asset->physicsAsset = nullptr;
			{
				constexpr auto model_scale = 0.03125f;
				constexpr auto contents_solid = 0x1;
				constexpr auto contents_foliage = 0x2;
				constexpr auto contents_static_prop = 0x3180u;
				// Diagnostic: ZT_MODELS_SOLID=1 gives props the player/monster clip bits stock
				// puts on the few props that do block (0x32180 -- crates, bumper cars), so a
				// model can be walked into sideways. That separates "sweeps never hit anything
				// in this zone" from "sweeps miss the world shape list specifically".
				const auto* solid_env = std::getenv("ZT_MODELS_SOLID");
				const auto body_contents = (solid_env && solid_env[0] == '1')
					? 0x32180u : contents_static_prop;

				ZoneTool::IW7::havok::builder::mesh_input mesh{};
				const char* source = nullptr;

				// Scales into Havok space, drops slivers, and winds counter-clockwise about
				// `normal` (the outward face normal) when one is given.
				const auto add_triangle = [&mesh](const float (&corners)[3][3], const float* normal,
					const int contents, const unsigned int material_crc, const unsigned short tag)
				{
					ZoneTool::IW7::havok::builder::triangle tri{};
					for (auto c = 0; c < 3; c++)
					{
						for (auto k = 0; k < 3; k++)
						{
							tri.verts[c][k] = corners[c][k] * model_scale;
						}
					}

					for (auto c = 0; c < 3; c++)
					{
						const auto n = (c + 1) % 3;
						if (tri.verts[c][0] == tri.verts[n][0] && tri.verts[c][1] == tri.verts[n][1]
							&& tri.verts[c][2] == tri.verts[n][2])
						{
							return;
						}
					}

					if (normal)
					{
						float e1[3], e2[3];
						for (auto k = 0; k < 3; k++)
						{
							e1[k] = tri.verts[1][k] - tri.verts[0][k];
							e2[k] = tri.verts[2][k] - tri.verts[0][k];
						}
						const float cross[3] = {
							e1[1] * e2[2] - e1[2] * e2[1],
							e1[2] * e2[0] - e1[0] * e2[2],
							e1[0] * e2[1] - e1[1] * e2[0],
						};
						if (cross[0] * normal[0] + cross[1] * normal[1] + cross[2] * normal[2] < 0.0f)
						{
							for (auto k = 0; k < 3; k++)
							{
								std::swap(tri.verts[1][k], tri.verts[2][k]);
							}
						}
					}

					tri.contents = contents;
					tri.material_crc = material_crc;
					tri.surface_tag = tag;
					mesh.triangles.emplace_back(tri);
				};

				if (asset->collSurfs && asset->numCollSurfs > 0)
				{
					for (auto i = 0; i < asset->numCollSurfs; i++)
					{
						const auto& surf = asset->collSurfs[i];

						// Pure foliage is something to push through, not stand against: the
						// potted plants' leaves are exactly this. Anything carrying solid or a
						// clip bit stays, with its own mask -- car windows are glass + clip.
						if (!surf.collTris || surf.numCollTris <= 0
							|| (surf.contents & ~contents_foliage) == 0)
						{
							continue;
						}

						source = "collision LOD";
						const auto material_crc = collision::iw7_material_crc(surf.surfFlags);

						for (auto t = 0; t < surf.numCollTris; t++)
						{
							// A collTri stores its triangle as three planes: the face plane
							// (normal, dist) and two edge planes that give barycentric
							// coordinates s = dot(svec, p) - svec[3] and t = dot(tvec, p) -
							// tvec[3], inside where s >= 0, t >= 0 and s + t <= 1. The corners
							// are where (s, t) is (0,0), (1,0) and (0,1) on the face plane.
							// Checked on com_pail_metal1: its 384 corners weld to 70 points
							// spanning the model; the opposite sign on w scatters them.
							const auto& ct = surf.collTris[t];
							const float* rows[3] = {ct.plane, ct.svec, ct.tvec};

							const auto cross = [](const float* a, const float* b, float(&out)[3])
							{
								out[0] = a[1] * b[2] - a[2] * b[1];
								out[1] = a[2] * b[0] - a[0] * b[2];
								out[2] = a[0] * b[1] - a[1] * b[0];
							};

							float c12[3], c20[3], c01[3];
							cross(rows[1], rows[2], c12);
							cross(rows[2], rows[0], c20);
							cross(rows[0], rows[1], c01);

							const auto det = rows[0][0] * c12[0] + rows[0][1] * c12[1]
								+ rows[0][2] * c12[2];
							if (std::fabs(det) < 1e-12f)
							{
								continue;
							}

							constexpr float barycentric[3][2] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}};
							float corners[3][3];
							for (auto c = 0; c < 3; c++)
							{
								const float rhs[3] = {
									ct.plane[3],
									barycentric[c][0] + ct.svec[3],
									barycentric[c][1] + ct.tvec[3],
								};
								for (auto k = 0; k < 3; k++)
								{
									corners[c][k] = (rhs[0] * c12[k] + rhs[1] * c20[k]
										+ rhs[2] * c01[k]) / det;
								}
							}

							add_triangle(corners, ct.plane, surf.contents, material_crc,
								static_cast<unsigned short>(i));
						}
					}
				}

				if (mesh.triangles.empty() && asset->physCollmap)
				{
					const auto hulls = collision::extract_phys_collmap(asset->physCollmap);
					const auto contents = asset->contents ? asset->contents : contents_solid;

					for (const auto& hull : hulls)
					{
						for (const auto& face : hull.faces)
						{
							// Fan the face polygon; a hull face is convex by construction and
							// already wound about its outward normal.
							for (std::size_t i = 2; i < face.indices.size(); i++)
							{
								const std::size_t corner[3] = {
									face.indices[0], face.indices[i - 1], face.indices[i]
								};
								if (corner[0] >= hull.verts.size() || corner[1] >= hull.verts.size()
									|| corner[2] >= hull.verts.size())
								{
									continue;
								}

								float corners[3][3];
								for (auto c = 0; c < 3; c++)
								{
									for (auto k = 0; k < 3; k++)
									{
										corners[c][k] = hull.verts[corner[c]][k];
									}
								}
								add_triangle(corners, nullptr, contents, 0x1AB7BC33u, 0);
							}
						}
					}

					source = "physCollmap";
				}

				if (mesh.triangles.empty())
				{
					if (source)
					{
						ZONETOOL_WARNING("XModel \"%s\": %s produced no triangles -- it will collide "
							"with nothing", asset->name, source);
					}
				}
				else
				{
					ZoneTool::IW7::havok::builder::physics_asset_input info{};
					info.body_name = asset->name;
					info.body_contents = body_contents;

					auto blob = ZoneTool::IW7::havok::builder::build_model_physics_asset(mesh, info);
					if (!blob.empty())
					{
						auto* physics = mem.allocate<IW7::PhysicsAsset>();
						physics->name = mem.duplicate_string(asset->name);
						physics->havokDataSize = static_cast<unsigned int>(blob.size());
						physics->havokData = mem.allocate<char>(
							static_cast<unsigned int>(blob.size()));
						std::memcpy(physics->havokData, blob.data(), blob.size());
						physics->numRigidBodies = 1;

						// The runtime indexes sfxEventAssets[i] and vfxEventAssets[i] and only
						// null-checks the element, never the array: sub_140573CE0 does
						//     v19 = *(QWORD*)(8 * i + *(QWORD*)(asset + 40));
						// so a null array reads address 8*i and faults. Every stock physics
						// asset carries numSFX/numVFX == 1 with a one-element array holding
						// null -- the dumps write "07 00", an asset record with existing = 0.
						physics->numSFXEventAssets = 1;
						physics->sfxEventAssets =
							mem.allocate<IW7::PhysicsSFXEventAsset PTR64>(1);
						physics->numVFXEventAssets = 1;
						physics->vfxEventAssets =
							mem.allocate<IW7::PhysicsVFXEventAsset PTR64>(1);

						iw7_asset->physicsAsset = physics;
					}
				}
			}

			if (asset->physCollmap)
			{
				// ?
				//iw7_asset->physFxShape = mem.allocate<IW7::PhysicsFXShape>();
				//iw7_asset->physFxShape->name = mem.duplicate_string(asset->physCollmap->name);
			}

			// idk
			iw7_asset->invHighMipRadius = mem.allocate<unsigned short>(asset->numsurfs);
			for (unsigned char i = 0; i < asset->numsurfs; i++)
			{
				iw7_asset->invHighMipRadius[i] = 0xFFFF;
			}

			//iw7_asset->quantization = 0.0f;

			iw7_asset->hasLods = asset->numLods ? 1 : 0;
			iw7_asset->shadowCutoffLod = 6;
			iw7_asset->characterCollBoundsType = 1; // CharCollBoundsType_Human

			iw7_asset->unknownIndex = 0xFF;
			iw7_asset->unknownIndex2 = 0xFF;

			//iw7_asset->flags |= 0x40;

			return iw7_asset;
		}

		IW7::XModel* convert(XModel* asset, allocator& allocator)
		{
			// generate IW7 model
			return GenerateIW7Model(asset, allocator);
		}
	}
}