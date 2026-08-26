#include "stdafx.hpp"
#include "../Include.hpp"

#include "ComWorld.hpp"

namespace ZoneTool::IW5
{
	namespace H1Converter
	{
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

		H1::ComWorld* GenerateH1ComWorld(ComWorld* asset, allocator& mem)
		{
			// allocate H1 ComWorld structure
			const auto h1_asset = mem.allocate<H1::ComWorld>();

			h1_asset->name = asset->name;
			h1_asset->isInUse = asset->isInUse;
			h1_asset->primaryLightCount = asset->primaryLightCount;
			h1_asset->primaryLights = mem.allocate<H1::ComPrimaryLight>(h1_asset->primaryLightCount);
			for (unsigned int i = 0; i < h1_asset->primaryLightCount; i++)
			{
				h1_asset->primaryLights[i].type = static_cast<H1::GfxLightType>(asset->primaryLights[i].type); // don't think there is a need for converting
				h1_asset->primaryLights[i].canUseShadowMap = asset->primaryLights[i].canUseShadowMap;
				h1_asset->primaryLights[i].exponent = asset->primaryLights[i].exponent;
				// unk1
				memcpy(&h1_asset->primaryLights[i].color, &asset->primaryLights[i].color, sizeof(float[3]));
				memcpy(&h1_asset->primaryLights[i].dir, &asset->primaryLights[i].dir, sizeof(float[3]));
				memcpy(&h1_asset->primaryLights[i].up, &asset->primaryLights[i].up, sizeof(float[3]));
				memcpy(&h1_asset->primaryLights[i].origin, &asset->primaryLights[i].origin, sizeof(float[3]));
				// pad
				h1_asset->primaryLights[i].radius = asset->primaryLights[i].radius;
				h1_asset->primaryLights[i].cosHalfFovOuter = asset->primaryLights[i].cosHalfFovOuter;
				h1_asset->primaryLights[i].cosHalfFovInner = asset->primaryLights[i].cosHalfFovInner;
				h1_asset->primaryLights[i].cosHalfFovExpanded = asset->primaryLights[i].cosHalfFovExpanded;
				h1_asset->primaryLights[i].rotationLimit = asset->primaryLights[i].rotationLimit;
				h1_asset->primaryLights[i].translationLimit = asset->primaryLights[i].translationLimit;
				h1_asset->primaryLights[i].cucRotationOffsetRad = 0;
				h1_asset->primaryLights[i].cucRotationSpeedRad = 0;
				memset(&h1_asset->primaryLights[i].cucScrollVector, 0, sizeof(float[2]));
				memset(&h1_asset->primaryLights[i].cucScaleVector, 0, sizeof(float[2]));
				memset(&h1_asset->primaryLights[i].cucTransVector, 0, sizeof(float[2]));
				h1_asset->primaryLights[i].defName = asset->primaryLights[i].defName && *asset->primaryLights[i].defName ? asset->primaryLights[i].defName : nullptr;

				if (h1_asset->primaryLights[i].type == H1::GFX_LIGHT_TYPE_SPOT)
				{
					h1_asset->primaryLights[i].canUseShadowMap = 1;
					h1_asset->primaryLights[i].physicallyBased = 1;
					h1_asset->primaryLights[i].lightingState = 0;

					h1_asset->primaryLights[i].cucScaleVector[0] = 1.0f;
					h1_asset->primaryLights[i].cucScaleVector[1] = 1.0f;

					h1_asset->primaryLights[i].bulbRadius = 3.0f;
					h1_asset->primaryLights[i].bulbLength[0] = 0.0f;
					h1_asset->primaryLights[i].bulbLength[1] = 0.0f;
					h1_asset->primaryLights[i].bulbLength[2] = 0.0f;
					h1_asset->primaryLights[i].fadeOffset[0] = 0.0f;
					h1_asset->primaryLights[i].fadeOffset[1] = 0.0f;

					convertColorToPhysicallyBased(h1_asset->primaryLights[i].color);

					if (!h1_asset->primaryLights[i].defName)
					{
						h1_asset->primaryLights[i].defName = mem.duplicate_string("light_point_linear");
					}
				}
				else if (h1_asset->primaryLights[i].type == H1::GFX_LIGHT_TYPE_OMNI)
				{
					h1_asset->primaryLights[i].canUseShadowMap = 1;
					h1_asset->primaryLights[i].physicallyBased = 1;
					h1_asset->primaryLights[i].lightingState = 0;

					h1_asset->primaryLights[i].cucScaleVector[0] = 1.0f;
					h1_asset->primaryLights[i].cucScaleVector[1] = 1.0f;

					h1_asset->primaryLights[i].bulbRadius = 3.0f;
					h1_asset->primaryLights[i].bulbLength[0] = 0.0f;
					h1_asset->primaryLights[i].bulbLength[1] = 0.0f;
					h1_asset->primaryLights[i].bulbLength[2] = 0.0f;
					h1_asset->primaryLights[i].fadeOffset[0] = 0.0f;
					h1_asset->primaryLights[i].fadeOffset[1] = 0.0f;

					convertColorToPhysicallyBased(h1_asset->primaryLights[i].color);

					if (!h1_asset->primaryLights[i].defName)
					{
						h1_asset->primaryLights[i].defName = mem.duplicate_string("light_point_linear");
					}
				}
			}

			h1_asset->primaryLightEnvCount = h1_asset->primaryLightCount + 1;
			h1_asset->primaryLightEnvs = mem.allocate<H1::ComPrimaryLightEnv>(h1_asset->primaryLightEnvCount);
			
			for (unsigned int i = 1; i < h1_asset->primaryLightCount; i++)
			{
				h1_asset->primaryLightEnvs[i].numIndices = 1;
				h1_asset->primaryLightEnvs[i].primaryLightIndices[0] = i;
			}

			h1_asset->primaryLightEnvs[h1_asset->primaryLightEnvCount - 1].numIndices = 1;
			h1_asset->primaryLightEnvs[h1_asset->primaryLightEnvCount - 1].primaryLightIndices[0] = 2047;

			return h1_asset;
		}

		H1::ComWorld* convert(ComWorld* asset, allocator& allocator)
		{
			// generate h1 comworld
			return GenerateH1ComWorld(asset, allocator);
		}
	}
}