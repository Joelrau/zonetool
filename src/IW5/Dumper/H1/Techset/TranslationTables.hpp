#pragma once

// =====================================================================================
// IW3 (CoD4, DX9) -> H1 (MWR, DX11) techset/material translation tables.
//
// This module provides pure, self-contained lookup functions that translate the small
// integer indices found inside an IW3 MaterialTechniqueSet (code constant sources, code
// sampler/texture sources, technique slots, vertex stream routing and shader-argument
// types) into their H1 equivalents so an IW3 techset can be rebuilt as a fully custom H1
// techset (no remapping onto stock H1 techsets).
//
// All functions are stateless and take/return plain integers, so this header pulls in no
// game structs. Enum values for both sides are embedded as local constants in the .cpp
// (see the source-citation comments there); nothing here depends on IW3/H1 struct headers.
// =====================================================================================

namespace ZoneTool
{
	namespace IW5::H1Dumper
	{
		namespace techset_translate
		{
			// code constant index: IW3 CONST_SRC_CODE_* -> H1 CONST_SRC_CODE_*. Returns -1 if unmappable.
			int map_code_const_iw3_to_h1(int iw3_index);

			// code sampler/texture index: IW3 -> H1 (H1 has 77 code textures). Returns -1 if unmappable.
			int map_code_sampler_iw3_to_h1(int iw3_index);

			// technique slot: IW3 slot (0..33) -> H1 slot (0..239). Returns -1 if no H1 equivalent.
			int map_technique_slot_iw3_to_h1(int iw3_slot);

			// stream routing: IW3 source/dest bytes -> H1 source/dest/mask bytes. Returns false if unmappable.
			bool map_stream_routing_iw3_to_h1(unsigned char iw3_source, unsigned char iw3_dest,
				unsigned char& h1_source, unsigned char& h1_dest, unsigned char& h1_mask);

			// arg type: IW3 MaterialShaderArgument.type -> H1 MaterialShaderArgumentType.
			// Sets is_pixel_stage based on the IW3 vertex/pixel arg type variants. Returns -1 if unmappable.
			//
			// NOTE for the caller:
			//  * H1 unified the old IW3 vertex/pixel arg-type split into a single type plus a per-arg
			//    "shader" stage bitmask. is_pixel_stage == true means the arg belongs to the pixel stage
			//    (caller should set MaterialShaderArgument::shader to the pixel-shader flag 0x10); false
			//    means the vertex stage (flag 0x2). Source: x64-zt iw6->h1 converter techset.cpp:1221-1232.
			//  * DX9 IW3 "samplers" are combined texture+sampler objects. In DX11/H1 a texture and a
			//    sampler are separate args. This function maps an IW3 sampler arg to its H1 *texture* arg
			//    (CODE_TEXTURE / MATERIAL_TEXTURE). The caller must additionally synthesize the companion
			//    H1 sampler arg (CODE_SAMPLER / MATERIAL_SAMPLER) for that same register.
			int map_arg_type_iw3_to_h1(int iw3_type, bool& is_pixel_stage);

			// debug names
			const char* iw3_code_const_name(int iw3_index);
			const char* h1_code_const_name(int h1_index);
			const char* iw3_technique_slot_name(int iw3_slot);
			const char* h1_technique_slot_name(int h1_slot);
		}
	}
}
