#pragma once

// ============================================================================
// Custom IW3 (CoD4, DX9) -> H1 (MWR, DX11) techset dumper.
//
// This emits a fully custom H1 techset asset file-set (techset / technique /
// vertexdecl / statebits / stateinfo / constant-buffer-index) that is
// byte-compatible with the x64-zt H1 linker readers
// (x64-zt/src/zonetool/zonetool/h1/assets/techset.cpp), plus staged DX9 shader
// blobs (.vs9/.ps9 + .json) for a separate DX9->SM5 shader-translation CLI.
//
// Entry point is invoked from the H1 Material dumper when custom mode is active
// AND the source game is IW3 (see Material.cpp). When the source techset pointer
// reaches the H1 dumper it still points at IW3-layout structs (the IW3->IW4->IW5
// convert() chain reinterpret_casts the techniqueSet without rebuilding it), so
// everything here interprets it with IW3 struct definitions.
// ============================================================================

namespace ZoneTool
{
	namespace IW5
	{
		struct Material; // ZoneTool::IW5::Material (the converted material we receive)

		namespace H1Dumper
		{
			// Runtime toggle for the fully-custom IW3->H1 techset port.
			// Defaults to false so the standard remap path (and its byte-identical
			// JSON output) is completely unchanged unless explicitly enabled.
			extern bool custom_iw3_techset_mode;

			// True when the custom techset port should run for this material,
			// i.e. custom mode is enabled and the dump source is IW3.
			bool custom_techset_active();

			// Dump the complete custom H1 techset file-set for an IW3-sourced
			// material. 'techset_name' is the (suffix-cleaned) original IW3 techset
			// name that will also be written into the material JSON.
			void dump_techset_iw3(const std::string& techset_name, Material* material);
		}
	}
}
