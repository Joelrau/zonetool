#include "stdafx.hpp"
#include "../Include.hpp"

#include "FxEffectDef.hpp"

namespace ZoneTool::IW5
{
	namespace S1Converter
	{
		S1::FxElemType convert_elem_type(IW5::FxElemType type)
		{
			switch (type)
			{
			case IW5::FX_ELEM_TYPE_SPRITE_BILLBOARD:
				return S1::FX_ELEM_TYPE_SPRITE_BILLBOARD;
				break;
			case IW5::FX_ELEM_TYPE_SPRITE_ORIENTED:
				return S1::FX_ELEM_TYPE_SPRITE_ORIENTED;
				break;
			case IW5::FX_ELEM_TYPE_TAIL:
				return S1::FX_ELEM_TYPE_TAIL;
				break;
			case IW5::FX_ELEM_TYPE_TRAIL:
				return S1::FX_ELEM_TYPE_TRAIL;
				break;
			case IW5::FX_ELEM_TYPE_CLOUD:
				return S1::FX_ELEM_TYPE_CLOUD;
				break;
			case IW5::FX_ELEM_TYPE_SPARKCLOUD:
				return S1::FX_ELEM_TYPE_SPARK_CLOUD;
				break;
			case IW5::FX_ELEM_TYPE_SPARKFOUNTAIN:
				return S1::FX_ELEM_TYPE_SPARK_FOUNTAIN;
				break;
			case IW5::FX_ELEM_TYPE_MODEL:
				return S1::FX_ELEM_TYPE_MODEL;
				break;
			case IW5::FX_ELEM_TYPE_OMNI_LIGHT:
				return S1::FX_ELEM_TYPE_OMNI_LIGHT;
				break;
			case IW5::FX_ELEM_TYPE_SPOT_LIGHT:
				return S1::FX_ELEM_TYPE_SPOT_LIGHT;
				break;
			case IW5::FX_ELEM_TYPE_SOUND:
				return S1::FX_ELEM_TYPE_SOUND;
				break;
			case IW5::FX_ELEM_TYPE_DECAL:
				return S1::FX_ELEM_TYPE_DECAL;
				break;
			case IW5::FX_ELEM_TYPE_RUNNER:
				return S1::FX_ELEM_TYPE_RUNNER;
				break;
			}

			return S1::FX_ELEM_TYPE_SPRITE_BILLBOARD;
		}

		S1::FxElemLitType generate_elem_lit_type(IW5::FxElemType type)
		{
			switch (type)
			{
			case IW5::FX_ELEM_TYPE_SPRITE_BILLBOARD:
				return S1::FX_ELEM_LIT_TYPE_LIGHTGRID_FRAME_SPRITE;
				break;
			case IW5::FX_ELEM_TYPE_SPRITE_ORIENTED:
				return S1::FX_ELEM_LIT_TYPE_LIGHTGRID_SPAWN_SINGLE;
				break;
			case IW5::FX_ELEM_TYPE_TAIL:
				return S1::FX_ELEM_LIT_TYPE_LIGHTGRID_FRAME_SPRITE;
				break;
			case IW5::FX_ELEM_TYPE_TRAIL:
				return S1::FX_ELEM_LIT_TYPE_LIGHTGRID_FRAME_VERTEX;
				break;
			case IW5::FX_ELEM_TYPE_CLOUD:
				return S1::FX_ELEM_LIT_TYPE_LIGHTGRID_SPAWN_SINGLE;
				break;
			case IW5::FX_ELEM_TYPE_SPARKCLOUD:
				return S1::FX_ELEM_LIT_TYPE_LIGHTGRID_SPAWN_SINGLE;
				break;
			case IW5::FX_ELEM_TYPE_SPARKFOUNTAIN:
				return S1::FX_ELEM_LIT_TYPE_LIGHTGRID_FRAME_SPRITE;
				break;
			case IW5::FX_ELEM_TYPE_MODEL:
				return S1::FX_ELEM_LIT_TYPE_NONE;
				break;
			case IW5::FX_ELEM_TYPE_OMNI_LIGHT:
				return S1::FX_ELEM_LIT_TYPE_LIGHTGRID_SPAWN_SINGLE;
				break;
			case IW5::FX_ELEM_TYPE_SPOT_LIGHT:
				return S1::FX_ELEM_LIT_TYPE_NONE;
				break;
			case IW5::FX_ELEM_TYPE_SOUND:
				return S1::FX_ELEM_LIT_TYPE_LIGHTGRID_SPAWN_SINGLE;
				break;
			case IW5::FX_ELEM_TYPE_DECAL:
				return S1::FX_ELEM_LIT_TYPE_NONE;
				break;
			case IW5::FX_ELEM_TYPE_RUNNER:
				return S1::FX_ELEM_LIT_TYPE_LIGHTGRID_SPAWN_SINGLE;
				break;
			}

			return S1::FX_ELEM_LIT_TYPE_NONE;
		}

		unsigned int convert_elem_flags(unsigned int flags)
		{
			unsigned int S1_flags = 0;
			auto convert = [&](unsigned int a, unsigned int b, unsigned int mask = 0)
			{
				S1_flags |= ((flags & (mask ? mask : a)) == a) ? b : 0;
			};

			convert(IW5::FxElemDefFlags::FX_ELEM_SPAWN_RELATIVE_TO_EFFECT, S1::FxElemDefFlags::FX_ELEM_SPAWN_RELATIVE_TO_EFFECT);
			convert(IW5::FxElemDefFlags::FX_ELEM_SPAWN_FRUSTUM_CULL, S1::FxElemDefFlags::FX_ELEM_SPAWN_FRUSTUM_CULL);
			convert(IW5::FxElemDefFlags::FX_ELEM_RUNNER_USES_RAND_ROT, S1::FxElemDefFlags::FX_ELEM_RUNNER_USES_RAND_ROT);
			convert(IW5::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_NONE, S1::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_NONE, IW5::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_SPHERE, S1::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_SPHERE, IW5::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_CYLINDER, S1::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_CYLINDER, IW5::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_WORLD, S1::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_WORLD, IW5::FxElemDefFlags::FX_ELEM_RUN_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_SPAWN, S1::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_SPAWN, IW5::FxElemDefFlags::FX_ELEM_RUN_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_EFFECT, S1::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_EFFECT, IW5::FxElemDefFlags::FX_ELEM_RUN_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_OFFSET, S1::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_OFFSET, IW5::FxElemDefFlags::FX_ELEM_RUN_MASK);
			//convert(IW5::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_CAMERA, S1::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_CAMERA);
			convert(IW5::FxElemDefFlags::FX_ELEM_DIE_ON_TOUCH, S1::FxElemDefFlags::FX_ELEM_DIE_ON_TOUCH);
			convert(IW5::FxElemDefFlags::FX_ELEM_DRAW_PAST_FOG, S1::FxElemDefFlags::FX_ELEM_DRAW_PAST_FOG);
			convert(IW5::FxElemDefFlags::FX_ELEM_DRAW_WITH_VIEWMODEL, S1::FxElemDefFlags::FX_ELEM_DRAW_WITH_VIEWMODEL);
			convert(IW5::FxElemDefFlags::FX_ELEM_BLOCK_SIGHT, S1::FxElemDefFlags::FX_ELEM_BLOCK_SIGHT);
			convert(IW5::FxElemDefFlags::FX_ELEM_DRAW_IN_THERMAL_VIEW_ONLY, S1::FxElemDefFlags::FX_ELEM_DRAW_IN_THERMAL_VIEW_ONLY);
			convert(IW5::FxElemDefFlags::FX_ELEM_TRAIL_ORIENT_BY_VELOCITY, S1::FxElemDefFlags::FX_ELEM_TRAIL_ORIENT_BY_VELOCITY);
			//convert(IW5::FxElemDefFlags::FX_ELEM_EMIT_BOLT, S1::FxElemDefFlags::FX_ELEM_EMIT_BOLT);
			convert(IW5::FxElemDefFlags::FX_ELEM_EMIT_ORIENT_BY_ELEM, S1::FxElemDefFlags::FX_ELEM_EMIT_ORIENT_BY_ELEM);
			convert(IW5::FxElemDefFlags::FX_ELEM_USE_OCCLUSION_QUERY, S1::FxElemDefFlags::FX_ELEM_USE_OCCLUSION_QUERY);
			//convert(IW5::FxElemDefFlags::FX_ELEM_NODRAW_IN_THERMAL_VIEW, S1::FxElemDefFlags::FX_ELEM_NODRAW_IN_THERMAL_VIEW, IW5::FxElemDefFlags::FX_ELEM_THERMAL_MASK);
			//convert(IW5::FxElemDefFlags::FX_ELEM_SPAWN_IMPACT_FX_WITH_SURFACE_NAME, S1::FxElemDefFlags::FX_ELEM_SPAWN_IMPACT_FX_WITH_SURFACE_NAME);
			//convert(IW5::FxElemDefFlags::FX_ELEM_RECEIVE_DYNAMIC_LIGHT, S1::FxElemDefFlags::FX_ELEM_RECEIVE_DYNAMIC_LIGHT);
			//convert(IW5::FxElemDefFlags::FX_ELEM_VOLUMETRIC_TRAIL, S1::FxElemDefFlags::FX_ELEM_VOLUMETRIC_TRAIL);
			convert(IW5::FxElemDefFlags::FX_ELEM_USE_COLLISION, S1::FxElemDefFlags::FX_ELEM_USE_COLLISION);
			//convert(IW5::FxElemDefFlags::FX_ELEM_USE_VECTORFIELDS, S1::FxElemDefFlags::FX_ELEM_USE_VECTORFIELDS);
			//convert(IW5::FxElemDefFlags::FX_ELEM_NO_SURFACE_HDR_SCALAR, S1::FxElemDefFlags::FX_ELEM_NO_SURFACE_HDR_SCALAR);
			convert(IW5::FxElemDefFlags::FX_ELEM_HAS_VELOCITY_GRAPH_LOCAL, S1::FxElemDefFlags::FX_ELEM_HAS_VELOCITY_GRAPH_LOCAL);
			convert(IW5::FxElemDefFlags::FX_ELEM_HAS_VELOCITY_GRAPH_WORLD, S1::FxElemDefFlags::FX_ELEM_HAS_VELOCITY_GRAPH_WORLD);
			convert(IW5::FxElemDefFlags::FX_ELEM_HAS_GRAVITY, S1::FxElemDefFlags::FX_ELEM_HAS_GRAVITY);
			convert(IW5::FxElemDefFlags::FX_ELEM_USE_MODEL_PHYSICS, S1::FxElemDefFlags::FX_ELEM_USE_MODEL_PHYSICS);
			convert(IW5::FxElemDefFlags::FX_ELEM_NONUNIFORM_SCALE, S1::FxElemDefFlags::FX_ELEM_NONUNIFORM_SCALE);
			convert(IW5::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_CUBE, S1::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_CUBE, IW5::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_SPHERE_LARGE, S1::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_SPHERE_LARGE, IW5::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_SPHERE_MEDIUM, S1::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_SPHERE_MEDIUM, IW5::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_SPHERE_SMALL, S1::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_SPHERE_SMALL, IW5::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_FOUNTAIN_DISABLE_COLLISION, S1::FxElemDefFlags::FX_ELEM_FOUNTAIN_DISABLE_COLLISION);

			return S1_flags;
		}

		void GenerateS1FxElemDef(S1::FxElemDef* S1_elem, FxElemDef* elem, allocator& mem)
		{
			S1_elem->flags = static_cast<S1::FxElemDefFlags>(convert_elem_flags(elem->flags));
			S1_elem->flags2 = 0;
			memcpy(&S1_elem->spawn, &elem->spawn, sizeof(FxSpawnDef));
			memcpy(&S1_elem->spawnRange, &elem->spawnRange, sizeof(FxFloatRange));
			memcpy(&S1_elem->fadeInRange, &elem->fadeInRange, sizeof(FxFloatRange));
			memcpy(&S1_elem->fadeOutRange, &elem->fadeOutRange, sizeof(FxFloatRange));
			S1_elem->spawnFrustumCullRadius = elem->spawnFrustumCullRadius;
			memcpy(&S1_elem->spawnDelayMsec, &elem->spawnDelayMsec, sizeof(FxIntRange));
			memcpy(&S1_elem->lifeSpanMsec, &elem->lifeSpanMsec, sizeof(FxIntRange));
			memcpy(&S1_elem->spawnOrigin, &elem->spawnOrigin, sizeof(FxFloatRange[3]));
			memcpy(&S1_elem->spawnOffsetRadius, &elem->spawnOffsetRadius, sizeof(FxFloatRange));
			memcpy(&S1_elem->spawnOffsetHeight, &elem->spawnOffsetHeight, sizeof(FxFloatRange));
			memcpy(&S1_elem->spawnAngles, &elem->spawnAngles, sizeof(FxFloatRange[3]));
			memcpy(&S1_elem->angularVelocity, &elem->angularVelocity, sizeof(FxFloatRange[3]));
			memcpy(&S1_elem->initialRotation, &elem->initialRotation, sizeof(FxFloatRange));
			memcpy(&S1_elem->gravity, &elem->gravity, sizeof(FxFloatRange));
			memcpy(&S1_elem->reflectionFactor, &elem->reflectionFactor, sizeof(FxFloatRange));
			memcpy(&S1_elem->atlas, &elem->atlas, sizeof(FxElemAtlas));
			S1_elem->elemType = convert_elem_type(elem->elemType);
			S1_elem->elemLitType = generate_elem_lit_type(elem->elemType); //S1::FX_ELEM_LIT_TYPE_NONE; // ?
			S1_elem->visualCount = elem->visualCount;
			S1_elem->velIntervalCount = elem->velIntervalCount;
			S1_elem->visStateIntervalCount = elem->visStateIntervalCount;

			REINTERPRET_CAST_SAFE(S1_elem->velSamples, elem->velSamples);

			if (elem->visSamples)
			{
				S1_elem->visSamples = mem.allocate<S1::FxElemVisStateSample>(elem->visStateIntervalCount + 1);
				for (int i = 0; i < elem->visStateIntervalCount + 1; i++)
				{
					// check

					// base
					S1_elem->visSamples[i].base.color[0] = static_cast<int>(elem->visSamples[i].base.color[2]) / 255.0f;
					S1_elem->visSamples[i].base.color[1] = static_cast<int>(elem->visSamples[i].base.color[1]) / 255.0f;
					S1_elem->visSamples[i].base.color[2] = static_cast<int>(elem->visSamples[i].base.color[0]) / 255.0f;
					S1_elem->visSamples[i].base.color[3] = static_cast<int>(elem->visSamples[i].base.color[3]) / 255.0f;
					S1_elem->visSamples[i].base.pad1[0] = 1.0f;
					S1_elem->visSamples[i].base.pad1[1] = 1.0f;
					S1_elem->visSamples[i].base.pad1[2] = 1.0f;
					S1_elem->visSamples[i].base.rotationDelta = elem->visSamples[i].base.rotationDelta;
					S1_elem->visSamples[i].base.rotationTotal = elem->visSamples[i].base.rotationTotal;
					memcpy(&S1_elem->visSamples[i].base.size, &elem->visSamples[i].base.size, sizeof(float[2]));
					S1_elem->visSamples[i].base.scale = elem->visSamples[i].base.scale;
					// pad2
					S1_elem->visSamples[i].base.pad2[1] = 0.0f;
					S1_elem->visSamples[i].base.pad2[1] = 0.0f;

					// amplitude
					S1_elem->visSamples[i].amplitude.color[0] = static_cast<int>(elem->visSamples[i].amplitude.color[2]) / 255.0f;
					S1_elem->visSamples[i].amplitude.color[1] = static_cast<int>(elem->visSamples[i].amplitude.color[1]) / 255.0f;
					S1_elem->visSamples[i].amplitude.color[2] = static_cast<int>(elem->visSamples[i].amplitude.color[0]) / 255.0f;
					S1_elem->visSamples[i].amplitude.color[3] = static_cast<int>(elem->visSamples[i].amplitude.color[3]) / 255.0f;
					S1_elem->visSamples[i].amplitude.pad1[0] = 1.0f;
					S1_elem->visSamples[i].amplitude.pad1[1] = 1.0f;
					S1_elem->visSamples[i].amplitude.pad1[2] = 1.0f;
					S1_elem->visSamples[i].amplitude.rotationDelta = elem->visSamples[i].amplitude.rotationDelta;
					S1_elem->visSamples[i].amplitude.rotationTotal = elem->visSamples[i].amplitude.rotationTotal;
					memcpy(&S1_elem->visSamples[i].amplitude.size, &elem->visSamples[i].amplitude.size, sizeof(float[2]));
					S1_elem->visSamples[i].amplitude.scale = elem->visSamples[i].amplitude.scale;
					// pad2
					S1_elem->visSamples[i].amplitude.pad2[1] = 0.0f;
					S1_elem->visSamples[i].amplitude.pad2[1] = 0.0f;
				}
			}

			if (elem->elemType == FX_ELEM_TYPE_DECAL)
			{
				S1_elem->visuals.markArray = mem.allocate<S1::FxElemMarkVisuals>(elem->visualCount);
				for (int i = 0; i < elem->visualCount; i++)
				{
					S1_elem->visuals.markArray[i].materials[0] = reinterpret_cast<S1::Material*>(elem->visuals.markArray[i].materials[0]); // mc
					S1_elem->visuals.markArray[i].materials[1] = reinterpret_cast<S1::Material*>(elem->visuals.markArray[i].materials[1]); // wc
					//S1_elem->visuals.markArray[i].materials[2] = reinterpret_cast<S1::Material*>(elem->visuals.markArray[i].materials[1]); // wc displacement
				}
			}
			else if (elem->visualCount > 1)
			{
				S1_elem->visuals.array = mem.allocate<S1::FxElemVisuals>(elem->visualCount);
				for (int i = 0; i < elem->visualCount; i++)
				{
					// this is fine
					S1_elem->visuals.array[i].anonymous = elem->visuals.array[i].anonymous;
				}
			}
			else if (elem->visualCount)
			{
				// this is fine
				S1_elem->visuals.instance.anonymous = elem->visuals.instance.anonymous;
			}

			memcpy(&S1_elem->collBounds, &elem->collBounds, sizeof(float[3][2]));

			S1_elem->effectOnImpact.name = elem->effectOnImpact.name;
			S1_elem->effectOnDeath.name = elem->effectOnDeath.name;
			S1_elem->effectEmitted.name = elem->effectEmitted.name;
			memcpy(&S1_elem->emitDist, &elem->emitDist, sizeof(FxFloatRange));
			memcpy(&S1_elem->emitDistVariance, &elem->emitDistVariance, sizeof(FxFloatRange));

			switch (elem->elemType)
			{
			case FX_ELEM_TYPE_TRAIL:
				S1_elem->extended.trailDef = mem.allocate<S1::FxTrailDef>();

				// check
				S1_elem->extended.trailDef->scrollTimeMsec = elem->extended.trailDef->scrollTimeMsec;
				S1_elem->extended.trailDef->repeatDist = elem->extended.trailDef->repeatDist;
				S1_elem->extended.trailDef->invSplitDist = elem->extended.trailDef->invSplitDist;
				S1_elem->extended.trailDef->invSplitArcDist = elem->extended.trailDef->invSplitArcDist;
				S1_elem->extended.trailDef->invSplitTime = elem->extended.trailDef->invSplitTime;
				// pad

				S1_elem->extended.trailDef->vertCount = elem->extended.trailDef->vertCount;
				S1_elem->extended.trailDef->verts = mem.allocate<S1::FxTrailVertex>(S1_elem->extended.trailDef->vertCount);
				for (int i = 0; i < S1_elem->extended.trailDef->vertCount; i++)
				{
					// check
					memcpy(&S1_elem->extended.trailDef->verts[i].pos, &elem->extended.trailDef->verts[i].pos, sizeof(float[2]));
					memcpy(&S1_elem->extended.trailDef->verts[i].normal, &elem->extended.trailDef->verts[i].normal, sizeof(float[2]));
					memcpy(&S1_elem->extended.trailDef->verts[i].texCoord, &elem->extended.trailDef->verts[i].texCoord, sizeof(float[2]));
					// pad
				}

				S1_elem->extended.trailDef->indCount = elem->extended.trailDef->indCount;
				REINTERPRET_CAST_SAFE(S1_elem->extended.trailDef->inds, elem->extended.trailDef->inds);
				break;
			case FX_ELEM_TYPE_SPARKFOUNTAIN:
				// check
				REINTERPRET_CAST_SAFE(S1_elem->extended.sparkFountainDef, elem->extended.sparkFountainDef);
				break;
			case FX_ELEM_TYPE_SPOT_LIGHT:
				// check
				S1_elem->extended.spotLightDef = mem.allocate<S1::FxSpotLightDef>();
				if (elem->extended.spotLightDef)
				{
					S1_elem->extended.spotLightDef->fovInnerFraction = elem->extended.spotLightDef->fovInnerFraction;
					S1_elem->extended.spotLightDef->startRadius = elem->extended.spotLightDef->startRadius;
					S1_elem->extended.spotLightDef->endRadius = elem->extended.spotLightDef->endRadius;
					S1_elem->extended.spotLightDef->brightness = elem->extended.spotLightDef->brightness;
					S1_elem->extended.spotLightDef->maxLength = elem->extended.spotLightDef->maxLength;
					S1_elem->extended.spotLightDef->exponent = elem->extended.spotLightDef->exponent;
					// pad
				}
				break;
			case FX_ELEM_TYPE_OMNI_LIGHT:
				S1_elem->extended.omniLightDef = mem.allocate<S1::FxOmniLightDef>();
				// todo?
				break;
			default:
				S1_elem->extended.unknownDef = elem->extended.unknownDef;
				break;
			}

			// check:
			S1_elem->sortOrder = elem->sortOrder;
			S1_elem->lightingFrac = elem->lightingFrac;
			S1_elem->useItemClip = elem->useItemClip;
			S1_elem->fadeInfo = elem->fadeInfo;
			S1_elem->randomSeed = elem->randomSeed;

			S1_elem->__pad0[0] = 0.0f;
			S1_elem->__pad0[1] = 1.0f;
			S1_elem->__pad0[2] = 0.0f;
			S1_elem->__pad0[3] = 1.0f;
			S1_elem->__pad0[4] = 0.0f;
			S1_elem->__pad0[5] = -1.0f;
		}

		S1::FxEffectDef* GenerateS1FxEffectDef(FxEffectDef* asset, allocator& mem)
		{
			auto* S1_asset = mem.allocate<S1::FxEffectDef>();
			S1_asset->name = asset->name;

			S1_asset->flags = asset->flags; // convert? should be the same...
			S1_asset->totalSize = asset->totalSize;
			S1_asset->msecLoopingLife = asset->msecLoopingLife;
			S1_asset->elemDefCountLooping = asset->elemDefCountLooping;
			S1_asset->elemDefCountOneShot = asset->elemDefCountOneShot;
			S1_asset->elemDefCountEmission = asset->elemDefCountEmission;
			S1_asset->elemMaxRadius = 0;
			S1_asset->occlusionQueryDepthBias = asset->occlusionQueryDepthBias;
			S1_asset->occlusionQueryFadeIn = asset->occlusionQueryFadeIn;
			S1_asset->occlusionQueryFadeOut = asset->occlusionQueryFadeOut;
			S1_asset->occlusionQueryScaleRange.base = asset->occlusionQueryScaleRange.base;
			S1_asset->occlusionQueryScaleRange.amplitude = asset->occlusionQueryScaleRange.amplitude;

			S1_asset->elemDefs = mem.allocate<S1::FxElemDef>(asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission);
			for (auto i = 0; i < asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission; i++)
			{
				GenerateS1FxElemDef(&S1_asset->elemDefs[i], &asset->elemDefs[i], mem);
			}

			return S1_asset;
		}

		S1::FxEffectDef* convert(FxEffectDef* asset, allocator& allocator)
		{
			// generate S1 fx
			return GenerateS1FxEffectDef(asset, allocator);
		}
	}
}