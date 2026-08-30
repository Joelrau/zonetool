#pragma once

// Builder for IW7's GPU tetrahedral light grid (GfxLightGridProbeData + GfxVoxelTree),
// reversed from the shipped maps in D:\Games\PC\IW7\dump (mp_paris, mp_afghan, mp_breakneck,
// cp_zmb) plus mp_dome_dusk / mp_frontend. IW7 has no other working static-model lighting
// path - the octree/palette light grid it inherited from the IW6/H1 lineage is vestigial
// (every authentic map ships a 3-entry stub palette and a 2-node stub tree), so a converted
// map with an empty probe volume renders every XModel black.
//
// Format notes that drove this implementation:
//
// - The shipped tetrahedralisation is NOT a Delaunay. Every tetrahedron is positively
//   oriented and the dominant volume is exactly 64^3/6, i.e. axis-aligned cubes split into
//   six tetrahedra. Cell size is adaptive in shipped data (32/64/128/256); we emit a single
//   uniform level, which removes the level-transition case entirely.
//
// - GfxGpuLightGridTetrahedron::indexFlags[i]: low 16 bits are the probe index, so a volume
//   can address at most 65536 probes - that is what bounds the grid resolution below. Bit 31
//   is a per-corner marker that tracks cell refinement level (all-clear and all-set dominate
//   the shipped data, mixed patterns only appear where cell sizes change). A uniform grid has
//   no transitions, so we emit one constant pattern.
//
// - GfxGpuLightGridTetrahedronNeighbors::neighbors[i] is the tetrahedron opposite vertex i,
//   with 0xFFFFFFFF meaning "no neighbour".
//
// - voxelStartTetrahedron is indexed by *voxel tree leaf*, not by a dense grid
//   (voxelLeafNodeCount == voxelStartTetrahedronCount in every shipped map), with
//   0xFFFFFFFF for an empty voxel. The leaf index comes out of this descent, validated
//   against all five authentic maps:
//
//       cx,cy,cz = (p - boundMin) >> shift[0]
//       col      = cy * rootNodeDimension[0] + cx
//       node     = topDownViewNode[col].firstNodeIndex + (cz - zMin)
//       for level in (1, 2):
//           child = ((dz >> shift[level]) & 3) << 4
//                 | ((dy >> shift[level]) & 3) << 2
//                 |  (dx >> shift[level]) & 3
//           index = (firstNodeIndex & 0x7FFFFFFF)
//                 + popcount(childNodeMask & ((1 << child) - 1))
//           if firstNodeIndex < 0: that index is a leaf
//
//   So the arrays must be emitted in exactly that nesting order, which is what build() does.
//
// Standalone: no zonetool dependencies.

#include <cstdint>
#include <functional>
#include <vector>

namespace lightgrid_probes
{
	// low 16 bits of indexFlags hold the probe index
	constexpr unsigned int max_probes = 0xFFFF;

	struct build_params
	{
		float bounds_min[3];
		float bounds_max[3];

		// log2 of the leaf cell size in world units. 0 asks build() to pick the finest
		// value that keeps the probe count inside max_probes.
		int leaf_shift = 0;

		// bit 31 of every indexFlags entry. A uniform grid never straddles a refinement
		// level, so shipped data gives no reason to prefer one - 0 matches the more common
		// all-clear pattern.
		bool corner_flag = false;
	};

	struct voxel_tree_node
	{
		int first_node_index[2];
		unsigned int child_node_mask[2];
	};

	struct top_down_view_node
	{
		int first_node_index;
		int z_min;
		int z_max;
	};

	struct probe_volume
	{
		// ---- GfxLightGridProbeData ----
		std::vector<unsigned short> probes;          // 32 float16 per probe (GfxProbeData)
		std::vector<float> probe_positions;          // xyz per probe
		std::vector<unsigned int> tetrahedrons;      // 4 per tetrahedron (indexFlags)
		std::vector<unsigned int> tetrahedron_neighbors; // 4 per tetrahedron
		std::vector<unsigned int> voxel_start_tetrahedron; // one per leaf

		unsigned int probe_count = 0;
		unsigned int tetrahedron_count = 0;

		// ---- GfxGpuLightGridZone ----
		unsigned int zone_num_probes = 0;
		unsigned int zone_first_probe = 0;
		unsigned int zone_num_tetrahedrons = 0;
		unsigned int zone_first_tetrahedron = 0;
		unsigned int zone_first_voxel_tetrahedron_index = 0;
		unsigned int zone_num_voxel_tetrahedron_indices = 0;
		unsigned short zone_fallback_coeffs[29]{};

		// ---- GfxVoxelTree ----
		int root_node_dimension[4]{};
		int node_coord_bit_shift[4]{};
		float bound_min[4]{};
		float bound_max[4]{};
		std::vector<top_down_view_node> top_down_view_nodes;
		std::vector<voxel_tree_node> internal_nodes;
		std::vector<unsigned short> leaf_nodes;      // lightListAddress per leaf
		std::vector<unsigned short> light_list;      // shared, we emit a single empty list

		unsigned int leaf_count = 0;

		bool valid = false;
	};

	// L2 spherical harmonics at a world position: 27 floats, three blocks of nine
	// (all R, then all G, then all B). Called once per probe.
	using sampler = std::function<void(const float position[3], float out_sh[27])>;

	// IW7 stores the standard orthonormal real SH basis in the standard order:
	//   Y00, Y1-1(y), Y10(z), Y11(x), Y2-2(xy), Y2-1(yz), Y20(3z^2-1), Y21(xz), Y22(x^2-y^2)
	// Established from shipped probes: reconstructing radiance under this basis makes
	// mp_dome_dusk 2.53x brighter looking up at the sky than down at the ground, while the
	// IW6/H1 ordering gives 1.04 (no vertical variation, physically implausible). It also
	// matches the coefficient magnitudes - c2, the Y10 "up" term, is the largest of the three
	// linear coefficients in shipped data.
	//
	// Least-squares fit of that basis to directional samples, which handles the 56 light grid
	// bin directions not being uniformly distributed over the sphere.
	void project_sh(const float samples[][3], const float directions[][3], unsigned int count,
		float scale, float out_sh[27]);

	// constant radiance: DC term only
	void constant_sh(const float rgb[3], float scale, float out_sh[27]);

	// packs 27 coefficients plus the trailing 1.0 every shipped probe carries
	void encode_probe_sh(const float sh[27], unsigned short out_coeffs[32]);

	probe_volume build(const build_params& params, const sampler& sample, float sh_scale);
}
