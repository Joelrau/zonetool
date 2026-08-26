#include "stdafx.hpp"
#include "../Include.hpp"

#include "FxEffectDef.hpp"

namespace ZoneTool::IW5
{
	namespace H1Converter
	{
		H1::FxElemType convert_elem_type(IW5::FxElemType type)
		{
			switch (type)
			{
			case IW5::FX_ELEM_TYPE_SPRITE_BILLBOARD:
				return H1::FX_ELEM_TYPE_SPRITE_BILLBOARD;
				break;
			case IW5::FX_ELEM_TYPE_SPRITE_ORIENTED:
				return H1::FX_ELEM_TYPE_SPRITE_ORIENTED;
				break;
			case IW5::FX_ELEM_TYPE_TAIL:
				return H1::FX_ELEM_TYPE_TAIL;
				break;
			case IW5::FX_ELEM_TYPE_TRAIL:
				return H1::FX_ELEM_TYPE_TRAIL;
				break;
			case IW5::FX_ELEM_TYPE_CLOUD:
				return H1::FX_ELEM_TYPE_CLOUD;
				break;
			case IW5::FX_ELEM_TYPE_SPARKCLOUD:
				return H1::FX_ELEM_TYPE_SPARK_CLOUD;
				break;
			case IW5::FX_ELEM_TYPE_SPARKFOUNTAIN:
				return H1::FX_ELEM_TYPE_SPARK_FOUNTAIN;
				break;
			case IW5::FX_ELEM_TYPE_MODEL:
				return H1::FX_ELEM_TYPE_MODEL;
				break;
			case IW5::FX_ELEM_TYPE_OMNI_LIGHT:
				return H1::FX_ELEM_TYPE_OMNI_LIGHT;
				break;
			case IW5::FX_ELEM_TYPE_SPOT_LIGHT:
				return H1::FX_ELEM_TYPE_SPOT_LIGHT;
				break;
			case IW5::FX_ELEM_TYPE_SOUND:
				return H1::FX_ELEM_TYPE_SOUND;
				break;
			case IW5::FX_ELEM_TYPE_DECAL:
				return H1::FX_ELEM_TYPE_DECAL;
				break;
			case IW5::FX_ELEM_TYPE_RUNNER:
				return H1::FX_ELEM_TYPE_RUNNER;
				break;
			}

			return H1::FX_ELEM_TYPE_SPRITE_BILLBOARD;
		}

		H1::FxElemLitType generate_elem_lit_type(IW5::FxElemType type)
		{
			switch (type)
			{
			case IW5::FX_ELEM_TYPE_SPRITE_BILLBOARD:
				return H1::FX_ELEM_LIT_TYPE_LIGHTGRID_SPAWN_SINGLE;
				break;
			case IW5::FX_ELEM_TYPE_SPRITE_ORIENTED:
				return H1::FX_ELEM_LIT_TYPE_LIGHTGRID_SPAWN_SINGLE;
				break;
			case IW5::FX_ELEM_TYPE_TAIL:
				return H1::FX_ELEM_LIT_TYPE_LIGHTGRID_SPAWN_SINGLE;
				break;
			case IW5::FX_ELEM_TYPE_TRAIL:
				return H1::FX_ELEM_LIT_TYPE_LIGHTGRID_FRAME_VERTEX;
				break;
			case IW5::FX_ELEM_TYPE_CLOUD:
				return H1::FX_ELEM_LIT_TYPE_LIGHTGRID_SPAWN_SINGLE;
				break;
			case IW5::FX_ELEM_TYPE_SPARKCLOUD:
				return H1::FX_ELEM_LIT_TYPE_LIGHTGRID_SPAWN_SINGLE;
				break;
			case IW5::FX_ELEM_TYPE_SPARKFOUNTAIN:
				return H1::FX_ELEM_LIT_TYPE_LIGHTGRID_SPAWN_SINGLE;
				break;
			case IW5::FX_ELEM_TYPE_MODEL:
				return H1::FX_ELEM_LIT_TYPE_NONE;
				break;
			case IW5::FX_ELEM_TYPE_OMNI_LIGHT:
				return H1::FX_ELEM_LIT_TYPE_LIGHTGRID_SPAWN_SINGLE;
				break;
			case IW5::FX_ELEM_TYPE_SPOT_LIGHT:
				return H1::FX_ELEM_LIT_TYPE_LIGHTGRID_SPAWN_SINGLE;
				break;
			case IW5::FX_ELEM_TYPE_SOUND:
				return H1::FX_ELEM_LIT_TYPE_NONE;
				break;
			case IW5::FX_ELEM_TYPE_DECAL:
				return H1::FX_ELEM_LIT_TYPE_NONE;
				break;
			case IW5::FX_ELEM_TYPE_RUNNER:
				return H1::FX_ELEM_LIT_TYPE_LIGHTGRID_SPAWN_SINGLE;
				break;
			}

			return H1::FX_ELEM_LIT_TYPE_NONE;
		}

		unsigned int convert_elem_flags(unsigned int flags)
		{
			unsigned int h1_flags = 0;
			auto convert = [&](unsigned int a, unsigned int b, unsigned int mask = 0)
			{
				h1_flags |= ((flags & (mask ? mask : a)) == a) ? b : 0;
			};

			convert(IW5::FxElemDefFlags::FX_ELEM_SPAWN_RELATIVE_TO_EFFECT, H1::FxElemDefFlags::FX_ELEM_SPAWN_RELATIVE_TO_EFFECT);
			convert(IW5::FxElemDefFlags::FX_ELEM_SPAWN_FRUSTUM_CULL, H1::FxElemDefFlags::FX_ELEM_SPAWN_FRUSTUM_CULL);
			convert(IW5::FxElemDefFlags::FX_ELEM_RUNNER_USES_RAND_ROT, H1::FxElemDefFlags::FX_ELEM_RUNNER_USES_RAND_ROT);
			convert(IW5::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_NONE, H1::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_NONE, IW5::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_SPHERE, H1::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_SPHERE, IW5::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_CYLINDER, H1::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_CYLINDER, IW5::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_WORLD, H1::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_WORLD, IW5::FxElemDefFlags::FX_ELEM_RUN_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_SPAWN, H1::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_SPAWN, IW5::FxElemDefFlags::FX_ELEM_RUN_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_EFFECT, H1::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_EFFECT, IW5::FxElemDefFlags::FX_ELEM_RUN_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_OFFSET, H1::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_OFFSET, IW5::FxElemDefFlags::FX_ELEM_RUN_MASK);
			//convert(IW5::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_CAMERA, H1::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_CAMERA);
			convert(IW5::FxElemDefFlags::FX_ELEM_DIE_ON_TOUCH, H1::FxElemDefFlags::FX_ELEM_DIE_ON_TOUCH);
			convert(IW5::FxElemDefFlags::FX_ELEM_DRAW_PAST_FOG, H1::FxElemDefFlags::FX_ELEM_DRAW_PAST_FOG);
			convert(IW5::FxElemDefFlags::FX_ELEM_DRAW_WITH_VIEWMODEL, H1::FxElemDefFlags::FX_ELEM_DRAW_WITH_VIEWMODEL);
			convert(IW5::FxElemDefFlags::FX_ELEM_BLOCK_SIGHT, H1::FxElemDefFlags::FX_ELEM_BLOCK_SIGHT);
			convert(IW5::FxElemDefFlags::FX_ELEM_DRAW_IN_THERMAL_VIEW_ONLY, H1::FxElemDefFlags::FX_ELEM_DRAW_IN_THERMAL_VIEW_ONLY);
			convert(IW5::FxElemDefFlags::FX_ELEM_TRAIL_ORIENT_BY_VELOCITY, H1::FxElemDefFlags::FX_ELEM_TRAIL_ORIENT_BY_VELOCITY);
			//convert(IW5::FxElemDefFlags::FX_ELEM_EMIT_BOLT, H1::FxElemDefFlags::FX_ELEM_EMIT_BOLT);
			convert(IW5::FxElemDefFlags::FX_ELEM_EMIT_ORIENT_BY_ELEM, H1::FxElemDefFlags::FX_ELEM_EMIT_ORIENT_BY_ELEM);
			convert(IW5::FxElemDefFlags::FX_ELEM_USE_OCCLUSION_QUERY, H1::FxElemDefFlags::FX_ELEM_USE_OCCLUSION_QUERY);
			//convert(IW5::FxElemDefFlags::FX_ELEM_USE_CAST_SHADOW, H1::FxElemDefFlags::FX_ELEM_USE_CAST_SHADOW);
			//convert(IW5::FxElemDefFlags::FX_ELEM_NODRAW_IN_THERMAL_VIEW, H1::FxElemDefFlags::FX_ELEM_NODRAW_IN_THERMAL_VIEW);
			//convert(IW5::FxElemDefFlags::FX_ELEM_SPAWN_IMPACT_FX_WITH_SURFACE_NAME, H1::FxElemDefFlags::FX_ELEM_SPAWN_IMPACT_FX_WITH_SURFACE_NAME);
			//convert(IW5::FxElemDefFlags::FX_ELEM_RECEIVE_DYNAMIC_LIGHT, H1::FxElemDefFlags::FX_ELEM_RECEIVE_DYNAMIC_LIGHT);
			//convert(IW5::FxElemDefFlags::FX_ELEM_VOLUMETRIC_TRAIL, H1::FxElemDefFlags::FX_ELEM_VOLUMETRIC_TRAIL);
			convert(IW5::FxElemDefFlags::FX_ELEM_USE_COLLISION, H1::FxElemDefFlags::FX_ELEM_USE_COLLISION);
			//convert(IW5::FxElemDefFlags::FX_ELEM_USE_VECTORFIELDS, H1::FxElemDefFlags::FX_ELEM_USE_VECTORFIELDS);
			//convert(IW5::FxElemDefFlags::FX_ELEM_NO_SURFACE_HDR_SCALAR, H1::FxElemDefFlags::FX_ELEM_NO_SURFACE_HDR_SCALAR);
			convert(IW5::FxElemDefFlags::FX_ELEM_HAS_VELOCITY_GRAPH_LOCAL, H1::FxElemDefFlags::FX_ELEM_HAS_VELOCITY_GRAPH_LOCAL);
			convert(IW5::FxElemDefFlags::FX_ELEM_HAS_VELOCITY_GRAPH_WORLD, H1::FxElemDefFlags::FX_ELEM_HAS_VELOCITY_GRAPH_WORLD);
			convert(IW5::FxElemDefFlags::FX_ELEM_HAS_GRAVITY, H1::FxElemDefFlags::FX_ELEM_HAS_GRAVITY);
			convert(IW5::FxElemDefFlags::FX_ELEM_USE_MODEL_PHYSICS, H1::FxElemDefFlags::FX_ELEM_USE_MODEL_PHYSICS);
			convert(IW5::FxElemDefFlags::FX_ELEM_NONUNIFORM_SCALE, H1::FxElemDefFlags::FX_ELEM_NONUNIFORM_SCALE);
			convert(IW5::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_CUBE, H1::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_CUBE, IW5::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_SPHERE_LARGE, H1::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_SPHERE_LARGE, IW5::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_SPHERE_MEDIUM, H1::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_SPHERE_MEDIUM, IW5::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_SPHERE_SMALL, H1::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_SPHERE_SMALL, IW5::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_MASK);
			convert(IW5::FxElemDefFlags::FX_ELEM_FOUNTAIN_DISABLE_COLLISION, H1::FxElemDefFlags::FX_ELEM_FOUNTAIN_DISABLE_COLLISION);

			return h1_flags;
		}

		void GenerateH1FxElemDef(H1::FxElemDef* h1_elem, FxElemDef* elem, allocator& mem, H1::FxEffectDef* h1_effect)
		{
			if (elem->spawnFrustumCullRadius > h1_effect->elemMaxRadius)
			{
				h1_effect->elemMaxRadius = elem->spawnFrustumCullRadius;
			}

			bool emissive = false;
			const auto emissive_check = [&](const Material* mat)
			{
				switch (elem->elemType)
				{
				case FX_ELEM_TYPE_SPRITE_BILLBOARD:
				case FX_ELEM_TYPE_SPRITE_ORIENTED:
				case FX_ELEM_TYPE_TAIL:
				case FX_ELEM_TYPE_TRAIL:
				{
					std::string mat_name = mat->name;
					if (mat_name.find("distort") != std::string::npos)
					{
						emissive = true;
					}
				}
				break;
				}
			};

			if (elem->elemType == FX_ELEM_TYPE_DECAL)
			{
			}
			else if (elem->visualCount > 1)
			{
				for (int i = 0; i < elem->visualCount; i++)
				{
					emissive_check(elem->visuals.array[i].material);
				}
			}
			else if (elem->visualCount)
			{
				emissive_check(elem->visuals.instance.material);
			}

			h1_elem->flags = static_cast<H1::FxElemDefFlags>(convert_elem_flags(elem->flags));
			h1_elem->flags2 = 0;

			if (emissive)
			{
				h1_effect->flags |= H1::FX_EFFECT_NEEDS_EMISSIVE_DRAW;
				h1_elem->flags2 |= H1::FX_ELEM2_USE_EMISSIVE_DRAW;
			}

			memcpy(&h1_elem->spawn, &elem->spawn, sizeof(FxSpawnDef));
			memcpy(&h1_elem->spawnRange, &elem->spawnRange, sizeof(FxFloatRange));
			memcpy(&h1_elem->fadeInRange, &elem->fadeInRange, sizeof(FxFloatRange));
			memcpy(&h1_elem->fadeOutRange, &elem->fadeOutRange, sizeof(FxFloatRange));
			h1_elem->spawnFrustumCullRadius = elem->spawnFrustumCullRadius;
			memcpy(&h1_elem->spawnDelayMsec, &elem->spawnDelayMsec, sizeof(FxIntRange));
			memcpy(&h1_elem->lifeSpanMsec, &elem->lifeSpanMsec, sizeof(FxIntRange));
			memcpy(&h1_elem->spawnOrigin, &elem->spawnOrigin, sizeof(FxFloatRange[3]));
			memcpy(&h1_elem->spawnOffsetRadius, &elem->spawnOffsetRadius, sizeof(FxFloatRange));
			memcpy(&h1_elem->spawnOffsetHeight, &elem->spawnOffsetHeight, sizeof(FxFloatRange));
			memcpy(&h1_elem->spawnAngles, &elem->spawnAngles, sizeof(FxFloatRange[3]));
			memcpy(&h1_elem->angularVelocity, &elem->angularVelocity, sizeof(FxFloatRange[3]));
			memcpy(&h1_elem->initialRotation, &elem->initialRotation, sizeof(FxFloatRange));
			memcpy(&h1_elem->gravity, &elem->gravity, sizeof(FxFloatRange));
			memcpy(&h1_elem->reflectionFactor, &elem->reflectionFactor, sizeof(FxFloatRange));
			memcpy(&h1_elem->atlas, &elem->atlas, sizeof(FxElemAtlas));
			h1_elem->elemType = convert_elem_type(elem->elemType);

			h1_elem->elemLitType = generate_elem_lit_type(elem->elemType);

			h1_elem->visualCount = elem->visualCount;
			h1_elem->velIntervalCount = elem->velIntervalCount;
			h1_elem->visStateIntervalCount = elem->visStateIntervalCount;

			REINTERPRET_CAST_SAFE_TO_FROM(h1_elem->velSamples, elem->velSamples);

			if (elem->visSamples)
			{
				h1_elem->visSamples = mem.allocate<H1::FxElemVisStateSample>(elem->visStateIntervalCount + 1);
				for (int i = 0; i < elem->visStateIntervalCount + 1; i++)
				{
					// base
					Byte4::Byte4UnpackRgba(h1_elem->visSamples[i].base.color, elem->visSamples[i].base.color);
					std::swap(h1_elem->visSamples[i].base.color[0], h1_elem->visSamples[i].base.color[2]); // bgra -> rgba

					h1_elem->visSamples[i].base.emissiveScale[0] = 0.0f;
					h1_elem->visSamples[i].base.emissiveScale[1] = 0.0f;
					h1_elem->visSamples[i].base.emissiveScale[2] = 0.0f;
					h1_elem->visSamples[i].base.rotationDelta = elem->visSamples[i].base.rotationDelta;
					h1_elem->visSamples[i].base.rotationTotal = elem->visSamples[i].base.rotationTotal;
					memcpy(&h1_elem->visSamples[i].base.size, &elem->visSamples[i].base.size, sizeof(float[2]));
					h1_elem->visSamples[i].base.scale = elem->visSamples[i].base.scale;
					h1_elem->visSamples[i].base.pivot[0] = 0.0f;
					h1_elem->visSamples[i].base.pivot[1] = 0.0f;

					// amplitude
					Byte4::Byte4UnpackRgba(h1_elem->visSamples[i].amplitude.color, elem->visSamples[i].amplitude.color);
					std::swap(h1_elem->visSamples[i].amplitude.color[0], h1_elem->visSamples[i].amplitude.color[2]); // bgra -> rgba

					h1_elem->visSamples[i].amplitude.emissiveScale[0] = 0.0f;
					h1_elem->visSamples[i].amplitude.emissiveScale[1] = 0.0f;
					h1_elem->visSamples[i].amplitude.emissiveScale[2] = 0.0f;
					h1_elem->visSamples[i].amplitude.rotationDelta = elem->visSamples[i].amplitude.rotationDelta;
					h1_elem->visSamples[i].amplitude.rotationTotal = elem->visSamples[i].amplitude.rotationTotal;
					memcpy(&h1_elem->visSamples[i].amplitude.size, &elem->visSamples[i].amplitude.size, sizeof(float[2]));
					h1_elem->visSamples[i].amplitude.scale = elem->visSamples[i].amplitude.scale;
					h1_elem->visSamples[i].amplitude.pivot[0] = 0.0f;
					h1_elem->visSamples[i].amplitude.pivot[1] = 0.0f;

					if (emissive)
					{
						h1_elem->visSamples[i].base.emissiveScale[0] = 1.0f;
						h1_elem->visSamples[i].base.emissiveScale[1] = 1.0f;
						h1_elem->visSamples[i].base.emissiveScale[2] = 1.0f;

						h1_elem->visSamples[i].amplitude.emissiveScale[0] = 1.0f;
						h1_elem->visSamples[i].amplitude.emissiveScale[1] = 1.0f;
						h1_elem->visSamples[i].amplitude.emissiveScale[2] = 1.0f;
					}
				}
			}

			if (elem->elemType == FX_ELEM_TYPE_DECAL)
			{
				h1_elem->visuals.markArray = mem.allocate<H1::FxElemMarkVisuals>(elem->visualCount);
				for (int i = 0; i < elem->visualCount; i++)
				{
					h1_elem->visuals.markArray[i].materials[0] = reinterpret_cast<H1::Material*>(elem->visuals.markArray[i].materials[0]); // mc
					h1_elem->visuals.markArray[i].materials[1] = reinterpret_cast<H1::Material*>(elem->visuals.markArray[i].materials[1]); // wc
					h1_elem->visuals.markArray[i].materials[2] = reinterpret_cast<H1::Material*>(elem->visuals.markArray[i].materials[1]); // wc displacement
				}
			}
			else if (elem->visualCount > 1)
			{
				h1_elem->visuals.array = mem.allocate<H1::FxElemVisuals>(elem->visualCount);
				for (int i = 0; i < elem->visualCount; i++)
				{
					// this is fine
					h1_elem->visuals.array[i].anonymous = elem->visuals.array[i].anonymous;
				}
			}
			else if (elem->visualCount)
			{
				// this is fine
				h1_elem->visuals.instance.anonymous = elem->visuals.instance.anonymous;
			}

			memcpy(&h1_elem->collBounds, &elem->collBounds, sizeof(float[3][2]));

			h1_elem->effectOnImpact.name = elem->effectOnImpact.name;
			h1_elem->effectOnDeath.name = elem->effectOnDeath.name;
			h1_elem->effectEmitted.name = elem->effectEmitted.name;
			memcpy(&h1_elem->emitDist, &elem->emitDist, sizeof(FxFloatRange));
			memcpy(&h1_elem->emitDistVariance, &elem->emitDistVariance, sizeof(FxFloatRange));

			switch (elem->elemType)
			{
			case FX_ELEM_TYPE_TRAIL:
				h1_elem->extended.trailDef = mem.allocate<H1::FxTrailDef>();

				// check
				h1_elem->extended.trailDef->scrollTimeMsec = elem->extended.trailDef->scrollTimeMsec;
				h1_elem->extended.trailDef->repeatDist = elem->extended.trailDef->repeatDist;
				h1_elem->extended.trailDef->invSplitDist = elem->extended.trailDef->invSplitDist;
				h1_elem->extended.trailDef->invSplitArcDist = elem->extended.trailDef->invSplitArcDist;
				h1_elem->extended.trailDef->invSplitTime = elem->extended.trailDef->invSplitTime;
				// pad

				h1_elem->extended.trailDef->vertCount = elem->extended.trailDef->vertCount;
				h1_elem->extended.trailDef->verts = mem.allocate<H1::FxTrailVertex>(h1_elem->extended.trailDef->vertCount);
				for (int i = 0; i < h1_elem->extended.trailDef->vertCount; i++)
				{
					// check
					memcpy(&h1_elem->extended.trailDef->verts[i].pos, &elem->extended.trailDef->verts[i].pos, sizeof(float[2]));
					memcpy(&h1_elem->extended.trailDef->verts[i].normal, &elem->extended.trailDef->verts[i].normal, sizeof(float[2]));
					memcpy(&h1_elem->extended.trailDef->verts[i].texCoord, &elem->extended.trailDef->verts[i].texCoord, sizeof(float[2]));
					// pad
				}

				h1_elem->extended.trailDef->indCount = elem->extended.trailDef->indCount;
				REINTERPRET_CAST_SAFE_TO_FROM(h1_elem->extended.trailDef->inds, elem->extended.trailDef->inds);
				break;
			case FX_ELEM_TYPE_SPARKFOUNTAIN:
				// check
				REINTERPRET_CAST_SAFE_TO_FROM(h1_elem->extended.sparkFountainDef, elem->extended.sparkFountainDef);
				break;
			case FX_ELEM_TYPE_SPOT_LIGHT:
				h1_elem->extended.spotLightDef = mem.allocate<H1::FxSpotLightDef>();
				if (elem->extended.spotLightDef)
				{
					// ai giving you the following code:

					auto* dst = h1_elem->extended.spotLightDef;
					auto* src = elem->extended.spotLightDef;

					// Defensive clamps
					const float epsilon = 1e-4f;
					const float maxLen = std::max(src->maxLength, epsilon);
					const float r0 = std::max(src->startRadius, 0.0f);
					const float r1 = std::max(src->endRadius, 0.0f);
					const float dR = std::max(r1 - r0, 0.0f);                 // beam widening
					const float frac = std::clamp(src->fovInnerFraction, 0.0f, 1.0f);

					// Half-angles (radians) from beam slope.
					// Outer: atan( (end-start) / length )
					// Inner uses the same slope scaled by the fraction.
					const float halfFovOuter = std::atan2(dR, maxLen);
					const float halfFovInner = std::atan2(frac * dR, maxLen);

					// Assign to H1
					dst->halfFovOuter = halfFovOuter;
					dst->halfFovInner = std::min(halfFovOuter, halfFovInner);      // safety
					dst->radius = r1;                                         // end radius at max distance
					dst->brightness = src->brightness;                            // might need a scale, engine-dependent
					dst->maxLength = maxLen;
					dst->exponent = src->exponent;

					// Use startRadius to populate emitter/bulb; near clip small to avoid acne
					dst->bulbRadius = r0;                                         // emitter base radius
					dst->bulbLength = 0.0f;                                       // unknown in src; 0 is safest
					dst->nearClip = 0.01f;                                      // tiny, or tie to % of maxLen if you prefer

					// Keep your fade ramp unless you have better data
					dst->fadeOffsetRt[0] = 0.0f;
					dst->fadeOffsetRt[1] = 1.0f;

				}
				break;
			case FX_ELEM_TYPE_OMNI_LIGHT:
				h1_elem->extended.omniLightDef = mem.allocate<H1::FxOmniLightDef>();
				h1_elem->extended.omniLightDef->bulbRadius = 1.0f;
				h1_elem->extended.omniLightDef->bulbLength = 0.0f;
				h1_elem->extended.omniLightDef->fadeOffsetRt[0] = 0.0f;
				h1_elem->extended.omniLightDef->fadeOffsetRt[1] = -1.0f;
				break;
			default:
				h1_elem->extended.unknownDef = elem->extended.unknownDef;
				break;
			}

			h1_elem->sortOrder = elem->sortOrder;
			h1_elem->lightingFrac = elem->lightingFrac;
			h1_elem->useItemClip = elem->useItemClip;
			h1_elem->fadeInfo = elem->fadeInfo;
			h1_elem->fadeOutInfo = 0;
			h1_elem->randomSeed = elem->randomSeed;

			h1_elem->emissiveScaleScale = 0.0f;
			h1_elem->hdrLightingFrac = 1.0f;
			h1_elem->shadowDensityScale = 1.0f;
			h1_elem->scatterRatio = 0.0f;
			h1_elem->volumetricTrailFadeStart = -1.0f;

			// hdr
			if (h1_elem->elemLitType == H1::FX_ELEM_LIT_TYPE_NONE)
			{
				h1_elem->hdrLightingFrac = 0.0f;
			}

			if (emissive)
			{
				h1_elem->hdrLightingFrac = 0.0f;
				h1_elem->emissiveScaleScale = 1.0f;
			}
		}

		H1::FxEffectDef* GenerateH1FxEffectDef(FxEffectDef* asset, allocator& mem)
		{
			auto* h1_asset = mem.allocate<H1::FxEffectDef>();
			h1_asset->name = asset->name;

			h1_asset->flags = asset->flags; // convert? should be the same...
			h1_asset->totalSize = asset->totalSize;
			h1_asset->msecLoopingLife = asset->msecLoopingLife;
			h1_asset->elemDefCountLooping = asset->elemDefCountLooping;
			h1_asset->elemDefCountOneShot = asset->elemDefCountOneShot;
			h1_asset->elemDefCountEmission = asset->elemDefCountEmission;
			h1_asset->elemMaxRadius = 0;
			h1_asset->occlusionQueryDepthBias = asset->occlusionQueryDepthBias;
			h1_asset->occlusionQueryFadeIn = asset->occlusionQueryFadeIn;
			h1_asset->occlusionQueryFadeOut = asset->occlusionQueryFadeOut;
			h1_asset->occlusionQueryScaleRange.base = asset->occlusionQueryScaleRange.base;
			h1_asset->occlusionQueryScaleRange.amplitude = asset->occlusionQueryScaleRange.amplitude;

			h1_asset->elemDefs = mem.allocate<H1::FxElemDef>(asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission);
			for (auto i = 0; i < asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission; i++)
			{
				GenerateH1FxElemDef(&h1_asset->elemDefs[i], &asset->elemDefs[i], mem, h1_asset);
			}

			return h1_asset;
		}

		H1::FxEffectDef* convert(FxEffectDef* asset, allocator& allocator)
		{
			// generate h1 fx
			return GenerateH1FxEffectDef(asset, allocator);
		}
	}
}