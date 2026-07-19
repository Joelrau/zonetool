#include "stdafx.hpp"
#include "Material.hpp"

//#include "IW7/Assets/Material.hpp"

// r0 - replace
// c0 - color map
// n0 - normal map
// s0 - specular map
// p0 - parallax
// a0 - Add
// b0 - Blend
// d0 - Detail
// t0 - transparent ? (means that its replace + alpha test >= 128 meaning either full / no transparency per pixel)
// q0 - (don't know the word, it's a detail map for normals)
// 
// _ct_ = colorTint

namespace ZoneTool
{
	namespace IW7
	{
		enum techset_map_type_e
		{
			regular,
			color_tint,
			count,
		};

		struct techset_map_s
		{
			std::string techset[techset_map_type_e::count];
		};

		std::unordered_map<std::string, techset_map_s> mapped_techsets =
		{
			{"mc_unlit",								{"mo_unlit_replace_lin", "mo_unlit_replace_lin_ct"}},

			{"mc_l_sm_r0c0",							{"mo_l_sm_replace_i0c0"}},
			{"mc_l_sm_r0c0s0",							{"mo_l_sm_replace_i0c0s0"}},
			{"mc_l_sm_r0c0n0s0",						{"mo_l_sm_replace_i0c0s0n0"}},
			{"2d",										{"2d", "2d|_ct"}},

			{"effect",									{"eq_effect_blend_lin_ndw_nocast"}},
			{"effect_nofog",							{"eq_effect_blend_lin_nofog_ndw_nocast"}},
			{"effect_zfeather_add",						{"eq_effect_zfeather_add_lin_ct_ndw_nocast"}},
			{"effect_zfeather_add_nofog_eyeoffset",		{"eq_effect_zfeather_add_lin_nofog_eyeoffset_ndw_nocast"}},
			{"effect_zfeather_add_nofog",				{"eq_effect_zfeather_add_lin_nofog_ndw_nocast"}},

			{"distortion_scale",						{"eq_distortion_scale"}},
			{"distortion_scale_zfeather",				{"eq_distortion_scale_zfeather"}},
		};

		std::unordered_map<std::string, techset_map_s> mapped_techsets_effect_vertlit =
		{
			{"effect",									{"ev_effect_blend_lin_ct_ndw_nocast"}},
		};

		enum MaterialType : std::uint8_t
		{
			MTL_TYPE_DEFAULT = 0x0, // ""
			MTL_TYPE_MODEL = 1, // "m"
			MLT_TYPE_MODEL_UNK2 = 2,
			MTL_TYPE_MODEL_UNK3 = 3,
			MTL_TYPE_MODEL_UNK4 = 4,
			MTL_TYPE_MODEL_UNK5 = 5,
			MTL_TYPE_MODEL_UNK6 = 6,
			MTL_TYPE_MODEL_UNK7 = 7,
			MTL_TYPE_MODEL_UNK8 = 8,
			MTL_TYPE_MODEL_UNK9 = 9,
			MTL_TYPE_MODEL_UNK10 = 10,
			MTL_TYPE_MODEL_UNK11 = 11,
			MTL_TYPE_MODEL_UNK12 = 12,
			MTL_TYPE_MODEL_UNK13 = 13,
			MTL_TYPE_MODEL_UNK14 = 14,
			MTL_TYPE_MODEL_UNK15 = 15,
			MTL_TYPE_MODEL_LMAP = 16, // "ml"
			MTL_TYPE_MODEL_LMAP_VERTCOL = 17, // "mlc"
			MTL_TYPE_MODEL_UNK18 = 18,
			MTL_TYPE_MODEL_UNK19 = 19,
			MTL_TYPE_MODEL_UNK20 = 20,
			MTL_TYPE_MODEL_UNK21 = 21,
			MTL_TYPE_MODEL_IMPACT = 22, // "mim"
			MTL_TYPE_MODEL_SELFVIS = 23, // "mo"
			MTL_TYPE_MODEL_VERTCOL_SELFVIS = 24, // "mco"
			MTL_TYPE_MODEL_UNK25 = 25, // "mvo"
			MTL_TYPE_MODEL_UNK26 = 26, // "mvco"
			MTL_TYPE_MODEL_UNK27 = 27,
			MTL_TYPE_MODEL_SECONDUV_SELFVIS = 28, // "m2o"
			MTL_TYPE_MODEL_SECONDUV_VERTCOL_SELFVIS = 29, // "m2co"
			MTL_TYPE_MODEL_UNK30 = 30, // "mop"
			MTL_TYPE_MODEL_UNK31 = 31, // "m2op"
			MTL_TYPE_MODEL_UNK32 = 32,
			MTL_TYPE_MODEL_UNK33 = 33,
			MTL_TYPE_MODEL_UNK34 = 34,
			MTL_TYPE_MODEL_UNK35 = 35,
			MTL_TYPE_MODEL_UNK36 = 36,
			MTL_TYPE_MODEL_UNK37 = 37, // "mopw"
			MTL_TYPE_MODEL_UNK38 = 38, // "m2opw"
			MTL_TYPE_MODEL_UNK39 = 39, // "m2copw"
			MTL_TYPE_MODEL_UNK40 = 40, // "mcopw"
			MTL_TYPE_MODEL_UNK41 = 41, // "mvopw"
			MTL_TYPE_MODEL_UNK42 = 42,
			MTL_TYPE_MODEL_UNK43 = 43,
			MTL_TYPE_MODEL_SUBDIV = 44, // "ms"
			MTL_TYPE_MODEL_UNK45 = 45,
			MTL_TYPE_MODEL_UNK46 = 46,
			MTL_TYPE_MODEL_UNK47 = 47, // "msa"
			MTL_TYPE_MODEL_UNK48 = 48,
			MTL_TYPE_MODEL_UNK49 = 49,
			MTL_TYPE_MODEL_UNK50 = 50,
			MTL_TYPE_MODEL_UNK51 = 51,
			MTL_TYPE_MODEL_UNK52 = 52,
			MTL_TYPE_MODEL_SUBDIV_VERTCOL_SELFVIS = 53, // "msco"
			MTL_TYPE_MODEL_UNK54 = 54, // "mszo"
			MTL_TYPE_MODEL_UNK55 = 55, // "msvo"
			MTL_TYPE_MODEL_UNK56 = 56, // "msvco"
			MTL_TYPE_MODEL_SUBDIV_SELFVIS = 57, // "mso"
			MTL_TYPE_MODEL_UNK58 = 58, // "msop"
			MTL_TYPE_MODEL_UNK59 = 59,
			MTL_TYPE_MODEL_UNK60 = 60,
			MTL_TYPE_MODEL_UNK61 = 61,
			MTL_TYPE_MODEL_UNK62 = 62,
			MTL_TYPE_MODEL_UNK63 = 63, // "msopw"
			MTL_TYPE_MODEL_UNK64 = 64, // "mscopw"
			MTL_TYPE_MODEL_UNK65 = 65,
			MTL_TYPE_MODEL_UNK66 = 66,
			MTL_TYPE_MODEL_UNK67 = 67,
			MTL_TYPE_WORLD = 68, // "w"
			MTL_TYPE_WORLD_VERTCOL = 69, // "wc"
			MTL_TYPE_WORLD_IMPACT = 70, // "wim"
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

		std::string get_mapped_techset(const std::string& techset, const bool effect_vertlit, const bool color_tint)
		{
			if (!effect_vertlit)
			{
				const auto it = mapped_techsets.find(techset);
				if (it != mapped_techsets.end())
				{
					auto tech = it->second.techset[color_tint ? techset_map_type_e::color_tint : techset_map_type_e::regular];
					return tech.empty() ? it->second.techset[techset_map_type_e::regular] : tech;
				}
			}
			else
			{
				const auto it = mapped_techsets_effect_vertlit.find(techset);
				if (it != mapped_techsets_effect_vertlit.end())
				{
					auto tech = it->second.techset[color_tint ? techset_map_type_e::color_tint : techset_map_type_e::regular];
					return tech.empty() ? it->second.techset[techset_map_type_e::regular] : tech;
				}
			}

			return "2d";
		}

		std::unordered_map<std::string, std::string> prefix_cache;

		std::string replace_material_prefix(const std::string& name, const std::string& techset, const bool effect_vertlit, const bool color_tint)
		{
			if (prefix_cache.contains(name))
			{
				return prefix_cache[name];
			}

			std::string new_tech = get_mapped_techset(techset, effect_vertlit, color_tint);

			for (const auto& prefix : prefixes)
			{
				if (new_tech.starts_with(prefix + "_"))
				{
					const auto slash_pos = name.find('/');
					const size_t replace_len = (slash_pos == std::string::npos) ? 0 : slash_pos + 1;
					const std::string replacement = prefix + "/";

					std::string replaced = name;
					replaced.replace(0, replace_len, replacement);

					prefix_cache[name] = replaced;
					return replaced;
				}
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

		namespace
		{
			std::string get_IW7_techset(std::string name, std::string matname, bool* result, bool effect_vertlit = false, bool has_ct = false)
			{
				auto iw7_techset = get_mapped_techset(name, effect_vertlit, has_ct);

				*result = true;
				if (name != "2d" && iw7_techset == "2d")
				{
					ZONETOOL_ERROR("Could not find mapped IW7 techset for techset \"%s\" (material: %s)%s",
						name.data(),
						matname.data(),
						effect_vertlit ? " (EFFECT_VERTLIT)" : "");
					*result = false;
				}

				return iw7_techset;
			}

			std::unordered_map<std::uint8_t, std::uint8_t> mapped_sortkeys =
			{
				{1, 2},		// Opaque
				{2, 3},     // Sky
				{43, 36},	// Distortion
				{48, 35},	// Effect auto sort!
				{52, 41},   // 2D
			};

			std::unordered_map<std::string, std::uint8_t> mapped_sortkeys_by_techset =
			{
				{"mc_shadowcaster_atest", 2},
				{"wc_shadowcaster", 2},
			};

			std::uint8_t get_IW7_sortkey(std::uint8_t sortkey, std::string matname, std::string IW7_techset)
			{
				if (mapped_sortkeys_by_techset.find(IW7_techset) != mapped_sortkeys_by_techset.end())
				{
					return mapped_sortkeys_by_techset[IW7_techset];
				}

				if (mapped_sortkeys.contains(sortkey))
				{
					return mapped_sortkeys[sortkey];
				}

				ZONETOOL_ERROR("Could not find mapped IW7 sortkey for sortkey: %d (material: %s)", sortkey, matname.data());

				return sortkey;
			}

			std::unordered_map<std::uint8_t, std::uint8_t> mapped_camera_regions =
			{
				{IW5::CAMERA_REGION_LIT_OPAQUE, 0},
				{IW5::CAMERA_REGION_LIT_TRANS, 1},
				{IW5::CAMERA_REGION_EMISSIVE, 4},
				{IW5::CAMERA_REGION_NONE, 4},
			};

			std::unordered_map<std::string, std::uint8_t> mapped_camera_regions_by_techset =
			{
				{"mc_shadowcaster_atest", 11},
				{"wc_shadowcaster", 11},
			};

			std::uint8_t get_IW7_camera_region(std::uint8_t camera_region, std::string matname, std::string IW7_techset)
			{
				if (mapped_camera_regions_by_techset.find(IW7_techset) != mapped_camera_regions_by_techset.end())
				{
					return mapped_camera_regions_by_techset[IW7_techset];
				}

				if (mapped_camera_regions.contains(camera_region))
				{
					return mapped_camera_regions[camera_region];
				}

				ZONETOOL_ERROR("Could not find mapped IW7 camera region for camera region: %d (material: %s)", camera_region, matname.data());

				return camera_region;
			}

			std::unordered_map<std::string, std::uint8_t> mapped_render_flags_by_techset =
			{
				{"2d", 0x1},
				{"mc_shadowcaster_atest", 0x2},
				{"wc_shadowcaster", 0x2},
			};

			std::int32_t get_render_flags_by_techset(std::string IW7_techset)
			{
				std::int32_t flags = 0;

				if (mapped_render_flags_by_techset.find(IW7_techset) != mapped_render_flags_by_techset.end())
				{
					flags |= mapped_render_flags_by_techset[IW7_techset];
				}

				if (IW7_techset.starts_with("eq_") || IW7_techset.starts_with("ev_"))
				{
					flags |= 0x1;
				}

				return flags;
			}
		}
	}

	namespace IW5::IW7Dumper
	{
		std::string clean_name(const std::string& name)
		{
			auto new_name = name;

			for (auto i = 0u; i < name.size(); i++)
			{
				switch (new_name[i])
				{
				case '*':
					new_name[i] = '_';
					break;
				}
			}

			return new_name;
		}

		bool has_color_tint(const Material* asset)
		{
			static constexpr float identity[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

			for (int i = 0; i < asset->constantCount; ++i)
			{
				const auto& constant = asset->constantTable[i];
				if (constant.nameHash == 3054254906u)
				{
					return !std::equal(std::begin(constant.literal), std::end(constant.literal), std::begin(identity));
				}
			}

			return false;
		}

		void dump(Material* asset, bool geotrail)
		{
			if (asset)
			{
				const auto has_ct = has_color_tint(asset);

				auto new_name = IW7::replace_material_prefix(asset->name, asset->techniqueSet ? asset->techniqueSet->name : "", geotrail, has_ct);
				auto c_name = clean_name(new_name);

				const auto path = "materials\\"s + new_name + ".json"s;
				auto file = zonetool::filesystem::file(path);

				ordered_json matdata;

				//matdata["name"] = new_name;

				std::string techset;
				std::string iw7_techset;
				if (asset->techniqueSet)
				{
					techset = asset->techniqueSet->name;

					bool result = false;
					iw7_techset = IW7::get_IW7_techset(techset, asset->name, &result, geotrail, has_ct);
					if (!result)
					{
						matdata["techniqueSet->original"] = techset;
						//ZONETOOL_ERROR("Not dumping material \"%s\"", asset->name);
						//return;
					}
					matdata["techniqueSet->name"] = iw7_techset;
				}

				matdata["gameFlags"] = asset->info.gameFlags; // convert
				matdata["unkFlags"] = 0; // idk
				matdata["sortKey"] = IW7::get_IW7_sortkey(asset->info.sortKey, asset->name, iw7_techset);
				matdata["renderFlags"] = IW7::get_render_flags_by_techset(iw7_techset); // idk

				matdata["textureAtlasRowCount"] = asset->info.textureAtlasRowCount;
				matdata["textureAtlasColumnCount"] = asset->info.textureAtlasColumnCount;
				matdata["textureAtlasFrameBlend"] = 0;
				matdata["textureAtlasAsArray"] = 0;

				matdata["surfaceTypeBits"] = asset->info.surfaceTypeBits; // convert
				// hashIndex;

				matdata["stateFlags"] = asset->stateFlags; // convert
				matdata["cameraRegion"] = IW7::get_IW7_camera_region(asset->cameraRegion, asset->name, iw7_techset);
				matdata["materialType"] = IW7::get_material_type_from_techset(iw7_techset);
				matdata["assetFlags"] = 0; // IW7::MTL_ASSETFLAG_NONE;

				// fixes
				if (matdata["cameraRegion"].get<uint8_t>() == 4 && matdata["sortKey"].get<uint8_t>() != 41)
				{
					matdata["cameraRegion"] = 11;
				}

				ordered_json constant_table;
				for (int i = 0; i < asset->constantCount && techset != "2d"; i++)
				{
					ordered_json table;
					std::string constant_name = asset->constantTable[i].name;
					const auto constant_hash = asset->constantTable[i].nameHash;

					if (constant_name.size() > 12)
					{
						constant_name.resize(12);
					}

					if (constant_hash == 1033475292) // envMapParms
					{
						continue;
					}

					table["name"] = constant_name.data();
					table["nameHash"] = constant_hash;

					nlohmann::json literal_entry;
					literal_entry[0] = asset->constantTable[i].literal[0];
					literal_entry[1] = asset->constantTable[i].literal[1];
					literal_entry[2] = asset->constantTable[i].literal[2];
					literal_entry[3] = asset->constantTable[i].literal[3];
					table["literal"] = literal_entry;

					constant_table[constant_table.size()] = table;
				}

#define CONSTANT_TABLE_ADD_IF_NOT_FOUND(CONST_NAME, CONST_HASH, LITERAL_1, LITERAL_2, LITERAL_3, LITERAL_4) \
				bool has_const = false; \
				std::size_t insert_position = constant_table.size(); \
				for (std::size_t i = 0; i < constant_table.size(); i++) \
				{ \
					if (constant_table[i]["nameHash"].get<std::size_t>() == CONST_HASH) \
					{ \
						has_const = true; \
						break; \
					} \
					if (constant_table[i]["nameHash"].get<std::size_t>() > CONST_HASH) \
					{ \
						insert_position = i; \
						break; \
					} \
				} \
				if (!has_const) \
				{ \
					ordered_json table; \
					table["name"] = CONST_NAME; \
					table["nameHash"] = CONST_HASH; \
					nlohmann::json literal_entry; \
					literal_entry[0] = LITERAL_1; \
					literal_entry[1] = LITERAL_2; \
					literal_entry[2] = LITERAL_3; \
					literal_entry[3] = LITERAL_4; \
					table["literal"] = literal_entry; \
					constant_table.insert(constant_table.begin() + insert_position, table); \
				}

				if (iw7_techset.find("s0") != std::string::npos)
				{
					CONSTANT_TABLE_ADD_IF_NOT_FOUND("reflectionRa", 3344177073u, 8096.0f, 0.0f, 0.0f, 0.0f);
				}
				if (iw7_techset.find("_lin") != std::string::npos)
				{
					CONSTANT_TABLE_ADD_IF_NOT_FOUND("textureAtlas", 1128936273u,
						static_cast<float>(asset->info.textureAtlasColumnCount), static_cast<float>(asset->info.textureAtlasRowCount), 1.0f, 1.0f);
				}

				matdata["constantTable"] = constant_table;

				ordered_json material_images;
				for (auto i = 0; i < asset->textureCount; i++)
				{
					ordered_json image;
					if (asset->textureTable[i].semantic == 11)
					{
						auto* water = asset->textureTable[i].u.water;
						if (water->image && water->image->name)
						{
							image["image"] = water->image->name;
						}
						else
						{
							image["image"] = "";
						}
					}
					else
					{
						if (asset->textureTable[i].u.image && asset->textureTable[i].u.image->name)
						{
							image["image"] = asset->textureTable[i].u.image->name;
						}
						else
						{
							image["image"] = "";
						}
					}

					image["semantic"] = asset->textureTable[i].semantic = IW7::convert_semantic(asset->textureTable[i].semantic);
					image["samplerState"] = asset->textureTable[i].samplerState == 11 ? 19 : asset->textureTable[i].samplerState; // convert? ( should be fine )
					image["lastCharacter"] = asset->textureTable[i].nameEnd;
					image["firstCharacter"] = asset->textureTable[i].nameStart;
					image["typeHash"] = asset->textureTable[i].nameHash;

					// add image data to material
					material_images.push_back(image);
				}

#define IMAGE_ADD_IF_NOT_FOUND(IMAGE, SEMANTIC, SAMPLER_STATE, LAST_CHARACTER, FIRST_CHARACTER, HASH) \
				bool has_image = false; \
				std::size_t insert_position = material_images.size(); \
				for (std::size_t i = 0; i < material_images.size(); i++) \
				{ \
					if (material_images[i]["typeHash"].get<std::size_t>() == HASH) \
					{ \
						has_image = true; \
						break; \
					} \
					if (material_images[i]["typeHash"].get<std::size_t>() > HASH) \
					{ \
						insert_position = i; \
						break; \
					} \
				} \
				if (!has_image) \
				{ \
					ordered_json image; \
					image["image"] = IMAGE; \
					image["semantic"] = SEMANTIC; \
					image["samplerState"] = SAMPLER_STATE; \
					image["lastCharacter"] = LAST_CHARACTER; \
					image["firstCharacter"] = FIRST_CHARACTER; \
					image["typeHash"] = HASH; \
					material_images.insert(material_images.begin() + insert_position, image); \
				}

				if (iw7_techset.find("n0") != std::string::npos)
				{
					IMAGE_ADD_IF_NOT_FOUND("$identitynormalmap", 5, 1, 112, 110, 1507003663);
				}

				matdata["textureTable"] = material_images;

				auto str = matdata.dump(4, ' ', true, nlohmann::detail::error_handler_t::replace);

				matdata.clear();

				file.open("wb");
				file.write(str);
				file.close();
			}
		}
	}
}