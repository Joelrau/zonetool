#include "stdafx.hpp"

#include "PhysCollmap.hpp"
#include "Converter/H1/Assets/PhysCollmap.hpp"
#include "H1/Assets/PhysCollmap.hpp"

namespace ZoneTool::IW5::H1Dumper
{
	void dump(PhysCollmap* asset)
	{
		// generate h1 asset
		allocator allocator;
		auto* h1_asset = H1Converter::convert(asset, allocator);

		// dump asset
		H1::IPhysCollmap::dump(h1_asset);
	}
}