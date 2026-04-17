#include "stdafx.hpp"

#include "XModel.hpp"
#include "Converter/IW6/Assets/XModel.hpp"
#include "IW6/Assets/XModel.hpp"

namespace ZoneTool::IW5::IW6Dumper
{
	void dump(XModel* asset)
	{
		// generate IW6 model
		allocator allocator;
		auto iw6_asset = IW6Converter::convert(asset, allocator);

		// dump IW6 model
		IW6::IXModel::dump(iw6_asset);
	}
}