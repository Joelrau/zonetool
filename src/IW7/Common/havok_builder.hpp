#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace ZoneTool::IW7
{
	namespace havok
	{
		// Builds an IW7 world-collision blob (clipMap_t::havokWorldShapeData) from raw
		// triangles. See docs/iw7-havok-collision.md for where every constant below came
		// from; everything here is derived from IW7's own reflection tables and verified
		// against the shipped mp_paris / mp_afghan / mp_breakneck blobs.
		//
		// Geometry goes in in Havok space: IW7 stores world collision at CoD units / 32,
		// just like per-model PhysicsAssets. Winding must be CCW as
		// seen from the front face or surface normals invert.
		namespace builder
		{
			struct triangle
			{
				float verts[3][3];
				// A quad is stored as the two triangles (v0,v1,v2) and (v0,v2,v3) sharing the
				// 0-2 diagonal, which is how Havok fans a 4-index primitive. Stock world meshes
				// are ~96% quads (afghan: 90,924 of 105,752 primitives), so emitting a face as
				// one quad rather than two triangles is what the shipped data looks like.
				bool is_quad = false;
				float vert3[3] = {};
				unsigned short surface_tag; // -> primitiveDataRuns, per-surface material id
				int contents = 1;           // CoD contents mask -> collisionFilterInfo
				// ShapeTagData::materialCRC. Drives footsteps, impact effects and
				// penetration; 0x1AB7BC33 is the value the shipped dummies use.
				std::uint32_t material_crc = 0x1AB7BC33u;
				// ShapeTagData::userData: IW7 surface flags in the low 32 bits and the brush
				// basis at bit 48 (see TAG_USERDATA_BRUSH_BASIS in the implementation).
				std::uint64_t user_data = 0;
			};

			struct mesh_input
			{
				std::vector<triangle> triangles;
				float convex_radius = 0.0f; // shipped stock world blobs use 0.0
			};

			// Produces a complete Havok 2014.2.5-r1 binary packfile. The returned buffer is
			// what havokWorldShapeData points at, and its size is havokWorldShapeDataSize.
			// Returns an empty vector (and logs) if the input is unusable.
			std::vector<std::uint8_t> build_world_shape(const mesh_input& input);

			// MapEnts::havokEntsShapeData -- the second shape list, holding one hknpShape per
			// brush model and per trigger, indexed by cmodel_t::physicsShapeOverrideIdx and
			// TriggerModel::physicsShapeOverrideIdx. See docs/iw7-ents-shapes.md.
			//
			// Passing no shapes builds the empty form, which is not merely a placeholder:
			// CM_ContentsOfBrushModel dereferences this list unconditionally for any
			// "model" "*N" entity, so a null blob is a null read, while an empty list makes
			// it take the `idx >= shapeContents.size` branch and return a permissive default.
			// Shipped converted maps (mp_bog, mp_frontend) carry exactly that.

			// One face of a convex polytope: its outward plane, and a counter-clockwise run
			// of vertex indices around it.
			struct polytope_face
			{
				float plane[4]; // normal xyz, dist -- Havok stores dot(n,p) + d = 0
				std::vector<std::uint8_t> indices;
			};

			// hknpConvexPolytopeShape. Vertex indices are uint8, so at most 255 vertices.
			struct polytope
			{
				std::vector<std::array<float, 3>> verts;
				std::vector<polytope_face> faces; // exactly one per plane
			};

			// One entry of the shape list: an hknpDynamicCompoundShape wrapping one polytope
			// per brush. `contents` lands in shapeContents, which is what
			// CM_ContentsOfBrushModel hands back as the entity's contents mask.
			struct ents_shape
			{
				std::vector<polytope> convexes;
				// The per-surface mask its shapeTagData entry carries, and what the runtime
				// collides the shape as.
				int contents = 1;
				// What CM_ContentsOfBrushModel hands back as the entity's contents mask.
				// Stock separates the two: a trigger stores 0xC7FFBFFF here while a brush
				// model stores its real mask (breakneck: 63 triggers at 0xC7FFBFFF, 20 shapes
				// at 0x30200, 16 at 0x30000).
				unsigned int entity_contents = 1;
				// ShapeTagData::materialCRC, same meaning and same table as triangle::material_crc
				// on the world path. Stock ents blobs carry the full spread of world materials
				// here (mp_afghan 12 distinct over 136 tags), so leaving it at the default
				// makes every brush model and trigger sound and shatter like concrete.
				std::uint32_t material_crc = 0x1AB7BC33u;
				// ShapeTagData::userData -- surface flags plus the brush basis at bit 48.
				std::uint64_t user_data = 0;
				std::string name; // shapeNames; "<map>:entity N" in stock data
			};

			struct ents_input
			{
				std::vector<ents_shape> shapes;

				// Shapes are stored at CoD units / 32 -- the opposite of the world blob,
				// which is 1:1. Verified at exactly 32.0000 over 204 stock samples; see
				// docs/iw7-ents-shapes.md. Geometry goes in as CoD units and is scaled here.
				float scale = 1.0f / 32.0f;
			};

			std::vector<std::uint8_t> build_ents_shape_list(const ents_input& input);

			// The .hkx beside a `physicsasset` dump. Only the dummy form is generated: a
			// static body wrapping a placeholder box, which is what IW7 attaches to script
			// brush models and triggers. `physicsShapeOverrideIdx` replaces the shape, so the
			// box is never collided against -- but the asset has to exist or the runtime
			// builds no body at all. Shipped dummies are byte-identical across maps and the
			// four variants differ only in these two fields.
			struct physics_asset_input
			{
				std::string body_name = "scriptbrushmodeldummy";
				std::uint32_t body_quality_crc = 0x7923E35Cu;
				std::uint32_t material_crc = 0x1AB7BC33u;
				// hknpBodyCinfo::collisionFilterInfo, the CoD contents mask the body collides
				// as. 1 (solid) is what the shipped brush-model and trigger dummies carry.
				std::uint32_t body_contents = 1;
			};

			std::vector<std::uint8_t> build_physics_asset(const physics_asset_input& input);

			// A model's own collision: the same compressed mesh the world blob uses, wrapped in
			// a HavokPhysicsAsset with a single static body. This is what IW7 attaches to an
			// XModel via XModel::physAsset, and without it a static model has no collision at
			// all -- 391 of mp_paris's 671 xmodels ship one. Geometry is CoD units / 32.
			std::vector<std::uint8_t> build_model_physics_asset(const mesh_input& input,
				const physics_asset_input& physics_asset);
		}
	}
}
