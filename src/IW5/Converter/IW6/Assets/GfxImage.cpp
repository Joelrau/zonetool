#include "stdafx.hpp"
#include "../Include.hpp"

#include "GfxImage.hpp"

#include <DirectXMath.h>
#include <DirectXPackedVector.h>

namespace ZoneTool::IW5
{
	namespace IW6Converter
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

		void ConvertD3DFMT_A8R8G8B8ToDXGI_R8G8B8A8_UNORM(
			const uint32_t* src,     // Source buffer (D3DFMT_A8R8G8B8)
			uint8_t* dest,           // Destination buffer (DXGI_FORMAT_R8G8B8A8_UNORM)
			size_t pixelCount        // Number of pixels to convert
		)
		{
			for (size_t i = 0; i < pixelCount; ++i)
			{
				uint32_t pixel = src[i];

				// Extract the ARGB components
				uint8_t a = (pixel >> 24) & 0xFF;
				uint8_t r = (pixel >> 16) & 0xFF;
				uint8_t g = (pixel >> 8) & 0xFF;
				uint8_t b = pixel & 0xFF;

				// Convert from sRGB to linear space
				auto srgbToLinear = [](uint8_t value) -> uint8_t {
					float normalized = value / 255.0f;
					float linear = (normalized <= 0.04045f)
						? normalized / 12.92f
						: std::pow((normalized + 0.055f) / 1.055f, 2.4f);
					return static_cast<uint8_t>(std::round(linear * 255));
				};

				dest[i * 4 + 0] = srgbToLinear(r);
				dest[i * 4 + 1] = srgbToLinear(g);
				dest[i * 4 + 2] = srgbToLinear(b);
				dest[i * 4 + 3] = a; // Preserve the alpha channel
			}
		}

		// Convert D3DFMT_A8R8G8B8 to DXGI_FORMAT_R8G8B8A8_UNORM
		void ConvertD3DFMT_A8R8G8B8ToDXGI_R8G8B8A8_UNORM_SRGB(
			const uint32_t* src,     // Source buffer (D3DFMT_A8R8G8B8)
			uint8_t* dest,           // Destination buffer (DXGI_FORMAT_R8G8B8A8_UNORM)
			size_t pixelCount        // Number of pixels to convert
		)
		{
			for (size_t i = 0; i < pixelCount; ++i)
			{
				uint32_t pixel = src[i];

				// Extract the ARGB components
				uint8_t a = (pixel >> 24) & 0xFF;
				uint8_t r = (pixel >> 16) & 0xFF;
				uint8_t g = (pixel >> 8) & 0xFF;
				uint8_t b = pixel & 0xFF;

				dest[i * 4 + 0] = r;
				dest[i * 4 + 1] = g;
				dest[i * 4 + 2] = b;
				dest[i * 4 + 3] = a;
			}
		}

		// Convert linear float to 16-bit half-float
		inline uint16_t ConvertLinearFloatToHalf(float value) {
			return DirectX::PackedVector::XMConvertFloatToHalf(value);
		}

		// Convert R8G8B8A8_UNORM (in linear space) to R16G16B16A16_FLOAT
		void ConvertR8G8B8A8ToR16G16B16A16Float(
			const uint8_t* src,          // Source buffer (R8G8B8A8_UNORM in linear space)
			uint16_t* dest,              // Destination buffer (R16G16B16A16_FLOAT)
			size_t pixelCount            // Number of pixels to convert
		) {
			for (size_t i = 0; i < pixelCount; ++i) {
				float r = src[i * 4 + 0] / 255.0f;
				float g = src[i * 4 + 1] / 255.0f;
				float b = src[i * 4 + 2] / 255.0f;
				float a = src[i * 4 + 3] / 255.0f;

				// Convert to 16-bit half-float
				dest[i * 4 + 0] = ConvertLinearFloatToHalf(r);
				dest[i * 4 + 1] = ConvertLinearFloatToHalf(g);
				dest[i * 4 + 2] = ConvertLinearFloatToHalf(b);
				dest[i * 4 + 3] = ConvertLinearFloatToHalf(a);
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

		IW6::GfxImage* GenerateIW6GfxImage(GfxImage* asset, allocator& mem)
		{
			// allocate IW6 GfxImage structure
			const auto iw6_asset = mem.allocate<IW6::GfxImage>();

			iw6_asset->name = asset->name;
			iw6_asset->imageFormat = get_d3d_to_dxgi(asset->texture.loadDef->format);
			iw6_asset->mapType = static_cast<IW6::MapType>(asset->mapType);
			iw6_asset->semantic = asset->semantic;
			iw6_asset->category = asset->category;
			iw6_asset->flags = asset->flags;
			iw6_asset->dataLen1 = asset->texture.loadDef->resourceSize;
			iw6_asset->dataLen2 = asset->texture.loadDef->resourceSize;
			iw6_asset->width = asset->width;
			iw6_asset->height = asset->height;
			iw6_asset->depth = asset->depth;
			iw6_asset->numElements = 1;
			iw6_asset->levelCount = Image_CountMipmaps(asset->texture.loadDef->flags, asset->width, asset->height, asset->depth);
			iw6_asset->streamed = false;
			iw6_asset->pixelData = reinterpret_cast<unsigned char*>(&asset->texture.loadDef->data);

			if (iw6_asset->imageFormat == DXGI_FORMAT_UNKNOWN)
			{
				ZONETOOL_INFO("Possible DXGIFORMAT: %d", MFMapDX9FormatToDXGIFormat(asset->texture.loadDef->format));
				ZONETOOL_FATAL("Unknown DXGIFORMAT for image \"%s\" (%d)", asset->name, asset->texture.loadDef->format);
			}

			std::string sname(iw6_asset->name);
			if (sname.starts_with("*lightmap") && sname.ends_with("_secondary") && asset->texture.loadDef->format == D3DFMT_A8R8G8B8)
			{
				iw6_asset->imageFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			}

			if (asset->texture.loadDef->format == D3DFMT_A8R8G8B8 && iw6_asset->imageFormat == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB)
			{
				auto* new_pixels = mem.allocate<unsigned char>(iw6_asset->dataLen1);

				// Calculate the number of pixels
				size_t pixelCount = iw6_asset->dataLen1 / 4;

				ConvertD3DFMT_A8R8G8B8ToDXGI_R8G8B8A8_UNORM_SRGB(reinterpret_cast<uint32_t*>(iw6_asset->pixelData), new_pixels, pixelCount);
				iw6_asset->pixelData = new_pixels;
			}
			else if (asset->texture.loadDef->format == D3DFMT_A8R8G8B8 && iw6_asset->imageFormat == DXGI_FORMAT_R8G8B8A8_UNORM)
			{
				auto* new_pixels = mem.allocate<unsigned char>(iw6_asset->dataLen1);

				// Calculate the number of pixels
				size_t pixelCount = iw6_asset->dataLen1 / 4;

				ConvertD3DFMT_A8R8G8B8ToDXGI_R8G8B8A8_UNORM(reinterpret_cast<uint32_t*>(iw6_asset->pixelData), new_pixels, pixelCount);
				iw6_asset->pixelData = new_pixels;
			}
			else if (asset->texture.loadDef->format == D3DFMT_L8 && iw6_asset->imageFormat == DXGI_FORMAT_R8_UNORM)
			{

			}
			else
			{
				ZONETOOL_FATAL("No known conversion for image \"%s\"", asset->name);
			}

			return iw6_asset;
		}

		IW6::GfxImage* convert(GfxImage* asset, allocator& allocator)
		{
			// generate IW6 gfximage
			return GenerateIW6GfxImage(asset, allocator);
		}
	}
}