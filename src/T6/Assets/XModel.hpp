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
		class IXModel : public IAsset
		{
		private:
			//std::string name_;
			//XModel* asset_ = nullptr;
			//bool is_scope_model_;

		public:
			//static XModel* parse_new(const std::string& name, ZoneMemory* mem, const std::string& filename);
			//static XModel* parse(std::string name, ZoneMemory* mem);

			//void init(const std::string& name, ZoneMemory* mem) override;
			//void prepare(ZoneBuffer* buf, ZoneMemory* mem) override;
			//void load_depending(IZone* zone) override;

			//void* pointer() override { return asset_; }
			//std::string name() override;
			//std::int32_t type() override;
			//void write(IZone* zone, ZoneBuffer* buffer) override;

			static void dump(XModel* asset);
		};
	}
}
