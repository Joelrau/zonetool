#include "stdafx.hpp"

namespace ZoneTool::IW7
{
	namespace
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
	}

	void IGfxLightMap::dump(GfxLightMap* asset)
	{
		auto c_name = clean_name(asset->name);

		const auto path = "lightmaps\\"s + c_name + ".json"s;
		auto file = filesystem::file(path);
		file.open("wb");

		ordered_json data;

		for (auto i = 0; i < 3; i++)
		{
			data["textures"][i] = asset->textures[i] ? asset->textures[i]->name : "";
		}

		auto str = data.dump(4);
		data.clear();
		file.write(str);
		file.close();
	}
}