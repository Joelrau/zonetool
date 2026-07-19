#include "stdafx.hpp"
#include "IW4/Assets/XModel.hpp"
#include "IW4/Assets/XSurface.hpp"
#include "IW5/Assets/PhysCollmap.hpp"

#include <cfloat>
#include <cmath>

namespace ZoneTool
{
	namespace IW3
	{
		namespace
		{
			IW5::cplane_s convert_plane(const cplane_s& src)
			{
				IW5::cplane_s dst{};
				dst.normal[0] = src.normal[0];
				dst.normal[1] = src.normal[1];
				dst.normal[2] = src.normal[2];
				dst.dist = src.dist;
				dst.type = static_cast<unsigned char>(src.type);
				dst.pad[0] = 0;
				dst.pad[1] = 0;
				dst.pad[2] = 0;
				return dst;
			}

			// IW3 PhysGeomInfo::type is a raw int, but pretty much maps out to:
			//   NONE=0, BOX=1, BRUSHMODEL=2, BRUSH=3, CYLINDER=4, CAPSULE=5.
			// IW5 inserts COLLMAP=4, shifting CYLINDER->5 and CAPSULE->6.
			IW5::PhysicsGeomType remap_geom_type(int type, const char* model_name)
			{
				switch (type)
				{
				case 0: return IW5::PHYS_GEOM_NONE;
				case 1: return IW5::PHYS_GEOM_BOX;
				case 2: return IW5::PHYS_GEOM_BRUSHMODEL;
				case 3: return IW5::PHYS_GEOM_BRUSH;
				case 4: return IW5::PHYS_GEOM_CYLINDER;
				case 5: return IW5::PHYS_GEOM_CAPSULE;
				default:
					ZONETOOL_WARNING("XModel \"%s\": unknown IW3 phys geom type %d, passing through",
						model_name, type);
					return static_cast<IW5::PhysicsGeomType>(type);
				}
			}

			IW5::BrushWrapper* convert_brush_wrapper(BrushWrapper* src, const char* model_name, allocator& mem)
			{
				auto* dst = mem.allocate<IW5::BrushWrapper>();

				bounds::compute(src->mins, src->maxs, &dst->bounds.midPoint);

				const unsigned int numsides = src->numsides;
				if (numsides > 0xFFFF)
				{
					ZONETOOL_WARNING("XModel \"%s\": brush numsides %u exceeds 0xFFFF, clamping",
						model_name, numsides);
				}
				const unsigned int side_count = numsides > 0xFFFF ? 0xFFFF : numsides;

				dst->brush.numsides = static_cast<unsigned short>(side_count);
				dst->brush.glassPieceIndex = 0;

				// convert planes (numsides entries)
				IW5::cplane_s* new_planes = nullptr;
				if (side_count)
				{
					new_planes = mem.allocate<IW5::cplane_s>(side_count);
					for (unsigned int i = 0; i < side_count; i++)
					{
						if (src->planes)
						{
							new_planes[i] = convert_plane(src->planes[i]);
						}
					}
				}
				dst->planes = new_planes;

				// convert sides
				if (side_count && src->sides)
				{
					dst->brush.sides = mem.allocate<IW5::cbrushside_t>(side_count);
					for (unsigned int i = 0; i < side_count; i++)
					{
						const cbrushside_t& s = src->sides[i];
						IW5::cbrushside_t& d = dst->brush.sides[i];

						// plane must point into the NEW planes array at the same index the
						// old side's plane had in the old planes array.
						if (s.plane)
						{
							const ptrdiff_t idx = src->planes ? (s.plane - src->planes) : -1;
							if (src->planes && new_planes && idx >= 0 &&
								idx < static_cast<ptrdiff_t>(side_count))
							{
								d.plane = &new_planes[idx];
							}
							else
							{
								ZONETOOL_WARNING("XModel \"%s\": brush side %u plane index out of range, "
									"allocating standalone plane", model_name, i);
								auto* p = mem.allocate<IW5::cplane_s>();
								*p = convert_plane(*s.plane);
								d.plane = p;
							}
						}

						d.materialNum = static_cast<unsigned short>(s.materialNum);

						if (s.firstAdjacentSideOffset < 0 || s.firstAdjacentSideOffset > 255)
						{
							ZONETOOL_WARNING("XModel \"%s\": brush side %u firstAdjacentSideOffset %d "
								"out of unsigned char range, clamping to 255", model_name, i,
								s.firstAdjacentSideOffset);
							d.firstAdjacentSideOffset = 255;
						}
						else
						{
							d.firstAdjacentSideOffset = static_cast<unsigned char>(s.firstAdjacentSideOffset);
						}

						d.edgeCount = static_cast<unsigned char>(s.edgeCount);
					}
				}

				// baseAdjacentSide: copy totalEdgeCount bytes
				dst->totalEdgeCount = src->totalEdgeCount;
				if (src->totalEdgeCount > 0 && src->baseAdjacentSide)
				{
					auto* bas = mem.allocate<unsigned char>(src->totalEdgeCount);
					memcpy(bas, src->baseAdjacentSide, src->totalEdgeCount);
					dst->brush.baseAdjacentSide = bas;
				}

				// axialMaterialNum[2][3]: both short, straight copy
				for (int a = 0; a < 2; a++)
				{
					for (int b = 0; b < 3; b++)
					{
						dst->brush.axialMaterialNum[a][b] = src->axialMaterialNum[a][b];
					}
				}

				// firstAdjacentSideOffsets[2][3]: __int16 -> unsigned char with clamp
				for (int a = 0; a < 2; a++)
				{
					for (int b = 0; b < 3; b++)
					{
						const __int16 v = src->firstAdjacentSideOffsets[a][b];
						if (v < 0 || v > 255)
						{
							ZONETOOL_WARNING("XModel \"%s\": brush firstAdjacentSideOffsets[%d][%d] %d "
								"out of unsigned char range, clamping to 255", model_name, a, b, v);
							dst->brush.firstAdjacentSideOffsets[a][b] = 255;
						}
						else
						{
							dst->brush.firstAdjacentSideOffsets[a][b] = static_cast<unsigned char>(v);
						}
					}
				}

				// edgeCount[2][3]: straight copy
				for (int a = 0; a < 2; a++)
				{
					for (int b = 0; b < 3; b++)
					{
						dst->brush.edgeCount[a][b] = static_cast<unsigned char>(src->edgeCount[a][b]);
					}
				}

				return dst;
			}

			IW5::PhysCollmap* GenerateIW5PhysCollmap(XModel* asset, allocator& mem)
			{
				auto* src = asset->physGeoms;
				if (!src || !src->count)
				{
					return nullptr;
				}

				auto* collmap = mem.allocate<IW5::PhysCollmap>();
				collmap->name = mem.duplicate_string(asset->name);
				collmap->count = src->count;
				collmap->geoms = mem.allocate<IW5::PhysGeomInfo>(src->count);

				// IW3 and IW5 PhysMass share an identical layout (3x vec3 floats).
				memcpy(&collmap->mass, &src->mass, sizeof(IW5::PhysMass));

				// IW3/ODE authors inertia about the COM; Domino expects it about the model origin (parallel-axis shift, per unit mass)
				{
					const float cx = collmap->mass.centerOfMass[0];
					const float cy = collmap->mass.centerOfMass[1];
					const float cz = collmap->mass.centerOfMass[2];
					collmap->mass.momentsOfInertia[0] += cy * cy + cz * cz;
					collmap->mass.momentsOfInertia[1] += cx * cx + cz * cz;
					collmap->mass.momentsOfInertia[2] += cx * cx + cy * cy;
					collmap->mass.productsOfInertia[0] -= cx * cy;
					collmap->mass.productsOfInertia[1] -= cx * cz;
					collmap->mass.productsOfInertia[2] -= cy * cz;
				}

				float mins[3] = { FLT_MAX, FLT_MAX, FLT_MAX };
				float maxs[3] = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

				for (unsigned int i = 0; i < src->count; i++)
				{
					const PhysGeomInfo& g = src->geoms[i];
					IW5::PhysGeomInfo& d = collmap->geoms[i];

					d.brushWrapper = g.brush ? convert_brush_wrapper(g.brush, asset->name, mem) : nullptr;
					d.type = remap_geom_type(g.type, asset->name);
					memcpy(d.orientation, g.orientation, sizeof(float[3][3]));

					d.bounds.midPoint[0] = g.offset[0];
					d.bounds.midPoint[1] = g.offset[1];
					d.bounds.midPoint[2] = g.offset[2];
					d.bounds.halfSize[0] = g.halfLengths[0];
					d.bounds.halfSize[1] = g.halfLengths[1];
					d.bounds.halfSize[2] = g.halfLengths[2];

					// IW3 packs cylinder/capsule as (halfHeight, radius, unused) with the symmetry axis along geom-local X (orientation maps it to model-up)
					if ((d.type == IW5::PHYS_GEOM_CYLINDER || d.type == IW5::PHYS_GEOM_CAPSULE)
						&& g.halfLengths[2] == 0.0f)
					{
						const float halfHeight = g.halfLengths[0];
						const float radius = g.halfLengths[1];
						d.bounds.halfSize[0] = halfHeight;
						d.bounds.halfSize[1] = radius;
						d.bounds.halfSize[2] = radius;
					}

					for (int a = 0; a < 3; a++)
					{
						// model-space extent of the oriented local box
						const float ext =
							std::fabs(d.orientation[a][0]) * d.bounds.halfSize[0] +
							std::fabs(d.orientation[a][1]) * d.bounds.halfSize[1] +
							std::fabs(d.orientation[a][2]) * d.bounds.halfSize[2];
						const float lo = d.bounds.midPoint[a] - ext;
						const float hi = d.bounds.midPoint[a] + ext;
						if (lo < mins[a]) mins[a] = lo;
						if (hi > maxs[a]) maxs[a] = hi;
					}
				}

				// collmap bounds = union of all geom bounds
				bounds::compute(mins, maxs, &collmap->bounds.midPoint);

				return collmap;
			}
		}

		IW4::XSurface* GenerateIW4Surface(XSurface* asset, IW4::XSurface* xsurface, allocator& mem)
		{
			xsurface->tileMode = asset->tileMode;
			xsurface->deformed = asset->deformed;
			xsurface->vertCount = asset->vertCount;
			xsurface->triCount = asset->triCount;
			xsurface->zoneHandle = 0;
			xsurface->baseTriIndex = asset->baseTriIndex;
			xsurface->baseVertIndex = asset->baseVertIndex;
			xsurface->triIndices = reinterpret_cast<IW4::Face*>(asset->triIndices);
			memcpy(&xsurface->vertexInfo, &asset->vertInfo, sizeof IW4::XSurfaceVertexInfo);
			xsurface->verticies = reinterpret_cast<IW4::GfxPackedVertex*>(asset->verts0);
			xsurface->vertListCount = asset->vertListCount;
			xsurface->rigidVertLists = reinterpret_cast<IW4::XRigidVertList*>(asset->vertList);
			memcpy(&xsurface->partBits, &asset->partBits, sizeof(int[4]));

			return xsurface;
		}

		IW4::XModel* GenerateIW4Model(XModel* asset, allocator& mem)
		{
			// allocate IW4 XModel structure
			const auto xmodel = mem.allocate<IW4::XModel>();

			// copy data over
			xmodel->name = const_cast<char*>(asset->name);
			xmodel->numBones = asset->numBones;
			xmodel->numRootBones = asset->numRootBones;
			xmodel->numSurfaces = asset->numsurfs;
			xmodel->lodRampType = asset->lodRampType;
			xmodel->scale = 1.0f;
			memset(xmodel->noScalePartBits, 0, sizeof(int) * 6);
			xmodel->parentList = reinterpret_cast<unsigned char*>(asset->parentList);
			xmodel->boneNames = reinterpret_cast<short*>(asset->boneNames);

			xmodel->tagAngles = reinterpret_cast<IW4::XModelAngle*>(asset->quats);
			xmodel->tagPositions = reinterpret_cast<IW4::XModelTagPos*>(asset->trans);

			xmodel->partClassification = asset->partClassification;
			xmodel->animMatrix = reinterpret_cast<IW4::DObjAnimMat*>(asset->baseMat);
			xmodel->materials = reinterpret_cast<IW4::Material**>(asset->materialHandles);

			// convert level of detail data
			for (int i = 0; i < asset->numLods; i++)
			{
				xmodel->lods[i].dist = asset->lodInfo[i].dist;
				xmodel->lods[i].numSurfacesInLod = asset->lodInfo[i].numsurfs;
				xmodel->lods[i].surfIndex = asset->lodInfo[i].surfIndex;
				memcpy(xmodel->lods[i].partBits, asset->lodInfo[i].partBits, sizeof(int[4]));
				xmodel->lods[i].lod = asset->lodInfo[i].lod;
				xmodel->lods[i].smcBaseIndexPlusOne = 0;
				xmodel->lods[i].smcSubIndexMask = 0;
				xmodel->lods[i].smcBucket = 0;

				// this is only necessary if the xmodel is used as a static model...
				//if (xmodel->lods[i].numSurfacesInLod > 16)
				//{
				//	xmodel->lods[i].numSurfacesInLod = 16;
				//	ZONETOOL_INFO("XModel %s has more than 16 surfaces in lod %d", asset->name, i);
				//}

				// generate ModelSurface object
				xmodel->lods[i].surfaces = mem.allocate<IW4::XModelSurfs>();

				xmodel->lods[i].surfaces->name = mem.duplicate_string(va("%s_lod%d", xmodel->name, i).data());
				xmodel->lods[i].surfaces->numsurfs = xmodel->lods[i].numSurfacesInLod;
				memcpy(xmodel->lods[i].surfaces->partBits, asset->lodInfo[i].partBits, sizeof(int[4]));

				// allocate xsurficies
				xmodel->lods[i].surfaces->surfs = mem.allocate<IW4::XSurface>(xmodel->lods[i].numSurfacesInLod);

				// loop through surfaces in current Level-of-Detail
				int surfIndex = asset->lodInfo[i].surfIndex;
				for (int surf = 0; surf < xmodel->lods[i].numSurfacesInLod; surf++)
				{
					// generate iw4 surface
					const auto surface = GenerateIW4Surface(&asset->surfs[surfIndex + surf], &xmodel->lods[i].surfaces->surfs[surf], mem);
				}
			}

			xmodel->numLods = asset->numLods;
			xmodel->collLod = asset->collLod;
			xmodel->flags = asset->flags;

			xmodel->collSurfs = reinterpret_cast<IW4::XModelCollSurf_s*>(asset->collSurfs);
			xmodel->numColSurfs = asset->numCollSurfs;
			xmodel->contents = asset->contents;

			// convert colsurf bounds
			for (int i = 0; i < xmodel->numColSurfs; i++)
			{
				bounds::compute(asset->collSurfs[i].mins, asset->collSurfs[i].maxs, &xmodel->collSurfs[i].bounds.midPoint);
			}

			// convert boneinfo
			xmodel->boneInfo = mem.allocate<IW4::XBoneInfo>(xmodel->numBones);
			for (int i = 0; i < xmodel->numBones; i++)
			{
				IW4::XBoneInfo* target = &xmodel->boneInfo[i];
				XBoneInfo* source = &asset->boneInfo[i];

				target->radiusSquared = source->radiusSquared;

				target->packedBounds.compute(source->bounds[0], source->bounds[1]);
				target->packedBounds.midPoint[0] += source->offset[0];
				target->packedBounds.midPoint[1] += source->offset[1];
				target->packedBounds.midPoint[2] += source->offset[2];
			}

			xmodel->radius = asset->radius;
			xmodel->memUsage = asset->memUsage;
			xmodel->bad = asset->bad;
			xmodel->physPreset = reinterpret_cast<IW4::PhysPreset*>(asset->physPreset);

			// create a physcollmap asset for IW5 based off XModel physGeoms
			if (asset->physGeoms && asset->physGeoms->count)
			{
				auto* iw5_collmap = GenerateIW5PhysCollmap(asset, mem);
				xmodel->physCollmap = reinterpret_cast<IW4::PhysCollmap*>(iw5_collmap);
			}

			bounds::compute(asset->mins, asset->maxs, &xmodel->bounds.midPoint);

			return xmodel;
		}

		void IXModel::dump(XModel* asset)
		{
			// generate iw4 model
			allocator allocator;
			auto iw4_model = GenerateIW4Model(asset, allocator);

			// dump model
			IW4::IXModel::dump(iw4_model);

			if (iw4_model->physCollmap)
			{
				IW5::IPhysCollmap::dump(reinterpret_cast<IW5::PhysCollmap*>(iw4_model->physCollmap));
			}

			// dump all xsurfaces
			for (int i = 0; i < iw4_model->numLods; i++)
			{
				IW4::IXSurface::dump(iw4_model->lods[i].surfaces);
			}
		}
	}
}