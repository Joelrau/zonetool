#include "stdafx.hpp"
#include "../Include.hpp"

#include "XModel.hpp"

namespace ZoneTool::IW5
{
	namespace IW6Converter
	{
		namespace
		{
			IW6::CSurfaceFlags surf_flags_conversion_table[31]
			{
				IW6::SURF_FLAG_DEFAULT,
				IW6::SURF_FLAG_BARK,
				IW6::SURF_FLAG_BRICK,
				IW6::SURF_FLAG_CARPET,
				IW6::SURF_FLAG_CLOTH,
				IW6::SURF_FLAG_CONCRETE,
				IW6::SURF_FLAG_DIRT,
				IW6::SURF_FLAG_FLESH,
				IW6::SURF_FLAG_FOLIAGE,
				IW6::SURF_FLAG_GLASS,
				IW6::SURF_FLAG_GRASS,
				IW6::SURF_FLAG_GRAVEL,
				IW6::SURF_FLAG_ICE,
				IW6::SURF_FLAG_METAL,
				IW6::SURF_FLAG_MUD,
				IW6::SURF_FLAG_PAPER,
				IW6::SURF_FLAG_PLASTER,
				IW6::SURF_FLAG_ROCK,
				IW6::SURF_FLAG_SAND,
				IW6::SURF_FLAG_SNOW,
				IW6::SURF_FLAG_WATER,
				IW6::SURF_FLAG_WOOD,
				IW6::SURF_FLAG_ASPHALT,
				IW6::SURF_FLAG_CERAMIC,
				IW6::SURF_FLAG_PLASTIC,
				IW6::SURF_FLAG_RUBBER,
				IW6::SURF_FLAG_CUSHION,
				IW6::SURF_FLAG_FRUIT,
				IW6::SURF_FLAG_PAINTEDMETAL,
				IW6::SURF_FLAG_RIOTSHIELD,
				IW6::SURF_FLAG_SLUSH,
			}; IW5::CSurfaceFlags;

			int convert_surf_flags(int flags)
			{
				int iw6_flags = surf_flags_conversion_table[flags >> 20];
				auto convert = [&](IW5::CSurfaceFlags a, IW6::CSurfaceFlags b)
				{
					iw6_flags |= ((flags & a) == a) ? b : 0;
				};
				convert(IW5::CSurfaceFlags::SURF_FLAG_OPAQUEGLASS, IW6::CSurfaceFlags::SURF_FLAG_DEFAULT);
				convert(IW5::CSurfaceFlags::SURF_FLAG_CLIPMISSILE, IW6::CSurfaceFlags::SURF_FLAG_CLIPMISSILE);
				convert(IW5::CSurfaceFlags::SURF_FLAG_AI_NOSIGHT, IW6::CSurfaceFlags::SURF_FLAG_AI_NOSIGHT);
				convert(IW5::CSurfaceFlags::SURF_FLAG_CLIPSHOT, IW6::CSurfaceFlags::SURF_FLAG_CLIPSHOT);
				convert(IW5::CSurfaceFlags::SURF_FLAG_PLAYERCLIP, IW6::CSurfaceFlags::SURF_FLAG_PLAYERCLIP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_MONSTERCLIP, IW6::CSurfaceFlags::SURF_FLAG_MONSTERCLIP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_AICLIPALLOWDEATH, IW6::CSurfaceFlags::SURF_FLAG_AICLIPALLOWDEATH);
				convert(IW5::CSurfaceFlags::SURF_FLAG_VEHICLECLIP, IW6::CSurfaceFlags::SURF_FLAG_VEHICLECLIP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_ITEMCLIP, IW6::CSurfaceFlags::SURF_FLAG_ITEMCLIP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NODROP, IW6::CSurfaceFlags::SURF_FLAG_NODROP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NONSOLID, IW6::CSurfaceFlags::SURF_FLAG_NONSOLID);
				convert(IW5::CSurfaceFlags::SURF_FLAG_DETAIL, IW6::CSurfaceFlags::SURF_FLAG_DETAIL);
				convert(IW5::CSurfaceFlags::SURF_FLAG_STRUCTURAL, IW6::CSurfaceFlags::SURF_FLAG_STRUCTURAL);
				convert(IW5::CSurfaceFlags::SURF_FLAG_PORTAL, IW6::CSurfaceFlags::SURF_FLAG_PORTAL);
				convert(IW5::CSurfaceFlags::SURF_FLAG_CANSHOOTCLIP, IW6::CSurfaceFlags::SURF_FLAG_CANSHOOTCLIP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_ORIGIN, IW6::CSurfaceFlags::SURF_FLAG_ORIGIN);
				convert(IW5::CSurfaceFlags::SURF_FLAG_SKY, IW6::CSurfaceFlags::SURF_FLAG_SKY);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NOCASTSHADOW, IW6::CSurfaceFlags::SURF_FLAG_NOCASTSHADOW);
				convert(IW5::CSurfaceFlags::SURF_FLAG_PHYSICSGEOM, IW6::CSurfaceFlags::SURF_FLAG_PHYSICSGEOM);
				convert(IW5::CSurfaceFlags::SURF_FLAG_LIGHTPORTAL, IW6::CSurfaceFlags::SURF_FLAG_LIGHTPORTAL);
				convert(IW5::CSurfaceFlags::SURF_FLAG_OUTDOORBOUNDS, IW6::CSurfaceFlags::SURF_FLAG_OUTDOORBOUNDS);
				convert(IW5::CSurfaceFlags::SURF_FLAG_SLICK, IW6::CSurfaceFlags::SURF_FLAG_SLICK);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NOIMPACT, IW6::CSurfaceFlags::SURF_FLAG_NOIMPACT);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NOMARKS, IW6::CSurfaceFlags::SURF_FLAG_NOMARKS);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NOPENETRATE, IW6::CSurfaceFlags::SURF_FLAG_NOPENETRATE);
				convert(IW5::CSurfaceFlags::SURF_FLAG_LADDER, IW6::CSurfaceFlags::SURF_FLAG_LADDER);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NODAMAGE, IW6::CSurfaceFlags::SURF_FLAG_NODAMAGE);
				convert(IW5::CSurfaceFlags::SURF_FLAG_MANTLEON, IW6::CSurfaceFlags::SURF_FLAG_MANTLEON);
				convert(IW5::CSurfaceFlags::SURF_FLAG_MANTLEOVER, IW6::CSurfaceFlags::SURF_FLAG_MANTLEOVER);
				convert(IW5::CSurfaceFlags::SURF_FLAG_STAIRS, IW6::CSurfaceFlags::SURF_FLAG_STAIRS);
				convert(IW5::CSurfaceFlags::SURF_FLAG_SOFT, IW6::CSurfaceFlags::SURF_FLAG_SOFT);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NOSTEPS, IW6::CSurfaceFlags::SURF_FLAG_NOSTEPS);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NODRAW, IW6::CSurfaceFlags::SURF_FLAG_NODRAW);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NOLIGHTMAP, IW6::CSurfaceFlags::SURF_FLAG_NOLIGHTMAP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NODLIGHT, IW6::CSurfaceFlags::SURF_FLAG_NODLIGHT);
				return iw6_flags;
			}
		}

		IW6::XModel* GenerateIW6Model(XModel* asset, allocator& mem)
		{
			// allocate IW6 XModel structure
			auto* iw6_asset = mem.allocate<IW6::XModel>();

			iw6_asset->name = asset->name;
			iw6_asset->numBones = asset->numBones;
			iw6_asset->numRootBones = asset->numRootBones;
			iw6_asset->numsurfs = asset->numsurfs;
			iw6_asset->numReactiveMotionParts = 0;
			//iw6_asset->__pad0[0] = asset->lodRampType; // maybe wrong
			iw6_asset->scale = asset->scale;
			memcpy(&iw6_asset->noScalePartBits, &asset->noScalePartBits, sizeof(asset->noScalePartBits));

			iw6_asset->boneNames = mem.allocate<IW6::scr_string_t>(asset->numBones);
			for (auto i = 0; i < asset->numBones; i++)
			{
				iw6_asset->boneNames[i] = static_cast<IW6::scr_string_t>(asset->boneNames[i]);
			}

			REINTERPRET_CAST_SAFE_TO_FROM(iw6_asset->parentList, asset->parentList);
			REINTERPRET_CAST_SAFE_TO_FROM(iw6_asset->tagAngles, asset->quats);
			REINTERPRET_CAST_SAFE_TO_FROM(iw6_asset->tagPositions, asset->trans);
			REINTERPRET_CAST_SAFE_TO_FROM(iw6_asset->partClassification, asset->partClassification);
			REINTERPRET_CAST_SAFE_TO_FROM(iw6_asset->baseMat, asset->baseMat);
			iw6_asset->reactiveMotionParts = nullptr;

			iw6_asset->materialHandles = mem.allocate<IW6::Material* __ptr64>(asset->numsurfs);
			for (auto i = 0; i < asset->numsurfs; i++)
			{
				if (asset->materialHandles[i])
				{
					iw6_asset->materialHandles[i] = mem.allocate<IW6::Material>();
					iw6_asset->materialHandles[i]->name = mem.duplicate_string(asset->materialHandles[i]->info.name);
				}
			}

			for (auto i = 0; i < 6; i++)
			{
				iw6_asset->lodInfo[i].dist = 1000000.0f;
			}

			// level of detail data
			for (auto i = 0; i < asset->numLods; i++)
			{
				iw6_asset->lodInfo[i].dist = asset->lodInfo[i].dist;
				iw6_asset->lodInfo[i].numsurfs = asset->lodInfo[i].numsurfs;
				iw6_asset->lodInfo[i].surfIndex = asset->lodInfo[i].surfIndex;
				iw6_asset->lodInfo[i].modelSurfs = mem.allocate<IW6::XModelSurfs>(asset->lodInfo[i].numsurfs);
				iw6_asset->lodInfo[i].modelSurfs->name = mem.duplicate_string(asset->lodInfo[i].modelSurfs->name);
				memcpy(&iw6_asset->lodInfo[i].partBits, &asset->lodInfo[i].partBits, sizeof(asset->lodInfo[i].partBits));
			}

			iw6_asset->maxLoadedLod = asset->maxLoadedLod;
			iw6_asset->numLods = asset->numLods;
			iw6_asset->collLod = asset->collLod;
			iw6_asset->flags = asset->flags;

			iw6_asset->numCollSurfs = asset->numCollSurfs;
			iw6_asset->collSurfs = mem.allocate<IW6::XModelCollSurf_s>(asset->numCollSurfs);
			for (auto i = 0; i < asset->numCollSurfs; i++)
			{
				memcpy(&iw6_asset->collSurfs[i].bounds, &asset->collSurfs[i].bounds, sizeof(float[2][3]));

				iw6_asset->collSurfs[i].boneIdx = asset->collSurfs[i].boneIdx;
				iw6_asset->collSurfs[i].contents = asset->collSurfs[i].contents;
				iw6_asset->collSurfs[i].surfFlags = convert_surf_flags(asset->collSurfs[i].surfFlags);
			}

			iw6_asset->contents = asset->contents;

			REINTERPRET_CAST_SAFE_TO_FROM(iw6_asset->boneInfo, asset->boneInfo);

			iw6_asset->radius = asset->radius;
			memcpy(&iw6_asset->bounds, &asset->bounds, sizeof(asset->bounds));
			iw6_asset->memUsage = asset->memUsage;

			if (asset->physPreset)
			{
				iw6_asset->physPreset = mem.allocate<IW6::PhysPreset>();
				iw6_asset->physPreset->name = mem.duplicate_string(asset->physPreset->name);
			}

			if (asset->physCollmap)
			{
				iw6_asset->physCollmap = mem.allocate<IW6::PhysCollmap>();
				iw6_asset->physCollmap->name = mem.duplicate_string(asset->physCollmap->name);
			}

			// idk
			iw6_asset->invHighMipRadius = mem.allocate<unsigned short>(asset->numsurfs);
			for (unsigned char i = 0; i < asset->numsurfs; i++)
			{
				iw6_asset->invHighMipRadius[i] = 0xFFFF;
			}

			iw6_asset->quantization = 0.0f; //1.0f;

			return iw6_asset;
		}

		IW6::XModel* convert(XModel* asset, allocator& allocator)
		{
			// generate IW6 model
			return GenerateIW6Model(asset, allocator);
		}
	}
}