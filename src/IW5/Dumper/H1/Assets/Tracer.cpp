#include "stdafx.hpp"

#include "Tracer.hpp"
#include "Converter/H1/Assets/Tracer.hpp"
#include "H1/Assets/TracerDef.hpp"

namespace ZoneTool::IW5::H1Dumper
{
	void dump(TracerDef* asset)
	{
		// generate h1 asset
		allocator allocator;
		auto* h1_asset = H1Converter::convert(asset, allocator);

		// dump soundcurve
		H1::ITracerDef::dump(h1_asset);
	}
}