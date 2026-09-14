#pragma once

namespace ZoneTool::IW5
{
	namespace IW7Converter
	{
		// A source image decoded to 8-bit RGBA, mip chain largest first.
		struct decoded_image
		{
			unsigned short width = 0;
			unsigned short height = 0;
			std::vector<std::vector<std::uint8_t>> mips;   // 4 bytes per texel
		};

		// Decodes an IW5/IW3 image (resident loadDef or .iwi) to RGBA. False when it cannot be
		// found or its format has no decoder.
		bool decode_source(GfxImage* asset, decoded_image& out);

		// IW7's base-layer pair. `spec` may be null, in which case gloss falls back to a constant
		// and the colour map is treated as a pure dielectric.
		//
		//   _packed_cs  semantic 14, BC7: RGB albedo, A metalness/reflectance
		//   _packed_ng  semantic 15, BC7: R gloss, G normal X, B occlusion, A normal Y
		//
		// See docs/iw7-packed-textures.md - the normal is hemi-octahedral, not raw XY.
		IW7::GfxImage* build_packed_cs(const char* name, const decoded_image& colour,
			const decoded_image* spec, allocator& mem);

		IW7::GfxImage* build_packed_ng(const char* name, const decoded_image& normal,
			const decoded_image* spec, allocator& mem);

		// The opacity slot a pa0 technique binds at semantic 16, taken from the colour map's
		// alpha. BC4, single channel. Only meaningful for a technique that reads alpha - a
		// blend or alpha-test one - since _packed_cs has already spent its own alpha on
		// reflectance.
		IW7::GfxImage* build_packed_a(const char* name, const decoded_image& colour,
			allocator& mem);
	}
}
