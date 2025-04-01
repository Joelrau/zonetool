#include "stdafx.hpp"
#include "Material.hpp"

//#include "IW6/Assets/Material.hpp"

namespace ZoneTool
{
	namespace IW6
	{
		std::unordered_map<std::string, std::string> mapped_techsets =
		{
//			IW5,										IW6
			{"wc_l_sm_a0c0",							"wc_l_sm_a0c0l0_nfwpf"},
			{"wc_l_sm_r0c0",							"wc_l_sm_r0c0l0_nfwpf"},
			{"wc_l_sm_r0c0n0",							"wc_l_sm_r0c0n0l0_nfwpf"},
			{"wc_l_sm_r0c0n0s0",						"wc_l_sm_r0c0n0s0l0_nfwpf"},
			{"wc_l_sm_r0c0n0s0p0",						"wc_l_sm_r0c0n0s0l0p0_nfwpf"},
			{"wc_l_sm_r0c0q0n0s0",						"wc_l_sm_r0c0q0n0s0l0_nfwpf"},
			{"wc_l_sm_r0c0q0n0s0p0",					"wc_l_sm_r0c0q0n0s0l0p0_nfwpf"},
			{"mc_l_sm_r0c0d0n0s0",						"mc_l_sm_r0c0d0n0s0l0_nfwpf"},
			{"wc_l_sm_r0c0d0n0s0",						"wc_l_sm_r0c0d0n0s0l0_nfwpf"},
			{"wc_l_sm_r0c0d0n0s0p0",					"wc_l_sm_r0c0d0n0s0l0p0_nfwpf"},
			{"wc_l_sm_r0c0s0",							"wc_l_sm_r0c0s0l0_nfwpf"},
			{"mc_l_sm_r0c0s0_custom_grenade",			"wc_l_sm_r0c0s0"}, // couldn't find
			{"wc_l_sm_t0c0",							"wc_l_sm_t0c0l0_nfwpf"},
			{"wc_l_sm_t0c0n0",							"wc_l_sm_t0c0n0l0_nfwpf"},
			{"wc_l_sm_t0c0n0s0",						"wc_l_sm_t0c0n0s0l0_nfwpf"},
			{"wc_l_sm_t0c0p0",							"wc_l_sm_t0c0l0_nfwpf"},
			{"wc_l_sm_b0c0",							"wc_l_sm_ndw_b0c0l0_nfwpf"},
			{"wc_l_sm_b0c0n0",							"wc_l_sm_ndw_b0c0n0l0_nfwpf"},
			{"wc_l_sm_b0c0n0s0",						"wc_l_sm_ndw_b0c0n0s0l0_nfwpf"},
			{"wc_l_sm_b0c0s0p0",						"wc_l_sm_ndw_b0c0s0l0p0_nfwpf"},
			{"wc_l_sm_b0c0n0s0p0",						"wc_l_sm_ndw_b0c0n0s0l0p0_nfwpf"},
			{"wc_l_sm_b0c0n0p0",						"wc_l_sm_ndw_b0c0n0l0p0_nfwpf"},
			{"wc_l_sm_b0c0s0",							"wc_l_sm_ndw_b0c0s0l0_nfwpf"},
			{"wc_l_sm_b0c0q0n0s0",						"wc_l_sm_ndw_b0c0q0n0s0l0_nfwpf"},
			{"wc_l_sm_ua_b0c0n0s0p0_nocast",			"wc_l_sm_ndw_ua_b0c0s0l0p0_cltrans"}, // coudln't find nocast
			{"wc_l_sm_b0c0n0s0_custom_growing_ice_cracks", "wc_l_sm_ndw_b0c0n0s0l0_custom_growing_ice_cracks_nfwpf"},
			{"wc_l_sm_b0c0n0s0_custom_growing_ice_cracks_sat", "wc_l_sm_ndw_b0c0n0s0l0_custom_growing_ice_cracks_cltrans"},
			{"wc_unlit_add_lin",						"wc_unlit_add_lin_ndw_bl_nfwpf"},
			{"wc_unlit_multiply_lin",					"wc_unlit_multiply_lin_ndw_bl_nfwpf"},
			{"wc_unlit_replace_lin",					"wc_unlit_replace_lin_bl_nfwpf"},
			{"wc_sky",									"wc_sky_bl"},
			{"wc_shadowcaster",							"wc_shadowcaster"},
			{"wc_water",								"2d"}, // no techset found
			{"wc_tools",								"2d"}, // no techset found

			{"mc_l_sm_r0c0",							"mc_l_sm_r0c0l0_nfwpf"},
			{"mc_l_sm_r0c0_sat",						"mc_l_sm_r0c0l0_nfwpf"}, // couldn't find cltrans
			{"mc_l_sm_r0c0s0",							"mc_l_sm_r0c0s0l0_nfwpf"},
			{"mc_l_sm_r0c0n0",							"mc_l_sm_r0c0n0l0_nfwpf"},
			{"mc_l_sm_r0c0n0s0",						"mc_l_sm_r0c0n0s0l0_nfwpf"},
			{"mc_l_sm_r0c0n0s0p0",						"mc_l_sm_r0c0n0s0l0p0_nfwpf"},
			{"mc_l_sm_r0c0n0s0p0_sat",					"mc_l_sm_r0c0n0s0l0p0_nfwpf"}, // couldn't find cltrans
			{"mc_l_sm_r0c0q0n0s0",						"mc_l_sm_r0c0q0n0s0l0_nfwpf"},
			{"mc_l_sm_r0c0q0n0s0p0",					"mc_l_sm_r0c0q0n0s0l0p0_nfwpf"},
			{"mc_l_sm_t0c0",							"mc_l_sm_t0c0l0_nfwpf"},
			{"mc_l_sm_t0c0_nocast",						"mc_l_sm_t0c0l0_nocast_nfwpf"},
			{"mc_l_sm_t0c0n0",							"mc_l_sm_t0c0n0l0_nfwpf"},
			{"mc_l_sm_t0c0n0s0",						"mc_l_sm_t0c0n0s0l0_nfwpf"},
			{"mc_l_sm_t0c0q0n0s0p0",					"mc_l_sm_t0c0q0n0s0l0_nfwpf"}, // couldn't find p0
			{"mc_l_sm_b0c0",							"mc_l_sm_ndw_b0c0l0_nfwpf"},
			{"mc_l_sm_b0c0_sat",						"mc_l_sm_ndw_b0c0l0_cltrans"},
			{"mc_l_sm_b0c0_nocast",						"mc_l_sm_ndw_b0c0l0_nfwpf"}, // couldn't find nocast
			{"mc_l_sm_b0c0s0",							"mc_l_sm_ndw_b0c0s0l0_nfwpf"},
			{"mc_l_sm_b0c0n0",							"mc_l_sm_ndw_b0c0n0l0_nfwpf"},
			{"mc_l_sm_b0c0n0_sat",						"mc_l_sm_ndw_b0c0n0l0_cltrans"},
			{"mc_l_sm_b0c0n0s0",						"mc_l_sm_ndw_b0c0n0s0l0_nfwpf"},
			{"mc_l_sm_b0c0n0s0_sat",					"mc_l_sm_ndw_b0c0n0s0l0_cltrans"},
			{"mc_l_sm_b0c0n0p0_sat",					"mc_l_sm_ndw_b0c0n0s0l0_cltrans"}, // couldn't find p0
			{"mc_l_sm_b0c0n0s0p0",						"mc_l_sm_ndw_b0c0n0s0l0p0_nfwpf"},
			{"mc_l_sm_b0c0n0s0p0_sat",					"mc_l_sm_ndw_b0c0n0s0l0p0_cltrans"},
			{"mc_l_sm_b0c0p0",							"mc_l_sm_ndw_b0c0l0_nfwpf"}, // couldn't find p0
			{"mc_l_sm_b0c0q0n0s0_sat",					"mc_l_sm_ndw_b0c0q0n0s0l0_nfwpf"},
			{"mc_l_sm_b0c0n0s0_custom_growing_ice_cracks", "mc_l_sm_ndw_b0c0n0s0l0_custom_growing_ice_cracks_nfwpf"},
			{"mc_l_sm_b0c0n0s0_custom_growing_ice_cracks_sat", "mc_l_sm_ndw_b0c0n0s0l0_custom_growing_ice_cracks_cltrans"},
			{"mc_l_sm_flag_t0c0n0s0",					"mc_l_sm_flag_t0c0n0s0l0_nfwpf"},
			{"mc_l_r0c0n0s0",							"mc_l_r0c0n0s0l0_nfwpf"},
			{"mc_l_r0c0n0s0_nocast",					"mc_l_r0c0n0s0l0_nocast_nfwpf"},
			{"mc_l_t0c0n0s0",							"mc_l_t0c0n0s0l0_nfwpf"},
			{"mc_unlit_add_lin",						"mc_unlit_add_lin_ndw_bl_nfwpf"},
			{"mc_unlit_blend_lin",						"mc_unlit_blend_lin_ndw_bl_abl_cltrans"}, // could be wrong
			{"mc_unlit_replace_lin",					"mc_unlit_replace_lin_bl_nfwpf"},
			{"mc_unlit_replace_lin_nocast",				"mc_unlit_replace_lin_nocast_bl_nfwpf"},
			{"mc_ambient_t0c0",							"mc_ambient_t0c0l0_nfwpf"},
			{"mc_ambient_t0c0_nocast",					"mc_ambient_t0c0l0_nocast_nfwpf"},
			{"mc_shadowcaster_atest",					"mc_shadowcaster_atest"},

			{"mc_effect_falloff_add_nofog",				"mc_effect_falloff_add_nofog_ndw_flow_nohdrcolor_blm_blmod"}, // could be wrong
			{"mc_effect_falloff_add_lin_nofog",			"mc_effect_falloff_add_lin_nofog_ndw_bl_blmod"},
			{"mc_effect_zfeather_falloff_add_lin_nofog", "mc_effect_zfeather_falloff_add_lin_nofog_ndw_bl_blmod"},
			{"mc_effect_zfeather_falloff_add_lin_nofog_eyeoffset", "mc_effect_zfeather_falloff_add_lin_nofog_eyeoffset_ndw_bl_blmod"},
			{"mc_effect_zfeather_falloff_screen_nofog_eyeoffset", "mc_effect_zfeather_falloff_screen_lin_nofog_eyeoffset_ndw_bl"},

			{"2d",										"2d"},

			{"distortion_scale",						"distortion_scale"},
			{"distortion_scale_zfeather",				"distortion_scale_zfeather"},

			{"effect_add_nofog",						"effect_add_nofog_ndw_bl_blmod"},
			{"effect_blend",							"effect_blend_lin_ndw_bl_abl"}, // could be wrong (lin)
			{"effect_blend_nofog",						"effect_blend_lin_nofog_ndw_bl_abl"}, // could be wrong (lin)
			{"effect_replace_lin",						"effect_replace_lin_ndw_bl_abl"}, // could be wrong
			{"effect_zfeather_add",						"effect_zfeather_add_lin_ndw_bl_blmod"}, // could be wrong (lin)
			{"effect_zfeather_add_nofog",				"effect_zfeather_add_lin_nofog_ndw_bl_blmod"}, // could be wrong (lin)
			{"effect_zfeather_blend",					"effect_zfeather_blend_ndw_bl_abl"}, // could be wrong
			{"effect_zfeather_blend_nofog",				"effect_zfeather_blend_ndw_bl_abl"}, // could be wrong + couldn't find nofog
			{"effect_zfeather_falloff_add",				"effect_zfeather_falloff_add_lin_ndw_bl_blmod"}, // (lin)
			{"effect_zfeather_falloff_add_nofog_eyeoffset", "effect_zfeather_falloff_add_lin_nofog_eyeoffset_ndw_bl_blmod"},
			{"effect_zfeather_falloff_blend",			"effect_zfeather_falloff_blend_lin_ndw_bl_abl"}, // could be wrong (lin)
			{"effect_zfeather_add_nofog_eyeoffset",		"effect_zfeather_add_lin_nofog_eyeoffset_ndw_bl_blmod"},

			{"particle_cloud",							"particle_cloud_replace_bl_abl_lin"}, // could be wrong
			{"particle_cloud_add",						"particle_cloud_add_bl_blmod_lin"},
			{"particle_cloud_outdoor_add",				"particle_cloud_outdoor_add_bl_blmod_lin"},
			{"particle_cloud_sparkf_add",				"particle_cloud_sparkf_add_bl_blmod_lin"},
			{"particle_cloud_spark_add",				"particle_cloud_spark_add_bl_blmod_lin"},

			{"tools_b0c0",								"tools_b0c0"},
		};

		namespace
		{
			std::string get_iw6_techset(std::string name, std::string matname, bool* result)
			{
				if (mapped_techsets.find(name) == mapped_techsets.end())
				{
					ZONETOOL_ERROR("Could not find mapped IW6 techset for IW5 techset \"%s\" (material: %s)", name.data(), matname.data());
					*result = false;
					return "2d";
				}
				*result = true;
				return mapped_techsets[name];
			}

			std::unordered_map<std::uint8_t, std::uint8_t> mapped_sortkeys =
			{
				// should be the same
			};

			std::unordered_map<std::string, std::uint8_t> mapped_sortkeys_by_techset =
			{
				
			};

			std::uint8_t get_iw6_sortkey(std::uint8_t sortkey, std::string matname, std::string iw6_techset)
			{
				if (mapped_sortkeys_by_techset.find(iw6_techset) != mapped_sortkeys_by_techset.end())
				{
					return mapped_sortkeys_by_techset[iw6_techset];
				}

				if (mapped_sortkeys.contains(sortkey))
				{
					return mapped_sortkeys[sortkey];
				}

				//ZONETOOL_ERROR("Could not find mapped IW6 sortkey for sortkey: %d (material: %s)", sortkey, matname.data());

				return sortkey;
			}

			std::unordered_map<std::uint8_t, std::uint8_t> mapped_camera_regions =
			{
				{IW5::CAMERA_REGION_LIT_OPAQUE, IW6::CAMERA_REGION_LIT_OPAQUE},
				{IW5::CAMERA_REGION_LIT_TRANS, IW6::CAMERA_REGION_LIT_TRANS},
				{IW5::CAMERA_REGION_EMISSIVE, IW6::CAMERA_REGION_EMISSIVE},
				{IW5::CAMERA_REGION_DEPTH_HACK, IW6::CAMERA_REGION_DEPTH_HACK},
				{IW5::CAMERA_REGION_NONE, IW6::CAMERA_REGION_NONE},
			};

			std::unordered_map<std::string, std::uint8_t> mapped_camera_regions_by_techset =
			{
				
			};

			std::uint8_t get_iw6_camera_region(std::uint8_t camera_region, std::string matname, std::string iw6_techset)
			{
				if (mapped_camera_regions_by_techset.find(iw6_techset) != mapped_camera_regions_by_techset.end())
				{
					return mapped_camera_regions_by_techset[iw6_techset];
				}

				if (mapped_camera_regions.contains(camera_region))
				{
					return mapped_camera_regions[camera_region];
				}

				ZONETOOL_ERROR("Could not find mapped IW6 camera region for camera region: %d (material: %s)", camera_region, matname.data());

				return camera_region;
			}

			std::uint8_t get_material_type_from_name(const std::string& name)
			{
				auto prefix_idx = name.find("/");
				if (prefix_idx != std::string::npos && prefix_idx)
				{
					std::string prefix = std::string(name.begin(), name.begin() + prefix_idx);
					if (prefix == "m")
					{
						return IW6::MTL_TYPE_MODEL;
					}
					else if (prefix == "me")
					{
						return IW6::MTL_TYPE_MODEL_VERTCOL;
					}
					else if (prefix == "mc")
					{
						return IW6::MTL_TYPE_MODEL_VERTCOL;
					}
					else if (prefix == "w")
					{
						return IW6::MTL_TYPE_WORLD;
					}
					else if (prefix == "wc")
					{
						return IW6::MTL_TYPE_WORLD_VERTCOL;
					}
					else if (prefix != "gfx")
					{
						ZONETOOL_WARNING("Unknown material type for prefix \"%s\" (material: %s)", prefix.data(), name.data());
					}
				}
				return IW6::MTL_TYPE_DEFAULT;
			}
		}
	}

	namespace IW5::IW6Dumper
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

#define MATERIAL_DUMP_STRING(entry) \
	matdata[#entry] = std::string(asset->entry);

#define MATERIAL_DUMP(entry) \
	matdata[#entry] = asset->entry;

#define MATERIAL_DUMP_INFO(entry) \
	matdata[#entry] = asset->info.entry;

		void dump(Material* asset)
		{
			if (asset)
			{
				auto c_name = clean_name(asset->name);

				const auto path = "materials\\"s + c_name + ".json"s;
				auto file = zonetool::filesystem::file(path);

				ordered_json matdata;

				MATERIAL_DUMP_STRING(name);

				std::string iw5_techset;
				std::string iw6_techset;
				if (asset->techniqueSet)
				{
					iw5_techset = asset->techniqueSet->name;
					if (iw5_techset.starts_with("iw3/"))
					{
						iw5_techset.erase(0, 4);
					}

					if (iw5_techset.ends_with("_sat"))
					{
						auto idx = iw5_techset.find("_sat");
						iw5_techset.erase(idx, 4);
					}

					if (iw5_techset.ends_with(".3x")) // iw3xport
					{
						auto idx = iw5_techset.find(".3x");
						iw5_techset.erase(idx, 3);
					}

					bool result = false;
					iw6_techset = IW6::get_iw6_techset(iw5_techset, asset->name, &result);
					if (!result)
					{
						ZONETOOL_ERROR("Not dumping material \"%s\"", asset->name);
						return;
					}
					matdata["techniqueSet->name"] = iw6_techset;
				}

				matdata["gameFlags"] = asset->info.gameFlags; // convert
				matdata["sortKey"] = IW6::get_iw6_sortkey(asset->info.sortKey, asset->name, iw6_techset); // convert
				matdata["renderFlags"] = 0; // idk

				matdata["textureAtlasRowCount"] = asset->info.textureAtlasRowCount;
				matdata["textureAtlasColumnCount"] = asset->info.textureAtlasColumnCount;
				matdata["textureAtlasFrameBlend"] = 0;
				matdata["textureAtlasAsArray"] = 0;

				matdata["surfaceTypeBits"] = asset->info.surfaceTypeBits; // convert
				// hashIndex;

				//matdata["stateFlags"] = asset->stateFlags; // convert
				matdata["cameraRegion"] = IW6::get_iw6_camera_region(asset->cameraRegion, asset->name, iw6_techset);
				matdata["materialType"] = IW6::get_material_type_from_name(asset->name);
				matdata["assetFlags"] = IW6::MTL_ASSETFLAG_NONE;

				ordered_json constant_table;
				for (int i = 0; i < asset->constantCount && iw5_techset != "2d"; i++)
				{
					ordered_json table;
					std::string constant_name = asset->constantTable[i].name;

					if (constant_name.size() > 12)
					{
						constant_name.resize(12);
					}

					if (constant_name == "colorTint" &&
						asset->constantTable[i].literal[0] == 1.0f &&
						asset->constantTable[i].literal[1] == 1.0f &&
						asset->constantTable[i].literal[2] == 1.0f &&
						asset->constantTable[i].literal[3] == 1.0f)
					{
						continue;
					}

					table["name"] = constant_name.data();
					table["nameHash"] = asset->constantTable[i].nameHash;

					nlohmann::json literal_entry;
					literal_entry[0] = asset->constantTable[i].literal[0];
					literal_entry[1] = asset->constantTable[i].literal[1];
					literal_entry[2] = asset->constantTable[i].literal[2];
					literal_entry[3] = asset->constantTable[i].literal[3];
					table["literal"] = literal_entry;

					constant_table[constant_table.size()] = table;
				}

#define CONSTANT_TABLE_ADD_IF_NOT_FOUND(CONST_NAME, CONST_HASH, LITERAL_1, LITERAL_2, LITERAL_3, LITERAL_4) \
				bool has_table = false; \
				unsigned int table_idx = 0; \
				for (std::size_t i = 0; i < constant_table.size(); i++) \
				{ \
					if (constant_table[i]["nameHash"].get<unsigned int>() == CONST_HASH) \
					{ \
						has_table = true; \
					} \
				} \
				if (!has_table) \
				{ \
					for (std::size_t i = 0; i < constant_table.size(); i++) \
					{ \
						if (constant_table[i]["nameHash"].get<unsigned int>() > CONST_HASH) \
						{ \
							table_idx = i; \
							break; \
						} \
					} \
					if (table_idx == 0) \
					{ \
						table_idx = static_cast<unsigned int>(constant_table.size()); \
					} \
					ordered_json table; \
					table["name"] = CONST_NAME; \
					table["nameHash"] = CONST_HASH; \
					nlohmann::json literal_entry; \
					literal_entry[0] = LITERAL_1; \
					literal_entry[1] = LITERAL_2; \
					literal_entry[2] = LITERAL_3; \
					literal_entry[3] = LITERAL_4; \
					table["literal"] = literal_entry; \
					if (constant_table.is_null()) \
					{ \
						constant_table[constant_table.size()] = table; \
					} \
					else \
					{ \
						constant_table.insert(constant_table.begin() + table_idx, table); \
					} \
				}

				if (iw6_techset.find("_flag_") != std::string::npos)
				{
					CONSTANT_TABLE_ADD_IF_NOT_FOUND("flagParms", 2292903761, 1.0f, 0.0f, 0.0f, 0.0f);
				}

				if (iw6_techset.find("_cso_") != std::string::npos)
				{
					CONSTANT_TABLE_ADD_IF_NOT_FOUND("hdrColorParm", 3379990750, 20.0f, 0.0f, 0.0f, 0.0f);
				}

				if (iw6_techset.find("shadowcaster") != std::string::npos)
				{
					CONSTANT_TABLE_ADD_IF_NOT_FOUND("uvScale", 3809220315, 1.0f, 1.0f, 0.0f, 0.0f);
				}

				if (iw6_techset != "2d")
				{
					CONSTANT_TABLE_ADD_IF_NOT_FOUND("bloomParams", 3733414975, 1.0f, 0.0f, 0.0f, 0.0f);
				}

				matdata["constantTable"] = constant_table;

				int i_3447584578 = -1;

				ordered_json material_images;
				for (auto i = 0; i < asset->textureCount; i++)
				{
					if (asset->textureTable[i].nameHash > 3447584578 && i_3447584578 == -1)
					{
						i_3447584578 = i;
					}

					ordered_json image;
					if (asset->textureTable[i].semantic == 11)
					{
						auto* water = reinterpret_cast<water_t*>(asset->textureTable[i].u.image);
						if (water->image && water->image->name)
						{
							image["image"] = water->image->name;
						}
						else
						{
							image["image"] = "";
						}
						//image["waterinfo"] = nullptr;
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

					image["semantic"] = asset->textureTable[i].semantic == 11 ? 2 : asset->textureTable[i].semantic; // convert? ( should be the same )
					image["samplerState"] = asset->textureTable[i].samplerState == 11 ? 19 : asset->textureTable[i].samplerState; // convert? ( should be fine )
					image["lastCharacter"] = asset->textureTable[i].nameEnd;
					image["firstCharacter"] = asset->textureTable[i].nameStart;
					image["typeHash"] = asset->textureTable[i].nameHash;

					// add image data to material
					material_images.push_back(image);
				}

				// fix for certain techniques
				if (iw6_techset.find("_lmpb_") != std::string::npos || iw6_techset.find("_flag_") != std::string::npos)
				{
					if (i_3447584578 == -1)
					{
						i_3447584578 = asset->textureCount;
					}

					ordered_json image;
					image["image"] = "~envbrdflut_ggx_16-rg";
					image["semantic"] = 15;
					image["samplerState"] = 226;
					image["lastCharacter"] = 116;
					image["firstCharacter"] = 101;
					image["typeHash"] = 3447584578;
					material_images.insert(material_images.begin() + i_3447584578, image);
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