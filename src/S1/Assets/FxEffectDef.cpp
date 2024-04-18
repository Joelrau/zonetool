#include "stdafx.hpp"

#include "FxEffectDef.hpp"

#include <H1\Assets\FxEffectDef.hpp>

namespace ZoneTool::S1
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
		case FX_ELEM_TYPE_VECTORFIELD:
			dump->dump_string(vis->soundName);
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

	void IFxEffectDef::dump(FxEffectDef* asset)
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
