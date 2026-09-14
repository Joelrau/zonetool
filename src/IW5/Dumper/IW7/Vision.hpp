#pragma once

namespace ZoneTool::IW5::IW7Dumper
{
	// Rewrites every dump/<zone>/vision/*.vision as an IW7 vision. Runs once the whole zone has
	// been dumped, and only when dumping for IW7; a no-op otherwise.
	void convert_visions(const std::string& zone);
}
