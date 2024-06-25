#include "stdafx.hpp"
#include "Material.hpp"

//#include "IW7/Assets/Material.hpp"

//r0 - replace
//c0 - color map
//no - normal map
//s0 - specular map
//p0 - parallax
//a0 - Add
//b0 - Blend
//d0 - Detail
//t0 - transparent ? (means that its replace + alpha test >= 128 meaning either full / no transparency per pixel)
//q0 - (don't know the word, it's a detail map for normals)

namespace ZoneTool
{
	namespace IW7
	{
		std::unordered_map<std::string, std::string> mapped_techsets =
		{
			{"mc_l_sm_r0c0",							"mo_l_sm_replace_i0c0"},
			{"mc_l_sm_r0c0s0",							"mo_l_sm_replace_i0c0s0"},
			{"mc_l_sm_r0c0n0s0",						"mo_l_sm_replace_i0c0s0n0"},

			{"2d",										"2d"},

			{"effect",									"eq_effect_blend_lin_ndw_nocast"},
			{"effect_nofog",							"eq_effect_blend_lin_nofog_ndw_nocast"},
			{"effect_zfeather_add",						"eq_effect_zfeather_add_lin_ct_ndw_nocast"},
			{"effect_zfeather_add_nofog_eyeoffset",		"eq_effect_zfeather_add_lin_nofog_eyeoffset_ndw_nocast"},
			{"effect_zfeather_add_nofog",				"eq_effect_zfeather_add_lin_nofog_ndw_nocast"},

			{"distortion_scale",						"eq_distortion_scale"},
			{"distortion_scale_zfeather",				"eq_distortion_scale_zfeather"},
		};

		std::unordered_map<std::string, std::string> mapped_techsets_effect_vertlit =
		{
			{"effect",									"ev_effect_blend_lin_ct_ndw_nocast"},
		};

		enum MaterialType : std::uint8_t
		{
			MTL_TYPE_DEFAULT = 0x0, // ""
			MTL_TYPE_MODEL = 1, // "m"
			MTL_TYPE_MODEL_SELFVIS = 23, // "mo"
			MTL_TYPE_MODEL_VERTCOL_SELFVIS = 24, // "mco"
			MTL_TYPE_WORLD = 68, // "w"
			MTL_TYPE_WORLD_VERTCOL = 69, // "wc"
			MTL_TYPE_EFFECT_LMAP = 71, // "el"
			MTL_TYPE_EFFECT_VERTLIT = 72, // "ev"
			MTL_TYPE_EFFECT_QUAD = 73, // "eq"
		};

		std::string prefixes[] =
		{
			"mo",
			"ev",
			"eq",
		};

		std::uint8_t prefixes_types[] =
		{
			MTL_TYPE_MODEL_SELFVIS,
			MTL_TYPE_EFFECT_VERTLIT,
			MTL_TYPE_EFFECT_QUAD,
		};

		std::string get_mapped_techset(const std::string& techset, bool effect_vertlit)
		{
			auto& map = effect_vertlit ? mapped_techsets_effect_vertlit : mapped_techsets;
			if (map.find(techset) == map.end())
			{
				return "2d";
			}
			return map[techset];
		}

		std::unordered_map<std::string, std::string> prefix_cache;
		std::string replace_material_prefix(const std::string& name, std::string techset, bool effect_vertlit)
		{
			auto& map = effect_vertlit ? mapped_techsets_effect_vertlit : mapped_techsets;

			if (!techset.empty() && map.find(techset) != map.end())
			{
				std::string new_tech = map[techset];
				for (auto& prefix : prefixes)
				{
					if (new_tech.starts_with(prefix + "_"))
					{
						auto offset_end = name.find("/");
						if (offset_end == std::string::npos)
						{
							offset_end = 0;
						}

						const auto replacement = prefix + "/";

						std::string replaced = name;
						replaced.replace(0, offset_end, replacement);
						prefix_cache[name] = replaced;
						return replaced;
					}
				}
			}

			if (prefix_cache.contains(name))
			{
				return prefix_cache[name];
			}

			return name;
		}

		std::uint8_t get_material_type_from_techset(std::string techset) // iw7_techset
		{
			if (!techset.empty())
			{
				for (auto prefix_idx = 0; prefix_idx < std::size(prefixes); prefix_idx++)
				{
					const auto prefix = prefixes[prefix_idx];
					const auto type = prefixes_types[prefix_idx];

					if (techset.starts_with(prefix + "_"))
					{
						return type;
					}
				}
			}

			return IW7::MTL_TYPE_DEFAULT;
		}

		IW7::TextureSemantic surf_flags_conversion_table[13]
		{
			IW7::TextureSemantic::TS_2D,
			IW7::TextureSemantic::TS_FUNCTION,
			IW7::TextureSemantic::TS_COLOR_MAP,
			IW7::TextureSemantic::TS_DETAIL_MAP,
			IW7::TextureSemantic::TS_UNUSED_4,
			IW7::TextureSemantic::TS_NORMAL_MAP,
			IW7::TextureSemantic::TS_UNUSED_6,
			IW7::TextureSemantic::TS_UNUSED_7,
			IW7::TextureSemantic::TS_SPECULAR_MAP,
			IW7::TextureSemantic::TS_UNUSED_4,
			IW7::TextureSemantic::TS_UNUSED_4,
			IW7::TextureSemantic::TS_UNUSED_4, // WATER_MAP
			IW7::TextureSemantic::TS_DISPLACEMENT_MAP,
		}; IW5::TextureSemantic;

		std::uint8_t convert_semantic(std::uint8_t from)
		{
			return surf_flags_conversion_table[from];
		}
	}

	namespace IW5::IW7Dumper
	{
	}
}