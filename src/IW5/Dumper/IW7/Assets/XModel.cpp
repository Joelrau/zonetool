#include "stdafx.hpp"

#include "XModel.hpp"
#include "Converter/IW7/Assets/XModel.hpp"
#include "IW7/Assets/XModel.hpp"
#include "IW7/Assets/PhysicsAsset.hpp"

namespace ZoneTool::IW5::IW7Dumper
{
	void dump(XModel* asset)
	{
		// generate IW7 model
		allocator allocator;
		auto iw7_asset = IW7Converter::convert(asset, allocator);

		// dump IW7 model
		// The XModel only writes a name reference to its physics asset, so the asset itself has
		// to be dumped alongside or the collision never reaches the zone.
		if (iw7_asset->physicsAsset)
		{
			IW7::IPhysicsAsset::dump(iw7_asset->physicsAsset);
		}

		IW7::IXModel::dump(iw7_asset);
	}
}