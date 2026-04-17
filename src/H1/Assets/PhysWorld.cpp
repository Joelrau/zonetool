#include "stdafx.hpp"

namespace ZoneTool::H1
{
	void IPhysWorld::dump(PhysWorld* asset)
	{
		const auto path = asset->name + ".physmap"s;

		assetmanager::dumper write;
		if (!write.open(path))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);

		write.dump_array(asset->brushModels, asset->brushModelCount);
		write.dump_array(asset->polytopeDatas, asset->polytopeCount);
		for (unsigned int i = 0; i < asset->polytopeCount; i++)
		{
			write.dump_array(asset->polytopeDatas[i].m_aVertices, asset->polytopeDatas[i].m_vertexCount);
			write.dump_array(asset->polytopeDatas[i].m_aPlanes, asset->polytopeDatas[i].m_faceCount);
			write.dump_array(asset->polytopeDatas[i].m_surfaceTypes, asset->polytopeDatas[i].m_faceCount);
			write.dump_array(asset->polytopeDatas[i].m_vertexMaterials, asset->polytopeDatas[i].m_vertexCount);
			write.dump_array(asset->polytopeDatas[i].m_aSubEdges, asset->polytopeDatas[i].m_subEdgeCount);
			write.dump_array(asset->polytopeDatas[i].m_aFaceSubEdges, asset->polytopeDatas[i].m_faceCount);
		}
		write.dump_array(asset->meshDatas, asset->meshDataCount);
		for (unsigned int i = 0; i < asset->meshDataCount; i++)
		{
			write.dump_array(asset->meshDatas[i].m_pRoot, asset->meshDatas[i].m_nodeCount);
			write.dump_array(asset->meshDatas[i].m_aVertices, asset->meshDatas[i].m_vertexCount);
			write.dump_array(asset->meshDatas[i].m_aTriangles, asset->meshDatas[i].m_triangleCount);
		}
		write.dump_array(asset->waterVolumeDefs, asset->waterVolumeDefCount);
		for (unsigned int i = 0; i < asset->waterVolumeDefCount; i++)
		{
			write.dump_asset(asset->waterVolumeDefs[i].physWaterPreset);
			write.dump_string(SL_ConvertToString(asset->waterVolumeDefs[i].string));
		}

		write.close();
	}
}