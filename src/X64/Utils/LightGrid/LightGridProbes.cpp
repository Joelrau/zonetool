#include <stdafx.hpp>
#include "LightGridProbes.hpp"

#include <algorithm>
#include <array>
#include <utility>
#include <cmath>
#include <cstring>
#include <map>

namespace lightgrid_probes
{
	namespace
	{
		constexpr unsigned int no_index = 0xFFFFFFFF;

		// IEEE-754 half, same conversion the rest of the light grid code uses
		unsigned short to_half(const float x)
		{
			unsigned int bits;
			std::memcpy(&bits, &x, sizeof(bits));

			const auto sign = static_cast<unsigned short>((bits >> 16) & 0x8000);
			auto exponent = static_cast<int>((bits >> 23) & 0xFF) - 127 + 15;
			auto mantissa = bits & 0x7FFFFF;

			if (exponent >= 31) // overflow / inf / nan -> largest finite
			{
				return static_cast<unsigned short>(sign | 0x7BFF);
			}
			if (exponent <= 0) // underflow -> zero (denormals are not worth it here)
			{
				return sign;
			}
			return static_cast<unsigned short>(sign | (exponent << 10) | (mantissa >> 13));
		}

		// The six-tetrahedron (Freudenthal) split of a cube. Corner bits: 1 = +x, 2 = +y,
		// 4 = +z. Every tetrahedron shares the 0-7 main diagonal, and the winding below is
		// chosen so all six come out positively oriented, matching the shipped data.
		constexpr int cube_tets[6][4] = {
			{ 0, 1, 3, 7 },
			{ 0, 3, 2, 7 },
			{ 0, 2, 6, 7 },
			{ 0, 6, 4, 7 },
			{ 0, 4, 5, 7 },
			{ 0, 5, 1, 7 },
		};

		struct face_key
		{
			unsigned int v[3];
			bool operator<(const face_key& o) const
			{
				if (v[0] != o.v[0]) return v[0] < o.v[0];
				if (v[1] != o.v[1]) return v[1] < o.v[1];
				return v[2] < o.v[2];
			}
		};

		face_key make_face(unsigned int a, unsigned int b, unsigned int c)
		{
			unsigned int t[3] = { a, b, c };
			std::sort(t, t + 3);
			return face_key{ { t[0], t[1], t[2] } };
		}
	}

	namespace
	{
		// orthonormal real SH, standard order
		void sh_basis(const float d[3], float out[9])
		{
			const float x = d[0], y = d[1], z = d[2];
			out[0] = 0.2820948f;
			out[1] = 0.4886025f * y;
			out[2] = 0.4886025f * z;
			out[3] = 0.4886025f * x;
			out[4] = 1.0925484f * x * y;
			out[5] = 1.0925484f * y * z;
			out[6] = 0.3153916f * ((3.0f * z * z) - 1.0f);
			out[7] = 1.0925484f * x * z;
			out[8] = 0.5462742f * ((x * x) - (y * y));
		}
	}

	void constant_sh(const float rgb[3], const float scale, float out_sh[27])
	{
		std::memset(out_sh, 0, sizeof(float) * 27);
		// a constant radiance L projects onto Y00 alone, with weight L * 4pi * Y00 = L * 3.5449
		for (int ch = 0; ch < 3; ch++)
		{
			out_sh[ch * 9] = rgb[ch] * scale * 3.5449077f;
		}
	}

	void project_sh(const float samples[][3], const float directions[][3], const unsigned int count,
		const float scale, float out_sh[27])
	{
		std::memset(out_sh, 0, sizeof(float) * 27);
		if (!count)
		{
			return;
		}

		// normal equations: (A^T A) c = A^T L, with A the basis evaluated at each direction.
		double ata[9][9] = {};
		double atl[9][3] = {};
		for (unsigned int i = 0; i < count; i++)
		{
			float basis[9];
			sh_basis(directions[i], basis);
			for (int r = 0; r < 9; r++)
			{
				for (int c = 0; c < 9; c++)
				{
					ata[r][c] += static_cast<double>(basis[r]) * basis[c];
				}
				for (int ch = 0; ch < 3; ch++)
				{
					atl[r][ch] += static_cast<double>(basis[r]) * samples[i][ch];
				}
			}
		}

		// Gauss-Jordan with partial pivoting; 9x9, so clarity beats cleverness
		double m[9][12];
		for (int r = 0; r < 9; r++)
		{
			for (int c = 0; c < 9; c++) m[r][c] = ata[r][c];
			for (int ch = 0; ch < 3; ch++) m[r][9 + ch] = atl[r][ch];
		}
		for (int col = 0; col < 9; col++)
		{
			int pivot = col;
			for (int r = col + 1; r < 9; r++)
			{
				if (std::fabs(m[r][col]) > std::fabs(m[pivot][col])) pivot = r;
			}
			if (std::fabs(m[pivot][col]) < 1e-12)
			{
				return; // singular, leave the fit at zero
			}
			if (pivot != col)
			{
				for (int c = 0; c < 12; c++) std::swap(m[col][c], m[pivot][c]);
			}
			const auto inv = 1.0 / m[col][col];
			for (int c = col; c < 12; c++) m[col][c] *= inv;
			for (int r = 0; r < 9; r++)
			{
				if (r == col) continue;
				const auto f = m[r][col];
				if (f == 0.0) continue;
				for (int c = col; c < 12; c++) m[r][c] -= f * m[col][c];
			}
		}

		for (int ch = 0; ch < 3; ch++)
		{
			for (int k = 0; k < 9; k++)
			{
				out_sh[ch * 9 + k] = static_cast<float>(m[k][9 + ch] * scale);
			}
		}
	}

	void encode_probe_sh(const float sh[27], unsigned short out_coeffs[32])
	{
		std::memset(out_coeffs, 0, sizeof(unsigned short) * 32);
		for (int k = 0; k < 27; k++)
		{
			out_coeffs[k] = to_half(sh[k]);
		}
		out_coeffs[27] = to_half(1.0f);
	}

	probe_volume build(const build_params& params, const sampler& sample, const float sh_scale)
	{
		probe_volume out{};

		float size[3];
		for (int i = 0; i < 3; i++)
		{
			size[i] = params.bounds_max[i] - params.bounds_min[i];
			if (size[i] <= 0.0f)
			{
				return out;
			}
		}

		// A root cell is 16 leaf cells across (two 4x4x4 subdivisions), and the probe grid
		// has one more corner than cells per axis. Pick the finest leaf size that still fits
		// inside the 16-bit probe index.
		int leaf_shift = params.leaf_shift;
		if (leaf_shift <= 0)
		{
			for (leaf_shift = 5; leaf_shift <= 12; leaf_shift++)
			{
				const auto root = static_cast<float>(1 << (leaf_shift + 4));
				const std::uint64_t rx = static_cast<std::uint64_t>(std::ceil(size[0] / root));
				const std::uint64_t ry = static_cast<std::uint64_t>(std::ceil(size[1] / root));
				const std::uint64_t rz = static_cast<std::uint64_t>(std::ceil(size[2] / root));
				const auto probes = (rx * 16 + 1) * (ry * 16 + 1) * (rz * 16 + 1);
				if (probes <= max_probes)
				{
					break;
				}
			}
			if (leaf_shift > 12)
			{
				return out;
			}
		}

		const int shift[3] = { leaf_shift + 4, leaf_shift + 2, leaf_shift };
		const auto leaf_size = static_cast<float>(1 << leaf_shift);
		const auto root_size = static_cast<float>(1 << shift[0]);

		// snap the origin down to a root-cell boundary so the shift arithmetic is exact
		float origin[3];
		int root_dim[3];
		for (int i = 0; i < 3; i++)
		{
			origin[i] = std::floor(params.bounds_min[i] / root_size) * root_size;
			root_dim[i] = static_cast<int>(std::ceil((params.bounds_max[i] - origin[i]) / root_size));
			if (root_dim[i] < 1) root_dim[i] = 1;
		}

		const int cells[3] = { root_dim[0] * 16, root_dim[1] * 16, root_dim[2] * 16 };
		const std::uint64_t probe_total =
			static_cast<std::uint64_t>(cells[0] + 1) * (cells[1] + 1) * (cells[2] + 1);
		if (probe_total > max_probes)
		{
			return out;
		}

		// ---- probes, one per cell corner -------------------------------------------------
		const auto probe_index = [&](const int x, const int y, const int z)
		{
			return static_cast<unsigned int>((z * (cells[1] + 1) + y) * (cells[0] + 1) + x);
		};

		out.probe_count = static_cast<unsigned int>(probe_total);
		out.probes.resize(probe_total * 32);
		out.probe_positions.resize(probe_total * 3);

		double accum[3] = { 0.0, 0.0, 0.0 };
		for (int z = 0; z <= cells[2]; z++)
		{
			for (int y = 0; y <= cells[1]; y++)
			{
				for (int x = 0; x <= cells[0]; x++)
				{
					const auto i = probe_index(x, y, z);
					float p[3] = {
						origin[0] + static_cast<float>(x) * leaf_size,
						origin[1] + static_cast<float>(y) * leaf_size,
						origin[2] + static_cast<float>(z) * leaf_size,
					};
					std::memcpy(&out.probe_positions[i * 3], p, sizeof(p));

					float sh[27] = {};
					if (sample) sample(p, sh);
					encode_probe_sh(sh, &out.probes[i * 32]);
					for (int c = 0; c < 3; c++) accum[c] += sh[c * 9];
				}
			}
		}

		// the zone fallback gets the volume average, so anything that misses the walk still
		// lands on something sane rather than black
		// the fallback keeps the average DC only - it stands in for "no tetrahedron resolved",
		// where a directional term would be meaningless
		float fallback_sh[27] = {};
		for (int c = 0; c < 3; c++)
		{
			fallback_sh[c * 9] = static_cast<float>(accum[c] / static_cast<double>(probe_total));
		}
		unsigned short fallback[32];
		encode_probe_sh(fallback_sh, fallback);
		std::memcpy(out.zone_fallback_coeffs, fallback, sizeof(out.zone_fallback_coeffs));

		// ---- voxel tree + tetrahedra, emitted in traversal order --------------------------
		//
		// A cell is present when it overlaps the requested bounds; the padding cells the root
		// grid adds around them are left out of the child masks, which is what keeps the leaf
		// and tetrahedron counts down.
		const auto cell_present = [&](const int cx, const int cy, const int cz)
		{
			const float lo[3] = {
				origin[0] + static_cast<float>(cx) * leaf_size,
				origin[1] + static_cast<float>(cy) * leaf_size,
				origin[2] + static_cast<float>(cz) * leaf_size,
			};
			for (int i = 0; i < 3; i++)
			{
				if (lo[i] + leaf_size <= params.bounds_min[i]) return false;
				if (lo[i] >= params.bounds_max[i]) return false;
			}
			return true;
		};

		// level 1 nodes are the 4x4x4 children of a root cell, level 2 nodes the children of
		// those; leaves are the children of level 2. Walk the same nesting the shader does.
		struct pending_leaf { int cx, cy, cz; };
		std::vector<pending_leaf> leaves;

		// first pass: which level-2 and level-1 nodes have any present descendant
		const auto l2_has_content = [&](const int bx, const int by, const int bz)
		{
			for (int i = 0; i < 64; i++)
			{
				const int lx = bx + (i & 3), ly = by + ((i >> 2) & 3), lz = bz + ((i >> 4) & 3);
				if (cell_present(lx, ly, lz)) return true;
			}
			return false;
		};
		const auto l1_has_content = [&](const int bx, const int by, const int bz)
		{
			for (int i = 0; i < 64; i++)
			{
				const int cx = bx + (i & 3) * 4, cy = by + ((i >> 2) & 3) * 4, cz = bz + ((i >> 4) & 3) * 4;
				if (l2_has_content(cx, cy, cz)) return true;
			}
			return false;
		};

		// root cells, ordered so a column's z cells are contiguous (topDownViewNode points at
		// the first of them and the descent adds cz - zMin)
		const int columns = root_dim[0] * root_dim[1];
		out.top_down_view_nodes.resize(columns);

		std::vector<std::array<int, 3>> root_cells; // in node order
		for (int cy = 0; cy < root_dim[1]; cy++)
		{
			for (int cx = 0; cx < root_dim[0]; cx++)
			{
				const int col = cy * root_dim[0] + cx;
				int z_lo = -1, z_hi = -1;
				for (int cz = 0; cz < root_dim[2]; cz++)
				{
					if (l1_has_content(cx * 16, cy * 16, cz * 16))
					{
						if (z_lo < 0) z_lo = cz;
						z_hi = cz;
					}
				}
				if (z_lo < 0)
				{
					out.top_down_view_nodes[col] = { -1, 0x7FFFFFFF, static_cast<int>(0x80000000) };
					continue;
				}
				out.top_down_view_nodes[col] = { static_cast<int>(root_cells.size()), z_lo, z_hi };
				for (int cz = z_lo; cz <= z_hi; cz++)
				{
					root_cells.push_back({ cx * 16, cy * 16, cz * 16 });
				}
			}
		}

		// Build the node arrays breadth-first by level, because a node's firstNodeIndex must
		// point at a contiguous run of its children.
		std::vector<std::array<int, 3>> level1; // origins of level-1 nodes, in order
		out.internal_nodes.resize(root_cells.size());
		for (size_t r = 0; r < root_cells.size(); r++)
		{
			const auto& o = root_cells[r];
			unsigned long long mask = 0;
			const auto first = static_cast<int>(level1.size());
			for (int i = 0; i < 64; i++)
			{
				const int bx = o[0] + (i & 3) * 4, by = o[1] + ((i >> 2) & 3) * 4, bz = o[2] + ((i >> 4) & 3) * 4;
				if (!l1_has_content(bx, by, bz)) continue;
				mask |= 1ull << i;
				level1.push_back({ bx, by, bz });
			}
			auto& n = out.internal_nodes[r];
			n.first_node_index[0] = first; // children are internal nodes, bit 31 clear
			n.first_node_index[1] = first;
			n.child_node_mask[0] = static_cast<unsigned int>(mask & 0xFFFFFFFF);
			n.child_node_mask[1] = static_cast<unsigned int>(mask >> 32);
		}

		// level-1 nodes come next in the same array; their children are leaves
		const auto level1_base = out.internal_nodes.size();
		out.internal_nodes.resize(level1_base + level1.size());
		for (size_t i = 0; i < level1.size(); i++)
		{
			const auto& o = level1[i];
			unsigned long long mask = 0;
			const auto first = static_cast<int>(leaves.size());
			for (int k = 0; k < 64; k++)
			{
				const int lx = o[0] + (k & 3), ly = o[1] + ((k >> 2) & 3), lz = o[2] + ((k >> 4) & 3);
				if (!cell_present(lx, ly, lz)) continue;
				mask |= 1ull << k;
				leaves.push_back({ lx, ly, lz });
			}
			auto& n = out.internal_nodes[level1_base + i];
			n.first_node_index[0] = static_cast<int>(first | 0x80000000); // children are leaves
			n.first_node_index[1] = n.first_node_index[0];
			n.child_node_mask[0] = static_cast<unsigned int>(mask & 0xFFFFFFFF);
			n.child_node_mask[1] = static_cast<unsigned int>(mask >> 32);
		}

		// the level-0 nodes point at level-1 nodes, which now start at level1_base
		for (size_t r = 0; r < root_cells.size(); r++)
		{
			auto& n = out.internal_nodes[r];
			n.first_node_index[0] += static_cast<int>(level1_base);
			n.first_node_index[1] = n.first_node_index[0];
		}

		out.leaf_count = static_cast<unsigned int>(leaves.size());
		out.leaf_nodes.assign(leaves.size(), 0); // every leaf shares the empty light list
		out.light_list.assign(1, 0);

		// ---- tetrahedra, six per present cell ---------------------------------------------
		out.voxel_start_tetrahedron.assign(leaves.size(), no_index);
		out.tetrahedrons.reserve(leaves.size() * 24);

		const unsigned int corner_bit = params.corner_flag ? 0x80000000u : 0u;
		std::map<face_key, std::pair<unsigned int, int>> faces; // face -> (tet, vertex opposite)

		for (size_t l = 0; l < leaves.size(); l++)
		{
			const auto& c = leaves[l];
			unsigned int corner[8];
			for (int i = 0; i < 8; i++)
			{
				corner[i] = probe_index(c.cx + (i & 1), c.cy + ((i >> 1) & 1), c.cz + ((i >> 2) & 1));
			}

			out.voxel_start_tetrahedron[l] = static_cast<unsigned int>(out.tetrahedrons.size() / 4);
			for (const auto& t : cube_tets)
			{
				for (int k = 0; k < 4; k++)
				{
					out.tetrahedrons.push_back(corner[t[k]] | corner_bit);
				}
			}
		}
		out.tetrahedron_count = static_cast<unsigned int>(out.tetrahedrons.size() / 4);

		// ---- neighbours: the tetrahedron opposite each vertex ------------------------------
		out.tetrahedron_neighbors.assign(out.tetrahedrons.size(), no_index);
		for (unsigned int t = 0; t < out.tetrahedron_count; t++)
		{
			for (int k = 0; k < 4; k++)
			{
				unsigned int v[3];
				int n = 0;
				for (int j = 0; j < 4; j++)
				{
					if (j != k) v[n++] = out.tetrahedrons[t * 4 + j] & 0xFFFF;
				}
				const auto key = make_face(v[0], v[1], v[2]);
				const auto it = faces.find(key);
				if (it == faces.end())
				{
					faces.emplace(key, std::make_pair(t, k));
				}
				else
				{
					const auto other = it->second;
					out.tetrahedron_neighbors[t * 4 + k] = other.first;
					out.tetrahedron_neighbors[other.first * 4 + other.second] = t;
					faces.erase(it);
				}
			}
		}

		// ---- header / zone ----------------------------------------------------------------
		for (int i = 0; i < 3; i++)
		{
			out.root_node_dimension[i] = root_dim[i];
			out.node_coord_bit_shift[i] = shift[i];
			out.bound_min[i] = origin[i];
			out.bound_max[i] = origin[i] + static_cast<float>(root_dim[i]) * root_size;
		}

		out.zone_num_probes = out.probe_count;
		out.zone_first_probe = 0;
		out.zone_num_tetrahedrons = out.tetrahedron_count;
		// shipped zones store the last tetrahedron index here; it reads as a walk seed
		out.zone_first_tetrahedron = out.tetrahedron_count ? out.tetrahedron_count - 1 : 0;
		out.zone_first_voxel_tetrahedron_index = 0;
		out.zone_num_voxel_tetrahedron_indices = out.leaf_count;

		out.valid = out.tetrahedron_count != 0;
		return out;
	}
}
