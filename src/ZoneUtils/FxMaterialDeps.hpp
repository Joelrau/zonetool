#pragma once

#include <string>
#include <unordered_set>

namespace ZoneTool
{
	// IW7 renames materials by techset (eq/, mc/, ...) and records the new name only when the material is
	// dumped; effects resolve their references through that record. Map fastfiles list the materials an effect
	// uses as additional assets *after* the effect, so without dumping them first every such reference keeps the
	// source name and points at a material that is never written (invisible sprites and decals).
	//
	// Call this from the dumper of the game the zone actually came from: the visual pointers are that game's own
	// Material structs, and converting through a later game's dumper would misread them.
	template <typename TFxEffectDef, typename TMaterial>
	void dump_fx_materials_first(TFxEffectDef* asset, int last_material_elem_type, int decal_elem_type, void (*dump_material)(TMaterial*))
	{
		// shared between effects; skipping a material here is harmless because the fastfile's own dump of it
		// still runs afterwards
		static std::unordered_set<std::string> dumped;
		const auto dump = [&](TMaterial* material)
		{
			if (material && material->name && dumped.emplace(material->name).second)
			{
				dump_material(material);
			}
		};

		const auto elem_count = asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission;
		for (auto i = 0; i < elem_count; i++)
		{
			const auto* elem = &asset->elemDefs[i];
			if (!elem->visualCount)
			{
				continue;
			}

			const auto type = static_cast<int>(elem->elemType);
			if (type == decal_elem_type)
			{
				for (auto j = 0; elem->visuals.markArray && j < elem->visualCount; j++)
				{
					dump(elem->visuals.markArray[j].materials[0]);
					dump(elem->visuals.markArray[j].materials[1]);
				}
			}
			else if (type <= last_material_elem_type)
			{
				for (auto j = 0; j < elem->visualCount; j++)
				{
					dump(elem->visualCount > 1 ? elem->visuals.array[j].material : elem->visuals.instance.material);
				}
			}
		}
	}
}
