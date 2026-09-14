#include "stdafx.hpp"
#include "../Include.hpp"

#include "GfxImage.hpp"
#include "Assets/Material.hpp"

#include <cstring>
#include <cmath>
#include <limits>
#include <vector>

namespace ZoneTool::IW5
{
	namespace IW7Converter
	{
		typedef enum _D3DFORMAT : std::int32_t
		{
			D3DFMT_UNKNOWN = 0x0,
			D3DFMT_R8G8B8 = 0x14,
			D3DFMT_A8R8G8B8 = 0x15,
			D3DFMT_X8R8G8B8 = 0x16,
			D3DFMT_R5G6B5 = 0x17,
			D3DFMT_X1R5G5B5 = 0x18,
			D3DFMT_A1R5G5B5 = 0x19,
			D3DFMT_A4R4G4B4 = 0x1A,
			D3DFMT_R3G3B2 = 0x1B,
			D3DFMT_A8 = 0x1C,
			D3DFMT_A8R3G3B2 = 0x1D,
			D3DFMT_X4R4G4B4 = 0x1E,
			D3DFMT_A2B10G10R10 = 0x1F,
			D3DFMT_A8B8G8R8 = 0x20,
			D3DFMT_X8B8G8R8 = 0x21,
			D3DFMT_G16R16 = 0x22,
			D3DFMT_A2R10G10B10 = 0x23,
			D3DFMT_A16B16G16R16 = 0x24,
			D3DFMT_A8P8 = 0x28,
			D3DFMT_P8 = 0x29,
			D3DFMT_L8 = 0x32,
			D3DFMT_A8L8 = 0x33,
			D3DFMT_A4L4 = 0x34,
			D3DFMT_V8U8 = 0x3C,
			D3DFMT_L6V5U5 = 0x3D,
			D3DFMT_X8L8V8U8 = 0x3E,
			D3DFMT_Q8W8V8U8 = 0x3F,
			D3DFMT_V16U16 = 0x40,
			D3DFMT_A2W10V10U10 = 0x43,
			D3DFMT_UYVY = 0x59565955,
			D3DFMT_R8G8_B8G8 = 0x47424752,
			D3DFMT_YUY2 = 0x32595559,
			D3DFMT_G8R8_G8B8 = 0x42475247,
			D3DFMT_DXT1 = 0x31545844,
			D3DFMT_DXT2 = 0x32545844,
			D3DFMT_DXT3 = 0x33545844,
			D3DFMT_DXT4 = 0x34545844,
			D3DFMT_DXT5 = 0x35545844,
			D3DFMT_D16_LOCKABLE = 0x46,
			D3DFMT_D32 = 0x47,
			D3DFMT_D15S1 = 0x49,
			D3DFMT_D24S8 = 0x4B,
			D3DFMT_D24X8 = 0x4D,
			D3DFMT_D24X4S4 = 0x4F,
			D3DFMT_D16 = 0x50,
			D3DFMT_D32F_LOCKABLE = 0x52,
			D3DFMT_D24FS8 = 0x53,
			D3DFMT_D32_LOCKABLE = 0x54,
			D3DFMT_S8_LOCKABLE = 0x55,
			D3DFMT_L16 = 0x51,
			D3DFMT_VERTEXDATA = 0x64,
			D3DFMT_INDEX16 = 0x65,
			D3DFMT_INDEX32 = 0x66,
			D3DFMT_Q16W16V16U16 = 0x6E,
			D3DFMT_MULTI2_ARGB8 = 0x3154454D,
			D3DFMT_R16F = 0x6F,
			D3DFMT_G16R16F = 0x70,
			D3DFMT_A16B16G16R16F = 0x71,
			D3DFMT_R32F = 0x72,
			D3DFMT_G32R32F = 0x73,
			D3DFMT_A32B32G32R32F = 0x74,
			D3DFMT_CxV8U8 = 0x75,
			D3DFMT_A1 = 0x76,
			D3DFMT_A2B10G10R10_XR_BIAS = 0x77,
			D3DFMT_BINARYBUFFER = 0xC7,
			D3DFMT_FORCE_DWORD = 0x7FFFFFFF,
		} D3DFORMAT;

		std::unordered_map<D3DFORMAT, DXGI_FORMAT> d3d_dxgi_map =
		{
			{D3DFMT_A8R8G8B8, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB},
			{D3DFMT_L8, DXGI_FORMAT_R8_UNORM},
		};

		DXGI_FORMAT get_d3d_to_dxgi(std::uint8_t d3d)
		{
			if (d3d_dxgi_map.find((D3DFORMAT)d3d) != d3d_dxgi_map.end())
			{
				return d3d_dxgi_map[(D3DFORMAT)d3d];
			}
			return DXGI_FORMAT_UNKNOWN;
		}

		// Convert D3DFMT_A8R8G8B8 (0xAARRGGBB) to DXGI_FORMAT_R8G8B8A8 (0xAABBGGRR)
		void argb_to_rgba(const std::uint32_t* src, std::uint8_t* dest, std::size_t pixel_count)
		{
			for (std::size_t i = 0; i < pixel_count; ++i)
			{
				const auto pixel = src[i];

				dest[i * 4 + 0] = (pixel >> 16) & 0xFF; // Red
				dest[i * 4 + 1] = (pixel >> 8) & 0xFF;  // Green
				dest[i * 4 + 2] = pixel & 0xFF;         // Blue
				dest[i * 4 + 3] = (pixel >> 24) & 0xFF; // Alpha
			}
		}

		unsigned int Image_CountMipmaps(unsigned int imageFlags, unsigned int width, unsigned int height, unsigned int depth)
		{
			unsigned int mipRes;
			unsigned int mipCount;

			if ((imageFlags & 2) != 0)
				return 1;
			mipCount = 1;
			for (mipRes = 1; mipRes < width || mipRes < height || mipRes < depth; mipRes *= 2)
				++mipCount;
			return mipCount;
		}

		IW7::GfxImage* GenerateIW7GfxImage(GfxImage* asset, allocator& mem)
		{
			// allocate IW7 GfxImage structure
			const auto IW7_asset = mem.allocate<IW7::GfxImage>();

			IW7_asset->name = asset->name;
			IW7_asset->imageFormat = get_d3d_to_dxgi(asset->texture.loadDef->format);
			IW7_asset->mapType = static_cast<IW7::MapType>(asset->mapType);
			IW7_asset->semantic = (IW7::TextureSemantic)IW7::convert_semantic(asset->semantic);
			IW7_asset->category = (IW7::GfxImageCategory)asset->category;
			IW7_asset->flags = asset->flags;
			IW7_asset->dataLen1 = asset->texture.loadDef->resourceSize;
			IW7_asset->dataLen2 = asset->texture.loadDef->resourceSize;
			IW7_asset->width = asset->width;
			IW7_asset->height = asset->height;
			IW7_asset->depth = asset->depth;
			IW7_asset->numElements = 1;
			IW7_asset->levelCount = Image_CountMipmaps(asset->texture.loadDef->flags, asset->width, asset->height, asset->depth);
			IW7_asset->streamed = false;
			IW7_asset->pixelData = reinterpret_cast<unsigned char*>(&asset->texture.loadDef->data);

			if (IW7_asset->imageFormat == DXGI_FORMAT_UNKNOWN)
			{
				ZONETOOL_INFO("Possible DXGIFORMAT: %d", MFMapDX9FormatToDXGIFormat(asset->texture.loadDef->format));
				ZONETOOL_FATAL("Unknown DXGIFORMAT for image \"%s\" (%d)", asset->name, asset->texture.loadDef->format);
			}

			if (asset->texture.loadDef->format == D3DFMT_A8R8G8B8 &&
				(IW7_asset->imageFormat == DXGI_FORMAT_R8G8B8A8_UNORM ||
				 IW7_asset->imageFormat == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB))
			{
				auto* new_pixels = mem.allocate<unsigned char>(IW7_asset->dataLen1);
				argb_to_rgba(reinterpret_cast<std::uint32_t*>(IW7_asset->pixelData), new_pixels, IW7_asset->dataLen1 / 4);
				IW7_asset->pixelData = new_pixels;
			}
			else if (asset->texture.loadDef->format == D3DFMT_L8 && IW7_asset->imageFormat == DXGI_FORMAT_R8_UNORM)
			{
				// single channel, pixel data can be used as-is
			}
			else
			{
				ZONETOOL_FATAL("No known conversion for image \"%s\"", asset->name);
			}

			return IW7_asset;
		}

		// IW7 gives every lightmap a third texture, *lightmapN_secondunorm: the per-texel dominant
		// light direction for lightmapped world surfaces. It is R8G8_UNORM at the primary lightmap's
		// resolution and holds the x and y of a unit vector with z implied. Measured on stock
		// mp_paris: both channels centre on exactly 128/255, neither ever clamps, correlation with
		// the radiance pages' chroma is only 0.15-0.22 (so it is not colour), and decoding as v*2-1
		// puts 97.7% of texels inside x^2+y^2 <= 1. The 13.5% of texels that are exactly (128,128)
		// are the ones the radiance pages leave at zero, so (128,128) -> (0,0,1) is the engine's own
		// neutral "no directionality" value.
		//
		// IW5 has no counterpart, so the name GfxWorld emits resolves to nothing and the game gets
		// whatever the linker substitutes - which decodes to x^2+y^2 = 2, an invalid direction pinned
		// to a corner of the tangent frame, applied uniformly to every normal-mapped world surface.
		// Emitting stock's neutral fill instead is flat, but well defined.
		IW7::GfxImage* GenerateLightmapSecondUnorm(const char* name, unsigned short width,
			unsigned short height, allocator& mem)
		{
			const auto image = mem.allocate<IW7::GfxImage>();

			image->name = mem.duplicate_string(name);
			image->imageFormat = DXGI_FORMAT_R8G8_UNORM;
			image->flags = 0x3B; // as shipped on every stock IW7 lightmap texture
			image->mapType = IW7::MAPTYPE_2D;
			image->semantic = IW7::TS_FUNCTION;
			image->category = IW7::IMG_CATEGORY_LIGHTMAP;
			image->width = width;
			image->height = height;
			image->depth = 1;
			image->numElements = 1;
			image->levelCount = 1; // stock lightmap textures ship a single level
			image->streamed = false;

			image->dataLen1 = static_cast<unsigned int>(width) * static_cast<unsigned int>(height) * 2;
			image->dataLen2 = image->dataLen1;

			auto* pixels = mem.allocate<unsigned char>(image->dataLen1);
			std::memset(pixels, 0x80, image->dataLen1);
			image->pixelData = pixels;

			return image;
		}

		// ---- reflection probe array ------------------------------------------------------
		//
		// IW7's GfxWorld binds exactly one reflection image, *reflection_probe_array, and every
		// glossy surface in the map samples it - the viewmodel included, which is why a missing
		// one shows up as artefacts on assets that have nothing to do with the map's materials.
		// IW5 ships one cube per probe instead (*reflection_probe0..N), which nothing in IW7 ever
		// asks for, so the converter has to fold them into a single cube array.
		//
		// Stock shape, identical on mp_paris, mp_afghan, mp_breakneck and cp_zmb: BC6H_UF16,
		// MAPTYPE_CUBE_ARRAY, 128x128, 8 levels, flags 0x28301, category AUTO_GENERATED, semantic
		// TS_FUNCTION. numElements is the probe count - 15, 19, 55 and 160 across the four - not
		// the count times six: the faces are implied by the map type, the same way they are for a
		// plain cube. dataLen is numElements * 6 * 21872 exactly on all four, 21872 being one
		// face's 128x128 8-level BC6H chain.
		//
		// IW5's probes are 64x64, so level 0 here is a 2x upsample of theirs and levels 1-7 are
		// theirs unchanged. That invents no detail, it just lands on the shape IW7 ships, which
		// removes the question of whether the roughness-to-mip mapping assumes eight levels.
		namespace
		{
			// IW5 probe faces are D3DFMT_A8R8G8B8 and get_d3d_to_dxgi tags them sRGB, while BC6H
			// is a linear HDR format - the transfer curve has to come off before encoding or
			// every probe reads back too bright.
			const float* srgb_to_linear_table()
			{
				static float table[256];
				static bool built = false;
				if (!built)
				{
					for (auto i = 0; i < 256; i++)
					{
						const auto c = static_cast<float>(i) / 255.0f;
						table[i] = c <= 0.04045f ? c / 12.92f : std::pow((c + 0.055f) / 1.055f, 2.4f);
					}
					built = true;
				}
				return table;
			}

			// Non-negative and normal-range only, which is all a probe carries: the source is LDR
			// so everything lands in [0, 1], and the darkest sRGB step, 1/255, is 3.0e-4 linear -
			// well above the smallest normal half. Anything under that flushes to zero rather
			// than growing a subnormal path for values that cannot occur.
			std::uint16_t float_to_half(float value)
			{
				if (!(value > 6.104e-5f)) return 0;
				if (value > 65504.0f) value = 65504.0f;

				std::uint32_t bits = 0;
				std::memcpy(&bits, &value, sizeof(bits));

				const auto exponent = ((bits >> 23) & 0xFF) - 127 + 15;
				const auto mantissa = (bits >> 13) & 0x3FF;
				return static_cast<std::uint16_t>((exponent << 10) | mantissa);
			}

			constexpr int bc6h_weights[16] = { 0, 4, 9, 13, 17, 21, 26, 30, 34, 38, 43, 47, 51, 55, 60, 64 };

			// The decoder's last unsigned step is half = (interpolated * 31) >> 6, so the encoder
			// works in that interpolation space and converts back only to measure error.
			int half_to_interpolated(std::uint16_t half_bits)
			{
				return (static_cast<int>(half_bits) << 6) / 31;
			}

			int interpolated_to_half(int value)
			{
				return (std::clamp(value, 0, 0xFFFF) * 31) >> 6;
			}

			// Endpoints are 10-bit codes unquantized into that same space. For unsigned 10-bit
			// the spec's ((code << 15) + 0x4000) >> 9 is just code * 64 + 32, with 0 and 1023
			// pinned to the ends of the range.
			int unquantize10(int code)
			{
				if (code <= 0) return 0;
				if (code >= 1023) return 0xFFFF;
				return (code << 6) + 32;
			}

			int quantize10(int value)
			{
				if (value <= 0) return 0;
				if (value >= 0xFFFF) return 1023;
				return std::clamp(value >> 6, 0, 1023);
			}

			struct bc6h_bit_writer
			{
				std::uint8_t* out;
				int pos = 0;

				void put(unsigned int value, int count)
				{
					for (auto i = 0; i < count; i++, pos++)
					{
						if ((value >> i) & 1)
						{
							out[pos >> 3] |= static_cast<std::uint8_t>(1 << (pos & 7));
						}
					}
				}
			};

			// One BC6H mode-10 block: a single region, three 10-bit endpoint pairs stored outright
			// with no delta, and one 4-bit index per texel shared by all three channels. Mode 10 is
			// the only mode whose fields sit in order - five mode bits, then rw, gw, bw, rx, gx, bx
			// at ten bits each, then 63 index bits - which makes it the cheap one to write, and
			// having no partitions costs little on data as smooth as a reflection probe.
			void bc6h_encode_block(const std::uint16_t texels[16][3], std::uint8_t* out)
			{
				int endpoint_a[3]{};
				int endpoint_b[3]{};
				for (auto c = 0; c < 3; c++)
				{
					auto lo = 0xFFFF;
					auto hi = 0;
					for (auto t = 0; t < 16; t++)
					{
						const auto v = half_to_interpolated(texels[t][c]);
						lo = std::min(lo, v);
						hi = std::max(hi, v);
					}
					endpoint_a[c] = quantize10(lo);
					endpoint_b[c] = quantize10(hi);
				}

				int unq_a[3]{};
				int unq_b[3]{};
				for (auto c = 0; c < 3; c++)
				{
					unq_a[c] = unquantize10(endpoint_a[c]);
					unq_b[c] = unquantize10(endpoint_b[c]);
				}

				int indices[16]{};
				for (auto t = 0; t < 16; t++)
				{
					auto best_error = std::numeric_limits<long long>::max();
					for (auto i = 0; i < 16; i++)
					{
						const auto w = bc6h_weights[i];
						long long error = 0;
						for (auto c = 0; c < 3; c++)
						{
							const auto interp = (unq_a[c] * (64 - w) + unq_b[c] * w + 32) >> 6;
							const long long d = interpolated_to_half(interp) - static_cast<int>(texels[t][c]);
							error += d * d;
						}
						if (error < best_error)
						{
							best_error = error;
							indices[t] = i;
						}
					}
				}

				// A one-region block stores the first index in three bits, so its high bit has to
				// be clear. When it is not, swapping the endpoints and mirroring every index
				// describes the same line from the other end - the weight table is symmetric
				// about 32, so this is exact rather than a re-fit.
				if (indices[0] >= 8)
				{
					for (auto c = 0; c < 3; c++)
					{
						std::swap(endpoint_a[c], endpoint_b[c]);
					}
					for (auto t = 0; t < 16; t++)
					{
						indices[t] = 15 - indices[t];
					}
				}

				std::memset(out, 0, 16);
				bc6h_bit_writer bits{ out };
				bits.put(3, 5); // mode 10
				bits.put(endpoint_a[0], 10);
				bits.put(endpoint_a[1], 10);
				bits.put(endpoint_a[2], 10);
				bits.put(endpoint_b[0], 10);
				bits.put(endpoint_b[1], 10);
				bits.put(endpoint_b[2], 10);
				bits.put(indices[0], 3);
				for (auto t = 1; t < 16; t++)
				{
					bits.put(indices[t], 4);
				}
			}

			unsigned int bc6h_surface_bytes(unsigned int width)
			{
				const auto blocks = (width + 3) / 4;
				return blocks * blocks * 16;
			}

			unsigned int bc6h_chain_bytes(unsigned int width, unsigned int levels)
			{
				unsigned int total = 0;
				for (auto l = 0u; l < levels; l++)
				{
					total += bc6h_surface_bytes(std::max(1u, width >> l));
				}
				return total;
			}

			unsigned int rgba_chain_bytes(unsigned int width, unsigned int levels)
			{
				unsigned int total = 0;
				for (auto l = 0u; l < levels; l++)
				{
					const auto w = std::max(1u, width >> l);
					total += w * w * 4;
				}
				return total;
			}

			void bc6h_encode_surface(const std::uint8_t* rgba, unsigned int width, std::uint8_t* out)
			{
				const auto* srgb = srgb_to_linear_table();
				const auto blocks = (width + 3) / 4;

				for (auto by = 0u; by < blocks; by++)
				{
					for (auto bx = 0u; bx < blocks; bx++)
					{
						std::uint16_t texels[16][3]{};
						for (auto t = 0; t < 16; t++)
						{
							const auto x = std::min(bx * 4 + (t % 4), width - 1);
							const auto y = std::min(by * 4 + (t / 4), width - 1);
							const auto* p = rgba + (static_cast<std::size_t>(y) * width + x) * 4;
							for (auto c = 0; c < 3; c++)
							{
								texels[t][c] = float_to_half(srgb[p[c]]);
							}
						}

						bc6h_encode_block(texels, out);
						out += 16;
					}
				}
			}

			// 2x bilinear with the edges clamped. A cube face's neighbours live on other faces, so
			// clamping is all that is available here, and the seam it leaves is one texel wide on
			// a level that is itself an upsample.
			void upsample_2x(const std::uint8_t* src, unsigned int width, std::uint8_t* dst)
			{
				const auto dst_width = width * 2;
				const auto last = static_cast<int>(width) - 1;

				for (auto y = 0u; y < dst_width; y++)
				{
					const auto sy = (static_cast<float>(y) + 0.5f) * 0.5f - 0.5f;
					const auto fy = sy - std::floor(sy);
					const auto y0 = std::clamp(static_cast<int>(std::floor(sy)), 0, last);
					const auto y1 = std::clamp(y0 + 1, 0, last);

					for (auto x = 0u; x < dst_width; x++)
					{
						const auto sx = (static_cast<float>(x) + 0.5f) * 0.5f - 0.5f;
						const auto fx = sx - std::floor(sx);
						const auto x0 = std::clamp(static_cast<int>(std::floor(sx)), 0, last);
						const auto x1 = std::clamp(x0 + 1, 0, last);

						for (auto c = 0; c < 4; c++)
						{
							const auto v =
								src[(static_cast<std::size_t>(y0) * width + x0) * 4 + c] * (1.0f - fx) * (1.0f - fy) +
								src[(static_cast<std::size_t>(y0) * width + x1) * 4 + c] * fx * (1.0f - fy) +
								src[(static_cast<std::size_t>(y1) * width + x0) * 4 + c] * (1.0f - fx) * fy +
								src[(static_cast<std::size_t>(y1) * width + x1) * 4 + c] * fx * fy;
							dst[(static_cast<std::size_t>(y) * dst_width + x) * 4 + c] =
								static_cast<std::uint8_t>(std::clamp(v + 0.5f, 0.0f, 255.0f));
						}
					}
				}
			}
		}

		// GfxWorldDraw::iesLookupTexture indexes the map's IES light profiles. IW5 has no IES
		// lights at all, so there is nothing to look up and the table is the degenerate one - and
		// that is not a guess: five stock maps and the IW7 linker's own build of mp_test_h1 all
		// ship an identical 1x1 whose payload is { 255, 0, 0, 0 }.
		//
		// The shape is the same everywhere, only the width tracks the profile count (cp_zmb, the
		// one map with real profiles, is 256x1 with a populated table): R8_UNORM, MAPTYPE_1D,
		// semantic TS_FUNCTION, category AUTO_GENERATED, flags 0x18003, one level. dataLen is the
		// width rounded up to four, which is why a 1x1 carries four bytes.
		IW7::GfxImage* GenerateIesLookup(allocator& mem)
		{
			constexpr unsigned int width = 1;
			constexpr unsigned int data_len = (width + 3) & ~3u;

			auto* image = mem.allocate<IW7::GfxImage>();
			image->name = mem.duplicate_string("*ieslookup");
			image->imageFormat = DXGI_FORMAT_R8_UNORM;
			image->flags = 0x18003;
			image->mapType = IW7::MAPTYPE_1D;
			image->semantic = IW7::TS_FUNCTION;
			image->category = IW7::IMG_CATEGORY_AUTO_GENERATED;
			image->width = width;
			image->height = 1;
			image->depth = 1;
			image->numElements = 1;
			image->levelCount = 1;
			image->streamed = false;
			image->dataLen1 = data_len;
			image->dataLen2 = data_len;

			auto* pixels = mem.allocate<unsigned char>(data_len);
			std::memset(pixels, 0, data_len);
			pixels[0] = 255;
			image->pixelData = pixels;

			return image;
		}

		IW7::GfxImage* GenerateReflectionProbeArray(const std::vector<IW7::GfxImage*>& source,
			allocator& mem)
		{
			const auto probe_count = static_cast<unsigned int>(source.size());
			if (!probe_count)
			{
				return nullptr;
			}

			// Every element of a cube array shares one shape, so the first usable probe fixes it
			// and anything disagreeing takes a black element rather than its slot being dropped.
			std::vector<IW7::GfxImage*> probes(probe_count, nullptr);

			unsigned int src_width = 0;
			unsigned int src_levels = 0;

			for (auto i = 0u; i < probe_count; i++)
			{
				auto* probe = source[i];
				if (!probe || !probe->pixelData || probe->imageFormat != DXGI_FORMAT_R8G8B8A8_UNORM_SRGB ||
					probe->width != probe->height || !probe->width || !probe->levelCount)
				{
					ZONETOOL_WARNING("Reflection probe %u is unusable, substituting a black one", i);
					continue;
				}

				if (!src_width)
				{
					src_width = probe->width;
					src_levels = probe->levelCount;
				}
				else if (probe->width != src_width || probe->levelCount != src_levels)
				{
					ZONETOOL_WARNING("Reflection probe %u is %ux%u/%u, expected %ux%u/%u - "
						"substituting a black one", i, probe->width, probe->height,
						probe->levelCount, src_width, src_width, src_levels);
					continue;
				}

				probes[i] = probe;
			}

			if (!src_width)
			{
				ZONETOOL_WARNING("No usable reflection probe, *reflection_probe_array not built");
				return nullptr;
			}

			const auto dst_width = src_width * 2;
			const auto dst_levels = src_levels + 1;
			const auto src_face_bytes = rgba_chain_bytes(src_width, src_levels);
			const auto dst_face_bytes = bc6h_chain_bytes(dst_width, dst_levels);
			const auto total = dst_face_bytes * 6 * probe_count;

			auto* pixels = mem.allocate<unsigned char>(total);
			auto* out = pixels;

			std::vector<std::uint8_t> upsampled(static_cast<std::size_t>(dst_width) * dst_width * 4);

			// One black face's mip chain, opaque so nothing reads alpha as coverage. Stands in
			// for any probe that would not convert.
			std::vector<std::uint8_t> black(src_face_bytes, 0);
			for (std::size_t i = 3; i < black.size(); i += 4)
			{
				black[i] = 255;
			}

			for (auto p = 0u; p < probe_count; p++)
			{
				// Face-major, which is D3D subresource order: face 0's whole mip chain, then
				// face 1's. Both the IW5 blob and IW7 use it, so the faces carry across in place.
				const auto* base = probes[p] ? probes[p]->pixelData : black.data();
				for (auto face = 0; face < 6; face++)
				{
					// The substitute is one black face reused six times, so it has no per-face
					// offset to advance by.
					const auto* src_face = probes[p]
						? base + static_cast<std::size_t>(face) * src_face_bytes
						: base;

					upsample_2x(src_face, src_width, upsampled.data());
					bc6h_encode_surface(upsampled.data(), dst_width, out);
					out += bc6h_surface_bytes(dst_width);

					const auto* level = src_face;
					for (auto l = 0u; l < src_levels; l++)
					{
						const auto w = std::max(1u, src_width >> l);
						bc6h_encode_surface(level, w, out);
						out += bc6h_surface_bytes(w);
						level += static_cast<std::size_t>(w) * w * 4;
					}
				}
			}

			auto* image = mem.allocate<IW7::GfxImage>();
			image->name = mem.duplicate_string("*reflection_probe_array");
			image->imageFormat = DXGI_FORMAT_BC6H_UF16;
			image->flags = 0x28301; // as shipped on every stock IW7 probe array
			image->mapType = IW7::MAPTYPE_CUBE_ARRAY;
			image->semantic = IW7::TS_FUNCTION;
			image->category = IW7::IMG_CATEGORY_AUTO_GENERATED;
			image->width = static_cast<unsigned short>(dst_width);
			image->height = static_cast<unsigned short>(dst_width);
			image->depth = 1;
			image->numElements = static_cast<unsigned short>(probe_count);
			image->levelCount = static_cast<unsigned char>(dst_levels);
			image->streamed = false;
			image->dataLen1 = total;
			image->dataLen2 = total;
			image->pixelData = pixels;

			ZONETOOL_INFO("*reflection_probe_array: %u probes, %ux%u, %u levels, %u bytes",
				probe_count, dst_width, dst_width, dst_levels, total);

			return image;
		}

		IW7::GfxImage* convert(GfxImage* asset, allocator& allocator)
		{
			// generate IW7 gfximage
			return GenerateIW7GfxImage(asset, allocator);
		}
	}
}