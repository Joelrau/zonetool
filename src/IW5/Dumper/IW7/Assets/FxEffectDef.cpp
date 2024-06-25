#include "stdafx.hpp"

#include "FxEffectDef.hpp"

#define CONVERT_TO_PARTICLE_SYSTEM

#ifdef CONVERT_TO_PARTICLE_SYSTEM
#include "Converter/IW7/Assets/ParticleSystem.hpp"
#include "IW7/Assets/ParticleSystem.hpp"
#else
#include "Converter/IW7/Assets/FxEffectDef.hpp"
#include "IW7/Assets/FxEffectDef.hpp"
#endif

namespace ZoneTool::IW5::IW7Dumper
{
	void dump(FxEffectDef* asset)
	{
#ifdef CONVERT_TO_PARTICLE_SYSTEM
		// generate IW7 vfx
		allocator allocator;
		IW7::ParticleSystemDef* iw7_asset = IW7Converter::convert(asset, allocator);

		// dump vfx
		IW7::IParticleSystem::dump(iw7_asset);
#else
		// generate IW7 fx
		allocator allocator;
		IW7::FxEffectDef* iw7_asset = IW7Converter::convert(asset, allocator);

		// dump fx
		IW7::IFxEffectDef::dump(iw7_asset);
#endif
	}
}