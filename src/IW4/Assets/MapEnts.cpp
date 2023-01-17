#include "stdafx.hpp"
#include "H1/Assets/MapEnts.hpp"

namespace
{
	namespace mapents2spawns
	{
		constexpr unsigned int MAX_SPAWNS = 0x1000;

		struct SpawnPoint
		{
			const char* name;
			const char* target;
			const char* script_noteworthy;
			const char* unknown;
			float origin[3];
			float angles[3];
		};

		struct SpawnPointList
		{
			int num_spawns;
			SpawnPoint* spawns;
		};

		SpawnPointList* generate_spawnpoint_list(char* entity_string)
		{
			SpawnPointList* list = new SpawnPointList;
			list->num_spawns = 0;
			list->spawns = new SpawnPoint[MAX_SPAWNS];
			memset(list->spawns, 0, sizeof(SpawnPoint) * MAX_SPAWNS);

			char* ents = entity_string;

			bool in_brackets = false;
			bool in_comma = false;
			bool in_comment = false;
			int line = 1;
			int col = 1;

			bool reading_value = false;
			std::string id;
			std::string value;
			std::vector<std::pair<std::string, std::string>> values;

			auto increment = [&]()
			{
				ents++;
				col++;
			};

			auto linebreak = [&]()
			{
				ents++;
				line++;
				col = 1;
			};

			auto add_spawnent = [&]()
			{
				if (list->num_spawns == MAX_SPAWNS)
				{
					throw std::runtime_error("mapents error: too many spawns, increase MAX_SPAWNS");
				}

				list->spawns[list->num_spawns].name = "";
				list->spawns[list->num_spawns].target = "";
				list->spawns[list->num_spawns].script_noteworthy = "";
				list->spawns[list->num_spawns].unknown = "";

				for (auto value : values)
				{
					if (value.first == "classname")
					{
						list->spawns[list->num_spawns].name = _strdup(value.second.data());
					}
					else if (value.first == "target")
					{
						list->spawns[list->num_spawns].target = _strdup(value.second.data());
					}
					else if (value.first == "script_noteworthy")
					{
						list->spawns[list->num_spawns].script_noteworthy = _strdup(value.second.data());
					}
					else if (value.first == "origin")
					{
						std::vector<std::string> tokens;
						std::stringstream check1(value.second);
						std::string intermediate;
						while (getline(check1, intermediate, ' '))
						{
							tokens.push_back(intermediate);
						}
						if (tokens.size() < 3)
						{
							throw std::runtime_error("mapents error: origin got fucked");
						}
						for (auto i = 0; i < 3; i++)
						{
							list->spawns[list->num_spawns].origin[i] = static_cast<float>(atof(tokens[i].data()));
						}
					}
					else if (value.first == "angles")
					{
						std::vector<std::string> tokens;
						std::stringstream check1(value.second);
						std::string intermediate;
						while (getline(check1, intermediate, ' '))
						{
							tokens.push_back(intermediate);
						}
						if (tokens.size() < 3)
						{
							throw std::runtime_error("mapents error: angles got fucked");
						}
						for (auto i = 0; i < 3; i++)
						{
							list->spawns[list->num_spawns].angles[i] = static_cast<float>(atof(tokens[i].data()));
						}
					}
				}

				list->num_spawns++;
			};

			auto check_spawnent = [&]()
			{
				bool valid = false;
				for (auto value : values)
				{
					if (value.first == "classname")
					{
						if (value.second.find("mp_") != std::string::npos && value.second.find("_spawn") != std::string::npos)
						{
							valid = true;
							break;
						}
					}
					if (valid)
					{
						break;
					}
				}
				if (valid)
				{
					add_spawnent();
				}
			};

			while (*ents != '\0')
			{
				if (*ents == '/' && ents[1] == '/')
				{
					in_comment = true;
				}
				if (*ents == '\n')
				{
					if (in_comment)
					{
						in_comment = false;
					}

					linebreak();
					continue;
				}
				if (in_comment)
				{
					increment();
					continue;
				}
				if (*ents == '{')
				{
					if (in_brackets)
					{
						throw std::runtime_error(va("mapents error: %c\nline: %d, col: %d\n%s", *ents, line, col, "bracket start inside another bracket"));
						return nullptr;
					}
					in_brackets = true;
					increment();
					continue;
				}
				if (*ents == '}')
				{
					if (!in_brackets)
					{
						throw std::runtime_error(va("mapents error: %c\nline: %d, col: %d\n%s", *ents, line, col, "unexpected bracket end"));
						return nullptr;
					}

					check_spawnent();
					values.clear();

					in_brackets = false;
					increment();
					continue;
				}
				if (!in_brackets)
				{
					increment();
					continue;
					//throw std::runtime_error(va("mapents error: %c\nline: %d, col: %d\n%s", *ents, line, col, "no brackets found"));
					//return nullptr;
				}
				if (*ents == ' ')
				{
					if (!in_comma)
					{
						increment();
						continue;
					}
				}
				if (*ents == '"')
				{
					if (in_comma)
					{
						if (reading_value)
						{
							values.push_back({ id, value });
							id.clear();
							value.clear();
						}
						reading_value = !reading_value;
						in_comma = false;
						increment();
						continue;
					}
					else
					{
						in_comma = true;
						increment();
						continue;
					}
				}
				if (!in_comma)
				{
					increment();
					continue;
					//throw std::runtime_error(va("mapents error: %c\nline: %d, col: %d\n%s", *ents, line, col, "no comma found"));
					//return nullptr;
				}
				if (!reading_value)
				{
					id.push_back(*ents);
					increment();
					continue;
				}
				else
				{
					value.push_back(*ents);
					increment();
					continue;
				}
			}

			return list;
		}

		void dump_spawnpoint_list(const std::string& output, SpawnPointList* list)
		{
			const auto path = output + ".ents.spawnList"s;
			auto file = filesystem::file(path);
			file.open("wb");

			if (!file.get_fp())
			{
				throw std::runtime_error(va("failed to open file \"%s\" for write", path.data()));
			}

			nlohmann::ordered_json data;

			for (int i = 0; i < list->num_spawns; i++)
			{
				data[i]["name"] = list->spawns[i].name;
				data[i]["target"] = list->spawns[i].target;
				data[i]["script_noteworthy"] = list->spawns[i].script_noteworthy;
				data[i]["unknown"] = list->spawns[i].unknown;
				for (auto j = 0; j < 3; j++)
				{
					data[i]["origin"][j] = list->spawns[i].origin[j];
					data[i]["angles"][j] = list->spawns[i].angles[j];
				}
			}

			const auto json = data.dump(4);
			file.write(json.data(), json.size(), 1);

			file.close();
		}

		void dump_spawns(const std::string& out, char* entity_string)
		{
			auto* spawn_list = mapents2spawns::generate_spawnpoint_list(entity_string);
			if (spawn_list)
			{
				if (spawn_list->num_spawns)
				{
					dump_spawnpoint_list(out, spawn_list);
				}

				delete[] spawn_list->spawns;
				delete spawn_list;
			}
		}
	}
}

namespace ZoneTool
{
	namespace IW5
	{
		H1::MapEnts* GenerateH1MapEnts(MapEnts* asset, ZoneMemory* mem)
		{
			// allocate H1 MapEnts structure
			const auto h1_asset = mem->Alloc<H1::MapEnts>();

			h1_asset->name = asset->name;

			h1_asset->entityString = asset->entityString;
			h1_asset->numEntityChars = asset->numEntityChars;

			h1_asset->trigger.count = asset->trigger.modelCount;
			h1_asset->trigger.models = reinterpret_cast<H1::TriggerModel*>(asset->trigger.models);
			h1_asset->trigger.hullCount = asset->trigger.hullCount;
			h1_asset->trigger.hulls = reinterpret_cast<H1::TriggerHull*>(asset->trigger.hulls);
			h1_asset->trigger.slabCount = asset->trigger.slabCount;
			h1_asset->trigger.slabs = reinterpret_cast<H1::TriggerSlab*>(asset->trigger.slabs);

			h1_asset->clientTrigger.trigger.count = asset->clientTrigger.trigger.modelCount;
			h1_asset->clientTrigger.trigger.models = reinterpret_cast<H1::TriggerModel*>(asset->clientTrigger.trigger.models);
			h1_asset->clientTrigger.trigger.hullCount = asset->clientTrigger.trigger.hullCount;
			h1_asset->clientTrigger.trigger.hulls = reinterpret_cast<H1::TriggerHull*>(asset->clientTrigger.trigger.hulls);
			h1_asset->clientTrigger.trigger.slabCount = asset->clientTrigger.trigger.slabCount;
			h1_asset->clientTrigger.trigger.slabs = reinterpret_cast<H1::TriggerSlab*>(asset->clientTrigger.trigger.slabs);
			h1_asset->clientTrigger.numClientTriggerNodes = asset->clientTrigger.numClientTriggerNodes;
			h1_asset->clientTrigger.clientTriggerAabbTree = reinterpret_cast<H1::ClientTriggerAabbNode*>(asset->clientTrigger.clientTriggerAabbTree);
			h1_asset->clientTrigger.triggerStringLength = asset->clientTrigger.triggerStringLength;
			h1_asset->clientTrigger.triggerString = asset->clientTrigger.triggerString;
			h1_asset->clientTrigger.visionSetTriggers = asset->clientTrigger.visionSetTriggers;
			h1_asset->clientTrigger.blendLookup = mem->Alloc<short>(asset->clientTrigger.trigger.modelCount); // todo?
			h1_asset->clientTrigger.unk1 = mem->Alloc<short>(asset->clientTrigger.trigger.modelCount); // todo?
			h1_asset->clientTrigger.triggerType = mem->Alloc<short>(asset->clientTrigger.numClientTriggerNodes);
			for (auto i = 0; i < asset->clientTrigger.numClientTriggerNodes; i++)
			{
				h1_asset->clientTrigger.triggerType[i] = asset->clientTrigger.triggerType[i]; // convert?
			}
			h1_asset->clientTrigger.origins = reinterpret_cast<float(*__ptr64)[3]>(asset->clientTrigger.origins);
			h1_asset->clientTrigger.scriptDelay = asset->clientTrigger.scriptDelay;
			h1_asset->clientTrigger.audioTriggers = mem->Alloc<short>(asset->clientTrigger.trigger.modelCount);
			h1_asset->clientTrigger.unk2 = mem->Alloc<short>(asset->clientTrigger.trigger.modelCount); // todo?
			h1_asset->clientTrigger.unk3 = mem->Alloc<short>(asset->clientTrigger.trigger.modelCount); // todo?
			h1_asset->clientTrigger.unk4 = mem->Alloc<short>(asset->clientTrigger.trigger.modelCount); // todo?
			h1_asset->clientTrigger.unk5 = mem->Alloc<short>(asset->clientTrigger.trigger.modelCount); // todo?
			h1_asset->clientTrigger.unk6 = mem->Alloc<short>(asset->clientTrigger.trigger.modelCount); // todo?

			h1_asset->clientTriggerBlend.numClientTriggerBlendNodes = 0;
			h1_asset->clientTriggerBlend.blendNodes = nullptr;

			// generate spawnents with mapents2spawns
			h1_asset->spawnList.spawnsCount = 0;
			h1_asset->spawnList.spawns = nullptr;

			h1_asset->splineList.splineCount = 0;
			h1_asset->splineList.splines = nullptr;

			return h1_asset;
		}

		void IMapEnts::dump(MapEnts* asset, ZoneMemory* mem)
		{
			// generate h1 mapents
			auto iw6_asset = GenerateH1MapEnts(asset, mem);

			// dump h1 mapents
			H1::IMapEnts::dump(iw6_asset, SL_ConvertToString);

			// dump spawns
			mapents2spawns::dump_spawns(asset->name, asset->entityString);
		}
	}
}