#pragma once
#include "H1/Structs.hpp"
#include <vector>

namespace ZoneTool::H1
{
	namespace physworld_gen
	{
		struct smodel_tri { float a[3]; float b[3]; float c[3]; };
		void set_static_model_tris(std::vector<smodel_tri>&& tris);

		PhysWorld* generate_physworld(clipMap_t* asset, allocator* mem);
	}
}