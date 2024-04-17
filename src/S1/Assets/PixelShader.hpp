#pragma once

#include <H1\Structs.hpp>

namespace ZoneTool::S1
{
	class IPixelShader : public IAsset
	{
	public:
		static void dump(MaterialPixelShader* asset);
	};
}
