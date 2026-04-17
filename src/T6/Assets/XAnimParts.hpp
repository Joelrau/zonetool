// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#pragma once

namespace ZoneTool
{
	namespace T6
	{
		class IXAnimParts : public IAsset
		{
		private:
			//std::string name_;
			//XAnimParts* asset_ = nullptr;

		public:
			//static XAnimParts* parse_xae2(const std::string& name, ZoneMemory* mem);
			//static XAnimParts* parse_xae3(const std::string& name, ZoneMemory* mem);
			//static XAnimParts* parse(const std::string& name, ZoneMemory* mem);

			//void init(const std::string& name, ZoneMemory* mem) override;
			//void prepare(ZoneBuffer* buf, ZoneMemory* mem) override;
			//void load_depending(IZone* zone) override;

			//std::string name() override;
			//std::int32_t type() override;
			//void write(IZone* zone, ZoneBuffer* buffer) override;

			static void dump_xae3(XAnimParts* asset);
			static void dump(XAnimParts* asset);
		};
	}
}
