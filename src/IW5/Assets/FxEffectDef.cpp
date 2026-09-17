#include "stdafx.hpp"
#include "Material.hpp"
#include "FxMaterialDeps.hpp"

#include "Dumper/H1/Assets/FxEffectDef.hpp"
#include "Dumper/IW6/Assets/FxEffectDef.hpp"
#include "Dumper/S1/Assets/FxEffectDef.hpp"
#include "Dumper/IW7/Assets/FxEffectDef.hpp"

namespace ZoneTool::IW5
{
	void IFxEffectDef::dump(FxEffectDef* asset)
	{
		if (zonetool::dumping_target == zonetool::dump_target::h1)
		{
			return H1Dumper::dump(asset);
		}
		else if (zonetool::dumping_target == zonetool::dump_target::iw6)
		{
			return IW6Dumper::dump(asset);
		}
		else if (zonetool::dumping_target == zonetool::dump_target::s1)
		{
			return S1Dumper::dump(asset);
		}
		else if (zonetool::dumping_target == zonetool::dump_target::iw7)
		{
			// IW3/IW4 zones reach this with pointers of their own games, their dumpers handle those
			if (get_linker_mode() == linker_mode::iw5)
			{
				dump_fx_materials_first(asset, FX_ELEM_TYPE_SPARKCLOUD, FX_ELEM_TYPE_DECAL, &IMaterial::dump);
			}

			return IW7Dumper::dump(asset);
		}
	}
}