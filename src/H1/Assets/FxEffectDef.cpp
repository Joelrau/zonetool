#include "stdafx.hpp"

#define DUMP_JSON

namespace ZoneTool::H1
{
	namespace fx::json
	{
		const std::string FX_ELEM_TYPE_s[] =
		{
			"FX_ELEM_TYPE_SPRITE_BILLBOARD",
			"FX_ELEM_TYPE_SPRITE_ORIENTED",
			"FX_ELEM_TYPE_SPRITE_ROTATED",
			"FX_ELEM_TYPE_TAIL",
			"FX_ELEM_TYPE_LINE",
			"FX_ELEM_TYPE_TRAIL",
			"FX_ELEM_TYPE_FLARE",
			"FX_ELEM_TYPE_PARTICLE_SIM_ANIMATION",
			"FX_ELEM_TYPE_CLOUD",
			"FX_ELEM_TYPE_SPARK_CLOUD",
			"FX_ELEM_TYPE_SPARK_FOUNTAIN",
			"FX_ELEM_TYPE_MODEL",
			"FX_ELEM_TYPE_OMNI_LIGHT",
			"FX_ELEM_TYPE_SPOT_LIGHT",
			"FX_ELEM_TYPE_SOUND",
			"FX_ELEM_TYPE_DECAL",
			"FX_ELEM_TYPE_RUNNER",
			"FX_ELEM_TYPE_VECTORFIELD",
		};

		const std::string FX_ELEM_LIT_TYPE_s[] =
		{
			"FX_ELEM_LIT_TYPE_NONE",
			"FX_ELEM_LIT_TYPE_LIGHTGRID_SPAWN_SINGLE",
			"FX_ELEM_LIT_TYPE_LIGHTGRID_FRAME_SINGLE",
			"FX_ELEM_LIT_TYPE_LIGHTGRID_FRAME_SPRITE",
			"FX_ELEM_LIT_TYPE_LIGHTGRID_FRAME_VERTEX",
		};

		enum elem_type_e
		{
			elem_type_looping,
			elem_type_oneshot,
			elem_type_emission,
			elem_type_error,
			elem_type_count = elem_type_error,
		};

		const std::string elem_type_s[] =
		{
			"looping",
			"oneshot",
			"emission"
		};

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

#define DUMP_ASSET(__field__) \
		if (asset->__field__) \
		{ \
			data[#__field__] = asset->__field__->name; \
		} \
		else \
		{ \
			data[#__field__] = nullptr; \
		}

#define DUMP_RANGE(__field__) \
		data[#__field__]["base"] = asset->__field__.base; \
		data[#__field__]["amplitude"] = asset->__field__.amplitude;

#define DUMP_RANGE_ARR(__field__, __count__) \
		for(auto idx##__field__ = 0u; idx##__field__ < (unsigned int)__count__; idx##__field__++) \
		{ \
			data[#__field__][idx##__field__]["base"] = asset->__field__[idx##__field__].base; \
			data[#__field__][idx##__field__]["amplitude"] = asset->__field__[idx##__field__].amplitude; \
		}

#define DUMP_ELEM_TYPE() \
		data["elemType"] = FX_ELEM_TYPE_s[asset->elemType];

#define DUMP_ELEM_LIT_TYPE() \
		data["elemLitType"] = FX_ELEM_LIT_TYPE_s[asset->elemLitType];

#define DUMP_BOUNDS(__field__) \
		for(auto i = 0; i < 3; i++) data[#__field__]["midPoint"][i] = asset->__field__.midPoint[i]; \
		for(auto i = 0; i < 3; i++) data[#__field__]["halfSize"][i] = asset->__field__.halfSize[i];

		void dump_visuals(FxElemVisuals* asset, FxElemDef* def, ordered_json& data)
		{
			switch (def->elemType)
			{
			case FX_ELEM_TYPE_MODEL:
				DUMP_ASSET(model);
				break;
			case FX_ELEM_TYPE_RUNNER:
				DUMP_ASSET(effectDef.handle);
				break;
			case FX_ELEM_TYPE_SOUND:
				DUMP_STRING(soundName);
				break;
			case FX_ELEM_TYPE_VECTORFIELD:
				DUMP_STRING(vectorFieldName);
				break;
			case FX_ELEM_TYPE_PARTICLE_SIM_ANIMATION:
				DUMP_ASSET(particleSimAnimation);
				break;
			default:
				if (def->elemType - 12 <= 1u)
				{
					if (def->elemType == FX_ELEM_TYPE_SPOT_LIGHT)
					{
						DUMP_ASSET(lightDef);
					}
				}
				else
				{
					DUMP_ASSET(material);
				}
				break;
			}
		}

		void dump_vec3_range(FxElemVec3Range* asset, ordered_json& data)
		{
			for (auto i = 0; i < 3; i++) data["base"][i] = asset->base[i];
			for (auto i = 0; i < 3; i++) data["amplitude"][i] = asset->amplitude[i];
		}

		void dump_vel_state_sample(FxElemVelStateSample* asset, ordered_json& data)
		{
			dump_vec3_range(&asset->local.velocity, data["local"]["velocity"]);
			dump_vec3_range(&asset->local.totalDelta, data["local"]["totalDelta"]);
			dump_vec3_range(&asset->world.velocity, data["world"]["velocity"]);
			dump_vec3_range(&asset->world.totalDelta, data["world"]["totalDelta"]);
		}

		void dump_visual_state(FxElemVisualState* asset, ordered_json& data)
		{
			DUMP_FIELD_ARR(color, 4);
			DUMP_FIELD_ARR(emissiveScale, 3);
			DUMP_FIELD(rotationDelta);
			DUMP_FIELD(rotationTotal);
			DUMP_FIELD_ARR(size, 2);
			DUMP_FIELD(scale);
			DUMP_FIELD_ARR(pivot, 2);
		}

		void dump_vis_sample_state(FxElemVisStateSample* asset, ordered_json& data)
		{
			dump_visual_state(&asset->base, data["base"]);
			dump_visual_state(&asset->amplitude, data["amplitude"]);
		}

		void dump_trail_vertex(FxTrailVertex* asset, ordered_json& data)
		{
			DUMP_FIELD_ARR(pos, 2);
			DUMP_FIELD_ARR(normal, 2);
			DUMP_FIELD_ARR(texCoord, 2);
			DUMP_FIELD_ARR(radialNormal, 2);
		}

		void dump_trail(FxTrailDef* asset, ordered_json& data)
		{
			DUMP_FIELD(scrollTimeMsec);
			DUMP_FIELD(repeatDist);
			DUMP_FIELD(invSplitDist);
			DUMP_FIELD(invSplitArcDist);
			DUMP_FIELD(invSplitTime);
			DUMP_FIELD(headFadingFactor);
			DUMP_FIELD(tailFadingFactor);

			// vertCount
			for (auto i = 0; i < asset->vertCount; i++)
			{
				dump_trail_vertex(&asset->verts[i], data["verts"][i]);
			}

			DUMP_FIELD_ARR(inds, asset->indCount); // alloc
		}

		void dump_spark_fountain(FxSparkFountainDef* asset, ordered_json& data)
		{
			DUMP_FIELD(gravity);
			DUMP_FIELD(bounceFrac);
			DUMP_FIELD(bounceRand);
			DUMP_FIELD(sparkSpacing);
			DUMP_FIELD(sparkLength);
			DUMP_FIELD(sparkCount);
			DUMP_FIELD(loopTime);
			DUMP_FIELD(velMin);
			DUMP_FIELD(velMax);
			DUMP_FIELD(velConeFrac);
			DUMP_FIELD(restSpeed);
			DUMP_FIELD(boostTime);
			DUMP_FIELD(boostFactor);
		}

		void dump_spot_light(FxSpotLightDef* asset, ordered_json& data)
		{
			DUMP_FIELD(halfFovOuter);
			DUMP_FIELD(halfFovInner);
			DUMP_FIELD(radius);
			DUMP_FIELD(brightness);
			DUMP_FIELD(maxLength);
			DUMP_FIELD(exponent);
			DUMP_FIELD(nearClip);
			DUMP_FIELD(bulbRadius);
			DUMP_FIELD(bulbLength);
			DUMP_FIELD_ARR(fadeOffsetRt, 2);
			DUMP_FIELD(unk1);
			DUMP_FIELD(opl);
			DUMP_FIELD(unk2);
			DUMP_FIELD(unused);
		}

		void dump_omni_light(FxOmniLightDef* asset, ordered_json& data)
		{
			DUMP_FIELD(bulbRadius);
			DUMP_FIELD(bulbLength);
			DUMP_FIELD_ARR(fadeOffsetRt, 2);
		}

		void dump_flare(FxFlareDef* asset, ordered_json& data)
		{
			DUMP_FIELD(position);
			DUMP_FIELD(angularRotCount);
			DUMP_FIELD(flags);
			DUMP_RANGE(depthScaleRange);
			DUMP_RANGE(depthScaleValue);
			DUMP_RANGE(radialRot);
			DUMP_RANGE(radialScaleX);
			DUMP_RANGE(radialScaleY);
			DUMP_FIELD_ARR(dir, 3);
			// intensityXIntervalCount
			// intensityYIntervalCount
			// srcCosIntensityIntervalCount
			// srcCosScaleIntervalCount
			DUMP_FIELD_ARR(intensityX, asset->intensityXIntervalCount); // alloc
			DUMP_FIELD_ARR(intensityY, asset->intensityYIntervalCount); // alloc
			DUMP_FIELD_ARR(srcCosIntensity, asset->srcCosIntensityIntervalCount); // alloc
			DUMP_FIELD_ARR(srcCosScale, asset->srcCosScaleIntervalCount); // alloc
		}

		void dump_elem(FxElemDef* asset, ordered_json& data, const elem_type_e type)
		{
			data["type"] = elem_type_s[type];

			DUMP_FIELD(flags);
			DUMP_FIELD(flags2);

			switch (type)
			{
			case elem_type_looping:
				DUMP_FIELD(spawn.looping.intervalMsec);
				DUMP_FIELD(spawn.looping.count);
				break;
			case elem_type_oneshot:
				DUMP_RANGE(spawn.oneShot.count);
				break;
			}

			DUMP_RANGE(spawnRange);
			DUMP_RANGE(fadeInRange);
			DUMP_RANGE(fadeOutRange);
			DUMP_FIELD(spawnFrustumCullRadius);
			DUMP_RANGE(spawnDelayMsec);
			DUMP_RANGE(lifeSpanMsec);
			DUMP_RANGE_ARR(spawnOrigin, 3);
			DUMP_RANGE(spawnOffsetRadius);
			DUMP_RANGE(spawnOffsetHeight);
			DUMP_RANGE_ARR(spawnAngles, 3);
			DUMP_RANGE_ARR(angularVelocity, 3);
			DUMP_RANGE(initialRotation);
			DUMP_RANGE(gravity);
			DUMP_RANGE(reflectionFactor);
			DUMP_FIELD(atlas.behavior);
			DUMP_FIELD(atlas.index);
			DUMP_FIELD(atlas.fps);
			DUMP_FIELD(atlas.loopCount);
			DUMP_FIELD(atlas.colIndexBits);
			DUMP_FIELD(atlas.rowIndexBits);
			DUMP_FIELD(atlas.entryCount);

			DUMP_ELEM_TYPE();
			DUMP_ELEM_LIT_TYPE();

			// visualCount
			// velIntervalCount;
			// visStateIntervalCount

			if (asset->velSamples)
			{
				for (auto i = 0; i < asset->velIntervalCount + 1; i++)
				{
					dump_vel_state_sample(&asset->velSamples[i], data["velSamples"][i]);
				}
			}
			else
			{
				data["velSamples"] = nullptr;
			}

			if (asset->visSamples)
			{
				for (auto i = 0; i < asset->visStateIntervalCount + 1; i++)
				{
					dump_vis_sample_state(&asset->visSamples[i], data["visSamples"][i]);
				}
			}
			else
			{
				data["visSamples"] = nullptr;
			}

			data["visuals"] = ::json::array();
			if (asset->elemType == FX_ELEM_TYPE_DECAL)
			{
				if (asset->visuals.markArray)
				{
					for (unsigned char a = 0; a < asset->visualCount; a++)
					{
						data["visuals"][a]["materials"][0] = nullptr;
						data["visuals"][a]["materials"][1] = nullptr;
						data["visuals"][a]["materials"][2] = nullptr;

						if (asset->visuals.markArray[a].materials[0])
						{
							data["visuals"][a]["materials"][0] = asset->visuals.markArray[a].materials[0]->name;
						}
						if (asset->visuals.markArray[a].materials[1])
						{
							data["visuals"][a]["materials"][1] = asset->visuals.markArray[a].materials[1]->name;
						}
						if (asset->visuals.markArray[a].materials[2])
						{
							data["visuals"][a]["materials"][2] = asset->visuals.markArray[a].materials[2]->name;
						}
					}
				}
			}
			else if (asset->visualCount > 1)
			{
				if (asset->visuals.array)
				{
					for (unsigned char vis = 0; vis < asset->visualCount; vis++)
					{
						dump_visuals(&asset->visuals.array[vis], asset, data["visuals"][vis]);
					}
				}
			}
			else if (asset->visualCount)
			{
				dump_visuals(&asset->visuals.instance, asset, data["visuals"][0]);
			}

			DUMP_BOUNDS(collBounds);

			// dump reference FX defs
			DUMP_ASSET(effectOnImpact.handle);
			DUMP_ASSET(effectOnDeath.handle);
			DUMP_ASSET(effectEmitted.handle);

			DUMP_RANGE(emitDist);
			DUMP_RANGE(emitDistVariance);

			// dump extended FX data
			data["extended"] = ::json::object();
			if (asset->extended.unknownDef)
			{
				// todo...
				if (asset->elemType == FX_ELEM_TYPE_TRAIL)
				{
					dump_trail(asset->extended.trailDef, data["extended"]["trail"]);
				}
				else if (asset->elemType == FX_ELEM_TYPE_SPARK_FOUNTAIN)
				{
					dump_spark_fountain(asset->extended.sparkFountainDef, data["extended"]["sparkFountain"]);
				}
				else if (asset->elemType == FX_ELEM_TYPE_SPOT_LIGHT)
				{
					dump_spot_light(asset->extended.spotLightDef, data["extended"]["spotLight"]);
				}
				else if (asset->elemType == FX_ELEM_TYPE_OMNI_LIGHT)
				{
					dump_omni_light(asset->extended.omniLightDef, data["extended"]["omniLight"]);
				}
				else if (asset->elemType == FX_ELEM_TYPE_FLARE)
				{
					dump_flare(asset->extended.flareDef, data["extended"]["flare"]);
				}
				else
				{
					__debugbreak();
				}
			}

			DUMP_FIELD(sortOrder);
			DUMP_FIELD(lightingFrac);
			DUMP_FIELD(useItemClip);
			DUMP_FIELD(fadeInfo);
			DUMP_FIELD(fadeOutInfo);
			DUMP_FIELD(randomSeed);
			DUMP_FIELD(emissiveScaleScale);
			DUMP_FIELD(hdrLightingFrac);
			DUMP_FIELD(shadowDensityScale);
			DUMP_FIELD(scatterRatio);
			DUMP_FIELD(volumetricTrailFadeStart);
		}

		void dump(FxEffectDef* asset)
		{
			const auto path = "effects\\"s + asset->name + ".json"s;

			ordered_json data;

			DUMP_STRING(name);
			DUMP_FIELD(flags);
			DUMP_FIELD(totalSize);
			DUMP_FIELD(msecLoopingLife);
			//DUMP_FIELD(elemDefCountLooping);
			//DUMP_FIELD(elemDefCountOneShot);
			//DUMP_FIELD(elemDefCountEmission);
			DUMP_FIELD(elemMaxRadius);
			DUMP_FIELD(occlusionQueryDepthBias);
			DUMP_FIELD(occlusionQueryFadeIn);
			DUMP_FIELD(occlusionQueryFadeOut);
			DUMP_RANGE(occlusionQueryScaleRange);

			for (auto i = 0; i < asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission; i++)
			{
				const auto is_looping = i < asset->elemDefCountLooping;
				const auto is_oneshot = !is_looping && i < asset->elemDefCountLooping + asset->elemDefCountOneShot;
				const auto is_emission = !is_oneshot && i < asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission;

				elem_type_e type = is_looping ? elem_type_looping : is_oneshot ? elem_type_oneshot : is_emission ? elem_type_emission : elem_type_error;
				assert(type != elem_type_error);

				dump_elem(&asset->elemDefs[i], data["elems"][i], type);
			}

			std::string json_str = data.dump(4);
			auto file = filesystem::file(path);
			file.open("wb");
			file.write(json_str.data(), json_str.size(), 1);
			file.close();
		}
	}

	namespace fx::binary
	{
		void dump_visuals(assetmanager::dumper* dump, FxElemDef* def, FxElemVisuals* vis)
		{
			switch (def->elemType)
			{
			case FX_ELEM_TYPE_MODEL:
				dump->dump_asset(vis->model);
				break;
			case FX_ELEM_TYPE_RUNNER:
				dump->dump_asset(vis->effectDef.handle);
				break;
			case FX_ELEM_TYPE_SOUND:
				dump->dump_string(vis->soundName);
				break;
			case FX_ELEM_TYPE_VECTORFIELD:
				dump->dump_string(vis->vectorFieldName);
				break;
			case FX_ELEM_TYPE_PARTICLE_SIM_ANIMATION:
				dump->dump_asset(vis->particleSimAnimation);
				break;
			default:
				if (def->elemType - 12 <= 1u)
				{
					if (def->elemType == FX_ELEM_TYPE_SPOT_LIGHT)
					{
						dump->dump_asset(vis->lightDef);
					}
				}
				else
				{
					dump->dump_asset(vis->material);
				}
				break;
			}
		}

		void dump(FxEffectDef* asset)
		{
			assetmanager::dumper dump;

			const auto path = "effects\\"s + asset->name + ".fxe"s;
			if (!dump.open(path))
			{
				return;
			}

			dump.dump_single(asset);
			dump.dump_string(asset->name);
			dump.dump_array(asset->elemDefs,
				asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission);

			// dump elemDefs
			for (auto i = 0; i < asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission; i++)
			{
				auto def = &asset->elemDefs[i];

				// dump elem samples
				dump.dump_array(def->velSamples, def->velIntervalCount + 1);
				dump.dump_array(def->visSamples, def->visStateIntervalCount + 1);

				// dump visuals
				if (def->elemType == FX_ELEM_TYPE_DECAL)
				{
					if (def->visuals.markArray)
					{
						dump.dump_array(def->visuals.markArray, def->visualCount);

						for (unsigned char a = 0; a < def->visualCount; a++)
						{
							if (def->visuals.markArray[a].materials[0])
							{
								dump.dump_asset(def->visuals.markArray[a].materials[0]);
							}
							if (def->visuals.markArray[a].materials[1])
							{
								dump.dump_asset(def->visuals.markArray[a].materials[1]);
							}
							if (def->visuals.markArray[a].materials[2])
							{
								dump.dump_asset(def->visuals.markArray[a].materials[2]);
							}
						}
					}
				}
				else if (def->visualCount > 1)
				{
					if (def->visuals.markArray)
					{
						dump.dump_array(def->visuals.array, def->visualCount);
						for (unsigned char vis = 0; vis < def->visualCount; vis++)
						{
							dump_visuals(&dump, def, &def->visuals.array[vis]);
						}
					}
				}
				else
				{
					dump_visuals(&dump, def, &def->visuals.instance);
				}

				// dump reference FX defs
				dump.dump_asset(def->effectOnImpact.handle);
				dump.dump_asset(def->effectOnDeath.handle);
				dump.dump_asset(def->effectEmitted.handle);

				// dump extended FX data
				if (def->extended.trailDef)
				{
					if (def->elemType == FX_ELEM_TYPE_TRAIL)
					{
						dump.dump_single(def->extended.trailDef);

						if (def->extended.trailDef->verts)
						{
							dump.dump_array(def->extended.trailDef->verts, def->extended.trailDef->vertCount);
						}

						if (def->extended.trailDef->inds)
						{
							dump.dump_array(def->extended.trailDef->inds, def->extended.trailDef->indCount);
						}
					}
					else if (def->elemType == FX_ELEM_TYPE_SPARK_FOUNTAIN)
					{
						dump.dump_single(def->extended.sparkFountainDef);
					}
					else if (def->elemType == FX_ELEM_TYPE_SPOT_LIGHT)
					{
						dump.dump_single(def->extended.spotLightDef);
					}
					else if (def->elemType == FX_ELEM_TYPE_OMNI_LIGHT)
					{
						dump.dump_single(def->extended.omniLightDef);
					}
					else if (def->elemType == FX_ELEM_TYPE_FLARE)
					{
						dump.dump_single(def->extended.flareDef);

						if (def->extended.flareDef->intensityX)
						{
							dump.dump_array(def->extended.flareDef->intensityX, def->extended.flareDef->intensityXIntervalCount + 1);
						}

						if (def->extended.flareDef->intensityY)
						{
							dump.dump_array(def->extended.flareDef->intensityY, def->extended.flareDef->intensityYIntervalCount + 1);
						}

						if (def->extended.flareDef->srcCosIntensity)
						{
							dump.dump_array(def->extended.flareDef->srcCosIntensity, def->extended.flareDef->srcCosIntensityIntervalCount + 1);
						}

						if (def->extended.flareDef->srcCosScale)
						{
							dump.dump_array(def->extended.flareDef->srcCosScale, def->extended.flareDef->srcCosScaleIntervalCount + 1);
						}
					}
					else
					{
						dump.dump_single(def->extended.unknownDef);
					}
				}
			}

			dump.close();
		}
	}

	void IFxEffectDef::dump(FxEffectDef* asset)
	{
#ifdef DUMP_JSON
		fx::json::dump(asset);
		return;
#else
		fx::binary::dump(asset);
		return;
#endif
	}
}