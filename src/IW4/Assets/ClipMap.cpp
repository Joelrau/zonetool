#include "stdafx.hpp"
#include "H1/Assets/ClipMap.hpp"

#include "H1/Assets/PhysWorld.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		namespace
		{
			H1::CSurfaceFlags surf_flags_conversion_table[31]
			{
				H1::SURF_FLAG_DEFAULT,
				H1::SURF_FLAG_BARK,
				H1::SURF_FLAG_BRICK,
				H1::SURF_FLAG_CARPET,
				H1::SURF_FLAG_CLOTH,
				H1::SURF_FLAG_CONCRETE,
				H1::SURF_FLAG_DIRT,
				H1::SURF_FLAG_FLESH,
				H1::SURF_FLAG_FOLIAGE_DEBRIS,
				H1::SURF_FLAG_GLASS,
				H1::SURF_FLAG_GRASS,
				H1::SURF_FLAG_GRAVEL,
				H1::SURF_FLAG_ICE,
				H1::SURF_FLAG_METAL_SOLID,
				H1::SURF_FLAG_MUD,
				H1::SURF_FLAG_PAPER,
				H1::SURF_FLAG_PLASTER,
				H1::SURF_FLAG_ROCK,
				H1::SURF_FLAG_SAND,
				H1::SURF_FLAG_SNOW,
				H1::SURF_FLAG_WATER_WAIST,
				H1::SURF_FLAG_WOOD_SOLID,
				H1::SURF_FLAG_ASPHALT,
				H1::SURF_FLAG_CERAMIC,
				H1::SURF_FLAG_PLASTIC_SOLID,
				H1::SURF_FLAG_RUBBER,
				H1::SURF_FLAG_CUSHION,
				H1::SURF_FLAG_FRUIT,
				H1::SURF_FLAG_PAINTEDMETAL,
				H1::SURF_FLAG_RIOTSHIELD,
				H1::SURF_FLAG_SLUSH,
			}; IW4::CSurfaceFlags;

			int convert_surf_flags(int flags)
			{
				int h1_flags = surf_flags_conversion_table[flags >> 20];
				auto convert = [&](IW4::CSurfaceFlags a, H1::CSurfaceFlags b)
				{
					h1_flags |= ((flags & a) == a) ? b : 0;
				};
				convert(IW4::CSurfaceFlags::SURF_FLAG_OPAQUEGLASS, H1::CSurfaceFlags::SURF_FLAG_DEFAULT);
				convert(IW4::CSurfaceFlags::SURF_FLAG_CLIPMISSILE, H1::CSurfaceFlags::SURF_FLAG_CLIPMISSILE);
				convert(IW4::CSurfaceFlags::SURF_FLAG_AI_NOSIGHT, H1::CSurfaceFlags::SURF_FLAG_AI_NOSIGHT);
				convert(IW4::CSurfaceFlags::SURF_FLAG_CLIPSHOT, H1::CSurfaceFlags::SURF_FLAG_CLIPSHOT);
				convert(IW4::CSurfaceFlags::SURF_FLAG_PLAYERCLIP, H1::CSurfaceFlags::SURF_FLAG_PLAYERCLIP);
				convert(IW4::CSurfaceFlags::SURF_FLAG_MONSTERCLIP, H1::CSurfaceFlags::SURF_FLAG_MONSTERCLIP);
				convert(IW4::CSurfaceFlags::SURF_FLAG_AICLIPALLOWDEATH, H1::CSurfaceFlags::SURF_FLAG_AICLIPALLOWDEATH);
				convert(IW4::CSurfaceFlags::SURF_FLAG_VEHICLECLIP, H1::CSurfaceFlags::SURF_FLAG_VEHICLECLIP);
				convert(IW4::CSurfaceFlags::SURF_FLAG_ITEMCLIP, H1::CSurfaceFlags::SURF_FLAG_ITEMCLIP);
				convert(IW4::CSurfaceFlags::SURF_FLAG_NODROP, H1::CSurfaceFlags::SURF_FLAG_NODROP);
				convert(IW4::CSurfaceFlags::SURF_FLAG_NONSOLID, H1::CSurfaceFlags::SURF_FLAG_NONSOLID);
				convert(IW4::CSurfaceFlags::SURF_FLAG_DETAIL, H1::CSurfaceFlags::SURF_FLAG_DETAIL);
				convert(IW4::CSurfaceFlags::SURF_FLAG_STRUCTURAL, H1::CSurfaceFlags::SURF_FLAG_STRUCTURAL);
				convert(IW4::CSurfaceFlags::SURF_FLAG_PORTAL, H1::CSurfaceFlags::SURF_FLAG_PORTAL);
				convert(IW4::CSurfaceFlags::SURF_FLAG_CANSHOOTCLIP, H1::CSurfaceFlags::SURF_FLAG_CANSHOOTCLIP);
				convert(IW4::CSurfaceFlags::SURF_FLAG_ORIGIN, H1::CSurfaceFlags::SURF_FLAG_ORIGIN);
				convert(IW4::CSurfaceFlags::SURF_FLAG_SKY, H1::CSurfaceFlags::SURF_FLAG_SKY);
				convert(IW4::CSurfaceFlags::SURF_FLAG_NOCASTSHADOW, H1::CSurfaceFlags::SURF_FLAG_NOCASTSHADOW);
				convert(IW4::CSurfaceFlags::SURF_FLAG_PHYSICSGEOM, H1::CSurfaceFlags::SURF_FLAG_PHYSICSGEOM);
				convert(IW4::CSurfaceFlags::SURF_FLAG_LIGHTPORTAL, H1::CSurfaceFlags::SURF_FLAG_LIGHTPORTAL);
				convert(IW4::CSurfaceFlags::SURF_FLAG_OUTDOORBOUNDS, H1::CSurfaceFlags::SURF_FLAG_OUTDOORBOUNDS);
				convert(IW4::CSurfaceFlags::SURF_FLAG_SLICK, H1::CSurfaceFlags::SURF_FLAG_SLICK);
				convert(IW4::CSurfaceFlags::SURF_FLAG_NOIMPACT, H1::CSurfaceFlags::SURF_FLAG_NOIMPACT);
				convert(IW4::CSurfaceFlags::SURF_FLAG_NOMARKS, H1::CSurfaceFlags::SURF_FLAG_NOMARKS);
				convert(IW4::CSurfaceFlags::SURF_FLAG_NOPENETRATE, H1::CSurfaceFlags::SURF_FLAG_NOPENETRATE);
				convert(IW4::CSurfaceFlags::SURF_FLAG_LADDER, H1::CSurfaceFlags::SURF_FLAG_LADDER);
				convert(IW4::CSurfaceFlags::SURF_FLAG_NODAMAGE, H1::CSurfaceFlags::SURF_FLAG_NODAMAGE);
				convert(IW4::CSurfaceFlags::SURF_FLAG_MANTLEON, H1::CSurfaceFlags::SURF_FLAG_MANTLEON);
				convert(IW4::CSurfaceFlags::SURF_FLAG_MANTLEOVER, H1::CSurfaceFlags::SURF_FLAG_MANTLEOVER);
				convert(IW4::CSurfaceFlags::SURF_FLAG_STAIRS, H1::CSurfaceFlags::SURF_FLAG_STAIRS);
				convert(IW4::CSurfaceFlags::SURF_FLAG_SOFT, H1::CSurfaceFlags::SURF_FLAG_SOFT);
				convert(IW4::CSurfaceFlags::SURF_FLAG_NOSTEPS, H1::CSurfaceFlags::SURF_FLAG_NOSTEPS);
				convert(IW4::CSurfaceFlags::SURF_FLAG_NODRAW, H1::CSurfaceFlags::SURF_FLAG_NODRAW);
				convert(IW4::CSurfaceFlags::SURF_FLAG_NOLIGHTMAP, H1::CSurfaceFlags::SURF_FLAG_NOLIGHTMAP);
				convert(IW4::CSurfaceFlags::SURF_FLAG_NODLIGHT, H1::CSurfaceFlags::SURF_FLAG_NODLIGHT);
				return h1_flags;
			}
		}

		void GenerateH1ClipInfo(H1::ClipInfo* info, clipMap_t* asset, ZoneMemory* mem)
		{
			info->planeCount = asset->numCPlanes;
			REINTERPRET_CAST_SAFE(info->planes, asset->cPlanes);

			info->numMaterials = asset->numMaterials;
			info->materials = mem->Alloc<H1::ClipMaterial>(info->numMaterials);
			for (unsigned int i = 0; i < info->numMaterials; i++)
			{
				info->materials[i].name = asset->materials[i].material;
				info->materials[i].surfaceFlags = convert_surf_flags(asset->materials[i].surfaceFlags);
				info->materials[i].contents = asset->materials[i].contentFlags;
			}

			info->bCollisionTree.leafbrushNodesCount = asset->numCLeafBrushNodes;
			info->bCollisionTree.leafbrushNodes = mem->Alloc<H1::cLeafBrushNode_s>(info->bCollisionTree.leafbrushNodesCount);
			for (unsigned int i = 0; i < info->bCollisionTree.leafbrushNodesCount; i++)
			{
				info->bCollisionTree.leafbrushNodes[i].axis = asset->cLeafBrushNodes[i].axis;
				info->bCollisionTree.leafbrushNodes[i].leafBrushCount = asset->cLeafBrushNodes[i].leafBrushCount;
				info->bCollisionTree.leafbrushNodes[i].contents = asset->cLeafBrushNodes[i].contents;
				if (info->bCollisionTree.leafbrushNodes[i].leafBrushCount > 0)
				{
					REINTERPRET_CAST_SAFE(info->bCollisionTree.leafbrushNodes[i].data.leaf.brushes, asset->cLeafBrushNodes[i].data.leaf.brushes);
				}
				else
				{
					info->bCollisionTree.leafbrushNodes[i].data.children.dist = asset->cLeafBrushNodes[i].data.children.dist;
					info->bCollisionTree.leafbrushNodes[i].data.children.range = asset->cLeafBrushNodes[i].data.children.range;
					
					info->bCollisionTree.leafbrushNodes[i].data.children.childOffset[0] = asset->cLeafBrushNodes[i].data.children.childOffset[0];
					info->bCollisionTree.leafbrushNodes[i].data.children.childOffset[1] = asset->cLeafBrushNodes[i].data.children.childOffset[1];
				}
			}

			info->bCollisionTree.numLeafBrushes = asset->numLeafBrushes;
			REINTERPRET_CAST_SAFE(info->bCollisionTree.leafbrushes, asset->leafBrushes);

			info->pCollisionTree.aabbTreeCount = asset->numCollisionAABBTrees;
			info->pCollisionTree.aabbTrees = mem->Alloc<H1::CollisionAabbTree>(info->pCollisionTree.aabbTreeCount);
			for (int i = 0; i < info->pCollisionTree.aabbTreeCount; i++)
			{
				std::memcpy(info->pCollisionTree.aabbTrees[i].midPoint, asset->collisionAABBTrees[i].midPoint, sizeof(float[3]));
				std::memcpy(info->pCollisionTree.aabbTrees[i].halfSize, asset->collisionAABBTrees[i].halfSize, sizeof(float[3]));
				info->pCollisionTree.aabbTrees[i].materialIndex = asset->collisionAABBTrees[i].materialIndex;
				info->pCollisionTree.aabbTrees[i].childCount = asset->collisionAABBTrees[i].childCount;

				info->pCollisionTree.aabbTrees[i].u.firstChildIndex = asset->collisionAABBTrees[i].u.firstChildIndex;
				//info->pCollisionTree.aabbTrees[i].u.partitionIndex = asset->collisionAABBTrees[i].u.partitionIndex;
			}

			info->sCollisionTree.numStaticModels = asset->numStaticModels;
			info->sCollisionTree.smodelNodeCount = asset->smodelNodeCount;
			REINTERPRET_CAST_SAFE(info->sCollisionTree.smodelNodes, asset->smodelNodes);

			std::unordered_map<cbrushside_t*, H1::cbrushside_t*> mapped_brush_sides;

			info->bCollisionData.numBrushSides = asset->numCBrushSides;
			info->bCollisionData.brushSides = mem->Alloc<H1::cbrushside_t>(info->bCollisionData.numBrushSides);
			for (unsigned int i = 0; i < info->bCollisionData.numBrushSides; i++)
			{
				mapped_brush_sides[&asset->cBrushSides[i]] = &info->bCollisionData.brushSides[i];

				// check
				info->bCollisionData.brushSides[i].planeIndex =
					(reinterpret_cast<std::uintptr_t>(asset->cBrushSides[i].plane) - reinterpret_cast<std::uintptr_t>(asset->cPlanes)) / sizeof(cplane_s);
				assert(info->bCollisionData.brushSides[i].planeIndex <= info->planeCount);

				info->bCollisionData.brushSides[i].materialNum = asset->cBrushSides[i].materialNum;
				info->bCollisionData.brushSides[i].firstAdjacentSideOffset = asset->cBrushSides[i].firstAdjacentSideOffset;
				info->bCollisionData.brushSides[i].edgeCount = asset->cBrushSides[i].edgeCount;
			}

			info->bCollisionData.numBrushEdges = asset->numCBrushEdges;
			REINTERPRET_CAST_SAFE(info->bCollisionData.brushEdges, asset->cBrushEdges);

			info->bCollisionData.numBrushes = asset->numBrushes;
			info->bCollisionData.brushes = mem->Alloc<H1::cbrush_t>(asset->numBrushes);
			for (unsigned int i = 0; i < info->bCollisionData.numBrushes; i++)
			{
				info->bCollisionData.brushes[i].numsides = asset->brushes[i].numsides;
				info->bCollisionData.brushes[i].glassPieceIndex = asset->brushes[i].glassPieceIndex;

				if (asset->brushes[i].sides)
					info->bCollisionData.brushes[i].sides = mapped_brush_sides.find(asset->brushes[i].sides)->second;

				REINTERPRET_CAST_SAFE(info->bCollisionData.brushes[i].baseAdjacentSide, asset->brushes[i].edge);

				memcpy(&info->bCollisionData.brushes[i].axialMaterialNum, &asset->brushes[i].axialMaterialNum, sizeof(short[2][3]));
				memcpy(&info->bCollisionData.brushes[i].firstAdjacentSideOffsets, &asset->brushes[i].firstAdjacentSideOffsets, sizeof(char[2][3]));
				memcpy(&info->bCollisionData.brushes[i].edgeCount, &asset->brushes[i].edgeCount, sizeof(char[2][3]));
			}

			REINTERPRET_CAST_SAFE(info->bCollisionData.brushBounds, asset->brushBounds);
			REINTERPRET_CAST_SAFE(info->bCollisionData.brushContents, asset->brushContents);

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
				//info->pCollisionData.partitions[i].firstVertSegment = asset->collisionPartitions[i].firstVertSegment;		// missing on IW4 struct?
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
				info->sCollisionData.staticModelList[i].lightingHandle = 0;
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
				memcpy(&h1_asset->leafs[i].bounds, &asset->cLeaf[i].bounds, sizeof(IW4::Bounds));
				h1_asset->leafs[i].leafBrushNode = asset->cLeaf[i].leafBrushNode;
			}

			h1_asset->numSubModels = asset->numCModels;
			h1_asset->cmodels = mem->Alloc<H1::cmodel_t>(h1_asset->numSubModels);
			for (unsigned int i = 0; i < h1_asset->numSubModels; i++)
			{
				memcpy(&h1_asset->cmodels[i].bounds, &asset->cModels[i].bounds, sizeof(IW4::Bounds));
				h1_asset->cmodels[i].radius = asset->cModels[i].radius;

				h1_asset->cmodels[i].info = nullptr; // mapped in h1

				h1_asset->cmodels[i].leaf.firstCollAabbIndex = asset->cModels[i].leaf.firstCollAabbIndex;
				h1_asset->cmodels[i].leaf.collAabbCount = asset->cModels[i].leaf.collAabbCount;
				h1_asset->cmodels[i].leaf.brushContents = asset->cModels[i].leaf.brushContents;
				h1_asset->cmodels[i].leaf.terrainContents = asset->cModels[i].leaf.terrainContents;
				memcpy(&h1_asset->cmodels[i].leaf.bounds, &asset->cModels[i].leaf.bounds, sizeof(IW4::Bounds));
				h1_asset->cmodels[i].leaf.leafBrushNode = asset->cModels[i].leaf.leafBrushNode;
			}

			h1_asset->mapEnts = mem->Alloc<H1::MapEnts>();
			h1_asset->mapEnts->name = asset->mapEnts->name;

			h1_asset->stageCount = asset->mapEnts->stageCount;
			h1_asset->stages = mem->Alloc<H1::Stage>(h1_asset->stageCount);
			for (unsigned int i = 0; i < h1_asset->stageCount; i++)
			{
				h1_asset->stages[i].name = reinterpret_cast<const char* __ptr64>(asset->mapEnts->stageNames[i].name);
				memcpy(&h1_asset->stages[i].origin, &asset->mapEnts->stageNames[i].origin, sizeof(float[3]));
				h1_asset->stages[i].triggerIndex = asset->mapEnts->stageNames[i].triggerIndex;
				h1_asset->stages[i].sunPrimaryLightIndex = asset->mapEnts->stageNames[i].sunPrimaryLightIndex;
				h1_asset->stages[i].unk = 0x3A83126F;
			}
			h1_asset->stageTrigger.count = 0;
			h1_asset->stageTrigger.models = nullptr;
			h1_asset->stageTrigger.hullCount = 0;
			h1_asset->stageTrigger.hulls = nullptr;
			h1_asset->stageTrigger.slabCount = 0;
			h1_asset->stageTrigger.slabs = nullptr;

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
					memcpy(&h1_asset->dynEntDefList[i][j].pose, &asset->dynEntDefList[i][j].pose, sizeof(IW4::GfxPlacement));
					h1_asset->dynEntDefList[i][j].baseModel = reinterpret_cast<H1::XModel * __ptr64>(asset->dynEntDefList[i][j].xModel);
					h1_asset->dynEntDefList[i][j].brushModel = asset->dynEntDefList[i][j].brushModel;
					h1_asset->dynEntDefList[i][j].physicsBrushModel = asset->dynEntDefList[i][j].physicsBrushModel;
					h1_asset->dynEntDefList[i][j].health = asset->dynEntDefList[i][j].health;
					h1_asset->dynEntDefList[i][j].destroyFx = reinterpret_cast<H1::FxEffectDef * __ptr64>(asset->dynEntDefList[i][j].destroyFx);
					h1_asset->dynEntDefList[i][j].sound = nullptr;
					h1_asset->dynEntDefList[i][j].physPreset = reinterpret_cast<H1::PhysPreset * __ptr64>(asset->dynEntDefList[i][j].physPreset);
					h1_asset->dynEntDefList[i][j].hinge = reinterpret_cast<H1::DynEntityHingeDef * __ptr64>(asset->dynEntDefList[i][j].hinge);
					h1_asset->dynEntDefList[i][j].linkTo = nullptr;
					memcpy(&h1_asset->dynEntDefList[i][j].mass, &asset->dynEntDefList[i][j].mass, sizeof(IW4::PhysMass));
					h1_asset->dynEntDefList[i][j].contents = asset->dynEntDefList[i][j].contents;
					if (1) // check
					{
						h1_asset->dynEntDefList[i][j].unk[0] = 2500.00000f;
						h1_asset->dynEntDefList[i][j].unk[1] = 0.0199999996f;
					}

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

		std::uint64_t dm_nullBodyId = 0xFFFF0000FFFFFFFF;
		std::uint64_t dm_nullFixtureId = 0xFFFF0000FFFFFFFF;

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
				physmap->models[i].fields.polytopeIndex = -1;
				physmap->models[i].fields.unk = -1;
				physmap->models[i].fields.worldIndex = 0;
				physmap->models[i].fields.meshIndex = -1;
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