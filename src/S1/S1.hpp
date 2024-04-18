#pragma once

#include <ZoneUtils.hpp>

#include "X64/X64.hpp"
#include "X64/Utils/IO/filesystem.hpp"
#include "X64/Utils/IO/assetmanager.hpp"

#define JSON_DIAGNOSTICS 1
#include "json.hpp"
using json = nlohmann::json;
using ordered_json = nlohmann::ordered_json;

using namespace zonetool;

#include "Structs.hpp"
#include <H1\Structs.hpp>

#include "Assets/ClipMap.hpp"
#include "Assets/ComWorld.hpp"
#include "Assets/FxEffectDef.hpp"
#include "Assets/FxWorld.hpp"
#include "Assets/GfxImage.hpp"
#include "Assets/GfxLightDef.hpp"
#include "Assets/GfxWorld.hpp"
#include "Assets/GlassWorld.hpp"
#include "Assets/MapEnts.hpp"
#include "Assets/PhysPreset.hpp"
#include "Assets/PhysWorld.hpp"
#include "Assets/RawFile.hpp"
#include "Assets/Sound.hpp"
#include "Assets/SoundCurve.hpp"
#include "Assets/XAnimParts.hpp"
#include "Assets/XModel.hpp"
#include "Assets/XSurface.hpp"
#include "Assets/Techset.hpp"
#include "Assets/PixelShader.hpp"
#include "Assets/VertexShader.hpp"
#include "Assets/VertexDecl.hpp"

namespace ZoneTool::S1
{

}
