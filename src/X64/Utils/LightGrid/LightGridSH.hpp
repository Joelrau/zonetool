#pragma once

// Bidirectional converter between the legacy "LDR" light grid colors
// (GfxLightGridColors, unsigned char rgb[56][3], used by IW3..IW5)
// and the IW6+/H1 spherical-harmonics palette bitstream
// (paletteEntryAddress / paletteBitstream, decoded by
// R_DecodeLightGridSphericalHarmonics and evaluated by
// R_SphericalHarmonicsToColorsLDR in iw6mp64_ship / h1_mp64_ship).
//
// The decode / evaluate paths are bit-exact mirrors of the game code
// (reversed from iw6_ds_xenon.exe / iw6mp64_ship_dump).
// The encode / fit paths are the inverse operations, written from scratch
// (the shipped games only ever decode; encoding was done by the linker).
//
// Standalone: no zonetool dependencies.

#include <cstdint>
#include <vector>

namespace lightgrid_sh
{
	// mirrors GfxLightGridEncoderConfig (copied from
	// GfxLightGrid::rangeExponent8BitsEncoding..rangeExponent16BitsEncoding).
	// a palette entry stores one power-of-two range exponent per channel;
	// the exponent picks the quantization width:
	//   exp <= range_exp_8bits                    -> 8 bits / coeff
	//   range_exp_8bits < exp <= range_exp_12bits -> 12 bits / coeff
	//   otherwise                                 -> 16 bits / coeff
	struct encoder_config
	{
		std::int8_t range_exp_8bits;
		std::int8_t range_exp_12bits;
		std::int8_t range_exp_16bits;
	};

	// iw6 default is { 0, 4, 23 }. our encoder intentionally never emits
	// 12-bit blocks (the game's 12-bit reader has an overlapping-byte quirk
	// that makes exact re-encoding impossible), so the recommended config
	// for authored palettes disables the 12-bit window entirely:
	constexpr encoder_config default_encode_config{ 0, 0, 23 };

	// 9 spherical harmonics coefficients per channel (r, g, b), matching
	// GfxLightGridSphericalHarmonics (__m128 coeffs[9], w unused).
	// basis (see R_SphericalHarmonicsToColorsLDR):
	//   { 1, x, y, z, x*z, y*z, x*y, 3*z*z - 1, x*x - y*y }
	struct sh_coeffs
	{
		float v[9][3];
	};

	// the 56 evaluation directions (gridBasisDirs). generated at startup by
	// the game; values below were extracted from a live iw6mp64_ship dump
	// (they are normalized with the CPU's approximate rsqrt, hence not
	// perfectly unit length - kept verbatim on purpose).
	extern const float grid_basis_dirs[56][3];

	// ---- low level: single palette entry <-> SH coefficients -------------

	// bit-exact mirror of R_DecodeLightGridSphericalHarmonics.
	// returns the total (2-byte aligned) size of the entry in the stream.
	unsigned int decode_entry(const encoder_config& cfg, const unsigned char* stream, sh_coeffs& out);

	// inverse of decode_entry. appends one entry to the bitstream and
	// returns its start offset (the value for paletteEntryAddress[i]).
	// never emits 12-bit blocks: cfg.range_exp_12bits must equal
	// cfg.range_exp_8bits (see default_encode_config).
	int encode_entry(const encoder_config& cfg, const sh_coeffs& sh, std::vector<unsigned char>& stream);

	// ---- low level: SH coefficients <-> 56 directional colors ------------

	// bit-exact mirror of R_SphericalHarmonicsToColorsLDR (3 SH bands,
	// contrastGain 0): rgb[d] = trunc(min(sqrt(max(eval, 2^-18)) * 127.5, 255))
	void sh_to_ldr_colors(const sh_coeffs& sh, unsigned char rgb[56][3]);

	// same evaluation but keeps the linear (HDR) values,
	// for GfxLightGrid::skyLightGridColors / defaultLightGridColors.
	void sh_to_hdr_colors(const sh_coeffs& sh, float rgb[56][3]);

	// least-squares fit of the 9-coefficient basis to 56 linear samples.
	// sh_to_hdr_colors(fit(x)) reproduces x exactly whenever x is
	// SH-representable, so SH -> LDR -> SH round-trips cleanly.
	void hdr_colors_to_sh(const float rgb[56][3], sh_coeffs& out);

	// LDR bytes -> linear -> fit. linear = (byte / 127.5)^2, the exact
	// inverse of the game's LDR packing.
	void ldr_colors_to_sh(const unsigned char rgb[56][3], sh_coeffs& out);

	// LDR bytes -> linear HDR floats (for sky/default grid colors).
	void ldr_colors_to_hdr(const unsigned char rgb[56][3], float out[56][3]);

	// ---- high level: whole palette ----------------------------------------

	struct palette
	{
		encoder_config config;                 // -> rangeExponent*Encoding
		std::vector<int> entry_address;        // -> paletteEntryAddress
		std::vector<unsigned char> bitstream;  // -> paletteBitstream
	};

	// old -> new: build a palette from `color_count` legacy
	// GfxLightGridColors entries (`colors` = first byte of colors[0].rgb,
	// entries are 168 bytes apart). entry i of the palette corresponds to
	// colorsIndex i, so GfxLightGridEntry::colorsIndex can be kept as-is.
	palette build_palette_from_ldr(const unsigned char* colors, unsigned int color_count);

	// new -> old: decode every palette entry back into legacy
	// GfxLightGridColors bytes (out = first byte of out_colors[0].rgb,
	// 168 bytes per entry).
	void extract_palette_to_ldr(const encoder_config& cfg, const unsigned char* bitstream,
		const int* entry_address, unsigned int entry_count, unsigned char* out_colors);

	// ---- verification ------------------------------------------------------

	struct roundtrip_stats
	{
		unsigned int entries;
		int max_byte_error;     // worst |b - b'| over all entries/dirs/channels
		double avg_byte_error;
	};

	// SH -> LDR -> SH accuracy check on an existing (game) palette:
	// for every entry, decode -> evaluate to LDR bytes -> fit -> re-encode
	// (with default_encode_config) -> decode -> evaluate again, and compare
	// the two LDR results.
	roundtrip_stats verify_sh_roundtrip(const encoder_config& cfg, const unsigned char* bitstream,
		const int* entry_address, unsigned int entry_count);
}
