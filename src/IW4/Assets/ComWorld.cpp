#include "stdafx.hpp"
#include "H1/Assets/ComWorld.hpp"
#include "IW5/Structs.hpp"

#include "IW5/Assets/ComWorld.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		void IComWorld::dump(ComWorld* asset, ZoneMemory* mem)
		{
			// alloc comworld
			auto* iw5_comworld = mem->Alloc<IW5::ComWorld>();
			memcpy(iw5_comworld, asset, sizeof ComWorld);

			// alloc lights
			iw5_comworld->primaryLights = mem->Alloc<IW5::ComPrimaryLight>(iw5_comworld->primaryLightCount);
			memset(iw5_comworld->primaryLights, 0, sizeof(IW5::ComPrimaryLight) * iw5_comworld->primaryLightCount);

			// copy data
			for (auto i = 0u; i < iw5_comworld->primaryLightCount; i++)
			{
				memcpy(iw5_comworld->primaryLights[i]._portpad0, asset->primaryLights[i]._portpad0, 28);
				memcpy(iw5_comworld->primaryLights[i]._portpad1, asset->primaryLights[i]._portpad1, 40);

				iw5_comworld->primaryLights[i].up[0] = 0.0f;
				iw5_comworld->primaryLights[i].up[1] = 0.0f;
				iw5_comworld->primaryLights[i].up[2] = 0.0f;
			}

			// dump comworld
			IW5::IComWorld::dump(iw5_comworld, mem);
		}
	}
}