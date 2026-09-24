#include "stdafx.hpp"

#include "FxEffectDef.hpp"

#include "Converter/IW7/Assets/ParticleSystem.hpp"
#include "IW7/Assets/ParticleSystem.hpp"
#include "Converter/IW7/Assets/FxEffectDef.hpp"
#include "IW7/Assets/FxEffectDef.hpp"

namespace ZoneTool::IW5::IW7Dumper
{
	void dump(FxEffectDef* asset)
	{
		allocator allocator;

		if (zonetool::iw7_effect_use_vfx)
		{
			// generate IW7 vfx
			IW7::ParticleSystemDef* iw7_asset = IW7Converter::convert_to_vfx(asset, allocator);

			// dump vfx
			IW7::IParticleSystem::dump(iw7_asset);
		}
		else
		{
			// generate IW7 fx
			IW7::FxEffectDef* iw7_asset = IW7Converter::convert_to_fx(asset, allocator);

			// dump fx
			IW7::IFxEffectDef::dump(iw7_asset);
		}
	}
}
