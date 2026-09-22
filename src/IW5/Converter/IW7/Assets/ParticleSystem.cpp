#include "stdafx.hpp"
#include "../Include.hpp"

#include "ParticleSystem.hpp"
#include "XModel.hpp"

#include "IW5/Structs.hpp"
#include "IW5/Dumper/IW7/Assets/Material.hpp"

namespace ZoneTool::IW5
{
	namespace IW7Converter
	{
		__int64 system_flags;
		__int64 state_flags;
		__int64 emitter_flags;

		int GetModuleNumCurves(IW7::ParticleModuleType moduleType)
		{
			switch (moduleType)
			{
			case IW7::PARTICLE_MODULE_INIT_ATLAS:
			case IW7::PARTICLE_MODULE_EMISSION_GRAPH:
			case IW7::PARTICLE_MODULE_INTENSITY_GRAPH:
			case IW7::PARTICLE_MODULE_PARENT_VELOCITY_GRAPH:
			case IW7::PARTICLE_MODULE_ROTATION_GRAPH:
				return 2;
				break;
			case IW7::PARTICLE_MODULE_INIT_FLARE:
			case IW7::PARTICLE_MODULE_INIT_SPAWN_SHAPE_SPHERE:
				return 4;
				break;
			case IW7::PARTICLE_MODULE_INIT_SPAWN:
				return 1;
				break;
			case IW7::PARTICLE_MODULE_INIT_SPAWN_SHAPE_BOX:
			case IW7::PARTICLE_MODULE_INIT_SPAWN_SHAPE_ELLIPSOID:
			case IW7::PARTICLE_MODULE_POSITION_GRAPH:
			case IW7::PARTICLE_MODULE_ROTATION_GRAPH_3D:
			case IW7::PARTICLE_MODULE_SIZE_GRAPH:
			case IW7::PARTICLE_MODULE_VELOCITY_GRAPH:
				return 6;
				break;
			case IW7::PARTICLE_MODULE_INIT_SPAWN_SHAPE_CYLINDER:
				return 5;
				break;
			case IW7::PARTICLE_MODULE_INIT_SPAWN_SHAPE_MESH:
				return 3;
				break;
			case IW7::PARTICLE_MODULE_COLOR_GRAPH:
				return 8;
				break;
			case IW7::PARTICLE_MODULE_EMISSIVE_GRAPH:
				return 10;
				break;
			default:
				return 0;
				break;
			}
			return 0;
		}

		// Stock vfx debris masses are 1-2; nothing stock is lighter than 1.
		constexpr auto FX_MODEL_PHYSICS_MASS = 1.0f;

		// Real physics bodies for FX_ELEM_USE_MODEL_PHYSICS model elements are OPT-IN
		// (ZT_FX_MODEL_PHYSICS=1). With the dynamic asset in place the pieces do become
		// Havok bodies, but two stock behaviours are still missing: the particle's launch
		// velocity is not handed to the body (chunks drop straight down) and the bodies do
		// not collide with the world the way clutter dynents do. Both need a stock vfx with
		// physics decoded first. Until then the ray-cast emulation stays the default.
		bool fx_model_physics_enabled()
		{
			const auto* env = std::getenv("ZT_FX_MODEL_PHYSICS");
			return env && env[0] == '1';
		}

		IW7::PARTICLE_ELEMENT_TYPE convert_elem_type(IW5::FxElemType type)
		{
			switch (type)
			{
			case IW5::FX_ELEM_TYPE_SPRITE_BILLBOARD:
				return IW7::PARTICLE_ELEMENT_TYPE_BILLBOARD_SPRITE;
				break;
			case IW5::FX_ELEM_TYPE_SPRITE_ORIENTED:
				return IW7::PARTICLE_ELEMENT_TYPE_ORIENTED_SPRITE;
				break;
			case IW5::FX_ELEM_TYPE_TAIL:
				return IW7::PARTICLE_ELEMENT_TYPE_TAIL;
				break;
			case IW5::FX_ELEM_TYPE_TRAIL:
				return IW7::PARTICLE_ELEMENT_TYPE_GEO_TRAIL;
				break;
			case IW5::FX_ELEM_TYPE_CLOUD:
				return IW7::PARTICLE_ELEMENT_TYPE_CLOUD;
				break;
			case IW5::FX_ELEM_TYPE_SPARKCLOUD:
				return IW7::PARTICLE_ELEMENT_TYPE_SPARK_CLOUD;
				break;
			case IW5::FX_ELEM_TYPE_SPARKFOUNTAIN:
				// no IW7 equivalent, is_elem_convertible skips these
				return IW7::PARTICLE_ELEMENT_TYPE_SPARK_CLOUD;
				break;
			case IW5::FX_ELEM_TYPE_MODEL:
				return IW7::PARTICLE_ELEMENT_TYPE_MODEL;
				break;
			case IW5::FX_ELEM_TYPE_OMNI_LIGHT:
				return IW7::PARTICLE_ELEMENT_TYPE_LIGHT_OMNI;
				break;
			case IW5::FX_ELEM_TYPE_SPOT_LIGHT:
				return IW7::PARTICLE_ELEMENT_TYPE_LIGHT_SPOT;
				break;
			case IW5::FX_ELEM_TYPE_SOUND:
				// IW7 has no sound element, stock plays sounds from an INIT_SOUND module on another type
				return IW7::PARTICLE_ELEMENT_TYPE_RUNNER;
				break;
			case IW5::FX_ELEM_TYPE_DECAL:
				return IW7::PARTICLE_ELEMENT_TYPE_DECAL;
				break;
			case IW5::FX_ELEM_TYPE_RUNNER:
				return IW7::PARTICLE_ELEMENT_TYPE_RUNNER;
				break;
			}

			return IW7::PARTICLE_ELEMENT_TYPE_BILLBOARD_SPRITE;
		}

		bool elem_uses_material(FxElemDef* elem)
		{
			switch (elem->elemType)
			{
			case FX_ELEM_TYPE_SPRITE_BILLBOARD:
			case FX_ELEM_TYPE_SPRITE_ORIENTED:
			case FX_ELEM_TYPE_TAIL:
			case FX_ELEM_TYPE_TRAIL:
			case FX_ELEM_TYPE_CLOUD:
			case FX_ELEM_TYPE_SPARKCLOUD:
				return true;
			default:
				return false;
			}
		}

		// IW7 draw and update code dereferences the element type module (and INIT_MATERIAL for material
		// elements) without null checks, so an element that can't provide them must not become an emitter
		bool is_elem_convertible(FxEffectDef* asset, FxElemDef* elem)
		{
			if (elem->elemType == FX_ELEM_TYPE_SPARKFOUNTAIN)
			{
				ZONETOOL_WARNING("Effect %s: spark fountain elements have no IW7 equivalent, skipping", asset->name);
				return false;
			}

			switch (elem->elemType)
			{
			case FX_ELEM_TYPE_OMNI_LIGHT:
			case FX_ELEM_TYPE_SPOT_LIGHT:
				return true;
			case FX_ELEM_TYPE_DECAL:
				return elem->visualCount && elem->visuals.markArray;
			default:
				break;
			}

			if (!elem->visualCount)
			{
				ZONETOOL_WARNING("Effect %s: element without visuals, skipping", asset->name);
				return false;
			}

			if (elem->visualCount == 1)
			{
				return elem->visuals.instance.anonymous != nullptr;
			}

			for (auto i = 0; i < elem->visualCount; i++)
			{
				if (!elem->visuals.array[i].anonymous)
				{
					ZONETOOL_WARNING("Effect %s: element with a null visual, skipping", asset->name);
					return false;
				}
			}

			return true;
		}

		namespace xoxor4d
		{
			enum SizeCurveType
			{
				Width = 0,
				Height = 1,
			};

			enum VelocityScaleType
			{
				Local,
				World,
			};

			enum VelocityDirectionType
			{
				Forward,
				Right,
				Up,
			};

			enum CurveSampleValueType
			{
				Base,
				Amplitude,
			};

			class MinMaxCurveSample
			{
			public:
				CurveSampleValueType MinType{};
				int MinIndex{};
				float MinComp{ FLT_MAX };

				CurveSampleValueType MaxType{};
				int MaxIndex{};
				float MaxComp{ -FLT_MAX };

				float GetAbsMax()
				{
					auto max = MaxComp;
					auto abs = std::abs(MinComp);

					if (abs > max)
					{
						max = abs;
					}

					return max;
				}
			};

			void GetMinMaxForSample(MinMaxCurveSample& sample, float compBase, float compAmpl, int index)
			{
				// min base
				if (compBase < sample.MinComp)
				{
					sample.MinIndex = index;
					sample.MinType = CurveSampleValueType::Base;
					sample.MinComp = compBase;
				}

				// max base
				if (compBase > sample.MaxComp)
				{
					sample.MaxIndex = index;
					sample.MaxType = CurveSampleValueType::Base;
					sample.MaxComp = compBase;
				}

				// min amplitude
				if (compAmpl < sample.MinComp)
				{
					sample.MinIndex = index;
					sample.MinType = CurveSampleValueType::Amplitude;
					sample.MinComp = compAmpl;
				}

				// max amplitude
				if (compAmpl > sample.MaxComp)
				{
					sample.MaxIndex = index;
					sample.MaxType = CurveSampleValueType::Amplitude;
					sample.MaxComp = compAmpl;
				}
			}

			float GetVelocityScale(VelocityScaleType type, VelocityDirectionType dir, MinMaxCurveSample& mmSample, float scalar)
			{
				auto min = mmSample.MinComp;
				auto max = mmSample.MaxComp;

				if (min != 0 || max != 0)
				{
					auto abs = std::abs(min);
					auto idx = mmSample.MinIndex;
					auto kind = mmSample.MinType;

					if (max > abs)
					{
						abs = max;
						idx = mmSample.MaxIndex;
						kind = mmSample.MaxType;
					}

					return abs / scalar * 2.0f;
				}

				return 0.0f;
			}

			float GetVelocityTotalDeltaValue(VelocityScaleType type, VelocityDirectionType dir, CurveSampleValueType kind, FxElemVelStateSample* samples, int index)
			{
				switch (type)
				{
				case VelocityScaleType::Local:
					switch (dir)
					{
					case VelocityDirectionType::Forward:
						switch (kind)
						{
						case CurveSampleValueType::Base: return samples[index].local.totalDelta.base[0];
						case CurveSampleValueType::Amplitude: return samples[index].local.totalDelta.amplitude[0];
						} break;

					case VelocityDirectionType::Right:
						switch (kind)
						{
						case CurveSampleValueType::Base: return samples[index].local.totalDelta.base[1];
						case CurveSampleValueType::Amplitude: return samples[index].local.totalDelta.amplitude[1];
						} break;

					case VelocityDirectionType::Up:
						switch (kind)
						{
						case CurveSampleValueType::Base: return samples[index].local.totalDelta.base[2];
						case CurveSampleValueType::Amplitude: return samples[index].local.totalDelta.amplitude[2];
						} break;
					}
					break;

				case VelocityScaleType::World:
					switch (dir)
					{
					case VelocityDirectionType::Forward:
						switch (kind)
						{
						case CurveSampleValueType::Base: return samples[index].world.totalDelta.base[0];
						case CurveSampleValueType::Amplitude: return samples[index].world.totalDelta.amplitude[0];
						} break;

					case VelocityDirectionType::Right:
						switch (kind)
						{
						case CurveSampleValueType::Base: return samples[index].world.totalDelta.base[1];
						case CurveSampleValueType::Amplitude: return samples[index].world.totalDelta.amplitude[1];
						} break;

					case VelocityDirectionType::Up:
						switch (kind)
						{
						case CurveSampleValueType::Base: return samples[index].world.totalDelta.base[2];
						case CurveSampleValueType::Amplitude: return samples[index].world.totalDelta.amplitude[2];
						} break;
					}
					break;
				}

				return 0.0f;
			}

			float GetVelocityValue(VelocityScaleType type, VelocityDirectionType dir, CurveSampleValueType kind, FxElemVelStateSample* samples, int index)
			{
				switch (type)
				{
				case VelocityScaleType::Local:
					switch (dir)
					{
					case VelocityDirectionType::Forward:
						switch (kind)
						{
						case CurveSampleValueType::Base: return samples[index].local.velocity.base[0];
						case CurveSampleValueType::Amplitude: return samples[index].local.velocity.amplitude[0];
						} break;

					case VelocityDirectionType::Right:
						switch (kind)
						{
						case CurveSampleValueType::Base: return samples[index].local.velocity.base[1];
						case CurveSampleValueType::Amplitude: return samples[index].local.velocity.amplitude[1];
						} break;

					case VelocityDirectionType::Up:
						switch (kind)
						{
						case CurveSampleValueType::Base: return samples[index].local.velocity.base[2];
						case CurveSampleValueType::Amplitude: return samples[index].local.velocity.amplitude[2];
						} break;
					}
					break;

				case VelocityScaleType::World:
					switch (dir)
					{
					case VelocityDirectionType::Forward:
						switch (kind)
						{
						case CurveSampleValueType::Base: return samples[index].world.velocity.base[0];
						case CurveSampleValueType::Amplitude: return samples[index].world.velocity.amplitude[0];
						} break;

					case VelocityDirectionType::Right:
						switch (kind)
						{
						case CurveSampleValueType::Base: return samples[index].world.velocity.base[1];
						case CurveSampleValueType::Amplitude: return samples[index].world.velocity.amplitude[1];
						} break;

					case VelocityDirectionType::Up:
						switch (kind)
						{
						case CurveSampleValueType::Base: return samples[index].world.velocity.base[2];
						case CurveSampleValueType::Amplitude: return samples[index].world.velocity.amplitude[2];
						} break;
					}
					break;
				}

				return 0.0f;
			}

			void GetLargestVelocitySampleValue(VelocityScaleType type, VelocityDirectionType dir, FxElemVelStateSample* velSamples, int index, MinMaxCurveSample& mmSample)
			{
				auto velBase = GetVelocityValue(type, dir, CurveSampleValueType::Base, velSamples, index);
				auto velAmp = GetVelocityValue(type, dir, CurveSampleValueType::Amplitude, velSamples, index);

				auto deltaBase = GetVelocityTotalDeltaValue(type, dir, CurveSampleValueType::Base, velSamples, index);
				auto deltaAmp = GetVelocityTotalDeltaValue(type, dir, CurveSampleValueType::Amplitude, velSamples, index);

				// min base
				if (velBase < mmSample.MinComp)
				{
					mmSample.MinIndex = index;
					mmSample.MinType = CurveSampleValueType::Base;
					mmSample.MinComp = velBase;
				}

				// max base
				if (velBase > mmSample.MaxComp)
				{
					mmSample.MaxIndex = index;
					mmSample.MaxType = CurveSampleValueType::Base;
					mmSample.MaxComp = velBase;
				}

				// min amplitude
				if (velAmp < mmSample.MinComp)
				{
					mmSample.MinIndex = index;
					mmSample.MinType = CurveSampleValueType::Amplitude;
					mmSample.MinComp = velAmp;
				}

				// max amplitude
				if (velAmp > mmSample.MaxComp)
				{
					mmSample.MaxIndex = index;
					mmSample.MaxType = CurveSampleValueType::Amplitude;
					mmSample.MaxComp = velAmp;
				}
			}

			void CalculateVelocityScales(float& lForward, float& lRight, float& lUp, float& wForward, float& wRight, float& wUp,
				FxElemVelStateSample* velSamples, unsigned int velSamplesCount, float sampleScalar)
			{
				MinMaxCurveSample localForward{};
				MinMaxCurveSample localRight{};
				MinMaxCurveSample localUp{};

				MinMaxCurveSample worldForward{};
				MinMaxCurveSample worldRight{};
				MinMaxCurveSample worldUp{};

				for (auto s = 0; s < velSamplesCount; s++)
				{
					GetLargestVelocitySampleValue(VelocityScaleType::Local, VelocityDirectionType::Forward, velSamples, s, localForward);
					GetLargestVelocitySampleValue(VelocityScaleType::Local, VelocityDirectionType::Right, velSamples, s, localRight);
					GetLargestVelocitySampleValue(VelocityScaleType::Local, VelocityDirectionType::Up, velSamples, s, localUp);
					GetLargestVelocitySampleValue(VelocityScaleType::World, VelocityDirectionType::Forward, velSamples, s, worldForward);
					GetLargestVelocitySampleValue(VelocityScaleType::World, VelocityDirectionType::Right, velSamples, s, worldRight);
					GetLargestVelocitySampleValue(VelocityScaleType::World, VelocityDirectionType::Up, velSamples, s, worldUp);
				}

				lForward = GetVelocityScale(VelocityScaleType::Local, VelocityDirectionType::Forward, localForward, sampleScalar);
				lRight = GetVelocityScale(VelocityScaleType::Local, VelocityDirectionType::Right, localRight, sampleScalar);
				lUp = GetVelocityScale(VelocityScaleType::Local, VelocityDirectionType::Up, localUp, sampleScalar);

				wForward = GetVelocityScale(VelocityScaleType::World, VelocityDirectionType::Forward, worldForward, sampleScalar);
				wRight = GetVelocityScale(VelocityScaleType::World, VelocityDirectionType::Right, worldRight, sampleScalar);
				wUp = GetVelocityScale(VelocityScaleType::World, VelocityDirectionType::Up, worldUp, sampleScalar);
			}
		}

		void calculate_inv_time_delta(IW7::ParticleCurveDef* curves, unsigned int curves_count)
		{
			for (auto i = 0u; i < curves_count; i++)
			{
				assert(curves[i].numControlPoints > 0);
				assert(curves[i].controlPoints[0].time == 0.0f);
				//assert(curves[i].controlPoints[curves[i].numControlPoints - 1].time == 1.0f);

				float prev_time = 0.0f;

				for (auto j = 1u; j < curves[i].numControlPoints; j++)
				{
					curves[i].controlPoints[j].invTimeDelta = 1.0f / (curves[i].controlPoints[j].time - prev_time);
					prev_time = curves[i].controlPoints[j].time;
				}
			}
		}

		void fixup_randomization_flags(IW7::ParticleCurveDef& curve_base, IW7::ParticleCurveDef& curve_ampl, unsigned int* flags)
		{
			if (curve_base.numControlPoints != curve_ampl.numControlPoints || curve_base.scale != curve_ampl.scale)
			{
				return;
			}

			assert(curve_base.numControlPoints == curve_ampl.numControlPoints);

			const auto num = curve_base.numControlPoints;
			for (auto i = 0u; i < num; i++)
			{
				if (curve_base.controlPoints[i].value != curve_ampl.controlPoints[i].value)
				{
					*flags |= IW7::PARTICLE_MODULE_FLAG_RANDOMIZE_BETWEEN_CURVES;
				}
			}
		}

		void set_default_size_values(IW7::ParticleCurveDef& curve)
		{
			curve.scale = 0.0f;
			assert(curve.numControlPoints == 2);

			curve.controlPoints[0].time = 0.0f;
			curve.controlPoints[0].value = 1.0f;

			curve.controlPoints[1].time = 1.0f;
			curve.controlPoints[1].value = 1.0f;
		}

		void set_default_velocity_values(IW7::ParticleCurveDef& curve)
		{
			curve.scale = 1.0f;
			assert(curve.numControlPoints == 2);

			curve.controlPoints[0].time = 0.0f;
			curve.controlPoints[0].value = 0.0f;

			curve.controlPoints[1].time = 1.0f;
			curve.controlPoints[1].value = 0.0f;
		}

		void fixup_geotrail_material(FxElemDef* def, Material* material)
		{
			if (def->elemType != FX_ELEM_TYPE_TRAIL) return;

			if (get_linker_mode() == linker_mode::iw3)
			{
				IW5::IW7Dumper::dump(reinterpret_cast<ZoneTool::IW5::Material*>(material), true);
			}
			else
			{
				ZONETOOL_WARNING("Material %s for GEO TRAIL needs manual fixup (eq->ev)...", material->name);
			}
		}

		void generate_color_module(FxElemDef* elem, allocator& allocator, std::vector<IW7::ParticleModuleDef>& modules)
		{
			if (!elem->visSamples)
			{
				return;
			}

			IW7::ParticleModuleDef module{};
			module.moduleType = IW7::PARTICLE_MODULE_COLOR_GRAPH;
			auto& moduleData = module.moduleData.colorGraph;
			moduleData.type = module.moduleType;
			moduleData.m_flags = 0;

			moduleData.firstCurve = false;
			moduleData.m_modulateColorByAlpha = false; // editorflag

			auto sampleCount = elem->visStateIntervalCount + 1;
			auto sampleSize = 1.0f / (sampleCount - 1);

			if (!sampleCount)
			{
				__debugbreak();
			}

			for (auto i = 0; i < 8; i++)
			{
				moduleData.m_curves[i].numControlPoints = sampleCount;
				moduleData.m_curves[i].controlPoints = allocator.allocate<IW7::ParticleCurveControlPointDef>(sampleCount);

				moduleData.m_curves[i].scale = 1.0f;
			}

			// bgra->rgba?
			for (auto i = 0; i < sampleCount; i++)
			{
				[[maybe_unused]] float base_color[4] = {
				elem->visSamples[i].base.color[0] / 255.0f,
				elem->visSamples[i].base.color[1] / 255.0f,
				elem->visSamples[i].base.color[2] / 255.0f,
				elem->visSamples[i].base.color[3] / 255.0f,
				};

				float base_color_unpacked[4]{};
				Byte4::Byte4UnpackRgba(base_color_unpacked, elem->visSamples[i].base.color);

				moduleData.m_curves[0].controlPoints[i].value = base_color_unpacked[2];
				moduleData.m_curves[1].controlPoints[i].value = base_color_unpacked[1];
				moduleData.m_curves[2].controlPoints[i].value = base_color_unpacked[0];
				moduleData.m_curves[3].controlPoints[i].value = base_color_unpacked[3];

				[[maybe_unused]] float ampl_color[4] = {
				elem->visSamples[i].amplitude.color[0] / 255.0f,
				elem->visSamples[i].amplitude.color[1] / 255.0f,
				elem->visSamples[i].amplitude.color[2] / 255.0f,
				elem->visSamples[i].amplitude.color[3] / 255.0f,
				};

				float ampl_color_unpacked[4]{};
				Byte4::Byte4UnpackRgba(ampl_color_unpacked, elem->visSamples[i].amplitude.color);

				moduleData.m_curves[4].controlPoints[i].value = ampl_color_unpacked[2];
				moduleData.m_curves[5].controlPoints[i].value = ampl_color_unpacked[1];
				moduleData.m_curves[6].controlPoints[i].value = ampl_color_unpacked[0];
				moduleData.m_curves[7].controlPoints[i].value = ampl_color_unpacked[3];

				for (auto j = 0; j < 8; j++)
				{
					moduleData.m_curves[j].controlPoints[i].time = sampleSize * i;
				}
			}

			calculate_inv_time_delta(moduleData.m_curves, GetModuleNumCurves(module.moduleType));

			fixup_randomization_flags(moduleData.m_curves[0], moduleData.m_curves[4], &moduleData.m_flags);
			fixup_randomization_flags(moduleData.m_curves[1], moduleData.m_curves[5], &moduleData.m_flags);
			fixup_randomization_flags(moduleData.m_curves[2], moduleData.m_curves[6], &moduleData.m_flags);
			fixup_randomization_flags(moduleData.m_curves[3], moduleData.m_curves[7], &moduleData.m_flags);

			state_flags |= IW7::PARTICLE_STATE_DEF_FLAG_HAS_COLOR;

			modules.push_back(module);
		}

		void generate_size_module(FxElemDef* elem, allocator& allocator, std::vector<IW7::ParticleModuleDef>& modules)
		{
			if (!elem->visSamples)
			{
				return;
			}

			// IW7 size graph is a vector: curves 0-2 are x/y/z, curves 3-5 the second set for randomization.
			// Which IW5 channel feeds which axis follows FX_GetVisualSampleRouting and stock usage:
			// sprites/tails/trails/clouds use size[0]/size[1] as x/y, decals are square (size[0] only),
			// lights pass x as radius and y as intensity to R_AddOmniLightToScene (IW5 scale, 1 when unused),
			// and models only have scale (stock models fill x or all of xyz).
			enum class channel { none, size0, size1, scale, scale_or_one };
			channel axes[3] = { channel::none, channel::none, channel::none };

			switch (elem->elemType)
			{
			case FX_ELEM_TYPE_SPRITE_BILLBOARD:
			case FX_ELEM_TYPE_SPRITE_ORIENTED:
			case FX_ELEM_TYPE_TAIL:
			case FX_ELEM_TYPE_TRAIL:
			case FX_ELEM_TYPE_CLOUD:
			case FX_ELEM_TYPE_SPARKCLOUD:
				axes[0] = channel::size0;
				axes[1] = channel::size1;
				break;
			case FX_ELEM_TYPE_DECAL:
				axes[0] = channel::size0;
				axes[1] = channel::size0;
				break;
			case FX_ELEM_TYPE_OMNI_LIGHT:
			case FX_ELEM_TYPE_SPOT_LIGHT:
				axes[0] = channel::size0;
				axes[1] = channel::scale_or_one;
				break;
			case FX_ELEM_TYPE_MODEL:
				axes[0] = channel::scale;
				axes[1] = channel::scale;
				axes[2] = channel::scale;
				break;
			default:
				return;
			}

			const auto sample_count = elem->visStateIntervalCount + 1;
			const auto sample_size = 1.0f / (sample_count - 1);

			bool scale_unused = true;
			for (auto s = 0; s < sample_count; s++)
			{
				if (elem->visSamples[s].base.scale != 0.0f || elem->visSamples[s].amplitude.scale != 0.0f)
				{
					scale_unused = false;
					break;
				}
			}

			const auto get_value = [&](channel ch, int sample, bool amplitude) -> float
			{
				const auto& vis = amplitude ? elem->visSamples[sample].amplitude : elem->visSamples[sample].base;
				switch (ch)
				{
				case channel::size0: return vis.size[0];
				case channel::size1: return vis.size[1];
				case channel::scale: return vis.scale;
				case channel::scale_or_one: return scale_unused ? (amplitude ? 0.0f : 1.0f) : vis.scale;
				default: return 0.0f;
				}
			};

			const auto get_curve_scale = [&](channel ch) -> float
			{
				if (ch == channel::none)
				{
					return 0.0f;
				}

				xoxor4d::MinMaxCurveSample sample{};
				for (auto s = 0; s < sample_count; s++)
				{
					xoxor4d::GetMinMaxForSample(sample, get_value(ch, s, false), get_value(ch, s, false) + get_value(ch, s, true), s);
				}
				return sample.GetAbsMax();
			};

			float scales[3]{};
			for (auto axis = 0; axis < 3; axis++)
			{
				scales[axis] = get_curve_scale(axes[axis]);
			}

			if (!scales[0] && !scales[1] && !scales[2])
			{
				return;
			}

			IW7::ParticleModuleDef module{};
			module.moduleType = IW7::PARTICLE_MODULE_SIZE_GRAPH;
			auto& moduleData = module.moduleData.sizeGraph;
			moduleData.type = module.moduleType;
			moduleData.m_flags = 0;

			moduleData.firstCurve = false;

			for (auto axis = 0; axis < 3; axis++)
			{
				auto& curve0 = moduleData.m_curves[axis];
				auto& curve1 = moduleData.m_curves[axis + 3];

				if (!scales[axis])
				{
					for (auto* curve : { &curve0, &curve1 })
					{
						curve->numControlPoints = 2;
						curve->controlPoints = allocator.allocate<IW7::ParticleCurveControlPointDef>(2);
						set_default_size_values(*curve);
					}
					continue;
				}

				for (auto* curve : { &curve0, &curve1 })
				{
					curve->scale = scales[axis];
					curve->numControlPoints = sample_count;
					curve->controlPoints = allocator.allocate<IW7::ParticleCurveControlPointDef>(sample_count);
				}

				for (auto i = 0; i < sample_count; i++)
				{
					// IW5 stores the second curve as a delta from the first
					const auto base = get_value(axes[axis], i, false);
					const auto ampl = get_value(axes[axis], i, true);

					curve0.controlPoints[i].time = sample_size * i;
					curve0.controlPoints[i].value = base / scales[axis];

					curve1.controlPoints[i].time = sample_size * i;
					curve1.controlPoints[i].value = (base + ampl) / scales[axis];
				}

				fixup_randomization_flags(curve0, curve1, &moduleData.m_flags);
			}

			calculate_inv_time_delta(moduleData.m_curves, GetModuleNumCurves(module.moduleType));

			modules.push_back(module);
		}

		void generate_rotation_module(FxElemDef* elem, allocator& allocator, std::vector<IW7::ParticleModuleDef>& modules)
		{
			if (!elem->visSamples)
			{
				return;
			}

			IW7::ParticleModuleDef module{};
			module.moduleType = IW7::PARTICLE_MODULE_ROTATION_GRAPH;
			auto& moduleData = module.moduleData.rotationGraph;
			moduleData.type = module.moduleType;
			moduleData.m_flags = 0;

			moduleData.m_useRotationRate = true;

			auto visSamplesCount = elem->visStateIntervalCount + 1;
			float rotationScale = 0.0f;

			// #
			// rotation bruh
			// => curve scale = largest :: getting the largest delta rotation (incl. abs(most negative))
			// => curve scale = (largest / 0.017453292f) * (sampleCount - 1) * 1000 * 2
			// => key values = (value / 0.017453292f) * ((sampleCount - 1) * 1000) / scale

			//{
				xoxor4d::MinMaxCurveSample rotation{};

				// find largest value (pos and neg)
				for (auto s = 0; s < visSamplesCount; s++)
				{
					auto rotBase = elem->visSamples[s].base.rotationDelta;
					auto rotAmpl = elem->visSamples[s].amplitude.rotationDelta;

					xoxor4d::GetMinMaxForSample(rotation, rotBase, rotAmpl, s);
				}

				// ... would take avarage here (needed?)
				// const float rotationScale = (edElemDef->rotationScale * 0.017453292f) / (elemDef->visStateIntervalCount * 1000.0f);
				rotationScale = rotation.GetAbsMax() * (visSamplesCount - 1) * 1000.0f * 2.0f;
			//}

			if (!rotationScale)
			{
				return;
			}

			auto sampleSize = 1.0f / (visSamplesCount - 1);
			auto sampleScalar = (visSamplesCount - 1) * 1000.0f;

			for (auto i = 0; i < 2; i++)
			{
				moduleData.m_curves[i].numControlPoints = visSamplesCount;
				moduleData.m_curves[i].controlPoints = allocator.allocate<IW7::ParticleCurveControlPointDef>(visSamplesCount);

				moduleData.m_curves[i].scale = rotationScale;
			}

			for (auto i = 0; i < visSamplesCount; i++)
			{
				auto keyValue = elem->visSamples[i].base.rotationDelta * sampleScalar / rotationScale;
				moduleData.m_curves[0].controlPoints[i].value = keyValue;
				moduleData.m_curves[0].controlPoints[i].time = sampleSize * i;
				
				auto baseVel = elem->visSamples[i].base.rotationDelta * sampleScalar / rotationScale;
				auto amplVel = elem->visSamples[i].amplitude.rotationDelta * sampleScalar / rotationScale;
				moduleData.m_curves[1].controlPoints[i].value = baseVel + amplVel;
				moduleData.m_curves[1].controlPoints[i].time = sampleSize * i;
			}

			calculate_inv_time_delta(moduleData.m_curves, GetModuleNumCurves(module.moduleType));

			fixup_randomization_flags(moduleData.m_curves[0], moduleData.m_curves[1], &moduleData.m_flags);

			state_flags |= IW7::PARTICLE_STATE_DEF_FLAG_HAS_ROTATION_1D_CURVE;

			modules.push_back(module);
		}

		void generate_init_relative_velocity_module(FxElemDef* elem, allocator& allocator, std::vector<IW7::ParticleModuleDef>& modules)
		{
			//if ((elem->flags & FX_ELEM_RUN_RELATIVE_TO_EFFECT) != 0) return;

			IW7::ParticleModuleDef module{};
			module.moduleType = IW7::PARTICLE_MODULE_INIT_RELATIVE_VELOCITY;
			auto& moduleData = module.moduleData.initRelativeVelocity;
			moduleData.type = module.moduleType;
			moduleData.m_flags = 0;

			moduleData.m_useBoltInfo = false;

			switch (elem->flags & FX_ELEM_RUN_MASK)
			{
			case FX_ELEM_RUN_RELATIVE_TO_WORLD:
				moduleData.m_velocityType = IW7::PARTICLE_RELATIVE_VELOCITY_TYPE_WORLD;
				moduleData.m_useBoltInfo = false;
				break;
			case FX_ELEM_RUN_RELATIVE_TO_SPAWN:
				moduleData.m_velocityType = IW7::PARTICLE_RELATIVE_VELOCITY_TYPE_LOCAL; // PARTICLE_RELATIVE_VELOCITY_TYPE_RELATIVE_TO_EFFECT_ORIGIN is just FUCKED?!
				moduleData.m_useBoltInfo = false;
				break;
			case FX_ELEM_RUN_RELATIVE_TO_EFFECT:
				moduleData.m_velocityType = IW7::PARTICLE_RELATIVE_VELOCITY_TYPE_LOCAL;
				moduleData.m_useBoltInfo = false;
				break;
			case FX_ELEM_RUN_RELATIVE_TO_OFFSET:
				moduleData.m_velocityType = IW7::PARTICLE_RELATIVE_VELOCITY_TYPE_RELATIVE_TO_EFFECT_ORIGIN; // idk
				moduleData.m_useBoltInfo = false;
				break;
			default:
				__debugbreak();
				break;
			}

			if (elem->elemType == FX_ELEM_TYPE_TRAIL)
			{
				moduleData.m_velocityType = IW7::PARTICLE_RELATIVE_VELOCITY_TYPE_LOCAL_WITH_BOLT_INFO;
				moduleData.m_useBoltInfo = true;
			}

			modules.push_back(module);
		}

		void generate_velocity_module(FxElemDef* elem, allocator& allocator, std::vector<IW7::ParticleModuleDef>& modules)
		{
			if (!elem->velSamples)
			{
				return;
			}

			IW7::ParticleModuleDef module{};
			module.moduleType = IW7::PARTICLE_MODULE_VELOCITY_GRAPH;
			auto& moduleData = module.moduleData.velocityGraph;
			moduleData.type = module.moduleType;
			moduleData.m_flags = 0;

			moduleData.m_velocityBegin.v[0] = 0.0f;
			moduleData.m_velocityBegin.v[1] = 0.0f;
			moduleData.m_velocityBegin.v[2] = 0.0f;
			moduleData.m_velocityBegin.v[3] = 0.0f;

			moduleData.m_velocityEnd.v[0] = 0.0f;
			moduleData.m_velocityEnd.v[1] = 0.0f;
			moduleData.m_velocityEnd.v[2] = 0.0f;
			moduleData.m_velocityEnd.v[3] = 0.0f;

			auto sampleCount = elem->velIntervalCount + 1;
			auto sampleSize = 1.0f / (sampleCount - 1);
			auto sampleScalar = 1.0f / ((sampleCount - 1) * 1000.0f);

			auto lForwardScale = 0.0f;
			auto lRightScale = 0.0f;
			auto lUpScale = 0.0f;

			auto wForwardScale = 0.0f;
			auto wRightScale = 0.0f;
			auto wUpScale = 0.0f;

			xoxor4d::CalculateVelocityScales(lForwardScale, lRightScale, lUpScale,
				wForwardScale, wRightScale, wUpScale,
				elem->velSamples, sampleCount, sampleScalar);

			if (!lForwardScale && !lRightScale && !lUpScale && !wForwardScale && !wRightScale && !wUpScale)
			{
				return;
			}

			bool local = (elem->flags & FX_ELEM_HAS_VELOCITY_GRAPH_LOCAL) != 0;
			bool world = (elem->flags & FX_ELEM_HAS_VELOCITY_GRAPH_WORLD) != 0;

			if (!local && !world)
			{
				// FX_SampleVelocityInFrame only sets these when the graph moves the particle
				return;
			}

			if (local && world)
			{
				// a velocity graph is either local or world space; keep the frame that moves the particle further
				// instead of dropping the motion entirely
				const auto& last = elem->velSamples[sampleCount - 1];
				const auto length_sq = [](const float* v) { return v[0] * v[0] + v[1] * v[1] + v[2] * v[2]; };
				const auto local_travel = length_sq(last.local.totalDelta.base);
				const auto world_travel = length_sq(last.world.totalDelta.base);
				local = local_travel >= world_travel;
				world = !local;
			}

			moduleData.m_flags |= world ? IW7::PARTICLE_MODULE_FLAG_USE_WORLD_SPACE : 0;

			enum module_velocity_curve_e
			{
				forward0,
				right0,
				up0,
				forward1,
				right1,
				up1,
			};

			const auto allocate = [&](module_velocity_curve_e type, unsigned int num)
			{
				moduleData.m_curves[type].numControlPoints = num;
				moduleData.m_curves[type].controlPoints = allocator.allocate<IW7::ParticleCurveControlPointDef>(num);
				if (num == 2)
				{
					set_default_velocity_values(moduleData.m_curves[type]);
				}
			};

			const auto do_alloc = [&](module_velocity_curve_e type, float scale)
			{
				if (scale)
				{
					allocate(type, sampleCount);
					moduleData.m_curves[type].scale = scale;
				}
				else
				{
					allocate(type, 2);
				}
			};

			if (local)
			{
				do_alloc(module_velocity_curve_e::forward0, lForwardScale);
				do_alloc(module_velocity_curve_e::right0, lRightScale);
				do_alloc(module_velocity_curve_e::up0, lUpScale);
				do_alloc(module_velocity_curve_e::forward1, lForwardScale);
				do_alloc(module_velocity_curve_e::right1, lRightScale);
				do_alloc(module_velocity_curve_e::up1, lUpScale);
			}
			else if (world)
			{
				do_alloc(module_velocity_curve_e::forward0, wForwardScale);
				do_alloc(module_velocity_curve_e::right0, wRightScale);
				do_alloc(module_velocity_curve_e::up0, wUpScale);
				do_alloc(module_velocity_curve_e::forward1, wForwardScale);
				do_alloc(module_velocity_curve_e::right1, wRightScale);
				do_alloc(module_velocity_curve_e::up1, wUpScale);
			}
			else
			{
				__debugbreak();
			}

			for (auto i = 0; i < sampleCount; i++)
			{
				const auto set_base = [&](module_velocity_curve_e curve_type, xoxor4d::VelocityDirectionType dir, float lScale, float wScale)
				{
					const auto stype = local ? xoxor4d::VelocityScaleType::Local : xoxor4d::VelocityScaleType::World;
					const auto scale = local ? lScale : wScale;

					if (!scale) return;

					auto baseVel = xoxor4d::GetVelocityValue(stype,  dir, xoxor4d::CurveSampleValueType::Base, elem->velSamples, i) / sampleScalar / scale;
					moduleData.m_curves[curve_type].controlPoints[i].value = baseVel;

					moduleData.m_curves[curve_type].controlPoints[i].time = sampleSize * i;
				};

				const auto set_ampl = [&](module_velocity_curve_e curve_type, xoxor4d::VelocityDirectionType dir, float lScale, float wScale)
				{
					const auto stype = local ? xoxor4d::VelocityScaleType::Local : xoxor4d::VelocityScaleType::World;
					const auto scale = local ? lScale : wScale;

					if (!scale) return;

					auto baseVel = xoxor4d::GetVelocityValue(stype, dir, xoxor4d::CurveSampleValueType::Base, elem->velSamples, i) / sampleScalar / scale;
					auto amplVel = xoxor4d::GetVelocityValue(stype, dir, xoxor4d::CurveSampleValueType::Amplitude, elem->velSamples, i) / sampleScalar / scale;
					moduleData.m_curves[curve_type].controlPoints[i].value = baseVel + amplVel;

					moduleData.m_curves[curve_type].controlPoints[i].time = sampleSize * i;
				};

				set_base(module_velocity_curve_e::forward0, xoxor4d::VelocityDirectionType::Forward, lForwardScale, wForwardScale);
				set_base(module_velocity_curve_e::right0, xoxor4d::VelocityDirectionType::Right, lRightScale, wRightScale);
				set_base(module_velocity_curve_e::up0, xoxor4d::VelocityDirectionType::Up, lUpScale, wUpScale);

				set_ampl(module_velocity_curve_e::forward1, xoxor4d::VelocityDirectionType::Forward, lForwardScale, wForwardScale);
				set_ampl(module_velocity_curve_e::right1, xoxor4d::VelocityDirectionType::Right, lRightScale, wRightScale);
				set_ampl(module_velocity_curve_e::up1, xoxor4d::VelocityDirectionType::Up, lUpScale, wUpScale);
			}

			calculate_inv_time_delta(moduleData.m_curves, GetModuleNumCurves(module.moduleType));

			fixup_randomization_flags(moduleData.m_curves[module_velocity_curve_e::forward0], moduleData.m_curves[module_velocity_curve_e::forward1], &moduleData.m_flags);
			fixup_randomization_flags(moduleData.m_curves[module_velocity_curve_e::right0], moduleData.m_curves[module_velocity_curve_e::right1], &moduleData.m_flags);
			fixup_randomization_flags(moduleData.m_curves[module_velocity_curve_e::up0], moduleData.m_curves[module_velocity_curve_e::up1], &moduleData.m_flags);

			state_flags |= world ? IW7::PARTICLE_STATE_DEF_FLAG_HAS_VELOCITY_CURVE_WORLD : IW7::PARTICLE_STATE_DEF_FLAG_HAS_VELOCITY_CURVE_LOCAL;

			modules.push_back(module);
		}

		void generate_gravity_module(FxElemDef* elem, allocator& allocator, std::vector<IW7::ParticleModuleDef>& modules)
		{
			if (elem->gravity.base == 0.0f && elem->gravity.amplitude == 0.0f)
			{
				return;
			}

			IW7::ParticleModuleDef module{};
			module.moduleType = IW7::PARTICLE_MODULE_GRAVITY;
			auto& moduleData = module.moduleData.gravity;
			moduleData.type = module.moduleType;
			moduleData.m_flags = 0;

			moduleData.m_gravityPercentage.min = elem->gravity.base;
			moduleData.m_gravityPercentage.max = elem->gravity.base + elem->gravity.amplitude;

			modules.push_back(module);
		}

		void generate_position_module(FxElemDef* elem, allocator& allocator, std::vector<IW7::ParticleModuleDef>& modules)
		{
			/*IW7::ParticleModuleDef module{};
			module.moduleType = IW7::PARTICLE_MODULE_POSITION_GRAPH;
			auto& moduleData = module.moduleData.positionGraph;
			moduleData.type = module.moduleType;
			moduleData.m_flags = 0;

			modules.push_back(module);*/
		}

		void generate_init_spawn_module(FxElemDef* elem, allocator& allocator, std::vector<IW7::ParticleModuleDef>& modules)
		{
			IW7::ParticleModuleDef module{};
			module.moduleType = IW7::PARTICLE_MODULE_INIT_SPAWN;
			auto& moduleData = module.moduleData.initSpawn;
			moduleData.type = module.moduleType;
			moduleData.m_flags = 0;

			// idk what this is supposed to be

			moduleData.m_curves->scale = 1.0f;
			moduleData.m_curves->numControlPoints = 3;
			moduleData.m_curves->controlPoints = allocator.allocate<IW7::ParticleCurveControlPointDef>(moduleData.m_curves->numControlPoints);
			moduleData.m_curves->controlPoints[0].value = 1.0f;
			moduleData.m_curves->controlPoints[1].value = 1.0f;
			moduleData.m_curves->controlPoints[2].value = 0.0f;

			moduleData.m_curves->controlPoints[0].time = 0.0f;
			moduleData.m_curves->controlPoints[1].time = 0.75f;
			moduleData.m_curves->controlPoints[2].time = 1.0f;

			calculate_inv_time_delta(moduleData.m_curves, GetModuleNumCurves(module.moduleType));

			modules.push_back(module);
		}

		void generate_init_attributes_module(FxElemDef* elem, allocator& allocator, std::vector<IW7::ParticleModuleDef>& modules)
		{
			if (elem->elemType == FX_ELEM_TYPE_TAIL)
			{
				//return;
			}

			IW7::ParticleModuleDef module{};
			module.moduleType = IW7::PARTICLE_MODULE_INIT_ATTRIBUTES;
			auto& moduleData = module.moduleData.initAttributes;
			moduleData.type = module.moduleType;
			moduleData.m_flags = 0;

			moduleData.m_useNonUniformInterpolationForColor = false;
			moduleData.m_useNonUniformInterpolationForSize = (elem->flags & FX_ELEM_NONUNIFORM_SCALE) != 0;

			moduleData.m_sizeMin.v[0] = 10.0f;
			moduleData.m_sizeMin.v[1] = 10.0f;
			moduleData.m_sizeMin.v[2] = 10.0f;
			moduleData.m_sizeMin.v[3] = 0.0f;
			moduleData.m_sizeMax.v[0] = 10.0f;
			moduleData.m_sizeMax.v[1] = 10.0f;
			moduleData.m_sizeMax.v[2] = 10.0f;
			moduleData.m_sizeMax.v[3] = 0.0f;

			moduleData.m_colorMin.v[0] = 1.0f;
			moduleData.m_colorMin.v[1] = 1.0f;
			moduleData.m_colorMin.v[2] = 1.0f;
			moduleData.m_colorMin.v[3] = 1.0f;
			moduleData.m_colorMax.v[0] = 1.0f;
			moduleData.m_colorMax.v[1] = 1.0f;
			moduleData.m_colorMax.v[2] = 1.0f;
			moduleData.m_colorMax.v[3] = 1.0f;

			moduleData.m_velocityMin.v[0] = 0.0f;
			moduleData.m_velocityMin.v[1] = 0.0f;
			moduleData.m_velocityMin.v[2] = 0.0f;
			moduleData.m_velocityMin.v[3] = 0.0f;
			moduleData.m_velocityMax.v[0] = 0.0f;
			moduleData.m_velocityMax.v[1] = 0.0f;
			moduleData.m_velocityMax.v[2] = 0.0f;
			moduleData.m_velocityMax.v[3] = 0.0f;

			modules.push_back(module);
		}

		void generate_init_rotation3d_module(FxElemDef* elem, allocator& allocator, std::vector<IW7::ParticleModuleDef>& modules)
		{
			const auto is_zero = [](const FxFloatRange& range) { return range.base == 0.0f && range.amplitude == 0.0f; };
			const bool has_angles = !is_zero(elem->spawnAngles[0]) || !is_zero(elem->spawnAngles[1]) || !is_zero(elem->spawnAngles[2]);
			const bool has_rate = !is_zero(elem->angularVelocity[0]) || !is_zero(elem->angularVelocity[1]) || !is_zero(elem->angularVelocity[2]);

			// stock clouds always carry this module
			const bool is_cloud = elem->elemType == FX_ELEM_TYPE_CLOUD || elem->elemType == FX_ELEM_TYPE_SPARKCLOUD;
			if (!has_angles && !has_rate && !is_cloud)
			{
				return;
			}

			IW7::ParticleModuleDef module{};
			module.moduleType = IW7::PARTICLE_MODULE_INIT_ROTATION_3D;
			auto& moduleData = module.moduleData.initRotation3D;
			moduleData.type = module.moduleType;
			moduleData.m_flags = 0;

			// IW5 stores angular velocity in rad/ms (FX_ConvertElemDef scales degrees by 0.000017453292),
			// IW7 rates are rad/s (stock values like 8.73 = 500 deg/s). spawn angles are radians in both
			for (auto i = 0; i < 3; i++)
			{
				moduleData.m_rotationRateMin.v[i] = elem->angularVelocity[i].base * 1000.0f;
				moduleData.m_rotationRateMax.v[i] = (elem->angularVelocity[i].base + elem->angularVelocity[i].amplitude) * 1000.0f;
			}
			moduleData.m_rotationRateMin.v[3] = 0.0f;
			moduleData.m_rotationRateMax.v[3] = 0.0f;

			moduleData.m_rotationAngleMin.v[0] = elem->spawnAngles[0].base;
			moduleData.m_rotationAngleMin.v[1] = elem->spawnAngles[1].base;
			moduleData.m_rotationAngleMin.v[2] = elem->spawnAngles[2].base;
			moduleData.m_rotationAngleMin.v[3] = 0.0f;
			moduleData.m_rotationAngleMax.v[0] = elem->spawnAngles[0].base + elem->spawnAngles[0].amplitude;
			moduleData.m_rotationAngleMax.v[1] = elem->spawnAngles[1].base + elem->spawnAngles[1].amplitude;
			moduleData.m_rotationAngleMax.v[2] = elem->spawnAngles[2].base + elem->spawnAngles[2].amplitude;
			moduleData.m_rotationAngleMax.v[3] = 0.0f;

			state_flags |= IW7::PARTICLE_STATE_DEF_FLAG_HAS_ROTATION_3D_INIT;

			modules.push_back(module);
		}

		void generate_init_rotation_module(FxElemDef* elem, allocator& allocator, std::vector<IW7::ParticleModuleDef>& modules)
		{
			if (elem->initialRotation.base == 0.0f && elem->initialRotation.amplitude == 0.0f)
			{
				return;
			}

			IW7::ParticleModuleDef module{};
			module.moduleType = IW7::PARTICLE_MODULE_INIT_ROTATION;
			auto& moduleData = module.moduleData.initRotation;
			moduleData.type = module.moduleType;
			moduleData.m_flags = 0;

			moduleData.m_rotationAngle.min = elem->initialRotation.base;
			moduleData.m_rotationAngle.max = elem->initialRotation.base + elem->initialRotation.amplitude;

			moduleData.m_rotationRate.min = 0.0f;
			moduleData.m_rotationRate.max = 0.0f;

			state_flags |= IW7::PARTICLE_STATE_DEF_FLAG_HAS_ROTATION_1D_INIT;

			modules.push_back(module);
		}

		void generate_init_atlas_module(FxElemDef* elem, allocator& allocator, std::vector<IW7::ParticleModuleDef>& modules)
		{
			if (elem->elemType != FX_ELEM_TYPE_SPRITE_BILLBOARD && 
				elem->elemType != FX_ELEM_TYPE_SPRITE_ORIENTED && 
				elem->elemType != FX_ELEM_TYPE_TAIL && 
				elem->elemType != FX_ELEM_TYPE_TRAIL &&
				elem->elemType != FX_ELEM_TYPE_CLOUD)
			{
				return;
			}

			if (elem->atlas.fps == 0.0f || elem->atlas.entryCount - 1 == 0 || elem->atlas.loopCount == 0)
			{
				//return;
			}

			IW7::ParticleModuleDef module{};
			module.moduleType = IW7::PARTICLE_MODULE_INIT_ATLAS;
			auto& moduleData = module.moduleData.initAtlas;
			moduleData.type = module.moduleType;
			moduleData.m_flags = 0;

			// stock m_startFrame values are frame counts minus one (3, 7, 15, 63) and 0 otherwise, which reads as
			// "random start within [0, m_startFrame]"; IW7 has no fixed non-zero start index
			const auto start = elem->atlas.behavior & FX_ATLAS_START_MASK;
			module.moduleData.initAtlas.m_startFrame = start == FX_ATLAS_START_RANDOM ? elem->atlas.entryCount - 1 : 0;

			// IW5 loopCount is the editor value + 1 (FX_ConvertAtlas) and only means something with
			// FX_ATLAS_LOOP_ONLY_N_TIMES, IW7 uses -1 for endless looping
			module.moduleData.initAtlas.m_loopCount = (elem->atlas.behavior & FX_ATLAS_LOOP_ONLY_N_TIMES) != 0 ? elem->atlas.loopCount : -1;

			// FX_ATLAS_PLAY_OVER_LIFE has no confirmed IW7 encoding yet, fps is kept either way
			module.moduleData.initAtlas.m_playRate = elem->atlas.fps;

			modules.push_back(module);
		}

		void generate_init_decal_module(FxElemDef* elem, allocator& allocator, std::vector<IW7::ParticleModuleDef>& modules)
		{
			if (elem->elemType != FX_ELEM_TYPE_DECAL)
			{
				return;
			}

			IW7::ParticleModuleDef module{};
			module.moduleType = IW7::PARTICLE_MODULE_INIT_DECAL;
			auto& moduleData = module.moduleData.initDecal;
			moduleData.type = module.moduleType;
			moduleData.m_flags = 0;

			// IW5 fadeInRange/fadeOutRange are camera distances, not times; stock decals mostly use 0
			moduleData.m_fadeInTime = 0;
			moduleData.m_fadeOutTime = 0;
			moduleData.m_stoppableFadeOutTime = 0;
			moduleData.m_lerpWaitTime = 1280;
			moduleData.m_lerpColor.v[0] = 1.0f;
			moduleData.m_lerpColor.v[1] = 1.0f;
			moduleData.m_lerpColor.v[2] = 1.0f;
			moduleData.m_lerpColor.v[3] = 1.0f;

			if (elem->visuals.markArray)
			{
				moduleData.m_linkedAssetList.numAssets = elem->visualCount;
				moduleData.m_linkedAssetList.assetList = allocator.allocate<IW7::ParticleLinkedAssetDef>(moduleData.m_linkedAssetList.numAssets);

				for (int idx = 0; idx < moduleData.m_linkedAssetList.numAssets; idx++)
				{
					if (elem->visuals.markArray[idx].materials[0])
					{
						// mim
						moduleData.m_linkedAssetList.assetList[idx].decal.materials[0] = allocator.manual_allocate<IW7::Material>(8);
						moduleData.m_linkedAssetList.assetList[idx].decal.materials[0]->name = allocator.duplicate_string(
							IW7::resolve_material_name(elem->visuals.markArray[idx].materials[0]->name));
					}
					if (elem->visuals.markArray[idx].materials[1])
					{
						// wim
						moduleData.m_linkedAssetList.assetList[idx].decal.materials[1] = allocator.manual_allocate<IW7::Material>(8);
						moduleData.m_linkedAssetList.assetList[idx].decal.materials[1]->name = allocator.duplicate_string(
							IW7::resolve_material_name(elem->visuals.markArray[idx].materials[1]->name));

						// wim autodisplacement
						moduleData.m_linkedAssetList.assetList[idx].decal.materials[2] = allocator.manual_allocate<IW7::Material>(8);
						moduleData.m_linkedAssetList.assetList[idx].decal.materials[2]->name = allocator.duplicate_string(
							IW7::resolve_material_name(elem->visuals.markArray[idx].materials[1]->name));
					}
				}

				//moduleData.m_flags |= IW7::PARTICLE_MODULE_FLAG_HAS_ASSETS;
			}

			modules.push_back(module);
		}

		void generate_init_model_module(FxElemDef* elem, allocator& allocator, std::vector<IW7::ParticleModuleDef>& modules)
		{
			if (elem->elemType != FX_ELEM_TYPE_MODEL)
			{
				return;
			}

			IW7::ParticleModuleDef module{};
			module.moduleType = IW7::PARTICLE_MODULE_INIT_MODEL;
			auto& moduleData = module.moduleData.initModel;
			moduleData.type = module.moduleType;
			moduleData.m_flags = 0;

			// m_usePhysics makes AddModule allocate physics instances that need an IW7 physics
			// asset on the model -- a DYNAMIC one, or the piece never moves (a compressed-mesh
			// body cannot be simulated). Models get that asset by being registered here: the
			// model converter builds one from the model's PhysCollmap, or a bounds box when it
			// has none, and the IW3 fx dumper re-dumps the model afterwards. This is what stock
			// does for its vfx debris (spheres / small convexes with mass properties). Elements
			// without model physics keep the ray-cast emulation.
			moduleData.m_usePhysics = fx_model_physics_enabled() && (elem->flags & FX_ELEM_USE_MODEL_PHYSICS) != 0;
			moduleData.m_motionBlurHQ = false;
			if (moduleData.m_usePhysics)
			{
				for (auto idx = 0; idx < elem->visualCount; idx++)
				{
					const auto* model = elem->visualCount > 1 ? elem->visuals.array[idx].model : elem->visuals.instance.model;
					if (model && model->name)
					{
						request_dynamic_box(model->name, FX_MODEL_PHYSICS_MASS);
					}
				}
			}

			if (elem->visualCount)
			{
				moduleData.m_linkedAssetList.numAssets = elem->visualCount;
				moduleData.m_linkedAssetList.assetList = allocator.allocate<IW7::ParticleLinkedAssetDef>(moduleData.m_linkedAssetList.numAssets);

				if (elem->visualCount > 1)
				{
					for (int idx = 0; idx < moduleData.m_linkedAssetList.numAssets; idx++)
					{
						moduleData.m_linkedAssetList.assetList[idx].model = reinterpret_cast<IW7::XModel*>(elem->visuals.array[idx].model);
					}
				}
				else
				{
					moduleData.m_linkedAssetList.assetList[0].model = reinterpret_cast<IW7::XModel*>(elem->visuals.instance.model);
				}

				//moduleData.m_flags |= IW7::PARTICLE_MODULE_FLAG_HAS_ASSETS;
			}
			else
			{
				return;
			}

			modules.push_back(module);
		}

		void generate_init_runner_module(FxElemDef* elem, allocator& allocator, std::vector<IW7::ParticleModuleDef>& modules)
		{
			if (elem->elemType != FX_ELEM_TYPE_RUNNER && elem->elemType != FX_ELEM_TYPE_SOUND)
			{
				return;
			}

			IW7::ParticleModuleDef module{};
			module.moduleType = IW7::PARTICLE_MODULE_INIT_RUNNER;
			auto& moduleData = module.moduleData.initRunner;
			moduleData.type = module.moduleType;
			moduleData.m_flags = 0;

			// sound elements become runners without child effects (stock has a few of those) plus INIT_SOUND,
			// the runner module is what AddModule stores as the element type module
			if (elem->elemType == FX_ELEM_TYPE_RUNNER && elem->visualCount)
			{
				moduleData.m_linkedAssetList.numAssets = elem->visualCount;
				moduleData.m_linkedAssetList.assetList = allocator.allocate<IW7::ParticleLinkedAssetDef>(moduleData.m_linkedAssetList.numAssets);

				for (int idx = 0; idx < moduleData.m_linkedAssetList.numAssets; idx++)
				{
					const auto* effect = elem->visualCount > 1 ? elem->visuals.array[idx].effectDef.handle : elem->visuals.instance.effectDef.handle;
					moduleData.m_linkedAssetList.assetList[idx].particleSystem = allocator.manual_allocate<IW7::ParticleSystemDef>(8);
					moduleData.m_linkedAssetList.assetList[idx].particleSystem->name = allocator.duplicate_string(effect->name);
				}

				state_flags |= IW7::PARTICLE_STATE_DEF_FLAG_HAS_CHILD_EFFECTS;
			}

			modules.push_back(module);
		}

		void generate_init_sound_module(FxElemDef* elem, allocator& allocator, std::vector<IW7::ParticleModuleDef>& modules)
		{
			if (elem->elemType != FX_ELEM_TYPE_SOUND || !elem->visualCount)
			{
				return;
			}

			IW7::ParticleModuleDef module{};
			module.moduleType = IW7::PARTICLE_MODULE_INIT_SOUND;
			auto& moduleData = module.moduleData.initSound;
			moduleData.type = module.moduleType;
			moduleData.m_flags = 0;

			moduleData.m_linkedAssetList.numAssets = elem->visualCount;
			moduleData.m_linkedAssetList.assetList = allocator.allocate<IW7::ParticleLinkedAssetDef>(elem->visualCount);
			for (auto idx = 0; idx < elem->visualCount; idx++)
			{
				const auto* sound = elem->visualCount > 1 ? elem->visuals.array[idx].soundName : elem->visuals.instance.soundName;
				moduleData.m_linkedAssetList.assetList[idx].sound = allocator.duplicate_string(sound ? sound : "");
			}

			// the runtime walks sound particles through this flag (KillSoundParticlesAll)
			state_flags |= IW7::PARTICLE_STATE_DEF_FLAG_PLAY_SOUNDS;

			modules.push_back(module);
		}

		void generate_init_cloud_module(FxElemDef* elem, allocator& allocator, std::vector<IW7::ParticleModuleDef>& modules)
		{
			if (elem->elemType != FX_ELEM_TYPE_CLOUD && elem->elemType != FX_ELEM_TYPE_SPARKCLOUD)
			{
				return;
			}

			IW7::ParticleModuleDef module{};
			module.moduleType = IW7::PARTICLE_MODULE_INIT_CLOUD;
			auto& moduleData = module.moduleData.initCloud;
			moduleData.type = module.moduleType;
			moduleData.m_flags = 0;

			// the cloud draw setup reads this module unconditionally and only draws a particle when the curve
			// value is non-zero, culling with value + max(size.x, size.y); IW5 FX_DrawElem_Cloud does exactly
			// that with visState.scale, so the curves carry the IW5 scale channel
			const auto sample_count = elem->visSamples ? elem->visStateIntervalCount + 1 : 0;

			xoxor4d::MinMaxCurveSample range{};
			for (auto s = 0; s < sample_count; s++)
			{
				const auto base = elem->visSamples[s].base.scale;
				xoxor4d::GetMinMaxForSample(range, base, base + elem->visSamples[s].amplitude.scale, s);
			}

			const auto scale = sample_count ? range.GetAbsMax() : 0.0f;
			for (auto i = 0; i < 2; i++)
			{
				auto& curve = moduleData.curves[i];
				if (!scale)
				{
					curve.scale = 1.0f;
					curve.numControlPoints = 2;
					curve.controlPoints = allocator.allocate<IW7::ParticleCurveControlPointDef>(2);
					curve.controlPoints[0].time = 0.0f;
					curve.controlPoints[0].value = 1.0f;
					curve.controlPoints[1].time = 1.0f;
					curve.controlPoints[1].value = 1.0f;
					continue;
				}

				curve.scale = scale;
				curve.numControlPoints = sample_count;
				curve.controlPoints = allocator.allocate<IW7::ParticleCurveControlPointDef>(sample_count);
				for (auto s = 0; s < sample_count; s++)
				{
					const auto base = elem->visSamples[s].base.scale;
					curve.controlPoints[s].time = static_cast<float>(s) / (sample_count - 1);
					curve.controlPoints[s].value = (i == 0 ? base : base + elem->visSamples[s].amplitude.scale) / scale;
				}
			}

			calculate_inv_time_delta(moduleData.curves, 2);
			fixup_randomization_flags(moduleData.curves[0], moduleData.curves[1], &moduleData.m_flags);

			modules.push_back(module);
		}

		void generate_physics_ray_cast_module(FxElemDef* elem, allocator& allocator, std::vector<IW7::ParticleModuleDef>& modules)
		{
			// IW5 only tests collision (and so impact effects / die on touch) with FX_ELEM_USE_COLLISION,
			// model physics collides on its own
			if ((elem->flags & (FX_ELEM_USE_COLLISION | FX_ELEM_USE_MODEL_PHYSICS)) == 0)
			{
				return;
			}

			// a model element that is a real physics body does not also ray cast
			if (elem->elemType == FX_ELEM_TYPE_MODEL && fx_model_physics_enabled()
				&& (elem->flags & FX_ELEM_USE_MODEL_PHYSICS) != 0)
			{
				return;
			}

			IW7::ParticleModuleDef module{};
			module.moduleType = IW7::PARTICLE_MODULE_PHYSICS_RAY_CAST;
			auto& moduleData = module.moduleData.physicsRayCast;
			moduleData.type = module.moduleType;
			moduleData.m_flags = 0;

			// IW5 reflectionFactor is the bounce elasticity in [0, 1]
			moduleData.m_bounce.min = elem->reflectionFactor.base;
			moduleData.m_bounce.max = elem->reflectionFactor.base + elem->reflectionFactor.amplitude;

			for (auto i = 0; i < 3; i++)
			{
				moduleData.m_bounds.midPoint[i] = elem->collBounds.midPoint[i];
				moduleData.m_bounds.halfSize[i] = elem->collBounds.halfSize[i];
			}

			moduleData.m_useItemClip = elem->useItemClip != 0;
			moduleData.m_useSurfaceType = false;
			moduleData.m_collideWithWater = false;
			moduleData.m_ignoreContentItem = false;

			state_flags |= IW7::PARTICLE_STATE_DEF_FLAG_HAS_RAY_CAST_PHYSICS;

			modules.push_back(module);
		}

		void generate_init_material_module(FxElemDef* elem, allocator& allocator, std::vector<IW7::ParticleModuleDef>& modules)
		{
			// clouds draw through the InitMaterial module too, and the cloud draw setup
			// dereferences it without a null check (crash at 0x140D06A50)
			const bool is_cloud = elem->elemType == FX_ELEM_TYPE_CLOUD || elem->elemType == FX_ELEM_TYPE_SPARKCLOUD;
			if (elem->elemType != FX_ELEM_TYPE_SPRITE_BILLBOARD && elem->elemType != FX_ELEM_TYPE_SPRITE_ORIENTED && elem->elemType != FX_ELEM_TYPE_TAIL && elem->elemType != FX_ELEM_TYPE_TRAIL && !is_cloud)
			{
				system_flags |= IW7::PARTICLE_SYSTEM_DEF_FLAG_HAS_NON_SPRITES; // add this here i guess..
				return;
			}

			IW7::ParticleModuleDef module{};
			module.moduleType = IW7::PARTICLE_MODULE_INIT_MATERIAL;
			auto& moduleData = module.moduleData.initMaterial;
			moduleData.type = module.moduleType;
			moduleData.m_flags = 0;

			if (elem->visualCount)
			{
				moduleData.m_linkedAssetList.numAssets = elem->visualCount;
				moduleData.m_linkedAssetList.assetList = allocator.allocate<IW7::ParticleLinkedAssetDef>(moduleData.m_linkedAssetList.numAssets);

				if (elem->visualCount > 1)
				{
					for (int idx = 0; idx < moduleData.m_linkedAssetList.numAssets; idx++)
					{
						fixup_geotrail_material(elem, elem->visuals.array[idx].material);

						moduleData.m_linkedAssetList.assetList[idx].material = allocator.manual_allocate<IW7::Material>(8);
						moduleData.m_linkedAssetList.assetList[idx].material->name = allocator.duplicate_string(
							IW7::resolve_material_name(elem->visuals.array[idx].material->name));
					}
				}
				else
				{
					fixup_geotrail_material(elem, elem->visuals.instance.material);

					moduleData.m_linkedAssetList.assetList[0].material = allocator.manual_allocate<IW7::Material>(8);
					moduleData.m_linkedAssetList.assetList[0].material->name = allocator.duplicate_string(
						IW7::resolve_material_name(elem->visuals.instance.material->name));
				}

				//moduleData.m_flags |= IW7::PARTICLE_MODULE_FLAG_HAS_ASSETS;
			}
			else
			{
				return;
			}

			if (is_cloud)
			{
				system_flags |= IW7::PARTICLE_SYSTEM_DEF_FLAG_HAS_NON_SPRITES;
			}
			else
			{
				state_flags |= IW7::PARTICLE_STATE_DEF_FLAG_IS_SPRITE;
				system_flags |= IW7::PARTICLE_SYSTEM_DEF_FLAG_HAS_SPRITES;

				// IW5 lights an element whenever lightingFrac is non-zero. The IW7 effect techsets
				// always sample the fx lightmap, and a state without this flag gets texel 0 of it -
				// whichever lit particle (muzzle smoke, explosions) last landed there.
				if (elem->lightingFrac)
				{
					state_flags |= IW7::PARTICLE_STATE_DEF_FLAG_GPU_LIGHTING;
				}
			}

			modules.push_back(module);
		}

		void generate_init_oriented_sprite_module(FxElemDef* elem, allocator& allocator, std::vector<IW7::ParticleModuleDef>& modules)
		{
			if (elem->elemType != FX_ELEM_TYPE_SPRITE_ORIENTED)
			{
				return;
			}

			IW7::ParticleModuleDef module{};
			module.moduleType = IW7::PARTICLE_MODULE_INIT_ORIENTED_SPRITE;
			auto& moduleData = module.moduleData.initOrientedSprite;
			moduleData.type = module.moduleType;
			moduleData.m_flags = 0;

			moduleData.m_orientationQuat.v[0] = 0.5f;
			moduleData.m_orientationQuat.v[1] = 0.5f;
			moduleData.m_orientationQuat.v[2] = 0.5f;
			moduleData.m_orientationQuat.v[3] = 0.5f;

			switch (elem->flags & FX_ELEM_RUN_MASK)
			{
			case FX_ELEM_RUN_RELATIVE_TO_WORLD:
				
				break;
			case FX_ELEM_RUN_RELATIVE_TO_SPAWN:
				// alrighttt
				moduleData.m_orientationQuat.v[1] *= -1.0f;
				moduleData.m_orientationQuat.v[2] *= -1.0f;
				break;
			case FX_ELEM_RUN_RELATIVE_TO_EFFECT:
				
				break;
			case FX_ELEM_RUN_RELATIVE_TO_OFFSET:
				
				break;
			}

			modules.push_back(module);
		}

		void generate_init_mirror_texture_module(FxElemDef* elem, allocator& allocator, std::vector<IW7::ParticleModuleDef>& modules)
		{
			if (elem->elemType != FX_ELEM_TYPE_SPRITE_BILLBOARD && elem->elemType != FX_ELEM_TYPE_SPRITE_ORIENTED)
			{
				return;
			}

			IW7::ParticleModuleDef module{};
			module.moduleType = IW7::PARTICLE_MODULE_INIT_MIRROR_TEXTURE;
			auto& moduleData = module.moduleData.initMirrorTexture;
			moduleData.type = module.moduleType;
			moduleData.m_flags = 0;

			moduleData.m_mirrorHorizontal = IW7::PARTICLE_MIRROR_TEXTURE_TYPE_RANDOM;
			moduleData.m_mirrorVertical = IW7::PARTICLE_MIRROR_TEXTURE_TYPE_RANDOM;

			state_flags |= IW7::PARTICLE_STATE_DEF_FLAG_MIRROR_TEXTURE_HORIZONTALLY;
			state_flags |= IW7::PARTICLE_STATE_DEF_FLAG_MIRROR_TEXTURE_HORIZONTALLY_RANDOM;
			state_flags |= IW7::PARTICLE_STATE_DEF_FLAG_MIRROR_TEXTURE_VERTICALLY;
			state_flags |= IW7::PARTICLE_STATE_DEF_FLAG_MIRROR_TEXTURE_VERTICALLY_RANDOM;

			modules.push_back(module);
		}

		void generate_init_spawn_shape_cylinder_module(FxElemDef* elem, allocator& allocator, std::vector<IW7::ParticleModuleDef>& modules)
		{
			if ((elem->flags & FX_ELEM_SPAWN_OFFSET_MASK) != FX_ELEM_SPAWN_OFFSET_CYLINDER)
			{
				return;
			}

			IW7::ParticleModuleDef module{};
			module.moduleType = IW7::PARTICLE_MODULE_INIT_SPAWN_SHAPE_CYLINDER;
			auto& moduleData = module.moduleData.initSpawnShapeCylinder;
			moduleData.type = module.moduleType;
			moduleData.m_flags = 0;

			moduleData.m_axisFlags = IW7::PARTICLE_MODULE_AXES_FLAG_ALL;
			moduleData.m_spawnFlags = 0;
			moduleData.m_normalAxis = 0;
			moduleData.m_spawnType = 0;
			moduleData.m_volumeCubeRoot = 0.0f;

			// IW5 FX_OffsetSpawnOrigin: radius in the effect's y/z plane, height along the effect's x axis
			// from base to base + amplitude. the IW7 cylinder is built around z, rotated by m_directionQuat
			// (stock uses this z->x quat) and then moved by the post-rotation offset (the float4 after m_radius)
			moduleData.m_hasRotation = true;
			moduleData.m_rotateCalculatedOffset = false;

			moduleData.m_directionQuat.v[0] = 0.0f;
			moduleData.m_directionQuat.v[1] = 0.7071067690849304f;
			moduleData.m_directionQuat.v[2] = 0.0f;
			moduleData.m_directionQuat.v[3] = 0.7071067690849304f;

			moduleData.m_radius.min = elem->spawnOffsetRadius.base;
			moduleData.m_radius.max = elem->spawnOffsetRadius.base + elem->spawnOffsetRadius.amplitude;

			moduleData.m_halfHeight = elem->spawnOffsetHeight.amplitude * 0.5f;
			moduleData.unk.v[0] = elem->spawnOffsetHeight.base + moduleData.m_halfHeight;
			moduleData.unk.v[1] = 0.0f;
			moduleData.unk.v[2] = 0.0f;
			moduleData.unk.v[3] = 0.0f;

			state_flags |= IW7::PARTICLE_STATE_DEF_FLAG_HAS_SPAWN_SHAPE;

			modules.push_back(module);
		}

		void generate_init_spawn_shape_sphere_module(FxElemDef* elem, allocator& allocator, std::vector<IW7::ParticleModuleDef>& modules)
		{
			if ((elem->flags & FX_ELEM_SPAWN_OFFSET_MASK) != FX_ELEM_SPAWN_OFFSET_SPHERE)
			{
				return;
			}

			IW7::ParticleModuleDef module{};
			module.moduleType = IW7::PARTICLE_MODULE_INIT_SPAWN_SHAPE_SPHERE;
			auto& moduleData = module.moduleData.initSpawnShapeSphere;
			moduleData.type = module.moduleType;
			moduleData.m_flags = 0;

			moduleData.m_axisFlags = IW7::PARTICLE_MODULE_AXES_FLAG_ALL;
			moduleData.m_spawnFlags = 0;
			moduleData.m_normalAxis = 0;
			moduleData.m_spawnType = 0;
			moduleData.m_volumeCubeRoot = 0.0f;

			// IW5: random direction, distance in [base, base + amplitude]
			moduleData.m_radius.min = elem->spawnOffsetRadius.base;
			moduleData.m_radius.max = elem->spawnOffsetRadius.base + elem->spawnOffsetRadius.amplitude;

			state_flags |= IW7::PARTICLE_STATE_DEF_FLAG_HAS_SPAWN_SHAPE;

			modules.push_back(module);
		}

		void generate_init_spawn_shape_box_module(FxElemDef* elem, allocator& allocator, std::vector<IW7::ParticleModuleDef>& modules)
		{
			IW7::ParticleModuleDef module{};
			module.moduleType = IW7::PARTICLE_MODULE_INIT_SPAWN_SHAPE_BOX;
			auto& moduleData = module.moduleData.initSpawnShapeBox;
			moduleData.type = module.moduleType;
			moduleData.m_flags = 0;

			// IW5 FX_GetSpawnOrigin applies the offset in effect space only with FX_ELEM_SPAWN_RELATIVE_TO_EFFECT
			if ((elem->flags & FX_ELEM_SPAWN_RELATIVE_TO_EFFECT) == 0 && elem->elemType != FX_ELEM_TYPE_TRAIL)
			{
				moduleData.m_flags |= IW7::PARTICLE_MODULE_FLAG_USE_WORLD_SPACE;
			}

			moduleData.m_axisFlags = IW7::PARTICLE_MODULE_AXES_FLAG_ALL;
			moduleData.m_spawnFlags = 0;
			moduleData.m_normalAxis = 0;
			moduleData.m_spawnType = 0;
			moduleData.m_volumeCubeRoot = 0.0f;

			for (auto i = 0; i < 3; i++)
			{
				moduleData.m_dimensionsMin.v[i] = elem->spawnOrigin[i].base;
				moduleData.m_dimensionsMax.v[i] = elem->spawnOrigin[i].base + elem->spawnOrigin[i].amplitude;
			}
			moduleData.m_dimensionsMin.v[3] = 0.0f;
			moduleData.m_dimensionsMax.v[3] = 0.0f;

			state_flags |= IW7::PARTICLE_STATE_DEF_FLAG_HAS_SPAWN_SHAPE;

			modules.push_back(module);
		}

		void set_light_def(unsigned int& flags, IW7::ParticleLinkedAssetListDef& list, FxElemDef* elem, allocator& allocator)
		{
			// every stock light module has exactly one light def (light_fx_default) and HAS_LIGHT_DEFS;
			// IW5 light elements usually have no GfxLightDef
			const GfxLightDef* light_def = nullptr;
			if (elem->visualCount == 1)
			{
				light_def = elem->visuals.instance.lightDef;
			}
			else if (elem->visualCount > 1)
			{
				light_def = elem->visuals.array[0].lightDef;
			}

			const auto* name = light_def && light_def->name && *light_def->name ? light_def->name : "light_fx_default";

			list.numAssets = 1;
			list.assetList = allocator.allocate<IW7::ParticleLinkedAssetDef>(1);
			list.assetList[0].lightDef = allocator.manual_allocate<IW7::GfxLightDef>(8);
			list.assetList[0].lightDef->name = allocator.duplicate_string(name);

			flags |= IW7::PARTICLE_MODULE_FLAG_HAS_LIGHT_DEFS;
		}

		void generate_init_omni_light_module(FxElemDef* elem, allocator& allocator, std::vector<IW7::ParticleModuleDef>& modules)
		{
			if (elem->elemType != FX_ELEM_TYPE_OMNI_LIGHT)
			{
				return;
			}

			IW7::ParticleModuleDef module{};
			module.moduleType = IW7::PARTICLE_MODULE_INIT_LIGHT_OMNI;
			auto& moduleData = module.moduleData.initLightOmni;
			moduleData.type = module.moduleType;
			moduleData.m_flags = 0;

			system_flags |= IW7::PARTICLE_SYSTEM_DEF_FLAG_HAS_LIGHTS;
			emitter_flags |= IW7::PARTICLE_EMITTER_DEF_FLAG_HAS_LIGHTS;

			moduleData.m_disableVolumetric = false;
			moduleData.m_tonemappingScaleFactor = 1.0f;
			moduleData.m_intensityIR = 0.0f;
			moduleData.m_exponent = 0;

			set_light_def(moduleData.m_flags, moduleData.m_linkedAssetList, elem, allocator);

			modules.push_back(module);
		}

		void generate_init_spot_light_module(FxElemDef* elem, allocator& allocator, std::vector<IW7::ParticleModuleDef>& modules)
		{
			if (elem->elemType != FX_ELEM_TYPE_SPOT_LIGHT)
			{
				return;
			}

			IW7::ParticleModuleDef module{};
			module.moduleType = IW7::PARTICLE_MODULE_INIT_LIGHT_SPOT;
			auto& moduleData = module.moduleData.initLightSpot;
			moduleData.type = module.moduleType;
			moduleData.m_flags = 0;

			system_flags |= IW7::PARTICLE_SYSTEM_DEF_FLAG_HAS_LIGHTS;
			emitter_flags |= IW7::PARTICLE_EMITTER_DEF_FLAG_HAS_LIGHTS;

			// IW7 fovs are radians (stock 0.785 = 45 degrees). IW5 only stores the inner cone as a fraction of
			// the outer one, the outer fov isn't part of the fx data, so use the common stock value
			const auto* spot = elem->extended.spotLightDef;
			moduleData.m_fovOuter = 0.7853981852531433f;
			moduleData.m_fovInner = spot ? moduleData.m_fovOuter * spot->fovInnerFraction : 0.0f;
			moduleData.m_bulbRadius = spot ? spot->startRadius : 1.0f;
			moduleData.m_bulbLength = 0.3f;
			moduleData.m_brightness = spot ? spot->brightness : 1.0f;
			moduleData.m_intensityUV = 0.0f;
			moduleData.m_intensityIR = 0.0f;
			moduleData.m_shadowSoftness = 0.5f;
			moduleData.m_shadowBias = 0.4f;
			moduleData.m_shadowArea = 0.01f;
			moduleData.m_shadowNearPlane = 0.0f;
			moduleData.m_toneMappingScaleFactor = 1.0f;
			moduleData.m_exponent = spot && spot->exponent != 0;

			set_light_def(moduleData.m_flags, moduleData.m_linkedAssetList, elem, allocator);

			modules.push_back(module);
		}

		void generate_init_tail_module(FxElemDef* elem, allocator& allocator, std::vector<IW7::ParticleModuleDef>& modules)
		{
			if (elem->elemType != FX_ELEM_TYPE_TAIL)
			{
				return;
			}

			IW7::ParticleModuleDef module{};
			module.moduleType = IW7::PARTICLE_MODULE_INIT_TAIL;
			auto& moduleData = module.moduleData.initTail;
			moduleData.type = module.moduleType;
			moduleData.m_flags = 0;

			moduleData.m_averagePastVelocities = 0;
			moduleData.m_maxParentSpeed = 0;
			moduleData.m_tailLeading = true;
			moduleData.m_scaleWithVelocity = false;
			moduleData.m_rotateAroundPivot = false;

			modules.push_back(module);
		}

		void generate_init_geo_trail_module(FxElemDef* elem, allocator& allocator, std::vector<IW7::ParticleModuleDef>& modules)
		{
			if (elem->elemType != FX_ELEM_TYPE_TRAIL)
			{
				return;
			}

			IW7::ParticleModuleDef module{};
			module.moduleType = IW7::PARTICLE_MODULE_INIT_GEO_TRAIL;
			auto& moduleData = module.moduleData.initGeoTrail;
			moduleData.type = module.moduleType;
			moduleData.m_flags = 0;

			moduleData.m_numPointsMax = 16; // default value
			moduleData.m_splitDistance = static_cast<float>(elem->extended.trailDef->repeatDist);
			moduleData.m_splitAngle = 0.0f;
			moduleData.m_centerOffset = 0.0f;
			moduleData.m_numSheets = 2; // 1 sheet = horizontal, 2 sheets = horizontal + vertical
			moduleData.m_fadeInDistance = 0.0f;
			moduleData.m_fadeOutDistance = 0.0f;
			moduleData.m_tileDistance = static_cast<float>(elem->extended.trailDef->repeatDist);
			moduleData.m_tileOffset.min = 0.0f;
			moduleData.m_tileOffset.max = 0.0f;
			moduleData.m_scrollTime = elem->extended.trailDef->scrollTimeMsec / 1000.f; // doesn't seem to do anything
			moduleData.m_useLocalVelocity = false;
			moduleData.m_useVerticalTexture = false;
			moduleData.m_cameraFacing = false;
			moduleData.m_fixLeadingEdge = false;
			moduleData.m_clampUVs = false;

			modules.push_back(module);
		}

		int test_module_index;

		void generate_death_module(FxElemDef* elem, allocator& allocator, std::vector<IW7::ParticleModuleDef>& modules)
		{
			if (!elem->effectOnDeath.handle)
			{
				return;
			}

			IW7::ParticleModuleDef module{};
			module.moduleType = IW7::PARTICLE_MODULE_TEST_DEATH;
			auto& moduleData = module.moduleData.testDeath;
			moduleData.type = module.moduleType;
			moduleData.m_flags = 0;

			moduleData.m_moduleIndex = test_module_index++;

			moduleData.m_eventHandlerData.m_kill = false;

			moduleData.m_eventHandlerData.m_linkedAssetList.numAssets = 1;
			moduleData.m_eventHandlerData.m_linkedAssetList.assetList = allocator.allocate<IW7::ParticleLinkedAssetDef>();
			moduleData.m_eventHandlerData.m_linkedAssetList.assetList->particleSystem = allocator.manual_allocate<IW7::ParticleSystemDef>(8);
			moduleData.m_eventHandlerData.m_linkedAssetList.assetList->particleSystem->name = allocator.duplicate_string(elem->effectOnDeath.handle->name);

			state_flags |= IW7::PARTICLE_STATE_DEF_FLAG_HAS_CHILD_EFFECTS;

			modules.push_back(module);
		}

		void generate_impact_module(FxElemDef* elem, allocator& allocator, std::vector<IW7::ParticleModuleDef>& modules)
		{
			// impacts only exist with collision (see generate_physics_ray_cast_module); IW5 kills the particle
			// only with FX_ELEM_DIE_ON_TOUCH and bounces it otherwise. stock has kill-only impact modules
			// without assets, which is what die on touch without an impact effect becomes
			const bool die_on_touch = (elem->flags & FX_ELEM_DIE_ON_TOUCH) != 0;
			if ((elem->flags & FX_ELEM_USE_COLLISION) == 0 || (!elem->effectOnImpact.handle && !die_on_touch))
			{
				return;
			}

			IW7::ParticleModuleDef module{};
			module.moduleType = IW7::PARTICLE_MODULE_TEST_IMPACT;
			auto& moduleData = module.moduleData.testImpact;
			moduleData.type = module.moduleType;
			moduleData.m_flags = 0;

			moduleData.m_moduleIndex = test_module_index++;

			moduleData.m_eventHandlerData.m_kill = die_on_touch;

			if (elem->effectOnImpact.handle)
			{
				moduleData.m_eventHandlerData.m_linkedAssetList.numAssets = 1;
				moduleData.m_eventHandlerData.m_linkedAssetList.assetList = allocator.allocate<IW7::ParticleLinkedAssetDef>();
				moduleData.m_eventHandlerData.m_linkedAssetList.assetList->particleSystem = allocator.manual_allocate<IW7::ParticleSystemDef>(8);
				moduleData.m_eventHandlerData.m_linkedAssetList.assetList->particleSystem->name = allocator.duplicate_string(elem->effectOnImpact.handle->name);

				state_flags |= IW7::PARTICLE_STATE_DEF_FLAG_HAS_CHILD_EFFECTS;
			}

			state_flags |= IW7::PARTICLE_STATE_DEF_FLAG_HANDLE_ON_IMPACT;

			modules.push_back(module);
		}

		void generate_emission_module(FxElemDef* elem, allocator& allocator, std::vector<IW7::ParticleModuleDef>& modules)
		{
			if (!elem->effectEmitted.handle)
			{
				return;
			}

			IW7::ParticleModuleDef module{};
			module.moduleType = IW7::PARTICLE_MODULE_TEST_BIRTH;
			auto& moduleData = module.moduleData.testBirth;
			moduleData.type = module.moduleType;
			moduleData.m_flags = 0;

			moduleData.m_moduleIndex = test_module_index++;

			moduleData.m_eventHandlerData.m_kill = false;

			moduleData.m_eventHandlerData.m_linkedAssetList.numAssets = 1;
			moduleData.m_eventHandlerData.m_linkedAssetList.assetList = allocator.allocate<IW7::ParticleLinkedAssetDef>();
			moduleData.m_eventHandlerData.m_linkedAssetList.assetList->particleSystem = allocator.manual_allocate<IW7::ParticleSystemDef>(8);
			moduleData.m_eventHandlerData.m_linkedAssetList.assetList->particleSystem->name = allocator.duplicate_string(elem->effectEmitted.handle->name);

			state_flags |= IW7::PARTICLE_STATE_DEF_FLAG_HAS_CHILD_EFFECTS;

			modules.push_back(module);
		}

		void set_module_group(IW7::ParticleStateDef* state, IW7::ParticleModuleGroup group, const std::vector<IW7::ParticleModuleDef>& modules, allocator& allocator)
		{
			if (modules.empty())
			{
				return;
			}

			auto& module_group = state->moduleGroupDefs[group];
			module_group.numModules = static_cast<int>(modules.size());
			module_group.moduleDefs = allocator.allocate<IW7::ParticleModuleDef>(module_group.numModules);
			for (auto i = 0; i < module_group.numModules; i++)
			{
				memcpy(&module_group.moduleDefs[i], &modules[i], sizeof(IW7::ParticleModuleDef));
			}
		}

		void convert_emitter(FxElemDef* elem, bool looping, IW7::ParticleEmitterDef* emitter, allocator& allocator)
		{
			emitter->flags = 0;
			emitter_flags = 0;

			emitter->particleSpawnRate.min = 5.0f;
			emitter->particleSpawnRate.max = 5.0f;

			emitter->particleBurstCount.min = 1;
			emitter->particleBurstCount.max = 1;

			emitter->emitterLife.min = 0.0f;
			emitter->emitterLife.max = 0.0f;

			emitter->emitterDelay.min = 0.0f;
			emitter->emitterDelay.max = 0.0f;

			emitter->particleLife.min = elem->lifeSpanMsec.base / 1000.0f;
			emitter->particleLife.max = elem->lifeSpanMsec.base / 1000.0f + elem->lifeSpanMsec.amplitude / 1000.0f;

			if (looping)
			{
				// IW5 spawns one particle every intervalMsec until spawn.looping.count particles were spawned,
				// 0x7FFFFFFF meaning forever (fx_update.cpp). IW7 rates are particles per second, emitter life 0 is
				// endless and particleCountMax is the number alive at once (stock: roughly rate * particle life)
				const auto interval = std::max(elem->spawn.looping.intervalMsec, 1);
				const auto spawn_rate = 1000.0f / static_cast<float>(interval);

				emitter->particleSpawnRate.min = spawn_rate;
				emitter->particleSpawnRate.max = spawn_rate;

				auto alive_max = static_cast<int>(std::ceil(spawn_rate * emitter->particleLife.max)) + 1;

				if (elem->spawn.looping.count != 0x7FFFFFFF)
				{
					const auto particle_count = std::max(elem->spawn.looping.count, 1);
					const auto emitter_life = (particle_count * interval) / 1000.0f;

					emitter->emitterLife.min = emitter_life;
					emitter->emitterLife.max = emitter_life;

					alive_max = std::min(alive_max, particle_count);
				}

				emitter->particleCountMax = std::max(alive_max, 1);
			}
			else
			{
				emitter->particleBurstCount.min = elem->spawn.oneShot.count.base;
				emitter->particleBurstCount.max = elem->spawn.oneShot.count.base + elem->spawn.oneShot.count.amplitude;
				emitter->particleCountMax = std::max(emitter->particleBurstCount.max, 1);

				emitter_flags |= IW7::PARTICLE_EMITTER_DEF_FLAG_USE_BURST_MODE;
			}

			emitter->particleDelay.min = elem->spawnDelayMsec.base / 1000.0f;
			emitter->particleDelay.max = elem->spawnDelayMsec.base / 1000.0f + elem->spawnDelayMsec.amplitude / 1000.0f;

			emitter->spawnRangeSq.min = elem->spawnRange.base;
			emitter->spawnRangeSq.max = elem->spawnRange.base + elem->spawnRange.amplitude;
			emitter->spawnRangeSq.min *= emitter->spawnRangeSq.min;
			emitter->spawnRangeSq.max *= emitter->spawnRangeSq.max;

			//emitter->fadeOutMaxDistance = elem->fadeOutRange.base + elem->fadeOutRange.amplitude;

			emitter->spawnFrustumCullRadius = elem->spawnFrustumCullRadius;
			emitter->randomSeed = elem->randomSeed;

			emitter->particleSpawnShapeRange.min = 0.0f; // idk (never used)
			emitter->particleSpawnShapeRange.max = 0.0f; // idk (never used)

			emitter->groupIDs[0] = 0; // idk
			emitter->groupIDs[1] = 0; // idk
			emitter->groupIDs[2] = 0; // idk
			emitter->groupIDs[3] = 0; // idk

			emitter->unk1 = 0; // idk
			emitter->unk2 = 100.0f; // idk

			emitter_flags |= (elem->flags & FX_ELEM_DRAW_PAST_FOG) != 0 ? IW7::PARTICLE_EMITTER_DEF_FLAG_DRAW_PAST_FOG : 0;

			emitter->numStates = 1;
			emitter->stateDefs = allocator.allocate<IW7::ParticleStateDef>(emitter->numStates);

			auto* state = emitter->stateDefs;

			state->elementType = convert_elem_type(elem->elemType);

			state->flags = 0;
			state_flags = 0;

			// FX_ELEM_DRAW_WITH_VIEWMODEL has no known IW7 state bit (0x20000000 is INIT_SOUND), collision and
			// model physics set their bits from generate_physics_ray_cast_module
			state_flags |= (elem->flags & FX_ELEM_BLOCK_SIGHT) != 0 ? IW7::PARTICLE_STATE_DEF_FLAG_BLOCKS_SIGHT : 0;

			if (!elem_uses_material(elem))
			{
				system_flags |= IW7::PARTICLE_SYSTEM_DEF_FLAG_HAS_NON_SPRITES;
			}

			state->moduleGroupDefs = allocator.allocate<IW7::ParticleModuleGroupDef>(IW7::PARTICLE_MODULE_GROUP_COUNT);

			// init modules, ordered like stock: spawn, attributes, element type module, then module enum order
			{
				std::vector<IW7::ParticleModuleDef> init_modules{};
				generate_init_spawn_module(elem, allocator, init_modules);
				generate_init_attributes_module(elem, allocator, init_modules);
				generate_init_cloud_module(elem, allocator, init_modules);
				generate_init_tail_module(elem, allocator, init_modules);
				generate_init_geo_trail_module(elem, allocator, init_modules);
				generate_init_omni_light_module(elem, allocator, init_modules);
				generate_init_spot_light_module(elem, allocator, init_modules);
				generate_init_model_module(elem, allocator, init_modules);
				generate_init_runner_module(elem, allocator, init_modules);
				generate_init_decal_module(elem, allocator, init_modules);
				generate_init_oriented_sprite_module(elem, allocator, init_modules);
				generate_init_material_module(elem, allocator, init_modules);
				generate_init_atlas_module(elem, allocator, init_modules);
				generate_init_relative_velocity_module(elem, allocator, init_modules);
				generate_init_rotation_module(elem, allocator, init_modules);
				generate_init_rotation3d_module(elem, allocator, init_modules);
				generate_init_sound_module(elem, allocator, init_modules);
				generate_init_spawn_shape_box_module(elem, allocator, init_modules);
				generate_init_spawn_shape_cylinder_module(elem, allocator, init_modules);
				generate_init_spawn_shape_sphere_module(elem, allocator, init_modules);
				//generate_init_mirror_texture_module(elem, allocator, init_modules);

				set_module_group(state, IW7::PARTICLE_MODULE_GROUP_INIT, init_modules, allocator);
			}

			// update modules
			{
				std::vector<IW7::ParticleModuleDef> update_modules{};
				generate_color_module(elem, allocator, update_modules);
				generate_size_module(elem, allocator, update_modules);
				generate_rotation_module(elem, allocator, update_modules);
				generate_velocity_module(elem, allocator, update_modules);
				generate_gravity_module(elem, allocator, update_modules);
				generate_physics_ray_cast_module(elem, allocator, update_modules);
				//generate_position_module(elem, allocator, update_modules);

				set_module_group(state, IW7::PARTICLE_MODULE_GROUP_UPDATE, update_modules, allocator);
			}

			// test modules
			{
				test_module_index = 0;

				std::vector<IW7::ParticleModuleDef> test_modules{};
				generate_death_module(elem, allocator, test_modules);
				generate_impact_module(elem, allocator, test_modules);
				generate_emission_module(elem, allocator, test_modules);

				set_module_group(state, IW7::PARTICLE_MODULE_GROUP_TEST, test_modules, allocator);
			}

			emitter->flags |= emitter_flags;

			state->flags |= state_flags;
		}

		IW7::ParticleSystemDef* convert(FxEffectDef* asset, allocator& allocator)
		{
			auto* iw7_asset = allocator.allocate<IW7::ParticleSystemDef>();

			iw7_asset->name = asset->name;

			system_flags = 0;

			// elemDefs holds looping, then one-shot, then emission elements. emission elements are copies of the
			// one-shot elements of effectEmitted (FX_CopyEmittedElemDefs) that IW5 only spawns while a particle
			// emits; that effect is referenced by generate_emission_module, so they must not become emitters here
			std::vector<std::pair<FxElemDef*, bool>> elems;
			const auto elem_count = asset->elemDefCountLooping + asset->elemDefCountOneShot;
			for (auto elem_index = 0; elem_index < elem_count; elem_index++)
			{
				auto* elem = &asset->elemDefs[elem_index];
				if (is_elem_convertible(asset, elem))
				{
					elems.emplace_back(elem, elem_index < asset->elemDefCountLooping);
				}
			}

			iw7_asset->numEmitters = static_cast<int>(elems.size());
			iw7_asset->emitterDefs = allocator.allocate<IW7::ParticleEmitterDef>(std::max(iw7_asset->numEmitters, 1));
			for (auto emitter_index = 0; emitter_index < iw7_asset->numEmitters; emitter_index++)
			{
				convert_emitter(elems[emitter_index].first, elems[emitter_index].second, &iw7_asset->emitterDefs[emitter_index], allocator);
			}

			system_flags |= IW7::PARTICLE_SYSTEM_DEF_FLAG_KILL_STOPPED_INFINITE_EFFECTS;
			//system_flags |= IW7::PARTICLE_SYSTEM_DEF_FLAG_CANNOT_PRE_ROLL; // not sure what this is

			iw7_asset->flags |= system_flags;

			iw7_asset->version = 15;

			iw7_asset->occlusionOverrideEmitterIndex = -1;

			iw7_asset->phaseOptions = IW7::PARTICLE_PHASE_OPTION_PHASE_NEVER;

			iw7_asset->drawFrustumCullRadius = -1.0f;
			iw7_asset->updateFrustumCullRadius = -1.0f;

			iw7_asset->sunDistance = 100000.000f;

			iw7_asset->preRollMSec = 0; // spawnTime delay

			iw7_asset->editorPosition.v[0] = 0.0f;
			iw7_asset->editorPosition.v[1] = 0.0f;
			iw7_asset->editorPosition.v[2] = 0.0f;
			iw7_asset->editorPosition.v[3] = 0.0f;

			iw7_asset->editorRotation.v[0] = 0.0f;
			iw7_asset->editorRotation.v[1] = 0.0f;
			iw7_asset->editorRotation.v[2] = 0.0f;
			iw7_asset->editorRotation.v[3] = 1.0f;

			return iw7_asset;
		}
	}
}
