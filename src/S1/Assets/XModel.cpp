#include "stdafx.hpp"

#include "XModel.hpp"

#include <H1\Assets\XModel.hpp>

namespace ZoneTool::S1
{
	void IXModel::dump(XModel* asset)
	{
		H1::IXModel::dump(reinterpret_cast<H1::XModel*>(asset));
	}
}
