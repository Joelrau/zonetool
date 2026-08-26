#pragma once

namespace ZoneTool::IW7
{
	class IClipMap
	{
	private:
	public:
		static void dump_info(ClipInfo* info, assetmanager::dumper& write);
		static void dump(clipMap_t* asset);
	};
}