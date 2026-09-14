#pragma once

namespace ZoneTool::IW5
{
	namespace IW7Converter
	{
		// The raw levels behind an image, largest first, however it is stored. `storage` owns
		// the bytes when they came off disk; for a resident image the pointers alias the zone.
		struct source_image
		{
			std::uint8_t format = 0;
			unsigned short width = 0;
			unsigned short height = 0;
			std::string storage;
			std::vector<std::pair<const std::uint8_t*, std::size_t>> levels;
		};

		bool load_source(GfxImage* asset, source_image& out);

		// Builds an IW7 GfxImage from the .iwi that backs an IW5/IW3 image. Returns nullptr when
		// the file cannot be read or its format has no mapping; both are logged.
		IW7::GfxImage* convert_iwi(const char* name, std::uint8_t iw5_semantic, allocator& mem);

		// Builds from the pixel data an IW3/IW4 zone carries inline, which is where images live
		// for everything but IW5. Returns nullptr when the image has no resident data.
		IW7::GfxImage* convert_resident(GfxImage* asset, allocator& mem);
	}
}
