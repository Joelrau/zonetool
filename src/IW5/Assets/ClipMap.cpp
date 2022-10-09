#include "stdafx.hpp"
#include "H1/Assets/ClipMap.hpp"

#include "H1/Assets/PhysWorld.hpp"

namespace ZoneTool
{
	namespace IW5
	{
		void GenerateH1ClipInfo(H1::ClipInfo* info, clipMap_t* asset, ZoneMemory* mem)
		{
			info->planeCount = asset->info.numCPlanes;
			REINTERPRET_CAST_SAFE(info->planes, asset->info.cPlanes);

			info->numMaterials = asset->info.numMaterials;
			info->materials = mem->Alloc<H1::ClipMaterial>(info->numMaterials);
			for (unsigned int i = 0; i < info->numMaterials; i++)
			{
				info->materials[i].name = asset->info.materials[i].material;
				info->materials[i].surfaceFlags = asset->info.materials[i].surfaceFlags; // convert?
				info->materials[i].contents = asset->info.materials[i].contentFlags;
			}

			info->bCollisionTree.leafbrushNodesCount = asset->info.numCLeafBrushNodes;
			info->bCollisionTree.leafbrushNodes = mem->Alloc<H1::cLeafBrushNode_s>(info->bCollisionTree.leafbrushNodesCount);
			for (unsigned int i = 0; i < info->bCollisionTree.leafbrushNodesCount; i++)
			{
				info->bCollisionTree.leafbrushNodes[i].axis = asset->info.cLeafBrushNodes[i].axis;
				info->bCollisionTree.leafbrushNodes[i].leafBrushCount = asset->info.cLeafBrushNodes[i].leafBrushCount;
				info->bCollisionTree.leafbrushNodes[i].contents = asset->info.cLeafBrushNodes[i].contents;
				if (info->bCollisionTree.leafbrushNodes[i].leafBrushCount > 0)
				{
					REINTERPRET_CAST_SAFE(info->bCollisionTree.leafbrushNodes[i].data.leaf.brushes, asset->info.cLeafBrushNodes[i].data.leaf.brushes);
				}
				else
				{
					// could be wrong
					memcpy(&info->bCollisionTree.leafbrushNodes[i].data.children, &asset->info.cLeafBrushNodes[i].data.children,
						sizeof(asset->info.cLeafBrushNodes[i].data.children));
				}
			}

			info->bCollisionTree.numLeafBrushes = asset->info.numLeafBrushes;
			REINTERPRET_CAST_SAFE(info->bCollisionTree.leafbrushes, asset->info.leafBrushes);

			info->pCollisionTree.aabbTreeCount = asset->numCollisionAABBTrees;
			REINTERPRET_CAST_SAFE(info->pCollisionTree.aabbTrees, asset->collisionAABBTrees); // origin -> midPoint

			info->sCollisionTree.numStaticModels = asset->numStaticModels;
			info->sCollisionTree.smodelNodeCount = asset->smodelNodeCount;
			REINTERPRET_CAST_SAFE(info->sCollisionTree.smodelNodes, asset->smodelNodes);

			std::unordered_map<cbrushside_t*, H1::cbrushside_t*> mapped_brush_sides;

			info->bCollisionData.numBrushSides = asset->info.numCBrushSides;
			info->bCollisionData.brushSides = mem->Alloc<H1::cbrushside_t>(info->bCollisionData.numBrushSides);
			for (unsigned int i = 0; i < info->bCollisionData.numBrushSides; i++)
			{
				mapped_brush_sides[&asset->info.cBrushSides[i]] = &info->bCollisionData.brushSides[i];

				// check
				info->bCollisionData.brushSides[i].planeIndex = 
					(reinterpret_cast<std::uintptr_t>(asset->info.cBrushSides[i].plane) - reinterpret_cast<std::uintptr_t>(asset->info.cPlanes)) / sizeof(cplane_s);
				assert(info->bCollisionData.brushSides[i].planeIndex <= info->planeCount);

				info->bCollisionData.brushSides[i].materialNum = asset->info.cBrushSides[i].materialNum;
				info->bCollisionData.brushSides[i].firstAdjacentSideOffset = asset->info.cBrushSides[i].firstAdjacentSideOffset;
				info->bCollisionData.brushSides[i].edgeCount = asset->info.cBrushSides[i].edgeCount;
			}

			info->bCollisionData.numBrushEdges = asset->info.numCBrushEdges;
			REINTERPRET_CAST_SAFE(info->bCollisionData.brushEdges, asset->info.cBrushEdges);

			info->bCollisionData.numBrushes = asset->info.numBrushes;
			info->bCollisionData.brushes = mem->Alloc<H1::cbrush_t>(asset->info.numBrushes);
			for (unsigned int i = 0; i < info->bCollisionData.numBrushes; i++)
			{
				info->bCollisionData.brushes[i].numsides = asset->info.brushes[i].numsides;
				info->bCollisionData.brushes[i].glassPieceIndex = asset->info.brushes[i].glassPieceIndex;

				if (asset->info.brushes[i].sides)
					info->bCollisionData.brushes[i].sides = mapped_brush_sides.find(asset->info.brushes[i].sides)->second;

				REINTERPRET_CAST_SAFE(info->bCollisionData.brushes[i].baseAdjacentSide, asset->info.brushes[i].edge);

				memcpy(&info->bCollisionData.brushes[i].axialMaterialNum, &asset->info.brushes[i].axialMaterialNum, sizeof(short[2][3]));
				memcpy(&info->bCollisionData.brushes[i].firstAdjacentSideOffsets, &asset->info.brushes[i].firstAdjacentSideOffsets, sizeof(char[2][3]));
				memcpy(&info->bCollisionData.brushes[i].edgeCount, &asset->info.brushes[i].edgeCount, sizeof(char[2][3]));
			}

			REINTERPRET_CAST_SAFE(info->bCollisionData.brushBounds, asset->info.brushBounds);
			REINTERPRET_CAST_SAFE(info->bCollisionData.brushContents, asset->info.brushContents);

			info->pCollisionData.vertCount = asset->numVerts;
			REINTERPRET_CAST_SAFE(info->pCollisionData.verts, asset->verts);
			info->pCollisionData.triCount = asset->numTriIndices;
			REINTERPRET_CAST_SAFE(info->pCollisionData.triIndices, asset->triIndices);
			REINTERPRET_CAST_SAFE(info->pCollisionData.triEdgeIsWalkable, asset->triEdgeIsWalkable);
			info->pCollisionData.borderCount = asset->numCollisionBorders;
			REINTERPRET_CAST_SAFE(info->pCollisionData.borders, asset->collisionBorders);
			info->pCollisionData.partitionCount = asset->numCollisionPartitions;
			info->pCollisionData.partitions = mem->Alloc<H1::CollisionPartition>(info->pCollisionData.partitionCount);
			for (int i = 0; i < info->pCollisionData.partitionCount; i++)
			{
				info->pCollisionData.partitions[i].triCount = asset->collisionPartitions[i].triCount;
				info->pCollisionData.partitions[i].borderCount = asset->collisionPartitions[i].borderCount;
				info->pCollisionData.partitions[i].firstVertSegment = 0;
				info->pCollisionData.partitions[i].firstTri = asset->collisionPartitions[i].firstTri;
				REINTERPRET_CAST_SAFE(info->pCollisionData.partitions[i].borders, asset->collisionPartitions[i].borders);
			}

			info->sCollisionData.numStaticModels = asset->numStaticModels;
			info->sCollisionData.staticModelList = mem->Alloc<H1::cStaticModel_s>(info->sCollisionData.numStaticModels);
			for (unsigned int i = 0; i < info->sCollisionData.numStaticModels; i++)
			{
				info->sCollisionData.staticModelList[i].xmodel = reinterpret_cast<H1::XModel * __ptr64>(asset->staticModelList[i].xmodel);

				memcpy(&info->sCollisionData.staticModelList[i].origin, &asset->staticModelList[i].origin, sizeof(float[3]));
				memcpy(&info->sCollisionData.staticModelList[i].invScaledAxis, &asset->staticModelList[i].invScaledAxis, sizeof(float[3][3]));
				memcpy(&info->sCollisionData.staticModelList[i].absBounds, &asset->staticModelList[i].absmin, sizeof(float[2][3]));
				//info->sCollisionData.staticModelList[i].contents = asset->staticModelList[i].xmodel->contents;
			}
		}

		H1::clipMap_t* GenerateH1ClipMap(clipMap_t* asset, ZoneMemory* mem)
		{
			// allocate H1 clipMap_t structure
			const auto h1_asset = mem->Alloc<H1::clipMap_t>();

			h1_asset->name = asset->name;
			h1_asset->isInUse = asset->isInUse;

			GenerateH1ClipInfo(&h1_asset->info, asset, mem);
			h1_asset->pInfo = &h1_asset->info;

			h1_asset->numNodes = asset->numCNodes;
			h1_asset->nodes = mem->Alloc<H1::cNode_t>(h1_asset->numNodes);
			for (unsigned int i = 0; i < h1_asset->numNodes; i++)
			{
				REINTERPRET_CAST_SAFE(h1_asset->nodes[i].plane, asset->cNodes[i].plane);
				h1_asset->nodes[i].children[0] = asset->cNodes[i].children[0];
				h1_asset->nodes[i].children[1] = asset->cNodes[i].children[1];
			}

			h1_asset->numLeafs = asset->numCLeaf;
			h1_asset->leafs = mem->Alloc<H1::cLeaf_t>(h1_asset->numLeafs);
			for (unsigned int i = 0; i < h1_asset->numLeafs; i++)
			{
				h1_asset->leafs[i].firstCollAabbIndex = asset->cLeaf[i].firstCollAabbIndex;
				h1_asset->leafs[i].collAabbCount = asset->cLeaf[i].collAabbCount;
				h1_asset->leafs[i].brushContents = asset->cLeaf[i].brushContents;
				h1_asset->leafs[i].terrainContents = asset->cLeaf[i].terrainContents;
				memcpy(&h1_asset->leafs[i].bounds, &asset->cLeaf[i].mins, sizeof(float[3]) * 2);
				h1_asset->leafs[i].leafBrushNode = asset->cLeaf[i].leafBrushNode;
			}
			
			h1_asset->numSubModels = asset->numCModels;
			h1_asset->cmodels = mem->Alloc<H1::cmodel_t>(h1_asset->numSubModels);
			for (unsigned int i = 0; i < h1_asset->numSubModels; i++)
			{
				memcpy(&h1_asset->cmodels[i].bounds, &asset->cModels[i].bounds, sizeof(IW5::Bounds));
				h1_asset->cmodels[i].radius = asset->cModels[i].radius;

				//GenerateH1ClipInfo(iw6_asset->cmodels[i].info, asset->cModels[i].info, mem);
				//if (!iw6_asset->cmodels[i].info)
				{
					h1_asset->cmodels[i].info = h1_asset->pInfo;
				}

				//memcpy(&h1_asset->cmodels[i].leaf, &asset->cModels[i].leaf, sizeof(IW5::cLeaf_t));
				h1_asset->cmodels[i].leaf.firstCollAabbIndex = asset->cModels[i].leaf.firstCollAabbIndex;
				h1_asset->cmodels[i].leaf.collAabbCount = asset->cModels[i].leaf.collAabbCount;
				h1_asset->cmodels[i].leaf.brushContents = asset->cModels[i].leaf.brushContents;
				h1_asset->cmodels[i].leaf.terrainContents = asset->cModels[i].leaf.terrainContents;
				memcpy(&h1_asset->cmodels[i].leaf.bounds, &asset->cModels[i].leaf.mins, sizeof(float[3]) * 2);
				h1_asset->cmodels[i].leaf.leafBrushNode = asset->cModels[i].leaf.leafBrushNode;
			}

			h1_asset->mapEnts = mem->Alloc<H1::MapEnts>();
			h1_asset->mapEnts->name = asset->mapEnts->name; // NEED TO DO MAPENTs LATER

			h1_asset->stageCount = asset->stageCount;
			h1_asset->stages = mem->Alloc<H1::Stage>(h1_asset->stageCount);
			for (unsigned int i = 0; i < h1_asset->stageCount; i++)
			{
				h1_asset->stages[i].name = reinterpret_cast<const char* __ptr64>(asset->stages[i].name);
				memcpy(&h1_asset->stages[i].origin, &asset->stages[i].origin, sizeof(float[3]));
				h1_asset->stages[i].sunPrimaryLightIndex = asset->stages[i].sunPrimaryLightIndex;
				h1_asset->stages[i].unk = 0x3A83126F;
			}
			h1_asset->stageTrigger; // NEED TO DO THIS LATER

			for (unsigned char i = 0; i < 2; i++)
			{
				h1_asset->dynEntCount[i] = asset->dynEntCount[i];
				h1_asset->dynEntDefList[i] = mem->Alloc<H1::DynEntityDef>(h1_asset->dynEntCount[i]);
				h1_asset->dynEntPoseList[i] = reinterpret_cast<H1::DynEntityPose * __ptr64>(asset->dynEntPoseList[i]); // check
				h1_asset->dynEntClientList[i] = mem->Alloc<H1::DynEntityClient>(h1_asset->dynEntCount[i]);
				h1_asset->dynEntCollList[i] = reinterpret_cast<H1::DynEntityColl * __ptr64>(asset->dynEntCollList[i]);
				for (unsigned short j = 0; j < h1_asset->dynEntCount[i]; j++)
				{
					h1_asset->dynEntDefList[i][j].type = static_cast<H1::DynEntityType>(asset->dynEntDefList[i][j].type);
					memcpy(&h1_asset->dynEntDefList[i][j].pose, &asset->dynEntDefList[i][j].pose, sizeof(IW5::GfxPlacement));
					h1_asset->dynEntDefList[i][j].baseModel = reinterpret_cast<H1::XModel * __ptr64>(asset->dynEntDefList[i][j].xModel);
					h1_asset->dynEntDefList[i][j].brushModel = asset->dynEntDefList[i][j].brushModel;
					h1_asset->dynEntDefList[i][j].physicsBrushModel = asset->dynEntDefList[i][j].physicsBrushModel;
					h1_asset->dynEntDefList[i][j].health = asset->dynEntDefList[i][j].health;
					h1_asset->dynEntDefList[i][j].destroyFx = reinterpret_cast<H1::FxEffectDef * __ptr64>(asset->dynEntDefList[i][j].destroyFx);
					h1_asset->dynEntDefList[i][j].sound = nullptr;
					h1_asset->dynEntDefList[i][j].physPreset = reinterpret_cast<H1::PhysPreset * __ptr64>(asset->dynEntDefList[i][j].physPreset);
					h1_asset->dynEntDefList[i][j].hinge = reinterpret_cast<H1::DynEntityHingeDef * __ptr64>(asset->dynEntDefList[i][j].hinge);
					h1_asset->dynEntDefList[i][j].linkTo = nullptr;
					memcpy(&h1_asset->dynEntDefList[i][j].mass, &asset->dynEntDefList[i][j].mass, sizeof(IW5::PhysMass));
					h1_asset->dynEntDefList[i][j].contents = asset->dynEntDefList[i][j].contents; // check

					h1_asset->dynEntClientList[i][j].physObjId = asset->dynEntClientList[i]->physObjId;
					h1_asset->dynEntClientList[i][j].flags = asset->dynEntClientList[i]->flags;
					h1_asset->dynEntClientList[i][j].lightingHandle = asset->dynEntClientList[i]->lightingHandle;
					h1_asset->dynEntClientList[i][j].health = asset->dynEntClientList[i]->health;
					h1_asset->dynEntClientList[i][j].hinge = nullptr;
					h1_asset->dynEntClientList[i][j].activeModel = nullptr;
					h1_asset->dynEntClientList[i][j].contents = asset->dynEntClientList[i]->contents;
				}
			}

			h1_asset->dynEntAnchorCount = 0;
			h1_asset->dynEntAnchorNames = nullptr;

			h1_asset->scriptableMapEnts;

			h1_asset->grappleData;

			h1_asset->checksum = asset->isPlutoniumMap;

			return h1_asset;
		}

		void IClipMap::dump(clipMap_t* asset, ZoneMemory* mem)
		{
			// generate h1 clipmap
			auto h1_asset = GenerateH1ClipMap(asset, mem);

			// dump h1 clipmap
			H1::IClipMap::dump(h1_asset, SL_ConvertToString);

			// dump physmap here too i guess, since it's needed.
			auto* physmap = mem->Alloc<H1::PhysWorld>();
			physmap->name = asset->name;

			physmap->modelsCount = h1_asset->numSubModels;
			physmap->models = mem->Alloc<H1::PhysBrushModel>(physmap->modelsCount);
			for (unsigned int i = 0; i < physmap->modelsCount; i++)
			{
				physmap->models[i].model = 0xFFFF0000FFFFFFFF;
			}

			physmap->polytopeDatasCount = 0;
			physmap->polytopeDatas = nullptr;

			// todo: mesh data
			physmap->meshDatasCount = 0;
			physmap->meshDatas = nullptr;

			physmap->waterVolumesCount = 0;
			physmap->waterVolumes = nullptr;

			H1::IPhysWorld::dump(physmap, SL_ConvertToString);
		}
	}
}