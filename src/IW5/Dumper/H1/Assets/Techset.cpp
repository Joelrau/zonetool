#include "stdafx.hpp"
#include "Techset.hpp"

#include <set>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>

// Translation tables are authored by a parallel agent at this path. They may not
// exist on disk yet at the time this file is written; that is expected. We code
// strictly against the agreed 'techset_translate' interface.
#include "../Techset/TranslationTables.hpp"

// ============================================================================
// Custom IW3 -> H1 techset dumper implementation.
//
// The material we receive is a ZoneTool::IW5::Material, but when the dump source
// is IW3 its 'techniqueSet', 'stateBitsTable' and 'constantTable' pointers still
// reference IW3-layout memory (the IW3->IW4->IW5 convert() chain only
// reinterpret_casts them). We therefore reinterpret those with the local IW3
// struct definitions below and emit fully custom H1 assets.
//
// Target (H1) struct definitions and the assetmanager tagged-binary writer are
// pulled in transitively through stdafx.hpp -> IW5.hpp:
//   ZoneTool::H1::*            (H1/Structs.hpp, PTR64-correct for x64 output)
//   zonetool::assetmanager     (X64/Utils/IO/assetmanager.hpp)
//   zonetool::filesystem       (X64/Utils/IO/filesystem.hpp)
//   ordered_json / json        (ZoneUtils/Json.hpp)
// ============================================================================

namespace ZoneTool
{
	namespace IW5
	{
		namespace H1Dumper
		{
			// ----------------------------------------------------------------
			// Local IW3 (CoD4, DX9) source structs.
			// Copied to match zonetool src/IW3/Structs.hpp exactly (native x86
			// pointers = 32-bit, matching the 32-bit IW3 process being dumped).
			//   MaterialTechniqueSet   IW3/Structs.hpp:362-380 (techniques[34])
			//   MaterialTechnique      IW3/Structs.hpp:356-360 (hdr + pass[1])
			//   MaterialTechniqueHdr   IW3/Structs.hpp:349-354
			//   MaterialPass           IW3/Structs.hpp:337-347
			//   MaterialShaderArgument IW3/Structs.hpp:309-314
			//   MaterialVertexDecl     IW3/Structs.hpp:316-335
			//   GfxStateBits           IW3/Structs.hpp:188-191 (loadBits[2])
			//   GfxVertexShaderLoadDef IW3/Structs.hpp:256-261
			//   GfxPixelShaderLoadDef  IW3/Structs.hpp:275-280
			// ----------------------------------------------------------------
			namespace iw3
			{
#pragma pack(push, 4)
				struct GfxVertexShaderLoadDef
				{
					unsigned int* program;
					unsigned short programSize; // in DWORDs
					unsigned short loadForRenderer;
				};

				struct MaterialVertexShaderProgram
				{
					void* vs;
					GfxVertexShaderLoadDef loadDef;
				};

				struct MaterialVertexShader
				{
					const char* name;
					MaterialVertexShaderProgram prog;
				};

				struct GfxPixelShaderLoadDef
				{
					unsigned int* program;
					unsigned short programSize; // in DWORDs
					unsigned short loadForRenderer;
				};

				struct MaterialPixelShaderProgram
				{
					void* ps;
					GfxPixelShaderLoadDef loadDef;
				};

				struct MaterialPixelShader
				{
					const char* name;
					MaterialPixelShaderProgram prog;
				};

				struct MaterialArgumentCodeConst
				{
					unsigned short index;
					char firstRow;
					char rowCount;
				};

				union MaterialArgumentDef
				{
					const float* literalConst;
					MaterialArgumentCodeConst codeConst;
					unsigned int codeSampler;
					unsigned int nameHash;
				};

				struct MaterialShaderArgument
				{
					unsigned short type;
					unsigned short dest;
					MaterialArgumentDef u;
				};

				struct MaterialStreamRouting
				{
					char source;
					char dest;
				};

				struct MaterialVertexStreamRouting
				{
					MaterialStreamRouting data[16];
					void* decl[16];
				};

				struct MaterialVertexDeclaration
				{
					char streamCount;
					bool hasOptionalSource;
					bool isLoaded;
					char pad[1];
					MaterialVertexStreamRouting routing;
				};

				struct MaterialPass
				{
					MaterialVertexDeclaration* vertexDecl;
					MaterialVertexShader* vertexShader;
					MaterialPixelShader* pixelShader;
					char perPrimArgCount;
					char perObjArgCount;
					char stableArgCount;
					char customSamplerFlags;
					MaterialShaderArgument* args;
				};

				struct MaterialTechniqueHdr
				{
					const char* name;
					unsigned short flags;
					unsigned short numPasses;
				};

				struct MaterialTechnique
				{
					MaterialTechniqueHdr hdr;
					MaterialPass pass[1];
				};

				struct MaterialTechniqueSet
				{
					const char* name;
					char worldVertFormat;
					bool hasBeenUploaded;
					char unused[1];
					char pad;
					MaterialTechniqueSet* remappedTechniqueSet;
					MaterialTechnique* techniques[34];
				};

				struct GfxStateBits
				{
					unsigned int loadBits[2];
				};

				constexpr int TECHNIQUE_COUNT = 34;

				// IW3/IW5 (DX9-era) surface statebit layout, used to decode
				// IW3 loadBits[0]/[1]. Values from IW5/Structs.hpp:2945-3024.
				enum : unsigned int
				{
					DX9_0_ATEST_DISABLE = 0x800,
					DX9_0_ATEST_GT_0 = 0x1000,
					DX9_0_ATEST_LT_128 = 0x2000,
					DX9_0_ATEST_GE_128 = 0x3000,
					DX9_0_ATEST_MASK = 0x3000,
					DX9_0_CULL_NONE = 0x4000,
					DX9_0_CULL_BACK = 0x8000,
					DX9_0_CULL_FRONT = 0xC000,
					DX9_0_CULL_MASK = 0xC000,
					DX9_0_COLORWRITE_RGB = 0x8000000,
					DX9_0_COLORWRITE_ALPHA = 0x10000000,
					DX9_0_GAMMAWRITE = 0x40000000,
					DX9_0_POLYMODE_LINE = 0x80000000,
				};

				enum : unsigned int
				{
					DX9_1_DEPTHWRITE = 0x1,
					DX9_1_DEPTHTEST_DISABLE = 0x2,
					DX9_1_DEPTHTEST_ALWAYS = 0x0,
					DX9_1_DEPTHTEST_LESS = 0x4,
					DX9_1_DEPTHTEST_EQUAL = 0x8,
					DX9_1_DEPTHTEST_LESSEQUAL = 0xC,
					DX9_1_DEPTHTEST_MASK = 0xC,
					DX9_1_POLYGON_OFFSET_MASK = 0x30,
					DX9_1_POLYGON_OFFSET_SHIFT = 0x4,
					DX9_1_STENCIL_FRONT_ENABLE = 0x40,
					DX9_1_STENCIL_BACK_ENABLE = 0x80,
					DX9_1_STENCIL_FRONT_PASS_SHIFT = 0x8,
					DX9_1_STENCIL_FRONT_FAIL_SHIFT = 0xB,
					DX9_1_STENCIL_FRONT_ZFAIL_SHIFT = 0xE,
					DX9_1_STENCIL_FRONT_FUNC_SHIFT = 0x11,
					DX9_1_STENCIL_BACK_PASS_SHIFT = 0x14,
					DX9_1_STENCIL_BACK_FAIL_SHIFT = 0x17,
					DX9_1_STENCIL_BACK_ZFAIL_SHIFT = 0x1A,
					DX9_1_STENCIL_BACK_FUNC_SHIFT = 0x1D,
				};

				// IW3/IW5 MaterialShaderArgumentType (IW5/Structs.hpp:2701-2715).
				enum ArgType
				{
					ARG_MATERIAL_VERTEX_CONST = 0x0,
					ARG_LITERAL_VERTEX_CONST = 0x1,
					ARG_MATERIAL_VERTEX_SAMPLER = 0x2,
					ARG_MATERIAL_PIXEL_SAMPLER = 0x3,
					ARG_CODE_VERTEX_CONST = 0x4,
					ARG_CODE_PIXEL_SAMPLER = 0x5,
					ARG_CODE_PIXEL_CONST = 0x6,
					ARG_MATERIAL_PIXEL_CONST = 0x7,
					ARG_LITERAL_PIXEL_CONST = 0x8,
				};
#pragma pack(pop)
			}

			bool custom_techset_active()
			{
				return zonetool::dumping_source == zonetool::dump_source::iw3;
			}

			namespace
			{
				// Tiny bump arena to keep temporary H1 struct pointers stable for
				// the duration of a single dump (the assetmanager dedups by
				// address, so allocations must not move).
				struct arena
				{
					std::vector<std::unique_ptr<std::uint8_t[]>> blocks;

					template <typename T>
					T* alloc(std::size_t count = 1)
					{
						auto block = std::make_unique<std::uint8_t[]>(sizeof(T) * count);
						std::memset(block.get(), 0, sizeof(T) * count);
						auto* ptr = reinterpret_cast<T*>(block.get());
						blocks.push_back(std::move(block));
						return ptr;
					}

					char* dup(const std::string& str)
					{
						auto block = std::make_unique<std::uint8_t[]>(str.size() + 1);
						std::memcpy(block.get(), str.data(), str.size());
						block[str.size()] = 0;
						auto* ptr = reinterpret_cast<char*>(block.get());
						blocks.push_back(std::move(block));
						return ptr;
					}
				};

				// Accumulated per-shader metadata for the DX9->SM5 shader CLI.
				struct shader_stage_meta
				{
					std::string stage; // "vs" | "ps"
					ordered_json args = ordered_json::array();
					std::set<std::pair<int, int>> seen; // (type, dest) dedup
				};

				struct dump_context
				{
					arena mem;
					std::string techset_name;
					std::string material_name;
					IW5::Material* material = nullptr;

					std::unordered_set<std::string> written_decls;
					std::unordered_set<std::string> written_shaders;
					std::unordered_map<std::string, shader_stage_meta> shader_meta;
				};

				// --------------------------------------------------------------
				// Statebits conversion (IW3 DX9 loadBits[2] -> H1 6-word raw).
				// Decodes the DX9 encoding, then re-encodes into the H1 raw
				// layout (H1/Structs.hpp GFXS*_ constants). depthStencilStateBits
				// and blendStateBits are written as zeros in the statebitsmap,
				// matching zonetool's own native H1 dumper
				// (H1/Assets/Techset.cpp dump_statebits_map): the H1 linker
				// resolves those from loadBits / global tables at link time.
				// --------------------------------------------------------------
				void convert_statebits(const iw3::GfxStateBits& src, H1::GfxStateBits& dst)
				{
					const unsigned int op0 = src.loadBits[0];
					const unsigned int op1 = src.loadBits[1];

					unsigned int lb0 = 0; // cull / atest / gamma / polymode
					unsigned int lb1 = 0; // depth / stencil
					unsigned int lb2 = 0xFFFF; // stencil read/write masks (H1 default)
					unsigned int lb3 = 0; // blend / colorwrite / depthwrite_opaque
					unsigned int lb4 = 0;
					unsigned int lb5 = 0;

					// --- decode DX9 ---
					const unsigned int srcBlendRgb = (op0 >> 0) & 0xF;
					const unsigned int dstBlendRgb = (op0 >> 4) & 0xF;
					const unsigned int blendOpRgb = (op0 >> 8) & 0x7;
					const unsigned int srcBlendAlpha = (op0 >> 16) & 0xF;
					const unsigned int dstBlendAlpha = (op0 >> 20) & 0xF;
					const unsigned int blendOpAlpha = (op0 >> 24) & 0x7;
					const unsigned int atest = op0 & iw3::DX9_0_ATEST_MASK;
					const unsigned int cull = op0 & iw3::DX9_0_CULL_MASK;
					const bool colorWriteRgb = (op0 & iw3::DX9_0_COLORWRITE_RGB) != 0;
					const bool colorWriteAlpha = (op0 & iw3::DX9_0_COLORWRITE_ALPHA) != 0;
					const bool gammaWrite = (op0 & iw3::DX9_0_GAMMAWRITE) != 0;
					const bool polymodeLine = (op0 & iw3::DX9_0_POLYMODE_LINE) != 0;

					const bool depthWrite = (op1 & iw3::DX9_1_DEPTHWRITE) != 0;
					const bool depthTestDisable = (op1 & iw3::DX9_1_DEPTHTEST_DISABLE) != 0;
					const unsigned int depthTest = op1 & iw3::DX9_1_DEPTHTEST_MASK;
					const unsigned int polygonOffset = (op1 & iw3::DX9_1_POLYGON_OFFSET_MASK) >> iw3::DX9_1_POLYGON_OFFSET_SHIFT;
					const bool stencilFrontEnable = (op1 & iw3::DX9_1_STENCIL_FRONT_ENABLE) != 0;
					const bool stencilBackEnable = (op1 & iw3::DX9_1_STENCIL_BACK_ENABLE) != 0;

					// --- encode H1 loadbit0 (atest / cull / polymode / gamma) ---
					if (atest == iw3::DX9_0_ATEST_GT_0) lb0 |= H1::GFXS0_ATEST_GT_0;
					else if (atest == iw3::DX9_0_ATEST_LT_128) lb0 |= H1::GFXS0_ATEST_LT_128;
					else if (atest == iw3::DX9_0_ATEST_GE_128) lb0 |= H1::GFXS0_ATEST_GE_128;
					else lb0 |= H1::GFXS0_ATEST_DISABLE;

					if (cull == iw3::DX9_0_CULL_BACK) lb0 |= H1::GFXS0_CULL_BACK;
					else if (cull == iw3::DX9_0_CULL_FRONT) lb0 |= H1::GFXS0_CULL_FRONT;
					else lb0 |= H1::GFXS0_CULL_NONE;

					if (polymodeLine) lb0 |= H1::GFXS0_POLYMODE_LINE;
					if (gammaWrite) lb0 |= H1::GFXS0_GAMMAWRITE;

					// --- encode H1 loadbit1 (depth / stencil) ---
					if (depthWrite) lb1 |= H1::GFXS1_DEPTHWRITE;
					if (depthTestDisable) lb1 |= H1::GFXS1_DEPTHTEST_DISABLE;
					else if (depthTest == iw3::DX9_1_DEPTHTEST_LESS) lb1 |= H1::GFXS1_DEPTHTEST_LESS;
					else if (depthTest == iw3::DX9_1_DEPTHTEST_EQUAL) lb1 |= H1::GFXS1_DEPTHTEST_EQUAL;
					else if (depthTest == iw3::DX9_1_DEPTHTEST_LESSEQUAL) lb1 |= H1::GFXS1_DEPTHTEST_LESSEQUAL;
					else lb1 |= H1::GFXS1_DEPTHTEST_ALWAYS;

					lb1 |= (polygonOffset << H1::GFXS1_POLYGON_OFFSET_SHIFT) & H1::GFXS1_POLYGON_OFFSET_MASK;

					if (stencilFrontEnable) lb1 |= H1::GFXS1_STENCIL_FRONT_ENABLE;
					if (stencilBackEnable) lb1 |= H1::GFXS1_STENCIL_BACK_ENABLE;

					// Stencil op/func fields keep the same 3-bit encodings between
					// DX9 and H1, only the containing word differs; copy across.
					const auto copy_field = [&](unsigned int dxShift, unsigned int h1Shift, unsigned int mask)
					{
						lb1 |= ((op1 >> dxShift) & mask) << h1Shift;
					};
					copy_field(iw3::DX9_1_STENCIL_FRONT_PASS_SHIFT, H1::GFXS1_STENCIL_FRONT_PASS_SHIFT, 0x7);
					copy_field(iw3::DX9_1_STENCIL_FRONT_FAIL_SHIFT, H1::GFXS1_STENCIL_FRONT_FAIL_SHIFT, 0x7);
					copy_field(iw3::DX9_1_STENCIL_FRONT_ZFAIL_SHIFT, H1::GFXS1_STENCIL_FRONT_ZFAIL_SHIFT, 0x7);
					copy_field(iw3::DX9_1_STENCIL_FRONT_FUNC_SHIFT, H1::GFXS1_STENCIL_FRONT_FUNC_SHIFT, 0x7);
					copy_field(iw3::DX9_1_STENCIL_BACK_PASS_SHIFT, H1::GFXS1_STENCIL_BACK_PASS_SHIFT, 0x7);
					copy_field(iw3::DX9_1_STENCIL_BACK_FAIL_SHIFT, H1::GFXS1_STENCIL_BACK_FAIL_SHIFT, 0x7);
					copy_field(iw3::DX9_1_STENCIL_BACK_ZFAIL_SHIFT, H1::GFXS1_STENCIL_BACK_ZFAIL_SHIFT, 0x7);
					copy_field(iw3::DX9_1_STENCIL_BACK_FUNC_SHIFT, H1::GFXS1_STENCIL_BACK_FUNC_SHIFT, 0x7);

					// --- encode H1 loadbit3 (blend / colorwrite) ---
					lb3 |= (srcBlendRgb << H1::GFXS3_SRCBLEND_RGB_SHIFT) & H1::GFXS3_SRCBLEND_RGB_MASK;
					lb3 |= (dstBlendRgb << H1::GFXS3_DSTBLEND_RGB_SHIFT) & H1::GFXS3_DSTBLEND_RGB_MASK;
					lb3 |= (blendOpRgb << H1::GFXS3_BLENDOP_RGB_SHIFT) & H1::GFXS3_BLENDOP_RGB_MASK;
					lb3 |= (srcBlendAlpha << H1::GFXS3_SRCBLEND_ALPHA_SHIFT) & H1::GFXS3_SRCBLEND_ALPHA_MASK;
					lb3 |= (dstBlendAlpha << H1::GFXS3_DSTBLEND_ALPHA_SHIFT) & H1::GFXS3_DSTBLEND_ALPHA_MASK;
					lb3 |= (blendOpAlpha << H1::GFXS3_BLENDOP_ALPHA_SHIFT) & H1::GFXS3_BLENDOP_ALPHA_MASK;
					if (colorWriteRgb) lb3 |= H1::GFXS3_BLEND_COLORWRITE_RGB;
					if (colorWriteAlpha) lb3 |= H1::GFXS3_BLEND_COLORWRITE_A;

					std::memset(&dst, 0, sizeof(dst));
					dst.loadBits[0] = lb0;
					dst.loadBits[1] = lb1;
					dst.loadBits[2] = lb2;
					dst.loadBits[3] = lb3;
					dst.loadBits[4] = lb4;
					dst.loadBits[5] = lb5;

					// --- rasterizerState byte ---
					unsigned char rs = 0;
					if (cull == iw3::DX9_0_CULL_BACK) rs |= H1::RASTERIZER_STATE_CULL_BACK;
					else if (cull == iw3::DX9_0_CULL_FRONT) rs |= H1::RASTERIZER_STATE_CULL_FRONT;
					else rs |= H1::RASTERIZER_STATE_CULL_NONE;
					rs |= (static_cast<unsigned char>(polygonOffset) << H1::RASTERIZER_STATE_POLYGON_OFFSET_SHIFT) & H1::RASTERIZER_STATE_POLYGON_OFFSET_MASK;
					if (polymodeLine) rs |= H1::RASTERIZER_STATE_POLYMODE_LINE_MASK;
					unsigned char atestRaster = 0;
					if (atest == iw3::DX9_0_ATEST_GT_0) atestRaster = 1;
					else if (atest == iw3::DX9_0_ATEST_LT_128) atestRaster = 2;
					else if (atest == iw3::DX9_0_ATEST_GE_128) atestRaster = 3;
					rs |= (atestRaster << H1::RASTERIZER_STATE_ATEST_SHIFT) & H1::RASTERIZER_STATE_ATEST_MASK;
					if (gammaWrite) rs |= H1::RASTERIZER_STATE_GAMMAWRITE_MASK;
					dst.rasterizerState = rs;
				}

				// --------------------------------------------------------------
				// Vertex declaration
				// --------------------------------------------------------------
				std::string vertexdecl_name(const iw3::MaterialVertexDeclaration* decl)
				{
					// Deterministic name from the routing signature so identical
					// declarations across passes/techsets share a single file.
					std::string sig = "iw3vd";
					const int count = decl->streamCount;
					char buf[16];
					for (int i = 0; i < count && i < 16; i++)
					{
						sprintf_s(buf, "_%02x%02x", static_cast<unsigned char>(decl->routing.data[i].source),
							static_cast<unsigned char>(decl->routing.data[i].dest));
						sig += buf;
					}
					if (decl->hasOptionalSource)
					{
						sig += "_o";
					}
					return sig;
				}

				H1::MaterialVertexDeclaration* build_vertexdecl(dump_context& ctx, const iw3::MaterialVertexDeclaration* src, const std::string& name)
				{
					auto* dst = ctx.mem.alloc<H1::MaterialVertexDeclaration>();
					dst->name = ctx.mem.dup(name);
					dst->streamCount = static_cast<unsigned char>(src->streamCount);
					dst->hasOptionalSource = src->hasOptionalSource;

					const int count = src->streamCount;
					for (int i = 0; i < count && i < 32; i++)
					{
						unsigned char h1_source = 0, h1_dest = 0, h1_mask = 0;
						if (!techset_translate::map_stream_routing_iw3_to_h1(
							static_cast<unsigned char>(src->routing.data[i].source),
							static_cast<unsigned char>(src->routing.data[i].dest),
							h1_source, h1_dest, h1_mask))
						{
							ZONETOOL_WARNING("Unmappable stream routing (src=%d dest=%d) in vertexdecl \"%s\"",
								src->routing.data[i].source, src->routing.data[i].dest, name.data());
						}
						dst->routing.data[i].source = h1_source;
						dst->routing.data[i].dest = h1_dest;
						dst->routing.data[i].mask = h1_mask;
					}
					return dst;
				}

				void dump_vertexdecl_file(H1::MaterialVertexDeclaration* decl)
				{
					const auto path = "techsets\\"s + decl->name + ".vertexdecl"s;
					zonetool::assetmanager::dumper write;
					if (!write.open(path))
					{
						return;
					}
					write.dump_single(decl);
					write.dump_string(decl->name);
					write.close();
				}

				// --------------------------------------------------------------
				// DX9 shader staging (raw bytecode + json metadata for the CLI)
				// --------------------------------------------------------------
				const char* group_buffer_name(int group) // 0=perPrim 1=perObj 2=stable
				{
					switch (group)
					{
					case 0: return "perPrim";
					case 1: return "perObj";
					default: return "material";
					}
				}

				void stage_shader_blob(const std::string& name, const std::string& ext, const unsigned int* program, unsigned int programSizeDwords)
				{
					// IW3 GfxVertex/PixelShaderLoadDef programSize is a DWORD
					// count; the raw DX9 bytecode is programSize * 4 bytes.
					const auto path = "techsets\\dx9\\"s + name + "."s + ext;
					auto file = zonetool::filesystem::file(path);
					file.open("wb");
					if (file.get_fp() && program && programSizeDwords)
					{
						file.write(program, sizeof(unsigned int), programSizeDwords);
					}
					file.close();
				}

				void accumulate_shader_arg(dump_context& ctx, const std::string& shader_name, const std::string& stage,
					const iw3::MaterialShaderArgument& iw3_arg, const H1::MaterialShaderArgument& h1_arg, int group)
				{
					auto& meta = ctx.shader_meta[shader_name];
					if (meta.stage.empty())
					{
						meta.stage = stage;
					}

					if (!meta.seen.insert({ iw3_arg.type, iw3_arg.dest }).second)
					{
						return; // already recorded this (type,dest) for this shader
					}

					ordered_json arg = {};
					arg["type_iw3"] = iw3_arg.type;
					arg["type_h1"] = h1_arg.type;
					arg["dest_register"] = iw3_arg.dest;
					arg["buffer"] = group_buffer_name(group);

					switch (h1_arg.type)
					{
					case H1::MTL_ARG_CODE_CONST:
						arg["code_const_iw3"] = iw3_arg.u.codeConst.index;
						arg["code_const_h1"] = h1_arg.u.codeConst.index;
						arg["firstRow"] = h1_arg.u.codeConst.firstRow;
						arg["rowCount"] = h1_arg.u.codeConst.rowCount;
						{
							const char* n = techset_translate::h1_code_const_name(h1_arg.u.codeConst.index);
							if (n) arg["name"] = n;
						}
						break;
					case H1::MTL_ARG_CODE_SAMPLER:
					case H1::MTL_ARG_CODE_TEXTURE:
						arg["sampler_slot"] = iw3_arg.dest;
						arg["code_sampler_h1"] = h1_arg.u.codeSampler;
						break;
					case H1::MTL_ARG_MATERIAL_CONST:
						arg["nameHash"] = h1_arg.u.nameHash;
						break;
					case H1::MTL_ARG_LITERAL_CONST:
						if (h1_arg.u.literalConst)
						{
							arg["literal"] = { h1_arg.u.literalConst[0], h1_arg.u.literalConst[1],
								h1_arg.u.literalConst[2], h1_arg.u.literalConst[3] };
						}
						break;
					case H1::MTL_ARG_MATERIAL_TEXTURE:
					case H1::MTL_ARG_MATERIAL_SAMPLER:
						arg["nameHash"] = h1_arg.u.nameHash;
						arg["sampler_slot"] = iw3_arg.dest;
						break;
					default:
						break;
					}

					meta.args.push_back(arg);
				}

				void write_shader_metadata(dump_context& ctx)
				{
					for (auto& [name, meta] : ctx.shader_meta)
					{
						ordered_json j = {};
						j["shader"] = name;
						j["stage"] = meta.stage;
						j["source_game"] = "iw3";
						j["args"] = meta.args;

						const auto path = "techsets\\dx9\\"s + name + ".json"s;
						auto file = zonetool::filesystem::file(path);
						file.open("wb");
						if (file.get_fp())
						{
							const auto dump = j.dump(4, ' ', true, nlohmann::detail::error_handler_t::replace);
							file.write(dump.data(), dump.size(), 1);
						}
						file.close();
					}
				}

				// Per-arg shader-stage bitmask (H1 MaterialShaderArgument.shader).
				// A DX9 arg belongs to exactly one stage; H1 encodes this as a
				// bitmask: pixel stage = 0x10, vertex stage = 0x2. (See
				// TranslationTables.hpp map_arg_type_iw3_to_h1 note / x64-zt
				// iw6->h1 converter.)
				constexpr unsigned char H1_ARG_STAGE_VERTEX = 0x2;
				constexpr unsigned char H1_ARG_STAGE_PIXEL = 0x10;

				// --------------------------------------------------------------
				// Argument translation for one pass.
				//
				// Builds the H1 arg array grouped as [perPrim][perObj][stable] and
				// accumulates shader metadata for the DX9 CLI. A single DX9
				// combined texture+sampler arg expands into TWO H1 args (a texture
				// arg + a companion sampler arg on the same register), so H1 arg
				// counts can exceed the IW3 counts. Returns false if any arg is
				// unmappable (caller skips the technique rather than emit bad data).
				// --------------------------------------------------------------
				bool translate_pass_args(dump_context& ctx, const iw3::MaterialPass& src, H1::MaterialPass& dst,
					const std::string& vs_name, const std::string& ps_name)
				{
					const int counts[3] = {
						static_cast<unsigned char>(src.perPrimArgCount),
						static_cast<unsigned char>(src.perObjArgCount),
						static_cast<unsigned char>(src.stableArgCount)
					};
					const int iw3Total = counts[0] + counts[1] + counts[2];

					if (iw3Total <= 0 || !src.args)
					{
						dst.perPrimArgCount = static_cast<unsigned char>(counts[0]);
						dst.perObjArgCount = static_cast<unsigned char>(counts[1]);
						dst.stableArgCount = static_cast<unsigned char>(counts[2]);
						dst.args = nullptr;
						return true;
					}

					std::vector<H1::MaterialShaderArgument> groups[3];
					int maxReg[3] = { -1, -1, -1 }; // highest vec4 register per cbuffer group

					int idx = 0;
					for (int group = 0; group < 3; group++)
					{
						for (int c = 0; c < counts[group]; c++, idx++)
						{
							const auto& a = src.args[idx];

							bool is_pixel_stage = false;
							const int h1_type = techset_translate::map_arg_type_iw3_to_h1(a.type, is_pixel_stage);
							if (h1_type < 0)
							{
								ZONETOOL_ERROR("Unmappable arg type %d (dest %d) in techset \"%s\"; skipping technique.",
									a.type, a.dest, ctx.techset_name.data());
								return false;
							}
							if (h1_type >= H1::MTL_ARG_COUNT)
							{
								ZONETOOL_ERROR("Translated arg type out of range (%d) in techset \"%s\".", h1_type, ctx.techset_name.data());
								return false;
							}

							H1::MaterialShaderArgument o{};
							o.type = static_cast<unsigned char>(h1_type);
							o.shader = is_pixel_stage ? H1_ARG_STAGE_PIXEL : H1_ARG_STAGE_VERTEX;
							o.dest = a.dest; // preserve original DX9 register / slot

							switch (h1_type)
							{
							case H1::MTL_ARG_CODE_CONST:
							{
								const int h1_idx = techset_translate::map_code_const_iw3_to_h1(a.u.codeConst.index);
								if (h1_idx < 0)
								{
									ZONETOOL_ERROR("Unmappable code const %d in techset \"%s\"; skipping technique.",
										a.u.codeConst.index, ctx.techset_name.data());
									return false;
								}
								o.u.codeConst.index = static_cast<unsigned short>(h1_idx);
								o.u.codeConst.firstRow = static_cast<unsigned char>(a.u.codeConst.firstRow);
								o.u.codeConst.rowCount = static_cast<unsigned char>(a.u.codeConst.rowCount);
								const int span = a.dest + std::max<int>(1, a.u.codeConst.rowCount) - 1;
								if (span > maxReg[group]) maxReg[group] = span;
								groups[group].push_back(o);
								break;
							}
							case H1::MTL_ARG_MATERIAL_CONST:
								o.u.nameHash = a.u.nameHash;
								if (a.dest > maxReg[group]) maxReg[group] = a.dest;
								groups[group].push_back(o);
								break;
							case H1::MTL_ARG_LITERAL_CONST:
							{
								auto* lit = ctx.mem.alloc<float>(4);
								if (a.u.literalConst)
								{
									std::memcpy(lit, a.u.literalConst, sizeof(float) * 4);
								}
								o.u.literalConst = lit;
								if (a.dest > maxReg[group]) maxReg[group] = a.dest;
								groups[group].push_back(o);
								break;
							}
							case H1::MTL_ARG_CODE_TEXTURE:
							{
								// DX9 combined code sampler -> H1 code texture + companion code sampler.
								const int h1_idx = techset_translate::map_code_sampler_iw3_to_h1(a.u.codeSampler);
								if (h1_idx < 0)
								{
									ZONETOOL_ERROR("Unmappable code sampler %u in techset \"%s\"; skipping technique.",
										a.u.codeSampler, ctx.techset_name.data());
									return false;
								}
								o.u.codeSampler = static_cast<unsigned int>(h1_idx);
								groups[group].push_back(o);

								H1::MaterialShaderArgument s{};
								s.type = H1::MTL_ARG_CODE_SAMPLER;
								s.shader = o.shader;
								s.dest = a.dest;
								s.u.codeSampler = static_cast<unsigned int>(h1_idx);
								groups[group].push_back(s);
								break;
							}
							case H1::MTL_ARG_MATERIAL_TEXTURE:
							{
								// DX9 combined material sampler -> H1 material texture + companion sampler.
								o.u.nameHash = a.u.nameHash;
								groups[group].push_back(o);

								H1::MaterialShaderArgument s{};
								s.type = H1::MTL_ARG_MATERIAL_SAMPLER;
								s.shader = o.shader;
								s.dest = a.dest;
								s.u.nameHash = a.u.nameHash;
								groups[group].push_back(s);
								break;
							}
							default:
								o.u.nameHash = a.u.nameHash;
								groups[group].push_back(o);
								break;
							}

							// One metadata record per IW3 arg (keyed on the primary H1 arg).
							const auto& shader_name = is_pixel_stage ? ps_name : vs_name;
							if (!shader_name.empty())
							{
								accumulate_shader_arg(ctx, shader_name, is_pixel_stage ? "ps" : "vs", a, o, group);
							}
						}
					}

					const int total = static_cast<int>(groups[0].size() + groups[1].size() + groups[2].size());
					auto* h1_args = ctx.mem.alloc<H1::MaterialShaderArgument>(total);
					int w = 0;
					for (int group = 0; group < 3; group++)
					{
						for (const auto& a : groups[group])
						{
							h1_args[w++] = a;
						}
					}

					dst.args = h1_args;
					dst.perPrimArgCount = static_cast<unsigned char>(groups[0].size());
					dst.perObjArgCount = static_cast<unsigned char>(groups[1].size());
					dst.stableArgCount = static_cast<unsigned char>(groups[2].size());
					dst.perPrimArgSize = static_cast<unsigned short>((maxReg[0] + 1) * 16);
					dst.perObjArgSize = static_cast<unsigned short>((maxReg[1] + 1) * 16);
					dst.stableArgSize = static_cast<unsigned short>((maxReg[2] + 1) * 16);

					unsigned int cbFlags = 0;
					if (dst.perPrimArgSize) cbFlags |= H1::CUSTOM_BUFFER_PER_PRIM;
					if (dst.perObjArgSize) cbFlags |= H1::CUSTOM_BUFFER_PER_OBJECT;
					if (dst.stableArgSize) cbFlags |= H1::CUSTOM_BUFFER_PER_STABLE;
					dst.customBufferFlags = cbFlags;

					return true;
				}

				// --------------------------------------------------------------
				// Build + dump a single H1 technique from an IW3 technique.
				// Returns the H1 technique (header + passArray) or nullptr on
				// failure (unmappable arg etc.).
				// --------------------------------------------------------------
				H1::MaterialTechnique* build_technique(dump_context& ctx, const iw3::MaterialTechnique* src, const std::string& tech_name)
				{
					const int passCount = src->hdr.numPasses;
					if (passCount <= 0)
					{
						return nullptr;
					}

					auto* header = ctx.mem.alloc<H1::MaterialTechniqueHeader>();
					header->name = ctx.mem.dup(tech_name);
					header->flags = src->hdr.flags;
					header->passCount = static_cast<unsigned short>(passCount);

					auto* passes = ctx.mem.alloc<H1::MaterialPass>(passCount);

					for (int p = 0; p < passCount; p++)
					{
						const auto& sp = src->pass[p];
						auto& dp = passes[p];

						std::string vs_name, ps_name;

						if (sp.vertexShader && sp.vertexShader->name)
						{
							vs_name = sp.vertexShader->name;
							auto* h1_vs = ctx.mem.alloc<H1::MaterialVertexShader>();
							h1_vs->name = ctx.mem.dup(vs_name);
							dp.vertexShader = h1_vs;

							if (ctx.written_shaders.insert("vs:" + vs_name).second)
							{
								stage_shader_blob(vs_name, "vs9", sp.vertexShader->prog.loadDef.program,
									sp.vertexShader->prog.loadDef.programSize);
							}
						}

						if (sp.pixelShader && sp.pixelShader->name)
						{
							ps_name = sp.pixelShader->name;
							auto* h1_ps = ctx.mem.alloc<H1::MaterialPixelShader>();
							h1_ps->name = ctx.mem.dup(ps_name);
							dp.pixelShader = h1_ps;

							if (ctx.written_shaders.insert("ps:" + ps_name).second)
							{
								stage_shader_blob(ps_name, "ps9", sp.pixelShader->prog.loadDef.program,
									sp.pixelShader->prog.loadDef.programSize);
							}
						}

						if (sp.vertexDecl)
						{
							const auto vd_name = vertexdecl_name(sp.vertexDecl);
							auto* h1_vd = build_vertexdecl(ctx, sp.vertexDecl, vd_name);
							dp.vertexDecl = h1_vd;
							if (ctx.written_decls.insert(vd_name).second)
							{
								dump_vertexdecl_file(h1_vd);
							}
						}

						dp.pixelOutputMask = 0x1; // single render target (assumption)
						dp.precompiledIndex = 0;   // fully custom, not precompiled
						dp.stageConfig = 0;
						dp.customSamplerFlags = static_cast<unsigned char>(sp.customSamplerFlags);

						if (!translate_pass_args(ctx, sp, dp, vs_name, ps_name))
						{
							return nullptr;
						}
					}

					// The .technique file is written as: header, passArray,
					// header name, then per-pass shader/decl asset refs + args.
					// Matches x64-zt parse_technique / zonetool ITechset::dump_technique.
					const auto path = "techsets\\"s + tech_name + ".technique"s;
					zonetool::assetmanager::dumper dumper;
					if (dumper.open(path))
					{
						dumper.dump_single(header);
						dumper.dump_array(passes, header->passCount);
						dumper.dump_string(header->name);

						for (unsigned short i = 0; i < header->passCount; i++)
						{
							auto& pass = passes[i];
							if (pass.vertexShader) dumper.dump_asset(pass.vertexShader);
							if (pass.vertexDecl) dumper.dump_asset(pass.vertexDecl);
							if (pass.hullShader) dumper.dump_asset(pass.hullShader);
							if (pass.domainShader) dumper.dump_asset(pass.domainShader);
							if (pass.pixelShader) dumper.dump_asset(pass.pixelShader);

							if (pass.args)
							{
								const int total = pass.perPrimArgCount + pass.perObjArgCount + pass.stableArgCount;
								dumper.dump_array(pass.args, total);
								for (int arg = 0; arg < total; arg++)
								{
									if (pass.args[arg].type == H1::MTL_ARG_LITERAL_CONST && pass.args[arg].u.literalConst)
									{
										dumper.dump_array(pass.args[arg].u.literalConst, 4);
									}
								}
							}
						}
						dumper.close();
					}

					// Assemble a contiguous MaterialTechnique (header + passArray)
					// so we can hand back one pointer for the techset writer.
					auto* tech = reinterpret_cast<H1::MaterialTechnique*>(
						ctx.mem.alloc<std::uint8_t>(sizeof(H1::MaterialTechniqueHeader) + sizeof(H1::MaterialPass) * passCount));
					std::memcpy(&tech->hdr, header, sizeof(H1::MaterialTechniqueHeader));
					std::memcpy(tech->passArray, passes, sizeof(H1::MaterialPass) * passCount);
					return tech;
				}

				// --------------------------------------------------------------
				// State files (.statebits / .statebitsmap / .stateinfo / .cbi / .cbt)
				// --------------------------------------------------------------
				void dump_state_files(dump_context& ctx, const bool* populated_h1_slots)
				{
					auto* material = ctx.material;
					const auto* iw3_statebits = reinterpret_cast<const iw3::GfxStateBits*>(material->stateBitsTable);
					const int stateBitsCount = material->stateBitsCount;

					// .stateinfo
					{
						const auto path = "techsets\\state\\"s + ctx.techset_name + "\\"s + ctx.material_name + ".stateinfo"s;
						ordered_json j = {};
						j["stateFlags"] = material->stateFlags;
						auto file = zonetool::filesystem::file(path);
						file.open("wb");
						if (file.get_fp())
						{
							const auto dump = j.dump(4);
							file.write(dump.data(), dump.size(), 1);
						}
						file.close();
					}

					// .statebits: 240-byte per-technique statebit index.
					// NOTE: the IW3 material's per-technique stateBitsEntry mapping
					// is discarded by the IW3->IW4->IW5 convert() chain (it memsets
					// the entries to 0xFF), so we cannot recover which statebit each
					// technique used. We approximate by pointing every populated H1
					// technique slot at statebit index 0. This is exact for the
					// common single-statebit material and a documented approximation
					// otherwise.
					{
						unsigned char statebits[H1::MaterialTechniqueType::TECHNIQUE_COUNT];
						std::memset(statebits, 0xFF, sizeof(statebits));
						if (stateBitsCount > 0)
						{
							for (int i = 0; i < H1::MaterialTechniqueType::TECHNIQUE_COUNT; i++)
							{
								if (populated_h1_slots[i])
								{
									statebits[i] = 0;
								}
							}
						}
						const auto path = "techsets\\state\\"s + ctx.techset_name + "\\"s + ctx.material_name + ".statebits"s;
						auto file = zonetool::filesystem::file(path);
						file.open("wb");
						if (file.get_fp())
						{
							fwrite(statebits, H1::MaterialTechniqueType::TECHNIQUE_COUNT, 1, file.get_fp());
						}
						file.close();
					}

					// .statebitsmap: converted GfxStateBits table.
					{
						ordered_json j = {};
						for (int i = 0; i < stateBitsCount; i++)
						{
							H1::GfxStateBits h1sb;
							convert_statebits(iw3_statebits[i], h1sb);

							ordered_json entry;
							for (int k = 0; k < 6; k++)
							{
								entry["loadBits"][k] = h1sb.loadBits[k];
							}
							for (int k = 0; k < 11; k++)
							{
								entry["depthStencilStateBits"][k] = 0; // resolved by H1 linker
							}
							for (int k = 0; k < 3; k++)
							{
								entry["blendStateBits"][k] = 0; // resolved by H1 linker
							}
							entry["rasterizerState"] = h1sb.rasterizerState;
							j[i] = entry;
						}

						const auto path = "techsets\\state\\"s + ctx.techset_name + "\\"s + ctx.material_name + ".statebitsmap"s;
						auto file = zonetool::filesystem::file(path);
						file.open("wb");
						if (file.get_fp())
						{
							const auto dump = j.dump(4);
							file.write(dump.data(), dump.size(), 1);
						}
						file.close();
					}

					// .cbi: 240-byte per-technique constant-buffer index. All 0xFF
					// means no technique references a custom constant-buffer def;
					// H1 binds material constants through the standard path.
					if (material->constantCount)
					{
						unsigned char cbi[H1::MaterialTechniqueType::TECHNIQUE_COUNT];
						std::memset(cbi, 0xFF, sizeof(cbi));
						const auto path = "techsets\\constantbuffer\\"s + ctx.techset_name + "\\"s + ctx.material_name + ".cbi"s;
						auto file = zonetool::filesystem::file(path);
						file.open("wb");
						if (file.get_fp())
						{
							fwrite(cbi, H1::MaterialTechniqueType::TECHNIQUE_COUNT, 1, file.get_fp());
						}
						file.close();

						// .cbt: emit an explicit empty table (count 0). A correct,
						// populated constant-buffer table requires DX11 reflection
						// of the translated SM5 shaders and is therefore produced by
						// the shader-translation CLI, which overwrites this file.
						// Emitting an empty one prevents the H1 linker's directory
						// fallback from picking up a sibling material's stale .cbt.
						const auto cbt_path = "techsets\\constantbuffer\\"s + ctx.techset_name + "\\"s + ctx.material_name + ".cbt"s;
						zonetool::assetmanager::dumper cbt;
						if (cbt.open(cbt_path))
						{
							cbt.dump_int(0);
							cbt.dump_array(reinterpret_cast<H1::MaterialConstantBufferDef*>(nullptr), 0);
							cbt.close();
						}
					}
				}
			}

			// ----------------------------------------------------------------
			// Public entry point
			// ----------------------------------------------------------------
			void dump_techset_iw3(const std::string& techset_name, Material* material)
			{
				if (!material || !material->techniqueSet)
				{
					return;
				}

				const auto* src = reinterpret_cast<const iw3::MaterialTechniqueSet*>(material->techniqueSet);

				dump_context ctx;
				ctx.techset_name = techset_name;
				ctx.material_name = material->name ? material->name : "";
				ctx.material = material;

				ZONETOOL_INFO("Dumping custom IW3->H1 techset \"%s\" (material \"%s\")",
					techset_name.data(), ctx.material_name.data());

				// Build the H1 techset (240 slots) by remapping IW3's 34 slots.
				auto* h1_set = ctx.mem.alloc<H1::MaterialTechniqueSet>();
				h1_set->name = ctx.mem.dup(techset_name);
				h1_set->flags = 0;
				h1_set->worldVertFormat = static_cast<unsigned char>(src->worldVertFormat);
				h1_set->preDisplacementOnlyCount = 0;

				bool populated[H1::MaterialTechniqueType::TECHNIQUE_COUNT] = { false };

				for (int i = 0; i < iw3::TECHNIQUE_COUNT; i++)
				{
					if (!src->techniques[i])
					{
						continue;
					}

					const int h1_slot = techset_translate::map_technique_slot_iw3_to_h1(i);
					if (h1_slot < 0 || h1_slot >= H1::MaterialTechniqueType::TECHNIQUE_COUNT)
					{
						ZONETOOL_ERROR("Unmappable technique slot %d (%s) for techset \"%s\"; skipping.",
							i, techset_translate::iw3_technique_slot_name(i), techset_name.data());
						continue;
					}

					const std::string tech_name = techset_name + "_" + std::to_string(h1_slot);
					auto* tech = build_technique(ctx, src->techniques[i], tech_name);
					if (!tech)
					{
						// build_technique already logged the reason; skip this slot
						// rather than emit broken data.
						continue;
					}

					h1_set->techniques[h1_slot] = tech;
					populated[h1_slot] = true;

					ZONETOOL_INFO("  technique slot %d (%s) -> H1 slot %d (%s)",
						i, techset_translate::iw3_technique_slot_name(i),
						h1_slot, techset_translate::h1_technique_slot_name(h1_slot));
				}

				// Write the .techset (assetmanager tagged binary):
				//   single(MaterialTechniqueSet), name string, then for each
				//   populated slot: technique name string (the .technique holds
				//   the rest). Matches x64-zt parse_internal / ITechset::dump.
				{
					const auto path = "techsets\\"s + techset_name + ".techset"s;
					zonetool::assetmanager::dumper dumper;
					if (dumper.open(path))
					{
						dumper.dump_single(h1_set);
						dumper.dump_string(h1_set->name);
						for (int i = 0; i < H1::MaterialTechniqueType::TECHNIQUE_COUNT; i++)
						{
							if (h1_set->techniques[i])
							{
								dumper.dump_string(h1_set->techniques[i]->hdr.name);
							}
						}
						dumper.close();
					}
				}

				// State + constant-buffer sidecar files.
				dump_state_files(ctx, populated);

				// DX9 shader metadata for the translation CLI.
				write_shader_metadata(ctx);
			}
		}
	}
}
