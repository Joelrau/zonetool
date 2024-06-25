#include "stdafx.hpp"
#include "IW5/Assets/XModel.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		void IXModel::dump(XModel* asset, const std::function<const char* (uint16_t)>& convertToString)
		{
			allocator allocator;
			auto* iw5_model = allocator.allocate<IW5::XModel>();
			memcpy(iw5_model, asset, sizeof(XModel));
			iw5_model->quantization = 0.0f;

			for (auto i = 0; i < asset->numLods; i++)
			{
				iw5_model->lodInfo[i].modelSurfs = generate_surface(asset->lods[i].surfaces, allocator);
			}

			// dump model
			IW5::IXModel::dump(iw5_model, convertToString);
		}
	}
}