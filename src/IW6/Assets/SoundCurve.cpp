#include "stdafx.hpp"

namespace ZoneTool::IW6
{
	void ISoundCurve::dump(SndCurve* asset, const std::string& type)
	{
		const auto path = type + "\\"s + asset->name + ".json";
		auto file = filesystem::file(path);
		file.open("wb");

		ordered_json j;

		j["isDefault"] = asset->isDefault;
		j["filename"] = asset->filename;
		j["knots"] = {};
		for (unsigned short i = 0; i < asset->knotCount; i++)
		{
			for (unsigned char e = 0; e < 2; e++)
			{
				j["knots"][i][e] = asset->knots[i][e];
			}
		}

		auto str = j.dump(4);
		j.clear();

		file.write(str);
		file.close();
	}

	void ISoundCurve::dump(SndCurve* asset)
	{
		dump(asset, "sndcurve");
	}
}