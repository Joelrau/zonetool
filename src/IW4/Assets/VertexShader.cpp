// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		IVertexShader::IVertexShader()
		{
		}

		IVertexShader::~IVertexShader()
		{
		}

		VertexShader* IVertexShader::parse(const std::string& name, ZoneMemory* mem, bool preferLocal)
		{
			auto path = "vertexshader\\" + name;

			if (!FileSystem::FileExists(path))
			{
				path = "techsets\\" + name + ".vertexshader";

				FileSystem::PreferLocalOverExternal(true);
				if (!FileSystem::FileExists(path))
				{
					FileSystem::PreferLocalOverExternal(false);
					return nullptr;
				}

				AssetReader read(mem);
				if (!read.open(path))
				{
					FileSystem::PreferLocalOverExternal(false);
					return nullptr;
				}

				ZONETOOL_INFO("Parsing vertexshader \"%s\"...", name.data());

				auto asset = read.read_array<VertexShader>();
				asset->name = read.read_string();
				asset->bytecode = read.read_array<DWORD>();
				read.close();

				FileSystem::PreferLocalOverExternal(false);

				return asset;
			}

			ZONETOOL_INFO("Parsing custom DirectX vertexshader \"%s\"...", name.data());

			const auto fp = FileSystem::FileOpen(path, "rb");
			const auto asset = mem->Alloc<VertexShader>();
			asset->name = mem->StrDup(name);
			asset->codeLen = FileSystem::FileSize(fp);
			asset->bytecode = mem->ManualAlloc<DWORD>(asset->codeLen);
			asset->shader = nullptr;
			fread(asset->bytecode, asset->codeLen, 1, fp);

			FileSystem::FileClose(fp);

			return asset;
		}

		void IVertexShader::init(void* asset, ZoneMemory* mem)
		{
			this->m_asset = reinterpret_cast<VertexShader*>(asset);
			this->m_name = this->m_asset->name + "_IW5"s;
		}

		void IVertexShader::init(const std::string& name, ZoneMemory* mem)
		{
			this->m_name = name;
			this->m_asset = this->parse(name, mem);

			if (!this->m_asset)
			{
				ZONETOOL_FATAL("VertexShader %s not found.", &name[0]);
				this->m_asset = DB_FindXAssetHeader(this->type(), this->name().data()).vertexshader;
			}
		}

		void IVertexShader::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void IVertexShader::load_depending(IZone* zone)
		{
		}

		std::string IVertexShader::name()
		{
			return this->m_name;
		}

		std::int32_t IVertexShader::type()
		{
			return vertexshader;
		}

		void IVertexShader::write(IZone* zone, ZoneBuffer* buf)
		{
			auto data = this->m_asset;
			auto dest = buf->write(data);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->name = buf->write_str(this->name());

			if (data->bytecode)
			{
				buf->align(3);
				buf->write(data->bytecode, data->codeLen);
				ZoneBuffer::ClearPointer(&dest->bytecode);
			}

			END_LOG_STREAM;
			buf->pop_stream();
		}

		void IVertexShader::dump(VertexShader* asset)
		{
			AssetDumper write;
			if (!write.open("techsets\\"s + asset->name + ".vertexshader"s))
			{
				return;
			}

			write.dump_array(asset, 1);
			write.dump_string(asset->name);
			write.dump_array(asset->bytecode, asset->codeLen);
			write.close();
		}
	}
}
