#pragma once

namespace ZoneTool::IW5::IW7Dumper
{
	void dump(GfxImage* asset);

	// ZONETOOL_FAST_DUMP=1: skip rebuilding ordinary textures, whose files are already in the
	// staging tree from the previous run. Encoding them (BC7 and BC6H, every mip) is most of a
	// dump's cost, and none of it changes when only world or lighting code has. Map images -
	// lightmaps and reflection probes - are always rebuilt, since the GfxWorld dumper assembles
	// its probe array and lightmap set from them as they pass through.
	bool fast_dump_enabled();

	// *reflection_probeN images, kept as the image dumper converts them so the GfxWorld dumper
	// can fold them into *reflection_probe_array. They have to be captured here: the game
	// consumes each probe's loadDef to build its D3D texture, so by the time the world is
	// converted there are no pixels left to read. Indexed by the probe number in the name;
	// entries the zone never supplied stay null.
	const std::vector<IW7::GfxImage*>& reflection_probes();
	void clear_reflection_probes();

	// *lightmapN_primary and *lightmapN_secondary as the image dumper converted them, kept for the
	// same reason as the probes: the GfxWorld's own GfxImage pointers carry stale dimensions (it
	// reports the primary as 512x512 where the asset itself is 1024x1024), and the three lightmap
	// textures have to be reconciled onto one grid once all of them have been seen.
	const std::vector<IW7::GfxImage*>& lightmap_primary_images();
	const std::vector<IW7::GfxImage*>& lightmap_secondary_images();
	void clear_lightmap_images();
}