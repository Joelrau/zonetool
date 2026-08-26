#include "stdafx.hpp"
#include "../Include.hpp"

#include "FxEffectDef.hpp"

namespace ZoneTool::IW5
{
	namespace IW6Converter
	{
		IW6::FxElemType convert_elem_type(IW5::FxElemType type)
		{
			switch (type)
			{
			case IW5::FX_ELEM_TYPE_SPRITE_BILLBOARD:
				return IW6::FX_ELEM_TYPE_SPRITE_BILLBOARD;
				break;
			case IW5::FX_ELEM_TYPE_SPRITE_ORIENTED:
				return IW6::FX_ELEM_TYPE_SPRITE_ORIENTED;
				break;
			case IW5::FX_ELEM_TYPE_TAIL:
				return IW6::FX_ELEM_TYPE_TAIL;
				break;
			case IW5::FX_ELEM_TYPE_TRAIL:
				return IW6::FX_ELEM_TYPE_TRAIL;
				break;
			case IW5::FX_ELEM_TYPE_CLOUD:
				return IW6::FX_ELEM_TYPE_CLOUD;
				break;
			case IW5::FX_ELEM_TYPE_SPARKCLOUD:
				return IW6::FX_ELEM_TYPE_SPARK_CLOUD;
				break;
			case IW5::FX_ELEM_TYPE_SPARKFOUNTAIN:
				return IW6::FX_ELEM_TYPE_SPARK_FOUNTAIN;
				break;
			case IW5::FX_ELEM_TYPE_MODEL:
				return IW6::FX_ELEM_TYPE_MODEL;
				break;
			case IW5::FX_ELEM_TYPE_OMNI_LIGHT:
				return IW6::FX_ELEM_TYPE_OMNI_LIGHT;
				break;
			case IW5::FX_ELEM_TYPE_SPOT_LIGHT:
				return IW6::FX_ELEM_TYPE_SPOT_LIGHT;
				break;
			case IW5::FX_ELEM_TYPE_SOUND:
				return IW6::FX_ELEM_TYPE_SOUND;
				break;
			case IW5::FX_ELEM_TYPE_DECAL:
				return IW6::FX_ELEM_TYPE_DECAL;
				break;
			case IW5::FX_ELEM_TYPE_RUNNER:
				return IW6::FX_ELEM_TYPE_RUNNER;
				break;
			}

			return IW6::FX_ELEM_TYPE_SPRITE_BILLBOARD;
		}

		IW6::FxElemLitType generate_elem_lit_type(IW5::FxElemType type)
		{
			return IW6::FX_ELEM_LIT_TYPE_NONE;
		}

		unsigned int convert_elem_flags(unsigned int flags)
		{
			unsigned int IW6_flags = 0;
			auto convert = [&](unsigned int a, unsigned int b, unsigned int mask = 0)
			{
				IW6_flags |= ((flags & (mask ? mask : a)) == a) ? b : 0;
			};

			convert(IW5::FxElemDefFlags::FX_ELEM_SPAWN_RELATIVE_TO_EFFECT, IW6::FxElemDefFlags::FX_ELEM_SPAWN_RELATIVE_TO_EFFECT);
			convert(IW5::FxElemDefFlags::FX_ELEM_SPAWN_FRUSTUM_CULL, IW6::FxElemDefFlags::FX_ELEM_SPAWN_FRUSTUM_CULL);
			convert(IW5::FxElemDefFlags::FX_ELEM_RUNNER_USES_RAND_ROT, IW6::FxElemDefFlags::FX_ELEM_RUNNER_USES_RAND_ROT);
			convert(IW5::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_NONE, IW6::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_NONE, IW5::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_SPHERE, IW6::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_SPHERE, IW5::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_CYLINDER, IW6::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_CYLINDER, IW5::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_WORLD, IW6::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_WORLD, IW5::FxElemDefFlags::FX_ELEM_RUN_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_SPAWN, IW6::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_SPAWN, IW5::FxElemDefFlags::FX_ELEM_RUN_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_EFFECT, IW6::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_EFFECT, IW5::FxElemDefFlags::FX_ELEM_RUN_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_OFFSET, IW6::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_OFFSET, IW5::FxElemDefFlags::FX_ELEM_RUN_MASK);
			//convert(IW5::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_CAMERA, IW6::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_CAMERA);
			convert(IW5::FxElemDefFlags::FX_ELEM_DIE_ON_TOUCH, IW6::FxElemDefFlags::FX_ELEM_DIE_ON_TOUCH);
			convert(IW5::FxElemDefFlags::FX_ELEM_DRAW_PAST_FOG, IW6::FxElemDefFlags::FX_ELEM_DRAW_PAST_FOG);
			convert(IW5::FxElemDefFlags::FX_ELEM_DRAW_WITH_VIEWMODEL, IW6::FxElemDefFlags::FX_ELEM_DRAW_WITH_VIEWMODEL);
			convert(IW5::FxElemDefFlags::FX_ELEM_BLOCK_SIGHT, IW6::FxElemDefFlags::FX_ELEM_BLOCK_SIGHT);
			convert(IW5::FxElemDefFlags::FX_ELEM_DRAW_IN_THERMAL_VIEW_ONLY, IW6::FxElemDefFlags::FX_ELEM_DRAW_IN_THERMAL_VIEW_ONLY);
			convert(IW5::FxElemDefFlags::FX_ELEM_TRAIL_ORIENT_BY_VELOCITY, IW6::FxElemDefFlags::FX_ELEM_TRAIL_ORIENT_BY_VELOCITY);
			//convert(IW5::FxElemDefFlags::FX_ELEM_EMIT_BOLT, IW6::FxElemDefFlags::FX_ELEM_EMIT_BOLT);
			convert(IW5::FxElemDefFlags::FX_ELEM_EMIT_ORIENT_BY_ELEM, IW6::FxElemDefFlags::FX_ELEM_EMIT_ORIENT_BY_ELEM);
			convert(IW5::FxElemDefFlags::FX_ELEM_USE_OCCLUSION_QUERY, IW6::FxElemDefFlags::FX_ELEM_USE_OCCLUSION_QUERY);
			//convert(IW5::FxElemDefFlags::FX_ELEM_NODRAW_IN_THERMAL_VIEW, IW6::FxElemDefFlags::FX_ELEM_NODRAW_IN_THERMAL_VIEW, IW5::FxElemDefFlags::FX_ELEM_THERMAL_MASK);
			//convert(IW5::FxElemDefFlags::FX_ELEM_SPAWN_IMPACT_FX_WITH_SURFACE_NAME, IW6::FxElemDefFlags::FX_ELEM_SPAWN_IMPACT_FX_WITH_SURFACE_NAME);
			//convert(IW5::FxElemDefFlags::FX_ELEM_RECEIVE_DYNAMIC_LIGHT, IW6::FxElemDefFlags::FX_ELEM_RECEIVE_DYNAMIC_LIGHT);
			//convert(IW5::FxElemDefFlags::FX_ELEM_VOLUMETRIC_TRAIL, IW6::FxElemDefFlags::FX_ELEM_VOLUMETRIC_TRAIL);
			convert(IW5::FxElemDefFlags::FX_ELEM_USE_COLLISION, IW6::FxElemDefFlags::FX_ELEM_USE_COLLISION);
			//convert(IW5::FxElemDefFlags::FX_ELEM_USE_VECTORFIELDS, IW6::FxElemDefFlags::FX_ELEM_USE_VECTORFIELDS);
			//convert(IW5::FxElemDefFlags::FX_ELEM_NO_SURFACE_HDR_SCALAR, IW6::FxElemDefFlags::FX_ELEM_NO_SURFACE_HDR_SCALAR);
			convert(IW5::FxElemDefFlags::FX_ELEM_HAS_VELOCITY_GRAPH_LOCAL, IW6::FxElemDefFlags::FX_ELEM_HAS_VELOCITY_GRAPH_LOCAL);
			convert(IW5::FxElemDefFlags::FX_ELEM_HAS_VELOCITY_GRAPH_WORLD, IW6::FxElemDefFlags::FX_ELEM_HAS_VELOCITY_GRAPH_WORLD);
			convert(IW5::FxElemDefFlags::FX_ELEM_HAS_GRAVITY, IW6::FxElemDefFlags::FX_ELEM_HAS_GRAVITY);
			convert(IW5::FxElemDefFlags::FX_ELEM_USE_MODEL_PHYSICS, IW6::FxElemDefFlags::FX_ELEM_USE_MODEL_PHYSICS);
			convert(IW5::FxElemDefFlags::FX_ELEM_NONUNIFORM_SCALE, IW6::FxElemDefFlags::FX_ELEM_NONUNIFORM_SCALE);
			convert(IW5::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_CUBE, IW6::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_CUBE, IW5::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_SPHERE_LARGE, IW6::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_SPHERE_LARGE, IW5::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_SPHERE_MEDIUM, IW6::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_SPHERE_MEDIUM, IW5::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_SPHERE_SMALL, IW6::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_SPHERE_SMALL, IW5::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_FOUNTAIN_DISABLE_COLLISION, IW6::FxElemDefFlags::FX_ELEM_FOUNTAIN_DISABLE_COLLISION);

			return IW6_flags;
		}

		void GenerateIW6FxElemDef(IW6::FxElemDef* iw6_elem, FxElemDef* elem, allocator& mem)
		{
			iw6_elem->flags = static_cast<IW6::FxElemDefFlags>(convert_elem_flags(elem->flags));
			iw6_elem->flags2 = 0;
			memcpy(&iw6_elem->spawn, &elem->spawn, sizeof(FxSpawnDef));
			memcpy(&iw6_elem->spawnRange, &elem->spawnRange, sizeof(FxFloatRange));
			memcpy(&iw6_elem->fadeInRange, &elem->fadeInRange, sizeof(FxFloatRange));
			memcpy(&iw6_elem->fadeOutRange, &elem->fadeOutRange, sizeof(FxFloatRange));
			iw6_elem->spawnFrustumCullRadius = elem->spawnFrustumCullRadius;
			memcpy(&iw6_elem->spawnDelayMsec, &elem->spawnDelayMsec, sizeof(FxIntRange));
			memcpy(&iw6_elem->lifeSpanMsec, &elem->lifeSpanMsec, sizeof(FxIntRange));
			memcpy(&iw6_elem->spawnOrigin, &elem->spawnOrigin, sizeof(FxFloatRange[3]));
			memcpy(&iw6_elem->spawnOffsetRadius, &elem->spawnOffsetRadius, sizeof(FxFloatRange));
			memcpy(&iw6_elem->spawnOffsetHeight, &elem->spawnOffsetHeight, sizeof(FxFloatRange));
			memcpy(&iw6_elem->spawnAngles, &elem->spawnAngles, sizeof(FxFloatRange[3]));
			memcpy(&iw6_elem->angularVelocity, &elem->angularVelocity, sizeof(FxFloatRange[3]));
			memcpy(&iw6_elem->initialRotation, &elem->initialRotation, sizeof(FxFloatRange));
			memcpy(&iw6_elem->gravity, &elem->gravity, sizeof(FxFloatRange));
			memcpy(&iw6_elem->reflectionFactor, &elem->reflectionFactor, sizeof(FxFloatRange));
			memcpy(&iw6_elem->atlas, &elem->atlas, sizeof(FxElemAtlas));
			iw6_elem->elemType = convert_elem_type(elem->elemType);
			iw6_elem->elemLitType = generate_elem_lit_type(elem->elemType);
			iw6_elem->visualCount = elem->visualCount;
			iw6_elem->velIntervalCount = elem->velIntervalCount;
			iw6_elem->visStateIntervalCount = elem->visStateIntervalCount;

			REINTERPRET_CAST_SAFE_TO_FROM(iw6_elem->velSamples, elem->velSamples);

			if (elem->visSamples)
			{
				iw6_elem->visSamples = mem.allocate<IW6::FxElemVisStateSample>(elem->visStateIntervalCount + 1);
				for (int i = 0; i < elem->visStateIntervalCount + 1; i++)
				{
					// base
					iw6_elem->visSamples[i].base.color[0] = static_cast<int>(elem->visSamples[i].base.color[2]) / 255.0f;
					iw6_elem->visSamples[i].base.color[1] = static_cast<int>(elem->visSamples[i].base.color[1]) / 255.0f;
					iw6_elem->visSamples[i].base.color[2] = static_cast<int>(elem->visSamples[i].base.color[0]) / 255.0f;
					iw6_elem->visSamples[i].base.color[3] = static_cast<int>(elem->visSamples[i].base.color[3]) / 255.0f;
					iw6_elem->visSamples[i].base.rotationDelta = elem->visSamples[i].base.rotationDelta;
					iw6_elem->visSamples[i].base.rotationTotal = elem->visSamples[i].base.rotationTotal;
					memcpy(&iw6_elem->visSamples[i].base.size, &elem->visSamples[i].base.size, sizeof(float[2]));
					iw6_elem->visSamples[i].base.scale = elem->visSamples[i].base.scale;

					// amplitude
					iw6_elem->visSamples[i].amplitude.color[0] = static_cast<int>(elem->visSamples[i].amplitude.color[2]) / 255.0f;
					iw6_elem->visSamples[i].amplitude.color[1] = static_cast<int>(elem->visSamples[i].amplitude.color[1]) / 255.0f;
					iw6_elem->visSamples[i].amplitude.color[2] = static_cast<int>(elem->visSamples[i].amplitude.color[0]) / 255.0f;
					iw6_elem->visSamples[i].amplitude.color[3] = static_cast<int>(elem->visSamples[i].amplitude.color[3]) / 255.0f;
					iw6_elem->visSamples[i].amplitude.rotationDelta = elem->visSamples[i].amplitude.rotationDelta;
					iw6_elem->visSamples[i].amplitude.rotationTotal = elem->visSamples[i].amplitude.rotationTotal;
					memcpy(&iw6_elem->visSamples[i].amplitude.size, &elem->visSamples[i].amplitude.size, sizeof(float[2]));
					iw6_elem->visSamples[i].amplitude.scale = elem->visSamples[i].amplitude.scale;
				}
			}

			if (elem->elemType == FX_ELEM_TYPE_DECAL)
			{
				iw6_elem->visuals.markArray = mem.allocate<IW6::FxElemMarkVisuals>(elem->visualCount);
				for (int i = 0; i < elem->visualCount; i++)
				{
					iw6_elem->visuals.markArray[i].materials[0] = reinterpret_cast<IW6::Material*>(elem->visuals.markArray[i].materials[0]); // mc
					iw6_elem->visuals.markArray[i].materials[1] = reinterpret_cast<IW6::Material*>(elem->visuals.markArray[i].materials[1]); // wc
					iw6_elem->visuals.markArray[i].materials[2] = reinterpret_cast<IW6::Material*>(elem->visuals.markArray[i].materials[1]); // wc displacement
				}
			}
			else if (elem->visualCount > 1)
			{
				iw6_elem->visuals.array = mem.allocate<IW6::FxElemVisuals>(elem->visualCount);
				for (int i = 0; i < elem->visualCount; i++)
				{
					// this is fine
					iw6_elem->visuals.array[i].anonymous = elem->visuals.array[i].anonymous;
				}
			}
			else if (elem->visualCount)
			{
				// this is fine
				iw6_elem->visuals.instance.anonymous = elem->visuals.instance.anonymous;
			}

			memcpy(&iw6_elem->collBounds, &elem->collBounds, sizeof(float[3][2]));

			iw6_elem->effectOnImpact.name = elem->effectOnImpact.name;
			iw6_elem->effectOnDeath.name = elem->effectOnDeath.name;
			iw6_elem->effectEmitted.name = elem->effectEmitted.name;
			memcpy(&iw6_elem->emitDist, &elem->emitDist, sizeof(FxFloatRange));
			memcpy(&iw6_elem->emitDistVariance, &elem->emitDistVariance, sizeof(FxFloatRange));

			switch (elem->elemType)
			{
			case FX_ELEM_TYPE_TRAIL:
				iw6_elem->extended.trailDef = mem.allocate<IW6::FxTrailDef>();
				iw6_elem->extended.trailDef->scrollTimeMsec = elem->extended.trailDef->scrollTimeMsec;
				iw6_elem->extended.trailDef->repeatDist = elem->extended.trailDef->repeatDist;
				iw6_elem->extended.trailDef->invSplitDist = elem->extended.trailDef->invSplitDist;
				iw6_elem->extended.trailDef->invSplitArcDist = elem->extended.trailDef->invSplitArcDist;
				iw6_elem->extended.trailDef->invSplitTime = elem->extended.trailDef->invSplitTime;

				iw6_elem->extended.trailDef->vertCount = elem->extended.trailDef->vertCount;
				iw6_elem->extended.trailDef->verts = mem.allocate<IW6::FxTrailVertex>(iw6_elem->extended.trailDef->vertCount);
				for (int i = 0; i < iw6_elem->extended.trailDef->vertCount; i++)
				{
					memcpy(&iw6_elem->extended.trailDef->verts[i].pos, &elem->extended.trailDef->verts[i].pos, sizeof(float[2]));
					memcpy(&iw6_elem->extended.trailDef->verts[i].normal, &elem->extended.trailDef->verts[i].normal, sizeof(float[2]));
					memcpy(&iw6_elem->extended.trailDef->verts[i].texCoord, &elem->extended.trailDef->verts[i].texCoord, sizeof(float[2]));
				}

				iw6_elem->extended.trailDef->indCount = elem->extended.trailDef->indCount;
				REINTERPRET_CAST_SAFE_TO_FROM(iw6_elem->extended.trailDef->inds, elem->extended.trailDef->inds);
				break;
			case FX_ELEM_TYPE_SPARKFOUNTAIN:
				REINTERPRET_CAST_SAFE_TO_FROM(iw6_elem->extended.sparkFountainDef, elem->extended.sparkFountainDef);
				break;
			case FX_ELEM_TYPE_SPOT_LIGHT:
				iw6_elem->extended.spotLightDef = mem.allocate<IW6::FxSpotLightDef>();
				if (elem->extended.spotLightDef)
				{
					iw6_elem->extended.spotLightDef->fovInnerFraction = elem->extended.spotLightDef->fovInnerFraction;
					iw6_elem->extended.spotLightDef->startRadius = elem->extended.spotLightDef->startRadius;
					iw6_elem->extended.spotLightDef->endRadius = elem->extended.spotLightDef->endRadius;
					iw6_elem->extended.spotLightDef->brightness = elem->extended.spotLightDef->brightness;
					iw6_elem->extended.spotLightDef->maxLength = elem->extended.spotLightDef->maxLength;
					iw6_elem->extended.spotLightDef->exponent = elem->extended.spotLightDef->exponent;
				}
				break;
			default:
				iw6_elem->extended.unknownDef = elem->extended.unknownDef;
				break;
			}

			// check:
			iw6_elem->sortOrder = elem->sortOrder;
			iw6_elem->lightingFrac = elem->lightingFrac;
			iw6_elem->useItemClip = elem->useItemClip;
			iw6_elem->fadeInfo = elem->fadeInfo;
			iw6_elem->randomSeed = elem->randomSeed;
		}

		IW6::FxEffectDef* GenerateIW6FxEffectDef(FxEffectDef* asset, allocator& mem)
		{
			auto* iw6_asset = mem.allocate<IW6::FxEffectDef>();
			iw6_asset->name = asset->name;

			iw6_asset->flags = asset->flags; // convert? should be the same...
			iw6_asset->totalSize = asset->totalSize;
			iw6_asset->msecLoopingLife = asset->msecLoopingLife;
			iw6_asset->elemDefCountLooping = asset->elemDefCountLooping;
			iw6_asset->elemDefCountOneShot = asset->elemDefCountOneShot;
			iw6_asset->elemDefCountEmission = asset->elemDefCountEmission;
			iw6_asset->elemMaxRadius = 0;
			iw6_asset->occlusionQueryDepthBias = asset->occlusionQueryDepthBias;
			iw6_asset->occlusionQueryFadeIn = asset->occlusionQueryFadeIn;
			iw6_asset->occlusionQueryFadeOut = asset->occlusionQueryFadeOut;
			iw6_asset->occlusionQueryScaleRange.base = asset->occlusionQueryScaleRange.base;
			iw6_asset->occlusionQueryScaleRange.amplitude = asset->occlusionQueryScaleRange.amplitude;

			iw6_asset->elemDefs = mem.allocate<IW6::FxElemDef>(asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission);
			for (auto i = 0; i < asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission; i++)
			{
				GenerateIW6FxElemDef(&iw6_asset->elemDefs[i], &asset->elemDefs[i], mem);
			}

			return iw6_asset;
		}

		IW6::FxEffectDef* convert(FxEffectDef* asset, allocator& allocator)
		{
			// generate IW6 fx
			return GenerateIW6FxEffectDef(asset, allocator);
		}
	}
}