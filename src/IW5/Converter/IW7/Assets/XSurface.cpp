#include "stdafx.hpp"
#include "../Include.hpp"

#include "XSurface.hpp"

#include "X64/Utils/Utils.hpp"

namespace ZoneTool::IW5
{
	namespace IW7Converter
	{
		void GenerateIW7BlendVerts(IW5::XSurface* surf_, IW7::XSurface* surf, allocator& mem)
		{
			unsigned int size = (2 * surf->blendVertCounts[0]
				+ 4 * surf->blendVertCounts[1]
				+ 6 * surf->blendVertCounts[2]
				+ 8 * surf->blendVertCounts[3]);

			if (!size) return;

			surf->blendVertSize = size * sizeof(unsigned short);
			surf->blendVerts = mem.manual_allocate<unsigned short>(surf->blendVertSize);

			assert(surf->blendVertSize % 2 == 0);

			unsigned short a = 0;
			unsigned short b = 0;
			for (unsigned short s = 0; s < (surf->blendVertCounts[0]); s++)
			{
				surf->blendVerts[a++] = surf_->vertInfo.vertsBlend[b + 0] / 64;
				surf->blendVerts[a++] = 0;

				b += 1;
			}

			for (unsigned short s = 0; s < (surf->blendVertCounts[1]); s++)
			{
				surf->blendVerts[a++] = surf_->vertInfo.vertsBlend[b + 0] / 64;
				surf->blendVerts[a++] = surf_->vertInfo.vertsBlend[b + 1] / 64;
				surf->blendVerts[a++] = surf_->vertInfo.vertsBlend[b + 2];
				surf->blendVerts[a++] = 0;
				b += 3;
			}

			for (unsigned short s = 0; s < (surf->blendVertCounts[2]); s++)
			{
				surf->blendVerts[a++] = surf_->vertInfo.vertsBlend[b + 0] / 64;
				surf->blendVerts[a++] = surf_->vertInfo.vertsBlend[b + 1] / 64;
				surf->blendVerts[a++] = surf_->vertInfo.vertsBlend[b + 2];
				surf->blendVerts[a++] = surf_->vertInfo.vertsBlend[b + 3] / 64;
				surf->blendVerts[a++] = surf_->vertInfo.vertsBlend[b + 4];
				surf->blendVerts[a++] = 0;
				b += 5;
			}

			for (unsigned short s = 0; s < (surf->blendVertCounts[3]); s++)
			{
				surf->blendVerts[a++] = surf_->vertInfo.vertsBlend[b + 0] / 64;
				surf->blendVerts[a++] = surf_->vertInfo.vertsBlend[b + 1] / 64;
				surf->blendVerts[a++] = surf_->vertInfo.vertsBlend[b + 2];
				surf->blendVerts[a++] = surf_->vertInfo.vertsBlend[b + 3] / 64;
				surf->blendVerts[a++] = surf_->vertInfo.vertsBlend[b + 4];
				surf->blendVerts[a++] = surf_->vertInfo.vertsBlend[b + 5] / 64;
				surf->blendVerts[a++] = surf_->vertInfo.vertsBlend[b + 6];
				surf->blendVerts[a++] = 0;
				b += 7;
			}

			assert(a == size);
		}

		void GenerateIW7XSurface(IW7::XSurface* IW7_asset, XSurface* asset, allocator& mem)
		{
			IW7_asset->flags = 0;
			IW7_asset->flags |= ((asset->flags & IW5::SURF_FLAG_VERTCOL_GREY) != 0) ? IW7::SURF_FLAG_VERTCOL_GREY : 0;
			IW7_asset->flags |= ((asset->flags & IW5::SURF_FLAG_VERTCOL_NONE) != 0) ? IW7::SURF_FLAG_VERTCOL_NONE : 0;
			IW7_asset->flags |= ((asset->flags & IW5::SURF_FLAG_SKINNED) != 0) ? IW7::SURF_FLAG_SKINNED : 0;
			IW7_asset->flags |= IW7::SURF_FLAG_SELF_VISIBILITY;

			IW7_asset->vertCount = asset->vertCount;
			IW7_asset->triCount = asset->triCount;
			IW7_asset->rigidVertListCount = asset->vertListCount;

			// blend verts
			memcpy(&IW7_asset->blendVertCounts, &asset->vertInfo.vertCount, sizeof(asset->vertInfo.vertCount));
			GenerateIW7BlendVerts(asset, IW7_asset, mem);

			// triIndices
			IW7_asset->triIndices = reinterpret_cast<IW7::Face * __ptr64>(asset->triIndices);

			// verts
			IW7_asset->verts0.packedVerts0 = mem.allocate<IW7::GfxPackedVertex>(asset->vertCount);
			for (unsigned short i = 0; i < asset->vertCount; i++)
			{
				memcpy(&IW7_asset->verts0.packedVerts0[i], &asset->verts0.packedVerts0[i], sizeof(IW5::GfxPackedVertex));

				float texCoord_unpacked[2]{ 0.0f, 0.0f };
				PackedVec::Vec2UnpackTexCoords(asset->verts0.packedVerts0[i].texCoord.packed, texCoord_unpacked);
				std::swap(texCoord_unpacked[0], texCoord_unpacked[1]); // these are inverted...
				IW7_asset->verts0.packedVerts0[i].texCoord.packed = PackedVec::Vec2PackTexCoords(texCoord_unpacked);

				// re-calculate these...
				float normal_unpacked[3]{ 0.0f, 0.0f, 0.0f };
				PackedVec::Vec3UnpackUnitVec(asset->verts0.packedVerts0[i].normal.array, normal_unpacked);

				float tangent_unpacked[3]{ 0.0f, 0.0f, 0.0f };
				PackedVec::Vec3UnpackUnitVec(asset->verts0.packedVerts0[i].tangent.array, tangent_unpacked);

				float normal[3] = { normal_unpacked[0], normal_unpacked[1], normal_unpacked[2] };
				float tangent[3] = { tangent_unpacked[0], tangent_unpacked[1], tangent_unpacked[2] };

				IW7_asset->verts0.packedVerts0[i].normal.packed = PackedVec::Vec3PackUnitVec(normal);
				IW7_asset->verts0.packedVerts0[i].tangent.packed = PackedVec::Vec3PackUnitVec(tangent);

				float default_visibility[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
				IW7_asset->verts0.packedVerts0[i].selfVisibility.packed = self_visibility::XSurfacePackSelfVisibility(default_visibility);
				
				// correct color : bgra->rgba
				IW7_asset->verts0.packedVerts0[i].color.array[0] = asset->verts0.packedVerts0[i].color.array[2];
				IW7_asset->verts0.packedVerts0[i].color.array[1] = asset->verts0.packedVerts0[i].color.array[1];
				IW7_asset->verts0.packedVerts0[i].color.array[2] = asset->verts0.packedVerts0[i].color.array[0];
				IW7_asset->verts0.packedVerts0[i].color.array[3] = asset->verts0.packedVerts0[i].color.array[3];
			}

			// rigidVertLists
			IW7_asset->rigidVertLists = mem.allocate<IW7::XRigidVertList>(asset->vertListCount);
			for (int i = 0; i < asset->vertListCount; i++)
			{
				IW7_asset->rigidVertLists[i].boneOffsetIndex = asset->vertList[i].boneOffset >> 6; // R_MarkFragments_AnimatedXModel
				IW7_asset->rigidVertLists[i].vertCount = asset->vertList[i].vertCount;
				IW7_asset->rigidVertLists[i].triOffset = asset->vertList[i].triOffset;
				IW7_asset->rigidVertLists[i].triCount = asset->vertList[i].triCount;

				if (asset->vertList[i].collisionTree)
				{
					IW7_asset->rigidVertLists[i].collisionTree = mem.allocate<IW7::XSurfaceCollisionTree>();
					memcpy(&IW7_asset->rigidVertLists[i].collisionTree->trans, &asset->vertList[i].collisionTree->trans,
						sizeof(asset->vertList[i].collisionTree->trans));
					memcpy(&IW7_asset->rigidVertLists[i].collisionTree->scale, &asset->vertList[i].collisionTree->scale,
						sizeof(asset->vertList[i].collisionTree->scale));

					IW7_asset->rigidVertLists[i].collisionTree->nodeCount = asset->vertList[i].collisionTree->nodeCount;
					IW7_asset->rigidVertLists[i].collisionTree->nodes = mem.allocate<IW7::XSurfaceCollisionNode>(
						asset->vertList[i].collisionTree->nodeCount);
					for (int j = 0; j < asset->vertList[i].collisionTree->nodeCount; j++)
					{
						memcpy(&IW7_asset->rigidVertLists[i].collisionTree->nodes[j].aabb.mins,
							&asset->vertList[i].collisionTree->nodes[j].aabb.mins,
							sizeof(asset->vertList[i].collisionTree->nodes[j].aabb.mins));
						memcpy(&IW7_asset->rigidVertLists[i].collisionTree->nodes[j].aabb.maxs,
							&asset->vertList[i].collisionTree->nodes[j].aabb.maxs,
							sizeof(asset->vertList[i].collisionTree->nodes[j].aabb.maxs));

						IW7_asset->rigidVertLists[i].collisionTree->nodes[j].childBeginIndex =
							asset->vertList[i].collisionTree->nodes[j].childBeginIndex;
						IW7_asset->rigidVertLists[i].collisionTree->nodes[j].childCount =
							asset->vertList[i].collisionTree->nodes[j].childCount;
					}

					IW7_asset->rigidVertLists[i].collisionTree->leafCount = asset->vertList[i].collisionTree->leafCount;
					IW7_asset->rigidVertLists[i].collisionTree->leafs = mem.allocate<IW7::XSurfaceCollisionLeaf>(
						asset->vertList[i].collisionTree->leafCount);
					for (int j = 0; j < asset->vertList[i].collisionTree->leafCount; j++)
					{
						IW7_asset->rigidVertLists[i].collisionTree->leafs[j].triangleBeginIndex =
							asset->vertList[i].collisionTree->leafs[j].triangleBeginIndex;
					}
				}
			}

			// partBits
			memcpy(&IW7_asset->partBits, &asset->partBits, sizeof(asset->partBits));
		}

		IW7::XModelSurfs* GenerateIW7XModelSurfs(XModelSurfs* asset, allocator& mem)
		{
			// allocate IW7 XModelSurfs structure
			const auto IW7_asset = mem.allocate<IW7::XModelSurfs>();

			IW7_asset->name = mem.duplicate_string(asset->name);
			IW7_asset->numsurfs = asset->numsurfs;
			memcpy(&IW7_asset->partBits, &asset->partBits, sizeof(asset->partBits));

			IW7_asset->surfs = mem.allocate<IW7::XSurface>(asset->numsurfs);
			for (int i = 0; i < asset->numsurfs; i++)
			{
				GenerateIW7XSurface(&IW7_asset->surfs[i], &asset->surfs[i], mem);
			}

			return IW7_asset;
		}

		IW7::XModelSurfs* convert(XModelSurfs* asset, allocator& allocator)
		{
			// generate IW7 surfaces
			return GenerateIW7XModelSurfs(asset, allocator);
		}
	}
}