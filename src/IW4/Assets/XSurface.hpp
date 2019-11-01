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
	namespace IW4
	{
		class IXSurface : public IAsset
		{
		private:
			std::string m_name;
			ModelSurface* m_asset;

			void write_xsurfices(IZone* zone, ZoneBuffer* buf, XSurface* data, XSurface* dest,
			                     std::uint16_t count);

		public:
			IXSurface();
			~IXSurface();

			ModelSurface* parse(const std::string& name, ZoneMemory* mem);

			void init(const std::string& name, ZoneMemory* mem) override;
			void init(void* asset, ZoneMemory* mem) override;

			void prepare(ZoneBuffer* buf, ZoneMemory* mem) override;
			void load_depending(IZone* zone) override;

			void* pointer() override { return m_asset; }
			std::string name() override;
			std::int32_t type() override;
			void write(IZone* zone, ZoneBuffer* buffer) override;

			static void dump(ModelSurface* asset);
		};
	}
}
