#include "stdafx.hpp"
#include "IW4/Assets/ClipMap.hpp"

namespace ZoneTool
{
	namespace IW3
	{
		IW4::clipMap_t* GenerateIW4ClipMap(clipMap_t* asset, allocator& mem)
		{
			auto* iw4_clipmap = mem.allocate<IW4::clipMap_t>();
			memset(iw4_clipmap, 0, sizeof IW4::clipMap_t);

			// convert clipmap to IW4 format
			iw4_clipmap->name = asset->name;
			iw4_clipmap->isInUse = asset->isInUse;

			iw4_clipmap->numCPlanes = asset->planeCount;
			iw4_clipmap->cPlanes = (IW4::cplane_s*)asset->planes;

			iw4_clipmap->numStaticModels = (int)asset->numStaticModels;
			iw4_clipmap->staticModelList = mem.allocate<IW4::cStaticModel_s>(iw4_clipmap->numStaticModels);
			for (unsigned int i = 0; i < asset->numStaticModels; ++i)
			{
				std::memcpy(&iw4_clipmap->staticModelList[i], &asset->staticModelList[i].xmodel,
					sizeof(IW4::cStaticModel_s));
				bounds::compute(asset->staticModelList[i].absmin, asset->staticModelList[i].absmax, &iw4_clipmap->staticModelList[i].absBounds.midPoint);
			}

			iw4_clipmap->numMaterials = (int)asset->numMaterials;
			iw4_clipmap->materials = mem.allocate<IW4::dmaterial_t>(iw4_clipmap->numMaterials);
			for (auto i = 0u; i < iw4_clipmap->numMaterials; i++)
			{
				iw4_clipmap->materials[i].material = (char*)mem.duplicate_string(asset->materials[i].material);
				iw4_clipmap->materials[i].contentFlags = asset->materials[i].contentFlags;
				iw4_clipmap->materials[i].surfaceFlags = asset->materials[i].surfaceFlags;
			}

			iw4_clipmap->numCBrushSides = (int)asset->numBrushSides;
			iw4_clipmap->cBrushSides = mem.allocate<IW4::cbrushside_t>(iw4_clipmap->numCBrushSides);

			std::unordered_map<cbrushside_t*, IW4::cbrushside_t*> mapped_brush_sides;

			for (unsigned int i = 0; i < asset->numBrushSides; ++i)
			{
				mapped_brush_sides[&asset->brushsides[i]] = &iw4_clipmap->cBrushSides[i];

				iw4_clipmap->cBrushSides[i].plane = (IW4::cplane_s*)asset->brushsides[i].plane;
				iw4_clipmap->cBrushSides[i].materialNum = asset->brushsides[i].materialNum;
				iw4_clipmap->cBrushSides[i].firstAdjacentSideOffset = (char)asset->brushsides[i].firstAdjacentSideOffset;
				iw4_clipmap->cBrushSides[i].edgeCount = asset->brushsides[i].edgeCount;
			}

			iw4_clipmap->numCBrushEdges = (int)asset->numBrushEdges;
			iw4_clipmap->cBrushEdges = (IW4::cbrushedge_t*)asset->brushEdges;

			iw4_clipmap->numCNodes = (int)asset->numNodes;
			iw4_clipmap->cNodes = (IW4::cNode_t*)asset->nodes;

			iw4_clipmap->numCLeaf = (int)asset->numLeafs;
			iw4_clipmap->cLeaf = mem.allocate<IW4::cLeaf_t>(iw4_clipmap->numCLeaf);
			for (unsigned int i = 0; i < asset->numLeafs; ++i)
			{
				std::memcpy(&iw4_clipmap->cLeaf[i], &asset->leafs[i], sizeof(IW4::cLeaf_t));
				bounds::compute(asset->leafs[i].mins, asset->leafs[i].maxs, & iw4_clipmap->cLeaf[i].bounds.midPoint);
			}

			iw4_clipmap->numCLeafBrushNodes = (int)asset->leafbrushNodesCount;
			iw4_clipmap->cLeafBrushNodes = (IW4::cLeafBrushNode_s*)asset->leafbrushNodes;

			iw4_clipmap->numLeafBrushes = (int)asset->numLeafBrushes;
			iw4_clipmap->leafBrushes = (short*)asset->leafbrushes;

			iw4_clipmap->numLeafSurfaces = (int)asset->numLeafSurfaces;
			iw4_clipmap->leafSurfaces = (int*)asset->leafsurfaces;

			iw4_clipmap->numVerts = (int)asset->vertCount;
			iw4_clipmap->verts = (IW4::VecInternal<3>*)asset->verts;

			iw4_clipmap->numTriIndices = asset->triCount;
			iw4_clipmap->triIndices = (short*)asset->triIndices;
			iw4_clipmap->triEdgeIsWalkable = asset->triEdgeIsWalkable;

			iw4_clipmap->numCollisionBorders = asset->borderCount;
			iw4_clipmap->collisionBorders = (IW4::CollisionBorder*)asset->borders;

			iw4_clipmap->numCollisionPartitions = asset->partitionCount;
			iw4_clipmap->collisionPartitions = (IW4::CollisionPartition*)asset->partitions;

			iw4_clipmap->numCollisionAABBTrees = asset->aabbTreeCount;
			iw4_clipmap->collisionAABBTrees = mem.allocate<IW4::CollisionAabbTree>(iw4_clipmap->numCollisionAABBTrees);
			for (int i = 0; i < asset->aabbTreeCount; ++i)
			{
				std::memcpy(&iw4_clipmap->collisionAABBTrees[i].midPoint, &asset->aabbTrees[i].origin, 12);
				std::memcpy(&iw4_clipmap->collisionAABBTrees[i].halfSize, &asset->aabbTrees[i].halfSize, 12);
				iw4_clipmap->collisionAABBTrees[i].materialIndex = asset->aabbTrees[i].materialIndex;
				iw4_clipmap->collisionAABBTrees[i].childCount = asset->aabbTrees[i].childCount;
				iw4_clipmap->collisionAABBTrees[i].u.firstChildIndex = asset->aabbTrees[i].u.firstChildIndex;
			}

			// cmodels!
			iw4_clipmap->numCModels = (int)asset->numSubModels;
			iw4_clipmap->cModels = mem.allocate<IW4::cmodel_t>(iw4_clipmap->numCModels);
			for (unsigned int i = 0; i < asset->numSubModels; ++i)
			{
				std::memcpy(&iw4_clipmap->cModels[i], &asset->cmodels[i], sizeof(IW4::cmodel_t));
				bounds::compute(asset->cmodels[i].mins, asset->cmodels[i].maxs, &iw4_clipmap->cModels[i].bounds.midPoint);
				bounds::compute(asset->cmodels[i].leaf.mins, asset->cmodels[i].leaf.maxs, &iw4_clipmap->cModels[i].leaf.bounds.midPoint);
			}

			iw4_clipmap->numBrushes = (short)asset->numBrushes;
			iw4_clipmap->brushes = mem.allocate<IW4::cbrush_t>(iw4_clipmap->numBrushes);
			iw4_clipmap->brushBounds = mem.allocate<IW4::Bounds>(iw4_clipmap->numBrushes);
			iw4_clipmap->brushContents = mem.allocate<int>(iw4_clipmap->numBrushes);
			for (unsigned int i = 0; i < asset->numBrushes; ++i)
			{
				std::memcpy(&iw4_clipmap->brushes[i].axialMaterialNum, &asset->brushes[i].axialMaterialNum,
					sizeof(iw4_clipmap->brushes[i].axialMaterialNum));
				std::memcpy(&iw4_clipmap->brushes[i].firstAdjacentSideOffsets,
					&asset->brushes[i].firstAdjacentSideOffsets,
					sizeof(iw4_clipmap->brushes[i].firstAdjacentSideOffsets));
				std::memcpy(&iw4_clipmap->brushes[i].edgeCount, &asset->brushes[i].edgeCount,
					sizeof(iw4_clipmap->brushes[i].edgeCount));

				iw4_clipmap->brushes[i].numsides = asset->brushes[i].numsides;
				if (asset->brushes[i].sides)
					iw4_clipmap->brushes[i].sides = mapped_brush_sides.find(asset->brushes[i].sides)->second;
				iw4_clipmap->brushes[i].edge = asset->brushes[i].baseAdjacentSide;
				iw4_clipmap->brushes[i].numsides = asset->brushes[i].numsides;

				bounds::compute(asset->brushes[i].mins, asset->brushes[i].maxs, &iw4_clipmap->brushBounds[i].midPoint);

				iw4_clipmap->brushContents[i] = asset->brushes[i].contents;
			}

			iw4_clipmap->smodelNodeCount = 1;
			iw4_clipmap->smodelNodes = mem.allocate<IW4::SModelAabbNode>();
			if (asset->numStaticModels == 0)
			{
				iw4_clipmap->smodelNodes[0].bounds.halfSize[0] = -131072.000f;
				iw4_clipmap->smodelNodes[0].bounds.halfSize[1] = -131072.000f;
				iw4_clipmap->smodelNodes[0].bounds.halfSize[2] = -131072.000f;
			}
			else
			{
				float maxs[3];
				float mins[3];

				maxs[0] = asset->staticModelList[0].absmax[0];
				maxs[1] = asset->staticModelList[1].absmax[1];
				maxs[2] = asset->staticModelList[2].absmax[2];

				mins[0] = asset->staticModelList[0].absmin[0];
				mins[1] = asset->staticModelList[1].absmin[1];
				mins[2] = asset->staticModelList[2].absmin[2];

				for (unsigned int i = 1; i < asset->numStaticModels; i++)
				{
					maxs[0] = std::max(maxs[0], asset->staticModelList[i].absmax[0]);
					maxs[1] = std::max(maxs[1], asset->staticModelList[i].absmax[1]);
					maxs[2] = std::max(maxs[2], asset->staticModelList[i].absmax[2]);

					mins[0] = std::min(mins[0], asset->staticModelList[i].absmin[0]);
					mins[1] = std::min(mins[1], asset->staticModelList[i].absmin[1]);
					mins[2] = std::min(mins[2], asset->staticModelList[i].absmin[2]);
				}

				bounds::compute(mins, maxs, &iw4_clipmap->smodelNodes[0].bounds.midPoint);
				iw4_clipmap->smodelNodes[0].childCount = static_cast<short>(asset->numStaticModels);
				iw4_clipmap->smodelNodes[0].firstChild = 0;
			}

			iw4_clipmap->mapEnts = mem.allocate<IW4::MapEnts>();
			iw4_clipmap->mapEnts->name = asset->mapEnts->name;
			iw4_clipmap->mapEnts->stageCount = 1;
			iw4_clipmap->mapEnts->stageNames = mem.allocate<IW4::Stage>();
			iw4_clipmap->mapEnts->stageNames[0].name = (char*)mem.duplicate_string("stage 0");
			iw4_clipmap->mapEnts->stageNames[0].triggerIndex = 0x400;
			iw4_clipmap->mapEnts->stageNames[0].sunPrimaryLightIndex = 0x1;

			auto* iw4_mapEnts = iw4_clipmap->mapEnts;

			// add triggers to mapEnts
			if (asset->cmodels)
			{
				iw4_mapEnts->trigger.count = asset->numSubModels;
				iw4_mapEnts->trigger.hullCount = asset->numSubModels;

				iw4_mapEnts->trigger.models = mem.allocate<IW4::TriggerModel>(asset->numSubModels);
				iw4_mapEnts->trigger.hulls = mem.allocate<IW4::TriggerHull>(asset->numSubModels);

				std::vector<IW4::TriggerSlab>* slabs = new std::vector<IW4::TriggerSlab>();

				for (int i = 0; i < asset->numSubModels; ++i)
				{
					IW4::TriggerHull* trigHull = &iw4_mapEnts->trigger.hulls[i];

					bounds::compute(asset->cmodels[i].mins, asset->cmodels[i].maxs, &trigHull->bounds.midPoint);
					trigHull->contents = asset->cmodels[i].leaf.brushContents | asset->cmodels[i].leaf.terrainContents;

					IW4::TriggerModel* trigMod = &iw4_mapEnts->trigger.models[i];
					trigMod->hullCount = 1;
					trigMod->firstHull = i;
					trigMod->contents = asset->cmodels[i].leaf.brushContents | asset->cmodels[i].leaf.terrainContents;

					auto* node = &asset->leafbrushNodes[asset->cmodels[i].leaf.leafBrushNode];
					if (node->leafBrushCount)
					{
						for (short j = 0; j < node->leafBrushCount; ++j)
						{
							auto* brush = &asset->brushes[node->data.leaf.brushes[j]];

							auto baseSlab = slabs->size();
							for (unsigned int k = 0; k < brush->numsides; ++k)
							{
								IW4::TriggerSlab curSlab;
								curSlab.dir[0] = brush->sides[k].plane->normal[0];
								curSlab.dir[1] = brush->sides[k].plane->normal[1];
								curSlab.dir[2] = brush->sides[k].plane->normal[2];
								curSlab.halfSize = brush->sides[k].plane->dist;
								curSlab.midPoint = 0.0f; // ??

								slabs->push_back(curSlab);
							}

							trigHull->firstSlab = (unsigned short)baseSlab;
							trigHull->slabCount = (unsigned short)(i - baseSlab);
						}
					}
				}

				iw4_mapEnts->trigger.slabs = mem.allocate<IW4::TriggerSlab>(slabs->size());

				// Save slabs
				iw4_mapEnts->trigger.slabCount = slabs->size();
				for (unsigned int i = 0; i < slabs->size(); i++)
				{
					IW4::TriggerSlab slab = (*slabs)[i];
					std::memcpy(&iw4_mapEnts->trigger.slabs[i], &slab, sizeof(IW4::TriggerSlab));
				}

				delete slabs;
			}

			assetmanager::dumper trigger_dumper;
			if (trigger_dumper.open(iw4_mapEnts->name + ".ents.triggers"s))
			{
				trigger_dumper.dump_int(iw4_mapEnts->trigger.count);
				trigger_dumper.dump_array<IW4::TriggerModel>(iw4_mapEnts->trigger.models, iw4_mapEnts->trigger.count);

				trigger_dumper.dump_int(iw4_mapEnts->trigger.hullCount);
				trigger_dumper.dump_array<IW4::TriggerHull>(iw4_mapEnts->trigger.hulls, iw4_mapEnts->trigger.hullCount);

				trigger_dumper.dump_int(iw4_mapEnts->trigger.slabCount);
				trigger_dumper.dump_array<IW4::TriggerSlab>(iw4_mapEnts->trigger.slabs, iw4_mapEnts->trigger.slabCount);

				trigger_dumper.close();
			}

			// dynEnts
			for (int i = 0; i < 2; i++)
			{
				iw4_clipmap->dynEntCount[i] = asset->dynEntCount[i];

				if (iw4_clipmap->dynEntCount[i] <= 0)
				{
					continue;
				}

				iw4_clipmap->dynEntDefList[i] = mem.allocate<IW4::DynEntityDef>(iw4_clipmap->dynEntCount[i]);
				iw4_clipmap->dynEntPoseList[i] = mem.allocate<IW4::DynEntityPose>(iw4_clipmap->dynEntCount[i]);
				iw4_clipmap->dynEntClientList[i] = mem.allocate<IW4::DynEntityClient>(iw4_clipmap->dynEntCount[i]);
				iw4_clipmap->dynEntCollList[i] = mem.allocate<IW4::DynEntityColl>(iw4_clipmap->dynEntCount[i]);

				for (int j = 0; j < iw4_clipmap->dynEntCount[i]; j++)
				{
					iw4_clipmap->dynEntDefList[i][j].type = (IW4::DynEntityType)asset->dynEntDefList[i][j].type;
					memcpy(&iw4_clipmap->dynEntDefList[i][j].pose, &asset->dynEntDefList[i][j].pose, sizeof(IW4::DynEntityPose));
					iw4_clipmap->dynEntDefList[i][j].xModel = (IW4::XModel*)asset->dynEntDefList[i][j].xModel;
					iw4_clipmap->dynEntDefList[i][j].brushModel = asset->dynEntDefList[i][j].brushModel;
					iw4_clipmap->dynEntDefList[i][j].physicsBrushModel = asset->dynEntDefList[i][j].physicsBrushModel;
					iw4_clipmap->dynEntDefList[i][j].destroyFx = (IW4::FxEffectDef*)asset->dynEntDefList[i][j].destroyFx;
					iw4_clipmap->dynEntDefList[i][j].physPreset = (IW4::PhysPreset*)asset->dynEntDefList[i][j].physPreset;
					iw4_clipmap->dynEntDefList[i][j].health = asset->dynEntDefList[i][j].health;
					memcpy(&iw4_clipmap->dynEntDefList[i][j].mass, &asset->dynEntDefList[i][j].mass, sizeof(IW4::PhysMass));
					iw4_clipmap->dynEntDefList[i][j].contents = asset->dynEntDefList[i][j].contents;

					memcpy(&iw4_clipmap->dynEntPoseList[i][j], &asset->dynEntPoseList[i][j], sizeof(IW4::DynEntityPose));
					memcpy(&iw4_clipmap->dynEntClientList[i][j], &asset->dynEntClientList[i][j], sizeof(IW4::DynEntityClient));
					memcpy(&iw4_clipmap->dynEntCollList[i][j], &asset->dynEntCollList[i][j], sizeof(IW4::DynEntityColl));
				}
			}

			iw4_clipmap->checksum = asset->checksum;

			return iw4_clipmap;
		}

		void IClipMap::dump(clipMap_t* asset)
		{
			// generate clipmap
			allocator allocator;
			auto iw4_asset = GenerateIW4ClipMap(asset, allocator);

			// dump clipmap
			IW4::IClipMap::dump(iw4_asset);
		}
	}
}