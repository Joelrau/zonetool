#include "stdafx.hpp"

namespace ZoneTool::IW6
{
	void IXModel::dump(XModel* asset)
	{
		const auto path = "xmodel\\"s + asset->name + ".xmb";

		assetmanager::dumper dump;
		if (!dump.open(path))
		{
			return;
		}

		// asset
		dump.dump_single(asset);
		dump.dump_string(asset->name);

		// tags
		dump.dump_array(asset->boneNames, asset->numBones);
		for (unsigned char i = 0; i < asset->numBones; i++)
		{
			dump.dump_string(SL_ConvertToString(asset->boneNames[i]));
		}

		// basic info
		dump.dump_array(asset->parentList, asset->numBones - asset->numRootBones);
		dump.dump_array(asset->tagAngles, asset->numBones - asset->numRootBones);
		dump.dump_array(asset->tagPositions, asset->numBones - asset->numRootBones);
		dump.dump_array(asset->partClassification, asset->numBones);
		dump.dump_array(asset->baseMat, asset->numBones);
		dump.dump_array(asset->reactiveMotionParts, asset->numReactiveMotionParts);
		dump.dump_array(asset->collSurfs, asset->numCollSurfs);
		dump.dump_array(asset->boneInfo, asset->numBones);
		dump.dump_array(asset->invHighMipRadius, asset->numsurfs);

		// surfaces
		dump.dump_array(asset->materialHandles, asset->numsurfs);
		for (unsigned char i = 0; i < asset->numsurfs; i++)
		{
			dump.dump_asset(asset->materialHandles[i]);
		}

		// lods
		for (auto i = 0; i < 6; i++)
		{
			dump.dump_asset(asset->lodInfo[i].modelSurfs);
		}

		// physics subassets
		dump.dump_asset(asset->physPreset);
		dump.dump_asset(asset->physCollmap);

		dump.close();
	}
}