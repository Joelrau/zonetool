#include "stdafx.hpp"

namespace ZoneTool::S1
{
	void IVertexShader::dump(MaterialVertexShader* asset)
	{
		const auto path = "techsets\\"s + asset->name + ".vertexshader"s;

		assetmanager::dumper write;
		if (!write.open(path))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);
		write.dump_array(asset->prog.loadDef.program, asset->prog.loadDef.programSize);
		write.close();
	}
}