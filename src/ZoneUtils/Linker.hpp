#pragma once

namespace ZoneTool
{
	class ILinker
	{
	public:
		virtual const char* version() = 0;
		virtual bool is_used() = 0;
		virtual void startup() = 0;
        virtual bool supports_building() = 0;

		virtual void dump_zone(const std::string& name, zonetool::dump_target target = zonetool::dump_target::h1) = 0;
		virtual void verify_zone(const std::string& name) = 0;
		virtual void load_zone(const std::string& name) = 0;
		virtual void unload_zones() = 0;

		virtual bool is_valid_asset_type(const std::string& type) = 0;
		virtual std::int32_t type_to_int(std::string type) = 0;
		virtual std::string type_to_string(std::int32_t type) = 0;
		
	private:
	};

	enum linker_mode
	{
		none = -1,
		iw3 = 0,
		iw4 = 1,
		iw5 = 2,
	};
	extern linker_mode currentlinkermode;

	static void set_linker_mode(linker_mode mode)
	{
		currentlinkermode = mode;
	}
	static linker_mode get_linker_mode()
	{
		return currentlinkermode;
	}
}
