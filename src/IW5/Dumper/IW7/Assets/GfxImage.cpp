#include "stdafx.hpp"

#include "GfxImage.hpp"
#include "Converter/IW7/Assets/GfxImage.hpp"
#include "Converter/IW7/Assets/IwiImage.hpp"
#include "IW7/Assets/GfxImage.hpp"

namespace ZoneTool::IW5::IW7Dumper
{
	namespace
	{
		// Outlives the per-asset allocators the dumper uses, because the array is not assembled
		// until the GfxWorld comes through - nine seconds later on mp_test_h1. A whole map's
		// probes are under a megabyte, so they are simply copied in and kept.
		allocator& probe_allocator()
		{
			static allocator mem;
			return mem;
		}

		std::vector<IW7::GfxImage*> probe_images;
		std::vector<IW7::GfxImage*> lightmap_primaries;
		std::vector<IW7::GfxImage*> lightmap_secondaries;

		// The converted image, copied somewhere that outlives the caller's allocator.
		IW7::GfxImage* keep(const std::string& name, IW7::GfxImage* image)
		{
			auto& mem = probe_allocator();
			auto* copy = mem.allocate<IW7::GfxImage>();
			*copy = *image;
			copy->name = mem.duplicate_string(name);

			auto* pixels = mem.allocate<unsigned char>(image->dataLen1);
			std::memcpy(pixels, image->pixelData, image->dataLen1);
			copy->pixelData = pixels;
			return copy;
		}

		// "<prefix><digits><suffix>" -> the digits, or -1.
		int indexed_name(const std::string& name, const std::string& prefix, const std::string& suffix)
		{
			if (name.size() <= prefix.size() + suffix.size() ||
				name.compare(0, prefix.size(), prefix) != 0 ||
				name.compare(name.size() - suffix.size(), suffix.size(), suffix) != 0)
			{
				return -1;
			}

			const auto digits = name.substr(prefix.size(), name.size() - prefix.size() - suffix.size());
			if (digits.empty() || digits.find_first_not_of("0123456789") != std::string::npos)
			{
				return -1;
			}

			const auto index = std::strtoul(digits.data(), nullptr, 10);
			return index < 1024 ? static_cast<int>(index) : -1;
		}

		void store_at(std::vector<IW7::GfxImage*>& slots, int index, IW7::GfxImage* image)
		{
			if (slots.size() <= static_cast<std::size_t>(index))
			{
				slots.resize(index + 1, nullptr);
			}
			slots[index] = image;
		}

		// IW7 keeps a lightmap's three textures on one texel grid - measured on cp_zmb, mp_afghan,
		// mp_breakneck, mp_frontend and mp_paris, where primary and secondunorm always match and
		// the secondary is that grid with the height doubled for its two radiance pages. IW5 does
		// not: on mp_test_h1 the primary arrives as 1024x1024 single-channel while the secondary
		// is two 512x512 RGBA pages. The GfxWorld dumper reconciles them, which is why both are
		// captured here rather than being written once and forgotten.
		void capture_lightmap(const std::string& name, IW7::GfxImage* image)
		{
			if (!image || !image->pixelData || !image->dataLen1)
			{
				return;
			}

			const auto primary = indexed_name(name, "*lightmap", "_primary");
			if (primary >= 0)
			{
				store_at(lightmap_primaries, primary, keep(name, image));
				return;
			}

			const auto secondary = indexed_name(name, "*lightmap", "_secondary");
			if (secondary >= 0)
			{
				store_at(lightmap_secondaries, secondary, keep(name, image));
			}
		}

		void capture_reflection_probe(const std::string& name, IW7::GfxImage* image)
		{
			const auto prefix = "*reflection_probe"s;
			if (!image || !image->pixelData || !image->dataLen1 || name.size() <= prefix.size() ||
				name.compare(0, prefix.size(), prefix) != 0)
			{
				return;
			}

			const auto digits = name.substr(prefix.size());
			if (digits.find_first_not_of("0123456789") != std::string::npos)
			{
				return;
			}

			const auto index = std::strtoul(digits.data(), nullptr, 10);
			if (index >= 1024) // a map with more probes than that is not a map we produced
			{
				return;
			}

			store_at(probe_images, static_cast<int>(index), keep(name, image));
		}
	}

	const std::vector<IW7::GfxImage*>& reflection_probes()
	{
		return probe_images;
	}

	void clear_reflection_probes()
	{
		probe_images.clear();
	}

	const std::vector<IW7::GfxImage*>& lightmap_primary_images()
	{
		return lightmap_primaries;
	}

	const std::vector<IW7::GfxImage*>& lightmap_secondary_images()
	{
		return lightmap_secondaries;
	}

	void clear_lightmap_images()
	{
		lightmap_primaries.clear();
		lightmap_secondaries.clear();
	}

	// Set ZONETOOL_FAST_DUMP=1 to skip re-converting ordinary textures. Image conversion is most
	// of a dump's cost (BC7 and BC6H encoding, then writing every mip), and when only world or
	// lighting code has changed those files are already sitting in the staging tree from the last
	// run, byte-identical to what this would produce.
	//
	// Map images are never skipped. Names beginning "*light" or "*refle" are the lightmaps and
	// reflection probes the GfxWorld dumper assembles its probe array and lightmap set from - they
	// are captured as a side effect of being dumped here, so skipping them would silently empty
	// those and leave the world asset referring to images nothing rebuilt.
	//
	// Materials deliberately have no equivalent switch: dumping a material is what registers its
	// renamed name, and every xmodel and effect resolves its material references through that
	// table. Skipping them would emit unrenamed references and produce a broken zone.
	bool fast_dump_enabled()
	{
		static const auto enabled = []
		{
			char buffer[8]{};
			size_t length = 0;
			if (getenv_s(&length, buffer, sizeof(buffer), "ZONETOOL_FAST_DUMP") || !length)
			{
				return false;
			}
			return buffer[0] == '1';
		}();
		return enabled;
	}

	void dump(GfxImage* asset)
	{
		std::string name = asset->name;

		bool isMapImage = (name.size() >= 6)
			? ((name.substr(0, 6) == "*light" || name.substr(0, 6) == "*refle" ||
				name == "$outdoor")
				? true
				: false)
			: false;

		if (!isMapImage && fast_dump_enabled())
		{
			return;
		}

		allocator allocator;

		if (!isMapImage)
		{
			// Where the pixels live depends on which game the zone came from, and only IW5 keeps
			// model textures out of the zone:
			//
			//   IW3 / IW4  the data is resident in texture.loadDef. IW3::GenerateIW4Image copies
			//              resourceSize bytes straight out of it, and by the time an IW3 zone
			//              reaches here it has been reinterpret_cast to an IW5 GfxImage, loadDef
			//              and all. There is no images\<name>.iwi for CoD4 to find, so reading one
			//              always comes back empty.
			//   IW5        model textures are streamed .iwi with no resident data at all.
			//
			// Prefer whatever is in the zone and only go to the filesystem when there is nothing.
			auto* converted = IW7Converter::convert_resident(asset, allocator);
			if (!converted)
			{
				converted = IW7Converter::convert_iwi(asset->name, asset->semantic, allocator);
			}

			if (converted)
			{
				IW7::IGfxImage::dump(converted);
			}
			return;
		}

		if (!asset->texture.loadDef->resourceSize)
		{
			ZONETOOL_INFO("Not dumping image %s", asset->name);
			return;
		}

		// generate IW7 gfximage
		auto* iw7_asset = IW7Converter::convert(asset, allocator);

		// dump IW7 gfximage
		IW7::IGfxImage::dump(iw7_asset);

		capture_reflection_probe(name, iw7_asset);
		capture_lightmap(name, iw7_asset);
	}
}
