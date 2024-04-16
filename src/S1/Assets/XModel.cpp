#include "stdafx.hpp"

#include <H1\Assets\XModel.hpp>
#include <H1\Structs.hpp>

namespace ZoneTool::S1
{
	void IXModel::dump(XModel* asset, const std::function<const char* (std::uint16_t)>& SL_ConvertToString)
	{
		H1::IXModel::dump(reinterpret_cast<H1::XModel*>(asset), SL_ConvertToString);
	}
}
