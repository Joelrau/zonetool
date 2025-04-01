#include "stdafx.hpp"
#include "IW4/Assets/Material.hpp"

namespace ZoneTool::IW4
{
	struct MaterialGameFlagsFields_
	{
		unsigned char unk1 : 1; // 0x1
		unsigned char addShadowToPrimaryLight : 1; // 0x2
		unsigned char isFoliageRequiresGroundLighting : 1; // 0x4
		unsigned char unk4 : 1; // 0x8
		unsigned char unk5 : 1; // 0x10
		unsigned char unk6 : 1; // 0x20
		unsigned char unk7 : 1; // 0x40
		unsigned char unkCastShadowMaybe : 1; // 0x80
	};

	union MaterialGameFlags_
	{
		MaterialGameFlagsFields_ fields;
		unsigned char packed;
	};
}

namespace ZoneTool::IW3
{
	std::unordered_map<char, char> sortKeysTable = {
		{0, 43},	// Distortion
		// Opaque water (never used)
		// Boat hull (never used)
		{3, 0},		// Opaque ambient
		{4, 1},		// Opaque
		{5, 2},		// Sky
		{6, 3},		// Skybox sun/moon
		{7, 4},		// Clouds  NOT SURE
		{8, 5},		// Horizon NOT SURE
		{9, 6},		// Decal bottom 1
		{10, 7},	// Decal bottom 2
		{11, 8},	// Decal bottom 3
		{12, 9},	// Decal static
		{13, 10},	// Decal mid 1
		{14, 11},	// Decal mid 2
		{15, 12},	// Decal mid 3
		{24, 13},	// Weapon Impact
		{29, 14},	// Decal top 1
		{30, 15},	// Decal top 2
		{31, 16},	// Decal top 3
		{32, 17},	// Multiplicative, verified on mp_countdown
		{33, 17},	// Banner/ Curtain (wild guess!)
		{34, 4 },	// Hair. I matched it with german shepherd material sortkey, i hope its ok.
		{35, 19},	// Underwater
		{36, 20},	// Transparent water
		{37, 33},	// Corona (wild guess)
		{38, 24},	// Window inside
		{39, 25},	// Window outside
		{40, 44},	// Before effects 1 (wild guess)
		{41, 45},	// Before effects 2 (wild guess)
		{42, 46},	// Before effects 3 (extremely wild guess)
		{43, 29},	// Blend / additive => to a decal layer (shouldn't this be to 47?)
		{48, 48},	// Effect auto sort!
		{51, 9},	// FX Top
		{56, 49},	// AE Bottom
		{57, 50},	// AE Middle
		{58, 51},	// AE top
		{59, 53}	// Viewmodel effect
	};

	IW4::Material* convert(Material* asset, allocator& mem)
	{
		auto* iw4_asset = mem.allocate<IW4::Material>();
		iw4_asset->name = asset->name;
		
		// iw3xpo
		//MaterialGameFlags iw3_game_flags;
		//iw3_game_flags.packed = asset->gameFlags;
		//IW4::MaterialGameFlags_ iw4_game_flags;
		//iw4_game_flags.packed = iw3_game_flags.packed;
		//iw4_game_flags.fields.unk7 = iw3_game_flags.fields.unkNeededForSModelDisplay;
		//iw4_game_flags.fields.unkCastShadowMaybe = iw3_game_flags.fields.MTL_GAMEFLAG_CASTS_SHADOW;

		iw4_asset->gameFlags = asset->gameFlags;

		if (sortKeysTable.contains(asset->sortKey))
		{
			iw4_asset->sortKey = sortKeysTable[asset->sortKey];
		}
		else
		{
			ZONETOOL_FATAL("sortkey unmapped iw3 -> iw4 for material %s", asset->name);
		}

		iw4_asset->textureAtlasRowCount = asset->textureAtlasRowCount;
		iw4_asset->textureAtlasColumnCount = asset->textureAtlasColumnCount;

		//iw4_asset->drawSurf;

		iw4_asset->surfaceTypeBits = asset->surfaceTypeBits; // convert
		iw4_asset->hashIndex = asset->hashIndex;

		//iw4_asset->stateBitsEntry; // convert
		// Set them all to -1 so they're not used if they dont exist in iw3
		std::memset(iw4_asset->stateBitsEntry, 0xFF, sizeof(iw4_asset->stateBitsEntry));

		iw4_asset->textureCount = asset->textureCount;
		iw4_asset->constantCount = asset->constantCount;
		iw4_asset->stateBitsCount = asset->stateBitsCount;
		iw4_asset->stateFlags = asset->stateFlags;

		iw4_asset->cameraRegion = asset->cameraRegion;
		if (asset->cameraRegion >= IW3::CAMERA_REGION_NONE)
		{
			iw4_asset->cameraRegion = IW4::CAMERA_REGION_NONE;
		}

		iw4_asset->techniqueSet = reinterpret_cast<IW4::MaterialTechniqueSet*>(asset->techniqueSet);

		iw4_asset->textureTable = mem.allocate<IW4::MaterialTextureDef>(iw4_asset->textureCount);
		for (char i = 0; i < iw4_asset->textureCount; i++)
		{
			auto iw3Def = &asset->textureTable[i];
			auto iw4Def = &iw4_asset->textureTable[i];

			static_assert(sizeof IW4::MaterialTextureDef == sizeof IW3::MaterialTextureDef);
			std::memcpy(iw4Def, iw3Def, sizeof IW4::MaterialTextureDef);

			if (iw4Def->semantic == 0xB) // TS_Water
			{
				iw4Def->u.water = mem.allocate<IW4::water_t>();

				static_assert(sizeof IW4::water_t == sizeof IW3::water_t);
				std::memcpy(iw4Def->u.water, iw3Def->u.water, sizeof IW4::water_t);

				iw4Def->u.water->image = mem.allocate<IW4::GfxImage>();
				iw4Def->u.water->image->name = iw3Def->u.water->image->name;
			}
			else
			{
				iw4Def->u.image = mem.allocate<IW4::GfxImage>();
				iw4Def->u.image->name = iw3Def->u.image->name;
			}
		}

		iw4_asset->constantTable = reinterpret_cast<IW4::MaterialConstantDef*>(asset->constantTable); // convert?
		iw4_asset->stateBitsTable = reinterpret_cast<IW4::GfxStateBits*>(asset->stateBitsTable); // convert?

		return iw4_asset;
	}

	void IMaterial::dump(Material* asset)
	{
		allocator allocator;
		auto* iw4_asset = convert(asset, allocator);

		// dump asset
		IW4::IMaterial::dump(iw4_asset);
	}
}