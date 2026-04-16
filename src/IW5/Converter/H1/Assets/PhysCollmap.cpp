#include "stdafx.hpp"
#include "../Include.hpp"

#include "PhysCollmap.hpp"

#include <immintrin.h>

namespace ZoneTool::IW5
{
	namespace H1Converter
	{
		struct dmTransform
		{
			__m128 q;
			__m128 p;
		};

		__m128 _xmm_1_0{ .m128_f32 = {1.0f, 1.0f, 1.0f, 1.0f} };
		__m128 _xmm_4_0{ .m128_f32 = {4.0f, 4.0f, 4.0f, 4.0f} };
		__m128 _xmm_0_0492126{ .m128_f32 = {0.0492126f, 0.0492126f, 0.0492126f, 0.0492126f} };
		__m128 _xmm_0_0166667f{ .m128_f32 = {0.0166667f, 0.0166667f, 0.0166667f, 0.0166667f} };
		__m128 _xmm_0_00833333f{ .m128_f32 = {0.00833333f, 0.00833333f, 0.00833333f, 0.00833333f} };
		__m128 _xmm_0_166667f{ .m128_f32 = {0.166667f, 0.166667f, 0.166667f, 0.166667f} };
		__m128 _xmm_0_0416667f{ .m128_f32 = {0.0416667f, 0.0416667f, 0.0416667f, 0.0416667f} };

		__m128 dm_vec4_maxFloat{ .m128_f32 = {3.4028235e38f, 3.4028235e38f, 3.4028235e38f, 3.4028235e38f} };
		__m128 dm_vec4_oneHalf{ .m128_f32 = {0.5f, 0.5f, 0.5f, 0.5f} };
		__m128 dm_vec4_zeroSafe{ .m128_f32 = {1.1754944e-35f, 1.1754944e-35f, 1.1754944e-35f, 1.1754944e-35f} };
		__m128 dm_vec4_two{ .m128_f32 = {2.0f, 2.0f, 2.0f, 2.0f} };
		__m128 dm_vec4_three{ .m128_f32 = {3.0f, 3.0f, 3.0f, 3.0f} };
		__m128 dm_vec4_epsilon{ .m128_f32 = {1.1920929e-7f, 1.1920929e-7f, 1.1920929e-7f, 1.1920929e-7f} };
		__m128 dm_vec4_linearSlop{ .m128_f32 = {0.0049999999f, 0.0049999999f, 0.0049999999f, 0.0049999999f} };

		__m128 dm_vec4_unitX{ .m128_f32 = {1.0f, 0.0f, 0.0f, 0.0f} };
		__m128 dm_vec4_unitY{ .m128_f32 = {0.0f, 1.0f, 0.0f, 0.0f} };
		__m128 dm_vec4_unitZ{ .m128_f32 = {0.0f, 0.0f, 1.0f, 0.0f} };
		__m128 dm_quat_identity{ .m128_f32 = {0.0f, 0.0f, 0.0f, 1.0f} };

		__m128 _xmm1[8] =
		{
			{.m128_u32 = {0, 0, 0, 0}},
			{.m128_u32 = {0xFFFFFFFF, 0, 0, 0}},
			{.m128_u32 = {0xFFFFFFFF, 0, 0xFFFFFFFF, 0}},
			{.m128_u32 = {0, 0, 0xFFFFFFFF, 0}},
			{.m128_u32 = {0, 0xFFFFFFFF, 0, 0}},
			{.m128_u32 = {0xFFFFFFFF, 0xFFFFFFFF, 0, 0}},
			{.m128_u32 = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}},
			{.m128_u32 = {0, 0xFFFFFFFF, 0xFFFFFFFF, 0}}
		};

		__m128 _xmm2[6] =
		{
			{.m128_f32 = {0.0f, 0.0f, -1.0f, 0.0f}},
			{.m128_f32 = {0.0f, -1.0f, 0.0f, 0.0f}},
			{.m128_f32 = {1.0f, 0.0f, 0.0f, 0.0f}},
			{.m128_f32 = {-1.0f, 0.0f, 0.0f, 0.0f}},
			{.m128_f32 = {0.0f, 0.0f, 1.0f, 0.0f}},
			{.m128_f32 = {0.0f, 1.0f, 0.0f, 0.0f}}
		};

		void GetExtents(const Bounds& bounds, vec3_t& minExtent, vec3_t& maxExtent)
		{
			// Calculate the minimum extent (min corner)
			minExtent[0] = bounds.midPoint[0] - bounds.halfSize[0];
			minExtent[1] = bounds.midPoint[1] - bounds.halfSize[1];
			minExtent[2] = bounds.midPoint[2] - bounds.halfSize[2];

			// Calculate the maximum extent (max corner)
			maxExtent[0] = bounds.midPoint[0] + bounds.halfSize[0];
			maxExtent[1] = bounds.midPoint[1] + bounds.halfSize[1];
			maxExtent[2] = bounds.midPoint[2] + bounds.halfSize[2];
		}

		Bounds GetBounds(PhysGeomInfo* geom)
		{
			auto bounds = geom->bounds;
			if (geom->type == PHYS_GEOM_NONE || geom->type == PHYS_GEOM_BRUSHMODEL || geom->type == PHYS_GEOM_BRUSH)
			{
				bounds = geom->brushWrapper->bounds;
			}
			return bounds;
		}

		void ComputeIntegralsBox(PhysCollmap* asset, PhysGeomInfo* geom, H1::PhysGeomInfo* h1_geom)
		{
			auto* data = h1_geom->data;
			auto bounds = GetBounds(geom);

			data->m_volume = data->m_vertexCount * bounds.halfSize[0] * bounds.halfSize[1] * bounds.halfSize[2];

			data->m_area = (bounds.halfSize[2] * bounds.halfSize[1]) +
				(bounds.halfSize[1] * bounds.halfSize[0]) +
				(bounds.halfSize[2] * bounds.halfSize[0]);
			data->m_area *= data->m_vertexCount;

			/*if (asset->count == 1)
			{
				memcpy(&data->m_centroid, &asset->mass.centerOfMass, sizeof(float[3]));

				data->m_inertiaMoments.x = data->m_volume * asset->mass.momentsOfInertia[0];
				data->m_inertiaMoments.y = data->m_volume * asset->mass.momentsOfInertia[1];
				data->m_inertiaMoments.z = data->m_volume * asset->mass.momentsOfInertia[2];

				data->m_inertiaProducts.x = data->m_volume * asset->mass.productsOfInertia[0];
				data->m_inertiaProducts.y = data->m_volume * asset->mass.productsOfInertia[1];
				data->m_inertiaProducts.z = data->m_volume * asset->mass.productsOfInertia[2];
			}*/

			vec3_t extents_min = {};
			vec3_t extents_max = {};
			GetExtents(bounds, extents_min, extents_max);

			H1::dmFloat3 centroid = { 0.0f, 0.0f, 0.0f };
			H1::dmFloat3 momentsOfInertia = { 0.0f, 0.0f, 0.0f };
			H1::dmFloat3 productsOfInertia = { 0.0f, 0.0f, 0.0f };

			float mass = 1.0f;

			// Dimensions of the box
			float width = extents_max[0] - extents_min[0];
			float height = extents_max[1] - extents_min[1];
			float depth = extents_max[2] - extents_min[2];

			// Calculate Centroid (assuming geometric center for uniform box)
			//centroid.x = (extents_max[0] - extents_min[0]) / 2.0f;
			//centroid.y = (extents_min[1] + extents_max[1]) / 2.0f;
			//centroid.z = (extents_min[2] + extents_max[2]) / 2.0f;

			//centroid.x = bounds.midPoint[0];
			//centroid.y = bounds.midPoint[1];
			//centroid.z = bounds.midPoint[2];

			centroid.x = 0;
			centroid.y = 0;
			centroid.z = 0;

			// Moments of inertia for each axis (precomputed formula for a rectangular box)
			momentsOfInertia.x = (mass / 12.0f) * (height * height + depth * depth); // Ixx
			momentsOfInertia.y = (mass / 12.0f) * (width * width + depth * depth);   // Iyy
			momentsOfInertia.z = (mass / 12.0f) * (width * width + height * height); // Izz

			// Products of inertia - assuming no offset (they are zero for a symmetric box)
			productsOfInertia.x = 0.0f; // Ixy
			productsOfInertia.y = 0.0f; // Ixz
			productsOfInertia.z = 0.0f; // Iyz

			// Assign the calculated centroid, moments, and products of inertia to the dmPolytopeData structure
			data->m_centroid.x = centroid.x;
			data->m_centroid.y = centroid.y;
			data->m_centroid.z = centroid.z;

			data->m_inertiaMoments.x = data->m_volume * momentsOfInertia.x;
			data->m_inertiaMoments.y = data->m_volume * momentsOfInertia.y;
			data->m_inertiaMoments.z = data->m_volume * momentsOfInertia.z;

			data->m_inertiaProducts.x = data->m_volume * productsOfInertia.x;
			data->m_inertiaProducts.y = data->m_volume * productsOfInertia.y;
			data->m_inertiaProducts.z = data->m_volume * productsOfInertia.z;
		}

		void GetBoxVerticesAndPlanes(dmTransform* xf, __m128* extents, H1::dmFloat4 vertices[8], H1::dmPlane planes[6])
		{
			__m128 v3; // xmm5
			__int64 v4; // rdi
			__m128 v5; // xmm10
			__m128 v6; // xmm4
			__m128 v7; // xmm9
			__m128 v8; // xmm6
			__int64 v11; // rax
			__int64 v12; // rcx
			__m128 v13; // xmm3
			__m128 v14; // xmm8
			__m128 v15; // xmm4
			__m128 v16; // xmm2
			__m128 v17; // xmm1
			__m128 v18; // xmm1
			__m128 v19; // xmm9
			__int64 v20; // rcx
			__m128 v21; // xmm8
			__int64 v22; // rdx
			__m128 v23; // xmm5
			__m128 v24; // xmm4
			__m128* v25; // rax
			__m128 v26; // xmm2
			__m128 v27; // xmm4
			__m128 v28; // xmm4
			__m128 v29; // xmm3
			__m128 v30; // xmm3
			__m128 v31; // xmm1
			__m128 v32; // xmm3
			__m128 v33; // xmm1
			__int64 v34; // rcx
			__m128 v35; // xmm2
			H1::dmFloat4* v36; // rax
			__int64 v37; // rdx
			__int64 v38; // rcx
			__m128 v39; // xmm2
			H1::dmPlane* v40; // rax
			__int64 v41; // rax
			__int64 v42; // rax
			float v43; // xmm7_4
			float v44; // xmm1_4
			H1::dm_float32 v45; // xmm8_4
			float v46; // xmm0_4
			float v47; // xmm2_4
			float v48; // xmm1_4
			float v49; // xmm5_4
			float v50; // xmm8_4
			float v51; // xmm1_4
			float v52; // [rsp+30h] [rbp-D0h]
			__m128 v53; // [rsp+40h] [rbp-C0h] BYREF
			float v54; // [rsp+60h] [rbp-A0h]
			__m128 v55[15]; // [rsp+70h] [rbp-90h] BYREF

			v3 = xf->q;
			v4 = 8i64;
			v5 = xf->p;
			v6 = xf->q;
			v7 = _mm_shuffle_ps(v3, v3, 255);
			v8 = _mm_max_ps(*extents, _mm_mul_ps((__m128)_xmm_4_0, (__m128)_xmm_0_0492126));
			v11 = 0i64;
			v12 = 8i64;
			v13 = _mm_shuffle_ps(xf->q, xf->q, 210);
			v14 = _mm_sub_ps(_mm_set_ps1(0i64), v8);
			v53 = v8;
			v15 = _mm_shuffle_ps(v6, v3, 201);
			v55[0] = v14;
			do
			{
				v16 = _mm_or_ps(
					_mm_andnot_ps(*(__m128*)(v11 * 16 + (uint64_t)_xmm1), v14),
					_mm_and_ps(*(__m128*)(v11 * 16 + (uint64_t)_xmm1), v8));
				v17 = _mm_add_ps(
					_mm_sub_ps(_mm_mul_ps(_mm_shuffle_ps(v16, v16, 210), v15), _mm_mul_ps(_mm_shuffle_ps(v16, v16, 201), v13)),
					_mm_mul_ps(v7, v16));
				v18 = _mm_sub_ps(_mm_mul_ps(_mm_shuffle_ps(v17, v17, 210), v15), _mm_mul_ps(_mm_shuffle_ps(v17, v17, 201), v13));
				v55[v11 + 7] = _mm_add_ps(_mm_add_ps(_mm_add_ps(v18, v18), v16), v5);
				++v11;
				--v12;
			} while (v12);
			v19 = _mm_shuffle_ps(v3, v3, 255);
			v20 = 0i64;
			v21 = _mm_shuffle_ps(v3, v3, 210);
			v22 = 6i64;
			v23 = _mm_shuffle_ps(v3, v3, 201);
			do
			{
				v24 = *(__m128*)(v20 * 16 + (uint64_t)_xmm2);
				v25 = v55;
				if ((_mm_movemask_ps(_mm_cmplt_ps(v24, _mm_set_ps1(0i64))) & 7) == 0)
					v25 = &v53;
				v26 = _mm_mul_ps(*v25, v24);
				v27 = _mm_shuffle_ps(v24, v24, 39);
				v27.m128_f32[0] = (float)(_mm_shuffle_ps(v26, v26, 85).m128_f32[0] + _mm_shuffle_ps(v26, v26, 0).m128_f32[0])
					+ _mm_shuffle_ps(v26, v26, 170).m128_f32[0];
				v28 = _mm_shuffle_ps(v27, v27, 39);
				v29 = _mm_add_ps(
					_mm_sub_ps(_mm_mul_ps(_mm_shuffle_ps(v28, v28, 210), v23), _mm_mul_ps(_mm_shuffle_ps(v28, v28, 201), v21)),
					_mm_mul_ps(v19, v28));
				v30 = _mm_sub_ps(_mm_mul_ps(_mm_shuffle_ps(v29, v29, 210), v23), _mm_mul_ps(_mm_shuffle_ps(v29, v29, 201), v21));
				v31 = _mm_add_ps(_mm_add_ps(v30, v30), v28);
				v32 = _mm_shuffle_ps(v31, v31, 39);
				v33 = _mm_mul_ps(v31, v5);
				v32.m128_f32[0] = (float)((float)(_mm_shuffle_ps(v33, v33, 85).m128_f32[0] + _mm_shuffle_ps(v33, v33, 0).m128_f32[0])
					+ _mm_shuffle_ps(v33, v33, 170).m128_f32[0])
					+ _mm_shuffle_ps(v28, v28, 255).m128_f32[0];
				v55[++v20] = _mm_shuffle_ps(v32, v32, 39);
				--v22;
			} while (v22);

			v34 = 0i64;
			do
			{
				v35 = v55[v34 + 7];
				++v34;
				v36 = vertices;
				v53.m128_f32[0] = v35.m128_f32[0];
				v53.m128_f32[1] = v35.m128_f32[1];
				v53.m128_f32[2] = v35.m128_f32[2];
				v53.m128_f32[3] = v35.m128_f32[3];
				v36[v34 - 1] = *(H1::dmFloat4*)&v53;
				--v4;
			} while (v4);

			v37 = 6i64;
			v38 = 0i64;
			do
			{
				v39 = v55[++v38];
				v40 = planes;
				v53.m128_f32[0] = v39.m128_f32[0];
				v53.m128_f32[1] = v39.m128_f32[1];
				v53.m128_f32[2] = v39.m128_f32[2];
				v53.m128_f32[3] = v39.m128_f32[3];
				v40[v38 - 1] = *(H1::dmPlane*)&v53;
				--v37;
			} while (v37);
		}

		void GenerateBox(PhysCollmap* asset, PhysGeomInfo* geom, H1::PhysGeomInfo* h1_geom, allocator& allocator)
		{
			auto* data = h1_geom->data;

			auto bounds = GetBounds(geom);

			const auto vertexCount = 8;
			const auto subEdgeCount = 24;
			const auto facesCount = 6;

			H1::dmSubEdge edge[facesCount][4] = {};
			edge[0][0] = { .twinOffset = 1, .tail = 4, .left = 0, .next = 20 };
			edge[0][1] = { .twinOffset = -1, .tail = 5, .left = 5, .next = 3 };
			edge[0][2] = { .twinOffset = 1, .tail = 7, .left = 3, .next = 5 };
			edge[0][3] = { .twinOffset = -1, .tail = 4, .left = 5, .next = 9 };
			edge[1][0] = { .twinOffset = 1, .tail = 0, .left = 0, .next = 0 };
			edge[1][1] = { .twinOffset = -1, .tail = 4, .left = 3, .next = 15 };
			edge[1][2] = { .twinOffset = 1, .tail = 1, .left = 0, .next = 4 };
			edge[1][3] = { .twinOffset = -1, .tail = 0, .left = 1, .next = 16 };
			edge[2][0] = { .twinOffset = 1, .tail = 6, .left = 4, .next = 13 };
			edge[2][1] = { .twinOffset = -1, .tail = 7, .left = 5, .next = 23 };
			edge[2][2] = { .twinOffset = 1, .tail = 6, .left = 2, .next = 17 };
			edge[2][3] = { .twinOffset = -1, .tail = 2, .left = 4, .next = 8 };
			edge[3][0] = { .twinOffset = 1, .tail = 3, .left = 3, .next = 2 };
			edge[3][1] = { .twinOffset = -1, .tail = 7, .left = 4, .next = 19 };
			edge[3][2] = { .twinOffset = 1, .tail = 3, .left = 1, .next = 7 };
			edge[3][3] = { .twinOffset = -1, .tail = 0, .left = 3, .next = 12 };
			edge[4][0] = { .twinOffset = 1, .tail = 1, .left = 1, .next = 18 };
			edge[4][1] = { .twinOffset = -1, .tail = 2, .left = 2, .next = 21 };
			edge[4][2] = { .twinOffset = 1, .tail = 2, .left = 1, .next = 14 };
			edge[4][3] = { .twinOffset = -1, .tail = 3, .left = 4, .next = 11 };
			edge[5][0] = { .twinOffset = 1, .tail = 5, .left = 0, .next = 6 };
			edge[5][1] = { .twinOffset = -1, .tail = 1, .left = 2, .next = 22 };
			edge[5][2] = { .twinOffset = 1, .tail = 5, .left = 2, .next = 10 };
			edge[5][3] = { .twinOffset = -1, .tail = 6, .left = 5, .next = 1 };

			H1::dm_uint8 faceSubEdges[facesCount] = {};
			faceSubEdges[0] = 0;
			faceSubEdges[1] = 7;
			faceSubEdges[2] = 10;
			faceSubEdges[3] = 2;
			faceSubEdges[4] = 8;
			faceSubEdges[5] = 1;

			H1::dmFloat4 vertices[vertexCount]{};
			H1::dmPlane planes[facesCount]{};

			__m128 extents{ bounds.halfSize[0], bounds.halfSize[1], bounds.halfSize[2] };
			dmTransform xf{};
			xf.q = { .m128_f32{0.0f, 0.0f, 0.0f, 1.0f} };
			xf.p = { .m128_f32{0.0f, 0.0f, 0.0f, 0.0f} };

			GetBoxVerticesAndPlanes(&xf, &extents, vertices, planes);
			
			data->m_vertexCount = vertexCount;
			data->m_subEdgeCount = subEdgeCount;
			data->m_faceCount = facesCount;

			data->m_aVertices = allocator.allocate<H1::dmFloat4>(data->m_vertexCount);
			data->m_aPlanes = allocator.allocate<H1::dmPlane>(data->m_faceCount);
			data->m_aSubEdges = allocator.allocate<H1::dmSubEdge>(data->m_subEdgeCount);
			data->m_aFaceSubEdges = allocator.allocate<H1::dm_uint8>(data->m_faceCount);

			for (auto i = 0; i < data->m_vertexCount; i++)
			{
				data->m_aVertices[i].x = vertices[i].x;
				data->m_aVertices[i].y = vertices[i].y;
				data->m_aVertices[i].z = vertices[i].z;
				data->m_aVertices[i].w = vertices[i].w;
			}

			for (auto i = 0; i < data->m_faceCount; i++)
			{
				data->m_aPlanes[i].normal.x = planes[i].normal.x;
				data->m_aPlanes[i].normal.y = planes[i].normal.y;
				data->m_aPlanes[i].normal.z = planes[i].normal.z;
				data->m_aPlanes[i].offset = planes[i].offset;
			}

			memcpy(data->m_aSubEdges, edge, sizeof(H1::dmSubEdge) * data->m_subEdgeCount);
			memcpy(data->m_aFaceSubEdges, faceSubEdges, sizeof(H1::dm_uint8) * data->m_faceCount);

			ComputeIntegralsBox(asset, geom, h1_geom);

			data->contents = -1;
		}

		void GenerateCylinder(PhysCollmap* asset, PhysGeomInfo* geom, H1::PhysGeomInfo* h1_geom, allocator& allocator)
		{
			auto* data = h1_geom->data;

			// generate...

			data->contents = -1;
		}

		void GenerateCapsule(PhysCollmap* asset, PhysGeomInfo* geom, H1::PhysGeomInfo* h1_geom, allocator& allocator)
		{
			auto* data = h1_geom->data;

			// generate...

			data->contents = -1;
		}

		void GenerateBrush(PhysCollmap* asset, PhysGeomInfo* geom, H1::PhysGeomInfo* h1_geom, allocator& allocator)
		{
			auto* data = h1_geom->data;

			// generate...

			data->contents = -1;
		}

		H1::PhysCollmap* GenerateH1Asset(PhysCollmap* asset, allocator& allocator)
		{
			ZONETOOL_INFO("Generating PhysCollmap %s", asset->name);

			auto* h1_asset = allocator.allocate<H1::PhysCollmap>();

			h1_asset->name = asset->name;
			
			h1_asset->count = asset->count;
			memcpy(&h1_asset->mass, &asset->mass, sizeof(PhysMass));
			memcpy(&h1_asset->bounds, &asset->bounds, sizeof(Bounds));

			float center[3] = { 0.0f, 0.0f, 0.0f };
			float inertia1[3] = { 0.0f, 0.0f, 0.0f };
			float inertia2[3] = { 0.0f, 0.0f, 0.0f };
			//Bounds bounds{};

			h1_asset->geoms = allocator.allocate<H1::PhysGeomInfo>(h1_asset->count);
			for (auto i = 0; i < h1_asset->count; i++)
			{
				auto* geom = &asset->geoms[i];
				auto* h1_geom = &h1_asset->geoms[i];

				h1_geom->data = allocator.allocate<H1::dmPolytopeData>();
				auto* data = h1_geom->data;

				/*auto type = geom->type;
				switch (type)
				{
				case PHYS_GEOM_NONE:
				{
					GenerateBrush(asset, geom, h1_geom, allocator);
					ZONETOOL_INFO("PhysCollmap geom %d (none?) for %s generated!", i, asset->name);
					break; // sphere...
				}
				case PHYS_GEOM_BOX:
				{
					GenerateBox(asset, geom, h1_geom, allocator);
					ZONETOOL_INFO("PhysCollmap geom %d (box) for %s generated!", i, asset->name);
					break;
				}
				case PHYS_GEOM_CYLINDER:
				{
					//GenerateCylinder(asset, geom, h1_geom, allocator);
					ZONETOOL_INFO("PhysCollmap geom %d (cylinder) for %s generated!", i, asset->name);
					break;
				}
				case PHYS_GEOM_CAPSULE:
				{
					GenerateCapsule(asset, geom, h1_geom, allocator);
					ZONETOOL_INFO("PhysCollmap geom %d (capsule) for %s generated!", i, asset->name);
					break;
				}
				case PHYS_GEOM_BRUSHMODEL:
				case PHYS_GEOM_BRUSH:
				{
					GenerateBrush(asset, geom, h1_geom, allocator);
					ZONETOOL_INFO("PhysCollmap geom %d (brush) for %s generated!", i, asset->name);
					break;
				}
				default:
					__debugbreak();
					break;
				}*/
				GenerateBox(asset, geom, h1_geom, allocator);

				center[0] += data->m_centroid.x;
				center[1] += data->m_centroid.y;
				center[2] += data->m_centroid.z;

				inertia1[0] += data->m_inertiaMoments.x / data->m_volume;
				inertia1[1] += data->m_inertiaMoments.y / data->m_volume;
				inertia1[2] += data->m_inertiaMoments.z / data->m_volume;

				inertia2[0] += data->m_inertiaProducts.x / data->m_volume;
				inertia2[1] += data->m_inertiaProducts.y / data->m_volume;
				inertia2[2] += data->m_inertiaProducts.z / data->m_volume;

				//bounds = GetBounds(geom);
			}

			asset->mass.centerOfMass[0] = center[0] / asset->count;
			asset->mass.centerOfMass[1] = center[1] / asset->count;
			asset->mass.centerOfMass[2] = center[2] / asset->count;

			asset->mass.momentsOfInertia[0] = (inertia1[0] / asset->count);
			asset->mass.momentsOfInertia[1] = (inertia1[1] / asset->count);
			asset->mass.momentsOfInertia[2] = (inertia1[2] / asset->count);

			asset->mass.productsOfInertia[0] = (inertia2[0] / asset->count);
			asset->mass.productsOfInertia[1] = (inertia2[1] / asset->count);
			asset->mass.productsOfInertia[2] = (inertia2[2] / asset->count);

			// re-calc bounds too?
			//asset->bounds = bounds;

			return h1_asset;
		}

		H1::PhysCollmap* convert(PhysCollmap* asset, allocator& allocator)
		{
			// generate h1 asset
			return GenerateH1Asset(asset, allocator);
		}
	}
}