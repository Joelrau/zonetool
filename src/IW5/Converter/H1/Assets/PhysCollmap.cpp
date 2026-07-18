#include "stdafx.hpp"
#include "../Include.hpp"

#include "PhysCollmap.hpp"

#include <immintrin.h>
#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include <cmath>

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
			if (geom->type == PHYS_GEOM_NONE || geom->type == PHYS_GEOM_BRUSHMODEL ||
				geom->type == PHYS_GEOM_BRUSH || geom->type == PHYS_GEOM_COLLMAP)
			{
				// brush based geoms carry their bounds inside the brush wrapper
				if (geom->brushWrapper)
				{
					bounds = geom->brushWrapper->bounds;
				}
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

		// produces H1 dmPolytopeData (Domino half-edge convex hull) from either an
		// explicit set of face loops (cylinder / capsule) or a brush plane set.
		// verified against GenerateBox above and the retail
		// dmPolytopeBuilder decompile):
		namespace hull
		{
			struct Vec3 { float x, y, z; };

			inline Vec3 operator+(const Vec3& a, const Vec3& b) { return { a.x + b.x, a.y + b.y, a.z + b.z }; }
			inline Vec3 operator-(const Vec3& a, const Vec3& b) { return { a.x - b.x, a.y - b.y, a.z - b.z }; }
			inline Vec3 operator*(const Vec3& a, float s) { return { a.x * s, a.y * s, a.z * s }; }
			inline float dot(const Vec3& a, const Vec3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
			inline Vec3 cross(const Vec3& a, const Vec3& b)
			{
				return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
			}
			inline float length(const Vec3& a) { return std::sqrt(dot(a, a)); }
			inline Vec3 normalize(const Vec3& a)
			{
				const float l = length(a);
				return l > 1e-8f ? a * (1.0f / l) : Vec3{ 0.0f, 0.0f, 0.0f };
			}

			struct Plane { Vec3 n; float d; };

			// volume / area / centroid / inertia via triangulated-face divergence integration.
			void computeMass(H1::dmPolytopeData* data, const std::vector<Vec3>& verts,
				const std::vector<std::vector<int>>& faces)
			{
				double vol = 0.0, area = 0.0;
				double cx = 0.0, cy = 0.0, cz = 0.0;
				double mxx = 0.0, myy = 0.0, mzz = 0.0, mxy = 0.0, myz = 0.0, mzx = 0.0;

				for (const auto& face : faces)
				{
					const Vec3& a = verts[face[0]];
					for (size_t i = 1; i + 1 < face.size(); i++)
					{
						const Vec3& b = verts[face[i]];
						const Vec3& c = verts[face[i + 1]];

						area += 0.5 * (double)length(cross(b - a, c - a));

						const double det =
							(double)a.x * ((double)b.y * c.z - (double)b.z * c.y) -
							(double)a.y * ((double)b.x * c.z - (double)b.z * c.x) +
							(double)a.z * ((double)b.x * c.y - (double)b.y * c.x);

						vol += det / 6.0;
						cx += det * ((double)a.x + b.x + c.x);
						cy += det * ((double)a.y + b.y + c.y);
						cz += det * ((double)a.z + b.z + c.z);

						const double sx = (double)a.x + b.x + c.x;
						const double sy = (double)a.y + b.y + c.y;
						const double sz = (double)a.z + b.z + c.z;
						mxx += det * (sx * sx + (double)a.x * a.x + (double)b.x * b.x + (double)c.x * c.x);
						myy += det * (sy * sy + (double)a.y * a.y + (double)b.y * b.y + (double)c.y * c.y);
						mzz += det * (sz * sz + (double)a.z * a.z + (double)b.z * b.z + (double)c.z * c.z);
						mxy += det * (sx * sy + (double)a.x * a.y + (double)b.x * b.y + (double)c.x * c.y);
						myz += det * (sy * sz + (double)a.y * a.z + (double)b.y * b.z + (double)c.y * c.z);
						mzx += det * (sz * sx + (double)a.z * a.x + (double)b.z * b.x + (double)c.z * c.x);
					}
				}

				// a negative running volume means the loops came out inward; magnitudes
				// are still correct, so normalise the sign of every moment together.
				const double sign = (vol < 0.0) ? -1.0 : 1.0;

				data->m_area = (float)area;
				data->m_volume = (float)(vol * sign);

				if (std::fabs(vol) > 1e-9)
				{
					data->m_centroid.x = (float)(cx / (24.0 * vol));
					data->m_centroid.y = (float)(cy / (24.0 * vol));
					data->m_centroid.z = (float)(cz / (24.0 * vol));
				}
				else
				{
					data->m_centroid = { 0.0f, 0.0f, 0.0f };
				}

				mxx = mxx * sign / 120.0; myy = myy * sign / 120.0; mzz = mzz * sign / 120.0;
				mxy = mxy * sign / 120.0; myz = myz * sign / 120.0; mzx = mzx * sign / 120.0;

				data->m_inertiaMoments.x = (float)(myy + mzz);
				data->m_inertiaMoments.y = (float)(mxx + mzz);
				data->m_inertiaMoments.z = (float)(mxx + myy);
				data->m_inertiaProducts.x = (float)(-mxy);
				data->m_inertiaProducts.y = (float)(-myz);
				data->m_inertiaProducts.z = (float)(-mzx);
			}

			// cook vertices + (arbitrary winding) face loops into a dmPolytopeData.
			bool build(H1::dmPolytopeData* data, std::vector<Vec3> verts,
				std::vector<std::vector<int>> faceLoops, allocator& alloc)
			{
				if (verts.size() < 4 || faceLoops.size() < 4)
				{
					return false;
				}

				Vec3 center{ 0.0f, 0.0f, 0.0f };
				for (const auto& v : verts) center = center + v;
				center = center * (1.0f / (float)verts.size());

				std::vector<std::vector<int>> faces;
				std::vector<Plane> planes;
				faces.reserve(faceLoops.size());
				planes.reserve(faceLoops.size());

				for (auto& loop : faceLoops)
				{
					if (loop.size() < 3) continue;

					// Newell's method -> robust polygon normal
					Vec3 n{ 0.0f, 0.0f, 0.0f };
					for (size_t i = 0; i < loop.size(); i++)
					{
						const Vec3& a = verts[loop[i]];
						const Vec3& b = verts[loop[(i + 1) % loop.size()]];
						n.x += (a.y - b.y) * (a.z + b.z);
						n.y += (a.z - b.z) * (a.x + b.x);
						n.z += (a.x - b.x) * (a.y + b.y);
					}
					n = normalize(n);
					if (n.x == 0.0f && n.y == 0.0f && n.z == 0.0f) continue; // degenerate face

					Vec3 fc{ 0.0f, 0.0f, 0.0f };
					for (int idx : loop) fc = fc + verts[idx];
					fc = fc * (1.0f / (float)loop.size());

					// force outward orientation (normal points away from the solid centre)
					if (dot(n, fc - center) < 0.0f)
					{
						std::reverse(loop.begin(), loop.end());
						n = n * -1.0f;
					}

					planes.push_back({ n, dot(n, verts[loop[0]]) });
					faces.push_back(loop);
				}

				if (faces.size() < 4) return false;

				// assign half-edge indices in twin pairs so twinOffset stays +/-1
				std::map<std::pair<int, int>, int> pairIndex;
				std::map<std::pair<int, int>, int> directedHe;
				int pairCount = 0;

				for (const auto& face : faces)
				{
					const size_t n = face.size();
					for (size_t i = 0; i < n; i++)
					{
						const int a = face[i];
						const int b = face[(i + 1) % n];
						if (a == b) return false;

						const auto key = std::make_pair(std::min(a, b), std::max(a, b));
						auto it = pairIndex.find(key);
						int k;
						if (it == pairIndex.end())
						{
							k = pairCount++;
							pairIndex[key] = k;
						}
						else
						{
							k = it->second;
						}

						const int he = (a < b) ? (2 * k) : (2 * k + 1);
						const auto dkey = std::make_pair(a, b);
						if (directedHe.count(dkey)) return false; // non-manifold
						directedHe[dkey] = he;
					}
				}

				const int subEdgeCount = pairCount * 2;
				if (verts.size() > 255 || faces.size() > 255 || subEdgeCount > 255)
				{
					return false;
				}

				std::vector<int> heTail(subEdgeCount, -1);
				std::vector<int> heLeft(subEdgeCount, -1);
				std::vector<int> heNext(subEdgeCount, -1);
				std::vector<signed char> heTwin(subEdgeCount, 0);
				std::vector<int> faceFirstHe(faces.size(), -1);

				for (size_t f = 0; f < faces.size(); f++)
				{
					const auto& face = faces[f];
					const size_t n = face.size();
					for (size_t i = 0; i < n; i++)
					{
						const int a = face[i];
						const int b = face[(i + 1) % n];
						const int c = face[(i + 2) % n];
						const int he = directedHe[std::make_pair(a, b)];

						heTail[he] = a;
						heLeft[he] = (int)f;
						heNext[he] = directedHe[std::make_pair(b, c)];
						heTwin[he] = (a < b) ? (signed char)1 : (signed char)-1;
						if (faceFirstHe[f] < 0) faceFirstHe[f] = he;
					}
				}

				// closed manifold check: every half-edge slot must be populated
				for (int i = 0; i < subEdgeCount; i++)
				{
					if (heTail[i] < 0) return false;
				}

				data->m_vertexCount = (int)verts.size();
				data->m_faceCount = (int)faces.size();
				data->m_subEdgeCount = subEdgeCount;

				data->m_aVertices = alloc.allocate<H1::dmFloat4>(data->m_vertexCount);
				data->m_aPlanes = alloc.allocate<H1::dmPlane>(data->m_faceCount);
				data->m_aSubEdges = alloc.allocate<H1::dmSubEdge>(data->m_subEdgeCount);
				data->m_aFaceSubEdges = alloc.allocate<H1::dm_uint8>(data->m_faceCount);

				for (int i = 0; i < data->m_vertexCount; i++)
				{
					data->m_aVertices[i].x = verts[i].x;
					data->m_aVertices[i].y = verts[i].y;
					data->m_aVertices[i].z = verts[i].z;
					data->m_aVertices[i].w = 0.0f;
				}
				for (int i = 0; i < data->m_faceCount; i++)
				{
					data->m_aPlanes[i].normal.x = planes[i].n.x;
					data->m_aPlanes[i].normal.y = planes[i].n.y;
					data->m_aPlanes[i].normal.z = planes[i].n.z;
					data->m_aPlanes[i].offset = planes[i].d;
					data->m_aFaceSubEdges[i] = (H1::dm_uint8)faceFirstHe[i];
				}
				for (int i = 0; i < subEdgeCount; i++)
				{
					data->m_aSubEdges[i].twinOffset = (H1::dm_int8)heTwin[i];
					data->m_aSubEdges[i].tail = (H1::dm_uint8)heTail[i];
					data->m_aSubEdges[i].left = (H1::dm_uint8)heLeft[i];
					data->m_aSubEdges[i].next = (H1::dm_uint8)heNext[i];
				}

				computeMass(data, verts, faces);

				if (!(data->m_volume > 1e-6f))
				{
					return false;
				}

				// m_surfaceTypes / m_vertexMaterials stay null (retail SetAsBox leaves
				// them null too), contents default matches dmPolytopeData ctor (-1).
				data->contents = -1;
				return true;
			}

			// local (axis-aligned) point -> collmap space using the geom transform
			inline Vec3 transformLocal(const float orient[3][3], const float mid[3], const Vec3& p)
			{
				return {
					orient[0][0] * p.x + orient[0][1] * p.y + orient[0][2] * p.z + mid[0],
					orient[1][0] * p.x + orient[1][1] * p.y + orient[1][2] * p.z + mid[1],
					orient[2][0] * p.x + orient[2][1] * p.y + orient[2][2] * p.z + mid[2],
				};
			}
		}

		// picks the "radius" plane pair (two ~equal half sizes) vs the axis half size.
		static void ResolveCylinderAxis(const Bounds& bounds, int& axis, float& radius, float& halfHeight)
		{
			const float hs[3] = { bounds.halfSize[0], bounds.halfSize[1], bounds.halfSize[2] };
			auto approxEqual = [](float a, float b)
			{
				return std::fabs(a - b) <= 0.05f * std::max(1.0f, std::max(std::fabs(a), std::fabs(b)));
			};

			if (approxEqual(hs[0], hs[1])) { axis = 2; radius = 0.5f * (hs[0] + hs[1]); halfHeight = hs[2]; }
			else if (approxEqual(hs[0], hs[2])) { axis = 1; radius = 0.5f * (hs[0] + hs[2]); halfHeight = hs[1]; }
			else if (approxEqual(hs[1], hs[2])) { axis = 0; radius = 0.5f * (hs[1] + hs[2]); halfHeight = hs[0]; }
			else { axis = 2; radius = 0.5f * (hs[0] + hs[1]); halfHeight = hs[2]; } // best effort
		}

		static hull::Vec3 CylPoint(int axis, float r, float ca, float sa, float h,
			const float orient[3][3], const float mid[3])
		{
			hull::Vec3 lp;
			if (axis == 2) lp = { r * ca, r * sa, h };
			else if (axis == 0) lp = { h, r * ca, r * sa };
			else lp = { r * ca, h, r * sa };
			return hull::transformLocal(orient, mid, lp);
		}

		bool GenerateCylinder(PhysCollmap* asset, PhysGeomInfo* geom, H1::PhysGeomInfo* h1_geom, allocator& allocator)
		{
			auto* data = h1_geom->data;
			const auto bounds = GetBounds(geom);

			int axis; float radius, halfHeight;
			ResolveCylinderAxis(bounds, axis, radius, halfHeight);
			if (radius <= 1e-4f || halfHeight <= 1e-4f)
			{
				ZONETOOL_WARNING("PhysCollmap \"%s\": degenerate cylinder geom, skipping", asset->name);
				return false;
			}

			const int SEG = 16; // matches retail SetAsCylinder (2*pi / 16 step)
			std::vector<hull::Vec3> verts;
			verts.reserve(SEG * 2);

			for (int ring = 0; ring < 2; ring++)
			{
				const float h = (ring == 0) ? -halfHeight : halfHeight;
				for (int i = 0; i < SEG; i++)
				{
					const float a = (2.0f * 3.14159265358979f * i) / SEG;
					verts.push_back(CylPoint(axis, radius, std::cos(a), std::sin(a), h,
						geom->orientation, bounds.midPoint));
				}
			}

			std::vector<std::vector<int>> faces;

			std::vector<int> bottom, top;
			for (int i = 0; i < SEG; i++) { bottom.push_back(i); top.push_back(SEG + i); }
			faces.push_back(bottom);
			faces.push_back(top);

			for (int i = 0; i < SEG; i++)
			{
				const int ni = (i + 1) % SEG;
				faces.push_back({ i, ni, SEG + ni, SEG + i });
			}

			if (!hull::build(data, verts, faces, allocator))
			{
				ZONETOOL_WARNING("PhysCollmap \"%s\": failed to cook cylinder geom", asset->name);
				return false;
			}
			return true;
		}

		bool GenerateCapsule(PhysCollmap* asset, PhysGeomInfo* geom, H1::PhysGeomInfo* h1_geom, allocator& allocator)
		{
			auto* data = h1_geom->data;
			const auto bounds = GetBounds(geom);

			int axis; float radius, halfHeight;
			ResolveCylinderAxis(bounds, axis, radius, halfHeight);
			if (radius <= 1e-4f || halfHeight <= 1e-4f)
			{
				ZONETOOL_WARNING("PhysCollmap \"%s\": degenerate capsule geom, skipping", asset->name);
				return false;
			}

			// cylindrical body plus one intermediate latitude ring per
			// hemispherical cap (kept coarse so vertex/face/edge counts stay < 256).
			const float cylHalf = std::max(0.0f, halfHeight - radius);
			const float capR = radius * 0.70710678f;  // 45 degree ring
			const float capH = radius * 0.70710678f;

			const int SEG = 10;

			// rings ordered bottom -> top; radius 0 marks a pole (single vertex)
			struct Ring { float r; float h; };
			const Ring rings[] = {
				{ 0.0f,   -(cylHalf + radius) }, // bottom pole
				{ capR,   -(cylHalf + capH)   }, // bottom cap ring
				{ radius, -cylHalf            }, // bottom rim
				{ radius,  cylHalf            }, // top rim
				{ capR,    (cylHalf + capH)   }, // top cap ring
				{ 0.0f,    (cylHalf + radius) }, // top pole
			};
			const int ringCount = (int)(sizeof(rings) / sizeof(rings[0]));

			std::vector<hull::Vec3> verts;
			std::vector<int> ringStart(ringCount);
			std::vector<int> ringSize(ringCount);

			for (int r = 0; r < ringCount; r++)
			{
				ringStart[r] = (int)verts.size();
				if (rings[r].r <= 1e-5f)
				{
					ringSize[r] = 1;
					verts.push_back(CylPoint(axis, 0.0f, 1.0f, 0.0f, rings[r].h,
						geom->orientation, bounds.midPoint));
				}
				else
				{
					ringSize[r] = SEG;
					for (int i = 0; i < SEG; i++)
					{
						const float a = (2.0f * 3.14159265358979f * i) / SEG;
						verts.push_back(CylPoint(axis, rings[r].r, std::cos(a), std::sin(a), rings[r].h,
							geom->orientation, bounds.midPoint));
					}
				}
			}

			std::vector<std::vector<int>> faces;
			for (int r = 0; r + 1 < ringCount; r++)
			{
				const int lo = ringStart[r], loN = ringSize[r];
				const int hi = ringStart[r + 1], hiN = ringSize[r + 1];

				if (loN == 1) // bottom pole fan
				{
					for (int i = 0; i < hiN; i++)
						faces.push_back({ lo, hi + i, hi + ((i + 1) % hiN) });
				}
				else if (hiN == 1) // top pole fan
				{
					for (int i = 0; i < loN; i++)
						faces.push_back({ lo + i, lo + ((i + 1) % loN), hi });
				}
				else // quad band (loN == hiN == SEG)
				{
					for (int i = 0; i < loN; i++)
					{
						const int ni = (i + 1) % loN;
						faces.push_back({ lo + i, lo + ni, hi + ni, hi + i });
					}
				}
			}

			if (!hull::build(data, verts, faces, allocator))
			{
				ZONETOOL_WARNING("PhysCollmap \"%s\": failed to cook capsule geom", asset->name);
				return false;
			}
			return true;
		}

		bool GenerateBrush(PhysCollmap* asset, PhysGeomInfo* geom, H1::PhysGeomInfo* h1_geom, allocator& allocator)
		{
			auto* data = h1_geom->data;
			auto* bw = geom->brushWrapper;
			if (!bw)
			{
				ZONETOOL_WARNING("PhysCollmap \"%s\": brush geom has null brushWrapper, skipping", asset->name);
				return false;
			}

			// the convex brush is the intersection of its half-spaces: the 6 axial
			// planes (tight to the wrapper bounds) plus the non-axial side planes.
			// IW5 plane dists share H1's offset = dot(outwardNormal, point) convention.
			std::vector<hull::Plane> planes;
			const auto& b = bw->bounds;
			const float minx = b.midPoint[0] - b.halfSize[0], maxx = b.midPoint[0] + b.halfSize[0];
			const float miny = b.midPoint[1] - b.halfSize[1], maxy = b.midPoint[1] + b.halfSize[1];
			const float minz = b.midPoint[2] - b.halfSize[2], maxz = b.midPoint[2] + b.halfSize[2];
			planes.push_back({ {  1.0f,  0.0f,  0.0f },  maxx });
			planes.push_back({ { -1.0f,  0.0f,  0.0f }, -minx });
			planes.push_back({ {  0.0f,  1.0f,  0.0f },  maxy });
			planes.push_back({ {  0.0f, -1.0f,  0.0f }, -miny });
			planes.push_back({ {  0.0f,  0.0f,  1.0f },  maxz });
			planes.push_back({ {  0.0f,  0.0f, -1.0f }, -minz });

			// coplanar duplicates would emit two identical face loops and fail the
			// manifold check, so sides that repeat an axial (or earlier) plane are skipped
			auto isDuplicatePlane = [&planes](const hull::Vec3& n, float d)
			{
				for (const auto& pl : planes)
				{
					if (hull::dot(pl.n, n) > 0.999f && std::fabs(pl.d - d) < 0.1f)
						return true;
				}
				return false;
			};

			for (int s = 0; s < bw->brush.numsides; s++)
			{
				auto* side = &bw->brush.sides[s];
				if (!side || !side->plane) continue;
				const hull::Vec3 n{ side->plane->normal[0], side->plane->normal[1], side->plane->normal[2] };
				if (isDuplicatePlane(n, side->plane->dist)) continue;
				planes.push_back({ n, side->plane->dist });
			}

			const int P = (int)planes.size();
			const float insideEps = 0.1f;
			const float dedupeEps = 0.05f;

			// vertex set = points where 3 planes meet inside every half-space
			std::vector<hull::Vec3> verts;
			for (int i = 0; i < P; i++)
			{
				for (int j = i + 1; j < P; j++)
				{
					for (int k = j + 1; k < P; k++)
					{
						const hull::Vec3 njk = hull::cross(planes[j].n, planes[k].n);
						const float det = hull::dot(planes[i].n, njk);
						if (std::fabs(det) < 1e-6f) continue;

						const hull::Vec3 nki = hull::cross(planes[k].n, planes[i].n);
						const hull::Vec3 nij = hull::cross(planes[i].n, planes[j].n);
						const hull::Vec3 p =
							(njk * planes[i].d + nki * planes[j].d + nij * planes[k].d) * (1.0f / det);

						bool inside = true;
						for (int m = 0; m < P; m++)
						{
							if (hull::dot(planes[m].n, p) > planes[m].d + insideEps) { inside = false; break; }
						}
						if (!inside) continue;

						bool dup = false;
						for (const auto& v : verts)
						{
							if (hull::length(v - p) < dedupeEps) { dup = true; break; }
						}
						if (!dup) verts.push_back(p);
					}
				}
			}

			if (verts.size() < 4)
			{
				ZONETOOL_WARNING("PhysCollmap \"%s\": brush geom produced %zu vertices, skipping",
					asset->name, verts.size());
				return false;
			}

			// build a face loop for every plane that actually carries >= 3 vertices
			const float onPlaneEps = 0.1f;
			std::vector<std::vector<int>> faces;
			for (int pi = 0; pi < P; pi++)
			{
				std::vector<int> onFace;
				for (int vi = 0; vi < (int)verts.size(); vi++)
				{
					if (std::fabs(hull::dot(planes[pi].n, verts[vi]) - planes[pi].d) < onPlaneEps)
						onFace.push_back(vi);
				}
				if (onFace.size() < 3) continue;

				hull::Vec3 c{ 0.0f, 0.0f, 0.0f };
				for (int idx : onFace) c = c + verts[idx];
				c = c * (1.0f / (float)onFace.size());

				const hull::Vec3 n = planes[pi].n;
				const hull::Vec3 t = (std::fabs(n.z) < 0.9f) ? hull::Vec3{ 0.0f, 0.0f, 1.0f } : hull::Vec3{ 1.0f, 0.0f, 0.0f };
				const hull::Vec3 u = hull::normalize(hull::cross(t, n));
				const hull::Vec3 w = hull::cross(n, u);

				std::sort(onFace.begin(), onFace.end(), [&](int A, int B)
				{
					const hull::Vec3 da = verts[A] - c;
					const hull::Vec3 db = verts[B] - c;
					return std::atan2(hull::dot(da, w), hull::dot(da, u)) <
						std::atan2(hull::dot(db, w), hull::dot(db, u));
				});

				faces.push_back(onFace);
			}

			if (!hull::build(data, verts, faces, allocator))
			{
				ZONETOOL_WARNING("PhysCollmap \"%s\": failed to cook brush geom", asset->name);
				return false;
			}
			return true;
		}

		H1::PhysCollmap* GenerateH1Asset(PhysCollmap* asset, allocator& allocator)
		{
			ZONETOOL_INFO("Generating PhysCollmap %s", asset->name);

			auto* h1_asset = allocator.allocate<H1::PhysCollmap>();

			h1_asset->name = asset->name;
			
			memcpy(&h1_asset->mass, &asset->mass, sizeof(PhysMass));
			memcpy(&h1_asset->bounds, &asset->bounds, sizeof(Bounds));

			// Cook each source geom into a Domino polytope. The game asserts that
			// every geom's polytopeData is non-null and that the SUM of per-geom
			// m_volume is > FLT_EPSILON, so we never emit a null / zero-volume geom:
			// failures fall back to a box approximation and are only dropped when even
			// that is degenerate.
			std::vector<H1::dmPolytopeData*> polys;
			polys.reserve(asset->count);

			for (auto i = 0u; i < asset->count; i++)
			{
				auto* geom = &asset->geoms[i];

				auto* data = allocator.allocate<H1::dmPolytopeData>();
				H1::PhysGeomInfo tmp_geom{};
				tmp_geom.data = data;

				bool ok = false;
				switch (geom->type)
				{
				case PHYS_GEOM_BOX:
					GenerateBox(asset, geom, &tmp_geom, allocator);
					ok = data->m_volume > 1e-6f;
					break;
				case PHYS_GEOM_CYLINDER:
					ok = GenerateCylinder(asset, geom, &tmp_geom, allocator);
					break;
				case PHYS_GEOM_CAPSULE:
					ok = GenerateCapsule(asset, geom, &tmp_geom, allocator);
					break;
				case PHYS_GEOM_BRUSHMODEL:
				case PHYS_GEOM_BRUSH:
				case PHYS_GEOM_COLLMAP:
					ok = GenerateBrush(asset, geom, &tmp_geom, allocator);
					break;
				case PHYS_GEOM_NONE:
				default:
					ZONETOOL_WARNING("PhysCollmap \"%s\": geom %u has unhandled type %d, approximating as box",
						asset->name, i, geom->type);
					break;
				}

				if (!ok)
				{
					// box fallback (GetBounds is null-safe for brush geoms)
					*data = {};
					GenerateBox(asset, geom, &tmp_geom, allocator);
					ok = data->m_volume > 1e-6f;
					if (ok)
					{
						ZONETOOL_WARNING("PhysCollmap \"%s\": geom %u fell back to box approximation",
							asset->name, i);
					}
				}

				if (ok)
				{
					polys.push_back(data);
					ZONETOOL_INFO("PhysCollmap geom %u (type %d) for %s generated (%d verts, %d faces)",
						i, geom->type, asset->name, data->m_vertexCount, data->m_faceCount);
				}
				else
				{
					ZONETOOL_WARNING("PhysCollmap \"%s\": geom %u produced no valid polytope, dropping",
						asset->name, i);
				}
			}

			if (polys.empty())
			{
				ZONETOOL_ERROR("PhysCollmap \"%s\": no valid geometry could be generated!", asset->name);
			}

			h1_asset->count = static_cast<unsigned int>(polys.size());
			h1_asset->geoms = allocator.allocate<H1::PhysGeomInfo>(polys.empty() ? 1 : polys.size());
			for (auto i = 0u; i < h1_asset->count; i++)
			{
				h1_asset->geoms[i].data = polys[i];
			}

			// h1_asset->mass keeps the authored IW5 PhysMass (centre of mass / inertia);
			// the runtime recomputes body mass from the polytope volumes on load.

			return h1_asset;
		}

		H1::PhysCollmap* convert(PhysCollmap* asset, allocator& allocator)
		{
			// generate h1 asset
			return GenerateH1Asset(asset, allocator);
		}
	}
}