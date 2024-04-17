#pragma once

#include <H1\Structs.hpp>

namespace ZoneTool::S1
{
	class IVertexShader : public IAsset
	{
	public:
		static void dump(MaterialVertexShader* asset);
	};
}