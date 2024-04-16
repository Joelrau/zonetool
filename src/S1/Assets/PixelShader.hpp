#pragma once

namespace ZoneTool::S1
{
	class IPixelShader : public IAsset
	{
	public:
		static void dump(MaterialPixelShader* asset);
	};
}
