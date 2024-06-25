#include "stdafx.hpp"
#include "../Include.hpp"

#include "ParticleSystem.hpp"

#include "IW3/Structs.hpp"
#include "IW3/Dumper/IW7/Assets/Material.hpp"

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
				__debugbreak();
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
				__debugbreak();
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
				IW3::IW7Dumper::dump(reinterpret_cast<ZoneTool::IW3::Material*>(material), true);
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

			IW7::ParticleModuleDef module{};
			module.moduleType = IW7::PARTICLE_MODULE_SIZE_GRAPH;
			auto& moduleData = module.moduleData.sizeGraph;
			moduleData.type = module.moduleType;
			moduleData.m_flags = 0;

			moduleData.firstCurve = false;

			float widthScale = 0.0f;
			float heightScale = 0.0f;
			float scaleScale = 0.0f;

			{
				// #
				// size curve brah (width height)
				// => curve scale = max * 2
				// => key values = key * 2 / scale
				xoxor4d::MinMaxCurveSample width{};
				xoxor4d::MinMaxCurveSample height{};

				// find largest value (pos and neg)
				for (auto s = 0; s < elem->visStateIntervalCount + 1; s++)
				{
					auto widthBase = elem->visSamples[s].base.size[0];
					auto widthAmpl = elem->visSamples[s].amplitude.size[0];

					auto heightBase = elem->visSamples[s].base.size[1];
					auto heightAmpl = elem->visSamples[s].amplitude.size[1];

					GetMinMaxForSample(width, widthBase, widthAmpl, s);
					GetMinMaxForSample(height, heightBase, heightAmpl, s);
				}

				widthScale = width.GetAbsMax() * 2.0f;
				heightScale = height.GetAbsMax() * 2.0f;
			}

			{
				// #
				// scale curve bree (model & clouds)
				// => curve scale = max
				// => key values = key / scale
				xoxor4d::MinMaxCurveSample scale{};

				// find largest value (pos and neg)
				for (auto s = 0; s < elem->visStateIntervalCount + 1; s++)
				{
					auto scaleBase = elem->visSamples[s].base.scale;
					auto scaleAmpl = elem->visSamples[s].amplitude.scale;

					xoxor4d::GetMinMaxForSample(scale, scaleBase, scaleAmpl, s);
				}

				scaleScale = scale.GetAbsMax() * 2.0f;
			}

			if (!widthScale && !heightScale && !scaleScale)
			{
				return;
			}

			int width_index0 = -1;
			int width_index1 = -1;

			int height_index0 = -1;
			int height_index1 = -1;

			int scale_index0 = -1;
			int scale_index1 = -1;

			{
				int index = 0;

				if (widthScale) width_index0 = index++;
				if (heightScale) height_index0 = index++;
				if (scaleScale) scale_index0 = index++;

				if (!widthScale) width_index0 = index++;
				if (!heightScale) height_index0 = index++;
				if (!scaleScale) scale_index0 = index++;

				width_index1 = width_index0 + 3;
				height_index1 = height_index0 + 3;
				scale_index1 = scale_index0 + 3;
			}

			auto sampleCount = elem->visStateIntervalCount + 1;
			auto sampleSize = 1.0f / (sampleCount - 1);
			
			if (widthScale)
			{
				moduleData.m_curves[width_index0].scale = widthScale;
				moduleData.m_curves[width_index1].scale = widthScale;

				moduleData.m_curves[width_index0].numControlPoints = sampleCount;
				moduleData.m_curves[width_index0].controlPoints =
					allocator.allocate<IW7::ParticleCurveControlPointDef>(moduleData.m_curves[width_index0].numControlPoints);

				moduleData.m_curves[width_index1].numControlPoints = sampleCount;
				moduleData.m_curves[width_index1].controlPoints =
					allocator.allocate<IW7::ParticleCurveControlPointDef>(moduleData.m_curves[width_index1].numControlPoints);
			}
			else
			{
				moduleData.m_curves[width_index0].numControlPoints = 2;
				moduleData.m_curves[width_index0].controlPoints =
					allocator.allocate<IW7::ParticleCurveControlPointDef>(moduleData.m_curves[width_index0].numControlPoints);

				moduleData.m_curves[width_index1].numControlPoints = 2;
				moduleData.m_curves[width_index1].controlPoints =
					allocator.allocate<IW7::ParticleCurveControlPointDef>(moduleData.m_curves[width_index1].numControlPoints);

				set_default_size_values(moduleData.m_curves[width_index0]);
				set_default_size_values(moduleData.m_curves[width_index1]);
			}

			if (heightScale)
			{
				moduleData.m_curves[height_index0].scale = heightScale;
				moduleData.m_curves[height_index1].scale = heightScale;

				moduleData.m_curves[height_index0].numControlPoints = sampleCount;
				moduleData.m_curves[height_index0].controlPoints =
					allocator.allocate<IW7::ParticleCurveControlPointDef>(moduleData.m_curves[height_index0].numControlPoints);

				moduleData.m_curves[height_index1].numControlPoints = sampleCount;
				moduleData.m_curves[height_index1].controlPoints =
					allocator.allocate<IW7::ParticleCurveControlPointDef>(moduleData.m_curves[height_index1].numControlPoints);
			}
			else
			{
				moduleData.m_curves[height_index0].numControlPoints = 2;
				moduleData.m_curves[height_index0].controlPoints =
					allocator.allocate<IW7::ParticleCurveControlPointDef>(moduleData.m_curves[height_index0].numControlPoints);

				moduleData.m_curves[height_index1].numControlPoints = 2;
				moduleData.m_curves[height_index1].controlPoints =
					allocator.allocate<IW7::ParticleCurveControlPointDef>(moduleData.m_curves[height_index1].numControlPoints);

				set_default_size_values(moduleData.m_curves[height_index0]);
				set_default_size_values(moduleData.m_curves[height_index1]);
			}

			if (scaleScale)
			{
				moduleData.m_curves[scale_index0].scale = scaleScale;
				moduleData.m_curves[scale_index1].scale = scaleScale;

				moduleData.m_curves[scale_index0].numControlPoints = sampleCount;
				moduleData.m_curves[scale_index0].controlPoints =
					allocator.allocate<IW7::ParticleCurveControlPointDef>(moduleData.m_curves[scale_index0].numControlPoints);

				moduleData.m_curves[scale_index1].numControlPoints = sampleCount;
				moduleData.m_curves[scale_index1].controlPoints =
					allocator.allocate<IW7::ParticleCurveControlPointDef>(moduleData.m_curves[scale_index1].numControlPoints);
			}
			else
			{
				moduleData.m_curves[scale_index0].numControlPoints = 2;
				moduleData.m_curves[scale_index0].controlPoints =
					allocator.allocate<IW7::ParticleCurveControlPointDef>(moduleData.m_curves[scale_index0].numControlPoints);

				moduleData.m_curves[scale_index1].numControlPoints = 2;
				moduleData.m_curves[scale_index1].controlPoints =
					allocator.allocate<IW7::ParticleCurveControlPointDef>(moduleData.m_curves[scale_index1].numControlPoints);

				set_default_size_values(moduleData.m_curves[scale_index0]);
				set_default_size_values(moduleData.m_curves[scale_index1]);
			}

			for (auto i = 0; i < sampleCount; i++)
			{
				if (widthScale)
				{
					moduleData.m_curves[width_index0].controlPoints[i].time = sampleSize * i;
					moduleData.m_curves[width_index0].controlPoints[i].value = elem->visSamples[i].base.size[xoxor4d::SizeCurveType::Width] / widthScale;

					moduleData.m_curves[width_index1].controlPoints[i].time = sampleSize * i;
					moduleData.m_curves[width_index1].controlPoints[i].value = elem->visSamples[i].amplitude.size[xoxor4d::SizeCurveType::Width] / widthScale;
					moduleData.m_curves[width_index1].controlPoints[i].value += moduleData.m_curves[width_index0].controlPoints[i].value;
				}

				if (heightScale)
				{
					moduleData.m_curves[height_index0].controlPoints[i].time = sampleSize * i;
					moduleData.m_curves[height_index0].controlPoints[i].value = elem->visSamples[i].base.size[xoxor4d::SizeCurveType::Height] / heightScale;

					moduleData.m_curves[height_index1].controlPoints[i].time = sampleSize * i;
					moduleData.m_curves[height_index1].controlPoints[i].value = elem->visSamples[i].amplitude.size[xoxor4d::SizeCurveType::Height] / heightScale;
					moduleData.m_curves[height_index1].controlPoints[i].value += moduleData.m_curves[height_index0].controlPoints[i].value;
				}

				if (scaleScale)
				{
					moduleData.m_curves[scale_index0].controlPoints[i].time = sampleSize * i;
					moduleData.m_curves[scale_index0].controlPoints[i].value = elem->visSamples[i].base.scale / scaleScale;

					moduleData.m_curves[scale_index1].controlPoints[i].time = sampleSize * i;
					moduleData.m_curves[scale_index1].controlPoints[i].value = elem->visSamples[i].amplitude.scale / scaleScale;
					moduleData.m_curves[scale_index1].controlPoints[i].value += moduleData.m_curves[scale_index0].controlPoints[i].value;
				}
			}

			calculate_inv_time_delta(moduleData.m_curves, GetModuleNumCurves(module.moduleType));

			if (widthScale)
			{
				fixup_randomization_flags(moduleData.m_curves[width_index0], moduleData.m_curves[width_index1], &moduleData.m_flags);
			}
			if (heightScale)
			{
				fixup_randomization_flags(moduleData.m_curves[height_index0], moduleData.m_curves[height_index1], &moduleData.m_flags);
			}
			if (scaleScale)
			{
				fixup_randomization_flags(moduleData.m_curves[scale_index0], moduleData.m_curves[scale_index1], &moduleData.m_flags);
			}

			state_flags |= IW7::PARTICLE_STATE_DEF_FLAG_HAS_SIZE_CURVE;

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

			if (elem->elemType == FX_ELEM_TYPE_TAIL)
			{
				return; // this is currently not supported, since it has both local and world vel?
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

			const bool local = (elem->flags & FX_ELEM_HAS_VELOCITY_GRAPH_LOCAL) != 0;
			const bool world = (elem->flags & FX_ELEM_HAS_VELOCITY_GRAPH_WORLD) != 0;
			
			if (local && world)
			{
				// i don't know how to do this
				return;
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

			state_flags |= local != 0 ? 0x100 : 0;
			state_flags |= world != 0 ? 0x200 : 0;

			//state_flags |= local != 0 ? IW7::PARTICLE_STATE_DEF_FLAG_HAS_VELOCITY_CURVE_LOCAL1 : 0;
			//state_flags |= local != 0 ? IW7::PARTICLE_STATE_DEF_FLAG_HAS_VELOCITY_CURVE_LOCAL2 : 0;

			//state_flags |= world != 0 ? IW7::PARTICLE_STATE_DEF_FLAG_HAS_VELOCITY_CURVE_WORLD1 : 0;
			//state_flags |= world != 0 ? IW7::PARTICLE_STATE_DEF_FLAG_HAS_VELOCITY_CURVE_WORLD2 : 0;

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
			if (elem->spawnAngles[0].base == 0.0f && elem->spawnAngles[1].base == 0.0f && elem->spawnAngles[2].base == 0.0f)
			{
				return;
			}

			IW7::ParticleModuleDef module{};
			module.moduleType = IW7::PARTICLE_MODULE_INIT_ROTATION_3D;
			auto& moduleData = module.moduleData.initRotation3D;
			moduleData.type = module.moduleType;
			moduleData.m_flags = 0;

			// idk if correct

			moduleData.m_rotationRateMin.v[0] = elem->angularVelocity[0].base;
			moduleData.m_rotationRateMin.v[1] = elem->angularVelocity[1].base;
			moduleData.m_rotationRateMin.v[2] = elem->angularVelocity[2].base;
			moduleData.m_rotationRateMin.v[3] = 0.0f;
			moduleData.m_rotationRateMax.v[0] = elem->angularVelocity[0].base + elem->angularVelocity[0].amplitude;
			moduleData.m_rotationRateMax.v[1] = elem->angularVelocity[1].base + elem->angularVelocity[1].amplitude;
			moduleData.m_rotationRateMax.v[2] = elem->angularVelocity[2].base + elem->angularVelocity[2].amplitude;
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

			module.moduleData.initAtlas.m_playRate = elem->atlas.fps;
			module.moduleData.initAtlas.m_startFrame = elem->atlas.entryCount - 1;
			module.moduleData.initAtlas.m_loopCount = elem->atlas.loopCount;

			if (elem->atlas.loopCount == 0xFF)
			{
				__debugbreak();
			}

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

			moduleData.m_fadeInTime = static_cast<short>(elem->fadeInRange.base); // idk
			moduleData.m_fadeOutTime = static_cast<short>(elem->fadeOutRange.base); // idk
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
							IW7::replace_material_prefix(elem->visuals.markArray[idx].materials[0]->name));
					}
					if (elem->visuals.markArray[idx].materials[1])
					{
						// wim
						moduleData.m_linkedAssetList.assetList[idx].decal.materials[1] = allocator.manual_allocate<IW7::Material>(8);
						moduleData.m_linkedAssetList.assetList[idx].decal.materials[1]->name = allocator.duplicate_string(
							IW7::replace_material_prefix(elem->visuals.markArray[idx].materials[1]->name));

						// wim autodisplacement
						moduleData.m_linkedAssetList.assetList[idx].decal.materials[2] = allocator.manual_allocate<IW7::Material>(8);
						moduleData.m_linkedAssetList.assetList[idx].decal.materials[2]->name = allocator.duplicate_string(
							IW7::replace_material_prefix(elem->visuals.markArray[idx].materials[1]->name));
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

			moduleData.m_usePhysics = (elem->flags & FX_ELEM_USE_MODEL_PHYSICS) != 0;
			moduleData.m_motionBlurHQ = false;

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
			if (elem->elemType != FX_ELEM_TYPE_RUNNER)
			{
				return;
			}

			IW7::ParticleModuleDef module{};
			module.moduleType = IW7::PARTICLE_MODULE_INIT_RUNNER;
			auto& moduleData = module.moduleData.initRunner;
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
						moduleData.m_linkedAssetList.assetList[idx].particleSystem = reinterpret_cast<IW7::ParticleSystemDef*>(elem->visuals.array[idx].effectDef.handle);
					}
				}
				else
				{
					moduleData.m_linkedAssetList.assetList[0].particleSystem = reinterpret_cast<IW7::ParticleSystemDef*>(elem->visuals.instance.effectDef.handle);
				}

				//moduleData.m_flags |= IW7::PARTICLE_MODULE_FLAG_HAS_ASSETS;
			}
			else
			{
				return;
			}

			modules.push_back(module);
		}

		void generate_init_material_module(FxElemDef* elem, allocator& allocator, std::vector<IW7::ParticleModuleDef>& modules)
		{
			if (elem->elemType != FX_ELEM_TYPE_SPRITE_BILLBOARD && elem->elemType != FX_ELEM_TYPE_SPRITE_ORIENTED && elem->elemType != FX_ELEM_TYPE_TAIL && elem->elemType != FX_ELEM_TYPE_TRAIL)
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
							IW7::replace_material_prefix(elem->visuals.array[idx].material->name));
					}
				}
				else
				{
					fixup_geotrail_material(elem, elem->visuals.instance.material);

					moduleData.m_linkedAssetList.assetList[0].material = allocator.manual_allocate<IW7::Material>(8);
					moduleData.m_linkedAssetList.assetList[0].material->name = allocator.duplicate_string(
						IW7::replace_material_prefix(elem->visuals.instance.material->name));
				}

				//moduleData.m_flags |= IW7::PARTICLE_MODULE_FLAG_HAS_ASSETS;
			}
			else
			{
				return;
			}

			state_flags |= IW7::PARTICLE_STATE_DEF_FLAG_IS_SPRITE;
			system_flags |= IW7::PARTICLE_SYSTEM_DEF_FLAG_HAS_SPRITES;

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
			if ((elem->flags & FX_ELEM_SPAWN_OFFSET_CYLINDER) == 0)
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

			moduleData.m_hasRotation = true;
			moduleData.m_rotateCalculatedOffset = false;

			moduleData.m_directionQuat.v[0] = 0.0f;
			moduleData.m_directionQuat.v[1] = 0.7071067690849304f;
			moduleData.m_directionQuat.v[2] = 0.0f;
			moduleData.m_directionQuat.v[3] = 0.7071067690849304f;

			moduleData.m_radius.min = elem->spawnOffsetRadius.base;
			moduleData.m_radius.min = elem->spawnOffsetRadius.base + elem->spawnOffsetRadius.amplitude;

			// idk where to put this
			// spawnOffsetHeight

			moduleData.m_flags |= IW7::PARTICLE_MODULE_FLAG_DISABLED;

			state_flags |= IW7::PARTICLE_STATE_DEF_FLAG_HAS_SPAWN_SHAPE;

			modules.push_back(module);
		}

		void generate_init_spawn_shape_sphere_module(FxElemDef* elem, allocator& allocator, std::vector<IW7::ParticleModuleDef>& modules)
		{
			if ((elem->flags & FX_ELEM_SPAWN_OFFSET_SPHERE) == 0)
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

			moduleData.m_radius.min = elem->spawnOffsetRadius.base;
			moduleData.m_radius.min = elem->spawnOffsetRadius.base + elem->spawnOffsetRadius.amplitude;

			moduleData.m_flags |= IW7::PARTICLE_MODULE_FLAG_DISABLED;

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

			if ((elem->flags & FX_ELEM_RUN_MASK) == FX_ELEM_RUN_RELATIVE_TO_WORLD && elem->elemType != FX_ELEM_TYPE_TRAIL)
			{
				moduleData.m_flags |= IW7::PARTICLE_MODULE_FLAG_USE_WORLD_SPACE;
			}

			moduleData.m_axisFlags = IW7::PARTICLE_MODULE_AXES_FLAG_ALL;
			moduleData.m_spawnFlags = 0;
			moduleData.m_normalAxis = 0;
			moduleData.m_spawnType = 0;
			moduleData.m_volumeCubeRoot = 0.0f;

			module.moduleData.initSpawnShapeBox.m_dimensionsMin.v[0] = elem->spawnOrigin[0].base;
			module.moduleData.initSpawnShapeBox.m_dimensionsMin.v[1] = elem->spawnOrigin[1].base;
			module.moduleData.initSpawnShapeBox.m_dimensionsMin.v[2] = elem->spawnOrigin[2].base;
			module.moduleData.initSpawnShapeBox.m_dimensionsMin.v[3] = 0.0f;

			module.moduleData.initSpawnShapeBox.m_dimensionsMax.v[0] = elem->spawnOrigin[0].base + elem->spawnOrigin[0].amplitude;
			module.moduleData.initSpawnShapeBox.m_dimensionsMax.v[1] = elem->spawnOrigin[1].base + elem->spawnOrigin[1].amplitude;
			module.moduleData.initSpawnShapeBox.m_dimensionsMax.v[2] = elem->spawnOrigin[2].base + elem->spawnOrigin[2].amplitude;
			module.moduleData.initSpawnShapeBox.m_dimensionsMax.v[3] = 0.0f;

			state_flags |= IW7::PARTICLE_STATE_DEF_FLAG_HAS_SPAWN_SHAPE;

			modules.push_back(module);
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
			// todo?

			if (elem->visualCount)
			{
				moduleData.m_linkedAssetList.numAssets = elem->visualCount;
				moduleData.m_linkedAssetList.assetList = allocator.allocate<IW7::ParticleLinkedAssetDef>(elem->visualCount);

				if (elem->visualCount > 1)
				{
					for (auto i = 0; i < elem->visualCount; i++)
					{
						if (elem->visuals.array[i].lightDef && *elem->visuals.array[i].lightDef->name)
						{
							moduleData.m_linkedAssetList.assetList[i].lightDef = allocator.manual_allocate<IW7::GfxLightDef>(8);
							moduleData.m_linkedAssetList.assetList[i].lightDef->name = allocator.duplicate_string(elem->visuals.array[i].lightDef->name);

							//moduleData.m_flags |= IW7::PARTICLE_MODULE_FLAG_HAS_ASSETS;
							moduleData.m_flags |= IW7::PARTICLE_MODULE_FLAG_HAS_LIGHT_DEFS;
						}
					}
				}
				else
				{
					if (elem->visuals.instance.lightDef && *elem->visuals.instance.lightDef->name)
					{
						moduleData.m_linkedAssetList.assetList[0].lightDef = allocator.manual_allocate<IW7::GfxLightDef>(8);
						moduleData.m_linkedAssetList.assetList[0].lightDef->name = allocator.duplicate_string(elem->visuals.instance.lightDef->name);

						//moduleData.m_flags |= IW7::PARTICLE_MODULE_FLAG_HAS_ASSETS;
						moduleData.m_flags |= IW7::PARTICLE_MODULE_FLAG_HAS_LIGHT_DEFS;
					}
				}
			}
			
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

			moduleData.m_fovOuter = elem->extended.spotLightDef->fovInnerFraction;
			moduleData.m_fovInner = elem->extended.spotLightDef->fovInnerFraction;
			moduleData.m_bulbRadius = elem->extended.spotLightDef->startRadius;
			moduleData.m_bulbLength = 0.3f;
			moduleData.m_brightness = elem->extended.spotLightDef->brightness;
			moduleData.m_intensityUV = 0.0f;
			moduleData.m_intensityIR = 0.0f;
			moduleData.m_shadowSoftness = 0.5f;
			moduleData.m_shadowBias = 0.4f;
			moduleData.m_shadowArea = 0.01f;
			moduleData.m_shadowNearPlane = 0.0f;
			moduleData.m_toneMappingScaleFactor = 1.0f;
			moduleData.m_exponent = elem->extended.spotLightDef->exponent;

			if (elem->visualCount)
			{
				moduleData.m_linkedAssetList.numAssets = elem->visualCount;
				moduleData.m_linkedAssetList.assetList = allocator.allocate<IW7::ParticleLinkedAssetDef>(elem->visualCount);

				if (elem->visualCount > 1)
				{
					for (auto i = 0; i < elem->visualCount; i++)
					{
						if (elem->visuals.array[i].lightDef && *elem->visuals.array[i].lightDef->name)
						{
							moduleData.m_linkedAssetList.assetList[i].lightDef = allocator.manual_allocate<IW7::GfxLightDef>(8);
							moduleData.m_linkedAssetList.assetList[i].lightDef->name = allocator.duplicate_string(elem->visuals.array[i].lightDef->name);

							//moduleData.m_flags |= IW7::PARTICLE_MODULE_FLAG_HAS_ASSETS;
							moduleData.m_flags |= IW7::PARTICLE_MODULE_FLAG_HAS_LIGHT_DEFS;
						}
					}
				}
				else
				{
					if (elem->visuals.instance.lightDef && *elem->visuals.instance.lightDef->name)
					{
						moduleData.m_linkedAssetList.assetList[0].lightDef = allocator.manual_allocate<IW7::GfxLightDef>(8);
						moduleData.m_linkedAssetList.assetList[0].lightDef->name = allocator.duplicate_string(elem->visuals.instance.lightDef->name);

						//moduleData.m_flags |= IW7::PARTICLE_MODULE_FLAG_HAS_ASSETS;
						moduleData.m_flags |= IW7::PARTICLE_MODULE_FLAG_HAS_LIGHT_DEFS;
					}
				}
			}

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

			modules.push_back(module);
		}

		void generate_impact_module(FxElemDef* elem, allocator& allocator, std::vector<IW7::ParticleModuleDef>& modules)
		{
			if (!elem->effectOnImpact.handle)
			{
				return;
			}

			IW7::ParticleModuleDef module{};
			module.moduleType = IW7::PARTICLE_MODULE_TEST_IMPACT;
			auto& moduleData = module.moduleData.testDeath;
			moduleData.type = module.moduleType;
			moduleData.m_flags = 0;

			moduleData.m_moduleIndex = test_module_index++;

			moduleData.m_eventHandlerData.m_kill = true;

			moduleData.m_eventHandlerData.m_linkedAssetList.numAssets = 1;
			moduleData.m_eventHandlerData.m_linkedAssetList.assetList = allocator.allocate<IW7::ParticleLinkedAssetDef>();
			moduleData.m_eventHandlerData.m_linkedAssetList.assetList->particleSystem = allocator.manual_allocate<IW7::ParticleSystemDef>(8);
			moduleData.m_eventHandlerData.m_linkedAssetList.assetList->particleSystem->name = allocator.duplicate_string(elem->effectOnImpact.handle->name);

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

		IW7::ParticleSystemDef* convert(FxEffectDef* asset, allocator& allocator)
		{
			auto* iw7_asset = allocator.allocate<IW7::ParticleSystemDef>();

			iw7_asset->name = asset->name;

			system_flags = 0;

			iw7_asset->numEmitters = asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission;
			iw7_asset->emitterDefs = allocator.allocate<IW7::ParticleEmitterDef>(iw7_asset->numEmitters);
			for (int emitter_index = 0; emitter_index < iw7_asset->numEmitters; emitter_index++)
			{
				const bool looping = emitter_index < asset->elemDefCountLooping;
				const bool one_shot = !looping && emitter_index < asset->elemDefCountOneShot;
				const bool emission = !looping && !one_shot && emitter_index < asset->elemDefCountEmission;

				auto* emitter = &iw7_asset->emitterDefs[emitter_index];

				int elem_index = emitter_index;
				auto* elem = &asset->elemDefs[elem_index];

				if (emission) __debugbreak();

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

				if (looping)
				{
					// forever
					if (elem->spawn.looping.count == 0x7FFFFFFF)
					{
						emitter->particleSpawnRate.min = static_cast<float>(elem->spawn.looping.intervalMsec);
						emitter->particleSpawnRate.max = static_cast<float>(elem->spawn.looping.intervalMsec);

						emitter->particleCountMax = 1;
						emitter_flags |= IW7::PARTICLE_EMITTER_DEF_FLAG_INFINITE_PARTICLE_LIFE;
					}
					else
					{
						int interval = elem->spawn.looping.intervalMsec;
						int particle_count = elem->spawn.looping.count;

						float emitter_life = (particle_count * interval) / 1000.0f;
						float spawn_rate = particle_count / emitter_life;

						emitter->particleSpawnRate.min = spawn_rate;
						emitter->particleSpawnRate.max = spawn_rate;

						emitter->emitterLife.min = emitter_life;
						emitter->emitterLife.max = emitter_life;

						emitter->particleCountMax = elem->spawn.looping.count;
					}
				}
				else
				{
					emitter->particleBurstCount.min = elem->spawn.oneShot.count.base;
					emitter->particleBurstCount.max = elem->spawn.oneShot.count.base + elem->spawn.oneShot.count.amplitude;
					emitter->particleCountMax = emitter->particleBurstCount.max;

					emitter_flags |= IW7::PARTICLE_EMITTER_DEF_FLAG_USE_BURST_MODE;
				}

				emitter->particleLife.min = elem->lifeSpanMsec.base / 1000.0f;
				emitter->particleLife.max = elem->lifeSpanMsec.base / 1000.0f + elem->lifeSpanMsec.amplitude / 1000.0f;

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

				state_flags |= (elem->flags & FX_ELEM_USE_MODEL_PHYSICS) != 0 ? IW7::PARTICLE_STATE_DEF_FLAG_USE_PHYSICS : 0;
				state_flags |= (elem->flags & FX_ELEM_USE_COLLISION) != 0 ? IW7::PARTICLE_STATE_DEF_FLAG_REQUIRES_WORLD_COLLISION : 0;
				state_flags |= (elem->flags & FX_ELEM_DRAW_WITH_VIEWMODEL) != 0 ? IW7::PARTICLE_STATE_DEF_FLAG_DRAW_WITH_VIEW_MODEL : 0;

				state->moduleGroupDefs = allocator.allocate<IW7::ParticleModuleGroupDef>(IW7::PARTICLE_MODULE_GROUP_COUNT);

				// generate init modules
				{
					std::vector<IW7::ParticleModuleDef> init_modules{};
					generate_init_spawn_module(elem, allocator, init_modules);
					generate_init_attributes_module(elem, allocator, init_modules);
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
					generate_init_spawn_shape_box_module(elem, allocator, init_modules);
					generate_init_spawn_shape_sphere_module(elem, allocator, init_modules);
					//generate_init_mirror_texture_module(elem, allocator, init_modules);
					
					if (init_modules.size())
					{
						state->moduleGroupDefs[IW7::PARTICLE_MODULE_GROUP_INIT].numModules = init_modules.size();
						state->moduleGroupDefs[IW7::PARTICLE_MODULE_GROUP_INIT].moduleDefs = allocator.allocate<IW7::ParticleModuleDef>(state->moduleGroupDefs[IW7::PARTICLE_MODULE_GROUP_INIT].numModules);
						for (auto i = 0; i < state->moduleGroupDefs[IW7::PARTICLE_MODULE_GROUP_INIT].numModules; i++)
						{
							memcpy(&state->moduleGroupDefs[IW7::PARTICLE_MODULE_GROUP_INIT].moduleDefs[i], &init_modules[i], sizeof(IW7::ParticleModuleDef));
						}
					}
				}

				// generate update modules
				{
					std::vector<IW7::ParticleModuleDef> update_modules{};
					generate_color_module(elem, allocator, update_modules);
					generate_size_module(elem, allocator, update_modules);
					generate_rotation_module(elem, allocator, update_modules);
					generate_velocity_module(elem, allocator, update_modules);
					generate_gravity_module(elem, allocator, update_modules);
					//generate_position_module(elem, allocator, update_modules);

					if (update_modules.size())
					{
						state->moduleGroupDefs[IW7::PARTICLE_MODULE_GROUP_UPDATE].numModules = update_modules.size();
						state->moduleGroupDefs[IW7::PARTICLE_MODULE_GROUP_UPDATE].moduleDefs = allocator.allocate<IW7::ParticleModuleDef>(state->moduleGroupDefs[IW7::PARTICLE_MODULE_GROUP_UPDATE].numModules);
						for (auto i = 0; i < state->moduleGroupDefs[IW7::PARTICLE_MODULE_GROUP_UPDATE].numModules; i++)
						{
							memcpy(&state->moduleGroupDefs[IW7::PARTICLE_MODULE_GROUP_UPDATE].moduleDefs[i], &update_modules[i], sizeof(IW7::ParticleModuleDef));
						}
					}
				}

				// generate test modules
				{
					test_module_index = 0;

					std::vector<IW7::ParticleModuleDef> test_modules{};
					generate_death_module(elem, allocator, test_modules);
					generate_impact_module(elem, allocator, test_modules);
					generate_emission_module(elem, allocator, test_modules);

					if (test_modules.size())
					{
						state->moduleGroupDefs[IW7::PARTICLE_MODULE_GROUP_TEST].numModules = test_modules.size();
						state->moduleGroupDefs[IW7::PARTICLE_MODULE_GROUP_TEST].moduleDefs = allocator.allocate<IW7::ParticleModuleDef>(state->moduleGroupDefs[IW7::PARTICLE_MODULE_GROUP_TEST].numModules);
						for (auto i = 0; i < state->moduleGroupDefs[IW7::PARTICLE_MODULE_GROUP_TEST].numModules; i++)
						{
							memcpy(&state->moduleGroupDefs[IW7::PARTICLE_MODULE_GROUP_TEST].moduleDefs[i], &test_modules[i], sizeof(IW7::ParticleModuleDef));
						}
					}
				}

				emitter->flags |= emitter_flags;

				state->flags |= state_flags;
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
			asset->msecLoopingLife;

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