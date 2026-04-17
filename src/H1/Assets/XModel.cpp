#include "stdafx.hpp"

namespace ZoneTool::H1
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
		dump.dump_single(asset->reactiveMotionTweaks);
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

		// weights
		dump.dump_array(asset->blendShapeWeightNames, asset->numberOfBlendShapeWeights);
		for (unsigned short i = 0; i < asset->numberOfBlendShapeWeights; i++)
		{
			dump.dump_string(SL_ConvertToString(asset->blendShapeWeightNames[i]));
		}

		// blendshapeweights
		dump.dump_array(asset->blendShapeWeightMaps, asset->numberOfBlendShapeWeightMaps);

		// mdao
		dump.dump_array(asset->mdaoVolumes, asset->mdaoVolumeCount);
		for (auto i = 0; i < asset->mdaoVolumeCount; i++)
		{
			dump.dump_asset(asset->mdaoVolumes[i].volumeData);
		}

		// extra models
		dump.dump_array(asset->compositeModels, asset->numCompositeModels);
		for (char i = 0; i < asset->numCompositeModels; i++)
		{
			dump.dump_asset(asset->compositeModels[i]);
		}

		// skeletonscript subasset
		dump.dump_asset(asset->skeletonScript);

		// bone physics
		dump.dump_array(asset->bonePhysics, asset->numBonePhysics);
		for (char i = 0; i < asset->numBonePhysics; i++)
		{
			dump.dump_asset(asset->bonePhysics[i].physPreset);
			dump.dump_asset(asset->bonePhysics[i].physContraint);
			dump.dump_asset(asset->bonePhysics[i].physCollmap);
		}

		dump.close();
	}
}