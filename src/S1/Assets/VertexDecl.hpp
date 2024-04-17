#pragma once

#include <H1\Structs.hpp>

namespace ZoneTool::S1
{
	class IVertexDecl : public IAsset
	{
	public:
		static void dump(MaterialVertexDeclaration* asset);
	};
}