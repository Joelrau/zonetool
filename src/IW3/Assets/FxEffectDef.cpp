#include "stdafx.hpp"
#include "IW4/Assets/FxEffectDef.hpp"

namespace ZoneTool
{
	namespace IW3
	{
		IW4::FxEffectDef* GenerateIW4FxEffectDef(FxEffectDef* asset, allocator& mem)
		{
			const auto iw4_fx = mem.allocate<IW4::FxEffectDef>();
			memcpy(iw4_fx, asset, sizeof(IW4::FxEffectDef));

			const auto elem_count = iw4_fx->elemDefCountEmission + iw4_fx->elemDefCountOneShot + iw4_fx->elemDefCountLooping;
			iw4_fx->elemDefs = mem.allocate<IW4::FxElemDef>(elem_count);

			for (auto i = 0; i < elem_count; i++)
			{
				memcpy(&iw4_fx->elemDefs[i], &asset->elemDefs[i], sizeof IW4::FxElemDef);

				if (iw4_fx->elemDefs[i].elemType >= 5)
				{
					iw4_fx->elemDefs[i].elemType += 2;
				}

				bounds::compute(iw4_fx->elemDefs[i].collBounds.midPoint, iw4_fx->elemDefs[i].collBounds.halfSize, &iw4_fx->elemDefs[i].collBounds.midPoint);

				if (iw4_fx->elemDefs[i].elemType == 3 && iw4_fx->elemDefs[i].extended.trailDef)
				{
					iw4_fx->elemDefs[i].extended.trailDef = mem.allocate<IW4::FxTrailDef>();
					iw4_fx->elemDefs[i].extended.trailDef->scrollTimeMsec = asset->elemDefs[i].trailDef->scrollTimeMsec;
					iw4_fx->elemDefs[i].extended.trailDef->repeatDist = asset->elemDefs[i].trailDef->repeatDist;
					iw4_fx->elemDefs[i].extended.trailDef->vertCount = asset->elemDefs[i].trailDef->vertCount;
					iw4_fx->elemDefs[i].extended.trailDef->verts = reinterpret_cast<IW4::FxTrailVertex*>(asset->elemDefs[i].trailDef->verts);
					iw4_fx->elemDefs[i].extended.trailDef->indCount = asset->elemDefs[i].trailDef->indCount;
					iw4_fx->elemDefs[i].extended.trailDef->inds = asset->elemDefs[i].trailDef->inds;
				}
				else
				{
					iw4_fx->elemDefs[i].extended.trailDef = nullptr;
				}

				if (asset->elemDefs[i].elemType == FX_ELEM_TYPE_OMNI_LIGHT || asset->elemDefs[i].elemType == FX_ELEM_TYPE_SPOT_LIGHT)
				{
					for (int j = 0; j < asset->elemDefs[i].visStateIntervalCount + 1; j++)
					{
						iw4_fx->elemDefs[i].visSamples[j].base.scale = 1.0f;
						iw4_fx->elemDefs[i].visSamples[j].amplitude.scale = 1.0f;
					}
				}
			}
			return iw4_fx;
		}

		void IFxEffectDef::dump(FxEffectDef* asset)
		{
			// generate fx
			allocator allocator;
			auto* iw4_asset = GenerateIW4FxEffectDef(asset, allocator);

			// dump fx
			IW4::IFxEffectDef::dump(iw4_asset);
		}
	}
}