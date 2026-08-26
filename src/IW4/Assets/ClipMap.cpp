#include "stdafx.hpp"
#include "IW5/Assets/ClipMap.hpp"

#include "MapEnts.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		IW5::clipMap_t* generate(clipMap_t* asset, allocator& mem)
		{
			auto* iw5_clipmap = mem.allocate<IW5::clipMap_t>();
			memset(iw5_clipmap, 0, sizeof IW5::clipMap_t);

			// convert clipmap to IW5 format
			iw5_clipmap->name = asset->name;
			iw5_clipmap->isInUse = asset->isInUse;

			iw5_clipmap->info.planeCount = asset->planeCount;
			iw5_clipmap->info.planes = (IW5::cplane_s*)asset->planes;

			iw5_clipmap->numStaticModels = asset->numStaticModels;
			iw5_clipmap->staticModelList = (IW5::cStaticModel_s*)asset->staticModelList;

			iw5_clipmap->info.numMaterials = asset->numMaterials;
			iw5_clipmap->info.materials = (IW5::ClipMaterial*)asset->materials;

			iw5_clipmap->info.numBrushSides = asset->numBrushSides;
			iw5_clipmap->info.brushsides = (IW5::cbrushside_t*)asset->brushsides;

			iw5_clipmap->info.numBrushEdges = asset->numBrushEdges;
			iw5_clipmap->info.brushEdges = (unsigned char*)asset->brushEdges;

			iw5_clipmap->numNodes = asset->numNodes;
			iw5_clipmap->nodes = (IW5::cNode_t*)asset->nodes;

			iw5_clipmap->numLeafs = asset->numLeafs;
			iw5_clipmap->leafs = (IW5::cLeaf_t*)asset->leafs;

			iw5_clipmap->info.leafbrushNodesCount = asset->leafbrushNodesCount;
			iw5_clipmap->info.leafbrushNodes = (IW5::cLeafBrushNode_s*)asset->leafbrushNodes;

			iw5_clipmap->info.numLeafBrushes = asset->numLeafBrushes;
			iw5_clipmap->info.leafbrushes = asset->leafbrushes;

			// leafSurfaces todo!

			iw5_clipmap->vertCount = asset->vertCount;
			iw5_clipmap->verts = (IW5::vec3_t*)asset->verts;

			iw5_clipmap->triCount = asset->triCount;
			iw5_clipmap->triIndices = asset->triIndices;
			iw5_clipmap->triEdgeIsWalkable = (unsigned char*)asset->triEdgeIsWalkable;

			iw5_clipmap->borderCount = asset->borderCount;
			iw5_clipmap->borders = (IW5::CollisionBorder*)asset->borders;

			iw5_clipmap->partitionCount = asset->partitionCount;
			iw5_clipmap->partitions = (IW5::CollisionPartition*)asset->partitions;

			iw5_clipmap->aabbTreeCount = asset->aabbTreeCount;
			iw5_clipmap->aabbTrees = (IW5::CollisionAabbTree*)asset->aabbTrees;

			// cmodels!
			iw5_clipmap->numSubModels = asset->numSubModels;
			iw5_clipmap->cmodels = mem.allocate<IW5::cmodel_t>(iw5_clipmap->numSubModels);
			memset(iw5_clipmap->cmodels, 0, sizeof(IW5::cmodel_t) * iw5_clipmap->numSubModels);

			for (int i = 0; i < iw5_clipmap->numSubModels; i++)
			{
				memcpy(&iw5_clipmap->cmodels[i].bounds, &asset->cmodels[i].bounds, sizeof(Bounds));
				iw5_clipmap->cmodels[i].radius = asset->cmodels[i].radius;
				memcpy(&iw5_clipmap->cmodels[i].leaf, &asset->cmodels[i].leaf, sizeof(cLeaf_t));
			}

			iw5_clipmap->info.numBrushes = asset->numBrushes;
			iw5_clipmap->info.brushes = (IW5::cbrush_t*)asset->brushes;
			iw5_clipmap->info.brushBounds = (IW5::Bounds*)asset->brushBounds;
			iw5_clipmap->info.brushContents = asset->brushContents;

			iw5_clipmap->mapEnts = IMapEnts::GenerateIW5MapEnts(asset->mapEnts, mem);
			iw5_clipmap->stageCount = asset->mapEnts->stageCount;
			iw5_clipmap->stages = (IW5::Stage*)asset->mapEnts->stages;

			iw5_clipmap->smodelNodeCount = asset->smodelNodeCount;
			iw5_clipmap->smodelNodes = (IW5::SModelAabbNode*)asset->smodelNodes;

			// dynEnts
			for (int i = 0; i < 2; i++)
			{
				iw5_clipmap->dynEntCount[i] = asset->dynEntCount[i];

				if (iw5_clipmap->dynEntCount[i] <= 0)
				{
					continue;
				}

				iw5_clipmap->dynEntDefList[i] = mem.allocate<IW5::DynEntityDef>(iw5_clipmap->dynEntCount[i]);
				iw5_clipmap->dynEntPoseList[i] = mem.allocate<IW5::DynEntityPose>(iw5_clipmap->dynEntCount[i]);
				iw5_clipmap->dynEntClientList[i] = mem.allocate<IW5::DynEntityClient>(iw5_clipmap->dynEntCount[i]);
				iw5_clipmap->dynEntCollList[i] = mem.allocate<IW5::DynEntityColl>(iw5_clipmap->dynEntCount[i]);

				for (int j = 0; j < iw5_clipmap->dynEntCount[i]; j++)
				{
					iw5_clipmap->dynEntDefList[i][j].type = (IW5::DynEntityType)asset->dynEntDefList[i][j].type;
					memcpy(&iw5_clipmap->dynEntDefList[i][j].pose, &asset->dynEntDefList[i][j].pose, sizeof(IW5::DynEntityPose));
					iw5_clipmap->dynEntDefList[i][j].xModel = (IW5::XModel*)asset->dynEntDefList[i][j].xModel;
					iw5_clipmap->dynEntDefList[i][j].brushModel = asset->dynEntDefList[i][j].brushModel;
					iw5_clipmap->dynEntDefList[i][j].physicsBrushModel = asset->dynEntDefList[i][j].physicsBrushModel;
					iw5_clipmap->dynEntDefList[i][j].destroyFx = (IW5::FxEffectDef*)asset->dynEntDefList[i][j].destroyFx;
					iw5_clipmap->dynEntDefList[i][j].physPreset = (IW5::PhysPreset*)asset->dynEntDefList[i][j].physPreset;
					iw5_clipmap->dynEntDefList[i][j].health = asset->dynEntDefList[i][j].health;
					iw5_clipmap->dynEntDefList[i][j].hinge = nullptr;
					memcpy(&iw5_clipmap->dynEntDefList[i][j].mass, &asset->dynEntDefList[i][j].mass, sizeof(IW5::PhysMass));
					iw5_clipmap->dynEntDefList[i][j].contents = asset->dynEntDefList[i][j].contents;

					//memcpy(&iw5_clipmap->dynEntPoseList[i][j], &asset->dynEntPoseList[i][j], sizeof(IW5::DynEntityPose));

					//memcpy(&iw5_clipmap->dynEntClientList[i][j], &asset->dynEntClientList[i][j], sizeof(IW5::DynEntityClient) - sizeof(int));
					//iw5_clipmap->dynEntClientList[i][j].hinge = 0;

					//memcpy(&iw5_clipmap->dynEntCollList[i][j], &asset->dynEntCollList[i][j], sizeof(IW5::DynEntityColl));
				}
			}

			return iw5_clipmap;
		}

		void IClipMap::dump(clipMap_t* asset)
		{
			allocator allocator;
			auto* iw5_clipmap = generate(asset, allocator);

			// dump clipmap
			IW5::IClipMap::dump(iw5_clipmap);
		}
	}
}