#include "stdafx.hpp"
#include "IW4/Assets/GfxImage.hpp"

namespace ZoneTool
{
	namespace IW3
	{
		IW4::GfxImage* IGfxImage::GenerateIW4Image(GfxImage* image, allocator& mem)
		{
			if (!image) return nullptr;

			auto* iw4_image = mem.allocate<IW4::GfxImage>();

			// copy image data
			iw4_image->mapType = image->mapType;
			iw4_image->semantic = (IW4::GfxImageCategory)image->semantic;
			iw4_image->category = image->category;
			iw4_image->cardMemory.platform[0] = image->texture.loadDef->resourceSize;
			iw4_image->cardMemory.platform[1] = image->texture.loadDef->resourceSize;
			iw4_image->width = image->texture.loadDef->dimensions[0];
			iw4_image->height = image->texture.loadDef->dimensions[1];
			iw4_image->depth = image->texture.loadDef->dimensions[2];
			iw4_image->delayLoadPixels = true;
			iw4_image->name = (char*)image->name;

			if (image->texture.loadDef->resourceSize == -1) return iw4_image;

			// alloc texture
			iw4_image->texture = mem.manual_allocate<IW4::GfxImageLoadDef>(16 + image->texture.loadDef->resourceSize);
			iw4_image->texture->levelCount = image->texture.loadDef->levelCount;
			iw4_image->texture->flags = image->texture.loadDef->flags;
			iw4_image->texture->format = image->texture.loadDef->format;
			iw4_image->texture->resourceSize = image->texture.loadDef->resourceSize;
			std::memcpy(&iw4_image->texture->data, image->texture.loadDef->data, image->texture.loadDef->resourceSize);

			return iw4_image;
		}

		void IGfxImage::dump(GfxImage* asset)
		{
			// alloc IW4 image
			allocator allocator;
			auto* iw4_image = IGfxImage::GenerateIW4Image(asset, allocator);

			// dump image
			IW4::IGfxImage::dump(iw4_image);
		}
	}
}