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

		extern std::string get_mapped_techset(const std::string& techset, bool effect_vertlit = false);

		extern std::string replace_material_prefix(const std::string& name, std::string techset = "", bool effect_vertlit = false);
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