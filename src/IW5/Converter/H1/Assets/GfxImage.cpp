#include "stdafx.hpp"
#include "../Include.hpp"

#include "GfxImage.hpp"

namespace ZoneTool::IW5
{
	namespace H1Converter
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
			{D3DFMT_A8R8G8B8, DXGI_FORMAT_R8G8B8A8_UNORM},
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

		inline std::uint32_t from_argb(std::uint32_t argb)
		{
			return
				// Source is in format: 0xAARRGGBB
				((argb & 0x00FF0000) >> 16) | //______RR
				((argb & 0x0000FF00)) | //____GG__
				((argb & 0x000000FF) << 16)  | //___BB____
				((argb & 0xFF000000));         //AA______
				// Return value is in format:  0xAABBGGRR 
		}

		unsigned int argb_to_rgba(unsigned char* input, unsigned int size, unsigned char* output)
		{
			unsigned int offset = 0;

			for (unsigned int i = 0; i < size / 4; i++)
			{
				auto argb = *(unsigned int*)(&input[offset]);
				auto rgba = from_argb(argb);

				*(unsigned int*)&output[offset] = rgba;

				offset += 4;
			}
			return offset;
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

		H1::GfxImage* GenerateH1GfxImage(GfxImage* asset, allocator& mem)
		{
			// allocate H1 GfxImage structure
			const auto h1_asset = mem.allocate<H1::GfxImage>();

			h1_asset->name = asset->name;
			h1_asset->imageFormat = get_d3d_to_dxgi(asset->texture.loadDef->format); //h1_asset->imageFormat = MFMapDX9FormatToDXGIFormat(asset->texture->format);
			h1_asset->mapType = static_cast<H1::MapType>(asset->mapType);
			h1_asset->semantic = asset->semantic;
			h1_asset->category = asset->category;
			h1_asset->flags = asset->flags;
			h1_asset->dataLen1 = asset->texture.loadDef->resourceSize;
			h1_asset->dataLen2 = asset->texture.loadDef->resourceSize;
			h1_asset->width = asset->width;
			h1_asset->height = asset->height;
			h1_asset->depth = asset->depth;
			h1_asset->numElements = 1;
			h1_asset->levelCount = Image_CountMipmaps(asset->texture.loadDef->flags, asset->width, asset->height, asset->depth);
			h1_asset->streamed = false;
			h1_asset->pixelData = reinterpret_cast<unsigned char*>(&asset->texture.loadDef->data);

			if (h1_asset->imageFormat == DXGI_FORMAT_UNKNOWN)
			{
				ZONETOOL_INFO("Possible DXGIFORMAT: %d", MFMapDX9FormatToDXGIFormat(asset->texture.loadDef->format));
				ZONETOOL_FATAL("Unknown DXGIFORMAT for image \"%s\" (%d)", asset->name, asset->texture.loadDef->format);
			}

			if (asset->texture.loadDef->format == D3DFMT_A8R8G8B8 && h1_asset->imageFormat == DXGI_FORMAT_R8G8B8A8_UNORM)
			{
				auto* new_pixels = mem.allocate<unsigned char>(asset->texture.loadDef->resourceSize);
				argb_to_rgba(h1_asset->pixelData, asset->texture.loadDef->resourceSize, new_pixels);
				h1_asset->pixelData = new_pixels;
			}
			else
			{
				//ZONETOOL_FATAL("No known convertion for image \"%s\"", asset->name);
			}

			return h1_asset;
		}

		H1::GfxImage* convert(GfxImage* asset, allocator& allocator)
		{
			// generate h1 gfximage
			return GenerateH1GfxImage(asset, allocator);
		}
	}
}