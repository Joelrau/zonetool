#pragma once

namespace ZoneTool::S1
{
	class IVertexShader : public IAsset
	{
	public:
		static void dump(MaterialVertexShader* asset);
	};
}