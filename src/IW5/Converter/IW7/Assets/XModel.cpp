#include "stdafx.hpp"
#include "../Include.hpp"

#include "XModel.hpp"
#include "Assets/Material.hpp"

namespace ZoneTool::IW5
{
	namespace IW7Converter
	{
		namespace
		{
			IW7::SurfaceFlags surf_flags_conversion_table[31]
			{
				IW7::SurfaceFlags::SURFACE_FLAG_NONE,
				IW7::SurfaceFlags::SURFACE_FLAG_BARK,
				IW7::SurfaceFlags::SURFACE_FLAG_BRICK,
				IW7::SurfaceFlags::SURFACE_FLAG_CARPET_SOLID,
				IW7::SurfaceFlags::SURFACE_FLAG_CLOTH,
				IW7::SurfaceFlags::SURFACE_FLAG_CONCRETE_DRY,
				IW7::SurfaceFlags::SURFACE_FLAG_DIRT,
				IW7::SurfaceFlags::SURFACE_FLAG_FLESH,
				IW7::SurfaceFlags::SURFACE_FLAG_FOLIAGE_DRY,
				IW7::SurfaceFlags::SURFACE_FLAG_GLASS_PANE,
				IW7::SurfaceFlags::SURFACE_FLAG_GRASS_SHORT,
				IW7::SurfaceFlags::SURFACE_FLAG_GRAVEL,
				IW7::SurfaceFlags::SURFACE_FLAG_ICE_SOLID,
				IW7::SurfaceFlags::SURFACE_FLAG_METAL_THICK,
				IW7::SurfaceFlags::SURFACE_FLAG_MUD,
				IW7::SurfaceFlags::SURFACE_FLAG_PAPER,
				IW7::SurfaceFlags::SURFACE_FLAG_PLASTER,
				IW7::SurfaceFlags::SURFACE_FLAG_ROCK,
				IW7::SurfaceFlags::SURFACE_FLAG_SAND,
				IW7::SurfaceFlags::SURFACE_FLAG_SNOW,
				IW7::SurfaceFlags::SURFACE_FLAG_WATER,
				IW7::SurfaceFlags::SURFACE_FLAG_WOOD_SOLID,
				IW7::SurfaceFlags::SURFACE_FLAG_ASPHALT_DRY,
				IW7::SurfaceFlags::SURFACE_FLAG_CERAMIC,
				IW7::SurfaceFlags::SURFACE_FLAG_PLASTIC,
				IW7::SurfaceFlags::SURFACE_FLAG_RUBBER,
				IW7::SurfaceFlags::SURFACE_FLAG_CUSHION,
				IW7::SurfaceFlags::SURFACE_FLAG_FRUIT,
				IW7::SurfaceFlags::SURFACE_FLAG_METAL_PAINTED,
				IW7::SurfaceFlags::SURFACE_FLAG_RIOTSHIELD,
				IW7::SurfaceFlags::SURFACE_FLAG_SLUSH,
			}; IW5::CSurfaceFlags;

			int convert_surf_flags(int flags)
			{
				int IW7_flags = surf_flags_conversion_table[flags >> 20];
				auto convert = [&](IW5::CSurfaceFlags a, IW7::SurfaceFlags b)
				{
					IW7_flags |= ((flags & a) == a) ? b : 0;
				};
				convert(IW5::CSurfaceFlags::SURF_FLAG_OPAQUEGLASS, IW7::SurfaceFlags::SURFACE_FLAG_OPAQUEGLASS);
				convert(IW5::CSurfaceFlags::SURF_FLAG_CLIPMISSILE, IW7::SurfaceFlags::SURFACE_FLAG_CLIPMISSILE);
				convert(IW5::CSurfaceFlags::SURF_FLAG_AI_NOSIGHT, IW7::SurfaceFlags::SURFACE_FLAG_AI_NOSIGHT);
				convert(IW5::CSurfaceFlags::SURF_FLAG_CLIPSHOT, IW7::SurfaceFlags::SURFACE_FLAG_CLIPSHOT);
				convert(IW5::CSurfaceFlags::SURF_FLAG_PLAYERCLIP, IW7::SurfaceFlags::SURFACE_FLAG_PLAYERCLIP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_MONSTERCLIP, IW7::SurfaceFlags::SURFACE_FLAG_MONSTERCLIP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_AICLIPALLOWDEATH, IW7::SurfaceFlags::SURFACE_FLAG_AICLIPALLOWDEATH);
				convert(IW5::CSurfaceFlags::SURF_FLAG_VEHICLECLIP, IW7::SurfaceFlags::SURFACE_FLAG_VEHICLECLIP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_ITEMCLIP, IW7::SurfaceFlags::SURFACE_FLAG_ITEMCLIP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NODROP, IW7::SurfaceFlags::SURFACE_FLAG_NODROP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NONSOLID, IW7::SurfaceFlags::SURFACE_FLAG_NONSOLID);
				convert(IW5::CSurfaceFlags::SURF_FLAG_DETAIL, IW7::SurfaceFlags::SURFACE_FLAG_DETAIL);
				convert(IW5::CSurfaceFlags::SURF_FLAG_STRUCTURAL, IW7::SurfaceFlags::SURFACE_FLAG_STRUCTURAL);
				convert(IW5::CSurfaceFlags::SURF_FLAG_PORTAL, IW7::SurfaceFlags::SURFACE_FLAG_PORTAL);
				convert(IW5::CSurfaceFlags::SURF_FLAG_CANSHOOTCLIP, IW7::SurfaceFlags::SURFACE_FLAG_CANSHOOTCLIP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_ORIGIN, IW7::SurfaceFlags::SURFACE_FLAG_ORIGIN);
				convert(IW5::CSurfaceFlags::SURF_FLAG_SKY, IW7::SurfaceFlags::SURFACE_FLAG_SKY);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NOCASTSHADOW, IW7::SurfaceFlags::SURFACE_FLAG_NOCASTSHADOW);
				convert(IW5::CSurfaceFlags::SURF_FLAG_PHYSICSGEOM, IW7::SurfaceFlags::SURFACE_FLAG_PHYSICSGEOM);
				convert(IW5::CSurfaceFlags::SURF_FLAG_LIGHTPORTAL, IW7::SurfaceFlags::SURFACE_FLAG_LIGHTPORTAL);
				convert(IW5::CSurfaceFlags::SURF_FLAG_OUTDOORBOUNDS, IW7::SurfaceFlags::SURFACE_FLAG_OUTDOORBOUNDS);
				convert(IW5::CSurfaceFlags::SURF_FLAG_SLICK, IW7::SurfaceFlags::SURFACE_FLAG_SLICK);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NOIMPACT, IW7::SurfaceFlags::SURFACE_FLAG_NOIMPACT);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NOMARKS, IW7::SurfaceFlags::SURFACE_FLAG_NOMARKS);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NOPENETRATE, IW7::SurfaceFlags::SURFACE_FLAG_NOPENETRATE);
				convert(IW5::CSurfaceFlags::SURF_FLAG_LADDER, IW7::SurfaceFlags::SURFACE_FLAG_LADDER);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NODAMAGE, IW7::SurfaceFlags::SURFACE_FLAG_NODAMAGE);
				convert(IW5::CSurfaceFlags::SURF_FLAG_MANTLEON, IW7::SurfaceFlags::SURFACE_FLAG_MANTLEON);
				convert(IW5::CSurfaceFlags::SURF_FLAG_MANTLEOVER, IW7::SurfaceFlags::SURFACE_FLAG_MANTLEOVER);
				convert(IW5::CSurfaceFlags::SURF_FLAG_STAIRS, IW7::SurfaceFlags::SURFACE_FLAG_STAIRS);
				convert(IW5::CSurfaceFlags::SURF_FLAG_SOFT, IW7::SurfaceFlags::SURFACE_FLAG_SOFT);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NOSTEPS, IW7::SurfaceFlags::SURFACE_FLAG_NOSTEPS);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NODRAW, IW7::SurfaceFlags::SURFACE_FLAG_NODRAW);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NOLIGHTMAP, IW7::SurfaceFlags::SURFACE_FLAG_NOLIGHTMAP);
				convert(IW5::CSurfaceFlags::SURF_FLAG_NODLIGHT, IW7::SurfaceFlags::SURFACE_FLAG_NODLIGHT);
				return IW7_flags;
			}
		}

		IW7::XModel* GenerateIW7Model(XModel* asset, allocator& mem)
		{
			// allocate IW7 XModel structure
			auto* iw7_asset = mem.allocate<IW7::XModel>();

			iw7_asset->name = asset->name;
			iw7_asset->numBones = asset->numBones;
			iw7_asset->numRootBones = asset->numRootBones;
			iw7_asset->numsurfs = asset->numsurfs;
			iw7_asset->numReactiveMotionParts = 0;
			iw7_asset->scale = asset->scale;
			memcpy(&iw7_asset->noScalePartBits, &asset->noScalePartBits, sizeof(asset->noScalePartBits));

			iw7_asset->boneNames = mem.allocate<IW7::scr_string_t>(asset->numBones);
			for (auto i = 0; i < asset->numBones; i++)
			{
				iw7_asset->boneNames[i] = static_cast<IW7::scr_string_t>(asset->boneNames[i]);
			}

			REINTERPRET_CAST_SAFE(iw7_asset->parentList, asset->parentList);
			REINTERPRET_CAST_SAFE(iw7_asset->tagAngles, asset->quats);
			REINTERPRET_CAST_SAFE(iw7_asset->tagPositions, asset->trans);
			REINTERPRET_CAST_SAFE(iw7_asset->partClassification, asset->partClassification);
			REINTERPRET_CAST_SAFE(iw7_asset->baseMat, asset->baseMat);
			iw7_asset->reactiveMotionParts = nullptr;

			iw7_asset->materialHandles = mem.allocate<IW7::Material* __ptr64>(asset->numsurfs);
			for (auto i = 0; i < asset->numsurfs; i++)
			{
				if (asset->materialHandles[i])
				{
					iw7_asset->materialHandles[i] = mem.allocate<IW7::Material>();
					iw7_asset->materialHandles[i]->name = mem.duplicate_string(IW7::replace_material_prefix(asset->materialHandles[i]->info.name));
				}
			}

			for (auto i = 0; i < 6; i++)
			{
				iw7_asset->lodInfo[i].dist = 1000000.0f;
			}

			// level of detail data
			for (auto i = 0; i < asset->numLods; i++)
			{
				iw7_asset->lodInfo[i].dist = asset->lodInfo[i].dist;
				iw7_asset->lodInfo[i].numsurfs = asset->lodInfo[i].numsurfs;
				iw7_asset->lodInfo[i].surfIndex = asset->lodInfo[i].surfIndex;
				iw7_asset->lodInfo[i].modelSurfs = mem.allocate<IW7::XModelSurfs>();
				iw7_asset->lodInfo[i].modelSurfs->name = mem.duplicate_string(asset->lodInfo[i].modelSurfs->name);
				memcpy(&iw7_asset->lodInfo[i].partBits, &asset->lodInfo[i].partBits, sizeof(asset->lodInfo[i].partBits));
			}

			iw7_asset->maxLoadedLod = asset->maxLoadedLod;
			iw7_asset->numLods = asset->numLods;
			iw7_asset->collLod = asset->collLod;
			iw7_asset->flags = asset->flags;

			iw7_asset->numCollSurfs = asset->numCollSurfs;
			iw7_asset->collSurfs = mem.allocate<IW7::XModelCollSurf_s>(asset->numCollSurfs);
			for (auto i = 0; i < asset->numCollSurfs; i++)
			{
				memcpy(&iw7_asset->collSurfs[i].bounds, &asset->collSurfs[i].bounds, sizeof(float[2][3]));

				iw7_asset->collSurfs[i].boneIdx = asset->collSurfs[i].boneIdx;
				iw7_asset->collSurfs[i].contents = asset->collSurfs[i].contents;
				iw7_asset->collSurfs[i].surfFlags = convert_surf_flags(asset->collSurfs[i].surfFlags);
			}

			iw7_asset->contents = asset->contents;

			REINTERPRET_CAST_SAFE(iw7_asset->boneInfo, asset->boneInfo);

			iw7_asset->radius = asset->radius;
			memcpy(&iw7_asset->bounds, &asset->bounds, sizeof(asset->bounds));
			iw7_asset->memUsage = asset->memUsage;

			if (asset->physPreset)
			{
				//iw7_asset->physAsset = mem.allocate<IW7::PhysicsAsset>();
				//iw7_asset->physAsset->name = mem.duplicate_string(asset->physPreset->name);
			}

			if (asset->physCollmap)
			{
				// ?
				//iw7_asset->physFxShape = mem.allocate<IW7::PhysicsFXShape>();
				//iw7_asset->physFxShape->name = mem.duplicate_string(asset->physCollmap->name);
			}

			// idk
			iw7_asset->invHighMipRadius = mem.allocate<unsigned short>(asset->numsurfs);
			for (unsigned char i = 0; i < asset->numsurfs; i++)
			{
				iw7_asset->invHighMipRadius[i] = 0xFFFF;
			}

			//iw7_asset->quantization = 0.0f;

			iw7_asset->hasLods = asset->numLods ? 1 : 0;
			iw7_asset->shadowCutoffLod = 6;
			iw7_asset->characterCollBoundsType = 1; // CharCollBoundsType_Human

			iw7_asset->unknownIndex = 0xFF;
			iw7_asset->unknownIndex2 = 0xFF;

			//iw7_asset->flags |= 0x40;

			return iw7_asset;
		}

		IW7::XModel* convert(XModel* asset, allocator& allocator)
		{
			// generate IW7 model
			return GenerateIW7Model(asset, allocator);
		}
	}
}