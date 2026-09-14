#pragma once

namespace ZoneTool::IW5
{
	namespace IW7Converter
	{
		IW7::GfxImage* convert(GfxImage* asset, allocator& allocator);

		IW7::GfxImage* GenerateLightmapSecondUnorm(const char* name, unsigned short width,
			unsigned short height, allocator& mem);

		// The IES profile lookup GfxWorldDraw binds. IW5 has no IES lights, so this is the
		// degenerate table stock ships on every map without them.
		IW7::GfxImage* GenerateIesLookup(allocator& mem);

		// Folds IW5's per-probe cubes into the single BC6H cube array IW7 binds as
		// *reflection_probe_array.
		//
		// Takes the probes already converted by the image dumper rather than the GfxWorld's own
		// GfxImage pointers: by the time the world converts, the game has consumed each probe's
		// loadDef to build its D3D texture, so reading pixels back through it yields whatever now
		// occupies that memory (measured on mp_test_h1: every probe claimed to be 512x512 L8).
		//
		// A null entry becomes a black element rather than being dropped - the array is indexed
		// by probe number, so a missing one has to keep its slot.
		IW7::GfxImage* GenerateReflectionProbeArray(const std::vector<IW7::GfxImage*>& probes,
			allocator& mem);
	}
}