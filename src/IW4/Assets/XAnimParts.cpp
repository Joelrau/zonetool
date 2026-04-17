#include "stdafx.hpp"
#include "IW5/Assets/XAnimParts.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		void IXAnimParts::dump(XAnimParts* asset)
		{
			// dump anims
			IW5::IXAnimParts::dump(reinterpret_cast<IW5::XAnimParts*>(asset));
		}
	}
}