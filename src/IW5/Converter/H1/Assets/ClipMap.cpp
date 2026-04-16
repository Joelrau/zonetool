#include "stdafx.hpp"
#include "../Include.hpp"

#include "ClipMap.hpp"

#include "H1/Utils/PhysWorld/generate.hpp"

namespace ZoneTool::IW5
{
	namespace H1Converter
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
			}; IW5::CSurfaceFlags;

			int convert_surf_flags(int flags)
			{
				int h1_flags = surf_flags_conversion_table[flags >> 20];
				auto convert = [&](IW5::CSurfaceFlags a, H1::CSurfaceFlags b)
				{
					h1_flags |= ((flags & a) == a) ? b : 0;
				};
				convert(IW5::CSurfaceFlags::SURF_FLAG_OPAQUEGLASS, H1::CSurfaceFlags::SURF_FLAG_DEFAULT);
				convert(IW5::CSurfaceFlags::SURF_FLAG_CLIPMISSILE, H1::CSurfaceFlags::SURF_FLAG_CLIPMISSILE);
				convert(IW5::CSurfaceFlags::SURF_FLAG_AI_NOSIGHT, H1::CSurfaceFlags::SURF_FLAG_AI_NOSIGHT);
				convert(IW5::CSurfaceFlags::SURF_FLAG_CLIPSHOT, H1::CSurfaceFlags::SURF_FLAG_CLIPSHOT);
				convert(IW5::CSurfaceFlags::SURF_FLAG_PLAYERCLIP, H1::CSurfaceFlags::SURF_FLAG_PLAYERCLIP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_MONSTERCLIP, H1::CSurfaceFlags::SURF_FLAG_MONSTERCLIP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_AICLIPALLOWDEATH, H1::CSurfaceFlags::SURF_FLAG_AICLIPALLOWDEATH);
				convert(IW5::CSurfaceFlags::SURF_FLAG_VEHICLECLIP, H1::CSurfaceFlags::SURF_FLAG_VEHICLECLIP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_ITEMCLIP, H1::CSurfaceFlags::SURF_FLAG_ITEMCLIP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NODROP, H1::CSurfaceFlags::SURF_FLAG_NODROP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NONSOLID, H1::CSurfaceFlags::SURF_FLAG_NONSOLID);
				convert(IW5::CSurfaceFlags::SURF_FLAG_DETAIL, H1::CSurfaceFlags::SURF_FLAG_DETAIL);
				convert(IW5::CSurfaceFlags::SURF_FLAG_STRUCTURAL, H1::CSurfaceFlags::SURF_FLAG_STRUCTURAL);
				convert(IW5::CSurfaceFlags::SURF_FLAG_PORTAL, H1::CSurfaceFlags::SURF_FLAG_PORTAL);
				convert(IW5::CSurfaceFlags::SURF_FLAG_CANSHOOTCLIP, H1::CSurfaceFlags::SURF_FLAG_CANSHOOTCLIP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_ORIGIN, H1::CSurfaceFlags::SURF_FLAG_ORIGIN);
				convert(IW5::CSurfaceFlags::SURF_FLAG_SKY, H1::CSurfaceFlags::SURF_FLAG_SKY);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NOCASTSHADOW, H1::CSurfaceFlags::SURF_FLAG_NOCASTSHADOW);
				convert(IW5::CSurfaceFlags::SURF_FLAG_PHYSICSGEOM, H1::CSurfaceFlags::SURF_FLAG_PHYSICSGEOM);
				convert(IW5::CSurfaceFlags::SURF_FLAG_LIGHTPORTAL, H1::CSurfaceFlags::SURF_FLAG_LIGHTPORTAL);
				convert(IW5::CSurfaceFlags::SURF_FLAG_OUTDOORBOUNDS, H1::CSurfaceFlags::SURF_FLAG_OUTDOORBOUNDS);
				convert(IW5::CSurfaceFlags::SURF_FLAG_SLICK, H1::CSurfaceFlags::SURF_FLAG_SLICK);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NOIMPACT, H1::CSurfaceFlags::SURF_FLAG_NOIMPACT);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NOMARKS, H1::CSurfaceFlags::SURF_FLAG_NOMARKS);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NOPENETRATE, H1::CSurfaceFlags::SURF_FLAG_NOPENETRATE);
				convert(IW5::CSurfaceFlags::SURF_FLAG_LADDER, H1::CSurfaceFlags::SURF_FLAG_LADDER);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NODAMAGE, H1::CSurfaceFlags::SURF_FLAG_NODAMAGE);
				convert(IW5::CSurfaceFlags::SURF_FLAG_MANTLEON, H1::CSurfaceFlags::SURF_FLAG_MANTLEON);
				convert(IW5::CSurfaceFlags::SURF_FLAG_MANTLEOVER, H1::CSurfaceFlags::SURF_FLAG_MANTLEOVER);
				convert(IW5::CSurfaceFlags::SURF_FLAG_STAIRS, H1::CSurfaceFlags::SURF_FLAG_STAIRS);
				convert(IW5::CSurfaceFlags::SURF_FLAG_SOFT, H1::CSurfaceFlags::SURF_FLAG_SOFT);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NOSTEPS, H1::CSurfaceFlags::SURF_FLAG_NOSTEPS);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NODRAW, H1::CSurfaceFlags::SURF_FLAG_NODRAW);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NOLIGHTMAP, H1::CSurfaceFlags::SURF_FLAG_NOLIGHTMAP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NODLIGHT, H1::CSurfaceFlags::SURF_FLAG_NODLIGHT);
				return h1_flags;
			}
		}

		void GenerateH1ClipInfo(H1::ClipInfo* info, clipMap_t* asset, allocator& mem)
		{
			info->planeCount = asset->info.planeCount;
			REINTERPRET_CAST_SAFE(info->planes, asset->info.planes);

			info->numMaterials = asset->info.numMaterials;
			info->materials = mem.allocate<H1::ClipMaterial>(info->numMaterials);
			for (unsigned int i = 0; i < info->numMaterials; i++)
			{
				info->materials[i].name = asset->info.materials[i].name;
				info->materials[i].surfaceFlags = convert_surf_flags(asset->info.materials[i].surfaceFlags);
				info->materials[i].contents = asset->info.materials[i].contents;
			}

			info->bCollisionTree.leafbrushNodesCount = asset->info.leafbrushNodesCount;
			info->bCollisionTree.leafbrushNodes = mem.allocate<H1::cLeafBrushNode_s>(info->bCollisionTree.leafbrushNodesCount);
			for (unsigned int i = 0; i < info->bCollisionTree.leafbrushNodesCount; i++)
			{
				info->bCollisionTree.leafbrushNodes[i].axis = asset->info.leafbrushNodes[i].axis;
				info->bCollisionTree.leafbrushNodes[i].leafBrushCount = asset->info.leafbrushNodes[i].leafBrushCount;
				info->bCollisionTree.leafbrushNodes[i].contents = asset->info.leafbrushNodes[i].contents;
				if (info->bCollisionTree.leafbrushNodes[i].leafBrushCount > 0)
				{
					REINTERPRET_CAST_SAFE(info->bCollisionTree.leafbrushNodes[i].data.leaf.brushes, asset->info.leafbrushNodes[i].data.leaf.brushes);
				}
				else
				{
					info->bCollisionTree.leafbrushNodes[i].data.children.dist = asset->info.leafbrushNodes[i].data.children.dist;
					info->bCollisionTree.leafbrushNodes[i].data.children.range = asset->info.leafbrushNodes[i].data.children.range;

					info->bCollisionTree.leafbrushNodes[i].data.children.childOffset[0] = asset->info.leafbrushNodes[i].data.children.childOffset[0];
					info->bCollisionTree.leafbrushNodes[i].data.children.childOffset[1] = asset->info.leafbrushNodes[i].data.children.childOffset[1];
				}
			}

			info->bCollisionTree.numLeafBrushes = asset->info.numLeafBrushes;
			REINTERPRET_CAST_SAFE(info->bCollisionTree.leafbrushes, asset->info.leafbrushes);

			info->pCollisionTree.aabbTreeCount = asset->aabbTreeCount;
			info->pCollisionTree.aabbTrees = mem.allocate<H1::CollisionAabbTree>(info->pCollisionTree.aabbTreeCount);
			for (int i = 0; i < info->pCollisionTree.aabbTreeCount; i++)
			{
				std::memcpy(info->pCollisionTree.aabbTrees[i].midPoint, asset->aabbTrees[i].midPoint, sizeof(float[3]));
				std::memcpy(info->pCollisionTree.aabbTrees[i].halfSize, asset->aabbTrees[i].halfSize, sizeof(float[3]));
				info->pCollisionTree.aabbTrees[i].materialIndex = asset->aabbTrees[i].materialIndex;
				info->pCollisionTree.aabbTrees[i].childCount = asset->aabbTrees[i].childCount;

				info->pCollisionTree.aabbTrees[i].u.firstChildIndex = asset->aabbTrees[i].u.firstChildIndex;
				//info->pCollisionTree.aabbTrees[i].u.partitionIndex = asset->aabbTrees[i].u.partitionIndex;
			}

			info->sCollisionTree.numStaticModels = asset->numStaticModels;
			info->sCollisionTree.smodelNodeCount = asset->smodelNodeCount;
			REINTERPRET_CAST_SAFE(info->sCollisionTree.smodelNodes, asset->smodelNodes);

			std::unordered_map<cbrushside_t*, H1::cbrushside_t*> mapped_brush_sides;

			info->bCollisionData.numBrushSides = asset->info.numBrushSides;
			info->bCollisionData.brushSides = mem.allocate<H1::cbrushside_t>(info->bCollisionData.numBrushSides);
			for (unsigned int i = 0; i < info->bCollisionData.numBrushSides; i++)
			{
				mapped_brush_sides[&asset->info.brushsides[i]] = &info->bCollisionData.brushSides[i];

				// check
				info->bCollisionData.brushSides[i].planeIndex =
					(reinterpret_cast<std::uintptr_t>(asset->info.brushsides[i].plane) - reinterpret_cast<std::uintptr_t>(asset->info.planes)) / sizeof(cplane_s);
				assert(info->bCollisionData.brushSides[i].planeIndex <= info->planeCount);

				info->bCollisionData.brushSides[i].materialNum = asset->info.brushsides[i].materialNum;
				info->bCollisionData.brushSides[i].firstAdjacentSideOffset = asset->info.brushsides[i].firstAdjacentSideOffset;
				info->bCollisionData.brushSides[i].edgeCount = asset->info.brushsides[i].edgeCount;
			}

			info->bCollisionData.numBrushEdges = asset->info.numBrushEdges;
			REINTERPRET_CAST_SAFE(info->bCollisionData.brushEdges, asset->info.brushEdges);

			info->bCollisionData.numBrushes = asset->info.numBrushes;
			info->bCollisionData.brushes = mem.allocate<H1::cbrush_t>(asset->info.numBrushes);
			for (unsigned int i = 0; i < info->bCollisionData.numBrushes; i++)
			{
				info->bCollisionData.brushes[i].numsides = asset->info.brushes[i].numsides;
				info->bCollisionData.brushes[i].glassPieceIndex = asset->info.brushes[i].glassPieceIndex;

				if (asset->info.brushes[i].sides)
					info->bCollisionData.brushes[i].sides = mapped_brush_sides.find(asset->info.brushes[i].sides)->second;

				REINTERPRET_CAST_SAFE(info->bCollisionData.brushes[i].baseAdjacentSide, asset->info.brushes[i].baseAdjacentSide);

				memcpy(&info->bCollisionData.brushes[i].axialMaterialNum, &asset->info.brushes[i].axialMaterialNum, sizeof(short[2][3]));
				memcpy(&info->bCollisionData.brushes[i].firstAdjacentSideOffsets, &asset->info.brushes[i].firstAdjacentSideOffsets, sizeof(char[2][3]));
				memcpy(&info->bCollisionData.brushes[i].edgeCount, &asset->info.brushes[i].edgeCount, sizeof(char[2][3]));
			}

			REINTERPRET_CAST_SAFE(info->bCollisionData.brushBounds, asset->info.brushBounds);
			REINTERPRET_CAST_SAFE(info->bCollisionData.brushContents, asset->info.brushContents);

			info->pCollisionData.vertCount = asset->vertCount;
			REINTERPRET_CAST_SAFE(info->pCollisionData.verts, asset->verts);
			info->pCollisionData.triCount = asset->triCount;
			REINTERPRET_CAST_SAFE(info->pCollisionData.triIndices, asset->triIndices);
			REINTERPRET_CAST_SAFE(info->pCollisionData.triEdgeIsWalkable, asset->triEdgeIsWalkable);
			info->pCollisionData.borderCount = asset->borderCount;
			REINTERPRET_CAST_SAFE(info->pCollisionData.borders, asset->borders);
			info->pCollisionData.partitionCount = asset->partitionCount;
			info->pCollisionData.partitions = mem.allocate<H1::CollisionPartition>(info->pCollisionData.partitionCount);
			for (int i = 0; i < info->pCollisionData.partitionCount; i++)
			{
				info->pCollisionData.partitions[i].triCount = asset->partitions[i].triCount;
				info->pCollisionData.partitions[i].borderCount = asset->partitions[i].borderCount;
				info->pCollisionData.partitions[i].firstVertSegment = asset->partitions[i].firstVertSegment;
				info->pCollisionData.partitions[i].firstTri = asset->partitions[i].firstTri;
				REINTERPRET_CAST_SAFE(info->pCollisionData.partitions[i].borders, asset->partitions[i].borders);
			}

			info->sCollisionData.numStaticModels = asset->numStaticModels;
			info->sCollisionData.staticModelList = mem.allocate<H1::cStaticModel_s>(info->sCollisionData.numStaticModels);
			for (unsigned int i = 0; i < info->sCollisionData.numStaticModels; i++)
			{
				info->sCollisionData.staticModelList[i].xmodel = reinterpret_cast<H1::XModel * __ptr64>(asset->staticModelList[i].xmodel);

				memcpy(&info->sCollisionData.staticModelList[i].origin, &asset->staticModelList[i].origin, sizeof(float[3]));
				memcpy(&info->sCollisionData.staticModelList[i].invScaledAxis, &asset->staticModelList[i].invScaledAxis, sizeof(float[3][3]));
				memcpy(&info->sCollisionData.staticModelList[i].absBounds, &asset->staticModelList[i].absBounds, sizeof(Bounds));
				info->sCollisionData.staticModelList[i].lightingHandle = 0;
			}
		}

		H1::clipMap_t* GenerateH1ClipMap(clipMap_t* asset, allocator& mem)
		{
			// allocate H1 clipMap_t structure
			const auto h1_asset = mem.allocate<H1::clipMap_t>();

			h1_asset->name = asset->name;
			h1_asset->isInUse = asset->isInUse;

			GenerateH1ClipInfo(&h1_asset->info, asset, mem);
			h1_asset->pInfo = &h1_asset->info;

			h1_asset->numNodes = asset->numNodes;
			h1_asset->nodes = mem.allocate<H1::cNode_t>(h1_asset->numNodes);
			for (unsigned int i = 0; i < h1_asset->numNodes; i++)
			{
				REINTERPRET_CAST_SAFE(h1_asset->nodes[i].plane, asset->nodes[i].plane);
				h1_asset->nodes[i].children[0] = asset->nodes[i].children[0];
				h1_asset->nodes[i].children[1] = asset->nodes[i].children[1];
			}

			h1_asset->numLeafs = asset->numLeafs;
			h1_asset->leafs = mem.allocate<H1::cLeaf_t>(h1_asset->numLeafs);
			for (unsigned int i = 0; i < h1_asset->numLeafs; i++)
			{
				h1_asset->leafs[i].firstCollAabbIndex = asset->leafs[i].firstCollAabbIndex;
				h1_asset->leafs[i].collAabbCount = asset->leafs[i].collAabbCount;
				h1_asset->leafs[i].brushContents = asset->leafs[i].brushContents;
				h1_asset->leafs[i].terrainContents = asset->leafs[i].terrainContents;
				memcpy(&h1_asset->leafs[i].bounds, &asset->leafs[i].bounds, sizeof(IW5::Bounds));
				h1_asset->leafs[i].leafBrushNode = asset->leafs[i].leafBrushNode;
			}

			h1_asset->numSubModels = asset->numSubModels;
			h1_asset->cmodels = mem.allocate<H1::cmodel_t>(h1_asset->numSubModels);
			for (unsigned int i = 0; i < h1_asset->numSubModels; i++)
			{
				memcpy(&h1_asset->cmodels[i].bounds, &asset->cmodels[i].bounds, sizeof(IW5::Bounds));
				h1_asset->cmodels[i].radius = asset->cmodels[i].radius;

				h1_asset->cmodels[i].info = nullptr; // mapped in h1

				h1_asset->cmodels[i].leaf.firstCollAabbIndex = asset->cmodels[i].leaf.firstCollAabbIndex;
				h1_asset->cmodels[i].leaf.collAabbCount = asset->cmodels[i].leaf.collAabbCount;
				h1_asset->cmodels[i].leaf.brushContents = asset->cmodels[i].leaf.brushContents;
				h1_asset->cmodels[i].leaf.terrainContents = asset->cmodels[i].leaf.terrainContents;
				memcpy(&h1_asset->cmodels[i].leaf.bounds, &asset->cmodels[i].leaf.bounds, sizeof(IW5::Bounds));
				h1_asset->cmodels[i].leaf.leafBrushNode = asset->cmodels[i].leaf.leafBrushNode;
			}

			h1_asset->mapEnts = mem.allocate<H1::MapEnts>();
			h1_asset->mapEnts->name = asset->mapEnts->name;

			h1_asset->stageCount = asset->stageCount;
			h1_asset->stages = mem.allocate<H1::Stage>(h1_asset->stageCount);
			for (unsigned int i = 0; i < h1_asset->stageCount; i++)
			{
				h1_asset->stages[i].name = reinterpret_cast<const char* __ptr64>(asset->stages[i].name);
				memcpy(&h1_asset->stages[i].origin, &asset->stages[i].origin, sizeof(float[3]));
				h1_asset->stages[i].triggerIndex = asset->stages[i].triggerIndex;
				h1_asset->stages[i].sunPrimaryLightIndex = asset->stages[i].sunPrimaryLightIndex;
				h1_asset->stages[i].radiometricScale = 0.001f;
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
				h1_asset->dynEntDefList[i] = mem.allocate<H1::DynEntityDef>(h1_asset->dynEntCount[i]);
				h1_asset->dynEntPoseList[i] = mem.allocate < H1::DynEntityPose>(h1_asset->dynEntCount[i]);
				h1_asset->dynEntClientList[i] = mem.allocate<H1::DynEntityClient>(h1_asset->dynEntCount[i]);
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
					h1_asset->dynEntDefList[i][j].contents = asset->dynEntDefList[i][j].contents;
					if (1) // check
					{
						h1_asset->dynEntDefList[i][j].unk[0] = 2500.00000f;
						h1_asset->dynEntDefList[i][j].unk[1] = 0.0199999996f;
					}

					memcpy(&h1_asset->dynEntPoseList[i][j].pose.quat, &asset->dynEntPoseList[i]->pose.quat, sizeof(float[4]));
					memcpy(&h1_asset->dynEntPoseList[i][j].pose.origin, &asset->dynEntPoseList[i]->pose.origin, sizeof(float[3]));
					h1_asset->dynEntPoseList[i][j].radius = asset->dynEntPoseList[i]->radius;
					//h1_asset->dynEntPoseList[i][j].__pad0;

					h1_asset->dynEntClientList[i][j].physObjId = asset->dynEntClientList[i]->physObjId;
					h1_asset->dynEntClientList[i][j].flags = asset->dynEntClientList[i]->flags;
					h1_asset->dynEntClientList[i][j].lightingHandle = asset->dynEntClientList[i]->lightingHandle;
					h1_asset->dynEntClientList[i][j].health = asset->dynEntClientList[i]->health;
					h1_asset->dynEntClientList[i][j].hinge = nullptr;
					h1_asset->dynEntClientList[i][j].activeModel = nullptr;
					h1_asset->dynEntClientList[i][j].contents = 0;
				}
			}

			h1_asset->dynEntAnchorCount = 0;
			h1_asset->dynEntAnchorNames = nullptr;

			//h1_asset->scriptableMapEnts;

			//h1_asset->grapple;

			h1_asset->checksum = asset->checksum;

			return h1_asset;
		}

		H1::clipMap_t* convert(clipMap_t* asset, allocator& allocator)
		{
			// generate h1 clipmap
			return GenerateH1ClipMap(asset, allocator);
		}
	}
}