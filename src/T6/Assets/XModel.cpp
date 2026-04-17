// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"
#include "H1/Assets/XModel.hpp"
#include "H1/Assets/XSurface.hpp"

namespace ZoneTool
{
	namespace T6
	{
		namespace
		{
			namespace bounds
			{
				static auto compute(float* mins, float* maxs)
				{
					float bounds[2][3]{ 0 };
					for (int i = 0; i < 3; ++i)
					{
						bounds[1][i] = (maxs[i] - mins[i]) / 2;
						bounds[0][i] = bounds[1][i] + mins[i];
					}
					return bounds;
				}
			}

			namespace PackedShit
			{
#define _BYTE  uint8_t
#define _WORD  uint16_t
#define _DWORD uint32_t
#define _QWORD uint64_t

#define LOBYTE(x)   (*((_BYTE*)&(x)))   // low byte
#define LOWORD(x)   (*((_WORD*)&(x)))   // low word
#define LODWORD(x)  (*((_DWORD*)&(x)))  // low dword
#define HIBYTE(x)   (*((_BYTE*)&(x)+1))
#define HIWORD(x)   (*((_WORD*)&(x)+1))
#define HIDWORD(x)  (*((_DWORD*)&(x)+1))
#define BYTEn(x, n)   (*((_BYTE*)&(x)+n))
#define WORDn(x, n)   (*((_WORD*)&(x)+n))
#define BYTE1(x)   BYTEn(x,  1)         // byte 1 (counting from 0)
#define BYTE2(x)   BYTEn(x,  2)

				PackedTexCoords Vec2PackTexCoords(float* in) // iw5 func
				{
					int v2; // eax
					int v3; // esi
					int v4; // eax
					int v5; // ecx
					PackedTexCoords result; // eax
					int v7; // [esp+8h] [ebp+8h]
					int v8; // [esp+8h] [ebp+8h]

					v7 = LODWORD(in[0]);
					v2 = (int)((2 * v7) ^ 0x80003FFF) >> 14;
					if (v2 < 0x3FFF)
					{
						if (v2 <= -16384)
							LOWORD(v2) = -16384;
					}
					else
					{
						LOWORD(v2) = 0x3FFF;
					}
					v3 = (v7 >> 16) & 0xC000 | v2 & 0x3FFF;
					v8 = LODWORD(in[1]);
					v4 = (int)((2 * v8) ^ 0x80003FFF) >> 14;
					v5 = (v8 >> 16) & 0xC000;
					if (v4 < 0x3FFF)
					{
						if (v4 <= -16384)
							LOWORD(v4) = -16384;
						result.packed = v3 + ((v5 | v4 & 0x3FFF) << 16);
					}
					else
					{
						result.packed = v3 + ((v5 | 0x3FFF) << 16);
					}
					return result;
				}

				void Vec2UnpackTexCoords(const PackedTexCoords in, float* out) // iw5 func
				{
					unsigned int val;

					if (LOWORD(in.packed))
						val = ((LOWORD(in.packed) & 0x8000) << 16) | (((((in.packed & 0x3FFF) << 14) - (~(LOWORD(in.packed) << 14) & 0x10000000)) ^ 0x80000001) >> 1);
					else
						val = 0;

					out[0] = *reinterpret_cast<float*>(&val);

					if (HIWORD(in.packed))
						val = ((HIWORD(in.packed) & 0x8000) << 16) | (((((HIWORD(in.packed) & 0x3FFF) << 14)
						- (~(HIWORD(in.packed) << 14) & 0x10000000)) ^ 0x80000001) >> 1);
					else
						val = 0;

					out[1] = *reinterpret_cast<float*>(&val);
				}

				PackedUnitVec Vec3PackUnitVec(float* in) // h1 func
				{
					int x = (int)floor(((fmaxf(-1.0f, fminf(1.0f, in[0])) + 1.0f) * 0.5f) * 1023.0f + 0.5f);
					int y = (int)floor(((fmaxf(-1.0f, fminf(1.0f, in[1])) + 1.0f) * 0.5f) * 1023.0f + 0.5f);
					int z = (int)floor(((fmaxf(-1.0f, fminf(1.0f, in[2])) + 1.0f) * 0.5f) * 1023.0f + 0.5f);
					return (PackedUnitVec)((z << 20) | (y << 10) | x);
				}

				void Vec3UnpackUnitVec(const PackedUnitVec in, float* out) // t6 func
				{
					float decodeScale;

					decodeScale = (in.array[3] - -192.0f) / 32385.0f;
					out[0] = (in.array[0] - 127.0f) * decodeScale;
					out[1] = (in.array[1] - 127.0f) * decodeScale;
					out[2] = (in.array[2] - 127.0f) * decodeScale;
				}
			}

			void GenerateH1BlendVertsShit(H1::XSurface* surf)
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

			void GenerateH1XSurface(H1::XSurface* h1_asset, XSurface* asset, allocator& allocator)
			{
				h1_asset->flags = asset->tileMode;

				h1_asset->vertCount = asset->vertCount;
				h1_asset->triCount = asset->triCount;
				h1_asset->rigidVertListCount = asset->vertListCount;

				// blend verts
				memcpy(&h1_asset->blendVertCounts, &asset->vertInfo.vertCount, sizeof(asset->vertInfo.vertCount));
				h1_asset->blendVerts = reinterpret_cast<unsigned short* __ptr64>(asset->vertInfo.vertsBlend);

				h1_asset->blendVertsTable = allocator.allocate<H1::BlendVertsUnknown>(asset->vertCount);
				GenerateH1BlendVertsShit(h1_asset);

				// triIndices
				h1_asset->triIndices = reinterpret_cast<H1::Face * __ptr64>(asset->triIndices); // this is draw indices?
				h1_asset->triIndices2 = allocator.allocate<H1::Face>(asset->triCount); // this is collision indices?
				for (unsigned short i = 0; i < asset->triCount; i++)
				{
					memcpy(&h1_asset->triIndices2[i], &h1_asset->triIndices[i], sizeof(H1::Face));
				}

				// verts
				//h1_asset->verts0.packedVerts0 = reinterpret_cast<IW6::GfxPackedVertex* __ptr64>(asset->verticies);
				h1_asset->verts0.packedVerts0 = allocator.allocate<H1::GfxPackedVertex>(asset->vertCount);
				for (unsigned short i = 0; i < asset->vertCount; i++)
				{
					memcpy(&h1_asset->verts0.packedVerts0[i], &asset->verts0[i], sizeof(T6::GfxPackedVertex));

					//float texCoord_unpacked[2]{ 0 };
					//PackedShit::Vec2UnpackTexCoords(asset->verts0[i].texCoord, texCoord_unpacked);
					//std::swap(texCoord_unpacked[0], texCoord_unpacked[1]); // these are inverted...
					//h1_asset->verts0.packedVerts0[i].texCoord.packed = PackedShit::Vec2PackTexCoords(texCoord_unpacked).packed;
					h1_asset->verts0.packedVerts0[i].texCoord.packed = asset->verts0[i].texCoord.packed;

					// re-calculate these...
					float normal_unpacked[3]{ 0 };
					PackedShit::Vec3UnpackUnitVec(asset->verts0[i].normal, normal_unpacked);
					h1_asset->verts0.packedVerts0[i].normal.packed = PackedShit::Vec3PackUnitVec(normal_unpacked).packed;

					// i don't understand why normal unpacked seems to be correct instead
					//float tangent_unpacked[3]{ 0 };
					//PackedShit::Vec3UnpackUnitVec(asset->verticies[i].tangent, tangent_unpacked);
					h1_asset->verts0.packedVerts0[i].tangent.packed = PackedShit::Vec3PackUnitVec(normal_unpacked).packed;

					// correct color : bgra->rgba
					h1_asset->verts0.packedVerts0[i].color.array[0] = asset->verts0[i].color.array[2];
					h1_asset->verts0.packedVerts0[i].color.array[1] = asset->verts0[i].color.array[1];
					h1_asset->verts0.packedVerts0[i].color.array[2] = asset->verts0[i].color.array[0];
					h1_asset->verts0.packedVerts0[i].color.array[3] = asset->verts0[i].color.array[3];
				}

				// unknown
				h1_asset->unknown0 = allocator.allocate<H1::UnknownXSurface0>(asset->vertCount); // related to indices2?
				for (unsigned short i = 0; i < asset->vertCount; i++)
				{
					h1_asset->unknown0[i].xyz[0] = h1_asset->verts0.packedVerts0[i].xyz[0];
					h1_asset->unknown0[i].xyz[1] = h1_asset->verts0.packedVerts0[i].xyz[1];
					h1_asset->unknown0[i].xyz[2] = h1_asset->verts0.packedVerts0[i].xyz[2];
					h1_asset->unknown0[i].normal = h1_asset->verts0.packedVerts0[i].normal;
				}

				// rigidVertLists
				h1_asset->rigidVertLists = allocator.allocate<H1::XRigidVertList>(asset->vertListCount);
				for (int i = 0; i < asset->vertListCount; i++)
				{
					h1_asset->rigidVertLists[i].boneOffset = asset->vertList[i].boneOffset;
					h1_asset->rigidVertLists[i].vertCount = asset->vertList[i].vertCount;
					h1_asset->rigidVertLists[i].triOffset = asset->vertList[i].triOffset;
					h1_asset->rigidVertLists[i].triCount = asset->vertList[i].triCount;

					if (asset->vertList[i].collisionTree)
					{
						h1_asset->rigidVertLists[i].collisionTree = allocator.allocate<H1::XSurfaceCollisionTree>();
						memcpy(&h1_asset->rigidVertLists[i].collisionTree->trans, &asset->vertList[i].collisionTree->trans,
							sizeof(asset->vertList[i].collisionTree->trans));
						memcpy(&h1_asset->rigidVertLists[i].collisionTree->scale, &asset->vertList[i].collisionTree->scale,
							sizeof(asset->vertList[i].collisionTree->scale));

						h1_asset->rigidVertLists[i].collisionTree->nodeCount = asset->vertList[i].collisionTree->nodeCount;
						h1_asset->rigidVertLists[i].collisionTree->nodes = allocator.allocate<H1::XSurfaceCollisionNode>(
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
						h1_asset->rigidVertLists[i].collisionTree->leafs = allocator.allocate<H1::XSurfaceCollisionLeaf>(
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

			H1::XModelSurfs* GenerateH1XModelSurfs(XModel* model, int index, allocator& allocator)
			{
				const auto h1_asset = allocator.allocate<H1::XModelSurfs>();

				h1_asset->name = allocator.duplicate_string(va("%s_lod%d", model->name, index));
				h1_asset->numsurfs = model->lods[index].numSurfacesInLod;

				if (h1_asset->numsurfs > 16)
				{
					h1_asset->numsurfs = 16;
				}

				memcpy(&h1_asset->partBits, &model->lods[index].partBits, sizeof(model->lods[index].partBits));

				h1_asset->surfs = allocator.allocate<H1::XSurface>(h1_asset->numsurfs);

				for (unsigned short i = 0; i < h1_asset->numsurfs; i++)
				{
					GenerateH1XSurface(&h1_asset->surfs[i], &model->surfs[model->lods[index].surfIndex + i], allocator);
				}

				return h1_asset;
			}

			H1::XModel* generate_h1_xmodel(XModel* asset, allocator& allocator)
			{
				const auto h1_asset = allocator.allocate<H1::XModel>();

				h1_asset->name = asset->name;
				h1_asset->numBones = asset->numBones;
				h1_asset->numRootBones = asset->numRootBones;
				h1_asset->numsurfs = asset->numSurfaces;
				h1_asset->numReactiveMotionParts = 0;
				h1_asset->lodRampType = asset->lodRampType;
				h1_asset->scale = 1.0f;
				memset(h1_asset->noScalePartBits, 0, sizeof(unsigned int[8]));

				h1_asset->boneNames = allocator.allocate<H1::scr_string_t>(asset->numBones);
				for (auto i = 0u; i < asset->numBones; i++)
				{
					h1_asset->boneNames[i] = static_cast<H1::scr_string_t>(asset->boneNames[i]);
				}

				REINTERPRET_CAST_SAFE(h1_asset->parentList, asset->parentList);
				h1_asset->tagAngles = reinterpret_cast<H1::XModelAngle*>(asset->quats);
				h1_asset->tagPositions = reinterpret_cast<H1::XModelTagPos*>(asset->trans);
				REINTERPRET_CAST_SAFE(h1_asset->partClassification, asset->partClassification);
				REINTERPRET_CAST_SAFE(h1_asset->baseMat, asset->animMatrix);

				h1_asset->reactiveMotionParts = nullptr;
				h1_asset->reactiveMotionTweaks = nullptr;

				h1_asset->materialHandles = allocator.allocate<H1::Material* __ptr64>(asset->numSurfaces);
				for (auto i = 0u; i < asset->numSurfaces; i++)
				{
					if (asset->materials[i])
					{
						h1_asset->materialHandles[i] = allocator.allocate<H1::Material>();
						h1_asset->materialHandles[i]->name = allocator.duplicate_string(asset->materials[i]->name);
					}
				}

				for (auto i = 0; i < 6; i++)
				{
					h1_asset->lodInfo[i].dist = 1000000.0f;
				}

				for (auto i = 0; i < asset->numLods; i++)
				{
					h1_asset->lodInfo[i].dist = asset->lods[i].dist;

					h1_asset->lodInfo[i].numsurfs = asset->lods[i].numSurfacesInLod;
					h1_asset->lodInfo[i].surfIndex = asset->lods[i].surfIndex;

					if (h1_asset->lodInfo[i].numsurfs > 16)
					{
						h1_asset->lodInfo[i].numsurfs = 16;
					}

					h1_asset->lodInfo[i].modelSurfs = GenerateH1XModelSurfs(asset, i, allocator);
					H1::IXSurface::dump(h1_asset->lodInfo[i].modelSurfs);

					std::memcpy(&h1_asset->lodInfo[i].partBits, &asset->lods[i].partBits, sizeof(asset->lods[i].partBits));
				}

				h1_asset->numLods = static_cast<char>(asset->numLods);
				h1_asset->collLod = static_cast<char>(asset->collLod);
				h1_asset->numCompositeModels = 0;
				h1_asset->flags = asset->flags;

				h1_asset->numCollSurfs = asset->numCollSurfs;
				h1_asset->collSurfs = allocator.allocate<H1::XModelCollSurf_s>(asset->numCollSurfs);
				for (auto i = 0; i < asset->numCollSurfs; i++)
				{
					memcpy(&h1_asset->collSurfs[i].bounds, bounds::compute(asset->collSurfs[i].mins, asset->collSurfs[i].maxs), sizeof(float[2][3]));

					h1_asset->collSurfs[i].boneIdx = asset->collSurfs[i].boneIdx;
					h1_asset->collSurfs[i].contents = asset->collSurfs[i].contents;
					h1_asset->collSurfs[i].surfFlags = /*convert_surf_flags(*/asset->collSurfs[i].surfFlags/*)*/;
				}

				h1_asset->contents = asset->contents;

				h1_asset->boneInfo = allocator.allocate<H1::XBoneInfo>(asset->numBones);
				for (auto i = 0u; i < asset->numBones; ++i)
				{
					auto* target = &h1_asset->boneInfo[i];
					auto* source = &asset->boneInfo[i];

					target->radiusSquared = source->radiusSquared;

					memcpy(&target->bounds, bounds::compute(source->bounds[0], source->bounds[1]), sizeof(float[2][3]));
					target->bounds.midPoint[0] += source->offset[0];
					target->bounds.midPoint[1] += source->offset[1];
					target->bounds.midPoint[2] += source->offset[2];
				}

				h1_asset->radius = asset->radius;
				memcpy(&h1_asset->bounds, bounds::compute(asset->mins, asset->maxs), sizeof(float[2][3]));
				h1_asset->memUsage = asset->memUsage;

				if (asset->physPreset)
				{
					h1_asset->physPreset = allocator.allocate<H1::PhysPreset>();
					h1_asset->physPreset->name = allocator.duplicate_string(asset->physPreset->name);
				}

				h1_asset->physCollmap = nullptr;

				h1_asset->invHighMipRadius = allocator.allocate<unsigned short>(asset->numSurfaces);
				for (unsigned char i = 0; i < asset->numSurfaces; i++)
				{
					h1_asset->invHighMipRadius[i] = 0xFFFF;
				}

				h1_asset->quantization = 0.0f;

				return h1_asset;
			}
		}

		void IXModel::dump(XModel* asset)
		{
			allocator allocator;
			const auto h1_asset = generate_h1_xmodel(asset, allocator);
			H1::IXModel::dump(h1_asset);
		}
	}
}
