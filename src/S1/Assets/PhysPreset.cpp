#include "stdafx.hpp"

#include "PhysPreset.hpp"

#include <H1\Assets\PhysPreset.hpp>

namespace ZoneTool::S1
{
	void IPhysPreset::dump(PhysPreset* asset)
	{
		H1::IPhysPreset::dump(reinterpret_cast<H1::PhysPreset*>(asset));
	}
}
