#include "stdafx.hpp"

#include "XModel.hpp"
#include "Converter/IW7/Assets/XModel.hpp"
#include "IW7/Assets/XModel.hpp"

namespace ZoneTool::IW5::IW7Dumper
{
	void dump(XModel* asset)
	{
		// generate IW7 model
		allocator allocator;
		auto iw7_asset = IW7Converter::convert(asset, allocator);

		// dump IW7 model
		IW7::IXModel::dump(iw7_asset);
	}
}