#include "stdafx.hpp"
#include "../Include.hpp"

#include "XSurface.hpp"

#include "X64/Utils/Utils.hpp"

namespace ZoneTool::IW5
{
	namespace IW6Converter
	{
		void GenerateIW6BlendVertsShit(IW6::XSurface* surf)
		{
			unsigned short a = 0;
			unsigned short b = 0;
			unsigned short index = 1;
			for (short s = 0; s < (surf->blendVertCounts[0]); s++)
			{
				surf->blendVertsTable[a].b[0] = surf->blendVerts[b + 0] / 64;
				surf->blendVertsTable[a].b[1] = 0;
				surf->blendVertsTable[a].b[2] = 0;
				surf->blendVertsTable[a].b[3] = 0;
				surf->blendVertsTable[a].b[4] = 0;
				surf->blendVertsTable[a].b[5] = 0;
				surf->blendVertsTable[a].b[6] = 0;
				surf->blendVertsTable[a].b[7] = 0;

				surf->blendVertsTable[a].b[8] = 0;
				surf->blendVertsTable[a].b[9] = 0;
				surf->blendVertsTable[a].b[10] = 0;
				surf->blendVertsTable[a].b[11] = 0;
				surf->blendVertsTable[a].b[12] = 0;
				surf->blendVertsTable[a].b[13] = 0;
				surf->blendVertsTable[a].b[14] = 0;

				surf->blendVertsTable[a].blendVertCountIndex = index;

				a++;
				b += 1;
			}
			index++;

			for (short s = 0; s < (surf->blendVertCounts[1]); s++)
			{
				surf->blendVertsTable[a].b[0] = surf->blendVerts[b + 0] / 64;
				surf->blendVertsTable[a].b[1] = surf->blendVerts[b + 1] / 64;
				surf->blendVertsTable[a].b[2] = 0;
				surf->blendVertsTable[a].b[3] = 0;
				surf->blendVertsTable[a].b[4] = 0;
				surf->blendVertsTable[a].b[5] = 0;
				surf->blendVertsTable[a].b[6] = 0;
				surf->blendVertsTable[a].b[7] = 0;

				surf->blendVertsTable[a].b[8] = surf->blendVerts[b + 2];
				surf->blendVertsTable[a].b[9] = 0;
				surf->blendVertsTable[a].b[10] = 0;
				surf->blendVertsTable[a].b[11] = 0;
				surf->blendVertsTable[a].b[12] = 0;
				surf->blendVertsTable[a].b[13] = 0;
				surf->blendVertsTable[a].b[14] = 0;

				surf->blendVertsTable[a].blendVertCountIndex = index;

				a++;
				b += 3;
			}
			index++;

			for (short s = 0; s < (surf->blendVertCounts[2]); s++)
			{
				surf->blendVertsTable[a].b[0] = surf->blendVerts[b + 0] / 64;
				surf->blendVertsTable[a].b[1] = surf->blendVerts[b + 1] / 64;
				surf->blendVertsTable[a].b[2] = surf->blendVerts[b + 3] / 64;
				surf->blendVertsTable[a].b[3] = 0;
				surf->blendVertsTable[a].b[4] = 0;
				surf->blendVertsTable[a].b[5] = 0;
				surf->blendVertsTable[a].b[6] = 0;
				surf->blendVertsTable[a].b[7] = 0;

				surf->blendVertsTable[a].b[8] = surf->blendVerts[b + 2];
				surf->blendVertsTable[a].b[9] = surf->blendVerts[b + 4];
				surf->blendVertsTable[a].b[10] = 0;
				surf->blendVertsTable[a].b[11] = 0;
				surf->blendVertsTable[a].b[12] = 0;
				surf->blendVertsTable[a].b[13] = 0;
				surf->blendVertsTable[a].b[14] = 0;

				surf->blendVertsTable[a].blendVertCountIndex = index;

				a++;
				b += 5;
			}
			index++;

			for (short s = 0; s < (surf->blendVertCounts[3]); s++)
			{
				surf->blendVertsTable[a].b[0] = surf->blendVerts[b + 0] / 64;
				surf->blendVertsTable[a].b[1] = surf->blendVerts[b + 1] / 64;
				surf->blendVertsTable[a].b[2] = surf->blendVerts[b + 3] / 64;
				surf->blendVertsTable[a].b[3] = surf->blendVerts[b + 5] / 64;
				surf->blendVertsTable[a].b[4] = 0;
				surf->blendVertsTable[a].b[5] = 0;
				surf->blendVertsTable[a].b[6] = 0;
				surf->blendVertsTable[a].b[7] = 0;

				surf->blendVertsTable[a].b[8] = surf->blendVerts[b + 2];
				surf->blendVertsTable[a].b[9] = surf->blendVerts[b + 4];
				surf->blendVertsTable[a].b[10] = surf->blendVerts[b + 6];
				surf->blendVertsTable[a].b[11] = 0;
				surf->blendVertsTable[a].b[12] = 0;
				surf->blendVertsTable[a].b[13] = 0;
				surf->blendVertsTable[a].b[14] = 0;

				surf->blendVertsTable[a].blendVertCountIndex = index;

				a++;
				b += 7;
			}
			index++;
		}

		void GenerateIW6XSurface(IW6::XSurface* iw6_asset, XSurface* asset, allocator& mem)
		{
			iw6_asset->flags = 0;
			iw6_asset->flags |= ((asset->flags & IW5::SURF_FLAG_VERTCOL_GREY) != 0) ? IW6::SURF_FLAG_VERTCOL_GREY : 0;
			iw6_asset->flags |= ((asset->flags & IW5::SURF_FLAG_VERTCOL_NONE) != 0) ? IW6::SURF_FLAG_VERTCOL_NONE : 0;
			//iw6_asset->flags |= ((asset->flags & IW5::SURF_FLAG_QUANTIZED) != 0) ? IW6::SURF_FLAG_QUANTIZED : 0;
			iw6_asset->flags |= ((asset->flags & IW5::SURF_FLAG_SKINNED) != 0) ? IW6::SURF_FLAG_SKINNED : 0;

			iw6_asset->vertCount = asset->vertCount;
			iw6_asset->triCount = asset->triCount;
			iw6_asset->rigidVertListCount = asset->vertListCount;

			// blend verts
			memcpy(&iw6_asset->blendVertCounts, &asset->vertInfo.vertCount, sizeof(asset->vertInfo.vertCount));
			iw6_asset->blendVerts = reinterpret_cast<unsigned short* __ptr64>(asset->vertInfo.vertsBlend);

			iw6_asset->blendVertsTable = mem.allocate<IW6::BlendVertsUnknown>(asset->vertCount);
			GenerateIW6BlendVertsShit(iw6_asset);

			// triIndices
			iw6_asset->triIndices = reinterpret_cast<IW6::Face * __ptr64>(asset->triIndices);

			// verts
			//iw6_asset->verts0.packedVerts0 = reinterpret_cast<IW6::GfxPackedVertex* __ptr64>(asset->verticies);
			iw6_asset->verts0.packedVerts0 = mem.allocate<IW6::GfxPackedVertex>(asset->vertCount);
			for (unsigned short i = 0; i < asset->vertCount; i++)
			{
				memcpy(&iw6_asset->verts0.packedVerts0[i], &asset->verts0.packedVerts0[i], sizeof(IW5::GfxPackedVertex));

				float texCoord_unpacked[2]{ 0.0f, 0.0f };
				PackedVec::Vec2UnpackTexCoords(asset->verts0.packedVerts0[i].texCoord.packed, texCoord_unpacked);
				std::swap(texCoord_unpacked[0], texCoord_unpacked[1]); // these are inverted...
				iw6_asset->verts0.packedVerts0[i].texCoord.packed = PackedVec::Vec2PackTexCoords(texCoord_unpacked);

				// re-calculate these...
				float normal_unpacked[3]{ 0.0f, 0.0f, 0.0f };
				PackedVec::Vec3UnpackUnitVec(asset->verts0.packedVerts0[i].normal.array, normal_unpacked);

				float tangent_unpacked[3]{ 0.0f, 0.0f, 0.0f };
				PackedVec::Vec3UnpackUnitVec(asset->verts0.packedVerts0[i].tangent.array, tangent_unpacked);

				float normal[3] = { normal_unpacked[0], normal_unpacked[1], normal_unpacked[2] };
				float tangent[3] = { tangent_unpacked[0], tangent_unpacked[1], tangent_unpacked[2] };

				float sign = 0.0f;
				if (asset->verts0.packedVerts0[i].binormalSign == -1.0f)
				{
					sign = 1.0f;
				}

				iw6_asset->verts0.packedVerts0[i].normal.packed = PackedVec::Vec3PackUnitVec(normal);
				iw6_asset->verts0.packedVerts0[i].tangent.packed = PackedVec::Vec3PackUnitVec(tangent);
				
				// correct color : bgra->rgba
				iw6_asset->verts0.packedVerts0[i].color.array[0] = asset->verts0.packedVerts0[i].color.array[2];
				iw6_asset->verts0.packedVerts0[i].color.array[1] = asset->verts0.packedVerts0[i].color.array[1];
				iw6_asset->verts0.packedVerts0[i].color.array[2] = asset->verts0.packedVerts0[i].color.array[0];
				iw6_asset->verts0.packedVerts0[i].color.array[3] = asset->verts0.packedVerts0[i].color.array[3];
			}

			// rigidVertLists
			iw6_asset->rigidVertLists = mem.allocate<IW6::XRigidVertList>(asset->vertListCount);
			for (int i = 0; i < asset->vertListCount; i++)
			{
				iw6_asset->rigidVertLists[i].boneOffset = asset->vertList[i].boneOffset;
				iw6_asset->rigidVertLists[i].vertCount = asset->vertList[i].vertCount;
				iw6_asset->rigidVertLists[i].triOffset = asset->vertList[i].triOffset;
				iw6_asset->rigidVertLists[i].triCount = asset->vertList[i].triCount;

				if (asset->vertList[i].collisionTree)
				{
					iw6_asset->rigidVertLists[i].collisionTree = mem.allocate<IW6::XSurfaceCollisionTree>();
					memcpy(&iw6_asset->rigidVertLists[i].collisionTree->trans, &asset->vertList[i].collisionTree->trans,
						sizeof(asset->vertList[i].collisionTree->trans));
					memcpy(&iw6_asset->rigidVertLists[i].collisionTree->scale, &asset->vertList[i].collisionTree->scale,
						sizeof(asset->vertList[i].collisionTree->scale));

					iw6_asset->rigidVertLists[i].collisionTree->nodeCount = asset->vertList[i].collisionTree->nodeCount;
					iw6_asset->rigidVertLists[i].collisionTree->nodes = mem.allocate<IW6::XSurfaceCollisionNode>(
						asset->vertList[i].collisionTree->nodeCount);
					for (int j = 0; j < asset->vertList[i].collisionTree->nodeCount; j++)
					{
						memcpy(&iw6_asset->rigidVertLists[i].collisionTree->nodes[j].aabb.mins,
							&asset->vertList[i].collisionTree->nodes[j].aabb.mins,
							sizeof(asset->vertList[i].collisionTree->nodes[j].aabb.mins));
						memcpy(&iw6_asset->rigidVertLists[i].collisionTree->nodes[j].aabb.maxs,
							&asset->vertList[i].collisionTree->nodes[j].aabb.maxs,
							sizeof(asset->vertList[i].collisionTree->nodes[j].aabb.maxs));

						iw6_asset->rigidVertLists[i].collisionTree->nodes[j].childBeginIndex =
							asset->vertList[i].collisionTree->nodes[j].childBeginIndex;
						iw6_asset->rigidVertLists[i].collisionTree->nodes[j].childCount =
							asset->vertList[i].collisionTree->nodes[j].childCount;
					}

					iw6_asset->rigidVertLists[i].collisionTree->leafCount = asset->vertList[i].collisionTree->leafCount;
					iw6_asset->rigidVertLists[i].collisionTree->leafs = mem.allocate<IW6::XSurfaceCollisionLeaf>(
						asset->vertList[i].collisionTree->leafCount);
					for (int j = 0; j < asset->vertList[i].collisionTree->leafCount; j++)
					{
						iw6_asset->rigidVertLists[i].collisionTree->leafs[j].triangleBeginIndex =
							asset->vertList[i].collisionTree->leafs[j].triangleBeginIndex;
					}
				}
			}

			// partBits
			memcpy(&iw6_asset->partBits, &asset->partBits, sizeof(asset->partBits));
		}

		IW6::XModelSurfs* GenerateIW6XModelSurfs(XModelSurfs* asset, allocator& mem)
		{
			// allocate IW6 XModelSurfs structure
			const auto iw6_asset = mem.allocate<IW6::XModelSurfs>();

			iw6_asset->name = mem.duplicate_string(asset->name);
			iw6_asset->numsurfs = asset->numsurfs;
			memcpy(&iw6_asset->partBits, &asset->partBits, sizeof(asset->partBits));

			iw6_asset->surfs = mem.allocate<IW6::XSurface>(asset->numsurfs);
			for (int i = 0; i < asset->numsurfs; i++)
			{
				GenerateIW6XSurface(&iw6_asset->surfs[i], &asset->surfs[i], mem);
			}

			return iw6_asset;
		}

		IW6::XModelSurfs* convert(XModelSurfs* asset, allocator& allocator)
		{
			// generate IW6 surfaces
			return GenerateIW6XModelSurfs(asset, allocator);
		}
	}
}