#include "stdafx.hpp"
#include "H1/Assets/GfxImage.hpp"
#include "IW5/Structs.hpp"

#include "IW5/Assets/GfxImage.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		void IGfxImage::dump(GfxImage* asset, ZoneMemory* mem)
		{
			// as of writing this, these individual structs are definitely wrong but they seem to be same size in bytes?
			// this should be fine for now, but if stuff starts looking weird, its not a bad assumption to assume it may be this
			IW5::IGfxImage::dump(reinterpret_cast<IW5::GfxImage*>(asset), mem);
		}
	}
}