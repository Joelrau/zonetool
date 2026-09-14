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
					// Not volumetric. A volumetric light is raymarched as participating media
					// against the depth buffer, so it never consults a surface normal - which is
					// why marking one lit a wall whose normal points away from it, and why none
					// of the light-list, proxy-hull or probe culling changed it at all.
					//
					// Stock does set this, but stock also ships the volumetric data to go with
					// it; GfxWorld here writes volumetrics.volumetricCount = 0, so claiming a
					// light is volumetric aims that pass at nothing we ever wrote.
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
					// ---- OMNI -> wide SPOT -------------------------------------------------
					//
					// IW7 has no omni shadow path at all. Grepping the IW8 decompilation gives
					// 1221 hits for SpotShadow and zero for OmniShadow, CubeShadow or PointShadow;
					// the engine then forces the flag off itself
					// (`scene.dynamicOmniLight[i].lightCommon.canUseShadowMap = 0`), and the zone
					// load asserts in r_bsp_load_obj.cpp that anything claiming a shadow map has a
					// real cone. So an omni lights everything inside its radius, through walls,
					// always - confirmed in game, where the only thing that stopped mp_test_h1's
					// omni was shrinking its radius below the distance to the wall.
					//
					// Shipped maps agree: IW7 uses spots for anything that has to respect geometry
					// (mp_frontend 34 spots / 0 omnis, mp_paris 65 / 0, mp_breakneck 49 / 1), and
					// cp_zmb's 142 omnis are small unshadowed fill lights that nearly all carry
					// canUseShadowMap = 0 because the flag does nothing for them.
					//
					// So convert. A wide cone keeps most of an omni's spread while gaining the
					// shadow path. 70 degrees stays inside the 80 degree cutoff in GfxWorld.cpp,
					// past which the frustum proxy falls back to a sphere hull.
					constexpr auto omni_as_spot = true;
					// 70 degrees. Narrowing to 55 (the median of mp_paris's shadowed spots) was
					// tried to sharpen distant shadows and quaK preferred the wider cone, so it
					// stays at 70. Note this is wider than any shadowed spot in the shipped maps
					// (frontend 15-45, paris 40-65) and it must stay under the 80 degree cutoff in
					// GfxWorld.cpp, past which the frustum proxy falls back to a sphere hull.
					constexpr auto omni_spot_cos_outer = 0.342020f; // 70 degrees
					constexpr auto omni_spot_cos_inner = 0.573576f; // 55 degrees

					if (omni_as_spot)
					{
						new_light.type = static_cast<IW7::GfxLightType>(2); // SPOT

						// dir points toward the light, so the cone runs along -dir. IW3 omnis do
						// carry a real direction (mp_test_h1's is 0.30, -0.90, 0.30), so use it and
						// only fall back to straight down when there is nothing to use.
						const auto len = std::sqrt((new_light.dir[0] * new_light.dir[0])
							+ (new_light.dir[1] * new_light.dir[1])
							+ (new_light.dir[2] * new_light.dir[2]));
						if (len > 1e-4f)
						{
							for (auto k = 0; k < 3; k++)
							{
								new_light.dir[k] /= len;
							}
						}
						else
						{
							new_light.dir[0] = 0.0f;
							new_light.dir[1] = 0.0f;
							new_light.dir[2] = 1.0f; // shine downward
						}

						// Strictly inside (0, 1) or the zone load assert trips.
						new_light.cosHalfFovOuter = omni_spot_cos_outer;
						new_light.cosHalfFovInner = omni_spot_cos_inner;
					}

					// Copied from the one omni stock IW7 ships (mp_breakneck light 51), the same
					// way the spot branch above copies its id. What entityId means is not known -
					// it is not an index into anything this converter writes - but that omni is
					// otherwise field-for-field what we emit (type, all three shadow flags, and an
					// empty lightViewFrustum alike), and a zero here is the one structural
					// difference left between them.
					new_light.entityId = 1731316930;

					// Radius is the only thing that bounds this light, measured 2026-09-10.
					//
					// mp_test_h1's omni sits 56 units from a wall with a radius of 160 and lights
					// straight through it onto whatever is on the far side. Dropping the radius to
					// 30 stops that completely; 60 does not, and 60 is the value that still reaches
					// the wall face. So the light is culled by its sphere and by nothing else:
					// clipping its frustumLights hull to exclude both the camera and the wall face
					// changed nothing, nor did the voxel light lists, the probe search or
					// primaryLightEnvIndex. With useForwardPlus = 1 the clustered binning evidently
					// uses origin + radius, and the proxy hull only supplies the z-binning extent.
					//
					// IW3 solved this per surface: every surface and light grid cell stores the one
					// primary light that reaches it, computed with visibility. IW7 has no equivalent
					// field, so a converted light needs a shadow map to be stopped by geometry -
					// which is the open problem. Do not shrink the radius to hide it: at 30 the light
					// no longer resembles what the map author placed.

					new_light.canUseShadowMap = 1;
					new_light.needsDynamicShadows = 1;
					// Not volumetric. A volumetric light is raymarched as participating media
					// against the depth buffer, so it never consults a surface normal - which is
					// why marking one lit a wall whose normal points away from it, and why none
					// of the light-list, proxy-hull or probe culling changed it at all.
					//
					// Stock does set this, but stock also ships the volumetric data to go with
					// it; GfxWorld here writes volumetrics.volumetricCount = 0, so claiming a
					// light is volumetric aims that pass at nothing we ever wrote.
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

			// One env per light, identity mapped, with env 0 left EMPTY on purpose.
			//
			// Verified against the primaryLightEnvs table of all nine dumped IW7 ComWorlds
			// (mp_paris, mp_afghan, mp_breakneck, cp_zmb, mp_dome_dusk, mp_frontend, and the
			// three ports): every one of them has primaryLightEnvCount == primaryLightCount,
			// light 0 of type NONE, env 0 with numIndices == 0, and env[i] = {i} for i >= 1.
			// There is no trailing sentinel env in any of them.
			//
			// So env 0 is the engine's "no primary light environment", and since every static
			// model in every stock map carries primaryLightEnvIndex == 0, IW7 static models are
			// simply not lit through this path. Filling env 0 would make us the only map that
			// does - do not "fix" it.
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