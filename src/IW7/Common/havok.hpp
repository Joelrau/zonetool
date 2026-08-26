#pragma once

namespace ZoneTool::IW7
{
	namespace havok
	{
		namespace xml
		{

		}

		namespace binary
		{
			constexpr auto havok_file_ext = ".hkx";

			void dump_havok_data(std::string path, char* data, unsigned int size);
		}
	}
}