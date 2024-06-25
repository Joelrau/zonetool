#include "stdafx.hpp"
#include "IW4/Assets/XModel.hpp"
#include "IW4/Assets/XSurface.hpp"

namespace ZoneTool
{
	namespace IW3
	{
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
				memcpy(&xmodel->lods[i].lod, &asset->lodInfo[i].lod, 3);

				if (xmodel->lods[i].numSurfacesInLod > 16)
				{
					xmodel->lods[i].numSurfacesInLod = 16;
					ZONETOOL_INFO("XModel %s has more than 16 surfaces in lod %d", asset->name, i);
				}

				// generate ModelSurface object
				xmodel->lods[i].surfaces = mem.allocate<IW4::XModelSurfs>();

				xmodel->lods[i].surfaces->name = mem.duplicate_string(va("%s_lod%d", xmodel->name, i).data());
				xmodel->lods[i].surfaces->numsurfs = xmodel->lods[i].numSurfacesInLod;
				memcpy(xmodel->lods[i].surfaces->partBits, asset->lodInfo[i].partBits, sizeof(int[4]));

				// allocate xsurficies
				xmodel->lods[i].surfaces->surfs = mem.allocate<IW4::XSurface>(xmodel->lods[i].numSurfacesInLod);

				// loop through surfaces in current Level-of-Detail
				for (int surf = 0; surf < xmodel->lods[i].numSurfacesInLod; surf++)
				{
					// generate iw4 surface
					const auto surface = GenerateIW4Surface(&asset->surfs[surf], &xmodel->lods[i].surfaces->surfs[surf], mem);
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
				memcpy(&xmodel->boneInfo[i].bounds, &asset->boneInfo[i].bounds, sizeof(Bounds));

				bounds::compute(asset->boneInfo[i].bounds[0], asset->boneInfo[i].bounds[1], &xmodel->boneInfo[i].packedBounds.midPoint);
				xmodel->boneInfo[i].radiusSquared = asset->boneInfo[i].radiusSquared;
			}

			xmodel->radius = asset->radius;
			xmodel->memUsage = asset->memUsage;
			xmodel->bad = asset->bad;
			xmodel->physPreset = reinterpret_cast<IW4::PhysPreset*>(asset->physPreset);

			bounds::compute(asset->mins, asset->maxs, &xmodel->bounds.midPoint);

			return xmodel;
		}

		void IXModel::dump(XModel* asset)
		{
			// generate iw4 model
			allocator allocator;
			auto iw4_model = GenerateIW4Model(asset, allocator);

			// dump model
			IW4::IXModel::dump(iw4_model, SL_ConvertToString);

			// dump all xsurfaces
			for (int i = 0; i < iw4_model->numLods; i++)
			{
				IW4::IXSurface::dump(iw4_model->lods[i].surfaces);
			}
		}
	}
}