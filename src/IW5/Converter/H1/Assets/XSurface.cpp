#include "stdafx.hpp"
#include "../Include.hpp"

#include "XSurface.hpp"

#include "X64/Utils/Utils.hpp"

namespace ZoneTool::IW5
{
	namespace H1Converter
	{
		void GenerateH1BlendVertsTable(H1::XSurface* surf)
		{
			unsigned short a = 0;
			unsigned short b = 0;
			unsigned short index = 1;
			for (short s = 0; s < (surf->blendVertCounts[0]); s++)
			{
				auto& table = surf->blendVertsTable[a];

				table.b[0] = surf->blendVerts[b + 0] / 64;
				table.b[1] = 0;
				table.b[2] = 0;
				table.b[3] = 0;
				table.b[4] = 0;
				table.b[5] = 0;
				table.b[6] = 0;
				table.b[7] = 0;

				table.b[8] = 0;
				table.b[9] = 0;
				table.b[10] = 0;
				table.b[11] = 0;
				table.b[12] = 0;
				table.b[13] = 0;
				table.b[14] = 0;

				table.blendVertCountIndex = index;

				a++;
				b += 1;
			}
			index++;

			for (short s = 0; s < (surf->blendVertCounts[1]); s++)
			{
				auto& table = surf->blendVertsTable[a];

				table.b[0] = surf->blendVerts[b + 0] / 64;
				table.b[1] = surf->blendVerts[b + 1] / 64;
				table.b[2] = 0;
				table.b[3] = 0;
				table.b[4] = 0;
				table.b[5] = 0;
				table.b[6] = 0;
				table.b[7] = 0;

				table.b[8] = surf->blendVerts[b + 2];
				table.b[9] = 0;
				table.b[10] = 0;
				table.b[11] = 0;
				table.b[12] = 0;
				table.b[13] = 0;
				table.b[14] = 0;

				table.blendVertCountIndex = index;

				a++;
				b += 3;
			}
			index++;

			for (short s = 0; s < (surf->blendVertCounts[2]); s++)
			{
				auto& table = surf->blendVertsTable[a];

				table.b[0] = surf->blendVerts[b + 0] / 64;
				table.b[1] = surf->blendVerts[b + 1] / 64;
				table.b[2] = surf->blendVerts[b + 3] / 64;
				table.b[3] = 0;
				table.b[4] = 0;
				table.b[5] = 0;
				table.b[6] = 0;
				table.b[7] = 0;

				table.b[8] = surf->blendVerts[b + 2];
				table.b[9] = surf->blendVerts[b + 4];
				table.b[10] = 0;
				table.b[11] = 0;
				table.b[12] = 0;
				table.b[13] = 0;
				table.b[14] = 0;

				table.blendVertCountIndex = index;

				a++;
				b += 5;
			}
			index++;

			for (short s = 0; s < (surf->blendVertCounts[3]); s++)
			{
				auto& table = surf->blendVertsTable[a];

				table.b[0] = surf->blendVerts[b + 0] / 64;
				table.b[1] = surf->blendVerts[b + 1] / 64;
				table.b[2] = surf->blendVerts[b + 3] / 64;
				table.b[3] = surf->blendVerts[b + 5] / 64;
				table.b[4] = 0;
				table.b[5] = 0;
				table.b[6] = 0;
				table.b[7] = 0;

				table.b[8] = surf->blendVerts[b + 2];
				table.b[9] = surf->blendVerts[b + 4];
				table.b[10] = surf->blendVerts[b + 6];
				table.b[11] = 0;
				table.b[12] = 0;
				table.b[13] = 0;
				table.b[14] = 0;

				table.blendVertCountIndex = index;

				a++;
				b += 7;
			}
			index++;
		}

		void GenerateH1XSurface(H1::XSurface* h1_asset, XSurface* asset, allocator& mem)
		{
			h1_asset->flags = 0;
			h1_asset->flags |= ((asset->flags & IW5::SURF_FLAG_VERTCOL_GREY) != 0) ? H1::SURF_FLAG_VERTCOL_GREY : 0;
			h1_asset->flags |= ((asset->flags & IW5::SURF_FLAG_VERTCOL_NONE) != 0) ? H1::SURF_FLAG_VERTCOL_NONE : 0;
			//h1_asset->flags |= ((asset->flags & IW5::SURF_FLAG_QUANTIZED) != 0) ? H1::SURF_FLAG_QUANTIZED : 0;
			h1_asset->flags |= ((asset->flags & IW5::SURF_FLAG_SKINNED) != 0) ? H1::SURF_FLAG_SKINNED : 0;

			h1_asset->vertCount = asset->vertCount;
			h1_asset->triCount = asset->triCount;
			h1_asset->rigidVertListCount = asset->vertListCount;

			// blend verts
			memcpy(&h1_asset->blendVertCounts, &asset->vertInfo.vertCount, sizeof(asset->vertInfo.vertCount));
			h1_asset->blendVerts = reinterpret_cast<unsigned short* __ptr64>(asset->vertInfo.vertsBlend);
			h1_asset->blendVertsTable = mem.allocate<H1::BlendVertsUnknown>(asset->vertCount);
			GenerateH1BlendVertsTable(h1_asset);

			// triIndices
			h1_asset->triIndices = reinterpret_cast<H1::Face * __ptr64>(asset->triIndices); // this is draw indices?
			h1_asset->triIndices2 = mem.allocate<H1::Face>(asset->triCount); // this is collision indices?
			for (unsigned short i = 0; i < asset->triCount; i++)
			{
				memcpy(&h1_asset->triIndices2[i], &h1_asset->triIndices[i], sizeof(H1::Face));
			}

			// verts
			//h1_asset->verts0.packedVerts0 = reinterpret_cast<IW6::GfxPackedVertex* __ptr64>(asset->verticies);
			h1_asset->verts0.packedVerts0 = mem.allocate<H1::GfxPackedVertex>(asset->vertCount);
			for (unsigned short i = 0; i < asset->vertCount; i++)
			{
				memcpy(&h1_asset->verts0.packedVerts0[i], &asset->verts0.packedVerts0[i], sizeof(IW5::GfxPackedVertex));

				float texCoord_unpacked[2]{ 0.0f, 0.0f };
				PackedVec::Vec2UnpackTexCoords(asset->verts0.packedVerts0[i].texCoord.packed, texCoord_unpacked);
				std::swap(texCoord_unpacked[0], texCoord_unpacked[1]); // these are inverted...
				h1_asset->verts0.packedVerts0[i].texCoord.packed = PackedVec::Vec2PackTexCoords(texCoord_unpacked);

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

				h1_asset->verts0.packedVerts0[i].normal.packed = PackedVec::Vec3PackUnitVec(normal);
				h1_asset->verts0.packedVerts0[i].tangent.packed = PackedVec::Vec3PackUnitVec(tangent);
				
				// correct color : bgra->rgba
				h1_asset->verts0.packedVerts0[i].color.array[0] = asset->verts0.packedVerts0[i].color.array[2];
				h1_asset->verts0.packedVerts0[i].color.array[1] = asset->verts0.packedVerts0[i].color.array[1];
				h1_asset->verts0.packedVerts0[i].color.array[2] = asset->verts0.packedVerts0[i].color.array[0];
				h1_asset->verts0.packedVerts0[i].color.array[3] = asset->verts0.packedVerts0[i].color.array[3];
			}

			// unknown
			h1_asset->unknown0 = mem.allocate<H1::UnknownXSurface0>(asset->vertCount); // related to indices2?
			for (unsigned short i = 0; i < asset->vertCount; i++)
			{
				h1_asset->unknown0[i].xyz[0] = h1_asset->verts0.packedVerts0[i].xyz[0];
				h1_asset->unknown0[i].xyz[1] = h1_asset->verts0.packedVerts0[i].xyz[1];
				h1_asset->unknown0[i].xyz[2] = h1_asset->verts0.packedVerts0[i].xyz[2];
				h1_asset->unknown0[i].normal = h1_asset->verts0.packedVerts0[i].normal;
			}

			// rigidVertLists
			h1_asset->rigidVertLists = mem.allocate<H1::XRigidVertList>(asset->vertListCount);
			for (int i = 0; i < asset->vertListCount; i++)
			{
				h1_asset->rigidVertLists[i].boneOffset = asset->vertList[i].boneOffset;
				h1_asset->rigidVertLists[i].vertCount = asset->vertList[i].vertCount;
				h1_asset->rigidVertLists[i].triOffset = asset->vertList[i].triOffset;
				h1_asset->rigidVertLists[i].triCount = asset->vertList[i].triCount;

				if (asset->vertList[i].collisionTree)
				{
					h1_asset->rigidVertLists[i].collisionTree = mem.allocate<H1::XSurfaceCollisionTree>();
					memcpy(&h1_asset->rigidVertLists[i].collisionTree->trans, &asset->vertList[i].collisionTree->trans,
						sizeof(asset->vertList[i].collisionTree->trans));
					memcpy(&h1_asset->rigidVertLists[i].collisionTree->scale, &asset->vertList[i].collisionTree->scale,
						sizeof(asset->vertList[i].collisionTree->scale));

					h1_asset->rigidVertLists[i].collisionTree->nodeCount = asset->vertList[i].collisionTree->nodeCount;
					h1_asset->rigidVertLists[i].collisionTree->nodes = mem.allocate<H1::XSurfaceCollisionNode>(
						asset->vertList[i].collisionTree->nodeCount);
					for (int j = 0; j < asset->vertList[i].collisionTree->nodeCount; j++)
					{
						memcpy(&h1_asset->rigidVertLists[i].collisionTree->nodes[j].aabb.mins,
							&asset->vertList[i].collisionTree->nodes[j].aabb.mins,
							sizeof(asset->vertList[i].collisionTree->nodes[j].aabb.mins));
						memcpy(&h1_asset->rigidVertLists[i].collisionTree->nodes[j].aabb.maxs,
							&asset->vertList[i].collisionTree->nodes[j].aabb.maxs,
							sizeof(asset->vertList[i].collisionTree->nodes[j].aabb.maxs));

						h1_asset->rigidVertLists[i].collisionTree->nodes[j].childBeginIndex =
							asset->vertList[i].collisionTree->nodes[j].childBeginIndex;
						h1_asset->rigidVertLists[i].collisionTree->nodes[j].childCount =
							asset->vertList[i].collisionTree->nodes[j].childCount;
					}

					h1_asset->rigidVertLists[i].collisionTree->leafCount = asset->vertList[i].collisionTree->leafCount;
					h1_asset->rigidVertLists[i].collisionTree->leafs = mem.allocate<H1::XSurfaceCollisionLeaf>(
						asset->vertList[i].collisionTree->leafCount);
					for (int j = 0; j < asset->vertList[i].collisionTree->leafCount; j++)
					{
						h1_asset->rigidVertLists[i].collisionTree->leafs[j].triangleBeginIndex =
							asset->vertList[i].collisionTree->leafs[j].triangleBeginIndex;
					}
				}
			}

			// partBits
			memcpy(&h1_asset->partBits, &asset->partBits, sizeof(asset->partBits));
		}

		H1::XModelSurfs* GenerateH1XModelSurfs(XModelSurfs* asset, allocator& mem)
		{
			// allocate H1 XModelSurfs structure
			const auto h1_asset = mem.allocate<H1::XModelSurfs>();

			h1_asset->name = mem.duplicate_string(asset->name);
			h1_asset->numsurfs = asset->numsurfs;
			memcpy(&h1_asset->partBits, &asset->partBits, sizeof(asset->partBits));

			h1_asset->surfs = mem.allocate<H1::XSurface>(asset->numsurfs);
			for (int i = 0; i < asset->numsurfs; i++)
			{
				GenerateH1XSurface(&h1_asset->surfs[i], &asset->surfs[i], mem);
			}

			return h1_asset;
		}

		H1::XModelSurfs* convert(XModelSurfs* asset, allocator& allocator)
		{
			// generate h1 surfaces
			return GenerateH1XModelSurfs(asset, allocator);
		}
	}
}