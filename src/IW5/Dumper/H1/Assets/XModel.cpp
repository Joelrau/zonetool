#include "stdafx.hpp"

#include "XModel.hpp"
#include "Converter/H1/Assets/XModel.hpp"
#include "H1/Assets/XModel.hpp"

namespace ZoneTool::IW5::H1Dumper
{
	void dump(XModel* asset)
	{
		// generate h1 model
		allocator allocator;
		auto h1_asset = H1Converter::convert(asset, allocator);

		// dump h1 model
		H1::IXModel::dump(h1_asset);
	}
}