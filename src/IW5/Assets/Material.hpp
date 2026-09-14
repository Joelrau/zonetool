#pragma once

namespace ZoneTool
{
	namespace H1
	{
		extern std::unordered_map<std::string, std::string> mapped_techsets;
	}

	namespace IW6
	{
		extern std::unordered_map<std::string, std::string> mapped_techsets;
	}

	namespace IW7
	{
		extern std::unordered_map<std::string, std::string> prefix_cache;

		extern std::string get_mapped_techset(const std::string& techset, const bool effect_vertlit = false, const bool color_tint = false);

		// The IW7 name of a material is a function of the *material*, not of its name alone: the
		// prefix follows the mapped IW7 techset, so mc/mtl_metal_pail on mc_l_sm_r0c0d0n0s0 is
		// written as mo/mtl_metal_pail. Every reference to it - the zone csv, XModel material
		// handles, fx and particle visuals - has to name the file the dumper actually wrote.
		//
		// Only the material dumper may call this: it computes the name and records it. The techset
		// argument has no default on purpose - passing none used to map to "2d", match no prefix
		// and hand back the original name, which is how references ended up pointing at files that
		// were never written.
		extern std::string replace_material_prefix(const std::string& name, const std::string& techset, const bool effect_vertlit = false, const bool color_tint = false);

		// What every *reference* to a material should use. A reference site has nothing but a name
		// - XModel material handles reach the IW5 converters as a reinterpret_cast of the source
		// game's own array, so only offset 0 is meaningful and reading techniqueSet off one is a
		// wild pointer - so this is a lookup of what the dumper decided, never a computation.
		// Fastfiles are written dependency-first, so a material is dumped before anything that
		// references it; if that ever fails to hold, this warns rather than inventing a name.
		extern std::string resolve_material_name(const std::string& name);
		extern std::uint8_t convert_semantic(std::uint8_t from);
		extern std::uint8_t get_material_type_from_techset(std::string techset = "");
	}

	namespace S1
	{
		extern std::unordered_map<std::string, std::string> mapped_techsets;
	}

	namespace IW5
	{
		class IMaterial
		{
		public:
			static void dump(Material* asset);
		};
	}
}