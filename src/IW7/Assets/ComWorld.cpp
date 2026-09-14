#include "stdafx.hpp"

#define DUMP_JSON

namespace ZoneTool::IW7
{
#define DUMP_STRING(__field__) \
	static_assert(std::is_same_v<decltype(asset->__field__), const char PTR64>, "Field is not of type const char*"); \
	asset->__field__ ? data[#__field__] = asset->__field__ : data[#__field__] = "";

#define DUMP_FIELD(__field__) \
	data[#__field__] = asset->__field__;

#define DUMP_FIELD_ARR(__field__, __size__) \
	for (auto idx##__field__ = 0u; idx##__field__ < (unsigned int)__size__; idx##__field__++) \
	{ \
		data[#__field__][idx##__field__] = asset->__field__[idx##__field__]; \
	}

	void dump(ComPrimaryLight* asset, ordered_json& data, std::uint32_t index)
	{
		data["primaryLightIndex"] = index;
		DUMP_FIELD(type);
		DUMP_FIELD(canUseShadowMap);
		DUMP_FIELD(needsDynamicShadows);
		DUMP_FIELD(isVolumetric);
		DUMP_FIELD(exponent);
		DUMP_FIELD(transientZoneList);
		DUMP_FIELD(entityId);
		DUMP_FIELD(uvIntensity);
		DUMP_FIELD(irIntensity);
		DUMP_FIELD_ARR(color, 3);
		DUMP_FIELD_ARR(dir, 3);
		DUMP_FIELD_ARR(up, 3);
		DUMP_FIELD_ARR(origin, 3);
		DUMP_FIELD(radius);
		DUMP_FIELD_ARR(fadeOffsetRt, 2);
		DUMP_FIELD(bulbRadius);
		DUMP_FIELD_ARR(bulbLength, 3);
		DUMP_FIELD(cosHalfFovOuter);
		DUMP_FIELD(cosHalfFovInner);
		DUMP_FIELD(shadowSoftness);
		DUMP_FIELD(shadowBias);
		DUMP_FIELD(shadowArea);
		DUMP_FIELD(distanceFalloff);
		DUMP_FIELD(rotationLimit);
		DUMP_FIELD(translationLimit);
		DUMP_STRING(defName);
	}

	void dump(ComPrimaryLightEnv* asset, ordered_json& data, std::uint32_t index)
	{
		data["envIndex"] = index;
		DUMP_FIELD_ARR(primaryLightIndices, 4);
		DUMP_FIELD(numIndices);
	}

	void dump_json(ComWorld* asset)
	{
		const auto path = asset->name + ".commap"s + ".json";
		auto file = filesystem::file(path);
		file.open("wb");

		ordered_json data;

		DUMP_FIELD(isInUse);
		DUMP_FIELD(useForwardPlus);
		DUMP_FIELD(bakeQuality);

		data["primaryLight"] = {};
		for (auto i = 0u; i < asset->primaryLightCount; i++)
		{
			dump(&asset->primaryLights[i], data["primaryLight"][i], i);
		}

		data["primaryLightEnv"] = {};
		for (auto i = 0u; i < asset->primaryLightEnvCount; i++)
		{
			dump(&asset->primaryLightEnvs[i], data["primaryLightEnv"][i], i);
		}

		DUMP_FIELD(scriptablePrimaryLightCount);
		DUMP_FIELD(firstScriptablePrimaryLight);
		DUMP_FIELD(changeListInfo.changeListNumber);
		DUMP_FIELD(changeListInfo.time);
		DUMP_STRING(changeListInfo.userName);

		data["umbraGateNames"] = {};
		for (auto i = 0u; i < asset->numUmbraGates; i++)
		{
			data["umbraGateNames"][i] = asset->umbraGateNames[i];
		}
		DUMP_FIELD_ARR(umbraGateInitialStates, 4);

		auto str = data.dump(4);
		data.clear();
		file.write(str);
		file.close();
	}

	void IComWorld::dump(ComWorld* asset)
	{
#ifdef DUMP_JSON
		dump_json(asset);
		return;
#else
		const auto path = asset->name + ".commap"s;

		assetmanager::dumper write;
		if (!write.open(path))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);

		write.dump_array(asset->primaryLights, asset->primaryLightCount);
		for (unsigned int i = 0; i < asset->primaryLightCount; i++)
		{
			write.dump_string(asset->primaryLights[i].defName);
		}
		write.dump_array(asset->primaryLightEnvs, asset->primaryLightEnvCount);

		write.dump_string(asset->changeListInfo.userName);

		for (unsigned int i = 0; i < asset->numUmbraGates; i++)
		{
			write.dump_string(asset->umbraGateNames[i]);
		}

		write.close();
#endif
	}
}