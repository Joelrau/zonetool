#pragma once

// Interchange format between the converter (which runs as a 32-bit DLL inside the
// source game) and umbra-tomegen.exe, the 64-bit process that drives the Umbra 3
// optimizer. Everything is world space: objects carry no transform, a model is
// simply a triangle soup already placed in the map.
//
// The file is the header followed by, in order, model_count models, object_count
// objects, view_volume_count view volumes and seed_point_count seed points, each
// laid out as documented on its struct. Nothing is aligned or padded beyond the
// natural 4-byte layout of the structs.

#include <cstdint>

namespace ZoneTool::Umbra
{
	constexpr char SCENE_MAGIC[8] = { 'Z', 'T', 'U', 'M', 'B', 'R', 'A', '1' };
	constexpr std::uint32_t SCENE_VERSION = 1;

	// Umbra::SceneObject::Flags
	enum scene_object_flags : std::uint32_t
	{
		SCENE_OBJECT_OCCLUDER = 1u << 0,
		SCENE_OBJECT_TARGET = 1u << 1,
		SCENE_OBJECT_GATE = 1u << 2,
		SCENE_OBJECT_VOLUME = 1u << 3,
	};

	// Umbra::ComputationParams. Distances are in world units.
	struct scene_params
	{
		float smallest_occluder;
		float smallest_hole;
		float backface_limit; // percent, 100 disables the test
		float cluster_size; // 0 = Umbra default
		float object_group_cost; // <= 0 disables object grouping
		float minimum_accurate_distance; // <= 0 = Umbra default
		std::uint32_t output_flags; // Umbra::ComputationParams::DataFlags
		std::uint32_t thread_count; // 0 = hardware concurrency
	};

	struct scene_header
	{
		char magic[8];
		std::uint32_t version;
		std::uint32_t header_size; // sizeof(scene_header)
		scene_params params;
		std::uint32_t model_count;
		std::uint32_t object_count;
		std::uint32_t view_volume_count;
		std::uint32_t seed_point_count;
	};

	// followed by float[vertex_count][3] then uint32_t[triangle_count][3]
	struct scene_model
	{
		std::uint32_t vertex_count;
		std::uint32_t triangle_count;
	};

	struct scene_object
	{
		std::uint32_t model; // index into the model list
		std::uint32_t user_id; // what the runtime hands back for this object
		std::uint32_t flags; // scene_object_flags
	};

	struct scene_view_volume
	{
		float mins[3];
		float maxs[3];
		std::uint32_t id;
	};

	struct scene_seed_point
	{
		float position[3];
	};

	static_assert(sizeof(scene_header) == 8 + 4 + 4 + 32 + 16);
	static_assert(sizeof(scene_model) == 8);
	static_assert(sizeof(scene_object) == 12);
	static_assert(sizeof(scene_view_volume) == 28);
	static_assert(sizeof(scene_seed_point) == 12);
}
