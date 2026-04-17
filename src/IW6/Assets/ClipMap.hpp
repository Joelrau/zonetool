#pragma once

namespace ZoneTool::IW6
{
	class IClipMap
	{
	private:
		static void dump_info(ClipInfo* info, assetmanager::dumper& write);

	public:
		static void dump(clipMap_t* asset);
	};
}