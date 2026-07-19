#include "stdafx.hpp"
#include "IW4/Assets/ClipMap.hpp"
#include "H1/Utils/PhysWorld/generate.hpp"

namespace ZoneTool
{
	namespace IW3
	{
		// copypaste iw3x-port
		void AddTriggersToMap(IW4::clipMap_t* iw4_clipmap, allocator& allocator);
		IW4::SModelAabbNode* BuildSimpleSModelNodes(IW4::clipMap_t* clipmap, unsigned short* size, allocator& allocator);
		void Optimizeclipmap(IW4::clipMap_t* clipmap);
		void AddStageToMap(IW4::clipMap_t* iw4_clipmap, allocator& allocator);
		void AddTriggersToMap(IW4::clipMap_t* iw4_clipmap, allocator& allocator);
		void AddCarePackagesToMap(IW4::clipMap_t* iw4_clipmap, allocator& allocator);
		IW4::Bounds MakeCarePackageBounds();

		IW4::SModelAabbNode* BuildSimpleSModelNodes(
			IW4::clipMap_t* clipmap,
			unsigned short* size,
			allocator& allocator)
		{
			float maxs[3]{};
			float mins[3]{};

			if (clipmap->numStaticModels > 0)
			{
				clipmap->staticModelList[0].absBounds.max(maxs);
				clipmap->staticModelList[0].absBounds.min(mins);
			}
			else
			{
				// We still need the smodel nodes even when there is no model
			}

			for (unsigned short i = 1; i < clipmap->numStaticModels; i++)
			{
				float thisMax[3];
				float thisMin[3];

				clipmap->staticModelList[i].absBounds.max(thisMax);
				clipmap->staticModelList[i].absBounds.min(thisMin);

				maxs[0] = std::max(thisMax[0], maxs[0]);
				maxs[1] = std::max(thisMax[1], maxs[1]);
				maxs[2] = std::max(thisMax[2], maxs[2]);

				mins[0] = std::min(thisMin[0], mins[0]);
				mins[1] = std::min(thisMin[1], mins[1]);
				mins[2] = std::min(thisMin[2], mins[2]);
			}

			IW4::SModelAabbNode* node = allocator.allocate<IW4::SModelAabbNode>();
			bounds::compute(&node->bounds, mins, maxs);
			node->childCount = static_cast<short>(clipmap->numStaticModels);
			node->firstChild = 0;

			*size = 1;
			return node;
		}

		void Optimizeclipmap(IW4::clipMap_t* clipmap)
		{
			std::vector<std::pair<int, IW4::cLeafBrushNode_s*>> nodesToRepair{};

			for (size_t i = 0; i < clipmap->leafbrushNodesCount; i++)
			{
				auto node = &clipmap->leafbrushNodes[i];
				if (node->leafBrushCount > 0)
				{
					bool found = false;
					for (size_t j = 0; j < clipmap->numLeafBrushes; j++)
					{
						if (&clipmap->leafbrushes[j] == node->data.leaf.brushes)
						{
							found = true;
							break;
						}
					}

					if (!found)
					{
						nodesToRepair.push_back({ i, node });
					}
				}
			}

			// Repair clipmap... i think
			size_t repairedTotal = 0;
			for (auto kv : nodesToRepair)
			{
				auto node = kv.second;
				bool repaired = false;

				int repairedBrushRefs = 0;

				for (size_t j = 0; j < clipmap->numLeafBrushes; j++)
				{
					// This is an error... i think ?
					if (clipmap->leafbrushes[j] == node->data.leaf.brushes[repairedBrushRefs] && // Value correct
						&node->data.leaf.brushes[repairedBrushRefs] != &clipmap->leafbrushes[j]) // Ref incorrect
					{
						repairedBrushRefs++;

						if (repairedBrushRefs == node->leafBrushCount)
						{
							repaired = true;
							node->data.leaf.brushes = &clipmap->leafbrushes[j];
							repairedBrushRefs = 0;
							break;
						}
					}
					else
					{
						j -= repairedBrushRefs;
						repairedBrushRefs = 0;
					}
				}

				if (repaired)
				{
					repairedTotal++;
				}
			}

			if (repairedTotal > 0)
			{
				ZONETOOL_INFO("Successfully repaired %i nodes on clipmap", repairedTotal);
			}
		}

		IW4::clipMap_t* Convert(const IW3::clipMap_t* clipmap, allocator& allocator)
		{
			if (!clipmap) return nullptr;

			auto iw4_clipmap = allocator.allocate<IW4::clipMap_t>();

#define COPY_MEMBER(x) iw4_clipmap->x = clipmap->x

			COPY_MEMBER(name);
			COPY_MEMBER(isInUse);
			COPY_MEMBER(planeCount);
			REINTERPRET_CAST_SAFE(iw4_clipmap->planes, clipmap->planes);

			iw4_clipmap->numStaticModels = clipmap->numStaticModels;
			iw4_clipmap->staticModelList = allocator.allocate<IW4::cStaticModel_t>(iw4_clipmap->numStaticModels);
			for (unsigned int i = 0; i < clipmap->numStaticModels; ++i)
			{
				std::memcpy(&iw4_clipmap->staticModelList[i], &clipmap->staticModelList[i].xmodel,
					sizeof(IW4::cStaticModel_t));
				bounds::compute(clipmap->staticModelList[i].absmin, clipmap->staticModelList[i].absmax, &iw4_clipmap->staticModelList[i].absBounds.midPoint);
			}

			COPY_MEMBER(numMaterials);
			iw4_clipmap->materials = allocator.allocate<IW4::dmaterial_t>(clipmap->numMaterials);
			for (size_t i = 0; i < clipmap->numMaterials; i++)
			{
				const auto iw3Material = &clipmap->materials[i];
				auto iw4Material = &iw4_clipmap->materials[i];
				iw4Material->contentFlags = iw3Material->contentFlags;
				iw4Material->surfaceFlags = iw3Material->surfaceFlags;
				iw4Material->material = iw3Material->material;
			}

			COPY_MEMBER(numBrushSides);
			iw4_clipmap->brushsides = allocator.allocate<IW4::cbrushside_t>(clipmap->numBrushSides);
			for (size_t i = 0; i < clipmap->numBrushSides; i++)
			{
				const auto iw3BrushSide = &clipmap->brushsides[i];
				auto iw4BrushSide = &iw4_clipmap->brushsides[i];

				iw4BrushSide->plane = (IW4::cplane_s*)iw3BrushSide->plane;
				iw4BrushSide->materialNum = static_cast<unsigned short>(iw3BrushSide->materialNum);
				iw4BrushSide->firstAdjacentSideOffset = static_cast<char>(iw3BrushSide->firstAdjacentSideOffset);
				iw4BrushSide->edgeCount = iw3BrushSide->edgeCount;
			}

			COPY_MEMBER(numBrushEdges);
			COPY_MEMBER(brushEdges);

			COPY_MEMBER(numNodes);
			REINTERPRET_CAST_SAFE(iw4_clipmap->nodes, clipmap->nodes);

			COPY_MEMBER(numLeafs);
			iw4_clipmap->leafs = allocator.allocate<IW4::cLeaf_t>(clipmap->numLeafs);
			for (size_t i = 0; i < clipmap->numLeafs; i++)
			{
				auto iw3Leaf = &clipmap->leafs[i];
				auto iw4Leaf = &iw4_clipmap->leafs[i];

				iw4Leaf->firstCollAabbIndex = iw3Leaf->firstCollAabbIndex;
				iw4Leaf->collAabbCount = iw3Leaf->collAabbCount;
				iw4Leaf->brushContents = iw3Leaf->brushContents;
				iw4Leaf->terrainContents = iw3Leaf->terrainContents;
				bounds::compute(&iw4Leaf->bounds, iw3Leaf->mins, iw3Leaf->maxs);
				iw4Leaf->leafBrushNode = iw3Leaf->leafBrushNode;
			}

			COPY_MEMBER(leafbrushNodesCount);
			REINTERPRET_CAST_SAFE(iw4_clipmap->leafbrushNodes, clipmap->leafbrushNodes);

			COPY_MEMBER(numLeafBrushes);
			COPY_MEMBER(leafbrushes);

			COPY_MEMBER(numLeafSurfaces);
			COPY_MEMBER(leafsurfaces);

			COPY_MEMBER(vertCount);
			COPY_MEMBER(verts);

			COPY_MEMBER(triCount);
			COPY_MEMBER(triIndices);
			COPY_MEMBER(triEdgeIsWalkable);

			COPY_MEMBER(borderCount);
			REINTERPRET_CAST_SAFE(iw4_clipmap->borders, clipmap->borders);

			COPY_MEMBER(partitionCount);
			REINTERPRET_CAST_SAFE(iw4_clipmap->partitions, clipmap->partitions);

			COPY_MEMBER(aabbTreeCount);
			iw4_clipmap->aabbTrees = allocator.allocate<IW4::CollisionAabbTree>(clipmap->aabbTreeCount);
			for (int i = 0; i < clipmap->aabbTreeCount; i++)
			{
				IW4::CollisionAabbTree aabbTree;

				std::memcpy(aabbTree.midPoint, clipmap->aabbTrees[i].origin, sizeof(aabbTree.midPoint));
				std::memcpy(aabbTree.halfSize, clipmap->aabbTrees[i].halfSize, sizeof(aabbTree.halfSize));
				aabbTree.materialIndex = clipmap->aabbTrees[i].materialIndex;
				aabbTree.childCount = clipmap->aabbTrees[i].childCount;
				aabbTree.u.firstChildIndex = clipmap->aabbTrees[i].u.firstChildIndex;
				aabbTree.u.partitionIndex = clipmap->aabbTrees[i].u.partitionIndex;

				iw4_clipmap->aabbTrees[i] = aabbTree;
			}

			COPY_MEMBER(numSubModels);
			iw4_clipmap->cmodels = allocator.allocate<IW4::cmodel_t>(clipmap->numSubModels);
			for (unsigned int i = 0; i < clipmap->numSubModels; ++i)
			{
				IW4::cmodel_t model;
				assert_sizeof(IW4::cmodel_t, 68);
				std::memcpy(&model, &clipmap->cmodels[i], sizeof IW4::cmodel_t);

				model.bounds.compute(clipmap->cmodels[i].mins, clipmap->cmodels[i].maxs);
				model.leaf.bounds.compute(clipmap->cmodels[i].leaf.mins, clipmap->cmodels[i].leaf.maxs);
				iw4_clipmap->cmodels[i] = model;
			}

			COPY_MEMBER(numBrushes);
			iw4_clipmap->brushes = allocator.allocate<IW4::cbrush_t>(clipmap->numBrushes);
			iw4_clipmap->brushBounds = allocator.allocate<IW4::Bounds>(clipmap->numBrushes);
			iw4_clipmap->brushContents = allocator.allocate<int>(clipmap->numBrushes);

			std::unordered_map<IW3::cbrush_t*, IW4::cbrush_t*> brushUpgradeMap{};

			for (size_t i = 0; i < clipmap->numBrushes; i++)
			{
				const auto iw3Brush = &clipmap->brushes[i];
				const auto iw4Brush = &iw4_clipmap->brushes[i];

				brushUpgradeMap[iw3Brush] = iw4Brush;

				iw4Brush->numsides = static_cast<unsigned short>(iw3Brush->numsides);
				iw4Brush->glassPieceIndex = 0;

				iw4Brush->sides = nullptr;
				if (iw3Brush->sides)
				{
					for (size_t sideIndex = 0; sideIndex < clipmap->numBrushSides; sideIndex++)
					{
						if (iw3Brush->sides == &clipmap->brushsides[sideIndex])
						{
							iw4Brush->sides = &iw4_clipmap->brushsides[sideIndex];
							break;
						}
					}

					assert(iw4Brush->sides);
				}

				iw4Brush->baseAdjacentSide = iw3Brush->baseAdjacentSide;
				for (size_t x = 0; x < 2; x++)
				{
					for (size_t y = 0; y < 3; y++)
					{
						iw4Brush->axialMaterialNum[x][y] = iw3Brush->axialMaterialNum[x][y];
						iw4Brush->firstAdjacentSideOffsets[x][y] = static_cast<char>(iw3Brush->firstAdjacentSideOffsets[x][y]);
						iw4Brush->edgeCount[x][y] = static_cast<char>(iw3Brush->edgeCount[x][y]);
					}
				}

				iw4_clipmap->brushBounds[i].compute(iw3Brush->mins, iw3Brush->maxs);
				iw4_clipmap->brushContents[i] = iw3Brush->contents;
			}

			iw4_clipmap->mapEnts = (IW4::MapEnts*)clipmap->mapEnts;

			iw4_clipmap->smodelNodes = BuildSimpleSModelNodes(iw4_clipmap, &iw4_clipmap->smodelNodeCount, allocator);

			std::memcpy(iw4_clipmap->dynEntCount, clipmap->dynEntCount, sizeof(unsigned short) * 2);

			for (size_t x = 0; x < 2; x++)
			{
				iw4_clipmap->dynEntDefList[x] = allocator.allocate<IW4::DynEntityDef>(iw4_clipmap->dynEntCount[x]);
				for (size_t i = 0; i < iw4_clipmap->dynEntCount[x]; i++)
				{
					const auto iw3Def = &clipmap->dynEntDefList[x][i];
					auto iw4Def = &iw4_clipmap->dynEntDefList[x][i];

					iw4Def->type = (IW4::DynEntityType)iw3Def->type;
					memcpy(&iw4Def->pose, &iw3Def->pose, sizeof(iw3Def->pose));

					if (iw3Def->xModel)
					{
						iw4Def->xModel = (IW4::XModel*)iw3Def->xModel;
					}

					iw4Def->brushModel = iw3Def->brushModel;
					iw4Def->physicsBrushModel = iw3Def->physicsBrushModel;

					if (iw3Def->destroyFx)
					{
						iw4Def->destroyFx = (IW4::FxEffectDef*)iw3Def->destroyFx;
					}

					if (iw3Def->physPreset)
					{
						iw4Def->physPreset = (IW4::PhysPreset*)iw3Def->physPreset;
					}

					iw4Def->health = iw3Def->health;
					memcpy(&iw4Def->mass, &iw3Def->mass, sizeof(iw3Def->mass));
					iw4Def->contents = iw3Def->contents;
				}
			}


			COPY_MEMBER(checksum);

			Optimizeclipmap(iw4_clipmap);

#undef COPY_MEMBER

			AddTriggersToMap(iw4_clipmap, allocator);

			// dump .triggers
			{
				assert_sizeof(H1::TriggerModel, sizeof(IW4::TriggerModel));
				assert_sizeof(H1::TriggerHull, sizeof(IW4::TriggerHull));
				assert_sizeof(H1::TriggerSlab, sizeof(IW4::TriggerSlab));

				assetmanager::dumper trigger_dumper;
				if (trigger_dumper.open(iw4_clipmap->mapEnts->name + ".ents.triggers"s))
				{
					trigger_dumper.dump_int(iw4_clipmap->mapEnts->trigger.count);
					trigger_dumper.dump_array<IW4::TriggerModel>(iw4_clipmap->mapEnts->trigger.models, iw4_clipmap->mapEnts->trigger.count);

					trigger_dumper.dump_int(iw4_clipmap->mapEnts->trigger.hullCount);
					trigger_dumper.dump_array<IW4::TriggerHull>(iw4_clipmap->mapEnts->trigger.hulls, iw4_clipmap->mapEnts->trigger.hullCount);

					trigger_dumper.dump_int(iw4_clipmap->mapEnts->trigger.slabCount);
					trigger_dumper.dump_array<IW4::TriggerSlab>(iw4_clipmap->mapEnts->trigger.slabs, iw4_clipmap->mapEnts->trigger.slabCount);

					trigger_dumper.close();
				}
			}

			static const auto addCarePackages = true;
			if (addCarePackages) 
			{
				AddCarePackagesToMap(iw4_clipmap, allocator);
			}

			AddStageToMap(iw4_clipmap, allocator);

			return iw4_clipmap;
		}

		void AddStageToMap(IW4::clipMap_t* iw4_clipmap, allocator& allocator)
		{
			iw4_clipmap->mapEnts->stageCount = 1;
			iw4_clipmap->mapEnts->stages = allocator.allocate<IW4::Stage>();
			iw4_clipmap->mapEnts->stages[0].name = "stage 0";
			iw4_clipmap->mapEnts->stages[0].triggerIndex = 0x400;
			iw4_clipmap->mapEnts->stages[0].sunPrimaryLightIndex = 0x1;
		}

		void AddTriggersToMap(IW4::clipMap_t* iw4_clipmap, allocator& allocator)
		{
			// Add triggers
			if (iw4_clipmap->cmodels)
			{
				iw4_clipmap->mapEnts->trigger.count = iw4_clipmap->numSubModels;
				iw4_clipmap->mapEnts->trigger.hullCount = iw4_clipmap->numSubModels;

				iw4_clipmap->mapEnts->trigger.models = allocator.allocate<IW4::TriggerModel>(iw4_clipmap->numSubModels);
				iw4_clipmap->mapEnts->trigger.hulls = allocator.allocate<IW4::TriggerHull>(iw4_clipmap->numSubModels);

				std::vector<IW4::TriggerSlab> slabs{};

				for (unsigned short i = 0; i < iw4_clipmap->mapEnts->trigger.count; ++i)
				{
					auto trigMod = &iw4_clipmap->mapEnts->trigger.models[i];
					auto trigHul = &iw4_clipmap->mapEnts->trigger.hulls[i];

					IW4::Bounds cmodelBounds = iw4_clipmap->cmodels[i].bounds;

					trigHul->bounds = cmodelBounds;
					trigHul->contents = iw4_clipmap->cmodels[i].leaf.brushContents | iw4_clipmap->cmodels[i].leaf.terrainContents;

					trigMod->hullCount = 1;
					trigMod->firstHull = i;
					trigMod->contents = iw4_clipmap->cmodels[i].leaf.brushContents | iw4_clipmap->cmodels[i].leaf.terrainContents;

					auto* node = &iw4_clipmap->leafbrushNodes[iw4_clipmap->cmodels[i].leaf.leafBrushNode];

					if (node->leafBrushCount)
					{
						for (int j = 0; j < node->leafBrushCount; ++j)
						{
							auto brush = &iw4_clipmap->brushes[node->data.leaf.brushes[j]];

							auto baseSlab = slabs.size();
							for (unsigned int k = 0; k < brush->numsides; ++k)
							{
								IW4::TriggerSlab curSlab;
								curSlab.dir[0] = brush->sides[k].plane->normal[0];
								curSlab.dir[1] = brush->sides[k].plane->normal[1];
								curSlab.dir[2] = brush->sides[k].plane->normal[2];
								curSlab.halfSize = brush->sides[k].plane->dist;
								curSlab.midPoint = 0.0f; // ??

								slabs.push_back(curSlab);
							}

							trigHul->firstSlab = static_cast<unsigned short>(baseSlab);
							trigHul->slabCount = static_cast<unsigned short>(slabs.size() - baseSlab);
						}
					}
				}

				iw4_clipmap->mapEnts->trigger.slabCount = slabs.size();
				iw4_clipmap->mapEnts->trigger.slabs = allocator.allocate<IW4::TriggerSlab>(slabs.size());

				for (unsigned int i = 0; i < slabs.size(); i++)
				{
					iw4_clipmap->mapEnts->trigger.slabs[i] = slabs[i];
				}
			}
		}

		void AddCarePackagesToMap(IW4::clipMap_t* iw4_clipmap, allocator& allocator)
		{
			const auto brushIndex = iw4_clipmap->numBrushes;
			const auto brushNodeIndex = iw4_clipmap->leafbrushNodesCount;
			const auto brushEdgeIndex = iw4_clipmap->numBrushEdges;
			const auto materialIndex = iw4_clipmap->numMaterials;
			const auto cModelIndex = iw4_clipmap->numSubModels;

			// Materials
			constexpr auto matSize = sizeof(IW4::dmaterial_t);
			auto reallocatedDMaterials = allocator.allocate<IW4::dmaterial_t>(iw4_clipmap->numMaterials + 1);

			memcpy_s(reallocatedDMaterials, iw4_clipmap->numMaterials * matSize, iw4_clipmap->materials, iw4_clipmap->numMaterials * matSize);

			// Add the care package material
			auto dmat = &reallocatedDMaterials[iw4_clipmap->numMaterials];
			dmat->contentFlags = 134420032;
			dmat->surfaceFlags = 13910176;
			const std::string materialName = "clip_nosight_metal";
			dmat->material = (char*)allocator.duplicate_string(materialName);

			iw4_clipmap->numMaterials++;
			iw4_clipmap->materials = reallocatedDMaterials;

			// Brush edges
			auto reallocatedBrushEdges = allocator.allocate<unsigned char>(iw4_clipmap->numBrushEdges + 1);
			memcpy_s(reallocatedBrushEdges, iw4_clipmap->numBrushEdges, iw4_clipmap->brushEdges, iw4_clipmap->numBrushEdges);
			reallocatedBrushEdges[brushEdgeIndex] = 2;

			//Redirect pointers
			unsigned int offset = reinterpret_cast<unsigned int>(reallocatedBrushEdges) - reinterpret_cast<unsigned int>(iw4_clipmap->brushEdges);
			for (size_t i = 0; i < iw4_clipmap->numBrushes; i++)
			{
				auto oldValue = *iw4_clipmap->brushes[i].baseAdjacentSide;

				iw4_clipmap->brushes[i].baseAdjacentSide = reinterpret_cast<unsigned char*>(reinterpret_cast<int>(iw4_clipmap->brushes[i].baseAdjacentSide) + offset);

				if (*iw4_clipmap->brushes[i].baseAdjacentSide != oldValue)
				{
					// Something wrong happened and i have no idea if this is normal behaviour or not
					if (*iw4_clipmap->brushes[i].edgeCount[0] == 0 && *iw4_clipmap->brushes[i].edgeCount[1] == 0)
					{
						iw4_clipmap->brushes[i].baseAdjacentSide = reallocatedBrushEdges;
					}
					else
					{
						// Okay this is definitely wrong
						assert(false);
					}
				}
			}

			iw4_clipmap->brushEdges = reallocatedBrushEdges;
			iw4_clipmap->numBrushEdges++;

			// Leaf Brush node
			constexpr auto lbnSize = sizeof(IW3::cLeafBrushNode_s);

			auto reallocatedBrushNodes = allocator.allocate<IW3::cLeafBrushNode_s>(iw4_clipmap->leafbrushNodesCount + 1);
			memcpy_s(reallocatedBrushNodes, iw4_clipmap->leafbrushNodesCount * lbnSize, iw4_clipmap->leafbrushNodes, iw4_clipmap->leafbrushNodesCount * lbnSize);

			// We add the care package brush node !
			IW3::cLeafBrushNode_s carePackageBrushNode;
			carePackageBrushNode.axis = 0;
			carePackageBrushNode.leafBrushCount = 1;
			carePackageBrushNode.contents = 134420032;
			carePackageBrushNode.data.leaf.brushes = allocator.allocate<unsigned short>();
			*carePackageBrushNode.data.leaf.brushes = brushIndex;

			reallocatedBrushNodes[iw4_clipmap->leafbrushNodesCount] = carePackageBrushNode;

			iw4_clipmap->leafbrushNodesCount++;
			iw4_clipmap->leafbrushNodes = (IW4::cLeafBrushNode_s*)reallocatedBrushNodes;


			// CModels
			constexpr auto cmodelSize = sizeof(IW4::cmodel_t);

			auto reallocatedCModels = allocator.allocate<IW4::cmodel_t>(iw4_clipmap->numSubModels + 2);
			memcpy_s(reallocatedCModels, iw4_clipmap->numSubModels * cmodelSize, iw4_clipmap->cmodels, iw4_clipmap->numSubModels * cmodelSize);

			// We add the two care packages we added in GfxWorld
			for (size_t i = 0; i < 2; i++)
			{
				IW4::cmodel_t model;
				IW4::Bounds bounds = MakeCarePackageBounds();

				model.radius = 47.f;

				model.leaf.firstCollAabbIndex = static_cast<unsigned short>(iw4_clipmap->aabbTreeCount); // This is invalid, but this is how rust does it!
				model.leaf.collAabbCount = 0;
				model.leaf.brushContents = 134420032;
				model.leaf.terrainContents = 0;

				model.bounds = bounds;

				// Had to be slightly smaller

				bounds.halfSize[0] *= 0.95f;
				bounds.halfSize[1] *= 0.95f;
				bounds.halfSize[2] *= 0.95f;

				model.leaf.bounds = bounds;
				model.leaf.leafBrushNode = brushNodeIndex;

				reallocatedCModels[cModelIndex + i] = model;
			}

			iw4_clipmap->numSubModels += 2;
			iw4_clipmap->cmodels = reallocatedCModels;

			// Brushes
			constexpr auto brushSize = sizeof(IW4::cbrush_t);

			auto reallocatedBrushes = allocator.allocate<IW4::cbrush_t>(iw4_clipmap->numBrushes + 1);
			memcpy_s(reallocatedBrushes, iw4_clipmap->numBrushes * brushSize, iw4_clipmap->brushes, iw4_clipmap->numBrushes * brushSize);

			auto bounds = MakeCarePackageBounds();
			IW4::cbrush_t carePackageBrush{};
			carePackageBrush.numsides = 0;
			carePackageBrush.baseAdjacentSide = &iw4_clipmap->brushEdges[brushEdgeIndex];
			carePackageBrush.sides = nullptr;

			for (int x = 0; x < 2; ++x)
			{
				for (int y = 0; y < 3; ++y)
				{
					carePackageBrush.axialMaterialNum[x][y] = static_cast<short>(materialIndex);
				}
			}

			constexpr short adjacentSideOffset[][3] = { {0,0,0}, {4, 12, 20} };
			for (int x = 0; x < 2; ++x)
			{
				for (int y = 0; y < 3; ++y)
				{
					// firstAdjacentSideOffsets
					carePackageBrush.firstAdjacentSideOffsets[x][y] = static_cast<char>(adjacentSideOffset[x][y]);
				}
			}

			constexpr char edgeCounts[] = { 4, 4, 4 };
			for (int x = 0; x < 2; ++x)
			{
				for (int y = 0; y < 3; ++y)
				{
					// edgeCount
					carePackageBrush.edgeCount[x][y] = edgeCounts[y];
				}
			}
			auto reallocatedBounds = allocator.allocate<IW4::Bounds>(iw4_clipmap->numBrushes + 1);
			memcpy_s(reallocatedBounds, iw4_clipmap->numBrushes * sizeof(IW4::Bounds), iw4_clipmap->brushBounds, iw4_clipmap->numBrushes * sizeof(IW4::Bounds));

			auto reallocatedContents = allocator.allocate<int>(iw4_clipmap->numBrushes + 1);
			memcpy_s(reallocatedContents, iw4_clipmap->numBrushes * sizeof(int), iw4_clipmap->brushContents, iw4_clipmap->numBrushes * sizeof(int));

			reallocatedBrushes[brushIndex] = carePackageBrush;
			reallocatedBounds[brushIndex] = bounds;
			reallocatedContents[brushIndex] = carePackageBrushNode.contents;

			iw4_clipmap->brushes = reallocatedBrushes;

			iw4_clipmap->numBrushes++;
		}

		IW4::Bounds MakeCarePackageBounds()
		{
			IW4::Bounds bounds;

			bounds.midPoint[0] = -1.0f;
			bounds.midPoint[1] = -0.045f;
			bounds.midPoint[2] = -0.5f;

			bounds.halfSize[0] = 30.0f;
			bounds.halfSize[1] = 17.75f;
			bounds.halfSize[2] = 16.0f;

			return bounds;
		}

		IW4::clipMap_t* GenerateIW4ClipMap(clipMap_t* asset, allocator& mem)
		{
			return Convert(asset, mem);
		}

		void IClipMap::dump(clipMap_t* asset)
		{
			// generate clipmap
			allocator allocator;
			auto iw4_asset = GenerateIW4ClipMap(asset, allocator);

			// extract world-space static-model collision tris so the h1 physworld generator can bake them into the world mesh
			if (asset->numStaticModels && asset->staticModelList)
			{
				std::vector<ZoneTool::H1::physworld_gen::smodel_tri> tris;

				for (auto i = 0u; i < asset->numStaticModels; i++)
				{
					const auto& sm = asset->staticModelList[i];
					const auto* m = sm.xmodel;
					if (!m)
					{
						continue;
					}

					if (m->collLod < 0 || m->collLod >= m->numLods)
					{
						continue;
					}

					if ((m->contents & 0x1) == 0)
					{
						continue;
					}

					const auto& lod = m->lodInfo[m->collLod];
					if (lod.numsurfs == 0)
					{
						continue;
					}

					// invScaledAxis = (R*scale)^-1 maps world->model, so its inverse restores the scaled rotation for model->world
					const auto& axis = sm.invScaledAxis;
					const auto det =
						axis[0][0] * (axis[1][1] * axis[2][2] - axis[1][2] * axis[2][1]) -
						axis[0][1] * (axis[1][0] * axis[2][2] - axis[1][2] * axis[2][0]) +
						axis[0][2] * (axis[1][0] * axis[2][1] - axis[1][1] * axis[2][0]);

					if (det > -1e-12f && det < 1e-12f)
					{
						continue;
					}

					const auto inv_det = 1.0f / det;
					float inv[3][3];
					inv[0][0] = (axis[1][1] * axis[2][2] - axis[1][2] * axis[2][1]) * inv_det;
					inv[0][1] = -(axis[0][1] * axis[2][2] - axis[0][2] * axis[2][1]) * inv_det;
					inv[0][2] = (axis[0][1] * axis[1][2] - axis[0][2] * axis[1][1]) * inv_det;
					inv[1][0] = -(axis[1][0] * axis[2][2] - axis[1][2] * axis[2][0]) * inv_det;
					inv[1][1] = (axis[0][0] * axis[2][2] - axis[0][2] * axis[2][0]) * inv_det;
					inv[1][2] = -(axis[0][0] * axis[1][2] - axis[0][2] * axis[1][0]) * inv_det;
					inv[2][0] = (axis[1][0] * axis[2][1] - axis[1][1] * axis[2][0]) * inv_det;
					inv[2][1] = -(axis[0][0] * axis[2][1] - axis[0][1] * axis[2][0]) * inv_det;
					inv[2][2] = (axis[0][0] * axis[1][1] - axis[0][1] * axis[1][0]) * inv_det;

					const auto to_world = [&](const float* v, float* out)
					{
						out[0] = inv[0][0] * v[0] + inv[0][1] * v[1] + inv[0][2] * v[2] + sm.origin[0];
						out[1] = inv[1][0] * v[0] + inv[1][1] * v[1] + inv[1][2] * v[2] + sm.origin[1];
						out[2] = inv[2][0] * v[0] + inv[2][1] * v[1] + inv[2][2] * v[2] + sm.origin[2];
					};

					for (auto k = 0; k < lod.numsurfs; k++)
					{
						const auto* surf = &m->surfs[lod.surfIndex + k];
						if (!surf->verts0 || !surf->triIndices)
						{
							continue;
						}

						for (auto t = 0; t < surf->triCount; t++)
						{
							const auto i0 = surf->triIndices[3 * t + 0];
							const auto i1 = surf->triIndices[3 * t + 1];
							const auto i2 = surf->triIndices[3 * t + 2];

							if (i0 >= surf->vertCount || i1 >= surf->vertCount || i2 >= surf->vertCount)
							{
								continue;
							}

							ZoneTool::H1::physworld_gen::smodel_tri tri{};
							to_world(surf->verts0[i0].xyz, tri.a);
							to_world(surf->verts0[i1].xyz, tri.b);
							to_world(surf->verts0[i2].xyz, tri.c);
							tris.emplace_back(tri);
						}
					}
				}

				ZoneTool::H1::physworld_gen::set_static_model_tris(std::move(tris));
			}

			// dump clipmap
			IW4::IClipMap::dump(iw4_asset);
		}
	}
}