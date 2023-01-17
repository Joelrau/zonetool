#include "stdafx.hpp"
#include "H1/Assets/ClipMap.hpp"

#include "H1/Assets/PhysWorld.hpp"

namespace ZoneTool
{
	namespace IW3
	{
		namespace
		{
			H1::CSurfaceFlags surf_flags_conversion_table[29]
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
			}; IW3::CSurfaceFlags;

			int convert_surf_flags(int flags)
			{
				int h1_flags = surf_flags_conversion_table[flags >> 20];
				auto convert = [&](IW3::CSurfaceFlags a, H1::CSurfaceFlags b)
				{
					h1_flags |= ((flags & a) == a) ? b : 0;
				};
				convert(IW3::CSurfaceFlags::SURF_FLAG_OPAQUEGLASS, H1::CSurfaceFlags::SURF_FLAG_DEFAULT);
				convert(IW3::CSurfaceFlags::SURF_FLAG_CLIPMISSILE, H1::CSurfaceFlags::SURF_FLAG_CLIPMISSILE);
				convert(IW3::CSurfaceFlags::SURF_FLAG_AI_NOSIGHT, H1::CSurfaceFlags::SURF_FLAG_AI_NOSIGHT);
				convert(IW3::CSurfaceFlags::SURF_FLAG_CLIPSHOT, H1::CSurfaceFlags::SURF_FLAG_CLIPSHOT);
				convert(IW3::CSurfaceFlags::SURF_FLAG_PLAYERCLIP, H1::CSurfaceFlags::SURF_FLAG_PLAYERCLIP);
				convert(IW3::CSurfaceFlags::SURF_FLAG_MONSTERCLIP, H1::CSurfaceFlags::SURF_FLAG_MONSTERCLIP);
				convert(IW3::CSurfaceFlags::SURF_FLAG_VEHICLECLIP, H1::CSurfaceFlags::SURF_FLAG_VEHICLECLIP);
				convert(IW3::CSurfaceFlags::SURF_FLAG_ITEMCLIP, H1::CSurfaceFlags::SURF_FLAG_ITEMCLIP);
				convert(IW3::CSurfaceFlags::SURF_FLAG_NODROP, H1::CSurfaceFlags::SURF_FLAG_NODROP);
				convert(IW3::CSurfaceFlags::SURF_FLAG_NONSOLID, H1::CSurfaceFlags::SURF_FLAG_NONSOLID);
				convert(IW3::CSurfaceFlags::SURF_FLAG_DETAIL, H1::CSurfaceFlags::SURF_FLAG_DETAIL);
				convert(IW3::CSurfaceFlags::SURF_FLAG_STRUCTURAL, H1::CSurfaceFlags::SURF_FLAG_STRUCTURAL);
				convert(IW3::CSurfaceFlags::SURF_FLAG_PORTAL, H1::CSurfaceFlags::SURF_FLAG_PORTAL);
				convert(IW3::CSurfaceFlags::SURF_FLAG_CANSHOOTCLIP, H1::CSurfaceFlags::SURF_FLAG_CANSHOOTCLIP);
				convert(IW3::CSurfaceFlags::SURF_FLAG_ORIGIN, H1::CSurfaceFlags::SURF_FLAG_ORIGIN);
				convert(IW3::CSurfaceFlags::SURF_FLAG_SKY, H1::CSurfaceFlags::SURF_FLAG_SKY);
				convert(IW3::CSurfaceFlags::SURF_FLAG_NOCASTSHADOW, H1::CSurfaceFlags::SURF_FLAG_NOCASTSHADOW);
				convert(IW3::CSurfaceFlags::SURF_FLAG_PHYSICSGEOM, H1::CSurfaceFlags::SURF_FLAG_PHYSICSGEOM);
				convert(IW3::CSurfaceFlags::SURF_FLAG_LIGHTPORTAL, H1::CSurfaceFlags::SURF_FLAG_LIGHTPORTAL);
				convert(IW3::CSurfaceFlags::SURF_FLAG_SLICK, H1::CSurfaceFlags::SURF_FLAG_SLICK);
				convert(IW3::CSurfaceFlags::SURF_FLAG_NOIMPACT, H1::CSurfaceFlags::SURF_FLAG_NOIMPACT);
				convert(IW3::CSurfaceFlags::SURF_FLAG_NOMARKS, H1::CSurfaceFlags::SURF_FLAG_NOMARKS);
				convert(IW3::CSurfaceFlags::SURF_FLAG_NOPENETRATE, H1::CSurfaceFlags::SURF_FLAG_NOPENETRATE);
				convert(IW3::CSurfaceFlags::SURF_FLAG_LADDER, H1::CSurfaceFlags::SURF_FLAG_LADDER);
				convert(IW3::CSurfaceFlags::SURF_FLAG_NODAMAGE, H1::CSurfaceFlags::SURF_FLAG_NODAMAGE);
				convert(IW3::CSurfaceFlags::SURF_FLAG_MANTLEON, H1::CSurfaceFlags::SURF_FLAG_MANTLEON);
				convert(IW3::CSurfaceFlags::SURF_FLAG_MANTLEOVER, H1::CSurfaceFlags::SURF_FLAG_MANTLEOVER);
				convert(IW3::CSurfaceFlags::SURF_FLAG_NOSTEPS, H1::CSurfaceFlags::SURF_FLAG_NOSTEPS);
				convert(IW3::CSurfaceFlags::SURF_FLAG_NODRAW, H1::CSurfaceFlags::SURF_FLAG_NODRAW);
				convert(IW3::CSurfaceFlags::SURF_FLAG_NOLIGHTMAP, H1::CSurfaceFlags::SURF_FLAG_NOLIGHTMAP);
				convert(IW3::CSurfaceFlags::SURF_FLAG_NODLIGHT, H1::CSurfaceFlags::SURF_FLAG_NODLIGHT);
				return h1_flags;
			}
		}

		void GenerateH1ClipInfo(H1::ClipInfo* info, clipMap_t* asset, ZoneMemory* mem)
		{
			info->planeCount = asset->planeCount;
			REINTERPRET_CAST_SAFE(info->planes, asset->planes);

			info->numMaterials = asset->numMaterials;
			info->materials = mem->Alloc<H1::ClipMaterial>(info->numMaterials);
			for (unsigned int i = 0; i < info->numMaterials; i++)
			{
				info->materials[i].name = asset->materials[i].material;
				info->materials[i].surfaceFlags = convert_surf_flags(asset->materials[i].surfaceFlags);
				info->materials[i].contents = asset->materials[i].contentFlags;
			}

			info->bCollisionTree.leafbrushNodesCount = asset->leafbrushNodesCount;
			info->bCollisionTree.leafbrushNodes = mem->Alloc<H1::cLeafBrushNode_s>(info->bCollisionTree.leafbrushNodesCount);
			for (unsigned int i = 0; i < info->bCollisionTree.leafbrushNodesCount; i++)
			{
				info->bCollisionTree.leafbrushNodes[i].axis = asset->leafbrushNodes[i].axis;
				info->bCollisionTree.leafbrushNodes[i].leafBrushCount = asset->leafbrushNodes[i].leafBrushCount;
				info->bCollisionTree.leafbrushNodes[i].contents = asset->leafbrushNodes[i].contents;
				if (info->bCollisionTree.leafbrushNodes[i].leafBrushCount > 0)
				{
					REINTERPRET_CAST_SAFE(info->bCollisionTree.leafbrushNodes[i].data.leaf.brushes, asset->leafbrushNodes[i].data.leaf.brushes);
				}
				else
				{
					info->bCollisionTree.leafbrushNodes[i].data.children.dist = asset->leafbrushNodes[i].data.children.dist;
					info->bCollisionTree.leafbrushNodes[i].data.children.range = asset->leafbrushNodes[i].data.children.range;
					
					info->bCollisionTree.leafbrushNodes[i].data.children.childOffset[0] = asset->leafbrushNodes[i].data.children.childOffset[0];
					info->bCollisionTree.leafbrushNodes[i].data.children.childOffset[1] = asset->leafbrushNodes[i].data.children.childOffset[1];
				}
			}

			info->bCollisionTree.numLeafBrushes = asset->numLeafBrushes;
			REINTERPRET_CAST_SAFE(info->bCollisionTree.leafbrushes, asset->leafbrushes);

			info->pCollisionTree.aabbTreeCount = asset->aabbTreeCount;
			info->pCollisionTree.aabbTrees = mem->Alloc<H1::CollisionAabbTree>(info->pCollisionTree.aabbTreeCount);
			for (int i = 0; i < info->pCollisionTree.aabbTreeCount; i++)
			{
				std::memcpy(info->pCollisionTree.aabbTrees[i].midPoint, asset->aabbTrees[i].origin, sizeof(float[3]));
				std::memcpy(info->pCollisionTree.aabbTrees[i].halfSize, asset->aabbTrees[i].halfSize, sizeof(float[3]));
				info->pCollisionTree.aabbTrees[i].materialIndex = asset->aabbTrees[i].materialIndex;
				info->pCollisionTree.aabbTrees[i].childCount = asset->aabbTrees[i].childCount;

				info->pCollisionTree.aabbTrees[i].u.firstChildIndex = asset->aabbTrees[i].u.firstChildIndex;
				//info->pCollisionTree.aabbTrees[i].u.partitionIndex = asset->aabbTrees[i].u.partitionIndex;
			}

			info->sCollisionTree.numStaticModels = asset->numStaticModels;
			info->sCollisionTree.smodelNodeCount = 1;
			info->sCollisionTree.smodelNodes = mem->Alloc<H1::SModelAabbNode>(info->sCollisionTree.smodelNodeCount);
			if (asset->numStaticModels == 0)
			{
				info->sCollisionTree.smodelNodes[0].bounds.halfSize[0] = -131072.000f;
				info->sCollisionTree.smodelNodes[0].bounds.halfSize[1] = -131072.000f;
				info->sCollisionTree.smodelNodes[0].bounds.halfSize[2] = -131072.000f;
			}
			else
			{
				float maxs[3];
				float mins[3];

				maxs[0] = asset->staticModelList[0].absmax[0];
				maxs[1] = asset->staticModelList[0].absmax[1];
				maxs[2] = asset->staticModelList[0].absmax[2];

				mins[0] = asset->staticModelList[0].absmin[0];
				mins[1] = asset->staticModelList[0].absmin[1];
				mins[2] = asset->staticModelList[0].absmin[2];

				for (unsigned int i = 1; i < asset->numStaticModels; i++)
				{
					maxs[0] = std::max(maxs[0], asset->staticModelList[i].absmax[0]);
					maxs[1] = std::max(maxs[1], asset->staticModelList[i].absmax[1]);
					maxs[2] = std::max(maxs[2], asset->staticModelList[i].absmax[2]);

					mins[0] = std::min(mins[0], asset->staticModelList[i].absmin[0]);
					mins[1] = std::min(mins[1], asset->staticModelList[i].absmin[1]);
					mins[2] = std::min(mins[2], asset->staticModelList[i].absmin[2]);
				}

				memcpy(&info->sCollisionTree.smodelNodes[0].bounds, bounds::compute(mins, maxs), sizeof(float[2][3]));
				info->sCollisionTree.smodelNodes[0].childCount = static_cast<short>(asset->numStaticModels);
				info->sCollisionTree.smodelNodes[0].firstChild = 0;
			}

			std::unordered_map<cbrushside_t*, H1::cbrushside_t*> mapped_brush_sides;

			info->bCollisionData.numBrushSides = asset->numBrushSides;
			info->bCollisionData.brushSides = mem->Alloc<H1::cbrushside_t>(info->bCollisionData.numBrushSides);
			for (unsigned int i = 0; i < info->bCollisionData.numBrushSides; i++)
			{
				mapped_brush_sides[&asset->brushsides[i]] = &info->bCollisionData.brushSides[i];

				// check
				info->bCollisionData.brushSides[i].planeIndex =
					(reinterpret_cast<std::uintptr_t>(asset->brushsides[i].plane) - reinterpret_cast<std::uintptr_t>(asset->planes)) / sizeof(cplane_s);
				assert(info->bCollisionData.brushSides[i].planeIndex <= info->planeCount);

				info->bCollisionData.brushSides[i].materialNum = asset->brushsides[i].materialNum;
				info->bCollisionData.brushSides[i].firstAdjacentSideOffset = asset->brushsides[i].firstAdjacentSideOffset;
				info->bCollisionData.brushSides[i].edgeCount = asset->brushsides[i].edgeCount;
			}

			info->bCollisionData.numBrushEdges = asset->numBrushEdges;
			REINTERPRET_CAST_SAFE(info->bCollisionData.brushEdges, asset->brushEdges);

			info->bCollisionData.numBrushes = asset->numBrushes;
			info->bCollisionData.brushes = mem->Alloc<H1::cbrush_t>(info->bCollisionData.numBrushes);
			for (unsigned int i = 0; i < info->bCollisionData.numBrushes; i++)
			{
				info->bCollisionData.brushes[i].numsides = asset->brushes[i].numsides;
				info->bCollisionData.brushes[i].glassPieceIndex = 0;

				if (asset->brushes[i].sides)
					info->bCollisionData.brushes[i].sides = mapped_brush_sides.find(asset->brushes[i].sides)->second;

				REINTERPRET_CAST_SAFE(info->bCollisionData.brushes[i].baseAdjacentSide, asset->brushes[i].baseAdjacentSide);

				memcpy(&info->bCollisionData.brushes[i].axialMaterialNum, &asset->brushes[i].axialMaterialNum, sizeof(info->bCollisionData.brushes[i].axialMaterialNum));
				info->bCollisionData.brushes[i].firstAdjacentSideOffsets[0][0] = static_cast<unsigned char>(asset->brushes[i].firstAdjacentSideOffsets[0][0]);
				info->bCollisionData.brushes[i].firstAdjacentSideOffsets[0][1] = static_cast<unsigned char>(asset->brushes[i].firstAdjacentSideOffsets[0][1]);
				info->bCollisionData.brushes[i].firstAdjacentSideOffsets[0][2] = static_cast<unsigned char>(asset->brushes[i].firstAdjacentSideOffsets[0][2]);
				info->bCollisionData.brushes[i].firstAdjacentSideOffsets[1][0] = static_cast<unsigned char>(asset->brushes[i].firstAdjacentSideOffsets[1][0]);
				info->bCollisionData.brushes[i].firstAdjacentSideOffsets[1][1] = static_cast<unsigned char>(asset->brushes[i].firstAdjacentSideOffsets[1][1]);
				info->bCollisionData.brushes[i].firstAdjacentSideOffsets[1][2] = static_cast<unsigned char>(asset->brushes[i].firstAdjacentSideOffsets[1][2]);
				memcpy(&info->bCollisionData.brushes[i].edgeCount, &asset->brushes[i].edgeCount, sizeof(info->bCollisionData.brushes[i].edgeCount));
			}

			info->bCollisionData.brushBounds = mem->Alloc<H1::Bounds>(info->bCollisionData.numBrushes);
			for (unsigned int i = 0; i < info->bCollisionData.numBrushes; i++)
			{
				memcpy(&info->bCollisionData.brushBounds[i], bounds::compute(asset->brushes[i].mins, asset->brushes[i].maxs), sizeof(float[2][3]));
			}

			info->bCollisionData.brushContents = mem->Alloc<int>(info->bCollisionData.numBrushes);
			for (unsigned int i = 0; i < info->bCollisionData.numBrushes; i++)
			{
				info->bCollisionData.brushContents[i] = asset->brushes[i].contents;
			}

			info->pCollisionData.vertCount = asset->vertCount;
			REINTERPRET_CAST_SAFE(info->pCollisionData.verts, asset->verts);
			info->pCollisionData.triCount = asset->triCount;
			REINTERPRET_CAST_SAFE(info->pCollisionData.triIndices, asset->triIndices);
			REINTERPRET_CAST_SAFE(info->pCollisionData.triEdgeIsWalkable, asset->triEdgeIsWalkable);
			info->pCollisionData.borderCount = asset->borderCount;
			REINTERPRET_CAST_SAFE(info->pCollisionData.borders, asset->borders);
			info->pCollisionData.partitionCount = asset->partitionCount;
			info->pCollisionData.partitions = mem->Alloc<H1::CollisionPartition>(info->pCollisionData.partitionCount);
			for (int i = 0; i < info->pCollisionData.partitionCount; i++)
			{
				info->pCollisionData.partitions[i].triCount = asset->partitions[i].triCount;
				info->pCollisionData.partitions[i].borderCount = asset->partitions[i].borderCount;
				info->pCollisionData.partitions[i].firstVertSegment = 0;
				info->pCollisionData.partitions[i].firstTri = asset->partitions[i].firstTri;
				REINTERPRET_CAST_SAFE(info->pCollisionData.partitions[i].borders, asset->partitions[i].borders);
			}

			info->sCollisionData.numStaticModels = asset->numStaticModels;
			info->sCollisionData.staticModelList = mem->Alloc<H1::cStaticModel_s>(info->sCollisionData.numStaticModels);
			for (unsigned int i = 0; i < info->sCollisionData.numStaticModels; i++)
			{
				info->sCollisionData.staticModelList[i].xmodel = reinterpret_cast<H1::XModel * __ptr64>(asset->staticModelList[i].xmodel);

				memcpy(&info->sCollisionData.staticModelList[i].origin, &asset->staticModelList[i].origin, sizeof(float[3]));
				memcpy(&info->sCollisionData.staticModelList[i].invScaledAxis, &asset->staticModelList[i].invScaledAxis, sizeof(float[3][3]));
				memcpy(&info->sCollisionData.staticModelList[i].absBounds, bounds::compute(asset->staticModelList[i].absmin, asset->staticModelList[i].absmax), sizeof(float[2][3]));
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

			h1_asset->numNodes = asset->numNodes;
			h1_asset->nodes = mem->Alloc<H1::cNode_t>(h1_asset->numNodes);
			for (unsigned int i = 0; i < h1_asset->numNodes; i++)
			{
				REINTERPRET_CAST_SAFE(h1_asset->nodes[i].plane, asset->nodes[i].plane);
				h1_asset->nodes[i].children[0] = asset->nodes[i].children[0];
				h1_asset->nodes[i].children[1] = asset->nodes[i].children[1];
			}

			h1_asset->numLeafs = asset->numLeafs;
			h1_asset->leafs = mem->Alloc<H1::cLeaf_t>(h1_asset->numLeafs);
			for (unsigned int i = 0; i < h1_asset->numLeafs; i++)
			{
				h1_asset->leafs[i].firstCollAabbIndex = asset->leafs[i].firstCollAabbIndex;
				h1_asset->leafs[i].collAabbCount = asset->leafs[i].collAabbCount;
				h1_asset->leafs[i].brushContents = asset->leafs[i].brushContents;
				h1_asset->leafs[i].terrainContents = asset->leafs[i].terrainContents;
				memcpy(&h1_asset->leafs[i].bounds, bounds::compute(asset->leafs[i].mins, asset->leafs[i].maxs), sizeof(float[2][3]));
				h1_asset->leafs[i].leafBrushNode = asset->leafs[i].leafBrushNode;
			}

			h1_asset->numSubModels = asset->numSubModels;
			h1_asset->cmodels = mem->Alloc<H1::cmodel_t>(h1_asset->numSubModels);
			for (unsigned int i = 0; i < h1_asset->numSubModels; i++)
			{
				memcpy(&h1_asset->cmodels[i].bounds, bounds::compute(asset->cmodels[i].mins, asset->cmodels[i].maxs), sizeof(float[2][3]));
				h1_asset->cmodels[i].radius = asset->cmodels[i].radius;

				h1_asset->cmodels[i].info = nullptr; // mapped in h1

				h1_asset->cmodels[i].leaf.firstCollAabbIndex = asset->cmodels[i].leaf.firstCollAabbIndex;
				h1_asset->cmodels[i].leaf.collAabbCount = asset->cmodels[i].leaf.collAabbCount;
				h1_asset->cmodels[i].leaf.brushContents = asset->cmodels[i].leaf.brushContents;
				h1_asset->cmodels[i].leaf.terrainContents = asset->cmodels[i].leaf.terrainContents;
				memcpy(&h1_asset->cmodels[i].leaf.bounds, bounds::compute(asset->cmodels[i].leaf.mins, asset->cmodels[i].leaf.maxs), sizeof(float[2][3]));
				h1_asset->cmodels[i].leaf.leafBrushNode = asset->cmodels[i].leaf.leafBrushNode;
			}

			h1_asset->mapEnts = mem->Alloc<H1::MapEnts>();
			h1_asset->mapEnts->name = asset->mapEnts->name;

			auto* h1_mapEnts = h1_asset->mapEnts;

			// add triggers to mapEnts - iw3xpo
			if (asset->cmodels)
			{
				h1_mapEnts->trigger.count = asset->numSubModels;
				h1_mapEnts->trigger.hullCount = asset->numSubModels;

				h1_mapEnts->trigger.models = mem->Alloc<H1::TriggerModel>(asset->numSubModels);
				h1_mapEnts->trigger.hulls = mem->Alloc<H1::TriggerHull>(asset->numSubModels);

				std::vector<H1::TriggerSlab>* slabs = new std::vector<H1::TriggerSlab>();

				for (int i = 0; i < asset->numSubModels; ++i)
				{
					H1::TriggerHull* trigHull = &h1_mapEnts->trigger.hulls[i];

					memcpy(&trigHull->bounds, bounds::compute(asset->cmodels[i].mins, asset->cmodels[i].maxs), sizeof(float[2][3]));
					trigHull->contents = asset->cmodels[i].leaf.brushContents | asset->cmodels[i].leaf.terrainContents;

					H1::TriggerModel* trigMod = &h1_mapEnts->trigger.models[i];
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
								H1::TriggerSlab curSlab;
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

				h1_mapEnts->trigger.slabs = mem->Alloc<H1::TriggerSlab>(slabs->size());

				// Save slabs
				h1_mapEnts->trigger.slabCount = slabs->size();
				for (unsigned int i = 0; i < slabs->size(); i++)
				{
					H1::TriggerSlab slab = (*slabs)[i];
					std::memcpy(&h1_mapEnts->trigger.slabs[i], &slab, sizeof(H1::TriggerSlab));
				}

				delete slabs;
			}

			// dump triggers
			assetmanager::dumper trigger_dumper;
			if (trigger_dumper.open(h1_mapEnts->name + ".ents.triggers"s))
			{
				trigger_dumper.dump_int(h1_mapEnts->trigger.count);
				trigger_dumper.dump_array<H1::TriggerModel>(h1_mapEnts->trigger.models, h1_mapEnts->trigger.count);

				trigger_dumper.dump_int(h1_mapEnts->trigger.hullCount);
				trigger_dumper.dump_array<H1::TriggerHull>(h1_mapEnts->trigger.hulls, h1_mapEnts->trigger.hullCount);

				trigger_dumper.dump_int(h1_mapEnts->trigger.slabCount);
				trigger_dumper.dump_array<H1::TriggerSlab>(h1_mapEnts->trigger.slabs, h1_mapEnts->trigger.slabCount);

				trigger_dumper.close();
			}

			h1_asset->stageCount = 1;
			h1_asset->stages = mem->Alloc<H1::Stage>(h1_asset->stageCount);
			for (unsigned int i = 0; i < h1_asset->stageCount; i++)
			{
				h1_asset->stages[i].name = mem->StrDup(va("stage %i", i));
				memset(&h1_asset->stages[i].origin, 0, sizeof(float[3]));
				h1_asset->stages[i].triggerIndex = 0x400;
				h1_asset->stages[i].sunPrimaryLightIndex = 0x1;
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
				h1_asset->dynEntCollList[i] = reinterpret_cast<H1::DynEntityColl * __ptr64>(asset->dynEntCollList[i]); // check
				for (unsigned short j = 0; j < h1_asset->dynEntCount[i]; j++)
				{
					h1_asset->dynEntDefList[i][j].type = static_cast<H1::DynEntityType>(asset->dynEntDefList[i][j].type);
					memcpy(&h1_asset->dynEntDefList[i][j].pose, &asset->dynEntDefList[i][j].pose, sizeof(IW3::GfxPlacement));
					h1_asset->dynEntDefList[i][j].baseModel = reinterpret_cast<H1::XModel * __ptr64>(asset->dynEntDefList[i][j].xModel);
					h1_asset->dynEntDefList[i][j].brushModel = asset->dynEntDefList[i][j].brushModel;
					h1_asset->dynEntDefList[i][j].physicsBrushModel = asset->dynEntDefList[i][j].physicsBrushModel;
					h1_asset->dynEntDefList[i][j].health = asset->dynEntDefList[i][j].health;
					h1_asset->dynEntDefList[i][j].destroyFx = reinterpret_cast<H1::FxEffectDef * __ptr64>(asset->dynEntDefList[i][j].destroyFx);
					h1_asset->dynEntDefList[i][j].sound = nullptr;
					h1_asset->dynEntDefList[i][j].physPreset = reinterpret_cast<H1::PhysPreset * __ptr64>(asset->dynEntDefList[i][j].physPreset);
					h1_asset->dynEntDefList[i][j].hinge = nullptr;
					h1_asset->dynEntDefList[i][j].linkTo = nullptr;
					memcpy(&h1_asset->dynEntDefList[i][j].mass, &asset->dynEntDefList[i][j].mass, sizeof(IW3::PhysMass));
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
					h1_asset->dynEntClientList[i][j].contents = asset->dynEntDefList[i][j].contents;
				}
			}

			h1_asset->dynEntAnchorCount = 0;
			h1_asset->dynEntAnchorNames = nullptr;

			h1_asset->scriptableMapEnts;

			h1_asset->grappleData;

			h1_asset->checksum = asset->checksum;

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