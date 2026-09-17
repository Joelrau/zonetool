#include "stdafx.hpp"
#include "IW5/Assets/FxEffectDef.hpp"
#include "Material.hpp"
#include "FxMaterialDeps.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		void IFxEffectDef::dump(FxEffectDef* asset)
		{
			// IW3 zones pass through here with IW3 material pointers, they are handled in IW3's dumper
			if (get_linker_mode() == linker_mode::iw4 && zonetool::dumping_target == zonetool::dump_target::iw7)
			{
				dump_fx_materials_first(asset, FX_ELEM_TYPE_SPARKCLOUD, FX_ELEM_TYPE_DECAL, &IMaterial::dump);
			}

			allocator allocator;

			// alloc comworld
			auto* iw5_fx = allocator.allocate<IW5::FxEffectDef>();
			memcpy(iw5_fx, asset, sizeof FxEffectDef);
			//memset(iw5_fx->pad, 0, sizeof iw5_fx->pad);

			// alloc elemdefs
			const auto elem_def_count = iw5_fx->elemDefCountEmission + iw5_fx->elemDefCountLooping + iw5_fx->elemDefCountOneShot;
			iw5_fx->elemDefs = allocator.allocate<IW5::FxElemDef>(elem_def_count);

			// transform elemdefs to iw5 format
			for (auto i = 0; i < elem_def_count; i++)
			{
				memcpy(&iw5_fx->elemDefs[i], &asset->elemDefs[i], sizeof(IW4::FxElemDef));
				iw5_fx->elemDefs[i].randomSeed = 0;
			}

			IW5::IFxEffectDef::dump(iw5_fx);
		}
	}
}