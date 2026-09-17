#pragma once

namespace ZoneTool::IW5
{
	namespace IW7Converter
	{
		IW7::XModel* convert(XModel* asset, allocator& allocator);

		// A clutter dynent references this model but IW5 gives it no PhysCollmap to build
		// a dynamic shape from. The model converter then falls back to a box over the
		// model's bounds at `mass` (CoD4 did the same for geom-less dynents). Registered
		// by the clipmap converter; the clipmap dumper re-dumps the model afterwards so
		// the box asset replaces whatever was written for it earlier.
		void request_dynamic_box(const std::string& model, float mass);
		bool wants_dynamic_box(const std::string& model, float* mass);

		// IW5 surfaceFlags (surface type in the top bits plus the SURF_FLAG_* bits) -> IW7.
		int convert_surf_flags(int flags);
	}
}