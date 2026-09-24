#pragma once

namespace ZoneTool::IW5
{
	namespace IW7Converter
	{
		IW7::ParticleSystemDef* convert_to_vfx(FxEffectDef* asset, allocator& allocator);
	}
}