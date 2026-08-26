#include "stdafx.hpp"
#include "../Include.hpp"

#include "XModel.hpp"

#define CONVERT_TAGS

namespace ZoneTool::IW5
{
	namespace H1Converter
	{
		namespace
		{
			H1::CSurfaceFlags surf_flags_conversion_table[31]
			{
				H1::SURF_FLAG_DEFAULT,
				H1::SURF_FLAG_BARK,
				H1::SURF_FLAG_BRICK,
				H1::SURF_FLAG_CARPET,
				H1::SURF_FLAG_CLOTH,
				H1::SURF_FLAG_CONCRETE,
				H1::SURF_FLAG_DIRT,
				H1::SURF_FLAG_FLESH,
				H1::SURF_FLAG_FOLIAGE_DEBRIS,
				H1::SURF_FLAG_GLASS,
				H1::SURF_FLAG_GRASS,
				H1::SURF_FLAG_GRAVEL,
				H1::SURF_FLAG_ICE,
				H1::SURF_FLAG_METAL_SOLID,
				H1::SURF_FLAG_MUD,
				H1::SURF_FLAG_PAPER,
				H1::SURF_FLAG_PLASTER,
				H1::SURF_FLAG_ROCK,
				H1::SURF_FLAG_SAND,
				H1::SURF_FLAG_SNOW,
				H1::SURF_FLAG_WATER_WAIST,
				H1::SURF_FLAG_WOOD_SOLID,
				H1::SURF_FLAG_ASPHALT,
				H1::SURF_FLAG_CERAMIC,
				H1::SURF_FLAG_PLASTIC_SOLID,
				H1::SURF_FLAG_RUBBER,
				H1::SURF_FLAG_CUSHION,
				H1::SURF_FLAG_FRUIT,
				H1::SURF_FLAG_PAINTEDMETAL,
				H1::SURF_FLAG_RIOTSHIELD,
				H1::SURF_FLAG_SLUSH,
			}; IW5::CSurfaceFlags;

			int convert_surf_flags(int flags)
			{
				int h1_flags = surf_flags_conversion_table[flags >> 20];
				auto convert = [&](IW5::CSurfaceFlags a, H1::CSurfaceFlags b)
				{
					h1_flags |= ((flags & a) == a) ? b : 0;
				};
				convert(IW5::CSurfaceFlags::SURF_FLAG_OPAQUEGLASS, H1::CSurfaceFlags::SURF_FLAG_DEFAULT);
				convert(IW5::CSurfaceFlags::SURF_FLAG_CLIPMISSILE, H1::CSurfaceFlags::SURF_FLAG_CLIPMISSILE);
				convert(IW5::CSurfaceFlags::SURF_FLAG_AI_NOSIGHT, H1::CSurfaceFlags::SURF_FLAG_AI_NOSIGHT);
				convert(IW5::CSurfaceFlags::SURF_FLAG_CLIPSHOT, H1::CSurfaceFlags::SURF_FLAG_CLIPSHOT);
				convert(IW5::CSurfaceFlags::SURF_FLAG_PLAYERCLIP, H1::CSurfaceFlags::SURF_FLAG_PLAYERCLIP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_MONSTERCLIP, H1::CSurfaceFlags::SURF_FLAG_MONSTERCLIP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_AICLIPALLOWDEATH, H1::CSurfaceFlags::SURF_FLAG_AICLIPALLOWDEATH);
				convert(IW5::CSurfaceFlags::SURF_FLAG_VEHICLECLIP, H1::CSurfaceFlags::SURF_FLAG_VEHICLECLIP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_ITEMCLIP, H1::CSurfaceFlags::SURF_FLAG_ITEMCLIP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NODROP, H1::CSurfaceFlags::SURF_FLAG_NODROP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NONSOLID, H1::CSurfaceFlags::SURF_FLAG_NONSOLID);
				convert(IW5::CSurfaceFlags::SURF_FLAG_DETAIL, H1::CSurfaceFlags::SURF_FLAG_DETAIL);
				convert(IW5::CSurfaceFlags::SURF_FLAG_STRUCTURAL, H1::CSurfaceFlags::SURF_FLAG_STRUCTURAL);
				convert(IW5::CSurfaceFlags::SURF_FLAG_PORTAL, H1::CSurfaceFlags::SURF_FLAG_PORTAL);
				convert(IW5::CSurfaceFlags::SURF_FLAG_CANSHOOTCLIP, H1::CSurfaceFlags::SURF_FLAG_CANSHOOTCLIP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_ORIGIN, H1::CSurfaceFlags::SURF_FLAG_ORIGIN);
				convert(IW5::CSurfaceFlags::SURF_FLAG_SKY, H1::CSurfaceFlags::SURF_FLAG_SKY);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NOCASTSHADOW, H1::CSurfaceFlags::SURF_FLAG_NOCASTSHADOW);
				convert(IW5::CSurfaceFlags::SURF_FLAG_PHYSICSGEOM, H1::CSurfaceFlags::SURF_FLAG_PHYSICSGEOM);
				convert(IW5::CSurfaceFlags::SURF_FLAG_LIGHTPORTAL, H1::CSurfaceFlags::SURF_FLAG_LIGHTPORTAL);
				convert(IW5::CSurfaceFlags::SURF_FLAG_OUTDOORBOUNDS, H1::CSurfaceFlags::SURF_FLAG_OUTDOORBOUNDS);
				convert(IW5::CSurfaceFlags::SURF_FLAG_SLICK, H1::CSurfaceFlags::SURF_FLAG_SLICK);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NOIMPACT, H1::CSurfaceFlags::SURF_FLAG_NOIMPACT);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NOMARKS, H1::CSurfaceFlags::SURF_FLAG_NOMARKS);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NOPENETRATE, H1::CSurfaceFlags::SURF_FLAG_NOPENETRATE);
				convert(IW5::CSurfaceFlags::SURF_FLAG_LADDER, H1::CSurfaceFlags::SURF_FLAG_LADDER);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NODAMAGE, H1::CSurfaceFlags::SURF_FLAG_NODAMAGE);
				convert(IW5::CSurfaceFlags::SURF_FLAG_MANTLEON, H1::CSurfaceFlags::SURF_FLAG_MANTLEON);
				convert(IW5::CSurfaceFlags::SURF_FLAG_MANTLEOVER, H1::CSurfaceFlags::SURF_FLAG_MANTLEOVER);
				convert(IW5::CSurfaceFlags::SURF_FLAG_STAIRS, H1::CSurfaceFlags::SURF_FLAG_STAIRS);
				convert(IW5::CSurfaceFlags::SURF_FLAG_SOFT, H1::CSurfaceFlags::SURF_FLAG_SOFT);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NOSTEPS, H1::CSurfaceFlags::SURF_FLAG_NOSTEPS);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NODRAW, H1::CSurfaceFlags::SURF_FLAG_NODRAW);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NOLIGHTMAP, H1::CSurfaceFlags::SURF_FLAG_NOLIGHTMAP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NODLIGHT, H1::CSurfaceFlags::SURF_FLAG_NODLIGHT);
				return h1_flags;
			}
		}

		H1::XModel* GenerateH1Model(XModel* asset, allocator& mem)
		{
			// allocate H1 XModel structure
			auto* h1_asset = mem.allocate<H1::XModel>();

			h1_asset->name = asset->name;
			h1_asset->numBones = asset->numBones;
			h1_asset->numRootBones = asset->numRootBones;
			h1_asset->numsurfs = asset->numsurfs;
			h1_asset->numReactiveMotionParts = 0;
			h1_asset->lodRampType = 0;
			h1_asset->scale = asset->scale;
			memcpy(&h1_asset->noScalePartBits, &asset->noScalePartBits, sizeof(asset->noScalePartBits));

			h1_asset->boneNames = mem.allocate<H1::scr_string_t>(asset->numBones);
			for (auto i = 0; i < asset->numBones; i++)
			{
#ifdef CONVERT_TAGS
				std::string bone_name = Shared::SL_ConvertToString(asset->boneNames[i]);
				if (bone_name == "tag_rail")
				{
					bone_name = "tag_sight_off";
				}
				h1_asset->boneNames[i] = Shared::SL_AllocString(bone_name.c_str());
#else
				h1_asset->boneNames[i] = static_cast<H1::scr_string_t>(asset->boneNames[i]);
#endif
			}

			REINTERPRET_CAST_SAFE_TO_FROM(h1_asset->parentList, asset->parentList);
			REINTERPRET_CAST_SAFE_TO_FROM(h1_asset->tagAngles, asset->quats);
			REINTERPRET_CAST_SAFE_TO_FROM(h1_asset->tagPositions, asset->trans);
			REINTERPRET_CAST_SAFE_TO_FROM(h1_asset->partClassification, asset->partClassification);
			REINTERPRET_CAST_SAFE_TO_FROM(h1_asset->baseMat, asset->baseMat);
			h1_asset->reactiveMotionParts = nullptr;
			h1_asset->reactiveMotionTweaks = nullptr;

			h1_asset->materialHandles = mem.allocate<H1::Material* __ptr64>(asset->numsurfs);
			for (auto i = 0; i < asset->numsurfs; i++)
			{
				if (asset->materialHandles[i])
				{
					h1_asset->materialHandles[i] = mem.allocate<H1::Material>();
					h1_asset->materialHandles[i]->name = mem.duplicate_string(asset->materialHandles[i]->info.name);
				}
			}

			for (auto i = 0; i < 6; i++)
			{
				h1_asset->lodInfo[i].dist = 1000000.0f;
			}

			// level of detail data
			for (auto i = 0; i < asset->numLods; i++)
			{
				h1_asset->lodInfo[i].dist = asset->lodInfo[i].dist;
				h1_asset->lodInfo[i].numsurfs = asset->lodInfo[i].numsurfs;
				h1_asset->lodInfo[i].surfIndex = asset->lodInfo[i].surfIndex;
				h1_asset->lodInfo[i].modelSurfs = mem.allocate<H1::XModelSurfs>();
				h1_asset->lodInfo[i].modelSurfs->name = mem.duplicate_string(asset->lodInfo[i].modelSurfs->name);
				memcpy(&h1_asset->lodInfo[i].partBits, &asset->lodInfo[i].partBits, sizeof(asset->lodInfo[i].partBits));
			}

			//h1_asset->maxLoadedLod = asset->maxLoadedLod;
			h1_asset->numLods = asset->numLods;
			h1_asset->collLod = asset->collLod;
			h1_asset->numCompositeModels = 0;
			h1_asset->flags = asset->flags;

			h1_asset->numCollSurfs = asset->numCollSurfs;
			h1_asset->collSurfs = mem.allocate<H1::XModelCollSurf_s>(asset->numCollSurfs);
			for (auto i = 0; i < asset->numCollSurfs; i++)
			{
				memcpy(&h1_asset->collSurfs[i].bounds, &asset->collSurfs[i].bounds, sizeof(float[2][3]));

				h1_asset->collSurfs[i].boneIdx = asset->collSurfs[i].boneIdx;
				h1_asset->collSurfs[i].contents = asset->collSurfs[i].contents;
				h1_asset->collSurfs[i].surfFlags = convert_surf_flags(asset->collSurfs[i].surfFlags);
			}

			h1_asset->contents = asset->contents;

			REINTERPRET_CAST_SAFE_TO_FROM(h1_asset->boneInfo, asset->boneInfo);

			h1_asset->radius = asset->radius;
			memcpy(&h1_asset->bounds, &asset->bounds, sizeof(asset->bounds));
			h1_asset->memUsage = asset->memUsage;

			if (asset->physPreset)
			{
				h1_asset->physPreset = mem.allocate<H1::PhysPreset>();
				h1_asset->physPreset->name = mem.duplicate_string(asset->physPreset->name);
			}

			if (asset->physCollmap)
			{
				h1_asset->physCollmap = mem.allocate<H1::PhysCollmap>();
				h1_asset->physCollmap->name = mem.duplicate_string(asset->physCollmap->name);
			}

			// idk
			h1_asset->invHighMipRadius = mem.allocate<unsigned short>(asset->numsurfs);
			for (unsigned char i = 0; i < asset->numsurfs; i++)
			{
				h1_asset->invHighMipRadius[i] = 0xFFFF;
			}

			h1_asset->quantization = 0.0f; //1.0f;

			return h1_asset;
		}

		H1::XModel* convert(XModel* asset, allocator& allocator)
		{
			// generate h1 model
			return GenerateH1Model(asset, allocator);
		}
	}
}