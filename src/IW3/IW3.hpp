// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#pragma once

#include <ZoneUtils.hpp>
#include "Functions.hpp"
#include "Structs.hpp"

#include "X64/X64.hpp"
#include "X64/Utils/IO/filesystem.hpp"
#include "X64/Utils/IO/assetmanager.hpp"

#include "H1/Structs.hpp"

#include "IW6/Structs.hpp"

#include "IW4/Structs.hpp"

#include "IW5/Structs.hpp"

using namespace zonetool;

#include "Json.hpp"
using json = nlohmann::json;
using ordered_json = nlohmann::ordered_json;

namespace bounds
{
	static void compute(float* mins, float* maxs, float(*dest)[3])
	{
		for (int i = 0; i < 3; ++i)
		{
			dest[1][i] = (maxs[i] - mins[i]) / 2;
			dest[0][i] = dest[1][i] + mins[i];
		}
	}

	static void compute(ZoneTool::IW4::Bounds* bounds, float* mins, float* maxs)
	{
		for (int i = 0; i < 3; ++i)
		{
			bounds->halfSize[i] = (maxs[i] - mins[i]) / 2;
			bounds->midPoint[i] = bounds->halfSize[i] + mins[i];
		}
	}
}

namespace ZoneTool
{
	namespace IW4
	{
		static void VectorSubtract(const vec3_t& a, const vec3_t& b, vec3_t& out)
		{
			out[0] = a[0] - b[0];
			out[1] = a[1] - b[1];
			out[2] = a[2] - b[2];
		}

		inline void Bounds::compute(vec3_t mins, vec3_t maxs)
		{
			bounds::compute(this, mins, maxs);
		}

		inline void Bounds::min(vec3_t& out)
		{
			for (int i = 0; i < 3; ++i)
			{
				out[i] = midPoint[i] - halfSize[i];
			}
		}

		inline bool Bounds::overlaps(const Bounds& other)
		{
			vec3_t points[6]{};

			VectorSubtract(other.midPoint, other.halfSize, points[0]);
			VectorSubtract(other.midPoint, { -other.halfSize[0], other.halfSize[1], other.halfSize[2] }, points[1]);
			VectorSubtract(other.midPoint, { -other.halfSize[0], -other.halfSize[1], other.halfSize[2] }, points[2]);
			VectorSubtract(other.midPoint, { -other.halfSize[0], -other.halfSize[1], -other.halfSize[2] }, points[3]);
			VectorSubtract(other.midPoint, { other.halfSize[0], -other.halfSize[1], -other.halfSize[2] }, points[4]);
			VectorSubtract(other.midPoint, { -other.halfSize[0], other.halfSize[1], -other.halfSize[2] }, points[5]);

			for (size_t i = 0; i < ARRAYSIZE(points); i++)
			{
				if (contains(points[i]))
				{
					return true;
				}
			}

			return false;
		}

		inline bool Bounds::contains(const vec3_t& point)
		{
			vec3_t min{};
			vec3_t max{};
			this->min(min);
			this->max(max);

			return
				point[0] >= min[0] && point[0] <= max[0] &&
				point[1] >= min[1] && point[1] <= max[1] &&
				point[2] >= min[2] && point[2] <= max[2];
		}

		inline void Bounds::max(vec3_t& out)
		{
			for (int i = 0; i < 3; ++i)
			{
				out[i] = midPoint[i] + halfSize[i];
			}
		}
	}
}

#include "Assets/ClipMap.hpp"
#include "Assets/ComWorld.hpp"
#include "Assets/FxEffectDef.hpp"
#include "Assets/GameWorldMp.hpp"
#include "Assets/GfxImage.hpp"
#include "Assets/GfxLightDef.hpp"
#include "Assets/GfxWorld.hpp"
#include "Assets/LoadedSound.hpp"
#include "Assets/MapEnts.hpp"
#include "Assets/Material.hpp"
#include "Assets/RawFile.hpp"
#include "Assets/Sound.hpp"
#include "Assets/StringTable.hpp"
#include "Assets/SoundCurve.hpp"
#include "Assets/XAnimParts.hpp"
#include "Assets/XModel.hpp"

namespace ZoneTool
{
	namespace IW3
	{
		typedef int (*FS_Read_t)(void* buffer, int len, int handle);
		extern FS_Read_t FS_Read;

		extern std::string filesystem_read_big_file(const char* filename);

		struct XAsset
		{
			XAssetType type;
			XAssetHeader ptr;
		};

		class Linker : public ILinker
		{
		public:
			Linker();
			~Linker();

			const char* version() override;
			bool is_used() override;
			void startup() override;
			std::shared_ptr<IZone> alloc_zone(const std::string& zone) override;
			std::shared_ptr<ZoneBuffer> alloc_buffer() override;
			void load_zone(const std::string& name) override;
			void unload_zones() override;
			bool is_valid_asset_type(const std::string& type) override;
			std::int32_t type_to_int(std::string type) override;
			std::string type_to_string(std::int32_t type) override;
            bool supports_building() override;
			bool supports_version(const zone_target_version version) override;

			void dump_zone(const std::string& name, zonetool::dump_target target) override;
			void verify_zone(const std::string& name) override;

			static void* Dedicated_RegisterDvarBool(const char* name, bool defaultValue, unsigned int flags,
			                                        const char* description);
			static void* DB_AddXAsset(XAsset* asset, int unk);
			static const char* GetAssetName(XAsset* asset);
			static void HandleAsset(XAsset* asset);
		};
	}
}
