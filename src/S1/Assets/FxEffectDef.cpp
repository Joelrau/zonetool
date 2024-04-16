#include "stdafx.hpp"

#include <H1\Assets\FxEffectDef.hpp>
#include <H1\Structs.hpp>

namespace ZoneTool::S1
{
	void IFxEffectDef::dump(FxEffectDef* asset)
	{
		H1::IFxEffectDef::dump(reinterpret_cast<H1::FxEffectDef*>(asset));
	}
}
