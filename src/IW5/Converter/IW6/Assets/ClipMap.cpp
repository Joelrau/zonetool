#include "stdafx.hpp"
#include "../Include.hpp"

#include "ClipMap.hpp"

namespace ZoneTool::IW5
{
	namespace IW6Converter
	{
		namespace
		{
			IW6::CSurfaceFlags surf_flags_conversion_table[31]
			{
				IW6::SURF_FLAG_DEFAULT,
				IW6::SURF_FLAG_BARK,
				IW6::SURF_FLAG_BRICK,
				IW6::SURF_FLAG_CARPET,
				IW6::SURF_FLAG_CLOTH,
				IW6::SURF_FLAG_CONCRETE,
				IW6::SURF_FLAG_DIRT,
				IW6::SURF_FLAG_FLESH,
				IW6::SURF_FLAG_FOLIAGE,
				IW6::SURF_FLAG_GLASS,
				IW6::SURF_FLAG_GRASS,
				IW6::SURF_FLAG_GRAVEL,
				IW6::SURF_FLAG_ICE,
				IW6::SURF_FLAG_METAL,
				IW6::SURF_FLAG_MUD,
				IW6::SURF_FLAG_PAPER,
				IW6::SURF_FLAG_PLASTER,
				IW6::SURF_FLAG_ROCK,
				IW6::SURF_FLAG_SAND,
				IW6::SURF_FLAG_SNOW,
				IW6::SURF_FLAG_WATER,
				IW6::SURF_FLAG_WOOD,
				IW6::SURF_FLAG_ASPHALT,
				IW6::SURF_FLAG_CERAMIC,
				IW6::SURF_FLAG_PLASTIC,
				IW6::SURF_FLAG_RUBBER,
				IW6::SURF_FLAG_CUSHION,
				IW6::SURF_FLAG_FRUIT,
				IW6::SURF_FLAG_PAINTEDMETAL,
				IW6::SURF_FLAG_RIOTSHIELD,
				IW6::SURF_FLAG_SLUSH,
			}; IW5::CSurfaceFlags;

			int convert_surf_flags(int flags)
			{
				int iw6_flags = surf_flags_conversion_table[flags >> 20];
				auto convert = [&](IW5::CSurfaceFlags a, IW6::CSurfaceFlags b)
				{
					iw6_flags |= ((flags & a) == a) ? b : 0;
				};
				convert(IW5::CSurfaceFlags::SURF_FLAG_OPAQUEGLASS, IW6::CSurfaceFlags::SURF_FLAG_OPAQUEGLASS);
				convert(IW5::CSurfaceFlags::SURF_FLAG_CLIPMISSILE, IW6::CSurfaceFlags::SURF_FLAG_CLIPMISSILE);
				convert(IW5::CSurfaceFlags::SURF_FLAG_AI_NOSIGHT, IW6::CSurfaceFlags::SURF_FLAG_AI_NOSIGHT);
				convert(IW5::CSurfaceFlags::SURF_FLAG_CLIPSHOT, IW6::CSurfaceFlags::SURF_FLAG_CLIPSHOT);
				convert(IW5::CSurfaceFlags::SURF_FLAG_PLAYERCLIP, IW6::CSurfaceFlags::SURF_FLAG_PLAYERCLIP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_MONSTERCLIP, IW6::CSurfaceFlags::SURF_FLAG_MONSTERCLIP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_AICLIPALLOWDEATH, IW6::CSurfaceFlags::SURF_FLAG_AICLIPALLOWDEATH);
				convert(IW5::CSurfaceFlags::SURF_FLAG_VEHICLECLIP, IW6::CSurfaceFlags::SURF_FLAG_VEHICLECLIP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_ITEMCLIP, IW6::CSurfaceFlags::SURF_FLAG_ITEMCLIP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NODROP, IW6::CSurfaceFlags::SURF_FLAG_NODROP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NONSOLID, IW6::CSurfaceFlags::SURF_FLAG_NONSOLID);
				convert(IW5::CSurfaceFlags::SURF_FLAG_DETAIL, IW6::CSurfaceFlags::SURF_FLAG_DETAIL);
				convert(IW5::CSurfaceFlags::SURF_FLAG_STRUCTURAL, IW6::CSurfaceFlags::SURF_FLAG_STRUCTURAL);
				convert(IW5::CSurfaceFlags::SURF_FLAG_PORTAL, IW6::CSurfaceFlags::SURF_FLAG_PORTAL);
				convert(IW5::CSurfaceFlags::SURF_FLAG_CANSHOOTCLIP, IW6::CSurfaceFlags::SURF_FLAG_CANSHOOTCLIP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_ORIGIN, IW6::CSurfaceFlags::SURF_FLAG_ORIGIN);
				convert(IW5::CSurfaceFlags::SURF_FLAG_SKY, IW6::CSurfaceFlags::SURF_FLAG_SKY);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NOCASTSHADOW, IW6::CSurfaceFlags::SURF_FLAG_NOCASTSHADOW);
				convert(IW5::CSurfaceFlags::SURF_FLAG_PHYSICSGEOM, IW6::CSurfaceFlags::SURF_FLAG_PHYSICSGEOM);
				convert(IW5::CSurfaceFlags::SURF_FLAG_LIGHTPORTAL, IW6::CSurfaceFlags::SURF_FLAG_LIGHTPORTAL);
				convert(IW5::CSurfaceFlags::SURF_FLAG_OUTDOORBOUNDS, IW6::CSurfaceFlags::SURF_FLAG_OUTDOORBOUNDS);
				convert(IW5::CSurfaceFlags::SURF_FLAG_SLICK, IW6::CSurfaceFlags::SURF_FLAG_SLICK);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NOIMPACT, IW6::CSurfaceFlags::SURF_FLAG_NOIMPACT);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NOMARKS, IW6::CSurfaceFlags::SURF_FLAG_NOMARKS);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NOPENETRATE, IW6::CSurfaceFlags::SURF_FLAG_NOPENETRATE);
				convert(IW5::CSurfaceFlags::SURF_FLAG_LADDER, IW6::CSurfaceFlags::SURF_FLAG_LADDER);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NODAMAGE, IW6::CSurfaceFlags::SURF_FLAG_NODAMAGE);
				convert(IW5::CSurfaceFlags::SURF_FLAG_MANTLEON, IW6::CSurfaceFlags::SURF_FLAG_MANTLEON);
				convert(IW5::CSurfaceFlags::SURF_FLAG_MANTLEOVER, IW6::CSurfaceFlags::SURF_FLAG_MANTLEOVER);
				convert(IW5::CSurfaceFlags::SURF_FLAG_STAIRS, IW6::CSurfaceFlags::SURF_FLAG_STAIRS);
				convert(IW5::CSurfaceFlags::SURF_FLAG_SOFT, IW6::CSurfaceFlags::SURF_FLAG_SOFT);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NOSTEPS, IW6::CSurfaceFlags::SURF_FLAG_NOSTEPS);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NODRAW, IW6::CSurfaceFlags::SURF_FLAG_NODRAW);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NOLIGHTMAP, IW6::CSurfaceFlags::SURF_FLAG_NOLIGHTMAP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NODLIGHT, IW6::CSurfaceFlags::SURF_FLAG_NODLIGHT);
				return iw6_flags;
			}
		}

		void GenerateIW6ClipInfo(IW6::ClipInfo* info, IW5::ClipInfo* dinfo, allocator& mem)
		{
			if (!dinfo)
			{
				info = nullptr;
				return;
			}

			info->planeCount = dinfo->planeCount;
			info->planes = reinterpret_cast<IW6::cplane_s*>(dinfo->planes);

			info->numMaterials = dinfo->numMaterials;
			info->materials = mem.allocate<IW6::ClipMaterial>(info->numMaterials);
			for (unsigned int i = 0; i < info->numMaterials; i++)
			{
				info->materials[i].name = dinfo->materials[i].name;
				info->materials[i].surfaceFlags = convert_surf_flags(dinfo->materials[i].surfaceFlags);
				info->materials[i].contents = dinfo->materials[i].contents;
			}

			std::unordered_map<cbrushside_t*, IW6::cbrushside_t*> mapped_brush_sides;

			info->numBrushSides = dinfo->numBrushSides;
			info->brushsides = mem.allocate<IW6::cbrushside_t>(info->numBrushSides);
			for (unsigned int i = 0; i < info->numBrushSides; i++)
			{
				mapped_brush_sides[&dinfo->brushsides[i]] = &info->brushsides[i];

				info->brushsides[i].plane = reinterpret_cast<IW6::cplane_s*>(dinfo->brushsides[i].plane);
				info->brushsides[i].materialNum = dinfo->brushsides[i].materialNum;
				info->brushsides[i].firstAdjacentSideOffset = dinfo->brushsides[i].firstAdjacentSideOffset;
				info->brushsides[i].edgeCount = dinfo->brushsides[i].edgeCount;
			}

			info->numBrushEdges = dinfo->numBrushEdges;
			info->brushEdges = reinterpret_cast<IW6::cbrushedge_t*>(dinfo->brushEdges);

			info->leafbrushNodesCount = dinfo->leafbrushNodesCount;
			info->leafbrushNodes = mem.allocate<IW6::cLeafBrushNode_s>(info->leafbrushNodesCount);
			for (unsigned int i = 0; i < info->leafbrushNodesCount; i++)
			{
				info->leafbrushNodes[i].axis = dinfo->leafbrushNodes[i].axis;
				info->leafbrushNodes[i].leafBrushCount = dinfo->leafbrushNodes[i].leafBrushCount;
				info->leafbrushNodes[i].contents = dinfo->leafbrushNodes[i].contents;
				if (info->leafbrushNodes[i].leafBrushCount > 0)
				{
					info->leafbrushNodes[i].data.leaf.brushes = dinfo->leafbrushNodes[i].data.leaf.brushes;
				}
				else
				{
					memcpy(&info->leafbrushNodes[i].data.children, &dinfo->leafbrushNodes[i].data.children,
						sizeof(dinfo->leafbrushNodes[i].data.children));
				}
			}

			info->numLeafBrushes = dinfo->numLeafBrushes;
			info->leafbrushes = dinfo->leafbrushes;

			info->numBrushes = dinfo->numBrushes;
			info->brushes = mem.allocate<IW6::cbrush_t>(info->numBrushes);
			for (unsigned int i = 0; i < info->numBrushes; i++)
			{
				info->brushes[i].numsides = dinfo->brushes[i].numsides;
				info->brushes[i].glassPieceIndex = dinfo->brushes[i].glassPieceIndex;

				if (dinfo->brushes[i].sides)
					info->brushes[i].sides = mapped_brush_sides.find(dinfo->brushes[i].sides)->second;

				//info->brushes[i].sides = reinterpret_cast<IW6::cbrushside_t*>(dinfo->brushes[i].sides);
				info->brushes[i].baseAdjacentSide = reinterpret_cast<IW6::cbrushedge_t*>(dinfo->brushes[i].baseAdjacentSide);

				info->brushes[i].axialMaterialNum[0][0] = dinfo->brushes[i].axialMaterialNum[0][0];
				info->brushes[i].axialMaterialNum[0][1] = dinfo->brushes[i].axialMaterialNum[0][1];
				info->brushes[i].axialMaterialNum[0][2] = dinfo->brushes[i].axialMaterialNum[0][2];
				info->brushes[i].axialMaterialNum[1][0] = dinfo->brushes[i].axialMaterialNum[1][0];
				info->brushes[i].axialMaterialNum[1][1] = dinfo->brushes[i].axialMaterialNum[1][1];
				info->brushes[i].axialMaterialNum[1][2] = dinfo->brushes[i].axialMaterialNum[1][2];

				info->brushes[i].firstAdjacentSideOffsets[0][0] = dinfo->brushes[i].firstAdjacentSideOffsets[0][0];
				info->brushes[i].firstAdjacentSideOffsets[0][1] = dinfo->brushes[i].firstAdjacentSideOffsets[0][1];
				info->brushes[i].firstAdjacentSideOffsets[0][2] = dinfo->brushes[i].firstAdjacentSideOffsets[0][2];
				info->brushes[i].firstAdjacentSideOffsets[1][0] = dinfo->brushes[i].firstAdjacentSideOffsets[1][0];
				info->brushes[i].firstAdjacentSideOffsets[1][1] = dinfo->brushes[i].firstAdjacentSideOffsets[1][1];
				info->brushes[i].firstAdjacentSideOffsets[1][2] = dinfo->brushes[i].firstAdjacentSideOffsets[1][2];

				info->brushes[i].edgeCount[0][0] = dinfo->brushes[i].edgeCount[0][0];
				info->brushes[i].edgeCount[0][1] = dinfo->brushes[i].edgeCount[0][1];
				info->brushes[i].edgeCount[0][2] = dinfo->brushes[i].edgeCount[0][2];
				info->brushes[i].edgeCount[1][0] = dinfo->brushes[i].edgeCount[1][0];
				info->brushes[i].edgeCount[1][1] = dinfo->brushes[i].edgeCount[1][1];
				info->brushes[i].edgeCount[1][2] = dinfo->brushes[i].edgeCount[1][2];
			}

			info->brushBounds = reinterpret_cast<IW6::Bounds*>(dinfo->brushBounds);
			info->brushContents = dinfo->brushContents;
		}

		IW6::clipMap_t* GenerateIW6ClipMap(clipMap_t* asset, allocator& mem)
		{
			// allocate IW6 clipMap_t structure
			const auto iw6_asset = mem.allocate<IW6::clipMap_t>();

			iw6_asset->name = asset->name;
			iw6_asset->isInUse = asset->isInUse;
			GenerateIW6ClipInfo(&iw6_asset->info, &asset->info, mem);
			iw6_asset->pInfo = &iw6_asset->info;
			iw6_asset->numStaticModels = asset->numStaticModels;
			iw6_asset->staticModelList = mem.allocate<IW6::cStaticModel_s>(iw6_asset->numStaticModels);
			for (unsigned int i = 0; i < iw6_asset->numStaticModels; i++)
			{
				iw6_asset->staticModelList[i].xmodel = reinterpret_cast<IW6::XModel*>(asset->staticModelList[i].xmodel);
				memcpy(&iw6_asset->staticModelList[i].origin, &asset->staticModelList[i].origin, sizeof(IW5::cStaticModel_s) - sizeof(IW5::XModel*));
			}
			iw6_asset->numNodes = asset->numNodes;
			iw6_asset->nodes = mem.allocate<IW6::cNode_t>(iw6_asset->numNodes);
			for (unsigned int i = 0; i < iw6_asset->numNodes; i++)
			{
				iw6_asset->nodes[i].plane = reinterpret_cast<IW6::cplane_s*>(asset->nodes[i].plane);
				iw6_asset->nodes[i].children[0] = asset->nodes[i].children[0];
				iw6_asset->nodes[i].children[1] = asset->nodes[i].children[1];
			}
			iw6_asset->numLeafs = asset->numLeafs;
			iw6_asset->leafs = reinterpret_cast<IW6::cLeaf_t *>(asset->leafs);
			iw6_asset->vertCount = asset->vertCount;
			iw6_asset->verts = reinterpret_cast<IW6::vec3_t*>(asset->verts);
			iw6_asset->triCount = asset->triCount;
			iw6_asset->triIndices = asset->triIndices;
			iw6_asset->triEdgeIsWalkable = asset->triEdgeIsWalkable;
			iw6_asset->borderCount = asset->borderCount;
			iw6_asset->borders = reinterpret_cast<IW6::CollisionBorder *>(asset->borders);
			iw6_asset->partitionCount = asset->partitionCount;
			iw6_asset->partitions = mem.allocate<IW6::CollisionPartition>(iw6_asset->partitionCount);
			for (int i = 0; i < iw6_asset->partitionCount; i++)
			{
				iw6_asset->partitions[i].triCount = asset->partitions[i].triCount;
				iw6_asset->partitions[i].borderCount = asset->partitions[i].borderCount;
				iw6_asset->partitions[i].firstVertSegment = asset->partitions[i].firstVertSegment;
				iw6_asset->partitions[i].firstTri = asset->partitions[i].firstTri;
				iw6_asset->partitions[i].borders = reinterpret_cast<IW6::CollisionBorder*>(asset->partitions[i].borders);
			}
			iw6_asset->aabbTreeCount = asset->aabbTreeCount;
			iw6_asset->aabbTrees = reinterpret_cast<IW6::CollisionAabbTree*>(asset->aabbTreeCount);
			iw6_asset->numSubModels = asset->numSubModels;
			iw6_asset->cmodels = mem.allocate<IW6::cmodel_t>(iw6_asset->numSubModels);
			for (unsigned int i = 0; i < iw6_asset->numSubModels; i++)
			{
				memcpy(&iw6_asset->cmodels[i].bounds, &asset->cmodels[i].bounds, sizeof(IW5::Bounds) + sizeof(float));
				memcpy(&iw6_asset->cmodels[i].leaf, &asset->cmodels[i].leaf, sizeof(IW5::cLeaf_t));

				iw6_asset->cmodels[i].info = nullptr; // mapped in h1
			}

			iw6_asset->mapEnts = mem.allocate<IW6::MapEnts>();
			iw6_asset->mapEnts->name = asset->mapEnts->name;

			iw6_asset->stageCount = asset->stageCount;
			iw6_asset->stages = mem.allocate<IW6::Stage>(iw6_asset->stageCount);
			for (unsigned int i = 0; i < iw6_asset->stageCount; i++)
			{
				iw6_asset->stages[i].name = asset->stages[i].name;
				memcpy(&iw6_asset->stages[i].origin, &asset->stages[i].origin, sizeof(float[3]));
				iw6_asset->stages[i].triggerIndex = asset->stages[i].triggerIndex;
				iw6_asset->stages[i].sunPrimaryLightIndex = asset->stages[i].sunPrimaryLightIndex;
			}
			iw6_asset->stageTrigger.count = 0;
			iw6_asset->stageTrigger.models = nullptr;
			iw6_asset->stageTrigger.hullCount = 0;
			iw6_asset->stageTrigger.hulls = nullptr;
			iw6_asset->stageTrigger.slabCount = 0;
			iw6_asset->stageTrigger.slabs = nullptr;

			iw6_asset->smodelNodeCount = asset->smodelNodeCount;
			iw6_asset->smodelNodes = reinterpret_cast<IW6::SModelAabbNode*>(asset->smodelNodes);
			for (unsigned char i = 0; i < 2; i++)
			{
				iw6_asset->dynEntCount[i] = asset->dynEntCount[i];
				iw6_asset->dynEntDefList[i] = mem.allocate<IW6::DynEntityDef>(iw6_asset->dynEntCount[i]);
				iw6_asset->dynEntPoseList[i] = reinterpret_cast<IW6::DynEntityPose*>(asset->dynEntPoseList[i]);
				iw6_asset->dynEntClientList[i] = mem.allocate<IW6::DynEntityClient>(iw6_asset->dynEntCount[i]);
				iw6_asset->dynEntCollList[i] = reinterpret_cast<IW6::DynEntityColl*>(asset->dynEntCollList[i]);
				for (unsigned short j = 0; j < iw6_asset->dynEntCount[i]; j++)
				{
					iw6_asset->dynEntDefList[i][j].type = static_cast<IW6::DynEntityType>(asset->dynEntDefList[i][j].type);
					memcpy(&iw6_asset->dynEntDefList[i][j].pose, &asset->dynEntDefList[i][j].pose, sizeof(IW5::GfxPlacement));
					iw6_asset->dynEntDefList[i][j].baseModel = reinterpret_cast<IW6::XModel*>(asset->dynEntDefList[i][j].xModel);
					iw6_asset->dynEntDefList[i][j].brushModel = asset->dynEntDefList[i][j].brushModel;
					iw6_asset->dynEntDefList[i][j].physicsBrushModel = asset->dynEntDefList[i][j].physicsBrushModel;
					iw6_asset->dynEntDefList[i][j].health = asset->dynEntDefList[i][j].health;
					iw6_asset->dynEntDefList[i][j].destroyFx = reinterpret_cast<IW6::FxEffectDef*>(asset->dynEntDefList[i][j].destroyFx);
					iw6_asset->dynEntDefList[i][j].physPreset = reinterpret_cast<IW6::PhysPreset*>(asset->dynEntDefList[i][j].physPreset);
					iw6_asset->dynEntDefList[i][j].hinge = reinterpret_cast<IW6::DynEntityHingeDef*>(asset->dynEntDefList[i][j].hinge);
					iw6_asset->dynEntDefList[i][j].linkTo = nullptr;
					memcpy(&iw6_asset->dynEntDefList[i][j].mass, &asset->dynEntDefList[i][j].mass, sizeof(IW5::PhysMass) + sizeof(int));

					iw6_asset->dynEntClientList[i][j].physObjId = asset->dynEntClientList[i]->physObjId;
					iw6_asset->dynEntClientList[i][j].flags = asset->dynEntClientList[i]->flags;
					iw6_asset->dynEntClientList[i][j].lightingHandle = asset->dynEntClientList[i]->lightingHandle;
					iw6_asset->dynEntClientList[i][j].health = asset->dynEntClientList[i]->health;
					iw6_asset->dynEntClientList[i][j].hinge = nullptr;
					iw6_asset->dynEntClientList[i][j].activeModel = nullptr;
					iw6_asset->dynEntClientList[i][j].contents = 0;
				}
			}
			iw6_asset->checksum = asset->checksum;

			return iw6_asset;
		}

		IW6::clipMap_t* convert(clipMap_t* asset, allocator& allocator)
		{
			return GenerateIW6ClipMap(asset, allocator);
		}
	}
}