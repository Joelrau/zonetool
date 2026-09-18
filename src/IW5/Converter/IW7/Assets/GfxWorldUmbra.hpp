#pragma once

namespace ZoneTool::IW5
{
	namespace IW7Converter
	{
		// Builds the IW7 GfxWorld's static occlusion tome (umbraTomeData/umbraTomeSize)
		// from the IW5 world's geometry and the IW7 world's renderable indexing. Falls
		// back to the draw-everything tome, with a warning, if the generator is not
		// available or fails; a converted map must never ship without a tome.
		void generate_umbra_tome(const GfxWorld* asset, IW7::GfxWorld* world, allocator& allocator);
	}
}
