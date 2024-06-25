#include "stdafx.hpp"

namespace ZoneTool::IW7
{
	void IXModel::dump(XModel* asset, const std::function<const char* (std::uint16_t)>& SL_ConvertToString)
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
		for (auto i = 0; i < asset->numBones + asset->numClientBones; i++)
		{
			dump.dump_string(SL_ConvertToString(asset->boneNames[i]));
		}

		// basic info
		dump.dump_array(asset->parentList, asset->numBones + asset->numClientBones - asset->numRootBones);
		dump.dump_array(asset->tagAngles, asset->numBones + asset->numClientBones - asset->numRootBones);
		dump.dump_array(asset->tagPositions, asset->numBones + asset->numClientBones - asset->numRootBones);
		dump.dump_array(asset->partClassification, asset->numBones);
		dump.dump_array(asset->baseMat, asset->numBones + asset->numClientBones);
		dump.dump_array(asset->reactiveMotionParts, asset->numReactiveMotionParts);
		dump.dump_single(asset->reactiveMotionTweaks);
		dump.dump_array(asset->collSurfs, asset->numCollSurfs);
		dump.dump_array(asset->boneInfo, asset->numBones + asset->numClientBones);
		dump.dump_array(asset->invHighMipRadius, asset->numsurfs);

		// surfaces
		dump.dump_array(asset->materialHandles, asset->numsurfs);
		for (unsigned int i = 0; i < asset->numsurfs; i++)
		{
			dump.dump_asset(asset->materialHandles[i]);
		}

		// lods
		for (auto i = 0; i < 6; i++)
		{
			dump.dump_asset(asset->lodInfo[i].modelSurfs);
		}

		// physics
		dump.dump_asset(asset->physicsAsset);
		dump.dump_asset(asset->physicsFXShape);

		// unknown
		dump.dump_array(asset->physicsLODData, asset->physicsLODDataSize);

		for (unsigned int i = 0; i < asset->physicsLODDataNameCount; i++)
		{
			dump.dump_string(SL_ConvertToString(asset->physicsLODDataNames[i]));
		}

		dump.dump_asset(asset->scriptableMoverDef);
		dump.dump_asset(asset->proceduralBones);

		for (unsigned int i = 0; i < asset->numAimAssistBones; i++)
		{
			dump.dump_string(SL_ConvertToString(asset->aimAssistBones[i]));
		}

		dump.dump_array(asset->unknown03, asset->unknown03Count);
		dump.dump_array(asset->unknownVec3, asset->unknownVec3Count);
		dump.dump_array(asset->unknown04, asset->unknown04Count);

		dump.close();
	}
}