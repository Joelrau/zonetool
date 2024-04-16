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
		extern std::unordered_map<std::string, std::string> mapped_techsets;
		extern std::string replace_material_prefix(const std::string& name);
		extern std::uint8_t convert_semantic(std::uint8_t from);
		extern std::uint8_t get_material_type_from_name(const std::string& name);
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