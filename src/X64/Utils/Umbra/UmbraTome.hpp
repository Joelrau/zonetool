#pragma once

// Generates IW7-compatible Umbra 3 occlusion tomes from world geometry.
//
// The heavy lifting is done by the real Umbra 3.3.13 optimizer, which lives in
// umbra-tomegen.exe (src/UmbraTomeGen) because it needs an x64 process and the
// converter runs as a 32-bit DLL inside the source game. This module builds the
// scene, hands it over, reads the tome back and reports what came out.
//
// Format notes, engine addresses and the reasons behind the parameter defaults
// are in RESEARCH.md next to this file.

#include "UmbraScene.hpp"

#include <array>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace ZoneTool::Umbra
{
	// IW7 user ID layout (0x1405FAA30): type in bits 28..30, index in bits 0..23.
	// Surfaces index dpvs.sortedSurfIndex, static models dpvs.lodData (1-based).
	enum user_id_type : std::uint32_t
	{
		USER_ID_SURFACE = 0x00000000,
		USER_ID_SMODEL = 0x10000000,
		USER_ID_VOLUMETRIC = 0x30000000,
		USER_ID_PRIMARY_LIGHT = 0x40000000,
		USER_ID_REFLECTION_PROBE = 0x50000000,
		USER_ID_DECAL = 0x60000000,
		USER_ID_INDEX_MASK = 0x00FFFFFF,
	};

	// 0x1405FAA30 reads an object's user IDs into a 4096 entry buffer and drops
	// the rest, so no object may carry more than that.
	constexpr std::uint32_t MAX_USER_IDS_PER_OBJECT = 4096;

	// world space triangle soup
	struct tome_model
	{
		std::vector<float> vertices; // xyz triples
		std::vector<std::uint32_t> indices; // triangles
	};

	struct tome_object
	{
		std::uint32_t model; // index into tome_input::models
		std::uint32_t user_id;
		std::uint32_t flags; // scene_object_flags
	};

	struct tome_input
	{
		std::string name; // used for temp file names and messages
		std::vector<tome_model> models;
		std::vector<tome_object> objects;
		std::vector<scene_view_volume> view_volumes;
		std::vector<std::array<float, 3>> seed_points;
		scene_params params = default_params();

		// where the scene, tome and logs are written; empty = %TEMP%\zonetool-umbra
		std::string work_directory;
		// run the generator's own runtime query pass over the result
		bool verify = true;
		// keep the scene/tome/log files after a successful run
		bool keep_files = true;

		static scene_params default_params();

		// appends a closed axis aligned box model and returns its index
		std::uint32_t add_box_model(const float* mins, const float* maxs);
	};

	struct tome_stats
	{
		std::uint32_t version = 0;
		std::uint32_t size = 0;
		std::uint32_t crc32 = 0;
		float lod_base_distance = 0.0f;
		float tree_min[3]{};
		float tree_max[3]{};
		std::int32_t object_count = 0;
		std::uint32_t user_id_count = 0;
		std::int32_t tile_count = 0;
		std::int32_t leaf_tile_count = 0;
		std::int32_t cluster_count = 0;
		std::int32_t gate_count = 0;
		std::uint32_t cell_count = 0;
		std::uint32_t portal_count = 0;
		std::uint32_t cells_without_portals = 0;
		std::uint32_t object_list_entries = 0;
	};

	struct tome_result
	{
		std::vector<unsigned char> data;
		tome_stats stats;
		double generation_seconds = 0.0;
		std::string error; // empty on success
	};

	using log_fn = std::function<void(const std::string&)>;

	// Full path of umbra-tomegen.exe: ZT_UMBRA_TOMEGEN if set, otherwise next to
	// the module this code is linked into. Empty if neither exists.
	std::string find_generator();

	// Runs the optimizer. On failure result.error explains why and result.data is
	// empty; the generator's output is forwarded line by line to `log` either way.
	bool generate_tome(const tome_input& input, tome_result& result, const log_fn& log);

	// Parses the header (versions 0x12..0x14) and walks the tiles. Returns false
	// if the buffer is not a tome.
	bool inspect_tome(const unsigned char* data, std::size_t size, tome_stats& stats);

	// Every user ID the tome can report, in object order. The optimizer silently
	// drops targets that end up in no cell, so callers must diff this against
	// what they put in.
	std::vector<std::uint32_t> read_user_ids(const unsigned char* data, std::size_t size);

	// Load_UmbraTome -> Umbra::Tome::init (0x140E92DB0) acceptance rules: magic
	// 0xD600 with version 0x12..0x14 and m_size <= buffer size. Alignment is the
	// linker's job.
	bool is_tome_accepted_by_iw7(const unsigned char* data, std::size_t size, std::string& reason);
}
