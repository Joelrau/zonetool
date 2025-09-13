// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"
#include "T6/Structs.hpp"
#include "Techset.hpp"
#include "H1/Assets/Techset.hpp"

namespace ZoneTool
{
	namespace T6
	{
		namespace
		{
			std::string clean_name(const std::string& name)
			{
				std::string new_name;

				for (auto i = 0u; i < name.size(); i++)
				{
					switch (name[i])
					{
					case '(':
						return new_name;
					case '*':
						new_name.push_back('_');
						break;
					default:
						new_name.push_back(name[i]);
					}
				}

				return new_name;
			}
		}

		std::uint8_t get_material_type_from_name(const std::string& name)
		{
			auto prefix_idx = name.find("/");
			if (prefix_idx != std::string::npos && prefix_idx)
			{
				std::string prefix = std::string(name.begin(), name.begin() + prefix_idx);
				if (prefix == "m")
				{
					return H1::MTL_TYPE_MODEL;
				}
				else if (prefix == "me")
				{
					return H1::MTL_TYPE_MODEL_EFFECT;
				}
				else if (prefix == "mc")
				{
					return H1::MTL_TYPE_MODEL_VERTCOL;
				}
				else if (prefix == "w")
				{
					return H1::MTL_TYPE_WORLD;
				}
				else if (prefix == "wc")
				{
					return H1::MTL_TYPE_WORLD_VERTCOL;
				}
				else if (prefix != "gfx")
				{
					ZONETOOL_WARNING("Unknown material type for prefix \"%s\" (material: %s)", prefix.data(), name.data());
				}
			}
			return H1::MTL_TYPE_DEFAULT;
		}

		unsigned char* generate_h1_statebits(unsigned char* bits, allocator& allocator)
		{
			const auto h1_bits = allocator.allocate<unsigned char>(H1::MaterialTechniqueType::TECHNIQUE_COUNT);
			for (auto i = 0; i < T6::TECHNIQUE_COUNT; i++)
			{
				const auto h1_technique = mapped_techniques.find(static_cast<T6::MaterialTechniqueType>(i));
				if (h1_technique != mapped_techniques.end())
				{
					h1_bits[h1_technique->second] = bits[i];
				}
			}

			return h1_bits;
		}

#define MATERIAL_DUMP_STRING(entry) \
	matdata[#entry] = std::string(asset->entry);

#define MATERIAL_DUMP_INT(entry) \
	matdata[#entry] = asset->entry;

#define MATERIAL_INT(entry) \
	mat->entry = matdata[#entry].get<int>();

#define STATEBITENTRYNUM 48

#define MATERIAL_DUMP_BITS_ENTRY(entry,size) \
	nlohmann::json carr##entry; \
	for (int i = 0; i < size; i++) \
	{ \
		carr##entry[i] = asset->entry[i]; \
	} \
	matdata[#entry] = carr##entry;

#define MATERIAL_INT(entry) \
	mat->entry = matdata[#entry].get<int>();

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

#define MATERIAL_DUMP_STATE_MAP(entry,size) \
	nlohmann::json carr##entry; \
	for (int i = 0; i < size; i++) \
	{ \
		nlohmann::json cent##entry; \
		cent##entry[0] = asset->entry[i].loadBits[0]; \
		cent##entry[1] = asset->entry[i].loadBits[1]; \
		carr##entry[i] = cent##entry; \
	} \

#define MATERIAL_DUMP_INFO(entry) \
	matdata[#entry] = asset->info.entry;

		void IMaterial::dump(Material* asset)
		{
			auto c_name = clean_name(asset->name);
			const auto path = "materials\\"s + c_name + ".json"s;
			auto file = zonetool::filesystem::file(path);

			nlohmann::ordered_json matdata;

			MATERIAL_DUMP_STRING(name);

			if (asset->techniqueSet)
			{
				allocator allocator;
				const auto technique = asset->techniqueSet->name + "_t6"s;

				matdata["techniqueSet->name"] = technique;

				H1::Material dummy_mat{};
				dummy_mat.stateFlags = asset->stateFlags;

				const auto h1_state_bits_table = allocator.allocate<H1::GfxStateBits>(asset->stateBitsCount);
				for (auto i = 0; i < asset->stateBitsCount; i++)
				{
					//h1_state_bits_table[i].loadBits[0] = asset->stateBitsTable[i].loadBits[0];
					//h1_state_bits_table[i].loadBits[1] = asset->stateBitsTable[i].loadBits[2];
				}

				H1::ITechset::dump_stateinfo(technique, c_name, &dummy_mat);
				H1::ITechset::dump_statebits(technique, c_name, generate_h1_statebits(asset->stateBitsEntry, allocator));
				H1::ITechset::dump_statebits_map(technique, c_name, h1_state_bits_table, 0);

				unsigned char dummy_buffer_indices[H1::MaterialTechniqueType::TECHNIQUE_COUNT]{};
				H1::MaterialConstantBufferDef dummy_constant_buffer{};

				H1::ITechset::dump_constant_buffer_indexes(technique, c_name, dummy_buffer_indices);
				H1::ITechset::dump_constant_buffer_def_array(technique, c_name, 0, &dummy_constant_buffer);
			}

			MATERIAL_DUMP_INFO(gameFlags);
			MATERIAL_DUMP_INFO(sortKey);
			matdata["renderFlags"] = 0; // idk

			MATERIAL_DUMP_INFO(textureAtlasRowCount);
			MATERIAL_DUMP_INFO(textureAtlasColumnCount);
			matdata["textureAtlasFrameBlend"] = 0;
			matdata["textureAtlasAsArray"] = 0;

			MATERIAL_DUMP_INFO(surfaceFlags);
			MATERIAL_DUMP_INFO(surfaceTypeBits);

			MATERIAL_DUMP_INT(stateFlags);
			MATERIAL_DUMP_INT(cameraRegion);
			matdata["assetFlags"] = H1::MTL_ASSETFLAG_NONE;
			matdata["materialType"] = get_material_type_from_name(c_name);

			MATERIAL_DUMP_CONST_ARRAY(constantTable, asset->constantCount);

			nlohmann::ordered_json material_images;
			for (int i = 0; i < asset->textureCount; i++)
			{
				nlohmann::ordered_json image;

				if (asset->textureTable[i].image)
				{
					image["image"] = asset->textureTable[i].image->name;
				}
				else
				{
					image["image"] = "";
				}
				//}

				image["semantic"] = asset->textureTable[i].semantic;
				image["samplerState"] = asset->textureTable[i].samplerState;
				image["firstCharacter"] = asset->textureTable[i].nameStart;
				image["lastCharacter"] = asset->textureTable[i].nameEnd;
				image["typeHash"] = asset->textureTable[i].nameHash;

				// add image data to material
				material_images[i] = image;
			}
			matdata["textureTable"] = material_images;

			std::string assetstr = matdata.dump(4);

			auto str = matdata.dump(4, ' ', true, nlohmann::detail::error_handler_t::replace);

			try
			{
				matdata.clear();
				file.open("wb");
				file.write(str);
				file.close();
			}
			catch (const std::exception& e)
			{
				printf("%s\n", e.what());
			}
		}
	}
}
