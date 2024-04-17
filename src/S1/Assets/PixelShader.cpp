#include "stdafx.hpp"

#include "PixelShader.hpp"

#include <H1\Assets\PixelShader.hpp>

namespace ZoneTool::S1
{
	void IPixelShader::dump(MaterialPixelShader* asset)
	{
		H1::IPixelShader::dump(reinterpret_cast<H1::MaterialPixelShader*>(asset));
	}
}
