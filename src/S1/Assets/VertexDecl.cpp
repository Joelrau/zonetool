#include "stdafx.hpp"

namespace ZoneTool::S1
{
	void IVertexDecl::dump(MaterialVertexDeclaration* asset)
	{
		const auto path = "techsets\\"s + asset->name + ".vertexdecl"s;

		assetmanager::dumper write;
		if (!write.open(path))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);
		write.close();
	}
}