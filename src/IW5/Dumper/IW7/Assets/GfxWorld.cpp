#include "stdafx.hpp"

#include "GfxWorld.hpp"
#include "Converter/IW7/Assets/GfxWorld.hpp"
#include "Converter/IW7/Assets/GfxImage.hpp"
#include "GfxImage.hpp"
#include "IW7/Assets/GfxWorld.hpp"
#include "IW7/Assets/GfxWorldTr.hpp"
#include "IW7/Assets/GfxLightMap.hpp"
#include "IW7/Assets/GfxImage.hpp"

namespace ZoneTool::IW5::IW7Dumper
{
	void dump(GfxWorld* asset)
	{
		// generate IW7 gfxworld
		allocator allocator;
		auto* iw7_asset = IW7Converter::convert(asset, allocator);

		// dump IW7 gfxworld
		IW7::IGfxWorld::dump(iw7_asset);

		// dump IW7 gfxworld_tr
		IW7::IGfxWorldTr::dump(iw7_asset->draw.transientZones[0]);

		// *reflection_probe_array is synthesised from IW5's per-probe cubes, so nothing else in
		// the pipeline writes it - same situation as *lightmapN_secondunorm below. The pixels
		// come from what the image dumper already converted, not from the world's own GfxImage
		// pointers, whose loadDefs the game has consumed by now.
		//
		// The world's probe count is what indexes the array, so it decides the element count:
		// any probe the image dumper never saw keeps its slot as a black element.
		{
			auto probes = reflection_probes();
			probes.resize(asset->draw.reflectionProbeCount, nullptr);

			// Skip IW5's invalid-probe sentinel, exactly as the converter does when it sizes the
			// probe metadata that indexes this array - see first_reflection_probe.
			const auto first = IW7Converter::first_reflection_probe(asset->draw.reflectionProbeCount);
			probes.erase(probes.begin(), probes.begin() + first);

			if (auto* probe_array = IW7Converter::GenerateReflectionProbeArray(probes, allocator))
			{
				IW7::IGfxImage::dump(probe_array);
			}
		}
		clear_reflection_probes();

		// Same story as the probe array: the converter builds *ieslookup because IW5 has no IES
		// lights to build it from, so nothing else in the pipeline writes it out.
		auto* ies = iw7_asset->draw.iesLookupTexture;
		if (ies && ies->pixelData)
		{
			IW7::IGfxImage::dump(ies);
		}

		// dump IW7 gfxlightmaps
		for (int i = 0; i < asset->draw.lightmapCount; i++)
		{
			IW7::IGfxLightMap::dump(iw7_asset->draw.lightMaps[i]);

			// GfxLightMap::textures[2] (*lightmapN_secondunorm) is IW7's lightmap direction texture and
			// has no IW5 counterpart, so nothing else in the pipeline creates it and the reference the
			// converter emits dangles. Synthesize the neutral one. Size it from the primary lightmap:
			// every stock IW7 map pairs secondunorm 1:1 with primary, while secondary is twice that
			// height because it stacks two radiance pages.
			const auto* source = asset->draw.lightmaps[i].primary
				? asset->draw.lightmaps[i].primary
				: asset->draw.lightmaps[i].secondary;
			if (source)
			{
				ZONETOOL_INFO("lightmap %d: primary %s %ux%u, secondary %s %ux%u -> secondunorm %ux%u",
					i,
					asset->draw.lightmaps[i].primary ? asset->draw.lightmaps[i].primary->name : "(none)",
					asset->draw.lightmaps[i].primary ? asset->draw.lightmaps[i].primary->width : 0,
					asset->draw.lightmaps[i].primary ? asset->draw.lightmaps[i].primary->height : 0,
					asset->draw.lightmaps[i].secondary ? asset->draw.lightmaps[i].secondary->name : "(none)",
					asset->draw.lightmaps[i].secondary ? asset->draw.lightmaps[i].secondary->width : 0,
					asset->draw.lightmaps[i].secondary ? asset->draw.lightmaps[i].secondary->height : 0,
					source->width, source->height);

				IW7::IGfxImage::dump(IW7Converter::GenerateLightmapSecondUnorm(
					va("*lightmap%d_secondunorm", i).data(), source->width, source->height, allocator));
			}
			else
			{
				ZONETOOL_WARNING("lightmap %d has no primary or secondary texture, "
					"*lightmap%d_secondunorm not emitted", i, i);
			}
		}

		clear_lightmap_images();
	}
}