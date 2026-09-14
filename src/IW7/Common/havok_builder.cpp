#include "stdafx.hpp"
#include "havok_builder.hpp"

// IW7/stdafx.hpp only pulls in <vector>, <memory>, <iostream> and <sstream>.
#include <array>
#include <algorithm>
#include <tuple>
#include <utility>
#include <cmath>
#include <cfloat>
#include <cstring>
#include <map>
#include <functional>

// Writer for hknpCompressedMeshShape inside a hk_2014.2.5-r1 binary packfile.
//
// Layout sources, all recorded in docs/iw7-havok-collision.md:
//   - container structs      : genuine Havok SDK headers (hkPackfileHeader /
//                              hkPackfileSectionHeader), round-tripped byte-exact
//                              against every shipped IW7 .hkx
//   - class field offsets    : IW7's own runtime hkClass tables, dumped to
//                              docs/iw7-havok-reflection.txt
//   - encodings              : derived from the shipped stock world blobs and from
//                              IW8's decoder/builder for the parts that are pure
//                              algorithm (vertex codecs, BVH traversal)
//
// The BVH node quantisation nibbles are fully encoded; see the quantisation note
// further down for the codec and how it was verified.

namespace ZoneTool::IW7
{
	namespace havok
	{
		namespace builder
		{
			namespace
			{
				// ---------------------------------------------------------- constants

				constexpr auto HK_MAGIC0 = 0x57E0E057u;
				constexpr auto HK_MAGIC1 = 0x10C0C010u;
				constexpr auto HK_FILE_VERSION = 11;
				constexpr auto HK_HEADER_SIZE = 64;
				constexpr auto HK_SECTION_HEADER_SIZE = 64;
				constexpr auto HK_MAX_PREDICATE = 21;

				// Every vertex this writer emits is a shared vertex, so the field that bounds
				// a section is hkcdStaticMeshTreeBaseSection::m_numSharedIndices, a uint8.
				// (m_numPackedVertices, also a uint8, is always 0 here.) Stock agrees: the
				// widest section in any shipped world blob carries 252-255 shared indices.
				constexpr auto MAX_SHARED_INDICES_PER_SECTION = 255;
				// hkcdStaticMeshTreeBasePrimitive::m_indices is uint8[4].
				constexpr auto MAX_VERTEX_INDEX = 255;
				// A primitive key is (section << 8) | (localPrimitive << 1) | triangleInQuad,
				// so a section can hold at most 127 primitives. Every shipped map uses this
				// same 8-bit shift, so match it rather than widening the key.
				constexpr auto KEY_SECTION_SHIFT = 8;

				// Contents mask for the generated world shape, taken from shipped IW7 world
				// blobs (mp_breakneck / mp_afghan both use 0x28033ED1).
				constexpr auto WORLD_SHAPE_CONTENTS = 0x28033ED1u;

				// The material CRC shipped world blobs use for their common surfaces
				// (mp_frontend uses it for every tag; mp_afghan for its first entries).
				constexpr auto DEFAULT_MATERIAL_CRC = 0x1AB7BC33u;
				constexpr auto MAX_PRIMITIVES_PER_SECTION = 127;

				// Object sizes, straight from IW7 reflection.
				constexpr auto SIZEOF_COMPRESSED_MESH_SHAPE = 160;
				constexpr auto SIZEOF_COMPRESSED_MESH_SHAPE_DATA = 256;
				constexpr auto SIZEOF_MESH_TREE = 160;
				constexpr auto SIZEOF_SECTION = 96;
				constexpr auto SIZEOF_PRIMITIVE = 4;
				constexpr auto SIZEOF_DATA_RUN = 4;
				constexpr auto SIZEOF_NODE_TOP = 5; // hkcdStaticTreeCodec3Axis5
				constexpr auto SIZEOF_NODE_SECTION = 4; // hkcdStaticTreeCodec3Axis4

				// Class signatures, read out of the __classnames__ table of shipped files.
				constexpr auto SIG_HK_CLASS = 0x33D42383u;
				constexpr auto SIG_HK_CLASS_MEMBER = 0xB0EFA719u;
				constexpr auto SIG_HK_CLASS_ENUM = 0x8A3609CFu;
				constexpr auto SIG_HK_CLASS_ENUM_ITEM = 0xCE6F8A6Cu;
				constexpr auto SIG_SHAPE_LIST = 0xC909A395u;
				constexpr auto SIG_COMPRESSED_MESH_SHAPE = 0x1318CC9Fu;
				constexpr auto SIG_COMPRESSED_MESH_SHAPE_DATA = 0x54FD8D57u;
				constexpr auto SIG_DYNAMIC_COMPOUND_SHAPE = 0x408A0623u;
				constexpr auto SIG_CONVEX_POLYTOPE_SHAPE = 0x6097F158u;
				constexpr auto SIG_CONVEX_POLYTOPE_CONNECTIVITY = 0xB51806FDu;
				constexpr auto SIG_DYNAMIC_COMPOUND_SHAPE_DATA = 0xF33DC3CCu;
				constexpr auto SIG_PHYSICS_ASSET = 0x0DFB2195u;
				constexpr auto SIG_PHYSICS_SYSTEM_DATA = 0xB26317A4u;
				constexpr auto SIG_REF_COUNTED_PROPERTIES = 0x7C574867u;
				constexpr auto SIG_SHAPE_MASS_PROPERTIES = 0xE9191728u;

				// Object sizes for the ents-side shapes, from IW7 reflection.
				constexpr auto SIZEOF_DYNAMIC_COMPOUND_SHAPE = 208;
				constexpr auto SIZEOF_DYNAMIC_COMPOUND_SHAPE_DATA = 56;
				constexpr auto SIZEOF_CONVEX_POLYTOPE_SHAPE = 96;
				constexpr auto SIZEOF_CONVEX_POLYTOPE_CONNECTIVITY = 48;
				constexpr auto SIZEOF_SHAPE_INSTANCE = 128;
				constexpr auto SIZEOF_DYNAMIC_TREE_NODE = 32;

				// hknpConvexShape::vertices is padded up to a multiple of four, repeating the
				// last real vertex, and hknpConvexPolytopeShapeConnectivity::vertexEdges is
				// padded the same way. True for 1,848 of 1,848 shipped convexes, with every
				// pad entry a byte-copy of the last real one. Havok reads vertices four at a
				// time, so an unpadded array is read past its end -- a six-vertex wedge would
				// pull two vertices out of the planes array that follows it.
				constexpr std::size_t padded_vertex_count(const std::size_t count)
				{
					return (count + 3) / 4 * 4;
				}

				// hknpShape header values, copied from every shipped ents blob.
				constexpr auto CONVEX_SHAPE_FLAGS = 0x0143u;
				constexpr auto CONVEX_DISPATCH_TYPE = 1;
				constexpr auto COMPOUND_SHAPE_FLAGS = 0x0004u;
				constexpr auto COMPOUND_DISPATCH_TYPE = 2;

				// ------------------------------------------------------------- helpers

				struct byte_buffer
				{
					std::vector<std::uint8_t> data;

					std::size_t size() const { return this->data.size(); }

					void align(const std::size_t alignment, const std::uint8_t fill = 0)
					{
						while (this->data.size() % alignment)
						{
							this->data.push_back(fill);
						}
					}

					void write(const void* src, const std::size_t count)
					{
						const auto* p = static_cast<const std::uint8_t*>(src);
						this->data.insert(this->data.end(), p, p + count);
					}

					template <typename T> void write(const T& value)
					{
						this->write(&value, sizeof(T));
					}

					void fill(const std::size_t count, const std::uint8_t value)
					{
						this->data.insert(this->data.end(), count, value);
					}

					// Reserve space and return the offset, for fields patched later.
					std::size_t reserve(const std::size_t count)
					{
						const auto offset = this->data.size();
						this->fill(count, 0);
						return offset;
					}

					template <typename T> void patch(const std::size_t offset, const T& value)
					{
						std::memcpy(this->data.data() + offset, &value, sizeof(T));
					}
				};

				// hkArray<T> is { T* m_data; int m_size; int m_capacityAndFlags; }. In a
				// packfile m_data is zero and fixed up by a local fixup; the flags word
				// always carries DONT_DEALLOCATE.
				constexpr auto HK_ARRAY_DONT_DEALLOCATE = 0x80000000u;

				struct hk_array_ref
				{
					std::size_t field_offset = 0; // where the hkArray lives
					std::size_t data_offset = 0; // where its payload lives
					int count = 0;
				};

				void write_hk_array_header(byte_buffer& buf, const int count)
				{
					buf.reserve(8); // m_data, patched by a local fixup
					buf.write<std::int32_t>(count);
					buf.write<std::uint32_t>(count | HK_ARRAY_DONT_DEALLOCATE);
				}

				// ------------------------------------------------------ vertex packing

				// Packed vertices are 11/11/10 bits in the owning section's codec space:
				//   pos[i] = codecParms[i] + raw[i] * codecParms[3 + i]
				constexpr int PACKED_BITS[3] = {11, 11, 10};

				// Shared vertices are quantised over the *tree* domain at 21/21/22 bits.
				// Every shipped world blob stores its geometry this way -- mp_frontend has
				// 0 packed / 307 shared, mp_afghan 16 / 176,653, mp_paris 63 / 316,789,
				// mp_breakneck 185 / 306,394. The only packed-only blob in existence is the
				// custom mp_shipment, whose collision is known to be broken, so the packed
				// path is not what the runtime expects for world geometry.
				constexpr int SHARED_BITS[3] = {21, 21, 22};

				std::uint64_t pack_shared_vertex(const float* pos, const float* mins,
					const float* maxs)
				{
					std::uint64_t packed = 0;
					auto shift = 0;

					for (auto i = 0; i < 3; i++)
					{
						const auto extent = maxs[i] - mins[i];
						const auto max_value = (1u << SHARED_BITS[i]) - 1u;
						// Havok's decoder scales by extent * 2^-bits, so encode against the
						// same step rather than extent / (2^bits - 1).
						const auto step = extent * std::ldexp(1.0f, -SHARED_BITS[i]);

						auto raw = 0u;
						if (step > 0.0f)
						{
							const auto q = std::lround((pos[i] - mins[i]) / step);
							raw = static_cast<std::uint32_t>(
								std::clamp<long>(q, 0, static_cast<long>(max_value)));
						}

						packed |= static_cast<std::uint64_t>(raw) << shift;
						shift += SHARED_BITS[i];
					}

					return packed;
				}

				// What the runtime reads back for a shared vertex. Quantisation is what decides
				// whether a quad counts as flat, so that call has to be made on these positions
				// and not on the source ones.
				void unpack_shared_vertex(const std::uint64_t packed, const float* mins,
					const float* maxs, float(&out)[3])
				{
					auto shift = 0;
					for (auto i = 0; i < 3; i++)
					{
						const auto max_value = (1ull << SHARED_BITS[i]) - 1ull;
						const auto raw = (packed >> shift) & max_value;
						const auto step = (maxs[i] - mins[i]) * std::ldexp(1.0f, -SHARED_BITS[i]);
						out[i] = mins[i] + static_cast<float>(raw) * step;
						shift += SHARED_BITS[i];
					}
				}

				std::uint32_t pack_vertex(const float* pos, const float* codec_parms)
				{
					std::uint32_t packed = 0;
					int shift = 0;

					for (auto i = 0; i < 3; i++)
					{
						const auto scale = codec_parms[3 + i];
						const auto max_value = (1 << PACKED_BITS[i]) - 1;

						auto raw = 0;
						if (scale > 0.0f)
						{
							raw = static_cast<int>(std::lround((pos[i] - codec_parms[i]) / scale));
						}

						raw = std::clamp(raw, 0, max_value);
						packed |= static_cast<std::uint32_t>(raw) << shift;
						shift += PACKED_BITS[i];
					}

					return packed;
				}

				// ------------------------------------------------------------ sections

				struct build_section
				{
					std::vector<std::array<float, 3>> verts;
					std::vector<std::array<std::uint8_t, 4>> primitives;
					std::vector<std::uint16_t> tags;
					std::vector<int> contents;
					std::vector<std::uint32_t> material_crcs;
					std::vector<std::uint64_t> user_data;
					std::vector<bool> quads;
					float codec_parms[6] = {};
					float mins[3] = {};
					float maxs[3] = {};
				};

				void finalise_section_codec(build_section& section)
				{
					for (auto i = 0; i < 3; i++)
					{
						section.mins[i] = FLT_MAX;
						section.maxs[i] = -FLT_MAX;
					}

					for (const auto& v : section.verts)
					{
						for (auto i = 0; i < 3; i++)
						{
							section.mins[i] = std::min(section.mins[i], v[i]);
							section.maxs[i] = std::max(section.maxs[i], v[i]);
						}
					}

					for (auto i = 0; i < 3; i++)
					{
						const auto extent = section.maxs[i] - section.mins[i];
						const auto max_value = static_cast<float>((1 << PACKED_BITS[i]) - 1);

						section.codec_parms[i] = section.mins[i];
						section.codec_parms[3 + i] = (extent > 0.0f) ? (extent / max_value) : 0.0f;
					}
				}

				// Split the triangle soup into sections. Every vertex goes in as a shared
				// vertex and numPackedVertices stays 0, which is what stock does too -- 9,117
				// of the 9,708 sections in the shipped world blobs use exactly that form, and
				// packed vertices are the rare leftover (the widest stock section has 7).
				std::vector<build_section> split_into_sections(const mesh_input& input)
				{
					std::vector<build_section> sections;
					build_section current{};

					const auto flush = [&]
					{
						if (!current.primitives.empty())
						{
							finalise_section_codec(current);
							sections.emplace_back(std::move(current));
							current = {};
						}
					};

					auto degenerate_dropped = 0;

					for (const auto& tri : input.triangles)
					{
						// Two independent uint8 limits: the shared-index count (which also
						// bounds the vertex index in hkcdStaticMeshTreeBasePrimitive), and the
						// primitive count packed into the low byte of the section's primitives
						// field (which also bounds the section BVH right-child offset to 0xFE).
						// The +4 is the worst case for one quad.
						if (current.verts.size() + 4 > MAX_SHARED_INDICES_PER_SECTION ||
							current.primitives.size() >= MAX_PRIMITIVES_PER_SECTION)
						{
							flush();
						}

						// A primitive whose indices[1..3] are all equal is not a triangle to
						// Havok -- it is a *custom primitive*, and the runtime then reads
						// sharedVerticesIndex[indices[0]] as a shape-type descriptor and
						// indexes hknpCompressedMeshShapeInternals::s_customPrimitiveToShapeType
						// with its low nibble. That table has three entries.
						//
						// Because a triangle is emitted as [a, b, c, c], any triangle whose
						// second and third vertices weld to the same index produces exactly
						// that pattern by accident. It happened for real: the shipped
						// mp_test_h1 conversion contains four such primitives, and their
						// descriptors decode to shape types 4, 10, 12 and 14 -- all out of
						// bounds on a three-entry table.
						//
						// Degenerate triangles have zero area and contribute no collision, so
						// drop them before they can be mistaken for custom primitives. The
						// test is on positions rather than welded indices so that no orphan
						// vertices are added for a triangle that is about to be discarded.
						const auto same = [](const float* a, const float* b)
						{
							return a[0] == b[0] && a[1] == b[1] && a[2] == b[2];
						};
						if (same(tri.verts[0], tri.verts[1]) || same(tri.verts[1], tri.verts[2])
							|| same(tri.verts[0], tri.verts[2]))
						{
							degenerate_dropped++;
							continue;
						}

						// A quad's fourth corner must be distinct from the rest, or the primitive
						// would weld back down to a triangle (or worse, to the [a,b,b,b] custom
						// pattern). Fall back to the triangle it is built from.
						auto quad = tri.is_quad;
						if (quad && (same(tri.vert3, tri.verts[0]) || same(tri.vert3, tri.verts[1])
							|| same(tri.vert3, tri.verts[2])))
						{
							quad = false;
						}

						const float* corners[4] = {
							tri.verts[0], tri.verts[1], tri.verts[2], tri.vert3
						};

						std::array<std::uint8_t, 4> indices{};
						for (auto i = 0; i < (quad ? 4 : 3); i++)
						{
							auto found = -1;
							for (auto v = 0u; v < current.verts.size(); v++)
							{
								if (current.verts[v][0] == corners[i][0] &&
									current.verts[v][1] == corners[i][1] &&
									current.verts[v][2] == corners[i][2])
								{
									found = static_cast<int>(v);
									break;
								}
							}

							if (found < 0)
							{
								found = static_cast<int>(current.verts.size());
								std::array<float, 3> v{};
								std::memcpy(v.data(), corners[i], sizeof(float[3]));
								current.verts.emplace_back(v);
							}

							indices[i] = static_cast<std::uint8_t>(found);
						}

						// indices[2] == indices[3] marks a triangle; anything else is a quad.
						if (!quad)
						{
							indices[3] = indices[2];
						}

						current.primitives.emplace_back(indices);
						current.quads.emplace_back(quad);
						current.tags.emplace_back(tri.surface_tag);
						current.contents.emplace_back(tri.contents);
						current.material_crcs.emplace_back(tri.material_crc);
						current.user_data.emplace_back(tri.user_data);
					}

					flush();

					if (degenerate_dropped)
					{
						ZONETOOL_INFO("havok: dropped %d degenerate triangle(s); emitting them "
							"would have produced primitives the runtime reads as custom "
							"primitives", degenerate_dropped);
					}

					return sections;
				}

				// ---------------------------------------------------------------- trees

				// Both BVH levels are plain binary trees stored in preorder with
				// numNodes == 2 * numLeaves - 1. Encoding, verified against all three
				// stock maps:
				//
				//   per-section (4 bytes, hkcdStaticTreeCodec3Axis4)
				//       internal : (data & 1) != 0, rightChild = n + (data & 0xFE)
				//       leaf     : (data & 1) == 0
				//
				//   top level   (5 bytes, hkcdStaticTreeCodec3Axis5)
				//       internal : (hiData & 0x80) != 0
				//                  rightChild = n + 2 * (((hiData & 0x7F) << 8) | loData)
				//       leaf     : sectionIndex = (hiData << 8) | loData
				//
				// The offset is stored halved at the top level because every internal node
				// has two children, so subtree sizes are always odd and the right-child
				// offset is always even.
				//
				// The three xyz bytes hold two 4-bit quantisation nibbles each; they are
				// encoded properly below rather than left at zero.

				// ------------------------------------------------------- BVH quantisation
				//
				// Each node's three xyz bytes hold two 4-bit fields that shrink the child box
				// in from the parent. IW8's decoder does this per axis, in SIMD:
				//
				//     scale    = (parentMax - parentMin) * K
				//     childMin = parentMin + hi*hi * scale
				//     childMax = parentMax - lo*lo * scale
				//
				// i.e. the inset is quadratic in the nibble, and K is exactly 1/226 -- NOT 1/225
				// (== 15^2): nibble 15 insets by 225/226 of the extent, never the whole of it.
				// Settled on stock mp_afghan by tightness, not just containment. With no
				// tolerance, 1400/1400 top-level leaf boxes contain their section domain at
				// 1/226 (median slack -0.002 of the extent), against 4/1400 at 1/225, where the
				// error compounds down the tree until boxes sit off their sections entirely.
				// Section trees agree: 99.3% of 25,822 leaves are within 0.1% at 1/226, against
				// 36% at 1/225 -- an earlier 1%-tolerance test could not tell the two apart.
				//
				// Encoding with 1/225 while the game decodes with 1/226 left 406 of
				// mp_test_h1's 732 world triangles outside their own leaf box. Rays go through
				// the separately built simdTree and still hit them, but shape queries -- the
				// player's movement sweep -- walk this tree, so the player passed through most
				// walls while bullets and debug traces stopped on them.
				//
				// Emitting zero nibbles is legal -- shipped single-primitive sections do it --
				// but it means every node box equals the whole tree domain, which is what made
				// collision far larger than the visible surfaces. Here each nibble is chosen
				// as the LARGEST value whose inset still stays outside the true child box, so
				// the decoded box always contains the geometry and is as tight as 4 bits allow.

				// --------------------------------------------- contents filter mask
				//
				// ShapeTagData::collisionFilterInfo is a CoD contents mask, but IW7 only ever
				// stores a SUBSET of the bits a CoD compiler emits. Across every shipped IW7
				// blob -- three world blobs plus four ents blobs, 31 distinct values -- the
				// union of all collisionFilterInfo bits is exactly 0xC7FFBFFF, and that same
				// value appears literally as the contents of stock trigger shapes. It is the
				// engine's "all valid contents" mask.
				//
				// The bits it excludes are 0x38004000. The one that matters in practice is
				// 0x08000000, CONTENTS_DETAIL, which every IW3/IW5 compiler sets on detail
				// brushes -- i.e. most of a map. Passing it through made IW7's collision
				// filter reject those surfaces and the player walked through the level.
				//
				// This applies to collisionFilterInfo, and to the ents list's shapeContents,
				// which measures clean against the mask: the union of every shapeContents
				// value in all six shipped ents blobs is exactly 0xC7FFBFFF, with no value
				// carrying a bit outside it.
				//
				// The WORLD shape's contents is the exception and is NOT masked -- stock uses
				// the full range there (0x28033ED1 and friends carry 0x08000000) -- but it is
				// a fixed constant rather than anything derived from the map, so it never
				// passes through here. See WORLD_SHAPE_CONTENTS.
				constexpr auto CONTENTS_FILTER_MASK = 0xC7FFBFFFu;

				// Always applied: no shipped tag carries a bit outside this mask, and the one
				// that mattered -- 0x08000000, CONTENTS_DETAIL, set by the IW3/IW5 compiler on
				// most of a map -- is rejected by IW7's collision filter.
				inline std::uint32_t filter_contents(const std::uint32_t contents)
				{
					return contents & CONTENTS_FILTER_MASK;
				}

				constexpr auto NIBBLE_MAX = 15;
				constexpr auto NIBBLE_SCALE = 226.0f; // Havok's divisor; see the note above

				// Largest n in [0,15] with (n*n / 226) * extent <= inset.
				int quantise_inset(const float inset, const float extent)
				{
					if (!(extent > 0.0f) || !(inset > 0.0f))
					{
						return 0;
					}

					const auto ratio = std::clamp(inset / extent, 0.0f, 1.0f);
					auto n = static_cast<int>(std::floor(std::sqrt(ratio * NIBBLE_SCALE)));
					n = std::clamp(n, 0, NIBBLE_MAX);

					// Round down until the inset genuinely fits, guarding float error.
					while (n > 0 && (static_cast<float>(n * n) / NIBBLE_SCALE) * extent > inset)
					{
						n--;
					}
					return n;
				}

				float decode_inset(const int nibble, const float extent)
				{
					return (static_cast<float>(nibble * nibble) / NIBBLE_SCALE) * extent;
				}

				struct aabb
				{
					float lo[3];
					float hi[3];

					void reset()
					{
						for (auto i = 0; i < 3; i++)
						{
							lo[i] = FLT_MAX;
							hi[i] = -FLT_MAX;
						}
					}

					void add(const aabb& other)
					{
						for (auto i = 0; i < 3; i++)
						{
							lo[i] = std::min(lo[i], other.lo[i]);
							hi[i] = std::max(hi[i], other.hi[i]);
						}
					}
				};

				// Writes the three xyz bytes for `child` relative to `parent`, and returns the
				// box the decoder will actually reconstruct -- which is what the child's own
				// children must then be encoded against.
				aabb encode_node_aabb(std::uint8_t* xyz, const aabb& parent, const aabb& child)
				{
					aabb decoded{};
					for (auto i = 0; i < 3; i++)
					{
						const auto extent = parent.hi[i] - parent.lo[i];
						const auto hi_n = quantise_inset(child.lo[i] - parent.lo[i], extent);
						const auto lo_n = quantise_inset(parent.hi[i] - child.hi[i], extent);

						xyz[i] = static_cast<std::uint8_t>((hi_n << 4) | lo_n);
						decoded.lo[i] = parent.lo[i] + decode_inset(hi_n, extent);
						decoded.hi[i] = parent.hi[i] - decode_inset(lo_n, extent);
					}
					return decoded;
				}

				// leaf_nodes[sectionIndex] receives the index of the node holding that
				// section's leaf. Shipped files store exactly that in
				// hkcdStaticMeshTreeBaseSection::leafIndex -- verified against all three
				// stock maps -- so it is not the section index.
				void emit_preorder_tree_top(byte_buffer& buf, const int first_leaf,
					const int leaf_count, std::vector<int>& leaf_nodes,
					const std::vector<aabb>& leaf_boxes, const aabb& parent)
				{
					const auto self = static_cast<int>(buf.size() / SIZEOF_NODE_TOP);

					aabb box{};
					box.reset();
					for (auto i = 0; i < leaf_count; i++)
					{
						box.add(leaf_boxes[first_leaf + i]);
					}

					std::uint8_t xyz[3] = {};
					const auto decoded = encode_node_aabb(xyz, parent, box);

					if (leaf_count <= 1)
					{
						// Leaf: section index in (hiData << 8) | loData.
						leaf_nodes[first_leaf] = self;
						buf.write(xyz, sizeof(xyz));
						buf.write<std::uint8_t>(static_cast<std::uint8_t>((first_leaf >> 8) & 0x7F));
						buf.write<std::uint8_t>(static_cast<std::uint8_t>(first_leaf & 0xFF));
						return;
					}

					const auto left_count = leaf_count / 2;
					const auto right_count = leaf_count - left_count;

					// Left subtree occupies (2 * left_count - 1) nodes immediately after
					// this one, so the right child sits at that offset + 1.
					const auto right_offset = 2 * left_count;
					const auto encoded = right_offset / 2;

					buf.write(xyz, sizeof(xyz));
					buf.write<std::uint8_t>(static_cast<std::uint8_t>(0x80 | ((encoded >> 8) & 0x7F)));
					buf.write<std::uint8_t>(static_cast<std::uint8_t>(encoded & 0xFF));

					emit_preorder_tree_top(buf, first_leaf, left_count, leaf_nodes,
						leaf_boxes, decoded);
					emit_preorder_tree_top(buf, first_leaf + left_count, right_count, leaf_nodes,
						leaf_boxes, decoded);
				}

				void emit_preorder_tree_section(byte_buffer& buf, const int first_leaf,
					const int leaf_count, const std::vector<aabb>& leaf_boxes, const aabb& parent)
				{
					aabb box{};
					box.reset();
					for (auto i = 0; i < leaf_count; i++)
					{
						box.add(leaf_boxes[first_leaf + i]);
					}

					std::uint8_t xyz[3] = {};
					const auto decoded = encode_node_aabb(xyz, parent, box);

					if (leaf_count <= 1)
					{
						// A section leaf's data byte is (primitiveIndex << 1) -- bit 0 clear
						// marks the leaf, and the remaining 7 bits ARE the index of the
						// primitive it bounds. Writing a bare 0 here pointed every leaf in
						// the section at primitive 0, so the tree bounded the right boxes
						// but named the wrong geometry. Verified against stock: in all 5,302
						// multi-primitive sections of the three shipped world blobs the leaf
						// indices are exactly a permutation of 0..primitiveCount-1, never
						// duplicated.
						if (first_leaf > 0x7F)
						{
							ZONETOOL_ERROR("havok: section BVH leaf primitive index %d does "
								"not fit in the 7-bit field", first_leaf);
						}

						buf.write(xyz, sizeof(xyz));
						buf.write<std::uint8_t>(
							static_cast<std::uint8_t>((first_leaf << 1) & 0xFE));
						return;
					}

					const auto left_count = leaf_count / 2;
					const auto right_count = leaf_count - left_count;
					const auto right_offset = 2 * left_count;

					if (right_offset > 0xFE)
					{
						// Cannot be encoded in one byte. split_into_sections keeps sections
						// small enough that this never happens; assert loudly if it does.
						ZONETOOL_ERROR("havok: section BVH right-child offset %d exceeds the "
							"8-bit field", right_offset);
					}

					buf.write(xyz, sizeof(xyz));
					buf.write<std::uint8_t>(static_cast<std::uint8_t>((right_offset & 0xFE) | 1));

					emit_preorder_tree_section(buf, first_leaf, left_count, leaf_boxes, decoded);
					emit_preorder_tree_section(buf, first_leaf + left_count, right_count,
						leaf_boxes, decoded);
				}

				// ------------------------------------------------------- data runs

				// hknpCompressedMeshShapeTreeDataRun is
				//   { uint16 value; uint8 index; uint8 count; }
				// i.e. run-length encoded per-surface tags: `value` applies to `count`
				// primitives starting at `index` within the section.
				void emit_data_runs(byte_buffer& buf, const std::vector<std::uint16_t>& tags,
					int& runs_written)
				{
					runs_written = 0;

					for (auto i = 0u; i < tags.size();)
					{
						auto j = i;
						while (j < tags.size() && tags[j] == tags[i] && (j - i) < 0xFF)
						{
							j++;
						}

						buf.write<std::uint16_t>(tags[i]);
						buf.write<std::uint8_t>(static_cast<std::uint8_t>(i));
						buf.write<std::uint8_t>(static_cast<std::uint8_t>(j - i));

						runs_written++;
						i = j;
					}
				}
			}

			// Shared by the world blob and per-model physics assets. The two differ only in
			// what wraps the mesh: the world hangs it off a HavokPhysicsShapeList, a model
			// hangs it off a HavokPhysicsAsset with a static body. The hknpCompressedMeshShape
			// and its data are byte-for-byte the same either way, which is why this is one
			// function rather than two.
			std::vector<std::uint8_t> build_mesh_blob(const mesh_input& input,
				const physics_asset_input* physics_asset)
			{
				if (input.triangles.empty())
				{
					ZONETOOL_ERROR("havok: refusing to build a mesh blob from 0 triangles");
					return {};
				}

				auto sections = split_into_sections(input);
				if (sections.empty())
				{
					ZONETOOL_ERROR("havok: no sections produced");
					return {};
				}

				// ------------------------------------------------------- shape tags
				// primitiveDataRuns.value indexes shapeTagData, so build the palette of
				// distinct surface tags first and remap every primitive onto it.
				//
				// Slot 0 is not special to the runtime, despite shipped files making it look
				// that way. Every stock world blob has shapeTagData[0] with
				// collisionFilterInfo 0x800 where the rest use 0x1, but measuring the data
				// runs shows value 0 *is* referenced -- by exactly 6 primitives in each of
				// mp_afghan, mp_paris and mp_breakneck. Six primitives carrying the sky
				// contents bit is the map's sky box, and it lands in slot 0 only because the
				// compiler emits it first. There is no evidence the index itself is reserved,
				// so the palette starts at 0 and correctness rests on each tag carrying the
				// right contents, which it does.
				//
				// A tag is (contents, material): collisionFilterInfo in shapeTagData is the
				// CoD contents mask, so two surfaces sharing a material but differing in
				// contents -- solid vs sky vs clip -- must not share a tag. Getting this
				// wrong hands real geometry somebody else's filter and it stops colliding.
				// The key is (contents, materialCRC, userData): shapeTagData carries exactly
				// those three things, so two surfaces differing in any must not share a tag, and
				// two IW5 materials that map to the same triple may safely collapse into one.
				std::vector<std::tuple<int, std::uint32_t, std::uint64_t>> palette;

				for (auto& section : sections)
				{
					for (auto i = 0u; i < section.tags.size(); i++)
					{
						const auto key = std::make_tuple(section.contents[i],
							section.material_crcs[i], section.user_data[i]);

						auto it = std::find(palette.begin(), palette.end(), key);
						if (it == palette.end())
						{
							palette.emplace_back(key);
							it = palette.end() - 1;
						}
						section.tags[i] = static_cast<unsigned short>(
							std::distance(palette.begin(), it));
					}
				}

				// -------------------------------------------------------- key space
				// A primitive key packs (section, primitive, triangle-within-primitive).
				// triangleIsInterior has one bit per key and quadIsFlat one per primitive
				// slot, which is how the shipped bit counts line up.
				const auto section_count = static_cast<int>(sections.size());

				// The largest key any primitive in ANY section can produce. Taking the primitive
				// count from sections.back() alone is only safe because the section shift
				// dominates, and the last section is the short one -- so it under-reports the
				// real maximum. bits_per_key, numPrimitiveKeys, maxKey and the two bitfield
				// lengths are all derived from this, so derive it from the widest section.
				auto max_key = 0;
				for (auto si = 0u; si < sections.size(); si++)
				{
					const auto last = static_cast<int>(sections[si].primitives.size()) - 1;
					const auto key = static_cast<int>(si << KEY_SECTION_SHIFT) | (last << 1) | 1;
					max_key = std::max(max_key, key);
				}

				auto bits_per_key = 1;
				while ((1 << bits_per_key) <= max_key)
				{
					bits_per_key++;
				}

				std::size_t total_prims = 0;
				std::size_t total_verts = 0;
				for (const auto& section : sections)
				{
					total_prims += section.primitives.size();
					total_verts += section.verts.size();
				}

				// A triangle owns one key, a quad two -- its second triangle is key | 1.
				auto num_primitive_keys = 0;
				for (const auto& section : sections)
				{
					for (auto pi = 0u; pi < section.primitives.size(); pi++)
					{
						num_primitive_keys += section.quads[pi] ? 2 : 1;
					}
				}

				float world_min[3] = {FLT_MAX, FLT_MAX, FLT_MAX};
				float world_max[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
				for (const auto& section : sections)
				{
					for (auto i = 0; i < 3; i++)
					{
						world_min[i] = std::min(world_min[i], section.mins[i]);
						world_max[i] = std::max(world_max[i], section.maxs[i]);
					}
				}

				// ---------------------------------------------------------- payloads
				aabb world_box{};
				for (auto i = 0; i < 3; i++)
				{
					world_box.lo[i] = world_min[i];
					world_box.hi[i] = world_max[i];
				}

				// Per-section boxes drive the top-level tree's quantisation.
				std::vector<aabb> section_boxes(sections.size());
				for (auto i = 0u; i < sections.size(); i++)
				{
					for (auto c = 0; c < 3; c++)
					{
						section_boxes[i].lo[c] = sections[i].mins[c];
						section_boxes[i].hi[c] = sections[i].maxs[c];
					}
				}

				byte_buffer top_nodes;
				std::vector<int> leaf_nodes(sections.size(), 0);
				emit_preorder_tree_top(top_nodes, 0, section_count, leaf_nodes,
					section_boxes, world_box);

				std::vector<byte_buffer> section_nodes(sections.size());
				for (auto i = 0u; i < sections.size(); i++)
				{
					const auto& section = sections[i];

					// Per-primitive boxes drive the section tree's quantisation.
					std::vector<aabb> prim_boxes(section.primitives.size());
					for (auto p = 0u; p < section.primitives.size(); p++)
					{
						prim_boxes[p].reset();
						// Four corners for a quad, three for a triangle -- a quad's fourth
						// corner is outside the box its first three describe.
						for (auto k = 0; k < (section.quads[p] ? 4 : 3); k++)
						{
							const auto& v = section.verts[section.primitives[p][k]];
							aabb point{};
							for (auto c = 0; c < 3; c++)
							{
								point.lo[c] = v[c];
								point.hi[c] = v[c];
							}
							prim_boxes[p].add(point);
						}
					}

					emit_preorder_tree_section(section_nodes[i], 0,
						static_cast<int>(section.primitives.size()),
						prim_boxes, section_boxes[i]);
				}

				byte_buffer primitives;
				byte_buffer shared_vertices;
				byte_buffer shared_vertex_index;
				byte_buffer data_runs;
				std::vector<int> section_first_prim(sections.size());
				std::vector<int> section_first_vert(sections.size());
				std::vector<int> section_page(sections.size());
				std::vector<int> section_first_run(sections.size());
				std::vector<int> section_run_count(sections.size());

				for (auto i = 0u; i < sections.size(); i++)
				{
					auto& section = sections[i];

					section_first_prim[i] = static_cast<int>(primitives.size() / SIZEOF_PRIMITIVE);
					for (const auto& prim : section.primitives)
					{
						primitives.write(prim.data(), 4);
					}

					// All geometry goes in as shared vertices, laid out section by section.
					//
					// The runtime resolves one as
					//     sharedVertices[0x10000 * section.page
					//                    + sharedVerticesIndex[first + v - numPackedVertices]]
					// so sharedVerticesIndex is a uint16 and can only reach 65,536 vertices on
					// its own. Everything above that is addressed through the section's page.
					// This used to write the identity mapping with page pinned at 0, which is
					// correct only while the whole mesh fits in one page: past 65,536 vertices
					// the index wrapped and the tail of the map's collision folded back onto
					// the start of the vertex pool. Nothing threw, because a wrapped index is
					// still in range. Stock pages every world blob it ships -- mp_afghan uses
					// 3, cp_zmb and mp_paris 5 -- and keeps each section's index values
					// page-relative.
					//
					// A section is never allowed to straddle a page boundary, which is what
					// stock does too: every per-page index range in every shipped blob starts
					// at 0. When the next section would cross one, the vertex pool is padded
					// up to the boundary first. A section holds at most
					// MAX_SHARED_INDICES_PER_SECTION vertices, so the padding is bounded by
					// that and costs at most a couple of kilobytes across a whole map.
					auto vert_base = shared_vertices.size() / 8;
					const auto vert_needed = section.verts.size();

					if (vert_needed > 0
						&& ((vert_base + vert_needed - 1) >> 16) != (vert_base >> 16))
					{
						const auto next_page = ((vert_base >> 16) + 1) << 16;
						while (shared_vertices.size() / 8 < next_page)
						{
							shared_vertices.write<std::uint64_t>(0);
						}
						vert_base = next_page;
					}

					section_page[i] = static_cast<int>(vert_base >> 16);

					if (section_page[i] > 0xFF)
					{
						ZONETOOL_ERROR("havok: section %u needs vertex page %d, but the page "
							"field is a uint8 -- the mesh has more than %u shared vertices",
							i, section_page[i], 0x100u << 16);
						return {};
					}

					// firstSharedVertexIndex indexes sharedVerticesIndex, not the vertex pool,
					// and is packed into 24 bits alongside numPackedVertices.
					section_first_vert[i] = static_cast<int>(shared_vertex_index.size() / 2);

					if (section_first_vert[i] > 0xFFFFFF)
					{
						ZONETOOL_ERROR("havok: sharedVerticesIndex has %d entries, which does "
							"not fit the section's 24-bit first-index field",
							section_first_vert[i]);
						return {};
					}

					const auto page_base = static_cast<std::size_t>(section_page[i]) << 16;
					for (auto v = 0u; v < section.verts.size(); v++)
					{
						shared_vertices.write<std::uint64_t>(
							pack_shared_vertex(section.verts[v].data(), world_min, world_max));
						shared_vertex_index.write<std::uint16_t>(
							static_cast<std::uint16_t>(vert_base + v - page_base));
					}

					section_first_run[i] = static_cast<int>(data_runs.size() / SIZEOF_DATA_RUN);
					auto runs = 0;
					emit_data_runs(data_runs, section.tags, runs);
					section_run_count[i] = runs;
				}

				// ------------------------------------------------------------ simd tree
				//
				// hknpCompressedMeshShapeData::simdTree is a second, mandatory acceleration
				// structure: a 4-wide BVH over *primitives*. IW7's raycast path goes through
				// it (IW8 names the query
				// hkcdSimdTreeUtils::ProcessSimdTreeRayCastLeaves<...RayCastQuery...>), so a
				// mesh with an empty simdTree loads fine and then collides with nothing.
				// None of the 257 shipped compressed meshes has an empty one.
				//
				// hkcdSimdTreeNode is hkcdFourAabb (six hkVector4f in SoA order
				// lx,hx,ly,hy,lz,hz -- four children per component) followed by uint32
				// data[4]:
				//     data & 1  -> leaf,     primitiveKey  = data >> 1
				//     else      -> internal, childNodeIndex = data >> 1
				// An unused slot has an inverted AABB (min = +FLT_MAX, max = -FLT_MAX) and
				// data = 0. Node 0 is an all-inverted sentinel and node 1 is the real root,
				// which is why shipped trees have exactly one unreferenced node.

				struct simd_node
				{
					float lo[3][4];
					float hi[3][4];
					std::uint32_t data[4];
				};

				struct simd_item
				{
					std::uint32_t key;
					float lo[3];
					float hi[3];
				};

				std::vector<simd_item> items;
				items.reserve(total_prims);
				for (auto si = 0u; si < sections.size(); si++)
				{
					const auto& section = sections[si];
					for (auto pi = 0u; pi < section.primitives.size(); pi++)
					{
						// One leaf per PRIMITIVE, not per triangle: in all three stock world
						// blobs the simdTree keys are a bijection onto (section, primitive)
						// with the low bit clear, even though the blobs are mostly quads. The
						// low bit selects the half of a quad only where a key names a triangle,
						// which is what numPrimitiveKeys counts.
						simd_item item{};
						item.key = static_cast<std::uint32_t>(
							(si << KEY_SECTION_SHIFT) | (pi << 1));
						for (auto c = 0; c < 3; c++)
						{
							item.lo[c] = FLT_MAX;
							item.hi[c] = -FLT_MAX;
						}
						for (auto k = 0; k < (section.quads[pi] ? 4 : 3); k++)
						{
							const auto& v = section.verts[section.primitives[pi][k]];
							for (auto c = 0; c < 3; c++)
							{
								item.lo[c] = std::min(item.lo[c], v[c]);
								item.hi[c] = std::max(item.hi[c], v[c]);
							}
						}
						items.emplace_back(item);
					}
				}

				std::vector<simd_node> simd_nodes;
				simd_nodes.reserve(items.size());

				const auto clear_node = [](simd_node& node)
				{
					for (auto c = 0; c < 3; c++)
					{
						for (auto s = 0; s < 4; s++)
						{
							node.lo[c][s] = FLT_MAX;
							node.hi[c][s] = -FLT_MAX;
						}
					}
					node.data[0] = node.data[1] = node.data[2] = node.data[3] = 0;
				};

				simd_nodes.emplace_back();
				clear_node(simd_nodes.back()); // node 0: sentinel

				// Recursive 4-way split on the longest axis. Returns the node index.
				const std::function<int(std::vector<simd_item>&)> build_simd =
					[&](std::vector<simd_item>& group) -> int
				{
					const auto self = static_cast<int>(simd_nodes.size());
					simd_nodes.emplace_back();
					clear_node(simd_nodes.back());

					const auto set_slot = [&](const int slot, const float* lo, const float* hi,
						const std::uint32_t data)
					{
						auto& node = simd_nodes[self];
						for (auto c = 0; c < 3; c++)
						{
							node.lo[c][slot] = lo[c];
							node.hi[c][slot] = hi[c];
						}
						node.data[slot] = data;
					};

					if (group.size() <= 4)
					{
						for (auto i = 0u; i < group.size(); i++)
						{
							set_slot(static_cast<int>(i), group[i].lo, group[i].hi,
								(group[i].key << 1) | 1);
						}
						return self;
					}

					float lo[3] = {FLT_MAX, FLT_MAX, FLT_MAX};
					float hi[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
					for (const auto& item : group)
					{
						for (auto c = 0; c < 3; c++)
						{
							lo[c] = std::min(lo[c], item.lo[c]);
							hi[c] = std::max(hi[c], item.hi[c]);
						}
					}

					auto axis = 0;
					for (auto c = 1; c < 3; c++)
					{
						if ((hi[c] - lo[c]) > (hi[axis] - lo[axis]))
						{
							axis = c;
						}
					}

					std::sort(group.begin(), group.end(),
						[axis](const simd_item& a, const simd_item& b)
						{
							return (a.lo[axis] + a.hi[axis]) < (b.lo[axis] + b.hi[axis]);
						});

					const auto total = group.size();
					for (auto slot = 0; slot < 4; slot++)
					{
						const auto begin = total * slot / 4;
						const auto end = total * (slot + 1) / 4;
						if (begin >= end)
						{
							continue;
						}

						std::vector<simd_item> sub(group.begin() + begin, group.begin() + end);

						float slo[3] = {FLT_MAX, FLT_MAX, FLT_MAX};
						float shi[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
						for (const auto& item : sub)
						{
							for (auto c = 0; c < 3; c++)
							{
								slo[c] = std::min(slo[c], item.lo[c]);
								shi[c] = std::max(shi[c], item.hi[c]);
							}
						}

						if (sub.size() == 1)
						{
							set_slot(slot, slo, shi, (sub[0].key << 1) | 1);
						}
						else
						{
							const auto child = build_simd(sub);
							set_slot(slot, slo, shi, static_cast<std::uint32_t>(child) << 1);
						}
					}

					return self;
				};

				build_simd(items); // becomes node 1

				// ------------------------------------------------------ __data__ pass
				byte_buffer buf;

				struct pending_fixup
				{
					std::size_t src;
					std::size_t dst;
				};
				std::vector<pending_fixup> local_fixups;
				std::vector<std::array<int, 3>> global_fixups;
				std::vector<std::pair<std::size_t, int>> virtual_fixups;

				const auto align16 = [&] { buf.align(16, 0); };

				// The mesh is reached through one of two containers. Slots that must end up
				// pointing at the shape are collected here so the mesh emitter below can patch
				// them without caring which container it is inside.
				std::vector<std::size_t> shape_ptr_slots;
				std::size_t shape_list_offset = 0;

				// Class indices for the virtual fixup table, in the order the name table below
				// writes them.
				const int class_shape = physics_asset ? 2 : 1;
				const int class_shape_data = physics_asset ? 3 : 2;

				// Reported at the end; only the shape-list container actually writes them.
				const auto tag_count = static_cast<int>(palette.size());
				// shapeContents[i] becomes the BODY's collisionFilterInfo (sub_140572EE0 reads
				// it straight into hknpBodyCinfo+12), and the player's trace is a query, which
				// filters on the body before it ever reaches a surface tag. A grenade -- a
				// simulated body, filtered body-vs-body -- bounces off our walls while the
				// player walks through them, so the body filter is the one field in this path
				// that has never been varied. ZT_HAVOK_WORLD_CONTENTS=<hex> overrides it;
				// 0xC7FFBFFF is the engine's "all valid contents" value.
				auto world_contents = WORLD_SHAPE_CONTENTS;
				if (const auto* env = std::getenv("ZT_HAVOK_WORLD_CONTENTS"))
				{
					world_contents = static_cast<std::uint32_t>(std::strtoul(env, nullptr, 16));
					ZONETOOL_INFO("havok: world shape contents overridden to 0x%08X",
						world_contents);
				}

				if (!physics_asset)
				{
					// --- HavokPhysicsShapeList (152 bytes) ---
					// Assigns the OUTER shape_list_offset -- the virtual fixup that registers this
					// object as a HavokPhysicsShapeList is emitted after this block and reads it.
					// A `const auto` here shadowed it and left the fixup pointing at offset 0,
					// which happens to be where the list starts today and would stop being true
					// the moment anything were written before it.
					shape_list_offset = buf.size();

					const std::array<int, 10> member_counts = {
						1, 1, 1, 1, 1, 2, 0, tag_count, 1, 1
					};

					std::array<std::size_t, 10> member_field{};
					for (auto i = 0; i < 6; i++)
					{
						member_field[i] = buf.size();
						write_hk_array_header(buf, member_counts[i]);
					}
					// numWorldGeoShapes is 0 in every shipped blob -- all three stock world
					// blobs, the map-ents blob, and the custom mp_shipment -- even though those
					// files are the world. Setting it to 1 is not what the runtime expects.
					buf.write<std::int32_t>(0); // numWorldGeoShapes
					buf.reserve(4); // pad to +104
					for (auto i = 7; i < 10; i++)
					{
						member_field[i] = buf.size();
						write_hk_array_header(buf, member_counts[i]);
					}

					if (buf.size() - shape_list_offset != 152)
					{
						ZONETOOL_ERROR("havok: HavokPhysicsShapeList is %zu bytes, expected 152",
							buf.size() - shape_list_offset);
						return {};
					}

					align16();

					const auto record_array = [&](const int index, const std::size_t payload)
					{
						local_fixups.push_back({member_field[index], payload});
					};

					const auto shapes_payload = buf.size();
					record_array(0, shapes_payload);
					const auto shape_ptr_slot = buf.reserve(8);
					align16();

					record_array(1, buf.size());
					buf.write<std::int32_t>(0);
					align16();

					// shapeNames is an hkStringPtr: a pointer to the string bytes.
					record_array(2, buf.size());
					const auto name_ptr_slot = buf.reserve(8);
					align16();
					const auto name_payload = buf.size();
					const char* world_name = "World Entity 0";
					buf.write(world_name, std::strlen(world_name) + 1);
					align16();
					local_fixups.push_back({name_ptr_slot, name_payload});

					record_array(3, buf.size());
					buf.write<std::int32_t>(static_cast<std::int32_t>(total_verts));
					align16();
					// The TRIANGLE count, not the primitive count: stock mp_frontend reports 240
					// here against 128 primitives, i.e. quads counted as two.
					record_array(4, buf.size());
					buf.write<std::int32_t>(static_cast<std::int32_t>(num_primitive_keys));
					align16();

					// minMaxes: two hkVector4f, min then max.
					record_array(5, buf.size());
					for (auto i = 0; i < 3; i++) buf.write<float>(world_min[i]);
					buf.write<float>(0.0f);
					for (auto i = 0; i < 3; i++) buf.write<float>(world_max[i]);
					buf.write<float>(0.0f);
					align16();

					record_array(7, buf.size());
					for (auto i = 0u; i < palette.size(); i++)
					{
						// materialId is 0xFFFF in every entry of every shipped file -- stock and
						// custom alike -- i.e. "no explicit material"; the material is identified
						// by materialCRC instead. Writing a real index here points at
						// HavokPhysicsMaterialList slots that may not exist, and an unresolved
						// material is a plausible way for otherwise-valid geometry to end up
						// non-collidable.
						//
						// collisionFilterInfo is the CoD contents mask, carried straight through
						// from cbrush contents: 0x1 solid, 0x800 sky, 0x10000/0x20000 the clip
						// bits. That matches the shipped spread (0x1, 0x2080, 0x30200, 0x30000,
						// 0x400 ...), all of which are contents combinations.
						//
						// materialCRC comes from the source surface -- see the IW5 surface-type
						// table in ClipMapCollision.cpp, which was derived empirically from the
						// materials 2,224 shipped IW7 PhysicsAssets use.
						//
						// userData is IW7's own per-surface word. The low 32 bits are the surface
						// flags a trace hit reports (stock: 0x280000 concrete, 0x680000 thick
						// metal, ...), and bit 48 is the brush basis. The player's movement cast
						// reads that bit on every mesh leaf it touches: the "Collision Dispatcher
						// castShapeConvex" path (0x14058EB70 in the IW7 ship dump) swaps the
						// player's cylinder for the query's separate non-brush shape whenever it
						// is clear. Stock sets it on brush-derived surfaces -- 74 of mp_afghan's
						// 132 world tags, covering 20,321 primitives -- and leaves it clear on
						// terrain and props. Writing 0 everywhere put every converted wall on
						// the non-brush path, and the player walked through them.
						//
						// Bits 32..39 are a third region: a 1-based per-glass-piece index, set
						// on GLASS_PANE and GLASS_SOLID tags and nowhere else. It arrives
						// already packed into the triangle's user_data.
						buf.write<std::uint32_t>(
							filter_contents(static_cast<std::uint32_t>(std::get<0>(palette[i]))));
						buf.write<std::uint32_t>(std::get<1>(palette[i]));
						buf.write<std::uint16_t>(0xFFFF); // materialId -- "none"
						buf.reserve(6); // pad, userData is at +16
						buf.write<std::uint64_t>(std::get<2>(palette[i]));
					}
					align16();

					// The world shape's contents mask is a FIXED value, not a function of the
					// map's geometry. All three shipped world blobs use essentially the same one
					// -- 0x29033ED1 (afghan), 0x28033ED7 (paris), 0x28033ED1 (breakneck) --
					// regardless of what the map actually contains.
					//
					// Deriving it by OR-ing the source triangles' contents looked reasonable and
					// was wrong: on mp_test_h1 it produced 0x08031E41, missing 0x20002090
					// relative to the shipped mask. The world body then failed to collide with
					// the player while bullet traces still hit, because the dropped bits are part
					// of what the player query filters on. An OR of IW3/IW5 brush contents can
					// only ever be a subset of what IW7 expects here.

					record_array(8, buf.size());
					buf.write<std::uint32_t>(world_contents);
					align16();
					record_array(9, buf.size());
					buf.write<std::int32_t>(0); // convexCounts
					align16();
					shape_ptr_slots.push_back(shape_ptr_slot);
				}
				else
				{
					// --- HavokPhysicsAsset (144) ---
					// Same wrapper build_physics_asset writes for the shipped dummies, which is
					// byte-identical to stock; only the shape it ends in differs. Reference
					// layout taken from cp_zmb's machinery_generator_portable_01.hkx, one of
					// the 16 stock assets that use exactly this minimal object set.
					virtual_fixups.emplace_back(static_cast<std::size_t>(0), 0);
					buf.reserve(8); // isRagdoll + pad
					const auto system_data_slot = buf.reserve(8);

					std::array<std::size_t, 8> lookup_field{};
					const int lookup_count[8] = {1, 1, 0, 1, 1, 1, 0, 1};
					for (auto i = 0; i < 8; i++)
					{
						lookup_field[i] = buf.size();
						write_hk_array_header(buf, lookup_count[i]);
					}
					if (buf.size() != 144)
					{
						ZONETOOL_ERROR("havok: HavokPhysicsAsset is %zu bytes, expected 144",
							buf.size());
						return {};
					}

					const std::uint32_t lookup_value[8] = {
						physics_asset->body_quality_crc, physics_asset->material_crc,
						0, 0, 0, 0, 0, 0
					};
					for (auto i = 0; i < 8; i++)
					{
						if (!lookup_count[i]) continue;
						local_fixups.push_back({lookup_field[i], buf.size()});
						buf.write<std::uint32_t>(lookup_value[i]);
						align16();
					}

					// --- hknpPhysicsSystemData (120) ---
					const auto system_data_offset = buf.size();
					global_fixups.push_back({static_cast<int>(system_data_slot), 2,
						static_cast<int>(system_data_offset)});
					virtual_fixups.emplace_back(system_data_offset, 1);

					buf.reserve(16); // hkReferencedObject
					write_hk_array_header(buf, 0); // materials
					write_hk_array_header(buf, 0); // motionProperties
					write_hk_array_header(buf, 0); // motionCinfos
					const auto body_cinfos_field = buf.size();
					write_hk_array_header(buf, 1); // bodyCinfos
					write_hk_array_header(buf, 0); // constraintCinfos
					const auto referenced_field = buf.size();
					write_hk_array_header(buf, 1); // referencedObjects
					const auto system_name_slot = buf.reserve(8);
					if (buf.size() - system_data_offset != 120)
					{
						ZONETOOL_ERROR("havok: hknpPhysicsSystemData is %zu bytes, expected 120",
							buf.size() - system_data_offset);
						return {};
					}
					align16();

					// --- hknpBodyCinfo (160) ---
					const auto body_offset = buf.size();
					local_fixups.push_back({body_cinfos_field, body_offset});
					const auto body_shape_slot = buf.reserve(8);
					buf.write<std::int32_t>(0);            // flags
					buf.write<std::uint32_t>(physics_asset->body_contents); // collisionFilterInfo
					buf.write<std::uint16_t>(0xFFFF);      // materialId
					buf.write<std::uint8_t>(0xFF);         // qualityId
					buf.reserve(5);
					buf.write<std::uint64_t>(0);           // userData
					const auto body_name_slot = buf.reserve(8);
					buf.write<std::uint8_t>(0);            // motionType -- static
					buf.reserve(7);
					for (auto i = 0; i < 2; i++)
					{
						buf.write<float>(0.0f); buf.write<float>(0.0f);
						buf.write<float>(0.0f); buf.write<float>(1.0f);
					}
					buf.reserve(32);
					buf.write<float>(-1.0f);
					buf.reserve(12);
					buf.write<std::uint16_t>(0xFFFF);
					buf.write<std::uint16_t>(0x0000);
					buf.write<std::uint32_t>(0x7FFFFFFFu);
					buf.write<std::uint32_t>(0x7FFFFFFFu);
					buf.write<std::uint32_t>(0);
					buf.reserve(16);
					if (buf.size() - body_offset != 160)
					{
						ZONETOOL_ERROR("havok: hknpBodyCinfo is %zu bytes, expected 160",
							buf.size() - body_offset);
						return {};
					}

					local_fixups.push_back({body_name_slot, buf.size()});
					buf.write(physics_asset->body_name.c_str(),
						physics_asset->body_name.size() + 1);
					align16();

					local_fixups.push_back({referenced_field, buf.size()});
					const auto referenced_slot = buf.reserve(8);
					align16();

					local_fixups.push_back({system_name_slot, buf.size()});
					const char* system_name = "Default Physics System Data";
					buf.write(system_name, std::strlen(system_name) + 1);
					align16();

					// Both the body and the referencedObjects entry point at the mesh.
					shape_ptr_slots.push_back(body_shape_slot);
					shape_ptr_slots.push_back(referenced_slot);
				}

				// --- hknpCompressedMeshShape (160 bytes) ---
				const auto shape_offset = buf.size();
				if (!physics_asset)
				{
					virtual_fixups.emplace_back(shape_list_offset, 0);
				}
				virtual_fixups.emplace_back(shape_offset, class_shape);

				for (const auto slot : shape_ptr_slots)
				{
					global_fixups.push_back({static_cast<int>(slot), 2,
						static_cast<int>(shape_offset)});
				}

				buf.reserve(16); // hkReferencedObject
				buf.write<std::uint16_t>(4); // flags
				buf.write<std::uint8_t>(static_cast<std::uint8_t>(bits_per_key)); // numShapeKeyBits
				buf.write<std::uint8_t>(2); // dispatchType
				buf.write<float>(input.convex_radius);
				buf.write<std::uint64_t>(0); // userData
				buf.reserve(8); // properties*
				// hknpShape's objectSize is 48 but its last reflected member ends at +40;
				// the tail padding is part of the object and has to be written.
				buf.reserve(8);
				buf.write<std::uint32_t>(0xFFFFFFFF); // edgeWeldingMap.secondaryKeyMask
				buf.write<std::uint32_t>(0); // edgeWeldingMap.sencondaryKeyBits
				write_hk_array_header(buf, 0); // edgeWeldingMap.primaryKeyToIndex
				write_hk_array_header(buf, 0); // edgeWeldingMap.valueAndSecondaryKeys
				buf.write<std::uint32_t>(0xFFFFFFFF); // shapeTagCodecInfo
				buf.reserve(4); // pad to +96
				const auto shape_data_slot = buf.reserve(8); // data*

				// Both bitfields are addressed by primitive key, so they have to span the
				// whole key space rather than just the primitive count -- shipped files use
				// numBits == maxKey + 1 for triangleIsInterior and half that for quadIsFlat.
				const auto interior_bits = max_key + 1;
				const auto quad_bits = interior_bits / 2;
				const auto interior_words = (interior_bits + 31) / 32;
				const auto quad_words = (quad_bits + 31) / 32;

				const auto quad_field = buf.size();
				write_hk_array_header(buf, quad_words);
				buf.write<std::int32_t>(quad_bits);
				buf.reserve(4);
				const auto interior_field = buf.size();
				write_hk_array_header(buf, interior_words);
				buf.write<std::int32_t>(interior_bits);
				buf.reserve(4);
				buf.write<std::int32_t>(0); // numTriangles    (SERIALIZE_IGNORED)
				buf.write<std::int32_t>(0); // numConvexShapes (SERIALIZE_IGNORED)

				if (buf.size() - shape_offset != SIZEOF_COMPRESSED_MESH_SHAPE)
				{
					ZONETOOL_ERROR("havok: hknpCompressedMeshShape is %zu bytes, expected %d",
						buf.size() - shape_offset, SIZEOF_COMPRESSED_MESH_SHAPE);
					return {};
				}
				align16();

				// quadIsFlat is indexed by primitive slot -- key >> 1 -- and means the fourth
				// corner lies exactly on the plane of the first three. Measured on stock
				// mp_afghan: all 1,698 quads with the bit set have a deviation of 0.0, while
				// the 4,748 without it deviate by up to 10 units, so this is a statement about
				// the geometry rather than a hint.
				//
				// It has to be judged on the DECODED positions. A face of a brush is planar by
				// construction, but 21/21/22-bit quantisation bends it, and claiming a bent
				// quad is flat hands the runtime a shape that does not match its own vertices --
				// which is what made collision worse when quads were first tried.
				std::vector<std::uint32_t> quad_bitfield(quad_words, 0);
				for (auto si = 0u; si < sections.size(); si++)
				{
					const auto& section = sections[si];
					for (auto pi = 0u; pi < section.primitives.size(); pi++)
					{
						if (!section.quads[pi])
						{
							continue;
						}

						float corner[4][3];
						for (auto k = 0; k < 4; k++)
						{
							const auto& v = section.verts[section.primitives[pi][k]];
							unpack_shared_vertex(
								pack_shared_vertex(v.data(), world_min, world_max),
								world_min, world_max, corner[k]);
						}

						float e1[3], e2[3], n[3];
						for (auto c = 0; c < 3; c++)
						{
							e1[c] = corner[1][c] - corner[0][c];
							e2[c] = corner[2][c] - corner[0][c];
						}
						n[0] = e1[1] * e2[2] - e1[2] * e2[1];
						n[1] = e1[2] * e2[0] - e1[0] * e2[2];
						n[2] = e1[0] * e2[1] - e1[1] * e2[0];

						const auto length = std::sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
						if (length < 1e-12f)
						{
							continue;
						}

						auto deviation = 0.0f;
						for (auto c = 0; c < 3; c++)
						{
							deviation += (corner[3][c] - corner[0][c]) * n[c] / length;
						}

						// Stock's flat quads deviate by exactly zero; allow only the last bits
						// of float error on top of that.
						if (std::fabs(deviation) > 1e-5f)
						{
							continue;
						}

						const auto bit = static_cast<std::size_t>(
							((si << KEY_SECTION_SHIFT) | (pi << 1)) >> 1);
						if (bit / 32 < quad_bitfield.size())
						{
							quad_bitfield[bit / 32] |= 1u << (bit % 32);
						}
					}
				}

				local_fixups.push_back({quad_field, buf.size()});
				for (const auto word : quad_bitfield)
				{
					buf.write<std::uint32_t>(word);
				}
				align16();
				local_fixups.push_back({interior_field, buf.size()});
				buf.fill(static_cast<std::size_t>(interior_words) * 4, 0);
				align16();

				// --- hknpCompressedMeshShapeData (256 bytes) ---
				const auto data_offset = buf.size();
				virtual_fixups.emplace_back(data_offset, class_shape_data);
				global_fixups.push_back({static_cast<int>(shape_data_slot), 2,
					static_cast<int>(data_offset)});

				buf.reserve(16); // hkReferencedObject
				const auto tree_offset = buf.size();

				const auto tree_nodes_field = buf.size();
				write_hk_array_header(buf, static_cast<int>(top_nodes.size() / SIZEOF_NODE_TOP));
				for (auto i = 0; i < 3; i++) buf.write<float>(world_min[i]);
				buf.write<float>(0.0f);
				for (auto i = 0; i < 3; i++) buf.write<float>(world_max[i]);
				buf.write<float>(0.0f);
				buf.write<std::int32_t>(num_primitive_keys);
				buf.write<std::int32_t>(bits_per_key);
				buf.write<std::uint32_t>(static_cast<std::uint32_t>(max_key));
				buf.reserve(4);
				const auto tree_sections_field = buf.size();
				write_hk_array_header(buf, section_count);
				const auto tree_primitives_field = buf.size();
				write_hk_array_header(buf, static_cast<int>(total_prims));
				const auto tree_svi_field = buf.size();
				write_hk_array_header(buf, static_cast<int>(shared_vertex_index.size() / 2));
				const auto tree_packed_field = buf.size();
				write_hk_array_header(buf, 0);
				const auto tree_shared_field = buf.size();
				write_hk_array_header(buf, static_cast<int>(shared_vertices.size() / 8));
				const auto tree_runs_field = buf.size();
				write_hk_array_header(buf, static_cast<int>(data_runs.size() / SIZEOF_DATA_RUN));
				(void)tree_packed_field;

				if (buf.size() - tree_offset != SIZEOF_MESH_TREE)
				{
					ZONETOOL_ERROR("havok: mesh tree is %zu bytes, expected %d",
						buf.size() - tree_offset, SIZEOF_MESH_TREE);
					return {};
				}

				// simdTree (24) then connectivity (48). Connectivity stays empty -- every
				// shipped world blob has it empty too.
				buf.reserve(8); // hkBaseObject
				const auto simd_field = buf.size();
				write_hk_array_header(buf, static_cast<int>(simd_nodes.size()));
				write_hk_array_header(buf, 0); // connectivity.headers
				write_hk_array_header(buf, 0); // connectivity.localLinks
				write_hk_array_header(buf, 0); // connectivity.globalLinks
				// connectivity ends at +248; objectSize is 256.
				buf.reserve(8);

				if (buf.size() - data_offset != SIZEOF_COMPRESSED_MESH_SHAPE_DATA)
				{
					ZONETOOL_ERROR("havok: hknpCompressedMeshShapeData is %zu bytes, expected %d",
						buf.size() - data_offset, SIZEOF_COMPRESSED_MESH_SHAPE_DATA);
					return {};
				}
				align16();

				// --- simd tree payload ---
				local_fixups.push_back({simd_field, buf.size()});
				for (const auto& node : simd_nodes)
				{
					// hkcdFourAabb is SoA: lx, hx, ly, hy, lz, hz.
					for (auto c = 0; c < 3; c++)
					{
						buf.write(node.lo[c], sizeof(node.lo[c]));
						buf.write(node.hi[c], sizeof(node.hi[c]));
					}
					buf.write(node.data, sizeof(node.data));
				}
				align16();

				// --- mesh tree payloads ---
				local_fixups.push_back({tree_nodes_field, buf.size()});
				buf.write(top_nodes.data.data(), top_nodes.size());
				align16();

				local_fixups.push_back({tree_sections_field, buf.size()});
				std::vector<std::size_t> section_node_field(sections.size());
				for (auto i = 0u; i < sections.size(); i++)
				{
					const auto& section = sections[i];
					const auto base = buf.size();

					section_node_field[i] = buf.size();
					write_hk_array_header(buf,
						static_cast<int>(section_nodes[i].size() / SIZEOF_NODE_SECTION));
					for (auto c = 0; c < 3; c++) buf.write<float>(section.mins[c]);
					buf.write<float>(0.0f);
					for (auto c = 0; c < 3; c++) buf.write<float>(section.maxs[c]);
					buf.write<float>(0.0f);
					// A section with no packed vertices writes this sentinel codec, exactly
					// as every shipped numPackedVertices == 0 section does.
					for (auto c = 0; c < 3; c++) buf.write<float>(FLT_MAX);
					for (auto c = 0; c < 3; c++) buf.write<float>(-INFINITY);
					buf.write<std::uint32_t>(0); // firstPackedVertex
					// sharedVertices packs (firstSharedVertexIndex << 8) | numPackedVertices.
					buf.write<std::uint32_t>(
						static_cast<std::uint32_t>(section_first_vert[i] << 8));
					buf.write<std::uint32_t>(static_cast<std::uint32_t>(
						(section_first_prim[i] << 8) | section.primitives.size()));
					buf.write<std::uint32_t>(static_cast<std::uint32_t>(
						(section_first_run[i] << 8) | section_run_count[i]));
					buf.write<std::uint8_t>(0); // numPackedVertices
					buf.write<std::uint8_t>(static_cast<std::uint8_t>(section.verts.size()));
					buf.write<std::uint16_t>(static_cast<std::uint16_t>(leaf_nodes[i])); // leafIndex
					// Which 65,536-vertex page of sharedVertices this section's indices are
					// relative to. See the paging note where sharedVerticesIndex is built.
					buf.write<std::uint8_t>(static_cast<std::uint8_t>(section_page[i]));
					buf.write<std::uint8_t>(0); // flags
					buf.write<std::uint8_t>(0); // layerData
					buf.write<std::uint8_t>(0); // unusedData

					if (buf.size() - base != SIZEOF_SECTION)
					{
						ZONETOOL_ERROR("havok: section is %zu bytes, expected %d",
							buf.size() - base, SIZEOF_SECTION);
						return {};
					}
				}
				align16();

				for (auto i = 0u; i < sections.size(); i++)
				{
					local_fixups.push_back({section_node_field[i], buf.size()});
					buf.write(section_nodes[i].data.data(), section_nodes[i].size());
					align16();
				}

				local_fixups.push_back({tree_primitives_field, buf.size()});
				buf.write(primitives.data.data(), primitives.size());
				align16();

				local_fixups.push_back({tree_svi_field, buf.size()});
				buf.write(shared_vertex_index.data.data(), shared_vertex_index.size());
				align16();

				local_fixups.push_back({tree_shared_field, buf.size()});
				buf.write(shared_vertices.data.data(), shared_vertices.size());
				align16();

				local_fixups.push_back({tree_runs_field, buf.size()});
				buf.write(data_runs.data.data(), data_runs.size());
				align16();

				const auto data_size = buf.size();

				// ------------------------------------------------------ class names
				byte_buffer names;
				const auto write_name = [&](const std::uint32_t sig, const char* name)
				{
					names.write<std::uint32_t>(sig);
					names.write<std::uint8_t>(0x09);
					const auto offset = names.size();
					names.write(name, std::strlen(name) + 1);
					return offset;
				};

				write_name(SIG_HK_CLASS, "hkClass");
				write_name(SIG_HK_CLASS_MEMBER, "hkClassMember");
				write_name(SIG_HK_CLASS_ENUM, "hkClassEnum");
				write_name(SIG_HK_CLASS_ENUM_ITEM, "hkClassEnumItem");

				std::array<std::size_t, 4> name_offsets{};
				if (!physics_asset)
				{
					name_offsets[0] = write_name(SIG_SHAPE_LIST, "HavokPhysicsShapeList");
					name_offsets[1] = write_name(SIG_COMPRESSED_MESH_SHAPE,
						"hknpCompressedMeshShape");
					name_offsets[2] = write_name(SIG_COMPRESSED_MESH_SHAPE_DATA,
						"hknpCompressedMeshShapeData");
				}
				else
				{
					name_offsets[0] = write_name(SIG_PHYSICS_ASSET, "HavokPhysicsAsset");
					name_offsets[1] = write_name(SIG_PHYSICS_SYSTEM_DATA,
						"hknpPhysicsSystemData");
					name_offsets[2] = write_name(SIG_COMPRESSED_MESH_SHAPE,
						"hknpCompressedMeshShape");
					name_offsets[3] = write_name(SIG_COMPRESSED_MESH_SHAPE_DATA,
						"hknpCompressedMeshShapeData");
				}
				names.align(16, 0xFF);

				// ------------------------------------------------------ fixup tables
				// Local fixups are ordered by destination, not source: true in all nine
				// shipped files checked (world, ents and per-model). Order does not change
				// what the loader does, but matching stock keeps generated files comparable.
				std::sort(local_fixups.begin(), local_fixups.end(),
					[](const pending_fixup& a, const pending_fixup& b)
					{
						return a.dst < b.dst;
					});

				// Each fixup table is padded out to a 16-byte boundary with 0xFF, so every
				// section offset in the header lands aligned. All nine shipped files checked
				// -- world, ents and per-model -- do this, with 0..12 bytes of slack between
				// tables. The padding counts towards the preceding table's size.
				byte_buffer fixups;
				for (const auto& fixup : local_fixups)
				{
					fixups.write<std::int32_t>(static_cast<std::int32_t>(fixup.src));
					fixups.write<std::int32_t>(static_cast<std::int32_t>(fixup.dst));
				}
				fixups.align(16, 0xFF);
				const auto local_size = fixups.size();

				std::sort(global_fixups.begin(), global_fixups.end(),
					[](const std::array<int, 3>& a, const std::array<int, 3>& b)
					{
						return a[0] < b[0];
					});
				for (const auto& fixup : global_fixups)
				{
					fixups.write<std::int32_t>(fixup[0]);
					fixups.write<std::int32_t>(fixup[1]);
					fixups.write<std::int32_t>(fixup[2]);
				}
				fixups.align(16, 0xFF);
				const auto global_size = fixups.size() - local_size;

				for (const auto& fixup : virtual_fixups)
				{
					fixups.write<std::int32_t>(static_cast<std::int32_t>(fixup.first));
					fixups.write<std::int32_t>(0);
					fixups.write<std::int32_t>(static_cast<std::int32_t>(name_offsets[fixup.second]));
				}
				fixups.align(16, 0xFF);
				const auto virtual_size = fixups.size() - local_size - global_size;

				// ------------------------------------------------------------ output
				byte_buffer file;
				file.write<std::uint32_t>(HK_MAGIC0);
				file.write<std::uint32_t>(HK_MAGIC1);
				file.write<std::int32_t>(0); // userTag
				file.write<std::int32_t>(HK_FILE_VERSION);
				file.write<std::uint8_t>(8); // pointerSize
				file.write<std::uint8_t>(1); // littleEndian
				file.write<std::uint8_t>(0); // reuseBaseClassPadding
				file.write<std::uint8_t>(1); // emptyBaseClassOptimization
				file.write<std::int32_t>(3); // numSections
				file.write<std::int32_t>(2); // contentsSectionIndex
				file.write<std::int32_t>(0); // contentsSectionOffset
				file.write<std::int32_t>(0); // contentsClassNameSectionIndex
				file.write<std::int32_t>(static_cast<std::int32_t>(name_offsets[0]));
				const char* version = "hk_2014.2.5-r1";
				const auto version_length = std::strlen(version) + 1;
				file.write(version, version_length);
				// hkPackfileHeader() memsets itself to -1 before assigning fields, so the
				// tail of contentsVersion stays 0xFF rather than 0.
				file.fill(16 - version_length, 0xFF);
				file.write<std::int32_t>(0); // flags
				file.write<std::uint16_t>(HK_MAX_PREDICATE);
				file.write<std::uint16_t>(0);

				const auto names_start = HK_HEADER_SIZE + 3 * HK_SECTION_HEADER_SIZE;
				const auto data_start = names_start + names.size();

				const auto write_section_header = [&](const char* tag, const std::size_t abs,
					const std::size_t payload, const std::size_t local, const std::size_t global,
					const std::size_t virt)
				{
					char name[19] = {};
					std::strncpy(name, tag, sizeof(name));
					file.write(name, sizeof(name));
					file.write<std::uint8_t>(0xFF); // m_nullByte
					file.write<std::int32_t>(static_cast<std::int32_t>(abs));
					file.write<std::int32_t>(static_cast<std::int32_t>(payload));
					file.write<std::int32_t>(static_cast<std::int32_t>(payload + local));
					file.write<std::int32_t>(static_cast<std::int32_t>(payload + local + global));
					const auto end = payload + local + global + virt;
					file.write<std::int32_t>(static_cast<std::int32_t>(end));
					file.write<std::int32_t>(static_cast<std::int32_t>(end));
					file.write<std::int32_t>(static_cast<std::int32_t>(end));
					file.fill(16, 0xFF); // m_pad[4]
				};

				write_section_header("__classnames__", names_start, names.size(), 0, 0, 0);
				write_section_header("__types__", data_start, 0, 0, 0, 0);
				write_section_header("__data__", data_start, data_size, local_size, global_size,
					virtual_size);

				file.write(names.data.data(), names.size());
				file.write(buf.data.data(), buf.size());
				file.write(fixups.data.data(), fixups.size());

				// DIAGNOSTIC: every shapeTagData entry as it was written -- the filtered
				// collisionFilterInfo, the material CRC and the full userData word.
				for (auto i = 0u; i < palette.size(); i++)
				{
					ZONETOOL_INFO("  tag %2u  filter 0x%08X (raw 0x%08X)  crc 0x%08X  "
						"userData 0x%016llX", i,
						filter_contents(static_cast<std::uint32_t>(std::get<0>(palette[i]))),
						static_cast<std::uint32_t>(std::get<0>(palette[i])),
						std::get<1>(palette[i]),
						static_cast<unsigned long long>(std::get<2>(palette[i])));
				}

				ZONETOOL_INFO("havok: %s built -- %zu triangles, %d sections, %d surface tags, "
					"contents 0x%08X, %zu bytes",
					physics_asset ? "model physics asset" : "world shape",
					input.triangles.size(), section_count, tag_count, world_contents,
					file.size());

				return file.data;
			}

			// -------------------------------------------------- map-ents shape list
			//
			// The ents blob is the same container and the same root class as the world blob,
			// but its shape list is per-entity rather than one big mesh: one
			// hknpDynamicCompoundShape of hknpConvexPolytopeShapes per brush model, indexed
			// by physicsShapeOverrideIdx. None of the world blob's hard parts appear here --
			// no vertex quantisation, no static BVH codec -- so this is a direct write of the
			// layouts decoded in docs/iw7-ents-shapes.md.
			//
			// Object order inside __data__ matches shipped blobs exactly, per shape:
			//
			//   compound (208) | instances (128*L) | for each convex { object (96) | verts |
			//   planes | faces | indices | connectivity (48) | vertexEdges | faceLinks } |
			//   compoundData (56) | tree nodes (32 * (2L+1))
			//
			// with every array payload 16-byte aligned.
			std::vector<std::uint8_t> build_world_shape(const mesh_input& input)
			{
				return build_mesh_blob(input, nullptr);
			}

			std::vector<std::uint8_t> build_model_physics_asset(const mesh_input& input,
				const physics_asset_input& physics_asset)
			{
				return build_mesh_blob(input, &physics_asset);
			}

			std::vector<std::uint8_t> build_ents_shape_list(const ents_input& input)
			{
				// A half-edge, addressed the way Havok does it: which face it belongs to and
				// its position in that face's index run.
				struct half_edge
				{
					std::uint16_t face = 0;
					std::uint8_t edge = 0;
				};

				struct tree_node
				{
					float mn[3] = {0.0f, 0.0f, 0.0f};
					float mx[3] = {0.0f, 0.0f, 0.0f};
					std::uint16_t parent = 0;
					std::uint32_t data = 0;
				};

				byte_buffer buf;
				// Havok splits pointer patches by what they point at, and shipped files are
				// completely consistent about it: a pointer to a registered object (anything
				// with a virtual fixup) is a GLOBAL fixup, and a pointer to raw payload --
				// an array's elements, a string's bytes -- is a LOCAL one. Measured across
				// the stock blobs: 532/532 global fixups land on an object, 0/1420 local
				// ones do. Both resolve to the same address inside one section, but the
				// loader uses the distinction to track objects, so match it.
				std::vector<std::pair<std::size_t, std::size_t>> local_fixups;
				std::vector<std::pair<std::size_t, std::size_t>> global_fixups;
				// (object offset, class-name index) -- the virtual fixup table is the only
				// thing that tells the loader which class each object is.
				std::vector<std::pair<std::size_t, int>> virtual_fixups;

				const auto align16 = [&] { buf.align(16, 0); };

				// The caller stores a slot index per brush model before the blob is built, so
				// shapes are written one-for-one with the input. Dropping one here would
				// silently shift every index after it; refuse instead.
				std::vector<const ents_shape*> shapes;
				for (const auto& shape : input.shapes)
				{
					if (shape.convexes.empty())
					{
						ZONETOOL_ERROR("havok: ents shape %zu (\"%s\") has no convexes -- "
							"writing it would shift every later shape index",
							shapes.size(), shape.name.c_str());
						return {};
					}
					shapes.emplace_back(&shape);
				}

				const auto shape_count = static_cast<int>(shapes.size());

				// One ShapeTagData per distinct (contents, materialCRC, userData). materialId
				// stays 0xFFFF ("none") as in every shipped entry, with the material identified
				// by materialCRC, which Physics_AddShapeList resolves against the game's CRC
				// table at load.
				//
				// The CRC and userData used to be constants here -- DEFAULT_MATERIAL_CRC and
				// the brush-basis bit on its own -- so every brush model and trigger came out
				// as untyped concrete. Stock ents blobs look nothing like that: mp_afghan
				// carries 12 distinct CRCs over 136 tags with surface flags set on 135 of
				// them, cp_zmb 16 over 476 with 472 flagged. Losing the flags also loses the
				// gameplay bits that ride in the low 32 -- LADDER, SLICK, NOPENETRATE, STAIRS,
				// MANTLEON -- on exactly the geometry (doors, hatches, ladders, moving
				// platforms) most likely to need them.
				using ents_tag = std::tuple<int, std::uint32_t, std::uint64_t>;
				std::vector<ents_tag> palette;
				const auto tag_for = [&](const ents_shape& shape)
				{
					const ents_tag key{shape.contents, shape.material_crc, shape.user_data};
					for (auto i = 0u; i < palette.size(); i++)
					{
						if (palette[i] == key)
						{
							return static_cast<std::uint16_t>(i);
						}
					}
					palette.emplace_back(key);
					return static_cast<std::uint16_t>(palette.size() - 1);
				};

				// Keep at least one tag: the material table the runtime registers should not
				// be empty even when there is no geometry.
				if (shapes.empty())
				{
					palette.emplace_back(ents_tag{1, DEFAULT_MATERIAL_CRC, 0});
				}
				else
				{
					for (const auto* shape : shapes)
					{
						tag_for(*shape);
					}
				}

				// --- HavokPhysicsShapeList (152 bytes) ---
				// shapes, shapeIndices, shapeNames, vertCounts, triCounts, minMaxes at
				// +0..+80; numWorldGeoShapes at +96; shapeTagData, shapeContents and
				// convexCounts at +104..+136.
				const std::array<int, 10> member_counts = {
					shape_count, shape_count, shape_count, shape_count, shape_count,
					shape_count * 2, 0, static_cast<int>(palette.size()), shape_count,
					shape_count
				};

				std::array<std::size_t, 10> member_field{};
				for (auto i = 0; i < 6; i++)
				{
					member_field[i] = buf.size();
					write_hk_array_header(buf, member_counts[i]);
				}
				buf.write<std::int32_t>(0); // numWorldGeoShapes -- 0 in every shipped blob
				buf.reserve(4); // pad to +104
				for (auto i = 7; i < 10; i++)
				{
					member_field[i] = buf.size();
					write_hk_array_header(buf, member_counts[i]);
				}

				if (buf.size() != 152)
				{
					ZONETOOL_ERROR("havok: ents HavokPhysicsShapeList is %zu bytes, expected 152",
						buf.size());
					return {};
				}

				virtual_fixups.emplace_back(static_cast<std::size_t>(0), 0); // root at +0
				align16();

				const auto record_array = [&](const int index, const std::size_t payload)
				{
					// An empty hkArray keeps m_data null and takes no fixup, which is what
					// the shipped stub blobs do.
					if (member_counts[index] > 0)
					{
						local_fixups.emplace_back(member_field[index], payload);
					}
				};

				// shapes: one pointer per shape, patched once the objects are laid out.
				std::vector<std::size_t> shape_ptr_slots;
				record_array(0, buf.size());
				for (auto i = 0; i < shape_count; i++)
				{
					shape_ptr_slots.emplace_back(buf.reserve(8));
				}
				align16();

				record_array(1, buf.size());
				for (auto i = 0; i < shape_count; i++)
				{
					buf.write<std::int32_t>(i); // shapeIndices is the identity in stock data
				}
				align16();

				// shapeNames is an hkArray<hkStringPtr>: a pointer array, then the bytes.
				record_array(2, buf.size());
				std::vector<std::size_t> name_ptr_slots;
				for (auto i = 0; i < shape_count; i++)
				{
					name_ptr_slots.emplace_back(buf.reserve(8));
				}
				align16();
				for (auto i = 0; i < shape_count; i++)
				{
					local_fixups.emplace_back(name_ptr_slots[i], buf.size());
					const auto& name = shapes[i]->name;
					buf.write(name.c_str(), name.size() + 1);

					// hkStringPtr keeps its "I allocated this, free it" flag in bit 0 of the
					// pointer itself -- hkStringPtr::operator= tests `ptr & 1` and calls
					// hkMemoryRouter::easyFree(ptr - 1), and IW7 masks it off with `ptr & ~1`
					// wherever it reads one. Section data starts 16-aligned, so a name written
					// at an odd offset becomes a pointer with bit 0 set, and unloading the zone
					// hands that address -- which is inside the fastfile, not the Havok heap --
					// to hkLargeBlockAllocator::blockFree. It reads the neighbouring name bytes
					// as a block header and dereferences a garbage free-tree node.
					//
					// Names are variable length, so writing them back to back puts every other
					// one on an odd offset. Pad to even. No stock string sits at an odd offset:
					// 992 of 992 in mp_paris's ents blob are even.
					if (buf.size() & 1)
					{
						buf.write<std::uint8_t>(0);
					}
				}
				align16();

				record_array(3, buf.size());
				for (const auto* shape : shapes)
				{
					auto verts = 0;
					for (const auto& convex : shape->convexes)
					{
						verts += static_cast<int>(padded_vertex_count(convex.verts.size()));
					}
					buf.write<std::int32_t>(verts);
				}
				align16();

				record_array(4, buf.size());
				for (auto i = 0; i < shape_count; i++)
				{
					buf.write<std::int32_t>(0); // triCounts -- 0 for convex shapes
				}
				align16();

				// minMaxes is two hkVector4f per shape and reads all-zero in every shipped
				// ents blob, so leave it zeroed rather than inventing values.
				record_array(5, buf.size());
				buf.reserve(static_cast<std::size_t>(shape_count) * 32);
				align16();

				record_array(7, buf.size());
				for (const auto& entry : palette)
				{
					buf.write<std::uint32_t>(
						filter_contents(static_cast<std::uint32_t>(std::get<0>(entry))));
					buf.write<std::uint32_t>(std::get<1>(entry)); // materialCRC
					buf.write<std::uint16_t>(0xFFFF); // materialId -- resolved at load
					buf.reserve(6);
					buf.write<std::uint64_t>(std::get<2>(entry)); // userData
				}
				align16();

				// shapeContents is what CM_ContentsOfBrushModel hands back as the entity's
				// contents mask, so it has to be the real CoD mask, not a wildcard.
				record_array(8, buf.size());
				for (const auto* shape : shapes)
				{
					buf.write<std::uint32_t>(
						filter_contents(shape->entity_contents));
				}
				align16();

				record_array(9, buf.size());
				for (const auto* shape : shapes)
				{
					buf.write<std::int32_t>(static_cast<std::int32_t>(shape->convexes.size()));
				}
				align16();

				// --- one compound per shape ---
				for (auto s = 0; s < shape_count; s++)
				{
					const auto& shape = *shapes[s];
					const auto instance_count = static_cast<int>(shape.convexes.size());

					const auto compound_offset = buf.size();
					global_fixups.emplace_back(shape_ptr_slots[s], compound_offset);
					virtual_fixups.emplace_back(compound_offset, 1);

					// hknpShape header.
					buf.reserve(16); // hkBaseObject vtable + hkReferencedObject
					buf.write<std::uint16_t>(static_cast<std::uint16_t>(COMPOUND_SHAPE_FLAGS));
					// numShapeKeyBits is the bit length of the instance count -- 1 for one
					// instance, 2 for two or three, 3 for four to seven. Checked against all
					// 644 compounds in the shipped ents blobs.
					auto key_bits = 0;
					for (auto n = instance_count; n > 0; n >>= 1)
					{
						key_bits++;
					}
					buf.write<std::uint8_t>(static_cast<std::uint8_t>(key_bits));
					buf.write<std::uint8_t>(static_cast<std::uint8_t>(COMPOUND_DISPATCH_TYPE));
					buf.write<float>(0.0f); // convexRadius
					buf.write<std::uint64_t>(0); // userData
					buf.reserve(8); // properties
					buf.reserve(8); // pad -- hknpShape is 48 bytes, its fields end at 40
					// hknpCompositeShape: edgeWeldingMap (40 bytes, empty) then
					// shapeTagCodecInfo. An empty map still carries secondaryKeyMask = ~0.
					buf.write<std::uint32_t>(0xFFFFFFFFu); // secondaryKeyMask
					buf.write<std::uint32_t>(0); // secondaryKeyBits
					write_hk_array_header(buf, 0); // primaryKeyToIndex
					write_hk_array_header(buf, 0); // valueAndSecondaryKeys
					buf.write<std::uint32_t>(0xFFFFFFFFu); // shapeTagCodecInfo
					buf.reserve(4); // pad to +96

					if (buf.size() - compound_offset != 96)
					{
						ZONETOOL_ERROR("havok: compound header is %zu bytes, expected 96",
							buf.size() - compound_offset);
						return {};
					}

					// hkFreeListArray<hknpShapeInstance> at +96: the array, then firstFree,
					// padded out to its 24-byte object size.
					const auto instances_field = buf.size();
					write_hk_array_header(buf, instance_count);
					buf.write<std::int32_t>(-1); // firstFree
					buf.reserve(4);
					buf.reserve(8); // pad -- aabb is 16-aligned at +128

					// aabb at +128, filled in once the children are known.
					const auto aabb_slot = buf.reserve(32);
					buf.write<std::uint8_t>(1); // isMutable at +160
					buf.reserve(7);
					buf.reserve(16); // mutationSignals at +168 -- SERIALIZE_IGNORED
					buf.reserve(8); // pad to +192
					const auto bvd_slot = buf.reserve(8); // boundingVolumeData at +192
					buf.reserve(8); // pad to the 208-byte object size

					if (buf.size() - compound_offset != SIZEOF_DYNAMIC_COMPOUND_SHAPE)
					{
						ZONETOOL_ERROR("havok: hknpDynamicCompoundShape is %zu bytes, expected %d",
							buf.size() - compound_offset, SIZEOF_DYNAMIC_COMPOUND_SHAPE);
						return {};
					}

					// hknpShapeInstance[], immediately after the compound.
					local_fixups.emplace_back(instances_field, buf.size());
					std::vector<std::size_t> instance_shape_slots;
					for (auto i = 0; i < instance_count; i++)
					{
						const auto instance_offset = buf.size();
						// hkTransform is three rotation columns then the translation. The
						// convexes are already in the compound's space, so identity.
						buf.write<float>(1.0f); buf.write<float>(0.0f);
						buf.write<float>(0.0f); buf.write<float>(0.0f);
						buf.write<float>(0.0f); buf.write<float>(1.0f);
						buf.write<float>(0.0f); buf.write<float>(0.0f);
						buf.write<float>(0.0f); buf.write<float>(0.0f);
						buf.write<float>(1.0f); buf.write<float>(0.0f);
						buf.write<float>(0.0f); buf.write<float>(0.0f);
						buf.write<float>(0.0f); buf.write<float>(0.0f);
						for (auto c = 0; c < 4; c++)
						{
							buf.write<float>(1.0f); // scale
						}
						instance_shape_slots.emplace_back(buf.reserve(8));
						buf.write<std::uint16_t>(tag_for(shape));
						buf.write<std::uint16_t>(0xFFFF); // destructionTag
						// padding[30] ends at +122; the object is 128, so it is padded out
						// again. Getting this wrong shifts every instance after the first.
						buf.reserve(36);

						if (buf.size() - instance_offset != SIZEOF_SHAPE_INSTANCE)
						{
							ZONETOOL_ERROR("havok: hknpShapeInstance is %zu bytes, expected %d",
								buf.size() - instance_offset, SIZEOF_SHAPE_INSTANCE);
							return {};
						}
					}
					align16();

					float shape_min[3] = {FLT_MAX, FLT_MAX, FLT_MAX};
					float shape_max[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
					std::vector<std::array<float, 6>> convex_bounds;

					for (auto c = 0; c < instance_count; c++)
					{
						const auto& convex = shape.convexes[c];

						auto index_total = 0u;
						for (const auto& face : convex.faces)
						{
							index_total += static_cast<unsigned int>(face.indices.size());
						}

						const auto vertex_count = padded_vertex_count(convex.verts.size());

						if (vertex_count > 255 || convex.faces.size() > 0xFFFF
							|| index_total > 0xFFFF)
						{
							ZONETOOL_ERROR("havok: convex %d of shape %d exceeds the format "
								"limits (%zu verts, %zu faces, %u indices)", c, s,
								convex.verts.size(), convex.faces.size(), index_total);
							return {};
						}

						const auto convex_offset = buf.size();
						global_fixups.emplace_back(instance_shape_slots[c], convex_offset);
						virtual_fixups.emplace_back(convex_offset, 2);

						buf.reserve(16); // hkReferencedObject
						buf.write<std::uint16_t>(static_cast<std::uint16_t>(CONVEX_SHAPE_FLAGS));
						buf.write<std::uint8_t>(0); // numShapeKeyBits
						buf.write<std::uint8_t>(static_cast<std::uint8_t>(CONVEX_DISPATCH_TYPE));
						buf.write<float>(0.0f); // convexRadius
						buf.write<std::uint64_t>(0); // userData
						buf.reserve(8); // properties
						buf.reserve(8); // pad -- hknpShape is 48 bytes, its fields end at 40
						// hkRelArray<T> is { uint16 size; uint16 offset }, the offset counted
						// from the address of the field itself. All four payloads follow the
						// 96-byte object in declaration order.
						const auto vertices_field = buf.size(); // +48
						buf.reserve(4);
						buf.reserve(12); // pad to +64
						const auto planes_field = buf.size();
						buf.reserve(4);
						const auto faces_field = buf.size();
						buf.reserve(4);
						const auto indices_field = buf.size();
						buf.reserve(4);
						buf.reserve(4); // pad to +80
						const auto connectivity_slot = buf.reserve(8);
						buf.reserve(8); // pad to +96

						if (buf.size() - convex_offset != SIZEOF_CONVEX_POLYTOPE_SHAPE)
						{
							ZONETOOL_ERROR("havok: hknpConvexPolytopeShape is %zu bytes, "
								"expected %d", buf.size() - convex_offset,
								SIZEOF_CONVEX_POLYTOPE_SHAPE);
							return {};
						}

						// --- connectivity ---
						// vertexEdges[v] is any half-edge starting at vertex v; faceLinks[g]
						// is the twin of half-edge g, i.e. the (face, edge) running the same
						// two vertices the other way. Both verified against a decoded stock
						// box.
						std::vector<half_edge> vertex_edges(vertex_count);
						std::vector<bool> vertex_seen(vertex_count, false);
						std::vector<half_edge> face_links(index_total);
						std::map<std::pair<std::uint8_t, std::uint8_t>, half_edge> directed;

						for (auto f = 0u; f < convex.faces.size(); f++)
						{
							const auto& face = convex.faces[f];
							for (auto e = 0u; e < face.indices.size(); e++)
							{
								const auto from = face.indices[e];
								const auto to = face.indices[(e + 1) % face.indices.size()];
								const half_edge self{
									static_cast<std::uint16_t>(f),
									static_cast<std::uint8_t>(e)
								};

								if (from < vertex_seen.size() && !vertex_seen[from])
								{
									vertex_seen[from] = true;
									vertex_edges[from] = self;
								}

								directed[{from, to}] = self;
							}
						}

						auto global = 0u;
						auto unmatched = 0;
						for (auto f = 0u; f < convex.faces.size(); f++)
						{
							const auto& face = convex.faces[f];
							for (auto e = 0u; e < face.indices.size(); e++)
							{
								const auto from = face.indices[e];
								const auto to = face.indices[(e + 1) % face.indices.size()];
								const auto twin = directed.find({to, from});
								if (twin != directed.end())
								{
									face_links[global] = twin->second;
								}
								else
								{
									// A hull whose faces do not pair up is not closed. Point
									// the edge at itself rather than at an arbitrary face.
									face_links[global] = {
										static_cast<std::uint16_t>(f),
										static_cast<std::uint8_t>(e)
									};
									unmatched++;
								}
								global++;
							}
						}

						if (unmatched)
						{
							ZONETOOL_WARNING("havok: convex %d of shape %d has %d unpaired "
								"edge(s) -- hull is not closed", c, s, unmatched);
						}

						const auto write_rel_array = [&](const std::size_t field,
							const std::size_t count)
						{
							buf.patch<std::uint16_t>(field, static_cast<std::uint16_t>(count));
							buf.patch<std::uint16_t>(field + 2,
								static_cast<std::uint16_t>(buf.size() - field));
						};

						float mn[3] = {FLT_MAX, FLT_MAX, FLT_MAX};
						float mx[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};

						write_rel_array(vertices_field, vertex_count);
						for (auto j = 0u; j < vertex_count; j++)
						{
							// past the real vertices, repeat the last one whole -- see
							// padded_vertex_count above
							const auto source = std::min<std::size_t>(
								j, convex.verts.size() - 1);
							const auto& v = convex.verts[source];
							for (auto k = 0; k < 3; k++)
							{
								const auto value = v[k] * input.scale;
								buf.write<float>(value);
								mn[k] = std::min(mn[k], value);
								mx[k] = std::max(mx[k], value);
							}
							// The w lane is not padding: it carries the vertex's own index in
							// the low mantissa bits of 0.5f. 16,257 of 16,660 shipped vertices
							// are exactly 0x3F000000 | index, and the 403 that are not are the
							// padded copies, which repeat the last real vertex's w with it.
							buf.write<std::uint32_t>(
								0x3F000000u | static_cast<std::uint32_t>(source));
						}
						align16();

						// Havok stores dot(n, p) + d = 0 with an outward normal, so the CoD
						// plane distance is negated. It scales with the geometry.
						write_rel_array(planes_field, convex.faces.size());
						for (const auto& face : convex.faces)
						{
							for (auto k = 0; k < 3; k++)
							{
								buf.write<float>(face.plane[k]);
							}
							buf.write<float>(-face.plane[3] * input.scale);
						}
						align16();

						// minHalfAngle is the smallest support half-angle to an adjacent face.
						// The quantisation is IW8's encoder verbatim (its `findFaceSupportAngle`
						// caller): with the support angle in radians,
						//     v = (int)((angle*0.5 - 2^-23) * 41720.875 + 0.5)
						//     minHalfAngle = clamp(v, 0, 65535) >> 8
						// 41720.875 is ~131072/pi, so this is floor(halfAngle / pi * 512). A
						// right angle gives pi/4 * 41720.875 + 0.5 = 32766.4 -> 127, which is
						// what every axial brush produces. Faces with no paired neighbour keep
						// the blunt end of the range.
						write_rel_array(faces_field, convex.faces.size());
						auto first_index = 0u;
						for (auto f = 0u; f < convex.faces.size(); f++)
						{
							const auto& face = convex.faces[f];
							buf.write<std::uint16_t>(static_cast<std::uint16_t>(first_index));
							buf.write<std::uint8_t>(
								static_cast<std::uint8_t>(face.indices.size()));

							auto smallest = 45.0f;
							for (auto e = 0u; e < face.indices.size(); e++)
							{
								const auto& twin = face_links[first_index + e];
								if (twin.face == f || twin.face >= convex.faces.size())
								{
									continue; // unpaired edge -- nothing to measure against
								}

								const auto& other = convex.faces[twin.face].plane;
								auto dot = face.plane[0] * other[0] + face.plane[1] * other[1]
									+ face.plane[2] * other[2];
								dot = std::max(-1.0f, std::min(1.0f, dot));

								// The angle between two outward normals is the supplement of
								// the dihedral angle between the faces.
								const auto dihedral = 180.0f
									- static_cast<float>(std::acos(dot) * 57.29577951308232);
								smallest = std::min(smallest, dihedral * 0.5f);
							}

							const auto half_radians = smallest * 0.017453292519943295f;
							auto quantised = static_cast<int>(
								(half_radians - 1.1920929e-7f) * 41720.875f + 0.5f);
							quantised = std::max(0, std::min(65535, quantised)) >> 8;
							buf.write<std::uint8_t>(static_cast<std::uint8_t>(quantised));

							first_index += static_cast<unsigned int>(face.indices.size());
						}
						align16();

						write_rel_array(indices_field, index_total);
						for (const auto& face : convex.faces)
						{
							for (const auto index : face.indices)
							{
								buf.write<std::uint8_t>(index);
							}
						}
						align16();

						// the padded vertexEdges entries repeat the last real one, as stock does
						for (auto j = convex.verts.size(); j < vertex_count; j++)
						{
							vertex_edges[j] = vertex_edges[convex.verts.size() - 1];
						}

						const auto connectivity_offset = buf.size();
						global_fixups.emplace_back(connectivity_slot, connectivity_offset);
						virtual_fixups.emplace_back(connectivity_offset, 3);

						buf.reserve(16); // hkReferencedObject
						const auto vertex_edges_field = buf.size();
						write_hk_array_header(buf, static_cast<int>(vertex_edges.size()));
						const auto face_links_field = buf.size();
						write_hk_array_header(buf, static_cast<int>(face_links.size()));

						if (buf.size() - connectivity_offset
							!= SIZEOF_CONVEX_POLYTOPE_CONNECTIVITY)
						{
							ZONETOOL_ERROR("havok: connectivity is %zu bytes, expected %d",
								buf.size() - connectivity_offset,
								SIZEOF_CONVEX_POLYTOPE_CONNECTIVITY);
							return {};
						}

						const auto write_edges = [&](const std::size_t field,
							const std::vector<half_edge>& edges)
						{
							if (edges.empty())
							{
								return;
							}
							local_fixups.emplace_back(field, buf.size());
							for (const auto& edge : edges)
							{
								buf.write<std::uint16_t>(edge.face);
								buf.write<std::uint8_t>(edge.edge);
								buf.write<std::uint8_t>(0);
							}
							align16();
						};

						write_edges(vertex_edges_field, vertex_edges);
						write_edges(face_links_field, face_links);

						convex_bounds.push_back({mn[0], mn[1], mn[2], mx[0], mx[1], mx[2]});
						for (auto k = 0; k < 3; k++)
						{
							shape_min[k] = std::min(shape_min[k], mn[k]);
							shape_max[k] = std::max(shape_max[k], mx[k]);
						}
					}

					// The compound's own aabb, now that the children are known.
					for (auto k = 0; k < 3; k++)
					{
						buf.patch<float>(aabb_slot + k * 4, shape_min[k]);
						buf.patch<float>(aabb_slot + 16 + k * 4, shape_max[k]);
					}
					buf.patch<float>(aabb_slot + 12, 0.0f);
					buf.patch<float>(aabb_slot + 28, 0.0f);

					// --- hknpDynamicCompoundShapeData + its dynamic AABB tree ---
					const auto bvd_offset = buf.size();
					global_fixups.emplace_back(bvd_slot, bvd_offset);
					virtual_fixups.emplace_back(bvd_offset, 4);

					buf.reserve(16); // hkReferencedObject
					const auto nodes_field = buf.size();
					const auto node_count = 2 * instance_count + 1;
					write_hk_array_header(buf, node_count);
					buf.write<std::int32_t>(2 * instance_count); // firstFree
					buf.reserve(4);
					buf.write<std::int32_t>(instance_count); // numLeaves
					buf.reserve(4);
					buf.write<std::int32_t>(1); // root
					buf.reserve(4);

					if (buf.size() - bvd_offset != SIZEOF_DYNAMIC_COMPOUND_SHAPE_DATA)
					{
						ZONETOOL_ERROR("havok: hknpDynamicCompoundShapeData is %zu bytes, "
							"expected %d", buf.size() - bvd_offset,
							SIZEOF_DYNAMIC_COMPOUND_SHAPE_DATA);
						return {};
					}

					align16();

					// Build the tree before writing it. Node 0 is an all-zero sentinel, node 1
					// is the root and node 2L is a free slot, so the usable range is 1..2L-1.
					// Split on the widest axis at the median, which reproduces the balanced
					// shape of the shipped trees.
					std::vector<tree_node> nodes(node_count);
					auto next_node = 1;

					std::vector<int> order(instance_count);
					for (auto i = 0; i < instance_count; i++)
					{
						order[i] = i;
					}

					std::function<int(int, int, std::uint16_t)> build_tree =
						[&](const int first, const int count, const std::uint16_t parent) -> int
					{
						const auto self = next_node++;
						auto& node = nodes[self];
						node.parent = parent;

						for (auto k = 0; k < 3; k++)
						{
							node.mn[k] = FLT_MAX;
							node.mx[k] = -FLT_MAX;
						}
						for (auto i = 0; i < count; i++)
						{
							const auto& b = convex_bounds[order[first + i]];
							for (auto k = 0; k < 3; k++)
							{
								node.mn[k] = std::min(node.mn[k], b[k]);
								node.mx[k] = std::max(node.mx[k], b[k + 3]);
							}
						}

						if (count == 1)
						{
							// Leaf: the low half stays zero, which is what distinguishes it
							// from an internal node -- a child index is always >= 2.
							node.data = static_cast<std::uint32_t>(order[first]) << 16;
							return self;
						}

						auto axis = 0;
						auto widest = node.mx[0] - node.mn[0];
						for (auto k = 1; k < 3; k++)
						{
							if (node.mx[k] - node.mn[k] > widest)
							{
								widest = node.mx[k] - node.mn[k];
								axis = k;
							}
						}

						const auto centre = [&](const int index)
						{
							const auto& b = convex_bounds[index];
							return (b[axis] + b[axis + 3]) * 0.5f;
						};
						std::sort(order.begin() + first, order.begin() + first + count,
							[&](const int a, const int b) { return centre(a) < centre(b); });

						const auto half = count / 2;
						const auto left = build_tree(first, half,
							static_cast<std::uint16_t>(self));
						const auto right = build_tree(first + half, count - half,
							static_cast<std::uint16_t>(self));
						nodes[self].data = static_cast<std::uint32_t>(left)
							| (static_cast<std::uint32_t>(right) << 16);
						return self;
					};

					if (instance_count > 0)
					{
						build_tree(0, instance_count, 0);
					}

					local_fixups.emplace_back(nodes_field, buf.size());
					for (auto n = 0; n < node_count; n++)
					{
						const auto& node = nodes[n];
						const auto used = (n != 0 && n != node_count - 1);
						for (auto k = 0; k < 3; k++)
						{
							buf.write<float>(used ? node.mn[k] : 0.0f);
						}
						buf.write<std::uint16_t>(used ? node.parent : 0);
						// The upper half of this w lane is the top of 0.5f in every shipped
						// node; the links live in the low half and in the max w lane.
						buf.write<std::uint16_t>(used ? 0x3F00 : 0);
						for (auto k = 0; k < 3; k++)
						{
							buf.write<float>(used ? node.mx[k] : 0.0f);
						}
						buf.write<std::uint32_t>(used ? node.data : 0);
					}
					align16();
				}

				const auto data_size = buf.size();

				// ------------------------------------------------------ class names
				byte_buffer names;
				const auto write_name = [&](const std::uint32_t sig, const char* name)
				{
					names.write<std::uint32_t>(sig);
					names.write<std::uint8_t>(0x09);
					const auto offset = names.size();
					names.write(name, std::strlen(name) + 1);
					return offset;
				};

				write_name(SIG_HK_CLASS, "hkClass");
				write_name(SIG_HK_CLASS_MEMBER, "hkClassMember");
				write_name(SIG_HK_CLASS_ENUM, "hkClassEnum");
				write_name(SIG_HK_CLASS_ENUM_ITEM, "hkClassEnumItem");

				std::array<std::size_t, 5> name_offsets{};
				name_offsets[0] = write_name(SIG_SHAPE_LIST, "HavokPhysicsShapeList");
				if (shape_count > 0)
				{
					name_offsets[1] = write_name(SIG_DYNAMIC_COMPOUND_SHAPE,
						"hknpDynamicCompoundShape");
					name_offsets[2] = write_name(SIG_CONVEX_POLYTOPE_SHAPE,
						"hknpConvexPolytopeShape");
					name_offsets[3] = write_name(SIG_CONVEX_POLYTOPE_CONNECTIVITY,
						"hknpConvexPolytopeShapeConnectivity");
					name_offsets[4] = write_name(SIG_DYNAMIC_COMPOUND_SHAPE_DATA,
						"hknpDynamicCompoundShapeData");
				}
				names.align(16, 0xFF);

				// ------------------------------------------------------ fixup tables
				// ordered by destination -- see the note in build_world_shape
				std::sort(local_fixups.begin(), local_fixups.end(),
					[](const std::pair<std::size_t, std::size_t>& a,
						const std::pair<std::size_t, std::size_t>& b)
					{
						return a.second < b.second;
					});

				byte_buffer fixups;
				for (const auto& fixup : local_fixups)
				{
					fixups.write<std::int32_t>(static_cast<std::int32_t>(fixup.first));
					fixups.write<std::int32_t>(static_cast<std::int32_t>(fixup.second));
				}
				// see the note in build_world_shape: each table is padded to 16 with 0xFF
				fixups.align(16, 0xFF);
				const auto local_size = fixups.size();

				std::sort(global_fixups.begin(), global_fixups.end());
				for (const auto& fixup : global_fixups)
				{
					fixups.write<std::int32_t>(static_cast<std::int32_t>(fixup.first));
					fixups.write<std::int32_t>(2); // dstSectionIndex -- __data__
					fixups.write<std::int32_t>(static_cast<std::int32_t>(fixup.second));
				}
				fixups.align(16, 0xFF);
				const auto global_size = fixups.size() - local_size;

				std::sort(virtual_fixups.begin(), virtual_fixups.end());
				for (const auto& fixup : virtual_fixups)
				{
					fixups.write<std::int32_t>(static_cast<std::int32_t>(fixup.first));
					fixups.write<std::int32_t>(0);
					fixups.write<std::int32_t>(
						static_cast<std::int32_t>(name_offsets[fixup.second]));
				}
				fixups.align(16, 0xFF);
				const auto virtual_size = fixups.size() - local_size - global_size;

				// ------------------------------------------------------------ output
				byte_buffer file;
				file.write<std::uint32_t>(HK_MAGIC0);
				file.write<std::uint32_t>(HK_MAGIC1);
				file.write<std::int32_t>(0); // userTag
				file.write<std::int32_t>(HK_FILE_VERSION);
				file.write<std::uint8_t>(8); // pointerSize
				file.write<std::uint8_t>(1); // littleEndian
				file.write<std::uint8_t>(0); // reuseBaseClassPadding
				file.write<std::uint8_t>(1); // emptyBaseClassOptimization
				file.write<std::int32_t>(3); // numSections
				file.write<std::int32_t>(2); // contentsSectionIndex
				file.write<std::int32_t>(0); // contentsSectionOffset
				file.write<std::int32_t>(0); // contentsClassNameSectionIndex
				file.write<std::int32_t>(static_cast<std::int32_t>(name_offsets[0]));
				const char* version = "hk_2014.2.5-r1";
				const auto version_length = std::strlen(version) + 1;
				file.write(version, version_length);
				// hkPackfileHeader() memsets itself to -1 before assigning fields, so the tail
				// of contentsVersion stays 0xFF rather than 0.
				file.fill(16 - version_length, 0xFF);
				file.write<std::int32_t>(0); // flags
				file.write<std::uint16_t>(HK_MAX_PREDICATE);
				file.write<std::uint16_t>(0);

				const auto names_start = HK_HEADER_SIZE + 3 * HK_SECTION_HEADER_SIZE;
				const auto data_start = names_start + names.size();

				const auto write_section_header = [&](const char* tag, const std::size_t abs,
					const std::size_t payload, const std::size_t local, const std::size_t global,
					const std::size_t virt)
				{
					char name[19] = {};
					std::strncpy(name, tag, sizeof(name));
					file.write(name, sizeof(name));
					file.write<std::uint8_t>(0xFF); // m_nullByte
					file.write<std::int32_t>(static_cast<std::int32_t>(abs));
					file.write<std::int32_t>(static_cast<std::int32_t>(payload));
					file.write<std::int32_t>(static_cast<std::int32_t>(payload + local));
					file.write<std::int32_t>(static_cast<std::int32_t>(payload + local + global));
					const auto end = payload + local + global + virt;
					file.write<std::int32_t>(static_cast<std::int32_t>(end));
					file.write<std::int32_t>(static_cast<std::int32_t>(end));
					file.write<std::int32_t>(static_cast<std::int32_t>(end));
					file.fill(16, 0xFF); // m_pad[4]
				};

				write_section_header("__classnames__", names_start, names.size(), 0, 0, 0);
				write_section_header("__types__", data_start, 0, 0, 0, 0);
				write_section_header("__data__", data_start, data_size, local_size, global_size,
					virtual_size);

				file.write(names.data.data(), names.size());
				file.write(buf.data.data(), buf.size());
				file.write(fixups.data.data(), fixups.size());

				auto convex_total = 0;
				for (const auto* shape : shapes)
				{
					convex_total += static_cast<int>(shape->convexes.size());
				}

				ZONETOOL_INFO("havok: ents shape list built -- %d shapes, %d convexes, "
					"%zu surface tags, %zu bytes", shape_count, convex_total, palette.size(),
					file.size());

				return file.data;
			}

			// ------------------------------------------------------- PhysicsAsset
			//
			// The blob beside a `physicsasset` dump. Only the dummy form is generated: a
			// single static body wrapping a placeholder box. IW7 attaches one of these to
			// every script brush model and trigger, and `physicsShapeOverrideIdx` then
			// replaces the shape with one from MapEnts::havokEntsShapeData -- so the box is
			// never actually collided against, but the asset has to exist or no body is
			// built at all (see docs/iw7-ents-shapes.md section 3).
			//
			// Shipped dummies are byte-identical across every map, and the four variants
			// (script brush model default/fixed, trigger model default/static) differ only in
			// the body name string and the body-quality CRC. This reproduces
			// `scriptbrushmodeldummydefault.hkx` exactly, which is how it is tested.
			std::vector<std::uint8_t> build_physics_asset(const physics_asset_input& input)
			{
				// The placeholder box's convex radius. Its size is irrelevant -- the shape is
				// overridden -- but reproducing the shipped values keeps the output
				// byte-comparable against scriptbrushmodeldummydefault.hkx.
				constexpr auto BOX_CONVEX_RADIUS = 0.01f;

				// hknpShape::flags on this convex is 0x01C3, where the shipped ents convexes
				// use 0x0143. The extra 0x0080 tracks with this shape carrying a `properties`
				// pointer, which the ents ones do not.
				constexpr auto DUMMY_CONVEX_FLAGS = 0x01C3u;

				byte_buffer buf;
				std::vector<std::pair<std::size_t, std::size_t>> local_fixups;
				std::vector<std::pair<std::size_t, std::size_t>> global_fixups;
				std::vector<std::pair<std::size_t, int>> virtual_fixups;

				const auto align16 = [&] { buf.align(16, 0); };

				// --- HavokPhysicsAsset (144) ---
				virtual_fixups.emplace_back(static_cast<std::size_t>(0), 0);
				buf.reserve(8); // isRagdoll + pad
				const auto system_data_slot = buf.reserve(8);
				// Eight hkArrays at +16..+128. Six carry one entry each; the two
				// motionProperties/constraint lookups are empty.
				std::array<std::size_t, 8> lookup_field{};
				const int lookup_count[8] = {1, 1, 0, 1, 1, 1, 0, 1};
				for (auto i = 0; i < 8; i++)
				{
					lookup_field[i] = buf.size();
					write_hk_array_header(buf, lookup_count[i]);
				}

				if (buf.size() != 144)
				{
					ZONETOOL_ERROR("havok: HavokPhysicsAsset is %zu bytes, expected 144",
						buf.size());
					return {};
				}

				// Each non-empty lookup is a single 4-byte value in its own 16-byte slot.
				// bodyQualityNameCRCLookup and materialNameCRCLookup carry name CRCs; the
				// SFX/VFX/serverUsage/drivers entries are zero.
				const std::uint32_t lookup_value[8] = {
					input.body_quality_crc, input.material_crc, 0, 0, 0, 0, 0, 0
				};
				for (auto i = 0; i < 8; i++)
				{
					if (!lookup_count[i])
					{
						continue;
					}
					local_fixups.emplace_back(lookup_field[i], buf.size());
					buf.write<std::uint32_t>(lookup_value[i]);
					align16();
				}

				// --- hknpPhysicsSystemData (120) ---
				const auto system_data_offset = buf.size();
				global_fixups.emplace_back(system_data_slot, system_data_offset);
				virtual_fixups.emplace_back(system_data_offset, 1);

				buf.reserve(16); // hkReferencedObject
				write_hk_array_header(buf, 0); // materials
				write_hk_array_header(buf, 0); // motionProperties
				write_hk_array_header(buf, 0); // motionCinfos
				const auto body_cinfos_field = buf.size();
				write_hk_array_header(buf, 1); // bodyCinfos
				write_hk_array_header(buf, 0); // constraintCinfos
				const auto referenced_field = buf.size();
				write_hk_array_header(buf, 1); // referencedObjects
				const auto system_name_slot = buf.reserve(8); // name

				if (buf.size() - system_data_offset != 120)
				{
					ZONETOOL_ERROR("havok: hknpPhysicsSystemData is %zu bytes, expected 120",
						buf.size() - system_data_offset);
					return {};
				}

				align16();

				// --- hknpBodyCinfo (160) ---
				const auto body_offset = buf.size();
				local_fixups.emplace_back(body_cinfos_field, body_offset);

				const auto body_shape_slot = buf.reserve(8); // shape
				buf.write<std::int32_t>(0); // flags
				buf.write<std::uint32_t>(input.body_contents); // collisionFilterInfo
				buf.write<std::uint16_t>(0xFFFF); // materialId -- resolved at load
				buf.write<std::uint8_t>(0xFF); // qualityId -- likewise
				buf.reserve(5); // pad to +24
				buf.write<std::uint64_t>(0); // userData
				const auto body_name_slot = buf.reserve(8); // name
				buf.write<std::uint8_t>(0); // motionType -- 0 is static
				buf.reserve(7); // pad to +48
				// position and orientation, both identity: an hkVector4 and a quaternion,
				// each (0, 0, 0, 1).
				for (auto i = 0; i < 2; i++)
				{
					buf.write<float>(0.0f);
					buf.write<float>(0.0f);
					buf.write<float>(0.0f);
					buf.write<float>(1.0f);
				}
				buf.reserve(32); // linear and angular velocity, both zero
				buf.write<float>(-1.0f); // +112 -- unnamed in the reflection dump
				buf.reserve(12);
				// +128: four unnamed words, verbatim from the shipped dummy. The 0x7FFFFFFF
				// pair reads as "no limit" sentinels.
				buf.write<std::uint16_t>(0xFFFF);
				buf.write<std::uint16_t>(0x0000);
				buf.write<std::uint32_t>(0x7FFFFFFFu);
				buf.write<std::uint32_t>(0x7FFFFFFFu);
				buf.write<std::uint32_t>(0);
				buf.reserve(16);

				if (buf.size() - body_offset != 160)
				{
					ZONETOOL_ERROR("havok: hknpBodyCinfo is %zu bytes, expected 160",
						buf.size() - body_offset);
					return {};
				}

				// body name string
				local_fixups.emplace_back(body_name_slot, buf.size());
				buf.write(input.body_name.c_str(), input.body_name.size() + 1);
				align16();

				// referencedObjects payload: one pointer, back at the convex shape
				local_fixups.emplace_back(referenced_field, buf.size());
				const auto referenced_slot = buf.reserve(8);
				align16();

				// system name string
				local_fixups.emplace_back(system_name_slot, buf.size());
				const char* system_name = "Default Physics System Data";
				buf.write(system_name, std::strlen(system_name) + 1);
				align16();

				// --- hknpConvexPolytopeShape (96) + payloads ---
				const auto convex_offset = buf.size();
				global_fixups.emplace_back(body_shape_slot, convex_offset);
				global_fixups.emplace_back(referenced_slot, convex_offset);
				virtual_fixups.emplace_back(convex_offset, 2);

				buf.reserve(16); // hkReferencedObject
				buf.write<std::uint16_t>(static_cast<std::uint16_t>(DUMMY_CONVEX_FLAGS));
				buf.write<std::uint8_t>(0); // numShapeKeyBits
				buf.write<std::uint8_t>(static_cast<std::uint8_t>(CONVEX_DISPATCH_TYPE));
				buf.write<float>(BOX_CONVEX_RADIUS);
				buf.write<std::uint64_t>(0); // userData
				const auto properties_slot = buf.reserve(8);
				buf.reserve(8); // pad -- hknpShape is 48 bytes
				const auto vertices_field = buf.size();
				buf.reserve(4);
				buf.reserve(12); // pad to +64
				const auto planes_field = buf.size();
				buf.reserve(4);
				const auto faces_field = buf.size();
				buf.reserve(4);
				const auto indices_field = buf.size();
				buf.reserve(4);
				buf.reserve(4); // pad to +80
				buf.reserve(8); // connectivity -- null on the shipped dummies
				buf.reserve(8); // pad to +96

				if (buf.size() - convex_offset != SIZEOF_CONVEX_POLYTOPE_SHAPE)
				{
					ZONETOOL_ERROR("havok: dummy hknpConvexPolytopeShape is %zu bytes, "
						"expected %d", buf.size() - convex_offset,
						SIZEOF_CONVEX_POLYTOPE_SHAPE);
					return {};
				}

				const auto write_rel_array = [&](const std::size_t field,
					const std::size_t count)
				{
					buf.patch<std::uint16_t>(field, static_cast<std::uint16_t>(count));
					buf.patch<std::uint16_t>(field + 2,
						static_cast<std::uint16_t>(buf.size() - field));
				};

				// Vertex order is Havok's own: bit 0 of the index selects -x, bit 1 -y and
				// bit 2 -z, so vertex 0 is (+x, +y, +z), and the w lane carries the index.
				// The values are bit patterns rather than decimal literals because they are
				// copied from the shipped dummy and have to reproduce it exactly -- the box
				// is 19.68 x 24.68 x 14.68 CoD units, i.e. half-extents 20/25/15 shrunk by
				// the 0.01 (= 0.32 CoD) convex radius. Havok stores a convex polytope's
				// vertices shrunk inward by convexRadius and adds it back when colliding.
				static const std::uint32_t DUMMY_VERTS[32] = {
					0x3F1D701E, 0x3F456FFC, 0x3EEAE07E, 0x3F000000,
					0x3F1D701E, 0xBF456FFC, 0x3EEAE07E, 0x3F000001,
					0x3F1D701E, 0x3F456FFC, 0xBEEAE07E, 0x3F000002,
					0xBF1D701E, 0x3F456FFC, 0x3EEAE07E, 0x3F000003,
					0x3F1D701E, 0xBF456FFC, 0xBEEAE07E, 0x3F000004,
					0xBF1D701E, 0x3F456FFC, 0xBEEAE07E, 0x3F000005,
					0xBF1D701E, 0xBF456FFC, 0x3EEAE07E, 0x3F000006,
					0xBF1D701E, 0xBF456FFC, 0xBEEAE07E, 0x3F000007,
				};
				write_rel_array(vertices_field, 8);
				for (const auto word : DUMMY_VERTS)
				{
					buf.write<std::uint32_t>(word);
				}
				align16();

				// Planes in the shipped order -- +z, -z, +x, -x, +y, -y -- again verbatim.
				// Havok stores dot(n, p) + d = 0, so d is the negated half-extent; several
				// normal lanes carry -0.0f, which only a bit-exact copy reproduces.
				static const std::uint32_t DUMMY_PLANES[24] = {
					0x00000000, 0x00000000, 0x3F800000, 0xBEEAE07E,
					0x80000000, 0x00000000, 0xBF800000, 0xBEEAE07E,
					0x3F800000, 0x00000000, 0x00000000, 0xBF1D701E,
					0xBF800000, 0x80000000, 0x00000000, 0xBF1D701E,
					0x00000000, 0x3F800000, 0x00000000, 0xBF456FFC,
					0x80000000, 0xBF800000, 0x00000000, 0xBF456FFC,
				};
				write_rel_array(planes_field, 6);
				for (const auto word : DUMMY_PLANES)
				{
					buf.write<std::uint32_t>(word);
				}
				align16();

				write_rel_array(faces_field, 6);
				for (auto i = 0; i < 6; i++)
				{
					buf.write<std::uint16_t>(static_cast<std::uint16_t>(i * 4));
					buf.write<std::uint8_t>(4);
					buf.write<std::uint8_t>(127); // every dihedral on a box is a right angle
				}
				align16();

				// Face index runs, verbatim from the shipped dummy -- counter-clockwise about
				// each outward normal given the vertex order above.
				static const std::uint8_t FACE_INDICES[24] = {
					0, 3, 6, 1,  7, 5, 2, 4,  0, 1, 4, 2,
					3, 5, 7, 6,  5, 3, 0, 2,  7, 4, 1, 6,
				};
				write_rel_array(indices_field, 24);
				for (const auto index : FACE_INDICES)
				{
					buf.write<std::uint8_t>(index);
				}
				align16();

				// --- hkRefCountedProperties (16) ---
				const auto properties_offset = buf.size();
				global_fixups.emplace_back(properties_slot, properties_offset);
				virtual_fixups.emplace_back(properties_offset, 3);
				const auto entries_field = buf.size();
				write_hk_array_header(buf, 1);

				local_fixups.emplace_back(entries_field, buf.size());
				const auto mass_slot = buf.reserve(8);
				buf.write<std::uint16_t>(0xF100); // key -- the mass-properties property id
				buf.write<std::uint16_t>(0); // flags
				buf.reserve(4);
				align16();

				// --- hknpShapeMassProperties (48) ---
				const auto mass_offset = buf.size();
				global_fixups.emplace_back(mass_slot, mass_offset);
				virtual_fixups.emplace_back(mass_offset, 4);
				buf.reserve(16); // hkReferencedObject
				// hkCompressedMassProperties, copied verbatim: it is an opaque quantised
				// inertia tensor for the placeholder box, and since the shape is overridden
				// there is nothing to recompute it from. Reproducing it keeps the output
				// byte-identical to the shipped dummy.
				static const std::uint8_t COMPRESSED_MASS[32] = {
					0x36, 0x5D, 0x00, 0x00, 0x00, 0x00, 0x80, 0x22,
					0x68, 0x42, 0xD4, 0x30, 0x14, 0x50, 0x00, 0x31,
					0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x30, 0xF5,
					0x00, 0x00, 0xA0, 0x40, 0xD1, 0x5D, 0xEA, 0x3F,
				};
				buf.write(COMPRESSED_MASS, sizeof(COMPRESSED_MASS));

				if (buf.size() - mass_offset != 48)
				{
					ZONETOOL_ERROR("havok: hknpShapeMassProperties is %zu bytes, expected 48",
						buf.size() - mass_offset);
					return {};
				}

				align16();
				const auto data_size = buf.size();

				// ------------------------------------------------------ class names
				byte_buffer names;
				const auto write_name = [&](const std::uint32_t sig, const char* name)
				{
					names.write<std::uint32_t>(sig);
					names.write<std::uint8_t>(0x09);
					const auto offset = names.size();
					names.write(name, std::strlen(name) + 1);
					return offset;
				};

				write_name(SIG_HK_CLASS, "hkClass");
				write_name(SIG_HK_CLASS_MEMBER, "hkClassMember");
				write_name(SIG_HK_CLASS_ENUM, "hkClassEnum");
				write_name(SIG_HK_CLASS_ENUM_ITEM, "hkClassEnumItem");

				std::array<std::size_t, 5> name_offsets{};
				name_offsets[0] = write_name(SIG_PHYSICS_ASSET, "HavokPhysicsAsset");
				name_offsets[1] = write_name(SIG_PHYSICS_SYSTEM_DATA, "hknpPhysicsSystemData");
				name_offsets[2] = write_name(SIG_CONVEX_POLYTOPE_SHAPE,
					"hknpConvexPolytopeShape");
				name_offsets[3] = write_name(SIG_REF_COUNTED_PROPERTIES,
					"hkRefCountedProperties");
				name_offsets[4] = write_name(SIG_SHAPE_MASS_PROPERTIES,
					"hknpShapeMassProperties");
				names.align(16, 0xFF);

				// ------------------------------------------------------ fixup tables
				// ordered by destination -- see the note in build_world_shape
				std::sort(local_fixups.begin(), local_fixups.end(),
					[](const std::pair<std::size_t, std::size_t>& a,
						const std::pair<std::size_t, std::size_t>& b)
					{
						return a.second < b.second;
					});
				byte_buffer fixups;
				for (const auto& fixup : local_fixups)
				{
					fixups.write<std::int32_t>(static_cast<std::int32_t>(fixup.first));
					fixups.write<std::int32_t>(static_cast<std::int32_t>(fixup.second));
				}
				fixups.align(16, 0xFF);
				const auto local_size = fixups.size();

				std::sort(global_fixups.begin(), global_fixups.end());
				for (const auto& fixup : global_fixups)
				{
					fixups.write<std::int32_t>(static_cast<std::int32_t>(fixup.first));
					fixups.write<std::int32_t>(2); // dstSectionIndex -- __data__
					fixups.write<std::int32_t>(static_cast<std::int32_t>(fixup.second));
				}
				fixups.align(16, 0xFF);
				const auto global_size = fixups.size() - local_size;

				std::sort(virtual_fixups.begin(), virtual_fixups.end());
				for (const auto& fixup : virtual_fixups)
				{
					fixups.write<std::int32_t>(static_cast<std::int32_t>(fixup.first));
					fixups.write<std::int32_t>(0);
					fixups.write<std::int32_t>(
						static_cast<std::int32_t>(name_offsets[fixup.second]));
				}
				fixups.align(16, 0xFF);
				const auto virtual_size = fixups.size() - local_size - global_size;

				// ------------------------------------------------------------ output
				byte_buffer file;
				file.write<std::uint32_t>(HK_MAGIC0);
				file.write<std::uint32_t>(HK_MAGIC1);
				file.write<std::int32_t>(0); // userTag
				file.write<std::int32_t>(HK_FILE_VERSION);
				file.write<std::uint8_t>(8); // pointerSize
				file.write<std::uint8_t>(1); // littleEndian
				file.write<std::uint8_t>(0); // reuseBaseClassPadding
				file.write<std::uint8_t>(1); // emptyBaseClassOptimization
				file.write<std::int32_t>(3); // numSections
				file.write<std::int32_t>(2); // contentsSectionIndex
				file.write<std::int32_t>(0); // contentsSectionOffset
				file.write<std::int32_t>(0); // contentsClassNameSectionIndex
				file.write<std::int32_t>(static_cast<std::int32_t>(name_offsets[0]));
				const char* version = "hk_2014.2.5-r1";
				const auto version_length = std::strlen(version) + 1;
				file.write(version, version_length);
				file.fill(16 - version_length, 0xFF);
				file.write<std::int32_t>(0); // flags
				file.write<std::uint16_t>(HK_MAX_PREDICATE);
				file.write<std::uint16_t>(0);

				const auto names_start = HK_HEADER_SIZE + 3 * HK_SECTION_HEADER_SIZE;
				const auto data_start = names_start + names.size();

				const auto write_section_header = [&](const char* tag, const std::size_t abs,
					const std::size_t payload, const std::size_t local, const std::size_t global,
					const std::size_t virt)
				{
					char name[19] = {};
					std::strncpy(name, tag, sizeof(name));
					file.write(name, sizeof(name));
					file.write<std::uint8_t>(0xFF); // m_nullByte
					file.write<std::int32_t>(static_cast<std::int32_t>(abs));
					file.write<std::int32_t>(static_cast<std::int32_t>(payload));
					file.write<std::int32_t>(static_cast<std::int32_t>(payload + local));
					file.write<std::int32_t>(static_cast<std::int32_t>(payload + local + global));
					const auto end = payload + local + global + virt;
					file.write<std::int32_t>(static_cast<std::int32_t>(end));
					file.write<std::int32_t>(static_cast<std::int32_t>(end));
					file.write<std::int32_t>(static_cast<std::int32_t>(end));
					file.fill(16, 0xFF); // m_pad[4]
				};

				write_section_header("__classnames__", names_start, names.size(), 0, 0, 0);
				write_section_header("__types__", data_start, 0, 0, 0, 0);
				write_section_header("__data__", data_start, data_size, local_size, global_size,
					virtual_size);

				file.write(names.data.data(), names.size());
				file.write(buf.data.data(), buf.size());
				file.write(fixups.data.data(), fixups.size());

				ZONETOOL_INFO("havok: physics asset \"%s\" built -- %zu bytes",
					input.body_name.c_str(), file.size());

				return file.data;
			}
		}
	}
}
