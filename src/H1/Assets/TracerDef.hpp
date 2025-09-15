#pragma once

namespace ZoneTool::H1
{
	class ITracerDef : public IAsset
	{
	public:
		static void dump(TracerDef* asset);
	};
}