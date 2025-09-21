#include "stdafx.hpp"
#include "IW5/Assets/StringTable.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		void IStringTable::dump(StringTable* asset)
		{
			// dump stringtable
			IW5::IStringTable::dump(reinterpret_cast<IW5::StringTable*>(asset));
		}
	}
}