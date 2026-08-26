#include "stdafx.hpp"
#include "../Include.hpp"

#include "ComWorld.hpp"

namespace ZoneTool::IW5
{
	namespace IW7Converter
	{
#define COPY_VALUE_COMWORLD(name) \
		new_lights[i].name = lights[i].name; \

#define COPY_ARR_COMWORLD(name) \
		std::memcpy(&new_lights[i].name, &lights[i].name, sizeof(lights[i].name)); \

		IW7::ComPrimaryLight* convert_primary_lights(ComPrimaryLight* lights, const unsigned int count,
			allocator& allocator)
		{
			const auto new_lights = allocator.allocate<IW7::ComPrimaryLight>(count);

			for (auto i = 0u; i < count; i++)
			{
				new_lights[i].type = static_cast<IW7::GfxLightType>(lights[i].type);
				COPY_VALUE_COMWORLD(canUseShadowMap);
				new_lights[i].needsDynamicShadows = 0;
				COPY_VALUE_COMWORLD(exponent);
				new_lights[i].isVolumetric = 0;
				COPY_ARR_COMWORLD(color);
				COPY_ARR_COMWORLD(dir);
				COPY_ARR_COMWORLD(up);
				COPY_ARR_COMWORLD(origin);
				COPY_VALUE_COMWORLD(radius);
				COPY_VALUE_COMWORLD(cosHalfFovOuter);
				COPY_VALUE_COMWORLD(cosHalfFovInner);
				COPY_VALUE_COMWORLD(rotationLimit);
				COPY_VALUE_COMWORLD(translationLimit);
				new_lights[i].defName = lights[i].defName;

				new_lights[i].bulbLength[0] = 0;
				new_lights[i].bulbLength[1] = 0;
				new_lights[i].bulbLength[2] = 0;
				new_lights[i].bulbRadius = 0;

				new_lights[i].transientZoneList = 0;
				new_lights[i].entityId = 0;
				new_lights[i].uvIntensity = 0.0f;
				new_lights[i].irIntensity = 0.0f;
				new_lights[i].shadowSoftness = 0.0f;
				new_lights[i].shadowBias = 0.0f;
				new_lights[i].distanceFalloff = 0.0f;
			}

			return new_lights;
		}

#undef COPY_VALUE_COMWORLD
#undef COPY_ARR_COMWORLD

		IW7::ComWorld* GenerateIW7ComWorld(ComWorld* asset, allocator& allocator)
		{
			const auto new_asset = allocator.allocate<IW7::ComWorld>();

			REINTERPRET_CAST_SAFE(name);

			new_asset->isInUse = 1;
			new_asset->useForwardPlus = 1;
			new_asset->bakeQuality = 3;

			COPY_VALUE(primaryLightCount);
			new_asset->primaryLights = convert_primary_lights(asset->primaryLights, asset->primaryLightCount, allocator);

			new_asset->firstScriptablePrimaryLight = new_asset->primaryLightCount;

			new_asset->primaryLightEnvCount = new_asset->primaryLightCount + 1;
			new_asset->primaryLightEnvs = allocator.allocate<IW7::ComPrimaryLightEnv>(new_asset->primaryLightEnvCount);

			for (unsigned int i = 1; i < new_asset->primaryLightCount; i++)
			{
				new_asset->primaryLightEnvs[i].numIndices = 1;
				new_asset->primaryLightEnvs[i].primaryLightIndices[0] = i;
			}

			new_asset->primaryLightEnvs[new_asset->primaryLightEnvCount - 1].numIndices = 1;
			new_asset->primaryLightEnvs[new_asset->primaryLightEnvCount - 1].primaryLightIndices[0] = 2047;

			new_asset->changeListInfo.changeListNumber = 1232774;
			new_asset->changeListInfo.time = 1480386331;
			new_asset->changeListInfo.userName = allocator.duplicate_string("manyomi");

			new_asset->numUmbraGates = 0;
			new_asset->umbraGateNames = nullptr;
			for (auto i = 0; i < 4; i++)
			{
				new_asset->umbraGateInitialStates[i] = -1;
			}

			return new_asset;
		}

		IW7::ComWorld* convert(ComWorld* asset, allocator& allocator)
		{
			// generate IW7 comworld
			return GenerateIW7ComWorld(asset, allocator);
		}
	}
}