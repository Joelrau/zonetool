#include "stdafx.hpp"

#define DUMP_JSON

namespace ZoneTool::IW7
{
	const std::string PARTICLE_ELEMENT_TYPE_s[15] =
	{
		"PARTICLE_ELEMENT_TYPE_BILLBOARD_SPRITE",
		"PARTICLE_ELEMENT_TYPE_BEAM",
		"PARTICLE_ELEMENT_TYPE_CLOUD",
		"PARTICLE_ELEMENT_TYPE_SPARK_CLOUD",
		"PARTICLE_ELEMENT_TYPE_DECAL",
		"PARTICLE_ELEMENT_TYPE_FLARE",
		"PARTICLE_ELEMENT_TYPE_GEO_TRAIL",
		"PARTICLE_ELEMENT_TYPE_LIGHT_OMNI",
		"PARTICLE_ELEMENT_TYPE_LIGHT_SPOT",
		"PARTICLE_ELEMENT_TYPE_MODEL",
		"PARTICLE_ELEMENT_TYPE_ORIENTED_SPRITE",
		"PARTICLE_ELEMENT_TYPE_RUNNER",
		"PARTICLE_ELEMENT_TYPE_TAIL",
		"PARTICLE_ELEMENT_TYPE_VECTOR_FIELD",
		"PARTICLE_ELEMENT_TYPE_VOLUMETRIC",
	};

	const std::string ParticleModuleType_s[] =
	{
		"PARTICLE_MODULE_INIT_ATLAS",              // 0x0
		"PARTICLE_MODULE_INIT_ATTRIBUTES",         // 0x1
		"PARTICLE_MODULE_INIT_BEAM",               // 0x2
		"PARTICLE_MODULE_INIT_CAMERA_OFFSET",      // 0x3
		"PARTICLE_MODULE_INIT_CLOUD",              // 0x4
		"PARTICLE_MODULE_INIT_DECAL",              // 0x5
		"PARTICLE_MODULE_INIT_FLARE",              // 0x6
		"PARTICLE_MODULE_INIT_GEO_TRAIL",          // 0x7
		"PARTICLE_MODULE_INIT_LIGHT_OMNI",         // 0x8
		"PARTICLE_MODULE_INIT_LIGHT_SPOT",         // 0x9
		"PARTICLE_MODULE_INIT_MATERIAL",           // 0xA
		"PARTICLE_MODULE_INIT_MIRROR_TEXTURE",     // 0xB
		"PARTICLE_MODULE_INIT_MODEL",              // 0xC
		"PARTICLE_MODULE_INIT_OCCLUSION_QUERY",    // 0xD
		"PARTICLE_MODULE_INIT_ORIENTED_SPRITE",    // 0xE
		"PARTICLE_MODULE_INIT_PARTICLE_SIM",       // 0xF
		"PARTICLE_MODULE_INIT_PLAYER_FACING",      // 0x10
		"PARTICLE_MODULE_INIT_RELATIVE_VELOCITY",  // 0x11
		"PARTICLE_MODULE_INIT_ROTATION",           // 0x12
		"PARTICLE_MODULE_INIT_ROTATION_3D",        // 0x13
		"PARTICLE_MODULE_INIT_RUNNER",             // 0x14
		"PARTICLE_MODULE_INIT_SOUND",              // 0x15
		"PARTICLE_MODULE_INIT_SPAWN",              // 0x16
		"PARTICLE_MODULE_INIT_SPAWN_SHAPE_BOX",    // 0x17
		"PARTICLE_MODULE_INIT_SPAWN_SHAPE_CYLINDER", // 0x18
		"PARTICLE_MODULE_INIT_SPAWN_SHAPE_ELLIPSOID", // 0x19
		"PARTICLE_MODULE_INIT_SPAWN_SHAPE_MESH",   // 0x1A
		"PARTICLE_MODULE_INIT_SPAWN_SHAPE_SPHERE", // 0x1B
		"PARTICLE_MODULE_INIT_TAIL",               // 0x1C
		"PARTICLE_MODULE_INIT_VECTOR_FIELD",       // 0x1D
		"PARTICLE_MODULE_INIT_VOLUMETRIC",         // 0x1E
		"PARTICLE_MODULE_ATTRACTOR",               // 0x1F
		"PARTICLE_MODULE_COLOR_GRAPH",             // 0x20
		"PARTICLE_MODULE_COLOR_LERP",              // 0x21
		"PARTICLE_MODULE_EMISSION_GRAPH",          // 0x22
		"PARTICLE_MODULE_EMISSIVE_GRAPH",          // 0x23
		"PARTICLE_MODULE_FORCE",                   // 0x24
		"PARTICLE_MODULE_GRAVITY",                 // 0x25
		"PARTICLE_MODULE_INTENSITY_GRAPH",         // 0x26
		"PARTICLE_MODULE_PARENT_VELOCITY_GRAPH",   // 0x27
		"PARTICLE_MODULE_PHYSICS_LIGHT",           // 0x28
		"PARTICLE_MODULE_PHYSICS_RAY_CAST",        // 0x29
		"PARTICLE_MODULE_POSITION_GRAPH",          // 0x2A
		"PARTICLE_MODULE_ROTATION_GRAPH",          // 0x2B
		"PARTICLE_MODULE_ROTATION_GRAPH_3D",       // 0x2C
		"PARTICLE_MODULE_SIZE_GRAPH",              // 0x2D
		"PARTICLE_MODULE_SIZE_LERP",               // 0x2E
		"PARTICLE_MODULE_VELOCITY_GRAPH",          // 0x2F
		"PARTICLE_MODULE_TEST_AGE",                // 0x30
		"PARTICLE_MODULE_TEST_BIRTH",              // 0x31
		"PARTICLE_MODULE_TEST_DEATH",              // 0x32
		"PARTICLE_MODULE_TEST_GRAVITY",            // 0x33
		"PARTICLE_MODULE_TEST_IMPACT",             // 0x34
		"PARTICLE_MODULE_TEST_POS",                // 0x35
	};

	const std::string ParticleModuleGroup_s[] =
	{
		"PARTICLE_MODULE_GROUP_INIT",
		"PARTICLE_MODULE_GROUP_UPDATE",
		"PARTICLE_MODULE_GROUP_TEST",
	};

	const std::string PARTICLE_RELATIVE_VELOCITY_TYPE_s[] =
	{
		"PARTICLE_RELATIVE_VELOCITY_TYPE_LOCAL",
		"PARTICLE_RELATIVE_VELOCITY_TYPE_WORLD",
		"PARTICLE_RELATIVE_VELOCITY_TYPE_RELATIVE_TO_EFFECT_ORIGIN",
		"PARTICLE_RELATIVE_VELOCITY_TYPE_LOCAL_WITH_BOLT_INFO",
		"PARTICLE_RELATIVE_VELOCITY_TYPE_WORLD_WITH_BOLT_INFO",
		"PARTICLE_RELATIVE_VELOCITY_TYPE_RELATIVE_TO_EFFECT_ORIGIN_WITH_BOLT_INFO",
	};

	ParticleModuleDef* parent_module;

	namespace vfx::binary
	{
		void dump_particle_linked_asset_def(ParticleLinkedAssetDef* asset, assetmanager::dumper& dump)
		{
			switch (parent_module->moduleType)
			{
			case PARTICLE_MODULE_INIT_MATERIAL:
				dump.dump_asset(asset->material);
				break;
			case PARTICLE_MODULE_INIT_MODEL:
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_MESH:
				dump.dump_asset(asset->model);
				break;
			case PARTICLE_MODULE_PHYSICS_LIGHT:
				dump.dump_asset(asset->physicsFXData.physicsFXPipeline);
				dump.dump_asset(asset->physicsFXData.physicsFXShape);
				break;
			case PARTICLE_MODULE_INIT_PARTICLE_SIM:
				dump.dump_asset(asset->particleSim);
				break;
			case PARTICLE_MODULE_INIT_DECAL:
				dump.dump_asset(asset->decal.materials[0]);
				dump.dump_asset(asset->decal.materials[1]);
				dump.dump_asset(asset->decal.materials[2]);
				break;
			case PARTICLE_MODULE_INIT_SOUND:
				dump.dump_string(asset->sound);
				break;
			case PARTICLE_MODULE_INIT_VECTOR_FIELD:
				dump.dump_asset(asset->vectorField);
				break;
			case PARTICLE_MODULE_INIT_LIGHT_OMNI:
			case PARTICLE_MODULE_INIT_LIGHT_SPOT:
				dump.dump_asset(asset->lightDef);
				break;
			case PARTICLE_MODULE_INIT_BEAM:
			case PARTICLE_MODULE_INIT_RUNNER:

			case PARTICLE_MODULE_TEST_AGE:
			case PARTICLE_MODULE_TEST_BIRTH:
			case PARTICLE_MODULE_TEST_DEATH:
			case PARTICLE_MODULE_TEST_GRAVITY:
			case PARTICLE_MODULE_TEST_IMPACT:
			case PARTICLE_MODULE_TEST_POS:
				dump.dump_asset(asset->particleSystem);
				break;
			default:
				__debugbreak();
				break;
			}
		}

		void dump_particle_linked_asset_list(ParticleLinkedAssetListDef& asset, assetmanager::dumper& dump)
		{
			dump.dump_array(asset.assetList, asset.numAssets);
			for (auto i = 0; i < asset.numAssets; i++)
			{
				dump_particle_linked_asset_def(&asset.assetList[i], dump);
			}
		}

		void dump_module(ParticleModuleDef* asset, assetmanager::dumper& dump)
		{
			parent_module = asset;

			switch (asset->moduleType)
			{
			case PARTICLE_MODULE_INIT_ATLAS:
				break;
			case PARTICLE_MODULE_INIT_ATTRIBUTES:
				break;
			case PARTICLE_MODULE_INIT_BEAM:
				dump_particle_linked_asset_list(asset->moduleData.initBeam.m_linkedAssetList, dump);
				break;
			case PARTICLE_MODULE_INIT_CAMERA_OFFSET:
				break;
			case PARTICLE_MODULE_INIT_CLOUD:
				for (auto i = 0; i < 2; i++)
				{
					dump.dump_array(asset->moduleData.initCloud.curves[i].controlPoints, asset->moduleData.initCloud.curves[i].numControlPoints);
				}
				break;
			case PARTICLE_MODULE_INIT_DECAL:
				dump_particle_linked_asset_list(asset->moduleData.initDecal.m_linkedAssetList, dump);
				break;
			case PARTICLE_MODULE_INIT_FLARE:
				for (auto i = 0; i < 4; i++)
				{
					dump.dump_array(asset->moduleData.initFlare.m_curves[i].controlPoints, asset->moduleData.initFlare.m_curves[i].numControlPoints);
				}
				break;
			case PARTICLE_MODULE_INIT_GEO_TRAIL:
				break;
			case PARTICLE_MODULE_INIT_LIGHT_OMNI:
				dump_particle_linked_asset_list(asset->moduleData.initLightOmni.m_linkedAssetList, dump);
				break;
			case PARTICLE_MODULE_INIT_LIGHT_SPOT:
				dump_particle_linked_asset_list(asset->moduleData.initLightSpot.m_linkedAssetList, dump);
				break;
			case PARTICLE_MODULE_INIT_MATERIAL:
				dump_particle_linked_asset_list(asset->moduleData.initMaterial.m_linkedAssetList, dump);
				break;
			case PARTICLE_MODULE_INIT_MIRROR_TEXTURE:
				break;
			case PARTICLE_MODULE_INIT_MODEL:
				dump_particle_linked_asset_list(asset->moduleData.initModel.m_linkedAssetList, dump);
				break;
			case PARTICLE_MODULE_INIT_OCCLUSION_QUERY:
				break;
			case PARTICLE_MODULE_INIT_ORIENTED_SPRITE:
				break;
			case PARTICLE_MODULE_INIT_PARTICLE_SIM:
				dump_particle_linked_asset_list(asset->moduleData.initParticleSim.m_linkedAssetList, dump);
				break;
			case PARTICLE_MODULE_INIT_PLAYER_FACING:
				break;
			case PARTICLE_MODULE_INIT_RELATIVE_VELOCITY:
				break;
			case PARTICLE_MODULE_INIT_ROTATION:
				break;
			case PARTICLE_MODULE_INIT_ROTATION_3D:
				break;
			case PARTICLE_MODULE_INIT_RUNNER:
				dump_particle_linked_asset_list(asset->moduleData.initRunner.m_linkedAssetList, dump);
				break;
			case PARTICLE_MODULE_INIT_SOUND:
				dump_particle_linked_asset_list(asset->moduleData.initSound.m_linkedAssetList, dump);
				break;
			case PARTICLE_MODULE_INIT_SPAWN:
				for (auto i = 0; i < 1; i++)
				{
					dump.dump_array(asset->moduleData.initSpawn.m_curves[i].controlPoints, asset->moduleData.initSpawn.m_curves[i].numControlPoints);
				}
				break;
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_BOX:
				break;
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_CYLINDER:
				break;
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_ELLIPSOID:
				break;
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_MESH:
				dump_particle_linked_asset_list(asset->moduleData.initSpawnShapeMesh.m_linkedAssetList, dump);
				dump.dump_array(asset->moduleData.initSpawnShapeMesh.m_meshAssetData, asset->moduleData.initSpawnShapeMesh.m_numMeshAssets);
				break;
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_SPHERE:
				break;
			case PARTICLE_MODULE_INIT_TAIL:
				break;
			case PARTICLE_MODULE_INIT_VECTOR_FIELD:
				dump_particle_linked_asset_list(asset->moduleData.initVectorField.m_linkedAssetList, dump);
				break;
			case PARTICLE_MODULE_INIT_VOLUMETRIC:
				break;
			case PARTICLE_MODULE_ATTRACTOR:
				break;
			case PARTICLE_MODULE_COLOR_GRAPH:
				for (auto i = 0; i < 8; i++)
				{
					dump.dump_array(asset->moduleData.colorGraph.m_curves[i].controlPoints, asset->moduleData.colorGraph.m_curves[i].numControlPoints);
				}
				break;
			case PARTICLE_MODULE_COLOR_LERP:
				break;
			case PARTICLE_MODULE_EMISSION_GRAPH:
				for (auto i = 0; i < 2; i++)
				{
					dump.dump_array(asset->moduleData.emissionGraph.m_curves[i].controlPoints, asset->moduleData.emissionGraph.m_curves[i].numControlPoints);
				}
				break;
			case PARTICLE_MODULE_EMISSIVE_GRAPH:
				for (auto i = 0; i < 2; i++)
				{
					dump.dump_array(asset->moduleData.emissiveGraph.m_curves[i].controlPoints, asset->moduleData.emissiveGraph.m_curves[i].numControlPoints);
				}
				break;
			case PARTICLE_MODULE_FORCE:
				break;
			case PARTICLE_MODULE_GRAVITY:
				break;
			case PARTICLE_MODULE_INTENSITY_GRAPH:
				for (auto i = 0; i < 2; i++)
				{
					dump.dump_array(asset->moduleData.intensityGraph.m_curves[i].controlPoints, asset->moduleData.intensityGraph.m_curves[i].numControlPoints);
				}
				break;
			case PARTICLE_MODULE_PARENT_VELOCITY_GRAPH:
				for (auto i = 0; i < 2; i++)
				{
					dump.dump_array(asset->moduleData.parentVelocityGraph.m_curves[i].controlPoints, asset->moduleData.parentVelocityGraph.m_curves[i].numControlPoints);
				}
				break;
			case PARTICLE_MODULE_PHYSICS_LIGHT:
				dump_particle_linked_asset_list(asset->moduleData.physicsLight.m_linkedAssetList, dump);
				break;
			case PARTICLE_MODULE_PHYSICS_RAY_CAST:
				break;
			case PARTICLE_MODULE_POSITION_GRAPH:
				for (auto i = 0; i < 6; i++)
				{
					dump.dump_array(asset->moduleData.positionGraph.m_curves[i].controlPoints, asset->moduleData.positionGraph.m_curves[i].numControlPoints);
				}
				break;
			case PARTICLE_MODULE_ROTATION_GRAPH:
				for (auto i = 0; i < 2; i++)
				{
					dump.dump_array(asset->moduleData.rotationGraph.m_curves[i].controlPoints, asset->moduleData.rotationGraph.m_curves[i].numControlPoints);
				}
				break;
			case PARTICLE_MODULE_ROTATION_GRAPH_3D:
				for (auto i = 0; i < 6; i++)
				{
					dump.dump_array(asset->moduleData.rotationGraph3D.m_curves[i].controlPoints, asset->moduleData.rotationGraph3D.m_curves[i].numControlPoints);
				}
				break;
			case PARTICLE_MODULE_SIZE_GRAPH:
				for (auto i = 0; i < 6; i++)
				{
					dump.dump_array(asset->moduleData.sizeGraph.m_curves[i].controlPoints, asset->moduleData.sizeGraph.m_curves[i].numControlPoints);
				}
				break;
			case PARTICLE_MODULE_SIZE_LERP:
				break;
			case PARTICLE_MODULE_VELOCITY_GRAPH:
				for (auto i = 0; i < 6; i++)
				{
					dump.dump_array(asset->moduleData.velocityGraph.m_curves[i].controlPoints, asset->moduleData.velocityGraph.m_curves[i].numControlPoints);
				}
				break;
			case PARTICLE_MODULE_TEST_AGE:
				dump_particle_linked_asset_list(asset->moduleData.testAge.m_eventHandlerData.m_linkedAssetList, dump);
				break;
			case PARTICLE_MODULE_TEST_BIRTH:
				dump_particle_linked_asset_list(asset->moduleData.testBirth.m_eventHandlerData.m_linkedAssetList, dump);
				break;
			case PARTICLE_MODULE_TEST_DEATH:
				dump_particle_linked_asset_list(asset->moduleData.testDeath.m_eventHandlerData.m_linkedAssetList, dump);
				break;
			case PARTICLE_MODULE_TEST_GRAVITY:
				dump_particle_linked_asset_list(asset->moduleData.testGravity.m_eventHandlerData.m_linkedAssetList, dump);
				break;
			case PARTICLE_MODULE_TEST_IMPACT:
				dump_particle_linked_asset_list(asset->moduleData.testImpact.m_eventHandlerData.m_linkedAssetList, dump);
				break;
			case PARTICLE_MODULE_TEST_POS:
				dump_particle_linked_asset_list(asset->moduleData.testPos.m_eventHandlerData.m_linkedAssetList, dump);
				break;
			}
		}

		void dump_module_group(ParticleModuleGroupDef* asset, assetmanager::dumper& dump)
		{
			dump.dump_array(asset->moduleDefs, asset->numModules);
			for (auto i = 0; i < asset->numModules; i++)
			{
				dump_module(&asset->moduleDefs[i], dump);
			}
		}

		void dump_state(ParticleStateDef* asset, assetmanager::dumper& dump)
		{
			dump.dump_array(asset->moduleGroupDefs, 3);
			for (auto i = 0; i < 3; i++)
			{
				dump_module_group(&asset->moduleGroupDefs[i], dump);
			}
		}

		void dump_emitter(ParticleEmitterDef* asset, assetmanager::dumper& dump)
		{
			dump.dump_array(asset->stateDefs, asset->numStates);
			for (auto i = 0; i < asset->numStates; i++)
			{
				dump_state(&asset->stateDefs[i], dump);
			}

			dump.dump_array(asset->fadeCurveDef.controlPoints, asset->fadeCurveDef.numControlPoints);
		}

		void dump(ParticleSystemDef* asset)
		{
			assetmanager::dumper dump;

			const auto path = "particlesystem\\"s + asset->name + ".iw7VFX"s;
			if (!dump.open(path))
			{
				return;
			}

			dump.dump_single(asset);
			dump.dump_string(asset->name);

			dump.dump_array(asset->emitterDefs, asset->numEmitters);
			for (auto i = 0; i < asset->numEmitters; i++)
			{
				dump_emitter(&asset->emitterDefs[i], dump);
			}

			dump.dump_array(asset->scriptedInputNodeDefs, asset->numScriptedInputNodes);

			dump.close();
		}
	}

	namespace vfx::json
	{
#define DUMP_STRING(__field__) \
	static_assert(std::is_same_v<decltype(asset->__field__), const char PTR64>, "Field is not of type const char*"); \
	asset->__field__ ? data[#__field__] = asset->__field__ : data[#__field__] = "";

#define DUMP_FIELD(__field__) \
	data[#__field__] = asset->__field__;

#define DUMP_FIELD_ARR(__field__, __size__) \
	for (auto idx420 = 0u; idx420 < (unsigned int)__size__; idx420++) \
	{ \
		data[#__field__][idx420] = asset->__field__[idx420]; \
	}

#define DUMP_ASSET_ARR(__field__, __size__) \
	if (asset->__field__ && __size__) \
	{ \
		for (auto idx420 = 0u; idx420 < (unsigned int)__size__; idx420++) \
		{ \
			if (asset->__field__[idx420]) \
			{ \
				data[#__field__][idx420] = asset->__field__[idx420]->name; \
			} \
			else \
			{ \
				data[#__field__][idx420] = ""; \
			} \
		} \
	} \
	else \
	{ \
		data[#__field__] = nullptr; \
	}

#define DUMP_ASSET(__field__) \
	if (asset->__field__) \
	{ \
		data[#__field__] = asset->__field__->name; \
	} \
	else \
	{ \
		data[#__field__] = ""; \
	}

		void dump_particle_linked_asset_def(ParticleLinkedAssetDef* asset, ordered_json& data)
		{
			switch (parent_module->moduleType)
			{
			case PARTICLE_MODULE_INIT_MATERIAL:
				DUMP_ASSET(material);
				break;
			case PARTICLE_MODULE_INIT_MODEL:
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_MESH:
				DUMP_ASSET(model);
				break;
			case PARTICLE_MODULE_PHYSICS_LIGHT:
				DUMP_ASSET(physicsFXData.physicsFXPipeline);
				DUMP_ASSET(physicsFXData.physicsFXShape);
				break;
			case PARTICLE_MODULE_INIT_PARTICLE_SIM:
				DUMP_ASSET(particleSim);
				break;
			case PARTICLE_MODULE_INIT_DECAL:
				DUMP_ASSET_ARR(decal.materials, 3);
				break;
			case PARTICLE_MODULE_INIT_SOUND:
				DUMP_STRING(sound);
				break;
			case PARTICLE_MODULE_INIT_VECTOR_FIELD:
				DUMP_ASSET(vectorField);
				break;
			case PARTICLE_MODULE_INIT_LIGHT_OMNI:
			case PARTICLE_MODULE_INIT_LIGHT_SPOT:
				DUMP_ASSET(lightDef);
				break;
			case PARTICLE_MODULE_INIT_BEAM:
			case PARTICLE_MODULE_INIT_RUNNER:

			case PARTICLE_MODULE_TEST_AGE:
			case PARTICLE_MODULE_TEST_BIRTH:
			case PARTICLE_MODULE_TEST_DEATH:
			case PARTICLE_MODULE_TEST_GRAVITY:
			case PARTICLE_MODULE_TEST_IMPACT:
			case PARTICLE_MODULE_TEST_POS:
				DUMP_ASSET(particleSystem);
				break;
			default:
				__debugbreak();
				break;
			}
		}

		void dump_particle_linked_asset_list(ParticleLinkedAssetListDef& asset, ordered_json& data)
		{
			for (auto i = 0; i < asset.numAssets; i++)
			{
				dump_particle_linked_asset_def(&asset.assetList[i], data[i]);
			}
		}

		void dump_control_points(ParticleCurveControlPointDef* asset, ordered_json& data)
		{
			DUMP_FIELD(value);
			DUMP_FIELD(time);
			DUMP_FIELD(invTimeDelta);
		}

		void dump_curve(ParticleCurveDef* asset, ordered_json& data)
		{
			DUMP_FIELD(scale);

			for (auto i = 0; i < asset->numControlPoints; i++)
			{
				dump_control_points(&asset->controlPoints[i], data["ControlPoint"][i]);
			}
		}

#define DUMP_TYPE(__type__) \
	data["type"] = ParticleModuleType_s[asset->type];

		void dump_module(ParticleModuleDef* asset_, ordered_json& data)
		{
			parent_module = asset_;

			{
				auto* asset = &asset_->moduleData.moduleBase;
				DUMP_TYPE(type);
				DUMP_FIELD(m_flags);
			}

			switch (asset_->moduleType)
			{
			case PARTICLE_MODULE_INIT_ATLAS:
			{
				auto* asset = &asset_->moduleData.initAtlas;

				DUMP_FIELD(m_playRate);
				DUMP_FIELD(m_startFrame);
				DUMP_FIELD(m_loopCount);
				break;
			}
			case PARTICLE_MODULE_INIT_ATTRIBUTES:
			{
				auto* asset = &asset_->moduleData.initAttributes;

				DUMP_FIELD(m_useNonUniformInterpolationForColor);
				DUMP_FIELD(m_useNonUniformInterpolationForSize);
				DUMP_FIELD_ARR(m_sizeMin.v, 4);
				DUMP_FIELD_ARR(m_sizeMax.v, 4);
				DUMP_FIELD_ARR(m_colorMin.v, 4);
				DUMP_FIELD_ARR(m_colorMax.v, 4);
				DUMP_FIELD_ARR(m_velocityMin.v, 4);
				DUMP_FIELD_ARR(m_velocityMax.v, 4);
				break;
			}
			case PARTICLE_MODULE_INIT_BEAM:
			{
				auto* asset = &asset_->moduleData.initBeam;

				DUMP_FIELD(m_beamFlags);
				DUMP_FIELD(m_splitDistance);
				DUMP_FIELD_ARR(m_startPos.v, 4);
				DUMP_FIELD_ARR(m_endPos.v, 4);
				DUMP_FIELD_ARR(m_offset.v, 4);
				DUMP_FIELD_ARR(m_curvePoint1.v, 4);
				DUMP_FIELD_ARR(m_curvePoint2.v, 4);
				DUMP_FIELD(m_scrollRateUV);
				DUMP_FIELD(m_scrollRateSpiralGraph);
				DUMP_FIELD(m_spiralRepeatDistance);

				dump_particle_linked_asset_list(asset->m_linkedAssetList, data["AssetList"]);
				break;
			}
			case PARTICLE_MODULE_INIT_CAMERA_OFFSET:
			{
				[[maybe_unused]] auto* asset = &asset_->moduleData.initCameraOffset;
				break;
			}
			case PARTICLE_MODULE_INIT_CLOUD:
			{
				auto* asset = &asset_->moduleData.initCloud;

				for (auto i = 0; i < 2; i++)
				{
					dump_curve(&asset->curves[i], data["Curves"][i]);
				}
				break;
			}
			case PARTICLE_MODULE_INIT_DECAL:
			{
				auto* asset = &asset_->moduleData.initDecal;

				DUMP_FIELD(m_fadeInTime);
				DUMP_FIELD(m_fadeOutTime);
				DUMP_FIELD(m_stoppableFadeOutTime);
				DUMP_FIELD(m_lerpWaitTime);

				DUMP_FIELD_ARR(m_lerpColor.v, 4);

				dump_particle_linked_asset_list(asset->m_linkedAssetList, data["AssetList"]);
				break;
			}
			case PARTICLE_MODULE_INIT_FLARE:
			{
				auto* asset = &asset_->moduleData.initFlare;

				DUMP_FIELD(m_position);
				DUMP_FIELD_ARR(m_direction.v, 4);
				DUMP_FIELD(m_angularRotCount);
				DUMP_FIELD(m_flareFlags);
				DUMP_FIELD(m_depthScaleRange.min);
				DUMP_FIELD(m_depthScaleRange.max);
				DUMP_FIELD(m_depthScaleValue.min);
				DUMP_FIELD(m_depthScaleValue.max);
				DUMP_FIELD(m_radialRot.min);
				DUMP_FIELD(m_radialRot.max);
				DUMP_FIELD(m_radialScaleX.min);
				DUMP_FIELD(m_radialScaleX.max);
				DUMP_FIELD(m_radialScaleY.min);
				DUMP_FIELD(m_radialScaleY.max);

				for (auto i = 0; i < 4; i++)
				{
					dump_curve(&asset->m_curves[i], data["Curves"][i]);
				}
				break;
			}
			case PARTICLE_MODULE_INIT_GEO_TRAIL:
			{
				auto* asset = &asset_->moduleData.initGeoTrail;

				DUMP_FIELD(m_numPointsMax);
				DUMP_FIELD(m_splitDistance);
				DUMP_FIELD(m_splitAngle);
				DUMP_FIELD(m_centerOffset);
				DUMP_FIELD(m_numSheets);
				DUMP_FIELD(m_fadeInDistance);
				DUMP_FIELD(m_fadeOutDistance);
				DUMP_FIELD(m_tileDistance);
				DUMP_FIELD(m_tileOffset.min);
				DUMP_FIELD(m_tileOffset.max);
				DUMP_FIELD(m_scrollTime);
				DUMP_FIELD(m_useLocalVelocity);
				DUMP_FIELD(m_useVerticalTexture);
				DUMP_FIELD(m_cameraFacing);
				DUMP_FIELD(m_fixLeadingEdge);
				DUMP_FIELD(m_clampUVs);

				break;
			}
			case PARTICLE_MODULE_INIT_LIGHT_OMNI:
			{
				auto* asset = &asset_->moduleData.initLightOmni;

				DUMP_FIELD(m_tonemappingScaleFactor);
				DUMP_FIELD(m_intensityIR);
				DUMP_FIELD(m_disableVolumetric);
				DUMP_FIELD(m_exponent);
				DUMP_FIELD(m_shadowSoftness);
				DUMP_FIELD(m_shadowBias);
				DUMP_FIELD(m_shadowArea);
				DUMP_FIELD_ARR(__pad0, 2);

				dump_particle_linked_asset_list(asset->m_linkedAssetList, data["AssetList"]);
				break;
			}
			case PARTICLE_MODULE_INIT_LIGHT_SPOT:
			{
				auto* asset = &asset_->moduleData.initLightSpot;

				DUMP_FIELD(m_fovOuter);
				DUMP_FIELD(m_fovInner);
				DUMP_FIELD(m_bulbRadius);
				DUMP_FIELD(m_bulbLength);
				DUMP_FIELD(m_brightness);
				DUMP_FIELD(unk1);
				DUMP_FIELD(m_intensityUV);
				DUMP_FIELD(m_intensityIR);
				DUMP_FIELD(m_shadowNearPlane);
				DUMP_FIELD(m_toneMappingScaleFactor);
				DUMP_FIELD(m_shadowSoftness);
				DUMP_FIELD(m_shadowBias);
				DUMP_FIELD(m_shadowArea);
				DUMP_FIELD(m_disableVolumetric);
				DUMP_FIELD(m_disableShadowMap);
				DUMP_FIELD(m_disableDynamicShadows);
				DUMP_FIELD(m_exponent);

				dump_particle_linked_asset_list(asset->m_linkedAssetList, data["AssetList"]);
				break;
			}
			case PARTICLE_MODULE_INIT_MATERIAL:
			{
				auto* asset = &asset_->moduleData.initMaterial;

				dump_particle_linked_asset_list(asset->m_linkedAssetList, data["AssetList"]);
				break;
			}
			case PARTICLE_MODULE_INIT_MIRROR_TEXTURE:
			{
				auto* asset = &asset_->moduleData.initMirrorTexture;

				DUMP_FIELD(m_mirrorHorizontal);
				DUMP_FIELD(m_mirrorVertical);

				break;
			}
			case PARTICLE_MODULE_INIT_MODEL:
			{
				auto* asset = &asset_->moduleData.initModel;

				DUMP_FIELD(m_usePhysics);
				DUMP_FIELD(m_motionBlurHQ);

				dump_particle_linked_asset_list(asset->m_linkedAssetList, data["AssetList"]);
				break;
			}
			case PARTICLE_MODULE_INIT_OCCLUSION_QUERY:
			{
				auto* asset = &asset_->moduleData.initOcclusionQuery;

				DUMP_FIELD(m_fadeInTime);
				DUMP_FIELD(m_fadeOutTime);
				DUMP_FIELD(m_scale.min);
				DUMP_FIELD(m_scale.max);
				DUMP_FIELD_ARR(m_worldSize, 2);

				break;
			}
			case PARTICLE_MODULE_INIT_ORIENTED_SPRITE:
			{
				auto* asset = &asset_->moduleData.initOrientedSprite;

				DUMP_FIELD_ARR(m_orientationQuat.v, 4);

				DUMP_FIELD_ARR(m_pad, 2);

				for (auto i = 0; i < 2; i++)
				{
					if (asset->m_pad[i] != 0)
					{
						__debugbreak();
					}
				}

				break;
			}
			case PARTICLE_MODULE_INIT_PARTICLE_SIM:
			{
				auto* asset = &asset_->moduleData.initParticleSim;

				DUMP_FIELD(m_scaleFactor.min);
				DUMP_FIELD(m_scaleFactor.max);
				DUMP_FIELD(m_holdLastFrame);

				dump_particle_linked_asset_list(asset->m_linkedAssetList, data["AssetList"]);
				break;
			}
			case PARTICLE_MODULE_INIT_PLAYER_FACING:
			{
				[[maybe_unused]] auto* asset = &asset_->moduleData.initPlayerFacing;

				break;
			}
			case PARTICLE_MODULE_INIT_RELATIVE_VELOCITY:
			{
				auto* asset = &asset_->moduleData.initRelativeVelocity;

				data["m_velocityType"] = PARTICLE_RELATIVE_VELOCITY_TYPE_s[asset->m_velocityType];
				DUMP_FIELD(m_useBoltInfo);

				break;
			}
			case PARTICLE_MODULE_INIT_ROTATION:
			{
				auto* asset = &asset_->moduleData.initRotation;

				DUMP_FIELD(m_rotationAngle.min);
				DUMP_FIELD(m_rotationAngle.max);
				DUMP_FIELD(m_rotationRate.min);
				DUMP_FIELD(m_rotationRate.max);

				break;
			}
			case PARTICLE_MODULE_INIT_ROTATION_3D:
			{
				auto* asset = &asset_->moduleData.initRotation3D;

				DUMP_FIELD_ARR(m_rotationAngleMin.v, 4);
				DUMP_FIELD_ARR(m_rotationAngleMin.v, 4);
				DUMP_FIELD_ARR(m_rotationAngleMax.v, 4);
				DUMP_FIELD_ARR(m_rotationAngleMax.v, 4);
				DUMP_FIELD_ARR(m_rotationRateMin.v, 4);
				DUMP_FIELD_ARR(m_rotationRateMin.v, 4);
				DUMP_FIELD_ARR(m_rotationRateMax.v, 4);
				DUMP_FIELD_ARR(m_rotationRateMax.v, 4);

				break;
			}
			case PARTICLE_MODULE_INIT_RUNNER:
			{
				auto* asset = &asset_->moduleData.initRunner;

				dump_particle_linked_asset_list(asset->m_linkedAssetList, data["AssetList"]);
				break;
			}
			case PARTICLE_MODULE_INIT_SOUND:
			{
				auto* asset = &asset_->moduleData.initSound;

				dump_particle_linked_asset_list(asset->m_linkedAssetList, data["AssetList"]);
				break;
			}
			case PARTICLE_MODULE_INIT_SPAWN:
			{
				auto* asset = &asset_->moduleData.initSpawn;

				for (auto i = 0; i < 1; i++)
				{
					dump_curve(&asset->m_curves[i], data["Curves"][i]);
				}
				break;
			}
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_BOX:
			{
				auto* asset = &asset_->moduleData.initSpawnShapeBox;

				DUMP_FIELD(m_axisFlags);
				DUMP_FIELD(m_spawnFlags);
				DUMP_FIELD(m_normalAxis);
				DUMP_FIELD(m_spawnType);
				DUMP_FIELD(m_volumeCubeRoot);
				DUMP_FIELD_ARR(m_offset.v, 4);

				DUMP_FIELD(m_useBeamInfo);
				DUMP_FIELD_ARR(m_dimensionsMin.v, 4);
				DUMP_FIELD_ARR(m_dimensionsMax.v, 4);

				DUMP_FIELD_ARR(m_pad, 15);

				break;
			}
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_CYLINDER:
			{
				auto* asset = &asset_->moduleData.initSpawnShapeCylinder;

				DUMP_FIELD(m_axisFlags);
				DUMP_FIELD(m_spawnFlags);
				DUMP_FIELD(m_normalAxis);
				DUMP_FIELD(m_spawnType);
				DUMP_FIELD(m_volumeCubeRoot);
				DUMP_FIELD_ARR(m_offset.v, 4);

				DUMP_FIELD(m_hasRotation);
				DUMP_FIELD(m_rotateCalculatedOffset);
				DUMP_FIELD(m_halfHeight);
				DUMP_FIELD(m_radius.min);
				DUMP_FIELD(m_radius.max);
				DUMP_FIELD_ARR(m_directionQuat.v, 4);

				DUMP_FIELD_ARR(unk.v, 4);

				break;
			}
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_ELLIPSOID:
			{
				auto* asset = &asset_->moduleData.initSpawnShapeEllipsoid;

				DUMP_FIELD(m_axisFlags);
				DUMP_FIELD(m_spawnFlags);
				DUMP_FIELD(m_normalAxis);
				DUMP_FIELD(m_spawnType);
				DUMP_FIELD(m_volumeCubeRoot);

				DUMP_FIELD_ARR(m_radiusMin.v, 4);
				DUMP_FIELD_ARR(m_radiusMax.v, 4);

				DUMP_FIELD_ARR(m_pad, 4);

				break;
			}

			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_MESH:
			{
				auto* asset = &asset_->moduleData.initSpawnShapeMesh;

				dump_particle_linked_asset_list(asset->m_linkedAssetList, data["AssetList"]);
				for (auto i = 0u; i < asset->m_numMeshAssets; i++)
				{
					data["meshdata"][i]["m_numVertsTotal"] = asset->m_meshAssetData[i].m_numVertsTotal;
					data["meshdata"][i]["m_numTrisTotal"] = asset->m_meshAssetData[i].m_numTrisTotal;
				}
				break;
			}
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_SPHERE:
			{
				auto* asset = &asset_->moduleData.initSpawnShapeSphere;

				DUMP_FIELD(m_axisFlags);
				DUMP_FIELD(m_spawnFlags);
				DUMP_FIELD(m_normalAxis);
				DUMP_FIELD(m_spawnType);
				DUMP_FIELD(m_volumeCubeRoot);

				DUMP_FIELD_ARR(m_offset.v, 4);
				DUMP_FIELD_ARR(m_offset2.v, 4);

				DUMP_FIELD(m_radius.min);
				DUMP_FIELD(m_radius.max);

				DUMP_FIELD_ARR(m_pad, 2);

				break;
			}
			case PARTICLE_MODULE_INIT_TAIL:
			{
				auto* asset = &asset_->moduleData.initTail;

				DUMP_FIELD(m_averagePastVelocities);
				DUMP_FIELD(m_maxParentSpeed);
				DUMP_FIELD(m_tailLeading);
				DUMP_FIELD(m_scaleWithVelocity);
				DUMP_FIELD(m_rotateAroundPivot);

				break;
			}
			case PARTICLE_MODULE_INIT_VECTOR_FIELD:
			{
				auto* asset = &asset_->moduleData.initVectorField;

				dump_particle_linked_asset_list(asset->m_linkedAssetList, data["AssetList"]);
				break;
			}
			case PARTICLE_MODULE_INIT_VOLUMETRIC:
			{
				auto* asset = &asset_->moduleData.initVolumetric;

				DUMP_FIELD(m_density);
				DUMP_FIELD(m_falloff);
				DUMP_FIELD(m_noiseAmplitude);
				DUMP_FIELD(m_noiseMode);

				DUMP_FIELD_ARR(m_pad, 2);

				break;
			}
			case PARTICLE_MODULE_ATTRACTOR:
			{
				auto* asset = &asset_->moduleData.attractor;

				DUMP_FIELD(m_killWhenNear);
				DUMP_FIELD(m_forceMag);
				DUMP_FIELD(m_nearDistanceSq);
				DUMP_FIELD_ARR(m_attractPoint.v, 4);

				DUMP_FIELD_ARR(m_pad, 3);

				break;
			}
			case PARTICLE_MODULE_COLOR_GRAPH:
			{
				auto* asset = &asset_->moduleData.colorGraph;

				DUMP_FIELD(firstCurve);
				DUMP_FIELD(m_modulateColorByAlpha);

				DUMP_FIELD_ARR(m_pad, 3);

				for (auto i = 0; i < 8; i++)
				{
					dump_curve(&asset->m_curves[i], data["Curves"][i]);
				}
				break;
			}
			case PARTICLE_MODULE_COLOR_LERP:
			{
				auto* asset = &asset_->moduleData.colorLerp;

				DUMP_FIELD_ARR(m_colorBegin.v, 4);
				DUMP_FIELD_ARR(m_colorEnd.v, 4);

				DUMP_FIELD_ARR(m_pad, 2);

				break;
			}
			case PARTICLE_MODULE_EMISSION_GRAPH:
			{
				auto* asset = &asset_->moduleData.emissionGraph;

				for (auto i = 0; i < 2; i++)
				{
					dump_curve(&asset->m_curves[i], data["Curves"][i]);
				}
				break;
			}
			case PARTICLE_MODULE_EMISSIVE_GRAPH:
			{
				auto* asset = &asset_->moduleData.emissiveGraph;

				DUMP_FIELD(firstCurve);

				DUMP_FIELD_ARR(m_pad, 7);

				for (auto i = 0; i < 2; i++)
				{
					dump_curve(&asset->m_curves[i], data["Curves"][i]);
				}
				break;
			}
			case PARTICLE_MODULE_FORCE:
			{
				auto* asset = &asset_->moduleData.force;

				DUMP_FIELD_ARR(m_forceMin.v, 4);
				DUMP_FIELD_ARR(m_forceMax.v, 4);

				break;
			}
			case PARTICLE_MODULE_GRAVITY:
			{
				auto* asset = &asset_->moduleData.gravity;

				DUMP_FIELD(m_gravityPercentage.min);
				DUMP_FIELD(m_gravityPercentage.max);

				break;
			}
			case PARTICLE_MODULE_INTENSITY_GRAPH:
			{
				auto* asset = &asset_->moduleData.intensityGraph;

				DUMP_FIELD(firstCurve);

				DUMP_FIELD_ARR(m_pad, 7);

				for (auto i = 0; i < 2; i++)
				{
					dump_curve(&asset->m_curves[i], data["Curves"][i]);
				}
				break;
			}

			case PARTICLE_MODULE_PARENT_VELOCITY_GRAPH:
			{
				auto* asset = &asset_->moduleData.velocityGraph;

				for (auto i = 0; i < 2; i++)
				{
					dump_curve(&asset->m_curves[i], data["Curves"][i]);
				}
				break;
			}
			case PARTICLE_MODULE_PHYSICS_LIGHT:
			{
				auto* asset = &asset_->moduleData.physicsLight;

				DUMP_FIELD(m_ignoreEmitterOrientation);
				DUMP_FIELD(m_useSurfaceType);

				DUMP_FIELD_ARR(m_pad, 6);

				dump_particle_linked_asset_list(asset->m_linkedAssetList, data["AssetList"]);
				break;
			}
			case PARTICLE_MODULE_PHYSICS_RAY_CAST:
			{
				auto* asset = &asset_->moduleData.physicsRayCast;

				DUMP_FIELD(m_bounce.min);
				DUMP_FIELD(m_bounce.max);
				DUMP_FIELD_ARR(m_bounds.midPoint, 3);
				DUMP_FIELD_ARR(m_bounds.halfSize, 3);
				DUMP_FIELD(m_useItemClip);
				DUMP_FIELD(m_useSurfaceType);
				DUMP_FIELD(m_collideWithWater);
				DUMP_FIELD(m_ignoreContentItem);

				DUMP_FIELD_ARR(m_pad, 3);

				break;
			}
			case PARTICLE_MODULE_POSITION_GRAPH:
			{
				auto* asset = &asset_->moduleData.positionGraph;

				for (auto i = 0; i < 6; i++)
				{
					dump_curve(&asset->m_curves[i], data["Curves"][i]);
				}
				break;
			}
			case PARTICLE_MODULE_ROTATION_GRAPH:
			{
				auto* asset = &asset_->moduleData.rotationGraph;

				DUMP_FIELD(m_useRotationRate);

				DUMP_FIELD_ARR(m_pad, 1);

				for (auto i = 0; i < 2; i++)
				{
					dump_curve(&asset->m_curves[i], data["Curves"][i]);
				}
				break;
			}

			case PARTICLE_MODULE_ROTATION_GRAPH_3D:
			{
				auto* asset = &asset_->moduleData.rotationGraph3D;

				DUMP_FIELD(m_useRotationRate);

				DUMP_FIELD_ARR(m_pad, 1);

				for (auto i = 0; i < 6; i++)
				{
					dump_curve(&asset->m_curves[i], data["Curves"][i]);
				}
				break;
			}
			case PARTICLE_MODULE_SIZE_GRAPH:
			{
				auto* asset = &asset_->moduleData.sizeGraph;

				DUMP_FIELD(firstCurve);
				DUMP_FIELD_ARR(m_sizeBegin.v, 4);
				DUMP_FIELD_ARR(m_sizeEnd.v, 4);

				DUMP_FIELD_ARR(m_pad, 7);

				for (auto i = 0; i < 6; i++)
				{
					dump_curve(&asset->m_curves[i], data["Curves"][i]);
				}
				break;
			}

			case PARTICLE_MODULE_SIZE_LERP:
			{
				auto* asset = &asset_->moduleData.sizeLerp;

				DUMP_FIELD_ARR(m_sizeBegin.v, 4);
				DUMP_FIELD_ARR(m_sizeEnd.v, 4);

				DUMP_FIELD_ARR(m_pad, 2);

				break;
			}
			case PARTICLE_MODULE_VELOCITY_GRAPH:
			{
				auto* asset = &asset_->moduleData.velocityGraph;

				DUMP_FIELD_ARR(m_velocityBegin.v, 4);
				DUMP_FIELD_ARR(m_velocityEnd.v, 4);

				for (auto i = 0; i < 6; i++)
				{
					dump_curve(&asset->m_curves[i], data["Curves"][i]);
				}
				break;
			}
			case PARTICLE_MODULE_TEST_AGE:
			{
				auto* asset = &asset_->moduleData.testAge;

				DUMP_FIELD(m_moduleIndex);
				DUMP_FIELD_ARR(m_useOrientationOptions, 1);
				DUMP_FIELD_ARR(m_useScaleOptions, 1);
				DUMP_FIELD_ARR(m_useVelocityOptions, 1);
				DUMP_FIELD(m_eventHandlerData.m_nextState);
				DUMP_FIELD(m_eventHandlerData.m_kill);
				DUMP_FIELD_ARR(m_eventHandlerData.m_pad, 1);

				DUMP_FIELD(m_age);

				dump_particle_linked_asset_list(asset->m_eventHandlerData.m_linkedAssetList, data["AssetList"]);
				break;
			}
			case PARTICLE_MODULE_TEST_BIRTH:
			{
				auto* asset = &asset_->moduleData.testBirth;

				DUMP_FIELD(m_moduleIndex);
				DUMP_FIELD_ARR(m_useOrientationOptions, 1);
				DUMP_FIELD_ARR(m_useScaleOptions, 1);
				DUMP_FIELD_ARR(m_useVelocityOptions, 1);
				DUMP_FIELD(m_eventHandlerData.m_nextState);
				DUMP_FIELD(m_eventHandlerData.m_kill);
				DUMP_FIELD_ARR(m_eventHandlerData.m_pad, 1);

				dump_particle_linked_asset_list(asset->m_eventHandlerData.m_linkedAssetList, data["AssetList"]);
				break;
			}
			case PARTICLE_MODULE_TEST_DEATH:
			{
				auto* asset = &asset_->moduleData.testDeath;

				DUMP_FIELD(m_moduleIndex);
				DUMP_FIELD_ARR(m_useOrientationOptions, 1);
				DUMP_FIELD_ARR(m_useScaleOptions, 1);
				DUMP_FIELD_ARR(m_useVelocityOptions, 1);
				DUMP_FIELD(m_eventHandlerData.m_nextState);
				DUMP_FIELD(m_eventHandlerData.m_kill);
				DUMP_FIELD_ARR(m_eventHandlerData.m_pad, 1);

				dump_particle_linked_asset_list(asset->m_eventHandlerData.m_linkedAssetList, data["AssetList"]);
				break;
			}
			case PARTICLE_MODULE_TEST_GRAVITY:
			{
				auto* asset = &asset_->moduleData.testGravity;

				DUMP_FIELD(m_moduleIndex);
				DUMP_FIELD_ARR(m_useOrientationOptions, 1);
				DUMP_FIELD_ARR(m_useScaleOptions, 1);
				DUMP_FIELD_ARR(m_useVelocityOptions, 1);
				DUMP_FIELD(m_eventHandlerData.m_nextState);
				DUMP_FIELD(m_eventHandlerData.m_kill);
				DUMP_FIELD_ARR(m_eventHandlerData.m_pad, 1);

				dump_particle_linked_asset_list(asset->m_eventHandlerData.m_linkedAssetList, data["AssetList"]);
				break;
			}
			case PARTICLE_MODULE_TEST_IMPACT:
			{
				auto* asset = &asset_->moduleData.testPos;

				DUMP_FIELD(m_moduleIndex);
				DUMP_FIELD_ARR(m_useOrientationOptions, 1);
				DUMP_FIELD_ARR(m_useScaleOptions, 1);
				DUMP_FIELD_ARR(m_useVelocityOptions, 1);
				DUMP_FIELD(m_eventHandlerData.m_nextState);
				DUMP_FIELD(m_eventHandlerData.m_kill);
				DUMP_FIELD_ARR(m_eventHandlerData.m_pad, 1);

				dump_particle_linked_asset_list(asset->m_eventHandlerData.m_linkedAssetList, data["AssetList"]);
				break;
			}
			case PARTICLE_MODULE_TEST_POS:
			{
				auto* asset = &asset_->moduleData.testImpact;

				DUMP_FIELD(m_moduleIndex);
				DUMP_FIELD_ARR(m_useOrientationOptions, 1);
				DUMP_FIELD_ARR(m_useScaleOptions, 1);
				DUMP_FIELD_ARR(m_useVelocityOptions, 1);
				DUMP_FIELD(m_eventHandlerData.m_nextState);
				DUMP_FIELD(m_eventHandlerData.m_kill);
				DUMP_FIELD_ARR(m_eventHandlerData.m_pad, 1);

				dump_particle_linked_asset_list(asset->m_eventHandlerData.m_linkedAssetList, data["AssetList"]);
				break;
			}
			default:
				__debugbreak();
			}
		}

#undef DUMP_TYPE

		void dump_module_group(ParticleModuleGroupDef* asset, ordered_json& data)
		{
			DUMP_FIELD(disabled);
			for (auto i = 0; i < asset->numModules; i++)
			{
				dump_module(&asset->moduleDefs[i], data["Module"][i]);
			}
		}

		void dump_state(ParticleStateDef* asset, ordered_json& data)
		{
			data["elementType"] = PARTICLE_ELEMENT_TYPE_s[asset->elementType]; //DUMP_FIELD(elementType);
			DUMP_FIELD(flags);
			for (auto i = 0; i < 3; i++)
			{
				dump_module_group(&asset->moduleGroupDefs[i], data["ModuleGroup"][ParticleModuleGroup_s[i]]);
			}
		}

		void dump_emitter(ParticleEmitterDef* asset, ordered_json& data)
		{
			DUMP_FIELD(particleSpawnRate.min);
			DUMP_FIELD(particleSpawnRate.max);

			DUMP_FIELD(particleLife.min);
			DUMP_FIELD(particleLife.max);

			DUMP_FIELD(particleDelay.min);
			DUMP_FIELD(particleDelay.max);

			DUMP_FIELD(particleCountMax);

			DUMP_FIELD(particleBurstCount.min);
			DUMP_FIELD(particleBurstCount.max);

			DUMP_FIELD(emitterLife.min);
			DUMP_FIELD(emitterLife.max);

			DUMP_FIELD(emitterDelay.min);
			DUMP_FIELD(emitterDelay.max);

			DUMP_FIELD(randomSeed);

			DUMP_FIELD(spawnRangeSq.min);
			DUMP_FIELD(spawnRangeSq.max);

			DUMP_FIELD(fadeOutMaxDistance);

			dump_curve(&asset->fadeCurveDef, data["FadeCurve"]);

			DUMP_FIELD(spawnFrustumCullRadius);

			DUMP_FIELD(gravityOptions);

			DUMP_FIELD_ARR(groupIDs, 4);

			DUMP_FIELD(unk1);
			DUMP_FIELD(unk2);

			DUMP_FIELD(particleSpawnShapeRange.min);
			DUMP_FIELD(particleSpawnShapeRange.max);

			DUMP_FIELD_ARR(pad, 1);

			DUMP_FIELD(flags);

			for (auto i = 0; i < asset->numStates; i++)
			{
				dump_state(&asset->stateDefs[i], data["State"][i]);
			}
		}

		void dump_scripted_input_node(ParticleScriptedInputNodeDef* asset, ordered_json& data)
		{
			DUMP_FIELD(nodeBase.m_type);
			DUMP_FIELD(nodeBase.m_flags);
			DUMP_FIELD(nodeBase.m_emitterIndex);

			DUMP_FIELD(KVP.m_inputRecord.key);
			DUMP_FIELD(KVP.m_inputRecord.dirty);
			DUMP_FIELD(KVP.m_inputRecord.value);
		}

		void dump(ParticleSystemDef* asset)
		{
			const auto path = "particlesystem\\"s + asset->name + ".json"s;

			ordered_json data;

			DUMP_STRING(name);

			DUMP_FIELD(version);
			DUMP_FIELD(flags);

			for (auto i = 0; i < asset->numEmitters; i++)
			{
				dump_emitter(&asset->emitterDefs[i], data["Emitter"][i]);
			}

			for (auto i = 0; i < asset->numScriptedInputNodes; i++)
			{
				dump_scripted_input_node(&asset->scriptedInputNodeDefs[i], data["scriptedInputNode"][i]);
			}

			DUMP_FIELD(occlusionOverrideEmitterIndex);
			DUMP_FIELD(phaseOptions);
			DUMP_FIELD(drawFrustumCullRadius);
			DUMP_FIELD(updateFrustumCullRadius);
			DUMP_FIELD(sunDistance);
			DUMP_FIELD(preRollMSec);

			DUMP_FIELD_ARR(editorPosition.v, 4);
			DUMP_FIELD_ARR(editorRotation.v, 4);
			DUMP_FIELD_ARR(gameTweakPosition.v, 4);
			DUMP_FIELD_ARR(gameTweakRotation.v, 4);

			std::string json_str = data.dump(4);
			auto file = filesystem::file(path);
			file.open("wb");
			file.write(json_str.data(), json_str.size(), 1);
			file.close();
		}
	}

	void IParticleSystem::dump(ParticleSystemDef* asset)
	{
#ifdef DUMP_JSON
		vfx::json::dump(asset);
#else
		vfx::binary::dump(asset);
#endif
	}
}