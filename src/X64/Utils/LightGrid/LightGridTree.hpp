#pragma once

// Builder + bit-exact reader for the IW6+/H1 compressed light grid tree
// (GfxLightGridTree: p_nodeTable / p_leafTable), reversed from
// h1 (PS4 2-h1_mp.elf, full symbols) and iw6_ds_xenon:
//   R_GetLightGrid / R_GetLightingStateLightGrid  (octree walk)
//   R_DecodeLightGridBlock                        (leaf chunk decode)
//   R_LoadLightGridNodeBitStream                  (chunk header)
//   R_LoadLightGridBlockBitStream                 (leaf block header)
//   R_LoadLightGridBlockIndexBitStream            (command list)
//   R_ExecLeafIndexEncodedCommand / R_Decode*Raw  (final voxel values)
//
// Format summary:
// - tree coords: tx=(x-minX)>>2, ty=(y-minY)>>2, tz=z-minZ (grid space u16,
//   x/y cells are 32 world units, z cells 64; same space as the legacy
//   rowDataStart lookup).
// - node = u32: (childMask << 24) | low24. Walk from node 0 testing coord
//   bit (1 << maxDepth) down to bit 0; octant = xBit | yBit<<1 | zBit<<2.
//   Internal node low24 = index of first child (children packed in
//   ascending octant order); node at the last level: low24 = byte offset
//   of its leaf chunk in p_leafTable.
// - leaf chunk = header (format byte, per-leaf byte sizes - 7 bits each on
//   H1, 6 bits on IW6 - and color/light index range headers) followed by
//   the per-octant leaf blocks, everything byte aligned.
// - leaf block = 16 voxels covering 4x4x1 grid cells;
//   voxelIndex = (x&1) | ((y&1)<<1) | ((x&2)<<1) | ((y&2)<<2).
//   Each voxel: colorsIndex (u32, 0 = empty), lightIndex (u16) and two
//   trace bits (bit0 = lower-z half, bit1 = upper-z half; the old per-entry
//   8-bit needsTrace corner mask maps to bit0 = (mask & 0x55) != 0,
//   bit1 = (mask & 0xAA) != 0).
//
// The encoder only emits the simple stream variants (explicit headers,
// raw type-0 command lists, raw/all-ones voxel masks), which every decoder
// variant accepts. Standalone: no zonetool dependencies.

#include <cstdint>
#include <vector>

namespace lightgrid_tree
{
	enum class leaf_size_bits : int
	{
		iw6 = 6, // leaf block sizes stored as 6 bits (max 63 bytes)
		h1 = 7,  // leaf block sizes stored as 7 bits (max 127 bytes)
	};

	// one populated grid position (absolute grid-space coordinates, the same
	// space as GfxLightGrid::mins/maxs and the legacy row data)
	struct grid_sample
	{
		unsigned short pos[3];
		unsigned int color_index;   // colorsIndex, 0 = empty (do not use 0 for real data)
		unsigned short light_index; // primaryLightEnvIndex
		bool trace_lo;              // old needsTrace & 0x55
		bool trace_hi;              // old needsTrace & 0xAA
	};

	// result of a lookup (mirrors GfxLightGridRaw)
	struct raw_result
	{
		unsigned int color_index;
		unsigned short light_index;
		bool trace_lo;
		bool trace_hi;
	};

	// built tree, maps 1:1 onto GfxLightGridTree
	struct tree_data
	{
		unsigned char max_depth;
		int node_count;
		int leaf_count;
		int coord_min_grid_space[3];
		int coord_max_grid_space[3];
		int coord_half_size_grid_space[3];
		int default_color_index_bit_count;
		int default_light_index_bit_count;
		std::vector<unsigned int> node_table;
		std::vector<unsigned char> leaf_table;
	};

	// raw view of an existing (game) tree, for decoding real assets
	struct tree_view
	{
		unsigned char max_depth;
		int node_count;
		const int* coord_min_grid_space; // int[3]
		const int* coord_max_grid_space; // int[3]
		int default_color_index_bit_count;
		int default_light_index_bit_count;
		const unsigned int* node_table;
		int leaf_table_size;
		const unsigned char* leaf_table;
		leaf_size_bits size_bits;
	};

	// ---- encoder --------------------------------------------------------

	// build an H1-format tree from populated grid positions.
	// duplicate positions keep the last sample.
	tree_data build_tree(const grid_sample* samples, size_t sample_count,
		leaf_size_bits size_bits = leaf_size_bits::h1);

	// ---- decoder (game-exact mirror, for verification) -------------------

	// returns false when the position holds no data (colorsIndex == 0 /
	// outside bounds), like R_GetLightGrid. *unsupported (optional) is set
	// when the stream overflows (malformed data).
	bool lookup(const tree_view& tree, const unsigned short pos[3], raw_result& out,
		bool* unsupported = nullptr);
	bool lookup(const tree_data& tree, const unsigned short pos[3], raw_result& out);

	// ---- legacy row data ---------------------------------------------------

	// one entry reference enumerated from the legacy row data
	struct grid_entry_ref
	{
		unsigned short pos[3];
		unsigned int entry_index; // into GfxLightGrid::entries
	};

	// walk the old (IW4/IW5/IW6) rowDataStart/rawRowData layout and produce
	// every populated grid position with its entry index
	// (exact mirror of the addressing in R_GetLightGridSampleEntryQuad).
	std::vector<grid_entry_ref> enumerate_row_data(
		const unsigned short mins[3], const unsigned short maxs[3],
		unsigned int row_axis, unsigned int col_axis,
		const unsigned short* row_data_start, const unsigned char* raw_row_data);
}
