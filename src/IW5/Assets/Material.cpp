#include "stdafx.hpp"
//#include "H1/Assets/Material.hpp"
#include "H1/Utils/IO/assetmanager.hpp"

#include "Json.hpp"
using json = nlohmann::json;
using ordered_json = nlohmann::ordered_json;

namespace
{
	std::unordered_map<std::string, std::string> mapped_techsets =
	{
		//	IW5,										H1
			{"wc_l_sm_a0c0",							"wc_l_sm_a0c0_nfwpf_frt_aat"},
			{"wc_l_sm_r0c0",							"wc_l_sm_r0c0_nfwpf"},
			{"mc_l_sm_r0c0d0n0s0",						"wc_l_sm_r0c0d0n0sd0_nfwpf"},
			{"wc_l_sm_r0c0d0n0s0p0",					"wc_l_sm_r0c0d0n0sd0_nfwpf"}, // couldn't find
			{"wc_l_sm_r0c0n0",							"wc_l_sm_r0c0n0_nfwpf"},
			{"wc_l_sm_r0c0n0s0",						"wc_l_sm_r0c0n0sd0_nfwpf"},
			{"wc_l_sm_r0c0n0s0p0",						"wc_l_sm_r0c0n0sd0p0_nfwpf"},
			{"wc_l_sm_r0c0q0n0s0",						"wc_l_sm_r0c0q0n0sd0_nfwpf"},
			{"wc_l_sm_r0c0q0n0s0p0",					"wc_l_sm_r0c0q0n0sd0p0_nfwpf"},
			{"wc_l_sm_r0c0s0",							"wc_l_sm_r0c0sd0_nfwpf"},
			{"mc_l_sm_r0c0s0_custom_grenade",			"wc_l_sm_r0c0sd0_nfwpf"}, // couldn't find
			{"mc_l_sm_r0c0s0p0",						"mc_l_sm_r0c0sd0p0_nfwpf"},
			{"wc_l_sm_t0c0",							"wc_l_sm_t0c0_nfwpf"},
			{"wc_l_sm_t0c0n0",							"wc_l_sm_t0c0n0_nfwpf"},
			{"wc_l_sm_t0c0n0s0",						"wc_l_sm_t0c0n0sd0_nfwpf"},
			{"wc_l_sm_t0c0p0",							"wc_l_sm_t0c0_nfwpf"}, // couldn't find
			{"wc_l_sm_b0c0",							"wc_l_sm_lmpb_ndw_b0c0_nfwpf_frt_im_aat"},
			{"wc_l_sm_b0c0n0",							"wc_l_sm_lmpb_ndw_b0c0n0_nfwpf_frt_im_aat"},
			{"wc_l_sm_b0c0n0s0",						"wc_l_sm_lmpb_ndw_b0c0n0sd0_nfwpf_frt_im_aat"},
			{"wc_l_sm_b0c0n0s0p0",						"wc_l_sm_lmpb_ndw_b0c0n0sd0_nfwpf_frt_im_aat"}, // couldn't find
			{"wc_l_sm_b0c0n0p0",						"wc_l_sm_lmpb_ndw_b0c0n0_nfwpf_frt_im_aat"}, // couldn't find
			{"wc_l_sm_b0c0s0",							"wc_l_sm_lmpb_ndw_b0c0sd0_nfwpf_frt_aat"},
			{"wc_l_sm_b0c0s0p0",						"wc_l_sm_lmpb_ndw_b0c0sd0_nfwpf_frt_aat"}, // couldn't find
			{"wc_l_sm_b0c0s0p0_sat",					"wc_l_sm_lmpb_ndw_b0c0sd0_nfwpf_frt_aat"}, // no sat
			{"wc_l_sm_b0c0q0n0s0",						"wc_l_sm_lmpb_ndw_b0c0n0sd0_nfwpf_frt_im_aat"},
			{"wc_l_sm_ua_b0c0n0s0p0_nocast",			"wc_l_sm_ndw_ua_b0c0n0sd0p0_cltrans_nocast_frt_aat"},
			{"wc_l_sm_b0c0n0s0_custom_growing_ice_cracks", "wc_l_sm_lmpb_ndw_b0c0n0sd0_nfwpf_frt_im_aat"}, // coudln't find
			{"wc_l_sm_b0c0n0s0_custom_growing_ice_cracks_sat", "wc_l_sm_lmpb_ndw_b0c0n0sd0_nfwpf_frt_im_aat"}, // couldn't find

			{"wc_unlit_multiply_lin",					"wc_unlit_multiply_lin_ndw_nfwpf"},
			{"wc_unlit_replace_lin",					"wc_unlit_replace_lin_nfwpf"},
			{"wc_sky",									"wc_sky_cso_nfwpf"},
			{"wc_shadowcaster",							"wc_shadowcaster"},
			//{"wc_water",								"2d"}, // couldn't find
			//{"wc_tools",								"2d"}, // couldn't find

			{"mc_l_sm_r0c0",							"mc_l_sm_r0c0_nfwpf"},
			{"mc_l_sm_r0c0_sat",						"mc_l_sm_r0c0_nfwpf"}, // no sat
			{"mc_l_sm_r0c0d0n0s0p0",					"mc_l_sm_r0c0d0n0sd0p0_nfwpf"},
			{"mc_l_sm_r0c0d0n0s0p0_sat",				"mc_l_sm_r0c0d0n0sd0p0_nfwpf"}, // no sat
			{"mc_l_sm_r0c0s0",							"mc_l_sm_r0c0sd0_nfwpf"},
			{"mc_l_sm_r0c0n0",							"mc_l_sm_r0c0n0_nfwpf"},
			{"mc_l_sm_r0c0n0s0",						"mc_l_sm_r0c0n0sd0_nfwpf"},
			{"mc_l_sm_r0c0n0s0_sat",					"mc_l_sm_r0c0n0sd0_nfwpf"}, // no sat
			{"mc_l_sm_r0c0n0s0p0",						"mc_l_sm_r0c0n0sd0p0_nfwpf"},
			{"mc_l_sm_r0c0n0s0p0_sat",					"mc_l_sm_r0c0n0sd0p0_nfwpf"}, // no sat
			{"mc_l_sm_r0c0q0n0s0",						"mc_l_sm_r0c0q0n0sd0p0_nfwpf"}, // couldn't find
			{"mc_l_sm_r0c0q0n0s0p0",					"mc_l_sm_r0c0q0n0sd0p0_nfwpf"},
			{"mc_l_sm_t0c0",							"mc_l_sm_t0c0_nfwpf"},
			{"mc_l_sm_t0c0_nocast",						"mc_l_sm_t0c0_nfwpf_nocast"},
			{"mc_l_sm_t0c0n0",							"mc_l_sm_t0c0n0_nfwpf_frt_aat"}, // could be wrong
			{"mc_l_sm_t0c0n0s0",						"mc_l_sm_t0c0n0sd0_nfwpf"},
			{"mc_l_sm_t0c0q0n0s0p0",					"mc_l_sm_t0c0n0sd0_nfwpf"}, // couldn't find
			{"mc_l_sm_b0c0",							"mc_l_sm_lmpb_ndw_b0c0_nfwpf_frt_im_aat"},
			{"mc_l_sm_b0c0_sat",						"mc_l_sm_lmpb_ndw_b0c0_nfwpf_frt_im_aat"}, // no sat
			{"mc_l_sm_b0c0_nocast",						"mc_l_sm_lmpb_ndw_b0c0_nfwpf_frt_im_aat"}, // couldn't find
			{"mc_l_sm_b0c0d0p0",						"mc_l_sm_ndw_b0c0d0p0_cltrans"},
			{"mc_l_sm_b0c0s0",							"mc_l_sm_lmpb_ndw_b0c0sd0_nfwpf_frt_aat"},
			{"mc_l_sm_b0c0n0",							"mc_l_sm_lmpb_ndw_b0c0n0_nfwpf_frt_im_aat"},
			{"mc_l_sm_b0c0n0_sat",						"mc_l_sm_lmpb_ndw_b0c0n0_nfwpf_frt_im_aat"}, // no sat
			{"mc_l_sm_b0c0n0s0",						"mc_l_sm_lmpb_ndw_b0c0n0sd0_nfwpf_frt_im_aat"},
			{"mc_l_sm_b0c0n0s0_sat",					"mc_l_sm_lmpb_ndw_b0c0n0sd0_nfwpf_frt_im_aat"}, // no sat
			{"mc_l_sm_b0c0n0p0_sat",					"mc_l_sm_lmpb_ndw_b0c0n0_nfwpf_frt_im_aat"}, // couldn't find
			{"mc_l_sm_b0c0n0s0p0",						"mc_l_sm_lmpb_ndw_b0c0n0sd0_nfwpf_frt_im_aat"}, // couldn't find
			{"mc_l_sm_b0c0n0s0p0_sat",					"mc_l_sm_lmpb_ndw_b0c0n0sd0_nfwpf_frt_im_aat"}, // couldn't find // no sat
			{"mc_l_sm_b0c0p0",							"mc_l_sm_lmpb_ndw_b0c0_nfwpf_frt_im_aat"}, // couldn't find
			{"mc_l_sm_b0c0q0n0s0_sat",					"mc_l_sm_lmpb_ndw_b0c0n0sd0_nfwpf_frt_im_aat"}, // couldn't find
			{"mc_l_sm_b0c0n0s0_custom_growing_ice_cracks", "mc_l_sm_lmpb_ndw_b0c0n0sd0_nfwpf_frt_im_aat"}, // couldn't find
			{"mc_l_sm_b0c0n0s0_custom_growing_ice_cracks_sat", "mc_l_sm_lmpb_ndw_b0c0n0sd0_nfwpf_frt_im_aat"}, // couldn't find
			{"mc_l_sm_flag_t0c0n0s0",					"mc_l_sm_flag_fuv_t0c0n0sd0_nfwpf"},
			{"mc_l_r0c0n0s0",							"mc_l_r0c0n0sd0_nfwpf"},
			{"mc_l_r0c0n0s0_nocast",					"mc_l_r0c0n0sd0_nfwpf"}, // no nocast
			{"mc_l_t0c0n0s0",							"mc_l_t0c0n0sd0_nfwpf"},

			{"mc_unlit_add_lin",						"mc_unlit_add_lin_ct_ndw_cltrans_objective2"},
			{"mc_unlit_add_lin_ua",						"mc_unlit_add_lin_ct_ndw_cltrans_objective2"}, // no ua
			{"mc_unlit_blend_lin",						"mc_unlit_blend_lin_ct_ndw_nfwpf"},
			{"mc_unlit_replace_lin",					"mc_unlit_replace_lin_nfwpf"},
			{"mc_unlit_replace_lin_ua",					"mc_unlit_replace_lin_nfwpf"}, // no ua
			{"mc_unlit_replace_lin_nocast",				"mc_unlit_replace_lin_nfwpf_nocast"},
			{"mc_ambient_t0c0",							"mc_ambient_t0c0_nfwpf"},
			{"mc_ambient_t0c0_nocast",					"mc_ambient_t0c0_nfwpf_nocast"},
			{"mc_shadowcaster_atest",					"mc_shadowcaster_atest"},
			{"mc_reflexsight",							"mc_reflexsight"},
			{"mc_effect_blend_nofog",					"mc_effect_blend_nofog_ndw"},
			//{"mc_effect_falloff_add_nofog",				"2d"}, // coudln't find
			//{"mc_effect_falloff_add_lin_nofog",			"2d"}, // coudln't find
			//{"mc_effect_zfeather_falloff_add_lin_nofog", "2d"}, // coudln't find
			//{"mc_effect_zfeather_falloff_add_lin_nofog_eyeoffset", "2d"}, // coudln't find
			//{"mc_effect_zfeather_falloff_screen_nofog_eyeoffset", "2d"}, // coudln't find

			{"2d",										"2d"},

			{"distortion_scale",						"distortion_scale_legacydst_dat"}, // could be wrong

			{"effect_add_nofog",						"effect_add_nofog_ndw"},
			{"effect_blend",							"effect_blend_ndw"},
			{"effect_blend_nofog",						"effect_blend_nofog_ndw"},
			//{"effect_replace_lin",						"2d"}, // m_effect_replace_ndw
			{"effect_zfeather_add",						"effect_zf_add_ndw"},
			{"effect_zfeather_add_nofog",				"effect_zf_add_nofog_ndw"},
			{"effect_zfeather_blend",					"effect_zf_blend_ndw"},
			{"effect_zfeather_blend_nofog",				"effect_zf_falloff_blend_nofog_ndw"},
			{"effect_zfeather_falloff_add_nofog_eyeoffset", "effect_zf_falloff_add_nofog_eo_ndw"},
			{"effect_zfeather_falloff_blend",			"effect_zf_falloff_blend_ndw"},
			{"effect_zfeather_add_nofog_eyeoffset",		"effect_zf_add_nofog_eo_ndw"},

			{"particle_cloud",							"particle_cloud_atlas_replace_ga"}, // could be wrong
			{"particle_cloud_add",						"particle_cloud_add_ga"},
			{"particle_cloud_outdoor_add",				"particle_cloud_outdoor_add_ga"},
			{"particle_cloud_sparkf_add",				"particle_cloud_sparkf_add_ga"},
			{"particle_cloud_spark_add",				"particle_cloud_spark_add_ga"},

			{"grain_overlay",							"grain_overlay"},

			{"tools_b0c0",								"tools_b0c0ct0"}, // could be wrong
	};

	std::string get_h1_techset(std::string name, std::string matname)
	{
		if (mapped_techsets.find(name) == mapped_techsets.end())
		{
			ZONETOOL_ERROR("Could not find mapped H1 techset for IW5 techset \"%s\" (material: %s)", name.data(), matname.data());
			return "2d";
		}
		return mapped_techsets[name];
	}

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

#define MATERIAL_DUMP_CONST_ARRAY(entry,size) \
	ordered_json carr##entry; \
	for (int i = 0; i < size; i++) \
	{ \
		ordered_json cent##entry; \
		std::string name = asset->constantTable[i].name; \
		name.resize(12); \
		cent##entry["name"] = name.data(); \
		cent##entry["nameHash"] = asset->entry[i].nameHash; \
		nlohmann::json centliteral##entry; \
		centliteral##entry[0] = asset->entry[i].literal[0]; \
		centliteral##entry[1] = asset->entry[i].literal[1]; \
		centliteral##entry[2] = asset->entry[i].literal[2]; \
		centliteral##entry[3] = asset->entry[i].literal[3]; \
		cent##entry["literal"] = centliteral##entry; \
		carr##entry[i] = cent##entry; \
	} \
	matdata[#entry] = carr##entry;
}

namespace ZoneTool
{
	namespace H1
	{
		void dump_material_constant_buffer_def_array(const std::string& name, int count, MaterialConstantBufferDef* def)
		{
			const auto path = "materials\\cbt\\"s + name;
			zonetool::assetmanager::dumper dump;
			if (!dump.open(path))
			{
				return;
			}

			dump.dump_array(def, count);
			for (int i = 0; i < count; i++)
			{
				if (def[i].vsData)
				{
					dump.dump_array(def[i].vsData, def[i].vsDataSize);
				}
				if (def[i].hsData)
				{
					dump.dump_array(def[i].hsData, def[i].hsDataSize);
				}
				if (def[i].dsData)
				{
					dump.dump_array(def[i].dsData, def[i].dsDataSize);
				}
				if (def[i].psData)
				{
					dump.dump_array(def[i].psData, def[i].psDataSize);
				}
				if (def[i].vsOffsetData)
				{
					dump.dump_array(def[i].vsOffsetData, def[i].vsOffsetDataSize);
				}
				if (def[i].hsOffsetData)
				{
					dump.dump_array(def[i].hsOffsetData, def[i].hsOffsetDataSize);
				}
				if (def[i].dsOffsetData)
				{
					dump.dump_array(def[i].dsOffsetData, def[i].dsOffsetDataSize);
				}
				if (def[i].psOffsetData)
				{
					dump.dump_array(def[i].psOffsetData, def[i].psOffsetDataSize);
				}
			}

			dump.close();
		}
	}

	namespace IW5
	{
		bool generate_default_cbt = false;
		H1::MaterialConstantBufferDef* GenerateH1DefaultCbt()
		{
			return nullptr;
		}

		void IMaterial::dump(Material* asset, ZoneMemory* mem)
		{
			if (asset)
			{
				auto c_name = clean_name(asset->name);

				const auto path = "materials\\"s + c_name + ".json"s;
				auto file = zonetool::filesystem::file(path);
				file.open("wb");

				if (asset && asset->techniqueSet)
				{
					//	ITechset::dump_statebits(asset->techniqueSet->name, asset->stateBitsEntry);
					//	ITechset::dump_statebits_map(asset->techniqueSet->name, asset->stateBitsTable, asset->stateBitsCount);
				}

				ordered_json matdata;

				MATERIAL_DUMP_STRING(name);

				if (asset->techniqueSet)
				{
					matdata["techniqueSet->name"] = get_h1_techset(asset->techniqueSet->name, asset->name);
				}

				matdata["gameFlags"] = asset->gameFlags; // convert
				matdata["sortKey"] = asset->sortKey; // convert
				matdata["renderFlags"] = 0; // idk

				matdata["textureAtlasRowCount"] = asset->animationX;
				matdata["textureAtlasColumnCount"] = asset->animationY;
				matdata["textureAtlasFrameBlend"] = 0;
				matdata["textureAtlasAsArray"] = 0;

				matdata["surfaceTypeBits"] = asset->surfaceTypeBits; // convert
				// hashIndex;

				matdata["stateFlags"] = asset->stateFlags; // convert
				matdata["cameraRegion"] = asset->cameraRegion; // convert
				matdata["materialType"] = 0; // idk
				matdata["assetFlags"] = 0; // idk

				MATERIAL_DUMP_CONST_ARRAY(constantTable, asset->constantCount);

				ordered_json material_images;
				for (auto i = 0; i < asset->numMaps; i++)
				{
					ordered_json image;
					if (asset->maps[i].semantic == 11)
					{
						image["image"] = "";
						//image["waterinfo"] = nullptr;
					}
					else
					{
						if (asset->maps[i].image->name)
						{
							image["image"] = asset->maps[i].image->name;
						}
						else
						{
							image["image"] = "";
						}
					}

					image["semantic"] = asset->maps[i].semantic;
					image["samplerState"] = asset->maps[i].sampleState; // convert?
					image["lastCharacter"] = asset->maps[i].secondLastCharacter;
					image["firstCharacter"] = asset->maps[i].firstCharacter;
					image["typeHash"] = asset->maps[i].typeHash;

					// add image data to material
					material_images.push_back(image);
				}
				matdata["textureTable"] = material_images;

				if (generate_default_cbt)
				{
					const auto cbt_name = c_name + ".cbt";
					H1::MaterialConstantBufferDef* generated_default_cbt = GenerateH1DefaultCbt();
					if (generated_default_cbt)
					{
						H1::dump_material_constant_buffer_def_array(cbt_name, 1, generated_default_cbt);
						matdata["constantBufferTable"] = cbt_name;
						matdata["constantBufferCount"] = 1;
					}
				}

				auto str = matdata.dump(4);

				matdata.clear();

				file.write(str);

				file.close();
			}
		}
	}
}