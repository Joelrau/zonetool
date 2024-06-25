#include "stdafx.hpp"
#include "Material.hpp"

#include "IW5/Structs.hpp"

//#include "IW7/Assets/Material.hpp"
#include "X64/Utils/IO/assetmanager.hpp"

#include "IW5/Assets/Material.hpp"

namespace ZoneTool
{
	namespace IW7
	{
		namespace
		{
			std::string get_IW7_techset(std::string name, std::string matname, bool* result, bool effect_vertlit = false)
			{
				auto iw7_techset = get_mapped_techset(name, effect_vertlit);

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
				{0, 36},	// Distortion
							// Opaque water (never used)
							// Boat hull (never used)
				//{3, 0},		// Opaque ambient
				{4, 2},		// Opaque
				//{5, 0},		// Sky
				//{6, 0},		// Skybox sun/moon
				//{7, 0},		// Clouds
				//{8, 0},		// Horizon
				//{9, 0},		// Decal bottom 1
				//{10, 0},	// Decal bottom 2
				//{11, 0},	// Decal bottom 3
				//{12, 0},	// Decal static
				//{13, 0},	// Decal mid 1
				//{14, 0},	// Decal mid 2
				//{15, 0},	// Decal mid 3
				//{24, 0},	// Weapon Impact
				//{29, 0},	// Decal top 1
				//{30, 0},	// Decal top 2
				//{31, 0},	// Decal top 3
				//{32, 0},	// Multiplicative
				//{33, 0},	// Banner/ Curtain
				//{34, 0},	// Hair
				//{35, 0},	// Underwater
				//{36, 0},	// Transparent water
				//{37, 0},	// Corona (wild guess)
				//{38, 0},	// Window inside
				//{39, 0},	// Window outside
				//{40, 0},	// Before effects 1 (wild guess)
				//{41, 0},	// Before effects 2 (wild guess)
				//{42, 0},	// Before effects 3 (extremely wild guess)
				{43, 41},	// Blend / additive => to a decal layer
				{48, 35},	// Effect auto sort!
				//{51, 0},	// FX Top
				//{56, 0},	// AE Bottom
				//{57, 0},	// AE Middle
				//{58, 0},	// AE top
				//{59, 0}		// Viewmodel effect
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
				{IW3::CAMERA_REGION_LIT, 0},
				{IW3::CAMERA_REGION_DECAL, 1},
				{IW3::CAMERA_REGION_EMISSIVE, 4},
				{IW3::CAMERA_REGION_NONE, 4},
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

	namespace IW3::IW7Dumper
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

		void dump(Material* asset, bool geotrail)
		{
			if (asset)
			{
				auto new_name = IW7::replace_material_prefix(asset->name, asset->techniqueSet ? asset->techniqueSet->name : "", geotrail);
				auto c_name = clean_name(new_name);

				const auto path = "materials\\"s + new_name + ".json"s;
				auto file = zonetool::filesystem::file(path);

				ordered_json matdata;

				//matdata["name"] = new_name;

				std::string iw3_techset;
				std::string iw7_techset;
				if (asset->techniqueSet)
				{
					iw3_techset = asset->techniqueSet->name;

					bool result = false;
					iw7_techset = IW7::get_IW7_techset(iw3_techset, asset->name, &result, geotrail);
					if (!result)
					{
						matdata["techniqueSet->original"] = iw3_techset;
						//ZONETOOL_ERROR("Not dumping material \"%s\"", asset->name);
						//return;
					}
					matdata["techniqueSet->name"] = iw7_techset;
				}

				matdata["gameFlags"] = asset->gameFlags; // convert
				matdata["unkFlags"] = 0; // idk
				matdata["sortKey"] = IW7::get_IW7_sortkey(asset->sortKey, asset->name, iw7_techset);
				matdata["renderFlags"] = IW7::get_render_flags_by_techset(iw7_techset); // idk

				matdata["textureAtlasRowCount"] = asset->textureAtlasRowCount;
				matdata["textureAtlasColumnCount"] = asset->textureAtlasColumnCount;
				matdata["textureAtlasFrameBlend"] = 0;
				matdata["textureAtlasAsArray"] = 0;

				matdata["surfaceTypeBits"] = asset->surfaceTypeBits; // convert
				// hashIndex;

				matdata["stateFlags"] = asset->stateFlags; // convert
				matdata["cameraRegion"] = IW7::get_IW7_camera_region(asset->cameraRegion, asset->name, iw7_techset);
				matdata["materialType"] = IW7::get_material_type_from_techset(iw7_techset);
				matdata["assetFlags"] = 0;//IW7::MTL_ASSETFLAG_NONE;

				ordered_json constant_table;
				for (int i = 0; i < asset->constantCount && iw3_techset != "2d"; i++)
				{
					ordered_json table;
					std::string constant_name = asset->constantTable[i].name;

					if (constant_name.size() > 12)
					{
						constant_name.resize(12);
					}

					if (constant_name == "envMapParms")
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

				if (iw7_techset.find("s0") != std::string::npos)
				{
					CONSTANT_TABLE_ADD_IF_NOT_FOUND("reflectionRa", 3344177073u, 8096.0f, 0.0f, 0.0f, 0.0f);
				}
				if (iw7_techset.find("_lin") != std::string::npos)
				{
					CONSTANT_TABLE_ADD_IF_NOT_FOUND("textureAtlas", 1128936273u, 
						static_cast<float>(asset->textureAtlasColumnCount), static_cast<float>(asset->textureAtlasRowCount), 1.0f, 1.0f);
				}

				matdata["constantTable"] = constant_table;

				ordered_json material_images;
				for (auto i = 0; i < asset->numMaps; i++)
				{
					ordered_json image;
					if (asset->maps[i].semantic == 11)
					{
						auto* water = reinterpret_cast<water_t*>(asset->maps[i].image);
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
						if (asset->maps[i].image && asset->maps[i].image->name)
						{
							image["image"] = asset->maps[i].image->name;
						}
						else
						{
							image["image"] = "";
						}
					}

					image["semantic"] = asset->maps[i].semantic = IW7::convert_semantic(asset->maps[i].semantic);
					image["samplerState"] = asset->maps[i].sampleState == 11 ? 19 : asset->maps[i].sampleState; // convert? ( should be fine )
					image["lastCharacter"] = asset->maps[i].secondLastCharacter;
					image["firstCharacter"] = asset->maps[i].firstCharacter;
					image["typeHash"] = asset->maps[i].typeHash;

					// add image data to material
					material_images.push_back(image);
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