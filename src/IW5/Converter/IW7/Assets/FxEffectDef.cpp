#include "stdafx.hpp"
#include "../Include.hpp"

#include "FxEffectDef.hpp"

namespace ZoneTool::IW5
{
	namespace IW7Converter
	{
		IW7::FxElemType convert_elem_type(IW5::FxElemType type)
		{
			switch (type)
			{
			case IW5::FX_ELEM_TYPE_SPRITE_BILLBOARD:
				return IW7::FX_ELEM_TYPE_SPRITE_BILLBOARD;
				break;
			case IW5::FX_ELEM_TYPE_SPRITE_ORIENTED:
				return IW7::FX_ELEM_TYPE_SPRITE_ORIENTED;
				break;
			case IW5::FX_ELEM_TYPE_TAIL:
				return IW7::FX_ELEM_TYPE_TAIL;
				break;
			case IW5::FX_ELEM_TYPE_TRAIL:
				return IW7::FX_ELEM_TYPE_TRAIL;
				break;
			case IW5::FX_ELEM_TYPE_CLOUD:
				return IW7::FX_ELEM_TYPE_CLOUD;
				break;
			case IW5::FX_ELEM_TYPE_SPARKCLOUD:
				return IW7::FX_ELEM_TYPE_SPARK_CLOUD;
				break;
			case IW5::FX_ELEM_TYPE_SPARKFOUNTAIN:
				return IW7::FX_ELEM_TYPE_SPARK_FOUNTAIN;
				break;
			case IW5::FX_ELEM_TYPE_MODEL:
				return IW7::FX_ELEM_TYPE_MODEL;
				break;
			case IW5::FX_ELEM_TYPE_OMNI_LIGHT:
				return IW7::FX_ELEM_TYPE_OMNI_LIGHT;
				break;
			case IW5::FX_ELEM_TYPE_SPOT_LIGHT:
				return IW7::FX_ELEM_TYPE_SPOT_LIGHT;
				break;
			case IW5::FX_ELEM_TYPE_SOUND:
				return IW7::FX_ELEM_TYPE_SOUND;
				break;
			case IW5::FX_ELEM_TYPE_DECAL:
				return IW7::FX_ELEM_TYPE_DECAL;
				break;
			case IW5::FX_ELEM_TYPE_RUNNER:
				return IW7::FX_ELEM_TYPE_RUNNER;
				break;
			}

			return IW7::FX_ELEM_TYPE_SPRITE_BILLBOARD;
		}

		unsigned int convert_elem_flags(unsigned int flags)
		{
			unsigned int IW7_flags = 0;
			auto convert = [&](unsigned int a, unsigned int b, unsigned int mask = 0)
			{
				IW7_flags |= ((flags & (mask ? mask : a)) == a) ? b : 0;
			};

			convert(IW5::FxElemDefFlags::FX_ELEM_SPAWN_RELATIVE_TO_EFFECT, IW7::FxElemDefFlags::FX_ELEM_SPAWN_RELATIVE_TO_EFFECT);
			convert(IW5::FxElemDefFlags::FX_ELEM_SPAWN_FRUSTUM_CULL, IW7::FxElemDefFlags::FX_ELEM_SPAWN_FRUSTUM_CULL);
			convert(IW5::FxElemDefFlags::FX_ELEM_RUNNER_USES_RAND_ROT, IW7::FxElemDefFlags::FX_ELEM_RUNNER_USES_RAND_ROT);
			convert(IW5::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_NONE, IW7::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_NONE, IW5::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_SPHERE, IW7::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_SPHERE, IW5::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_CYLINDER, IW7::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_CYLINDER, IW5::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_WORLD, IW7::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_WORLD, IW5::FxElemDefFlags::FX_ELEM_RUN_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_SPAWN, IW7::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_SPAWN, IW5::FxElemDefFlags::FX_ELEM_RUN_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_EFFECT, IW7::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_EFFECT, IW5::FxElemDefFlags::FX_ELEM_RUN_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_OFFSET, IW7::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_OFFSET, IW5::FxElemDefFlags::FX_ELEM_RUN_MASK);
			//convert(IW5::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_CAMERA, IW7::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_CAMERA);
			convert(IW5::FxElemDefFlags::FX_ELEM_DIE_ON_TOUCH, IW7::FxElemDefFlags::FX_ELEM_DIE_ON_TOUCH);
			convert(IW5::FxElemDefFlags::FX_ELEM_DRAW_PAST_FOG, IW7::FxElemDefFlags::FX_ELEM_DRAW_PAST_FOG);
			convert(IW5::FxElemDefFlags::FX_ELEM_DRAW_WITH_VIEWMODEL, IW7::FxElemDefFlags::FX_ELEM_DRAW_WITH_VIEWMODEL);
			convert(IW5::FxElemDefFlags::FX_ELEM_BLOCK_SIGHT, IW7::FxElemDefFlags::FX_ELEM_BLOCK_SIGHT);
			convert(IW5::FxElemDefFlags::FX_ELEM_DRAW_IN_THERMAL_VIEW_ONLY, IW7::FxElemDefFlags::FX_ELEM_DRAW_IN_THERMAL_VIEW_ONLY);
			convert(IW5::FxElemDefFlags::FX_ELEM_TRAIL_ORIENT_BY_VELOCITY, IW7::FxElemDefFlags::FX_ELEM_TRAIL_ORIENT_BY_VELOCITY);
			//convert(IW5::FxElemDefFlags::FX_ELEM_EMIT_BOLT, IW7::FxElemDefFlags::FX_ELEM_EMIT_BOLT);
			convert(IW5::FxElemDefFlags::FX_ELEM_EMIT_ORIENT_BY_ELEM, IW7::FxElemDefFlags::FX_ELEM_EMIT_ORIENT_BY_ELEM);
			convert(IW5::FxElemDefFlags::FX_ELEM_USE_OCCLUSION_QUERY, IW7::FxElemDefFlags::FX_ELEM_USE_OCCLUSION_QUERY);
			//convert(IW5::FxElemDefFlags::FX_ELEM_NODRAW_IN_THERMAL_VIEW, IW7::FxElemDefFlags::FX_ELEM_NODRAW_IN_THERMAL_VIEW, IW5::FxElemDefFlags::FX_ELEM_THERMAL_MASK);
			//convert(IW5::FxElemDefFlags::FX_ELEM_SPAWN_IMPACT_FX_WITH_SURFACE_NAME, IW7::FxElemDefFlags::FX_ELEM_SPAWN_IMPACT_FX_WITH_SURFACE_NAME);
			//convert(IW5::FxElemDefFlags::FX_ELEM_RECEIVE_DYNAMIC_LIGHT, IW7::FxElemDefFlags::FX_ELEM_RECEIVE_DYNAMIC_LIGHT);
			//convert(IW5::FxElemDefFlags::FX_ELEM_VOLUMETRIC_TRAIL, IW7::FxElemDefFlags::FX_ELEM_VOLUMETRIC_TRAIL);
			convert(IW5::FxElemDefFlags::FX_ELEM_USE_COLLISION, IW7::FxElemDefFlags::FX_ELEM_USE_COLLISION);
			//convert(IW5::FxElemDefFlags::FX_ELEM_USE_VECTORFIELDS, IW7::FxElemDefFlags::FX_ELEM_USE_VECTORFIELDS);
			//convert(IW5::FxElemDefFlags::FX_ELEM_NO_SURFACE_HDR_SCALAR, IW7::FxElemDefFlags::FX_ELEM_NO_SURFACE_HDR_SCALAR);
			convert(IW5::FxElemDefFlags::FX_ELEM_HAS_VELOCITY_GRAPH_LOCAL, IW7::FxElemDefFlags::FX_ELEM_HAS_VELOCITY_GRAPH_LOCAL);
			convert(IW5::FxElemDefFlags::FX_ELEM_HAS_VELOCITY_GRAPH_WORLD, IW7::FxElemDefFlags::FX_ELEM_HAS_VELOCITY_GRAPH_WORLD);
			convert(IW5::FxElemDefFlags::FX_ELEM_HAS_GRAVITY, IW7::FxElemDefFlags::FX_ELEM_HAS_GRAVITY);
			convert(IW5::FxElemDefFlags::FX_ELEM_USE_MODEL_PHYSICS, IW7::FxElemDefFlags::FX_ELEM_USE_MODEL_PHYSICS);
			convert(IW5::FxElemDefFlags::FX_ELEM_NONUNIFORM_SCALE, IW7::FxElemDefFlags::FX_ELEM_NONUNIFORM_SCALE);
			convert(IW5::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_CUBE, IW7::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_CUBE, IW5::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_SPHERE_LARGE, IW7::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_SPHERE_LARGE, IW5::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_SPHERE_MEDIUM, IW7::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_SPHERE_MEDIUM, IW5::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_SPHERE_SMALL, IW7::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_SPHERE_SMALL, IW5::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_FOUNTAIN_DISABLE_COLLISION, IW7::FxElemDefFlags::FX_ELEM_FOUNTAIN_DISABLE_COLLISION);

			return IW7_flags;
		}

		void GenerateIW7FxElemDef(IW7::FxElemDef* iw7_elem, FxElemDef* elem, allocator& mem)
		{
			iw7_elem->flags = static_cast<IW7::FxElemDefFlags>(convert_elem_flags(elem->flags));
			iw7_elem->flags2 = 0;
			memcpy(&iw7_elem->spawn, &elem->spawn, sizeof(FxSpawnDef));
			memcpy(&iw7_elem->spawnRange, &elem->spawnRange, sizeof(FxFloatRange));
			memcpy(&iw7_elem->fadeInRange, &elem->fadeInRange, sizeof(FxFloatRange));
			memcpy(&iw7_elem->fadeOutRange, &elem->fadeOutRange, sizeof(FxFloatRange));
			iw7_elem->spawnFrustumCullRadius = elem->spawnFrustumCullRadius;
			memcpy(&iw7_elem->spawnDelayMsec, &elem->spawnDelayMsec, sizeof(FxIntRange));
			memcpy(&iw7_elem->lifeSpanMsec, &elem->lifeSpanMsec, sizeof(FxIntRange));
			memcpy(&iw7_elem->spawnOrigin, &elem->spawnOrigin, sizeof(FxFloatRange[3]));
			memcpy(&iw7_elem->spawnOffsetRadius, &elem->spawnOffsetRadius, sizeof(FxFloatRange));
			memcpy(&iw7_elem->spawnOffsetHeight, &elem->spawnOffsetHeight, sizeof(FxFloatRange));
			memcpy(&iw7_elem->spawnAngles, &elem->spawnAngles, sizeof(FxFloatRange[3]));
			memcpy(&iw7_elem->angularVelocity, &elem->angularVelocity, sizeof(FxFloatRange[3]));
			memcpy(&iw7_elem->initialRotation, &elem->initialRotation, sizeof(FxFloatRange));
			memcpy(&iw7_elem->gravity, &elem->gravity, sizeof(FxFloatRange));
			memcpy(&iw7_elem->reflectionFactor, &elem->reflectionFactor, sizeof(FxFloatRange));

			iw7_elem->atlas.behavior = elem->atlas.behavior;
			iw7_elem->atlas.index = elem->atlas.index;
			iw7_elem->atlas.fps = elem->atlas.fps;
			iw7_elem->atlas.loopCount = elem->atlas.loopCount;
			iw7_elem->atlas.colIndexBits = elem->atlas.colIndexBits;
			iw7_elem->atlas.rowIndexBits = elem->atlas.rowIndexBits;
			iw7_elem->atlas.entryCount = elem->atlas.entryCount;

			iw7_elem->elemType = convert_elem_type(elem->elemType);
			iw7_elem->visualCount = elem->visualCount;
			iw7_elem->velIntervalCount = elem->velIntervalCount;
			iw7_elem->visStateIntervalCount = elem->visStateIntervalCount;

			REINTERPRET_CAST_SAFE(iw7_elem->velSamples, elem->velSamples);

			if (elem->visSamples)
			{
				iw7_elem->visSamples = mem.allocate<IW7::FxElemVisStateSample>(elem->visStateIntervalCount + 1);
				for (int i = 0; i < elem->visStateIntervalCount + 1; i++)
				{
					// base
					iw7_elem->visSamples[i].base.color[0] = static_cast<int>(elem->visSamples[i].base.color[2]) / 255.0f;
					iw7_elem->visSamples[i].base.color[1] = static_cast<int>(elem->visSamples[i].base.color[1]) / 255.0f;
					iw7_elem->visSamples[i].base.color[2] = static_cast<int>(elem->visSamples[i].base.color[0]) / 255.0f;
					iw7_elem->visSamples[i].base.color[3] = static_cast<int>(elem->visSamples[i].base.color[3]) / 255.0f;
					iw7_elem->visSamples[i].base.rotationDelta = elem->visSamples[i].base.rotationDelta;
					iw7_elem->visSamples[i].base.rotationTotal = elem->visSamples[i].base.rotationTotal;
					memcpy(&iw7_elem->visSamples[i].base.size, &elem->visSamples[i].base.size, sizeof(float[2]));
					iw7_elem->visSamples[i].base.scale = elem->visSamples[i].base.scale;

					// amplitude
					iw7_elem->visSamples[i].amplitude.color[0] = static_cast<int>(elem->visSamples[i].amplitude.color[2]) / 255.0f;
					iw7_elem->visSamples[i].amplitude.color[1] = static_cast<int>(elem->visSamples[i].amplitude.color[1]) / 255.0f;
					iw7_elem->visSamples[i].amplitude.color[2] = static_cast<int>(elem->visSamples[i].amplitude.color[0]) / 255.0f;
					iw7_elem->visSamples[i].amplitude.color[3] = static_cast<int>(elem->visSamples[i].amplitude.color[3]) / 255.0f;
					iw7_elem->visSamples[i].amplitude.rotationDelta = elem->visSamples[i].amplitude.rotationDelta;
					iw7_elem->visSamples[i].amplitude.rotationTotal = elem->visSamples[i].amplitude.rotationTotal;
					memcpy(&iw7_elem->visSamples[i].amplitude.size, &elem->visSamples[i].amplitude.size, sizeof(float[2]));
					iw7_elem->visSamples[i].amplitude.scale = elem->visSamples[i].amplitude.scale;
				}
			}

			if (elem->elemType == FX_ELEM_TYPE_DECAL)
			{
				iw7_elem->visuals.markArray = mem.allocate<IW7::FxElemMarkVisuals>(elem->visualCount);
				for (int i = 0; i < elem->visualCount; i++)
				{
					if (elem->visuals.markArray[i].materials[0])
					{
						iw7_elem->visuals.markArray[i].materials[0] = mem.manual_allocate<IW7::Material>(8);
						iw7_elem->visuals.markArray[i].materials[0]->name = mem.duplicate_string(IW7::replace_material_prefix(elem->visuals.markArray[i].materials[0]->name)); // mc
					}
					if (elem->visuals.markArray[i].materials[1])
					{
						iw7_elem->visuals.markArray[i].materials[1] = mem.manual_allocate<IW7::Material>(8);
						iw7_elem->visuals.markArray[i].materials[1]->name = mem.duplicate_string(IW7::replace_material_prefix(elem->visuals.markArray[i].materials[1]->name)); // wc

						iw7_elem->visuals.markArray[i].materials[2] = mem.manual_allocate<IW7::Material>(8);
						iw7_elem->visuals.markArray[i].materials[2]->name = mem.duplicate_string(IW7::replace_material_prefix(elem->visuals.markArray[i].materials[1]->name)); // wc displacement
					}
				}
			}
			else if (elem->visualCount > 1)
			{
				iw7_elem->visuals.array = mem.allocate<IW7::FxElemVisuals>(elem->visualCount);
				for (int i = 0; i < elem->visualCount; i++)
				{
					// this is fine
					iw7_elem->visuals.array[i].anonymous = elem->visuals.array[i].anonymous;

					if (elem->elemType == FX_ELEM_TYPE_SPRITE_BILLBOARD
						|| elem->elemType == FX_ELEM_TYPE_SPRITE_ORIENTED
						|| elem->elemType == FX_ELEM_TYPE_TAIL
						|| elem->elemType == FX_ELEM_TYPE_TRAIL
						|| elem->elemType == FX_ELEM_TYPE_CLOUD
						|| elem->elemType == FX_ELEM_TYPE_SPARKCLOUD
						|| elem->elemType == FX_ELEM_TYPE_SPARKFOUNTAIN)
					{
						iw7_elem->visuals.array[i].material = mem.manual_allocate<IW7::Material>(8);
						iw7_elem->visuals.array[i].material->name = mem.duplicate_string(IW7::replace_material_prefix(elem->visuals.array[i].material->name));
					}
				}
			}
			else if (elem->visualCount)
			{
				// this is fine
				iw7_elem->visuals.instance.anonymous = elem->visuals.instance.anonymous;

				if (elem->elemType == FX_ELEM_TYPE_SPRITE_BILLBOARD 
					|| elem->elemType == FX_ELEM_TYPE_SPRITE_ORIENTED
					|| elem->elemType == FX_ELEM_TYPE_TAIL
					|| elem->elemType == FX_ELEM_TYPE_TRAIL
					|| elem->elemType == FX_ELEM_TYPE_CLOUD
					|| elem->elemType == FX_ELEM_TYPE_SPARKCLOUD
					|| elem->elemType == FX_ELEM_TYPE_SPARKFOUNTAIN)
				{
					iw7_elem->visuals.instance.material = mem.manual_allocate<IW7::Material>(8);
					iw7_elem->visuals.instance.material->name = mem.duplicate_string(IW7::replace_material_prefix(elem->visuals.instance.material->name));
				}
			}

			memcpy(&iw7_elem->collBounds, &elem->collBounds, sizeof(float[3][2]));

			iw7_elem->effectOnImpact.name = elem->effectOnImpact.name;
			iw7_elem->effectOnDeath.name = elem->effectOnDeath.name;
			iw7_elem->effectEmitted.name = elem->effectEmitted.name;
			memcpy(&iw7_elem->emitDist, &elem->emitDist, sizeof(FxFloatRange));
			memcpy(&iw7_elem->emitDistVariance, &elem->emitDistVariance, sizeof(FxFloatRange));

			switch (elem->elemType)
			{
			case FX_ELEM_TYPE_TRAIL:
				iw7_elem->extended.trailDef = mem.allocate<IW7::FxTrailDef>();

				iw7_elem->extended.trailDef->scrollTimeMsec = elem->extended.trailDef->scrollTimeMsec;
				iw7_elem->extended.trailDef->repeatDist = elem->extended.trailDef->repeatDist;
				iw7_elem->extended.trailDef->invSplitDist = elem->extended.trailDef->invSplitDist;
				iw7_elem->extended.trailDef->invSplitArcDist = elem->extended.trailDef->invSplitArcDist;
				iw7_elem->extended.trailDef->invSplitTime = elem->extended.trailDef->invSplitTime;

				iw7_elem->extended.trailDef->vertCount = elem->extended.trailDef->vertCount;
				iw7_elem->extended.trailDef->verts = mem.allocate<IW7::FxTrailVertex>(iw7_elem->extended.trailDef->vertCount);
				for (int i = 0; i < iw7_elem->extended.trailDef->vertCount; i++)
				{
					memcpy(&iw7_elem->extended.trailDef->verts[i].pos, &elem->extended.trailDef->verts[i].pos, sizeof(float[2]));
					memcpy(&iw7_elem->extended.trailDef->verts[i].normal, &elem->extended.trailDef->verts[i].normal, sizeof(float[2]));
					memcpy(&iw7_elem->extended.trailDef->verts[i].texCoord, &elem->extended.trailDef->verts[i].texCoord, sizeof(float[2]));
				}

				iw7_elem->extended.trailDef->indCount = elem->extended.trailDef->indCount;
				REINTERPRET_CAST_SAFE(iw7_elem->extended.trailDef->inds, elem->extended.trailDef->inds);
				break;
			case FX_ELEM_TYPE_SPARKFOUNTAIN:
				REINTERPRET_CAST_SAFE(iw7_elem->extended.sparkFountainDef, elem->extended.sparkFountainDef);
				break;
			case FX_ELEM_TYPE_SPOT_LIGHT:
				iw7_elem->extended.spotLightDef = mem.allocate<IW7::FxSpotLightDef>();
				if (elem->extended.spotLightDef)
				{
					iw7_elem->extended.spotLightDef->fovInnerFraction = elem->extended.spotLightDef->fovInnerFraction;
					iw7_elem->extended.spotLightDef->startRadius = elem->extended.spotLightDef->startRadius;
					iw7_elem->extended.spotLightDef->endRadius = elem->extended.spotLightDef->endRadius;
					iw7_elem->extended.spotLightDef->brightness = elem->extended.spotLightDef->brightness;
					iw7_elem->extended.spotLightDef->maxLength = elem->extended.spotLightDef->maxLength;
					iw7_elem->extended.spotLightDef->exponent = elem->extended.spotLightDef->exponent;
				}
				break;
			default:
				iw7_elem->extended.unknownDef = elem->extended.unknownDef;
				break;
			}

			iw7_elem->sortOrder = elem->sortOrder;
			iw7_elem->lightingFrac = elem->lightingFrac;
			iw7_elem->useItemClip = elem->useItemClip;
			iw7_elem->fadeInfo = elem->fadeInfo;
			iw7_elem->randomSeed = elem->randomSeed;

			iw7_elem->litUnlitBlendFactor = 1.0f;
		}

		IW7::FxEffectDef* GenerateIW7FxEffectDef(FxEffectDef* asset, allocator& mem)
		{
			auto* iw7_asset = mem.allocate<IW7::FxEffectDef>();
			iw7_asset->name = asset->name;

			iw7_asset->flags = asset->flags; // convert? should be the same...
			iw7_asset->totalSize = asset->totalSize;
			iw7_asset->msecLoopingLife = asset->msecLoopingLife;
			iw7_asset->elemDefCountLooping = asset->elemDefCountLooping;
			iw7_asset->elemDefCountOneShot = asset->elemDefCountOneShot;
			iw7_asset->elemDefCountEmission = asset->elemDefCountEmission;
			iw7_asset->elemMaxRadius = 0;
			iw7_asset->occlusionQueryDepthBias = asset->occlusionQueryDepthBias;
			iw7_asset->occlusionQueryFadeIn = asset->occlusionQueryFadeIn;
			iw7_asset->occlusionQueryFadeOut = asset->occlusionQueryFadeOut;
			iw7_asset->occlusionQueryScaleRange.base = asset->occlusionQueryScaleRange.base;
			iw7_asset->occlusionQueryScaleRange.amplitude = asset->occlusionQueryScaleRange.amplitude;

			iw7_asset->elemDefs = mem.allocate<IW7::FxElemDef>(asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission);
			for (auto i = 0; i < asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission; i++)
			{
				GenerateIW7FxElemDef(&iw7_asset->elemDefs[i], &asset->elemDefs[i], mem);
			}

			return iw7_asset;
		}

		IW7::FxEffectDef* convert(FxEffectDef* asset, allocator& allocator)
		{
			// generate IW7 fx
			return GenerateIW7FxEffectDef(asset, allocator);
		}
	}
}