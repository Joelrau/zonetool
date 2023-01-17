#include "stdafx.hpp"
#include "H1/Assets/XModel.hpp"

namespace ZoneTool
{
	namespace IW5
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

		H1::XModel* GenerateH1Model(XModel* asset, ZoneMemory* mem)
		{
			// allocate H1 XModel structure
			auto* h1_asset = mem->Alloc<H1::XModel>();

			h1_asset->name = asset->name;
			h1_asset->numBones = asset->numBones;
			h1_asset->numRootBones = asset->numRootBones;
			h1_asset->numsurfs = asset->numSurfaces;
			h1_asset->numReactiveMotionParts = 0;
			h1_asset->lodRampType = asset->lodRampType;
			h1_asset->scale = asset->scale;
			memcpy(&h1_asset->noScalePartBits, &asset->noScalePartBits, sizeof(asset->noScalePartBits));

			h1_asset->boneNames = mem->Alloc<H1::scr_string_t>(asset->numBones);
			for (auto i = 0; i < asset->numBones; i++)
			{
				h1_asset->boneNames[i] = static_cast<H1::scr_string_t>(asset->boneNames[i]);
			}

			REINTERPRET_CAST_SAFE(h1_asset->parentList, asset->parentList);
			REINTERPRET_CAST_SAFE(h1_asset->tagAngles, asset->tagAngles);
			REINTERPRET_CAST_SAFE(h1_asset->tagPositions, asset->tagPositions);
			REINTERPRET_CAST_SAFE(h1_asset->partClassification, asset->partClassification);
			REINTERPRET_CAST_SAFE(h1_asset->baseMat, asset->animMatrix);
			h1_asset->reactiveMotionParts = nullptr;
			h1_asset->reactiveMotionTweaks = nullptr;

			h1_asset->materialHandles = mem->Alloc<H1::Material* __ptr64>(asset->numSurfaces);
			for (auto i = 0; i < asset->numSurfaces; i++)
			{
				if (asset->materials[i])
				{
					h1_asset->materialHandles[i] = mem->Alloc<H1::Material>();
					h1_asset->materialHandles[i]->name = mem->StrDup(asset->materials[i]->name);
				}
			}

			for (auto i = 0; i < 6; i++)
			{
				h1_asset->lodInfo[i].dist = 1000000.0f;
			}

			// level of detail data
			for (auto i = 0; i < asset->numLods; i++)
			{
				h1_asset->lodInfo[i].dist = asset->lods[i].dist;
				h1_asset->lodInfo[i].numsurfs = asset->lods[i].numSurfacesInLod;
				h1_asset->lodInfo[i].surfIndex = asset->lods[i].surfIndex;
				h1_asset->lodInfo[i].modelSurfs = mem->Alloc<H1::XModelSurfs>(asset->lods[i].numSurfacesInLod);
				for (auto j = 0; j < asset->lods[i].numSurfacesInLod; j++)
				{
					h1_asset->lodInfo[i].modelSurfs[j].name = mem->StrDup(asset->lods[i].surfaces[j].name);
				}
				memcpy(&h1_asset->lodInfo[i].partBits, &asset->lods[i].partBits, sizeof(asset->lods[i].partBits));
			}

			//h1_asset->maxLoadedLod = asset->maxLoadedLod;
			h1_asset->numLods = asset->numLods;
			h1_asset->collLod = asset->collLod;
			h1_asset->numCompositeModels = 0;
			h1_asset->flags = asset->flags;

			h1_asset->numCollSurfs = asset->numColSurfs;
			h1_asset->collSurfs = mem->Alloc<H1::XModelCollSurf_s>(asset->numColSurfs);
			for (auto i = 0; i < asset->numColSurfs; i++)
			{
				memcpy(&h1_asset->collSurfs[i].bounds, &asset->colSurf[i].mins, sizeof(float[2][3]));

				h1_asset->collSurfs[i].boneIdx = asset->colSurf[i].boneIdx;
				h1_asset->collSurfs[i].contents = asset->colSurf[i].contents;
				h1_asset->collSurfs[i].surfFlags = convert_surf_flags(asset->colSurf[i].surfFlags);
			}

			h1_asset->contents = asset->contents;

			REINTERPRET_CAST_SAFE(h1_asset->boneInfo, asset->boneInfo);

			h1_asset->radius = asset->radius;
			memcpy(&h1_asset->bounds, &asset->bounds, sizeof(asset->bounds));
			h1_asset->memUsage = asset->memUsage;

			if (asset->physPreset)
			{
				h1_asset->physPreset = mem->Alloc<H1::PhysPreset>();
				h1_asset->physPreset->name = mem->StrDup(asset->physPreset->name);
			}

			if (asset->physCollmap)
			{
				h1_asset->physCollmap = mem->Alloc<H1::PhysCollmap>();
				h1_asset->physCollmap->name = mem->StrDup(asset->physCollmap->name);
			}

			// idk
			h1_asset->invHighMipRadius = mem->Alloc<unsigned short>(asset->numSurfaces);
			for (unsigned char i = 0; i < asset->numSurfaces; i++)
			{
				h1_asset->invHighMipRadius[i] = 0xFFFF;
			}

			h1_asset->quantization = 0.0f; //1.0f;

			return h1_asset;
		}

		void IXModel::dump(XModel* asset, ZoneMemory* mem)
		{
			// generate h1 model
			auto h1_asset = GenerateH1Model(asset, mem);

			// dump h1 model
			H1::IXModel::dump(h1_asset, SL_ConvertToString);
		}
	}
}