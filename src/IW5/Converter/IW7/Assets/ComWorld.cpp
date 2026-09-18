#include "stdafx.hpp"
#include "../Include.hpp"

#include "ComWorld.hpp"

namespace ZoneTool::IW5
{
	namespace IW7Converter
	{
		IW7::ComWorld* converter_com_world = nullptr;

#define COPY_VALUE_COMWORLD(name) \
		new_light.name = light.name; \

#define COPY_ARR_COMWORLD(name) \
		std::memcpy(&new_light.name, &light.name, sizeof(light.name)); \

		float PhysicallyBasedLight_IntensityFromCandelas(float candelas)
		{
			return candelas * 1550.0f;
		}

		float PhysicallyBasedLight_FramebufferUnitsFromIntensity(float intensity, float radiometricScale)
		{
			return intensity * radiometricScale;
		}

		void convertColorToPhysicallyBased(float* color)
		{
			float extracted_intensity = std::max({ color[0], color[1], color[2] });
			float normalized_color[3] = {
				color[0] / extracted_intensity,
				color[1] / extracted_intensity,
				color[2] / extracted_intensity
			};

			auto intensity = PhysicallyBasedLight_IntensityFromCandelas(extracted_intensity * 1000.0f);
			auto framebuffer_units = PhysicallyBasedLight_FramebufferUnitsFromIntensity(intensity, 0.001f);

			color[0] = normalized_color[0] * framebuffer_units;
			color[1] = normalized_color[1] * framebuffer_units;
			color[2] = normalized_color[2] * framebuffer_units;
		}

		IW7::ComPrimaryLight* convert_primary_lights(ComPrimaryLight* lights, const unsigned int count,
			allocator& allocator)
		{
			const auto new_lights = allocator.allocate<IW7::ComPrimaryLight>(count);

			for (auto i = 0u; i < count; i++)
			{
				auto& light = lights[i];
				auto& new_light = new_lights[i];

				new_light.type = static_cast<IW7::GfxLightType>(light.type);
				COPY_VALUE_COMWORLD(canUseShadowMap);
				new_light.needsDynamicShadows = 0;
				COPY_VALUE_COMWORLD(exponent);
				new_light.isVolumetric = 0;
				COPY_ARR_COMWORLD(color);
				COPY_ARR_COMWORLD(dir);
				COPY_ARR_COMWORLD(up);
				COPY_ARR_COMWORLD(origin);
				COPY_VALUE_COMWORLD(radius);
				COPY_VALUE_COMWORLD(cosHalfFovOuter);
				COPY_VALUE_COMWORLD(cosHalfFovInner);
				COPY_VALUE_COMWORLD(rotationLimit);
				COPY_VALUE_COMWORLD(translationLimit);
				new_light.defName = light.defName;

				new_light.bulbLength[0] = 0;
				new_light.bulbLength[1] = 0;
				new_light.bulbLength[2] = 0;
				new_light.bulbRadius = 0;

				new_light.transientZoneList = 0;
				new_light.entityId = 0;
				new_light.uvIntensity = 0.0f;
				new_light.irIntensity = 0.0f;
				new_light.shadowSoftness = 0.0f;
				new_light.shadowBias = 0.0f;
				new_light.distanceFalloff = 0.0f;

				if (new_light.type == H1::GFX_LIGHT_TYPE_SPOT)
				{
					new_light.entityId = 739795875;

					new_light.canUseShadowMap = 1;
					new_light.needsDynamicShadows = 1;
					new_light.isVolumetric = 0;

					new_light.bulbRadius = 3.0f;
					new_light.bulbLength[0] = 0.0f;
					new_light.bulbLength[1] = 0.0f;
					new_light.bulbLength[2] = 0.0f;

					new_light.shadowSoftness = 0.55f;
					new_light.shadowBias = 0.4f;
					new_light.shadowArea = 0.00179f;

					new_light.distanceFalloff = 0.2f;

					convertColorToPhysicallyBased(new_light.color);

					if (!new_light.defName)
					{
						new_light.defName = allocator.duplicate_string("light_point_linear");
					}
				}
				else if (new_light.type == H1::GFX_LIGHT_TYPE_OMNI)
				{
					new_light.entityId = 1731316930;

					new_light.canUseShadowMap = 0;
					new_light.needsDynamicShadows = 1;
					new_light.isVolumetric = 0;

					new_light.bulbRadius = 3.0f;
					new_light.bulbLength[0] = 0.0f;
					new_light.bulbLength[1] = 0.0f;
					new_light.bulbLength[2] = 0.0f;

					new_light.shadowSoftness = 0.55f;
					new_light.shadowBias = 0.4f;
					new_light.shadowArea = 0.00179f;

					new_light.distanceFalloff = 0.2f;

					convertColorToPhysicallyBased(new_light.color);

					if (!new_light.defName)
					{
						new_light.defName = allocator.duplicate_string("light_point_linear");
					}
				}
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

			new_asset->primaryLightEnvCount = new_asset->primaryLightCount;
			new_asset->primaryLightEnvs = allocator.allocate<IW7::ComPrimaryLightEnv>(new_asset->primaryLightEnvCount);

			for (unsigned int i = 1; i < new_asset->primaryLightCount; i++)
			{
				new_asset->primaryLightEnvs[i].numIndices = 1;
				new_asset->primaryLightEnvs[i].primaryLightIndices[0] = i;
			}

			new_asset->changeListInfo.changeListNumber = 1232774;
			new_asset->changeListInfo.time = 1480386331;
			new_asset->changeListInfo.userName = allocator.duplicate_string("manyomi");

			new_asset->numUmbraGates = 0;
			new_asset->umbraGateNames = nullptr;
			for (auto i = 0; i < 4; i++)
			{
				new_asset->umbraGateInitialStates[i] = -1;
			}

			converter_com_world = new_asset;
			return new_asset;
		}

		IW7::ComWorld* convert(ComWorld* asset, allocator& allocator)
		{
			// generate IW7 comworld
			return GenerateIW7ComWorld(asset, allocator);
		}
	}
}