#pragma once

namespace ZoneTool::IW5
{
	namespace IW7Converter
	{
		IW7::XModel* convert(XModel* asset, allocator& allocator);

		// IW5 surfaceFlags (surface type in the top bits plus the SURF_FLAG_* bits) -> IW7.
		int convert_surf_flags(int flags);
	}
}