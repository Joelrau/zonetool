#pragma once

#include <H1\Structs.hpp>
#include <S1\Structs.hpp>

namespace ZoneTool::S1
{
	class ITechset : public IAsset
	{
	public:
		static void dump_stateinfo(const std::string& techset, const std::string& material, Material* mat);
		static void dump_statebits(const std::string& techset, const std::string& material, unsigned char* statebits);
		static void dump_statebits_map(const std::string& techset, const std::string& material, GfxStateBits* map, unsigned char count);
		static void dump_constant_buffer_indexes(const std::string& techset, const std::string& material, unsigned char* cbi);
		static void dump_constant_buffer_def_array(const std::string& techset, const std::string& material, unsigned char count, MaterialConstantBufferDef* def);
		static void dump_technique(MaterialTechnique* asset);
		static void dump(MaterialTechniqueSet* asset);
	};
}