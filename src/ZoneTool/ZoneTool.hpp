#pragma once

// include zonetool utilities
#include <ZoneUtils/ZoneUtils.hpp>

extern std::string currentzone;

namespace ZoneTool
{
	void startup();
	void register_command(const std::string& name, std::function<void(std::vector<std::string>)> cb);
}
