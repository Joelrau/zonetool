#include "stdafx.hpp"
#include "H1/Assets/ComWorld.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		H1::ComWorld* GenerateH1ComWorld(ComWorld* asset, ZoneMemory* mem)
		{
			// allocate H1 ComWorld structure
			const auto h1_asset = mem->Alloc<H1::ComWorld>();

			h1_asset->name = asset->name;
			h1_asset->isInUse = asset->isInUse;
			h1_asset->primaryLightCount = asset->primaryLightCount;
			h1_asset->primaryLights = mem->Alloc<H1::ComPrimaryLight>(h1_asset->primaryLightCount);
			for (unsigned int i = 0; i < h1_asset->primaryLightCount; i++)
			{
				h1_asset->primaryLights[i].type = static_cast<H1::GfxLightType>(asset->primaryLights[i].type); // don't think there is a need for converting
				h1_asset->primaryLights[i].canUseShadowMap = asset->primaryLights[i].canUseShadowMap;
				h1_asset->primaryLights[i].exponent = asset->primaryLights[i].exponent;
				// unk1
				memcpy(&h1_asset->primaryLights[i].color, &asset->primaryLights[i].color, sizeof(float[3]));
				memcpy(&h1_asset->primaryLights[i].dir, &asset->primaryLights[i].dir, sizeof(float[3]));
				
				// up doesn't exist on IW4, only IW5
				for (auto j = 0; i < 3; i++)
				{
					h1_asset->primaryLights[i].up[j] = 0.0f;
				}

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
				h1_asset->primaryLights[i].defName = asset->primaryLights[i].defName;

				if (h1_asset->primaryLights[i].type == H1::GFX_LIGHT_TYPE_SPOT)
				{
					//h1_asset->primaryLights[i].needsDynamicShadows = 1;

					h1_asset->primaryLights[i].cucScaleVector[0] = 1.0f;
					h1_asset->primaryLights[i].cucScaleVector[1] = 1.0f;
				}
				else if (h1_asset->primaryLights[i].type == H1::GFX_LIGHT_TYPE_OMNI)
				{
					//h1_asset->primaryLights[i].needsDynamicShadows = 1;

					h1_asset->primaryLights[i].cucScaleVector[0] = 1.0f;
					h1_asset->primaryLights[i].cucScaleVector[1] = 1.0f;
				}
			}

			h1_asset->primaryLightEnvCount = h1_asset->primaryLightCount + 1;
			h1_asset->primaryLightEnvs = mem->Alloc<H1::ComPrimaryLightEnv>(h1_asset->primaryLightEnvCount);
			
			for (unsigned int i = 1; i < h1_asset->primaryLightCount; i++)
			{
				if (h1_asset->primaryLights[i].type == H1::GFX_LIGHT_TYPE_SPOT || h1_asset->primaryLights[i].type == H1::GFX_LIGHT_TYPE_OMNI)
				{
					h1_asset->primaryLightEnvs[i].numIndices = 1;
					h1_asset->primaryLightEnvs[i].primaryLightIndices[0] = 1;
				}
				else
				{
					h1_asset->primaryLightEnvs[i].numIndices = 1;
					h1_asset->primaryLightEnvs[i].primaryLightIndices[0] = i;
				}
			}

			h1_asset->primaryLightEnvs[h1_asset->primaryLightEnvCount - 1].numIndices = 1;
			h1_asset->primaryLightEnvs[h1_asset->primaryLightEnvCount - 1].primaryLightIndices[0] = 2047;

			return h1_asset;
		}

		void IComWorld::dump(ComWorld* asset, ZoneMemory* mem)
		{
			// generate h1 comworld
			auto h1_asset = GenerateH1ComWorld(asset, mem);

			// dump h1 comworld
			H1::IComWorld::dump(h1_asset);
		}
	}
}