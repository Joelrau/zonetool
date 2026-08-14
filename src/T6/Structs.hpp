#pragma once

#include <d3d11.h>

namespace ZoneTool
{
	namespace T6
	{
		struct _GUID;
		struct dvar_t;
		struct MenuCell;
		struct cplane_s;
		struct cbrushside_t;
		struct MaterialVertexDeclaration;
		struct MaterialVertexShader;
		struct MaterialPixelShader;
		struct MaterialShaderArgument;
		struct MaterialTechnique;
		struct MaterialTechniqueSet;
		struct GfxImageLoadDef;
		struct GfxImage;
		struct MaterialTextureDef;
		struct MaterialConstantDef;
		struct GfxStateBits;
		struct Material;
		struct MenuRow;
		struct Glyph;
		struct KerningPairs;
		struct Font_s;
		struct BrushWrapper;
		struct multiDef_s;
		struct XModelCollTri_s;
		struct PhysConstraints;
		struct ScriptCondition;
		struct listBoxDef_s;
		struct expressionRpn;
		struct GenericEventScript;
		struct GenericEventHandler;
		struct animParamsDef_t;
		struct enumDvarDef_s;
		struct PhysPreset;
		struct profileMultiDef_s;
		struct PhysGeomInfo;
		struct XModelCollSurf_s;
		struct gameMsgDef_s;
		struct SndAlias;
		struct SndAliasList;
		struct XBoneInfo;
		struct editFieldDef_s;
		struct WeaponCamoMaterial;
		struct textExp_s;
		struct ItemKeyHandler;
		struct focusItemDef_s;
		struct textDef_s;
		struct imageDef_s;
		struct ownerDrawDef_s;
		struct menuDef_t;
		struct rectData_s;
		struct UIAnimInfo;
		struct itemDef_s;
		struct WeaponCamoMaterialSet;
		struct Collmap;
		struct TracerDef;
		struct WeaponAttachment;
		struct DObjAnimMat;
		struct GfxPackedVertex;
		struct XSurfaceCollisionNode;
		struct XSurfaceCollisionLeaf;
		struct XSurfaceCollisionTree;
		struct XRigidVertList;
		struct XSurface;
		struct XModel;
		struct flameTable;
		struct cStaticModel_s;
		struct FxElemVelStateSample;
		struct FxElemVisStateSample;
		struct FxElemMarkVisuals;
		struct FxEffectDef;
		struct GfxLightDef;
		union FxElemVisuals;
		struct FxTrailVertex;
		struct FxTrailDef;
		struct FxSpotLightDef;
		struct FxElemDef;
		struct WeaponCamoSet;
		struct WeaponCamo;
		struct WeaponDef;
		struct WeaponAttachmentUnique;
		struct WeaponVariantDef;
		struct cLeafBrushNode_s;
		struct cbrush_t;
		struct ClipMaterial;
		struct Bounds;
		struct cLeaf_s;
		struct cmodel_t;
		struct CollisionAabbTree;
		struct pathlink_s;
		struct pathnode_t;
		struct DestructiblePiece;
		struct DestructibleDef;
		struct GraphFloat;
		struct VehicleDef;
		struct XAnimNotifyInfo;
		struct XAnimPartTrans;
		struct XAnimDeltaPartQuat2;
		struct XAnimDeltaPartQuat;
		struct XAnimDeltaPart;
		struct XAnimParts;
		struct ZBarrierDef;
		struct GfxLight;
		struct ddlHash_t;
		struct ddlEnumDef_t;
		struct SndVolumeGroup;
		struct XModelPiece;
		struct XModelPieces;
		struct DynEntityDef;
		struct CollisionPartition;
		struct ddlMemberDef_t;
		struct ddlStructDef_t;
		struct ddlDef_t;
		struct GlassDef;
		struct Glass;
		struct GfxAabbTree;
		struct FxImpactEntry;
		struct StringTableCell;
		struct ddlRoot_t;
		struct StringTable;
		struct FxImpactTable;
		struct SndIndexEntry;
		struct SndRadverb;
		struct SndDuck;
		struct SndAssetBankEntry;
		struct SndDialogScriptIdLookup;
		struct SndBank;
		struct SndPatch;
		struct cNode_t;
		struct TriggerModel;
		struct TriggerHull;
		struct TriggerSlab;
		struct MapEnts;
		struct DynEntityPose;
		struct DynEntityClient;
		struct DynEntityServer;
		struct DynEntityColl;
		struct rope_t;
		struct clipMap_t;
		struct ComPrimaryLight;
		struct ComWorld;
		struct pathbasenode_t;
		struct pathnode_tree_t;
		struct GameWorldSp;
		struct GameWorldMp;
		struct GfxStreamingAabbTree;
		struct GfxLightCorona;
		struct GfxShadowMapVolume;
		struct GfxVolumePlane;
		struct GfxExposureVolume;
		struct GfxWorldFogVolume;
		struct GfxWorldFogModifierVolume;
		struct GfxLutVolume;
		struct GfxPortal;
		struct GfxCell;
		struct GfxReflectionProbeVolumeData;
		struct GfxReflectionProbe;
		struct GfxLightmapArray;
		struct GfxLightGridEntry;
		struct GfxCompressedLightGridColors;
		struct GfxCompressedLightGridCoeffs;
		struct GfxSkyGridVolume;
		struct GfxBrushModel;
		struct MaterialMemory;
		struct GfxSceneDynModel;
		struct GfxSceneDynBrush;
		struct SSkinShaders;
		struct SSkinVert;
		struct SSkinModel;
		struct SSkinAnim;
		struct SSkinInstance;
		struct GfxShadowGeometry;
		struct GfxLightRegionAxis;
		struct GfxLightRegionHull;
		struct GfxLightRegion;
		struct GfxStaticModelInst;
		struct GfxSurface;
		struct GfxStaticModelDrawInst;
		struct Occluder;
		struct GfxOutdoorBounds;
		struct GfxHeroLight;
		struct GfxHeroLightTree;
		struct GfxWorld;
		struct FontIconEntry;
		struct FontIconAlias;
		struct FontIcon;
		struct MenuList;
		struct LocalizeEntry;
		struct SndCurve;
		struct SndPan;
		struct SndDuckGroup;
		struct SndContext;
		struct SndMaster;
		struct SndSidechainDuck;
		struct SndFutz;
		struct SndDriverGlobals;
		struct RawFile;
		struct LbColumnDef;
		struct LeaderboardDef;
		struct XGlobals;
		struct Glasses;
		struct EmblemSet;
		struct ScriptParseTree;
		struct KeyValuePair;
		struct KeyValuePairs;
		struct MemoryBlock;
		struct AddonMapEnts;
		struct SkinnedVertsDef;
		struct Qdb;
		struct Slug;
		struct FootstepTableDef;
		struct FootstepFXTableDef;

		enum XAssetType : std::int32_t
		{
			xmodelpieces,
			physpreset,
			physconstraints,
			destructibledef,
			xanimparts,
			xmodel,
			material,
			technique_set,
			image,
			sound,
			sound_patch,
			clipmap,
			clipmap_pvs,
			comworld,
			gameworld_sp,
			gameworld_mp,
			map_ents,
			gfxworld,
			light_def,
			ui_map,
			font,
			fonticon,
			menulist,
			menu,
			localize_entry,
			weapon,
			weapondef,
			weapon_variant,
			weapon_full,
			attachment,
			attachment_unique,
			weapon_camo,
			snddriver_globals,
			fx,
			impact_fx,
			aitype,
			mptype,
			mpbody,
			mphead,
			character,
			xmodelalias,
			rawfile,
			stringtable,
			leaderboard,
			xglobals,
			ddl,
			glasses,
			emblemset,
			scriptparsetree,
			keyvaluepairs,
			vehicledef,
			memoryblock,
			addon_map_ents,
			tracer,
			skinnedverts,
			qdb,
			slug,
			footstep_table,
			footstepfx_table,
			zbarrier,
			count,

			col_map_sp = clipmap,
			col_map_mp = clipmap_pvs,
			game_map_sp = gameworld_sp,
			game_map_mp = gameworld_mp,
			gfx_map = gfxworld,
		};

		/* 329 */
		enum ConstraintType : __int32
		{
			CONSTRAINT_NONE = 0x0,
			CONSTRAINT_POINT = 0x1,
			CONSTRAINT_DISTANCE = 0x2,
			CONSTRAINT_HINGE = 0x3,
			CONSTRAINT_JOINT = 0x4,
			CONSTRAINT_ACTUATOR = 0x5,
			CONSTRAINT_FAKE_SHAKE = 0x6,
			CONSTRAINT_LAUNCH = 0x7,
			CONSTRAINT_ROPE = 0x8,
			CONSTRAINT_LIGHT = 0x9,
			NUM_CONSTRAINT_TYPES = 0xA,
		};

		/* 330 */
		enum AttachPointType : __int32
		{
			ATTACH_POINT_WORLD = 0x0,
			ATTACH_POINT_DYNENT = 0x1,
			ATTACH_POINT_ENT = 0x2,
			ATTACH_POINT_BONE = 0x3,
		};

		/* 922 */
		enum SndBankState : __int32
		{
			SND_BANK_STATE_NEW = 0x0,
			SND_BANK_STATE_STREAM_HEADER = 0x1,
			SND_BANK_STATE_STREAM_TOC = 0x2,
			SND_BANK_STATE_LOADED_HEADER = 0x3,
			SND_BANK_STATE_LOADED_TOC = 0x4,
			SND_BANK_STATE_LOADED_ASSET_WAIT = 0x5,
			SND_BANK_STATE_LOADED_ASSETS = 0x6,
			SND_BANK_STATE_READY_TO_USE = 0x7,
			SND_BANK_STATE_ERROR = 0x8,
		};

		/* 396 */
		enum weaponIconRatioType_t : __int32
		{
			WEAPON_ICON_RATIO_1TO1 = 0x0,
			WEAPON_ICON_RATIO_2TO1 = 0x1,
			WEAPON_ICON_RATIO_4TO1 = 0x2,
			WEAPON_ICON_RATIO_COUNT = 0x3,
		};

		/* 231 */
		enum eAttachment : __int32
		{
			ATTACHMENT_NONE = 0x0,
			ATTACHMENT_ACOG = 0x1,
			ATTACHMENT_DUALCLIP = 0x2,
			ATTACHMENT_DUALOPTIC = 0x3,
			ATTACHMENT_DW = 0x4,
			ATTACHMENT_EXTBARREL = 0x5,
			ATTACHMENT_EXTCLIP = 0x6,
			ATTACHMENT_EXTRAMAGS = 0x7,
			ATTACHMENT_FASTADS = 0x8,
			ATTACHMENT_FASTRELOAD = 0x9,
			ATTACHMENT_FMJ = 0xA,
			ATTACHMENT_GL = 0xB,
			ATTACHMENT_GRIP = 0xC,
			ATTACHMENT_HOLO = 0xD,
			ATTACHMENT_IR = 0xE,
			ATTACHMENT_IRONSIGHTS = 0xF,
			ATTACHMENT_LONGBREATH = 0x10,
			ATTACHMENT_MK = 0x11,
			ATTACHMENT_MMS = 0x12,
			ATTACHMENT_RANGEFINDER = 0x13,
			ATTACHMENT_REFLEX = 0x14,
			ATTACHMENT_RF = 0x15,
			ATTACHMENT_SELECTFIRE = 0x16,
			ATTACHMENT_SILENCER = 0x17,
			ATTACHMENT_STACKFIRE = 0x18,
			ATTACHMENT_STALKER = 0x19,
			ATTACHMENT_STEADYAIM = 0x1A,
			ATTACHMENT_SWAYREDUC = 0x1B,
			ATTACHMENT_TACKNIFE = 0x1C,
			ATTACHMENT_VZOOM = 0x1D,
			ATTACHMENT_TYPE_COUNT = 0x1E,
			ATTACHMENT_INVALID = 0x1E,
		};

		/* 235 */
		enum eAttachmentPoint : __int32
		{
			ATTACHMENT_POINT_NONE = 0x0,
			ATTACHMENT_POINT_TOP = 0x1,
			ATTACHMENT_POINT_FIRST = 0x1,
			ATTACHMENT_POINT_BOTTOM = 0x2,
			ATTACHMENT_POINT_TRIGGER = 0x3,
			ATTACHMENT_POINT_MUZZLE = 0x4,
			ATTACHMENT_POINT_GUNPERK = 0x5,
			ATTACHMENT_POINT_COUNT = 0x6,
			ATTACHMENT_POINT_INVALID = 0x6,
		};

		/* 413 */
		enum PenetrateType : __int32
		{
			PENETRATE_TYPE_NONE = 0x0,
			PENETRATE_TYPE_SMALL = 0x1,
			PENETRATE_TYPE_MEDIUM = 0x2,
			PENETRATE_TYPE_LARGE = 0x3,
			PENETRATE_TYPE_COUNT = 0x4,
		};

		/* 374 */
		enum weapFireType_t : __int32
		{
			WEAPON_FIRETYPE_FULLAUTO = 0x0,
			WEAPON_FIRETYPE_SINGLESHOT = 0x1,
			WEAPON_FIRETYPE_BURSTFIRE2 = 0x2,
			WEAPON_FIRETYPE_BURSTFIRE3 = 0x3,
			WEAPON_FIRETYPE_BURSTFIRE4 = 0x4,
			WEAPON_FIRETYPE_BURSTFIRE5 = 0x5,
			WEAPON_FIRETYPE_STACKED = 0x6,
			WEAPON_FIRETYPE_MINIGUN = 0x7,
			WEAPON_FIRETYPE_CHARGESHOT = 0x8,
			WEAPON_FIRETYPE_JETGUN = 0x9,
			WEAPON_FIRETYPECOUNT = 0xA,
		};

		/* 391 */
		enum weapOverlayReticle_t : __int32
		{
			WEAPOVERLAYRETICLE_NONE = 0x0,
			WEAPOVERLAYRETICLE_CROSSHAIR = 0x1,
			WEAPOVERLAYRETICLE_NUM = 0x2,
		};

		/* 1179 */
		enum LbUpdateType : __int32
		{
			LBUPDATE_TYPE_NORMAL = 0x0,
			LBUPDATE_TYPE_RANK = 0x1,
			LBUPDATE_TYPE_COMBINE = 0x2,
			LBUPDATE_TYPE_COUNT = 0x3,
		};

		/* 219 */
		enum TractionType : __int32
		{
			TRACTION_TYPE_FRONT = 0x0,
			TRACTION_TYPE_BACK = 0x1,
			TRACTION_TYPE_ALL_WD = 0x2,
			NUM_TRACTION_TYPES = 0x3,
		};

		/* 352 */
		enum tracerType_t : __int32
		{
			TRACERTYPE_LASER = 0x0,
			TRACERTYPE_SMOKE = 0x1,
			TRACERTYPE_NUM = 0x2,
		};

		/* 526 */
		enum DynEntityType : __int32
		{
			DYNENT_TYPE_INVALID = 0x0,
			DYNENT_TYPE_CLUTTER = 0x1,
			DYNENT_TYPE_DESTRUCT = 0x2,
			DYNENT_TYPE_COUNT = 0x3,
		};

		/* 477 */
		enum rope_constraint_e : __int32
		{
			ROPE_PAIR_CONSTRAINT = 0x0,
			ROPE_WORLD_CONSTRAINT = 0x1,
			ROPE_DENTITY_CONSTRAINT = 0x2,
			ROPE_CENTITY_CONSTRAINT = 0x3,
		};

		/* 328 */
		enum expDataType : __int32
		{
			VAL_INT = 0x0,
			VAL_FLOAT = 0x1,
			VAL_STRING = 0x2,
			VAL_ADDRESS = 0x3,
			VAL_DVAR = 0x4,
		};

		/* 452 */
		enum nodeType : __int32
		{
			NODE_BADNODE = 0x0,
			NODE_PATHNODE = 0x1,
			NODE_COVER_STAND = 0x2,
			NODE_COVER_CROUCH = 0x3,
			NODE_COVER_CROUCH_WINDOW = 0x4,
			NODE_COVER_PRONE = 0x5,
			NODE_COVER_RIGHT = 0x6,
			NODE_COVER_LEFT = 0x7,
			NODE_COVER_PILLAR = 0x8,
			NODE_AMBUSH = 0x9,
			NODE_EXPOSED = 0xA,
			NODE_CONCEALMENT_STAND = 0xB,
			NODE_CONCEALMENT_CROUCH = 0xC,
			NODE_CONCEALMENT_PRONE = 0xD,
			NODE_REACQUIRE = 0xE,
			NODE_BALCONY = 0xF,
			NODE_SCRIPTED = 0x10,
			NODE_NEGOTIATION_BEGIN = 0x11,
			NODE_NEGOTIATION_END = 0x12,
			NODE_TURRET = 0x13,
			NODE_GUARD = 0x14,
			NODE_NUMTYPES = 0x15,
			NODE_DONTLINK = 0x15,
		};

		/* 357 */
		enum weapType_t : __int32
		{
			WEAPTYPE_BULLET = 0x0,
			WEAPTYPE_GRENADE = 0x1,
			WEAPTYPE_PROJECTILE = 0x2,
			WEAPTYPE_BINOCULARS = 0x3,
			WEAPTYPE_GAS = 0x4,
			WEAPTYPE_BOMB = 0x5,
			WEAPTYPE_MINE = 0x6,
			WEAPTYPE_MELEE = 0x7,
			WEAPTYPE_RIOTSHIELD = 0x8,
			WEAPTYPE_NUM = 0x9,
		};

		/* 385 */
		enum weapClass_t : __int32
		{
			WEAPCLASS_RIFLE = 0x0,
			WEAPCLASS_MG = 0x1,
			WEAPCLASS_SMG = 0x2,
			WEAPCLASS_SPREAD = 0x3,
			WEAPCLASS_PISTOL = 0x4,
			WEAPCLASS_GRENADE = 0x5,
			WEAPCLASS_ROCKETLAUNCHER = 0x6,
			WEAPCLASS_TURRET = 0x7,
			WEAPCLASS_NON_PLAYER = 0x8,
			WEAPCLASS_GAS = 0x9,
			WEAPCLASS_ITEM = 0xA,
			WEAPCLASS_MELEE = 0xB,
			WEAPCLASS_KILLSTREAK_ALT_STORED_WEAPON = 0xC,
			WEAPCLASS_PISTOL_SPREAD = 0xD,
			WEAPCLASS_NUM = 0xE,
		};

		/* 338 */
		enum ImpactType : __int32
		{
			IMPACT_TYPE_NONE = 0x0,
			IMPACT_TYPE_BULLET_SMALL = 0x1,
			IMPACT_TYPE_BULLET_LARGE = 0x2,
			IMPACT_TYPE_BULLET_AP = 0x3,
			IMPACT_TYPE_BULLET_XTREME = 0x4,
			IMPACT_TYPE_SHOTGUN = 0x5,
			IMPACT_TYPE_GRENADE_BOUNCE = 0x6,
			IMPACT_TYPE_GRENADE_EXPLODE = 0x7,
			IMPACT_TYPE_RIFLE_GRENADE = 0x8,
			IMPACT_TYPE_ROCKET_EXPLODE = 0x9,
			IMPACT_TYPE_ROCKET_EXPLODE_XTREME = 0xA,
			IMPACT_TYPE_PROJECTILE_DUD = 0xB,
			IMPACT_TYPE_MORTAR_SHELL = 0xC,
			IMPACT_TYPE_TANK_SHELL = 0xD,
			IMPACT_TYPE_BOLT = 0xE,
			IMPACT_TYPE_BLADE = 0xF,
			IMPACT_TYPE_COUNT = 0x10,
		};

		/* 341 */
		enum weapInventoryType_t : __int32
		{
			WEAPINVENTORY_PRIMARY = 0x0,
			WEAPINVENTORY_OFFHAND = 0x1,
			WEAPINVENTORY_ITEM = 0x2,
			WEAPINVENTORY_ALTMODE = 0x3,
			WEAPINVENTORY_MELEE = 0x4,
			WEAPINVENTORY_DWLEFTHAND = 0x5,
			WEAPINVENTORYCOUNT = 0x6,
		};

		/* 399 */
		enum weapClipType_t : __int32
		{
			WEAPON_CLIPTYPE_BOTTOM = 0x0,
			WEAPON_CLIPTYPE_TOP = 0x1,
			WEAPON_CLIPTYPE_LEFT = 0x2,
			WEAPON_CLIPTYPE_DP28 = 0x3,
			WEAPON_CLIPTYPE_PTRS = 0x4,
			WEAPON_CLIPTYPE_LMG = 0x5,
			WEAPON_CLIPTYPECOUNT = 0x6,
		};

		/* 345 */
		enum barrelType_t : __int32
		{
			BARREL_TYPE_SINGLE = 0x0,
			BARREL_TYPE_DUAL = 0x1,
			BARREL_TYPE_DUAL_ALTERNATE = 0x2,
			BARREL_TYPE_QUAD = 0x3,
			BARREL_TYPE_QUAD_ALTERNATE = 0x4,
			BARREL_TYPE_QUAD_DOUBLE_ALTERNATE = 0x5,
			BARREL_TYPE_COUNT = 0x6,
		};

		/* 334 */
		enum OffhandClass : __int32
		{
			OFFHAND_CLASS_NONE = 0x0,
			OFFHAND_CLASS_FRAG_GRENADE = 0x1,
			OFFHAND_CLASS_SMOKE_GRENADE = 0x2,
			OFFHAND_CLASS_FLASH_GRENADE = 0x3,
			OFFHAND_CLASS_GEAR = 0x4,
			OFFHAND_CLASS_SUPPLYDROP_MARKER = 0x5,
			OFFHAND_CLASS_COUNT = 0x6,
		};

		/* 392 */
		enum OffhandSlot : __int32
		{
			OFFHAND_SLOT_NONE = 0x0,
			OFFHAND_SLOT_LETHAL_GRENADE = 0x1,
			OFFHAND_SLOT_TACTICAL_GRENADE = 0x2,
			OFFHAND_SLOT_EQUIPMENT = 0x3,
			OFFHAND_SLOT_SPECIFIC_USE = 0x4,
			OFFHAND_SLOT_COUNT = 0x5,
		};

		/* 411 */
		enum weapStance_t : __int32
		{
			WEAPSTANCE_STAND = 0x0,
			WEAPSTANCE_DUCK = 0x1,
			WEAPSTANCE_PRONE = 0x2,
			WEAPSTANCE_NUM = 0x3,
		};

		/* 336 */
		enum activeReticleType_t : __int32
		{
			VEH_ACTIVE_RETICLE_NONE = 0x0,
			VEH_ACTIVE_RETICLE_PIP_ON_A_STICK = 0x1,
			VEH_ACTIVE_RETICLE_BOUNCING_DIAMOND = 0x2,
			VEH_ACTIVE_RETICLE_MISSILE_LOCK = 0x3,
			VEH_ACTIVE_RETICLE_COUNT = 0x4,
		};

		/* 337 */
		enum ammoCounterClipType_t : __int32
		{
			AMMO_COUNTER_CLIP_NONE = 0x0,
			AMMO_COUNTER_CLIP_MAGAZINE = 0x1,
			AMMO_COUNTER_CLIP_SHORTMAGAZINE = 0x2,
			AMMO_COUNTER_CLIP_SHOTGUN = 0x3,
			AMMO_COUNTER_CLIP_ROCKET = 0x4,
			AMMO_COUNTER_CLIP_BELTFED = 0x5,
			AMMO_COUNTER_CLIP_ALTWEAPON = 0x6,
			AMMO_COUNTER_CLIP_COUNT = 0x7,
		};

		/* 371 */
		enum WeapOverlayInteface_t : __int32
		{
			WEAPOVERLAYINTERFACE_NONE = 0x0,
			WEAPOVERLAYINTERFACE_JAVELIN = 0x1,
			WEAPOVERLAYINTERFACE_TURRETSCOPE = 0x2,
			WEAPOVERLAYINTERFACECOUNT = 0x3,
		};

		/* 335 */
		enum weapProjExposion_t : __int32
		{
			WEAPPROJEXP_GRENADE = 0x0,
			WEAPPROJEXP_ROCKET = 0x1,
			WEAPPROJEXP_FLASHBANG = 0x2,
			WEAPPROJEXP_NONE = 0x3,
			WEAPPROJEXP_DUD = 0x4,
			WEAPPROJEXP_SMOKE = 0x5,
			WEAPPROJEXP_HEAVY = 0x6,
			WEAPPROJEXP_FIRE = 0x7,
			WEAPPROJEXP_NAPALMBLOB = 0x8,
			WEAPPROJEXP_BOLT = 0x9,
			WEAPPROJEXP_SHRAPNELSPAN = 0xA,
			WEAPPROJEXP_NUM = 0xB,
		};

		/* 384 */
		enum WeapStickinessType : __int32
		{
			WEAPSTICKINESS_NONE = 0x0,
			WEAPSTICKINESS_ALL = 0x1,
			WEAPSTICKINESS_ALL_NO_SENTIENTS = 0x2,
			WEAPSTICKINESS_GROUND = 0x3,
			WEAPSTICKINESS_GROUND_WITH_YAW = 0x4,
			WEAPSTICKINESS_FLESH = 0x5,
			WEAPSTICKINESS_COUNT = 0x6,
		};

		/* 407 */
		enum WeapRotateType : __int32
		{
			WEAPROTATE_GRENADE_ROTATE = 0x0,
			WEAPROTATE_BLADE_ROTATE = 0x1,
			WEAPROTATE_CYLINDER_ROTATE = 0x2,
			WEAPROTATE_COUNT = 0x3,
		};

		/* 332 */
		enum guidedMissileType_t : __int32
		{
			MISSILE_GUIDANCE_NONE = 0x0,
			MISSILE_GUIDANCE_SIDEWINDER = 0x1,
			MISSILE_GUIDANCE_HELLFIRE = 0x2,
			MISSILE_GUIDANCE_JAVELIN = 0x3,
			MISSILE_GUIDANCE_BALLISTIC = 0x4,
			MISSILE_GUIDANCE_WIREGUIDED = 0x5,
			MISSILE_GUIDANCE_TVGUIDED = 0x6,
			MISSILE_GUIDANCE_DRONE = 0x7,
			MISSILE_GUIDANCE_HEATSEEKING = 0x8,
			MISSILE_GUIDANCE_COUNT = 0x9,
		};

		/* 500 */
		enum SndMenuCategory : __int32
		{
			SND_CATEGORY_SFX = 0x0,
			SND_CATEGORY_MUSIC = 0x1,
			SND_CATEGORY_VOICE = 0x2,
			SND_CATEGORY_UI = 0x3,
			SND_CATEGORY_CINEMATIC = 0x4,
			SND_CATEGORY_COUNT = 0x5,
		};

		/* 1180 */
		enum LbColType : __int32
		{
			LBCOL_TYPE_NUMBER = 0x0,
			LBCOL_TYPE_TIME = 0x1,
			LBCOL_TYPE_LEVELXP = 0x2,
			LBCOL_TYPE_PRESTIGE = 0x3,
			LBCOL_TYPE_BIGNUMBER = 0x4,
			LBCOL_TYPE_PERCENT = 0x5,
			LBCOL_TYPE_TIME_FULL = 0x6,
			LBCOL_TYPE_COUNT = 0x7,
		};

		/* 1178 */
		enum LbAggType : __int32
		{
			LBAGG_TYPE_MIN = 0x0,
			LBAGG_TYPE_MAX = 0x1,
			LBAGG_TYPE_ADD = 0x2,
			LBAGG_TYPE_REPLACE = 0x3,
			LBAGG_TYPE_COUNT = 0x4,
		};

		/* 656 */
		enum DevEventType : __int32
		{
			EVENT_ACTIVATE = 0x0,
			EVENT_DEACTIVATE = 0x1,
			EVENT_ACCEPT = 0x2,
			EVENT_UPDATE = 0x3,
			EVENT_DRAW = 0x4,
			EVENT_SAVE = 0x5,
		};

		/* 284 */
		enum LocalClientNum_t
		{
			INVALID_LOCAL_CLIENT = 0xFFFFFFFF,
			LOCAL_CLIENT_FIRST = 0x0,
			LOCAL_CLIENT_0 = 0x0,
			ONLY_LOCAL_CLIENT = 0x0,
			LOCAL_CLIENT_COUNT = 0x1,
		};

		/* 1010 */
		enum dvarType_t : __int32
		{
			DVAR_TYPE_INVALID = 0x0,
			DVAR_TYPE_BOOL = 0x1,
			DVAR_TYPE_FLOAT = 0x2,
			DVAR_TYPE_FLOAT_2 = 0x3,
			DVAR_TYPE_FLOAT_3 = 0x4,
			DVAR_TYPE_FLOAT_4 = 0x5,
			DVAR_TYPE_INT = 0x6,
			DVAR_TYPE_ENUM = 0x7,
			DVAR_TYPE_STRING = 0x8,
			DVAR_TYPE_COLOR = 0x9,
			DVAR_TYPE_INT64 = 0xA,
			DVAR_TYPE_LINEAR_COLOR_RGB = 0xB,
			DVAR_TYPE_COLOR_XYZ = 0xC,
			DVAR_TYPE_COUNT = 0xD,
		};

		struct XBlock
		{
			unsigned __int8* data;
			unsigned int size;
		};

		struct XZone
		{
			unsigned __int16 index;
			int flags;
			XBlock blocks[8];
		};

		struct __declspec(align(128)) DB_LOAD_STREAM
		{
			unsigned __int8 decompressedData[32768];
			unsigned __int8 checksum[32];
			unsigned __int8* compressedData;
			int compressedDataAvail;
			int decompressedDataAvail;
			int decompressedDataOffset;
			bool busy;
		};

		struct DBDecompressCmd_t
		{
			int which;
			DB_LOAD_STREAM* stream;
			char* ioBuffer;
			char iv[8];
		};


		struct DB_LoadData
		{
			int f;
			const char* filename;
			XBlock* blocks;
			int flags;
			int allocType;
			unsigned __int64 readOffset;
			unsigned __int64 endOffset;
			unsigned __int8* ioBuffer;
			int ioBufferOffset;
			unsigned __int8* compressData;
			int tail;
			int head;
			DBDecompressCmd_t cmd[4];
			DB_LOAD_STREAM stream[4];
			int next_buffer;
			int locks[4];
			bool loadPending[4];
			bool abort;
			int requiredVersion;
			int lastError;
			int startTime;
		};

		typedef float vec4_t[4];
		typedef float vec3_t[3];
		typedef float vec2_t[2];

		struct Bounds
		{
			vec3_t midPoint;
			vec3_t halfSize;
		};

		/* 3485 */
		struct XModelPieces
		{
			const char* name;
			int numpieces;
			XModelPiece* pieces;
		};

		/* 2160 */
		struct PhysPreset
		{
			const char* name;
			int flags;
			float mass;
			float bounce;
			float friction;
			float bulletForceScale;
			float explosiveForceScale;
			const char* sndAliasPrefix;
			float piecesSpreadFraction;
			float piecesUpwardVelocity;
			int canFloat;
			float gravityScale;
			float centerOfMassOffset[3];
			float buoyancyBoxMin[3];
			float buoyancyBoxMax[3];
		};

		/* 1984 */
		struct PhysConstraint
		{
			unsigned __int16 targetname;
			ConstraintType type;
			AttachPointType attach_point_type1;
			int target_index1;
			unsigned __int16 target_ent1;
			const char* target_bone1;
			AttachPointType attach_point_type2;
			int target_index2;
			unsigned __int16 target_ent2;
			const char* target_bone2;
			vec3_t offset;
			vec3_t pos;
			vec3_t pos2;
			vec3_t dir;
			int flags;
			int timeout;
			int min_health;
			int max_health;
			float distance;
			float damp;
			float power;
			vec3_t scale;
			float spin_scale;
			float minAngle;
			float maxAngle;
			Material* material;
			int constraintHandle;
			int rope_index;
			int centity_num[4];
		};

		/* 2134 */
		struct PhysConstraints
		{
			const char* name;
			unsigned int count;
			PhysConstraint data[16];
		};

		/* 3064 */
		struct DestructibleDef
		{
			const char* name;
			XModel* model;
			XModel* pristineModel;
			int numPieces;
			DestructiblePiece* pieces;
			int clientOnly;
		};

		/* 3168 */
		union XAnimIndices
		{
			unsigned __int8* _1;
			unsigned __int16* _2;
			void* data;
		};

		/* 3182 */
		struct XAnimParts
		{
			const char* name;
			unsigned __int16 dataByteCount;
			unsigned __int16 dataShortCount;
			unsigned __int16 dataIntCount;
			unsigned __int16 randomDataByteCount;
			unsigned __int16 randomDataIntCount;
			unsigned __int16 framecount; // = numframes
			bool bLoop;
			bool bDelta;
			bool bDelta3D;
			bool bLeftHandGripIK;
			unsigned int streamedFileSize;
			unsigned __int8 boneCount[10];
			unsigned __int8 notetrackCount; // = notifyCount
			unsigned __int8 assetType;
			bool isDefault;
			unsigned int randomDataShortCount;
			unsigned int indexcount; // = indexcount
			float framerate;
			float frequency;
			float primedLength;
			float loopEntryTime;
			unsigned __int16* tagnames; // = names
			unsigned __int8* dataByte;
			__int16* dataShort;
			int* dataInt;
			__int16* randomDataShort;
			unsigned __int8* randomDataByte;
			int* randomDataInt;
			XAnimIndices indices;
			XAnimNotifyInfo* notetracks; // = notify
			XAnimDeltaPart* delta; // =deltaPart
		};

		/* 2677 */
		union $C8C573B57ACA1D7542AD56C4163862EC
		{
			unsigned __int16* localBoneNames;
			unsigned __int16* boneNames;
		};

		/* 2678 */
		union $37E13A30061EC97EDA76440B3BA5E97A
		{
			unsigned __int8* localParentList;
			unsigned __int8* parentList;
		};

		/* 2679 */
		union $BF9640C0B3AB5E078C286DD9616EC22F
		{
			__int16* localQuats;
			__int16* quats;
		};

		/* 2680 */
		union $E1CD6A013C1D28F2956F4983A8D1052C
		{
			float* localTrans;
			float* trans;
		};

		/* 2524 */
		struct XModelLodInfo
		{
			float dist;
			unsigned __int16 numSurfacesInLod;
			unsigned __int16 surfIndex;
			int partBits[5];
		};

		/* 2681 */
		union $D34615AEDEA286F5435AC593BA4E2048
		{
			XBoneInfo* localBoneInfo;
			XBoneInfo* boneInfo;
		};

		/* 2682 */

		struct XModelAngle
		{
			short x;
			short y;
			short z;
			short base;
		};

		struct XModel
		{
			const char* name;
			unsigned __int8 numBones;
			unsigned __int8 numRootBones;
			unsigned __int8 numSurfaces;
			unsigned __int8 lodRampType;
			unsigned __int16* boneNames;
			unsigned __int8* parentList;
			short* quats;
			float* trans;
			unsigned __int8* partClassification;
			DObjAnimMat* animMatrix;
			XSurface* surfs;
			Material** materials;
			XModelLodInfo lods[4];
			XModelCollSurf_s* collSurfs;
			int numCollSurfs;
			int contents;
			XBoneInfo* boneInfo;
			float radius;
			vec3_t mins;
			vec3_t maxs;
			__int16 numLods;
			__int16 collLod;
			float* himipInvSqRadii;
			int memUsage;
			int flags;
			bool bad;
			PhysPreset* physPreset;
			unsigned __int8 numCollmaps;
			Collmap* physCollmap;
			PhysConstraints* physConstraints;
			vec3_t lightingOriginOffset;
			float lightingOriginRange;
		}; //Size 0xF

		//int XModelSize = sizeof(XModel);

		/* 1852 */
		struct GfxDrawSurfFields
		{
			unsigned __int64 objectId : 16;
			unsigned __int64 customIndex : 9;
			unsigned __int64 reflectionProbeIndex : 5;
			unsigned __int64 dlightMask : 2;
			unsigned __int64 materialSortedIndex : 12;
			unsigned __int64 primaryLightIndex : 8;
			unsigned __int64 surfType : 4;
			unsigned __int64 prepass : 2;
			unsigned __int64 primarySortKey : 6;
		};

		/* 1853 */
		union GfxDrawSurf
		{
			GfxDrawSurfFields fields;
			unsigned __int64 packed;
		};

		enum MaterialWorldVertexFormat
		{
			MTL_WORLDVERT_TEX_1_NRM_1 = 0x0,
			MTL_WORLDVERT_TEX_2_NRM_1 = 0x1,
			MTL_WORLDVERT_TEX_2_NRM_2 = 0x2,
			MTL_WORLDVERT_TEX_3_NRM_1 = 0x3,
			MTL_WORLDVERT_TEX_3_NRM_2 = 0x4,
			MTL_WORLDVERT_TEX_3_NRM_3 = 0x5,
			MTL_WORLDVERT_TEX_4_NRM_1 = 0x6,
			MTL_WORLDVERT_TEX_4_NRM_2 = 0x7,
			MTL_WORLDVERT_TEX_4_NRM_3 = 0x8,
		};

		enum MaterialShaderArgumentType
		{
			MTL_ARG_MATERIAL_VERTEX_CONST = 0x0,
			MTL_ARG_LITERAL_VERTEX_CONST = 0x1,
			MTL_ARG_MATERIAL_PIXEL_SAMPLER = 0x2,
			MTL_ARG_CODE_VERTEX_CONST = 0x3,
			MTL_ARG_CODE_PIXEL_SAMPLER = 0x4,
			MTL_ARG_CODE_PIXEL_CONST = 0x5,
			MTL_ARG_MATERIAL_PIXEL_CONST = 0x6,
			MTL_ARG_LITERAL_PIXEL_CONST = 0x7,
			MLT_ARG_COUNT = 0x8,
		};

		enum MaterialConstSource : std::int32_t
		{
			CONST_SRC_CODE_MAYBE_DIRTY_PS_BEGIN = 0x0,
			CONST_SRC_CODE_LIGHT_POSITION = 0x0,
			CONST_SRC_CODE_LIGHT_DIFFUSE = 0x1,
			CONST_SRC_CODE_LIGHT_SPOTDIR = 0x2,
			CONST_SRC_CODE_LIGHT_SPOTFACTORS = 0x3,
			CONST_SRC_CODE_LIGHT_ATTENUATION = 0x4,
			CONST_SRC_CODE_LIGHT_FALLOFF_A = 0x5,
			CONST_SRC_CODE_LIGHT_FALLOFF_B = 0x6,
			CONST_SRC_CODE_LIGHT_SPOT_MATRIX0 = 0x7,
			CONST_SRC_CODE_LIGHT_SPOT_MATRIX1 = 0x8,
			CONST_SRC_CODE_LIGHT_SPOT_MATRIX2 = 0x9,
			CONST_SRC_CODE_LIGHT_SPOT_MATRIX3 = 0xA,
			CONST_SRC_CODE_LIGHT_SPOT_AABB = 0xB,
			CONST_SRC_CODE_LIGHT_CONE_CONTROL1 = 0xC,
			CONST_SRC_CODE_LIGHT_CONE_CONTROL2 = 0xD,
			CONST_SRC_CODE_LIGHT_SPOT_COOKIE_SLIDE_CONTROL = 0xE,
			CONST_SRC_CODE_SHADOW_PARMS = 0xF,
			CONST_SRC_CODE_SHADOWMAP_POLYGON_OFFSET = 0x10,
			CONST_SRC_CODE_RENDER_TARGET_SIZE = 0x11,
			CONST_SRC_CODE_UPSCALED_TARGET_SIZE = 0x12,
			CONST_SRC_CODE_DOF_EQUATION_VIEWMODEL_AND_FAR_BLUR = 0x13,
			CONST_SRC_CODE_DOF_EQUATION_SCENE = 0x14,
			CONST_SRC_CODE_DOF_LERP_SCALE = 0x15,
			CONST_SRC_CODE_DOF_LERP_BIAS = 0x16,
			CONST_SRC_CODE_DOF_ROW_DELTA = 0x17,
			CONST_SRC_CODE_PARTICLE_CLOUD_COLOR = 0x18,
			CONST_SRC_CODE_GAMETIME = 0x19,
			CONST_SRC_CODE_MAYBE_DIRTY_PS_END = 0x1A,
			CONST_SRC_CODE_ALWAYS_DIRTY_PS_BEGIN = 0x1A,
			CONST_SRC_CODE_FILTER_TAP_0 = 0x1A,
			CONST_SRC_CODE_FILTER_TAP_1 = 0x1B,
			CONST_SRC_CODE_FILTER_TAP_2 = 0x1C,
			CONST_SRC_CODE_FILTER_TAP_3 = 0x1D,
			CONST_SRC_CODE_FILTER_TAP_4 = 0x1E,
			CONST_SRC_CODE_FILTER_TAP_5 = 0x1F,
			CONST_SRC_CODE_FILTER_TAP_6 = 0x20,
			CONST_SRC_CODE_FILTER_TAP_7 = 0x21,
			CONST_SRC_CODE_COLOR_MATRIX_R = 0x22,
			CONST_SRC_CODE_COLOR_MATRIX_G = 0x23,
			CONST_SRC_CODE_COLOR_MATRIX_B = 0x24,
			CONST_SRC_CODE_ALWAYS_DIRTY_PS_END = 0x25,
			CONST_SRC_CODE_NEVER_DIRTY_PS_BEGIN = 0x25,
			CONST_SRC_CODE_SHADOWMAP_SWITCH_PARTITION = 0x25,
			CONST_SRC_CODE_SUNSHADOWMAP_PIXEL_SIZE = 0x26,
			CONST_SRC_CODE_SHADOWMAP_SCALE = 0x27,
			CONST_SRC_CODE_ZNEAR = 0x28,
			CONST_SRC_CODE_SUN_POSITION = 0x29,
			CONST_SRC_CODE_SUN_DIFFUSE = 0x2A,
			CONST_SRC_CODE_LIGHTING_LOOKUP_SCALE = 0x2B,
			CONST_SRC_CODE_DEBUG_BUMPMAP = 0x2C,
			CONST_SRC_CODE_DEBUG_PERFORMANCE = 0x2D,
			CONST_SRC_CODE_MATERIAL_COLOR = 0x2E,
			CONST_SRC_CODE_FOG = 0x2F,
			CONST_SRC_CODE_FOG2 = 0x30,
			CONST_SRC_CODE_FOG_COLOR = 0x31,
			CONST_SRC_CODE_SUN_FOG = 0x32,
			CONST_SRC_CODE_SUN_FOG_DIR = 0x33,
			CONST_SRC_CODE_SUN_FOG_COLOR = 0x34,
			CONST_SRC_CODE_GLOW_SETUP = 0x35,
			CONST_SRC_CODE_GLOW_APPLY = 0x36,
			CONST_SRC_CODE_COLOR_BIAS = 0x37,
			CONST_SRC_CODE_COLOR_TINT_BASE = 0x38,
			CONST_SRC_CODE_COLOR_TINT_DELTA = 0x39,
			CONST_SRC_CODE_OUTDOOR_FEATHER_PARMS = 0x3A,
			CONST_SRC_CODE_SKY_TRANSITION = 0x3B,
			CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST = 0x3C,
			CONST_SRC_CODE_DLIGHT_SPOT_SHADOWMAP_PIXEL_ADJUST = 0x3D,
			CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE = 0x3E,
			CONST_SRC_CODE_CLIP_SPACE_LOOKUP_OFFSET = 0x3F,
			CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX = 0x40,
			CONST_SRC_CODE_PARTICLE_CLOUD_VEL_WORLD = 0x41,
			CONST_SRC_CODE_DEPTH_FROM_CLIP = 0x42,
			CONST_SRC_CODE_CODE_MESH_ARG_0 = 0x43,
			CONST_SRC_CODE_CODE_MESH_ARG_1 = 0x44,
			CONST_SRC_CODE_CODE_MESH_ARG_LAST = 0x44,
			CONST_SRC_CODE_GRID_LIGHTING_COORDS_AND_VIS = 0x45,
			CONST_SRC_CODE_GRID_LIGHTING_SH_0 = 0x46,
			CONST_SRC_CODE_GRID_LIGHTING_SH_1 = 0x47,
			CONST_SRC_CODE_GRID_LIGHTING_SH_2 = 0x48,
			CONST_SRC_CODE_REFLECTION_LIGHTING_SH_0 = 0x49,
			CONST_SRC_CODE_REFLECTION_LIGHTING_SH_1 = 0x4A,
			CONST_SRC_CODE_REFLECTION_LIGHTING_SH_2 = 0x4B,
			CONST_SRC_CODE_WIND_DIRECTION = 0x4C,
			CONST_SRC_CODE_MOTIONBLUR_DIRECTION_AND_MAGNITUDE = 0x4D,
			CONST_SRC_CODE_COMPOSITE_FX_DISTORTION = 0x4E,
			CONST_SRC_CODE_GLOW_BLOOM_SCALE = 0x4F,
			CONST_SRC_CODE_COMPOSITE_FX_OVERLAY_TEXCOORD = 0x50,
			CONST_SRC_CODE_COLOR_BIAS1 = 0x51,
			CONST_SRC_CODE_COLOR_TINT_BASE1 = 0x52,
			CONST_SRC_CODE_COLOR_TINT_DELTA1 = 0x53,
			CONST_SRC_CODE_POSTFX_FADE_EFFECT = 0x54,
			CONST_SRC_CODE_VIEWPORT_DIMENSIONS = 0x55,
			CONST_SRC_CODE_FRAMEBUFFER_READ = 0x56,
			CONST_SRC_CODE_RESIZE_PARAMS1 = 0x57,
			CONST_SRC_CODE_RESIZE_PARAMS2 = 0x58,
			CONST_SRC_CODE_RESIZE_PARAMS3 = 0x59,
			CONST_SRC_CODE_VARIANT_WIND_SPRING_0 = 0x5A,
			CONST_SRC_CODE_VARIANT_WIND_SPRING_1 = 0x5B,
			CONST_SRC_CODE_VARIANT_WIND_SPRING_2 = 0x5C,
			CONST_SRC_CODE_VARIANT_WIND_SPRING_3 = 0x5D,
			CONST_SRC_CODE_VARIANT_WIND_SPRING_4 = 0x5E,
			CONST_SRC_CODE_VARIANT_WIND_SPRING_5 = 0x5F,
			CONST_SRC_CODE_VARIANT_WIND_SPRING_6 = 0x60,
			CONST_SRC_CODE_VARIANT_WIND_SPRING_7 = 0x61,
			CONST_SRC_CODE_VARIANT_WIND_SPRING_8 = 0x62,
			CONST_SRC_CODE_VARIANT_WIND_SPRING_9 = 0x63,
			CONST_SRC_CODE_VARIANT_WIND_SPRING_10 = 0x64,
			CONST_SRC_CODE_VARIANT_WIND_SPRING_11 = 0x65,
			CONST_SRC_CODE_VARIANT_WIND_SPRING_12 = 0x66,
			CONST_SRC_CODE_VARIANT_WIND_SPRING_13 = 0x67,
			CONST_SRC_CODE_VARIANT_WIND_SPRING_14 = 0x68,
			CONST_SRC_CODE_VARIANT_WIND_SPRING_15 = 0x69,
			CONST_SRC_CODE_CHARACTER_CHARRED_AMOUNT = 0x6A,
			CONST_SRC_CODE_POSTFX_CONTROL0 = 0x6B,
			CONST_SRC_CODE_POSTFX_CONTROL1 = 0x6C,
			CONST_SRC_CODE_POSTFX_CONTROL2 = 0x6D,
			CONST_SRC_CODE_POSTFX_CONTROL3 = 0x6E,
			CONST_SRC_CODE_POSTFX_CONTROL4 = 0x6F,
			CONST_SRC_CODE_POSTFX_CONTROL5 = 0x70,
			CONST_SRC_CODE_POSTFX_CONTROL6 = 0x71,
			CONST_SRC_CODE_POSTFX_CONTROL7 = 0x72,
			CONST_SRC_CODE_POSTFX_CONTROL8 = 0x73,
			CONST_SRC_CODE_POSTFX_CONTROL9 = 0x74,
			CONST_SRC_CODE_POSTFX_CONTROLA = 0x75,
			CONST_SRC_CODE_POSTFX_CONTROLB = 0x76,
			CONST_SRC_CODE_POSTFX_CONTROLC = 0x77,
			CONST_SRC_CODE_POSTFX_CONTROLD = 0x78,
			CONST_SRC_CODE_POSTFX_CONTROLE = 0x79,
			CONST_SRC_CODE_POSTFX_CONTROLF = 0x7A,
			CONST_SRC_CODE_HDRCONTROL_0 = 0x7B,
			CONST_SRC_CODE_HDRCONTROL_1 = 0x7C,
			CONST_SRC_CODE_GLIGHT_POSXS = 0x7D,
			CONST_SRC_CODE_GLIGHT_POSYS = 0x7E,
			CONST_SRC_CODE_GLIGHT_POSZS = 0x7F,
			CONST_SRC_CODE_GLIGHT_FALLOFFS = 0x80,
			CONST_SRC_CODE_GLIGHT_REDS = 0x81,
			CONST_SRC_CODE_GLIGHT_GREENS = 0x82,
			CONST_SRC_CODE_GLIGHT_BLUES = 0x83,
			CONST_SRC_CODE_DLIGHT_POSITION = 0x84,
			CONST_SRC_CODE_DLIGHT_DIFFUSE = 0x85,
			CONST_SRC_CODE_DLIGHT_ATTENUATION = 0x86,
			CONST_SRC_CODE_DLIGHT_FALLOFF = 0x87,
			CONST_SRC_CODE_DLIGHT_SPOT_MATRIX_0 = 0x88,
			CONST_SRC_CODE_DLIGHT_SPOT_MATRIX_1 = 0x89,
			CONST_SRC_CODE_DLIGHT_SPOT_MATRIX_2 = 0x8A,
			CONST_SRC_CODE_DLIGHT_SPOT_MATRIX_3 = 0x8B,
			CONST_SRC_CODE_DLIGHT_SPOT_DIR = 0x8C,
			CONST_SRC_CODE_DLIGHT_SPOT_FACTORS = 0x8D,
			CONST_SRC_CODE_DLIGHT_SHADOW_LOOKUP_MATRIX_0 = 0x8E,
			CONST_SRC_CODE_DLIGHT_SHADOW_LOOKUP_MATRIX_1 = 0x8F,
			CONST_SRC_CODE_DLIGHT_SHADOW_LOOKUP_MATRIX_2 = 0x90,
			CONST_SRC_CODE_DLIGHT_SHADOW_LOOKUP_MATRIX_3 = 0x91,
			CONST_SRC_CODE_CLOUD_LAYER_CONTROL0 = 0x92,
			CONST_SRC_CODE_CLOUD_LAYER_CONTROL1 = 0x93,
			CONST_SRC_CODE_CLOUD_LAYER_CONTROL2 = 0x94,
			CONST_SRC_CODE_CLOUD_LAYER_CONTROL3 = 0x95,
			CONST_SRC_CODE_CLOUD_LAYER_CONTROL4 = 0x96,
			CONST_SRC_CODE_HERO_LIGHTING_R = 0x97,
			CONST_SRC_CODE_HERO_LIGHTING_G = 0x98,
			CONST_SRC_CODE_HERO_LIGHTING_B = 0x99,
			CONST_SRC_CODE_LIGHT_HERO_SCALE = 0x9A,
			CONST_SRC_CODE_CINEMATIC_BLUR_BOX = 0x9B,
			CONST_SRC_CODE_CINEMATIC_BLUR_BOX2 = 0x9C,
			CONST_SRC_CODE_ADSZSCALE = 0x9D,
			CONST_SRC_CODE_UI3D_UV_SETUP_0 = 0x9E,
			CONST_SRC_CODE_UI3D_UV_SETUP_1 = 0x9F,
			CONST_SRC_CODE_UI3D_UV_SETUP_2 = 0xA0,
			CONST_SRC_CODE_UI3D_UV_SETUP_3 = 0xA1,
			CONST_SRC_CODE_UI3D_UV_SETUP_4 = 0xA2,
			CONST_SRC_CODE_UI3D_UV_SETUP_5 = 0xA3,
			CONST_SRC_CODE_CHARACTER_DISSOLVE_COLOR = 0xA4,
			CONST_SRC_CODE_CAMERA_LOOK = 0xA5,
			CONST_SRC_CODE_CAMERA_UP = 0xA6,
			CONST_SRC_CODE_CAMERA_SIDE = 0xA7,
			CONST_SRC_CODE_RIMINTENSITY = 0xA8,
			CONST_SRC_CODE_GENERIC_PARAM0 = 0xA9,
			CONST_SRC_CODE_GENERIC_PARAM1 = 0xAA,
			CONST_SRC_CODE_GENERIC_PARAM2 = 0xAB,
			CONST_SRC_CODE_GENERIC_PARAM3 = 0xAC,
			CONST_SRC_CODE_GENERIC_PARAM4 = 0xAD,
			CONST_SRC_CODE_GENERIC_PARAM5 = 0xAE,
			CONST_SRC_CODE_GENERIC_PARAM6 = 0xAF,
			CONST_SRC_CODE_GENERIC_PARAM7 = 0xB0,
			CONST_SRC_CODE_GENERIC_EYEOFFSET = 0xB1,
			CONST_SRC_CODE_GENERIC_QUADINTENSITY = 0xB2,
			CONST_SRC_CODE_WEAPON_PARAM0 = 0xB3,
			CONST_SRC_CODE_WEAPON_PARAM1 = 0xB4,
			CONST_SRC_CODE_WEAPON_PARAM2 = 0xB5,
			CONST_SRC_CODE_WEAPON_PARAM3 = 0xB6,
			CONST_SRC_CODE_WEAPON_PARAM4 = 0xB7,
			CONST_SRC_CODE_WEAPON_PARAM5 = 0xB8,
			CONST_SRC_CODE_WEAPON_PARAM6 = 0xB9,
			CONST_SRC_CODE_WEAPON_PARAM7 = 0xBA,
			CONST_SRC_CODE_WEAPON_PARAM8 = 0xBB,
			CONST_SRC_CODE_WEAPON_PARAM9 = 0xBC,
			CONST_SRC_CODE_QRCODE_0 = 0xBD,
			CONST_SRC_CODE_QRCODE_1 = 0xBE,
			CONST_SRC_CODE_QRCODE_2 = 0xBF,
			CONST_SRC_CODE_QRCODE_3 = 0xC0,
			CONST_SRC_CODE_QRCODE_4 = 0xC1,
			CONST_SRC_CODE_QRCODE_5 = 0xC2,
			CONST_SRC_CODE_QRCODE_6 = 0xC3,
			CONST_SRC_CODE_QRCODE_7 = 0xC4,
			CONST_SRC_CODE_QRCODE_8 = 0xC5,
			CONST_SRC_CODE_QRCODE_9 = 0xC6,
			CONST_SRC_CODE_QRCODE_10 = 0xC7,
			CONST_SRC_CODE_QRCODE_11 = 0xC8,
			CONST_SRC_CODE_EYEOFFSET = 0xC9,
			CONST_SRC_CODE_SKY_COLOR_MULTIPLIER = 0xCA,
			CONST_SRC_CODE_EXTRA_CAM_PARAM = 0xCB,
			CONST_SRC_CODE_EMBLEM_LUT_SELECTOR = 0xCC,
			CONST_SRC_CODE_DEBUG_COLOR_OVERRIDE = 0xCD,
			CONST_SRC_CODE_DEBUG_ALPHA_OVERRIDE = 0xCE,
			CONST_SRC_CODE_DEBUG_NORMAL_OVERRIDE = 0xCF,
			CONST_SRC_CODE_DEBUG_SPECULAR_OVERRIDE = 0xD0,
			CONST_SRC_CODE_DEBUG_GLOSS_OVERRIDE = 0xD1,
			CONST_SRC_CODE_DEBUG_OCCLUSION_OVERRIDE = 0xD2,
			CONST_SRC_CODE_NEVER_DIRTY_PS_END = 0xD3,
			CONST_SRC_CODE_COUNT_FLOAT4 = 0xD3,
			CONST_SRC_FIRST_CODE_MATRIX = 0xD3,
			CONST_SRC_CODE_WORLD_MATRIX = 0xD3,
			CONST_SRC_CODE_INVERSE_WORLD_MATRIX = 0xD4,
			CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX = 0xD5,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX = 0xD6,
			CONST_SRC_CODE_VIEW_MATRIX = 0xD7,
			CONST_SRC_CODE_INVERSE_VIEW_MATRIX = 0xD8,
			CONST_SRC_CODE_TRANSPOSE_VIEW_MATRIX = 0xD9,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_MATRIX = 0xDA,
			CONST_SRC_CODE_PROJECTION_MATRIX = 0xDB,
			CONST_SRC_CODE_INVERSE_PROJECTION_MATRIX = 0xDC,
			CONST_SRC_CODE_TRANSPOSE_PROJECTION_MATRIX = 0xDD,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_PROJECTION_MATRIX = 0xDE,
			CONST_SRC_CODE_WORLD_VIEW_MATRIX = 0xDF,
			CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX = 0xE0,
			CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX = 0xE1,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX = 0xE2,
			CONST_SRC_CODE_VIEW_PROJECTION_MATRIX = 0xE3,
			CONST_SRC_CODE_INVERSE_VIEW_PROJECTION_MATRIX = 0xE4,
			CONST_SRC_CODE_TRANSPOSE_VIEW_PROJECTION_MATRIX = 0xE5,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_PROJECTION_MATRIX = 0xE6,
			CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX = 0xE7,
			CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX = 0xE8,
			CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX = 0xE9,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX = 0xEA,
			CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX = 0xEB,
			CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX = 0xEC,
			CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX = 0xED,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX = 0xEE,
			CONST_SRC_CODE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0xEF,
			CONST_SRC_CODE_INVERSE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0xF0,
			CONST_SRC_CODE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0xF1,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0xF2,
			CONST_SRC_TOTAL_COUNT = 0xF3,
			CONST_SRC_NONE = 0xF4,
		};

		enum MaterialTextureSource
		{
			TEXTURE_SRC_CODE_BLACK = 0x0,
			TEXTURE_SRC_CODE_WHITE = 0x1,
			TEXTURE_SRC_CODE_IDENTITY_NORMAL_MAP = 0x2,
			TEXTURE_SRC_CODE_MODEL_LIGHTING = 0x3,
			TEXTURE_SRC_CODE_LIGHTMAP_PRIMARY = 0x4,
			TEXTURE_SRC_CODE_LIGHTMAP_SECONDARY = 0x5,
			TEXTURE_SRC_CODE_SHADOWMAP_SUN = 0x6,
			TEXTURE_SRC_CODE_SHADOWMAP_SPOT = 0x7,
			TEXTURE_SRC_CODE_FEEDBACK = 0x8,
			TEXTURE_SRC_CODE_RESOLVED_POST_SUN = 0x9,
			TEXTURE_SRC_CODE_RESOLVED_SCENE = 0xA,
			TEXTURE_SRC_CODE_POST_EFFECT_SRC = 0xB,
			TEXTURE_SRC_CODE_POST_EFFECT_GODRAYS = 0xC,
			TEXTURE_SRC_CODE_POST_EFFECT_0 = 0xD,
			TEXTURE_SRC_CODE_POST_EFFECT_1 = 0xE,
			TEXTURE_SRC_CODE_LIGHT_ATTENUATION = 0xF,
			TEXTURE_SRC_CODE_DLIGHT_ATTENUATION = 0x10,
			TEXTURE_SRC_CODE_OUTDOOR = 0x11,
			TEXTURE_SRC_CODE_FLOATZ = 0x12,
			TEXTURE_SRC_CODE_PROCESSED_FLOATZ = 0x13,
			TEXTURE_SRC_CODE_RAW_FLOATZ = 0x14,
			TEXTURE_SRC_CODE_STENCIL = 0x15,
			TEXTURE_SRC_CODE_CINEMATIC_Y = 0x16,
			TEXTURE_SRC_CODE_CINEMATIC_CR = 0x17,
			TEXTURE_SRC_CODE_CINEMATIC_CB = 0x18,
			TEXTURE_SRC_CODE_CINEMATIC_A = 0x19,
			TEXTURE_SRC_CODE_REFLECTION_PROBE = 0x1A,
			TEXTURE_SRC_CODE_FEATHER_FLOAT_Z = 0x1B,
			TEXTURE_SRC_CODE_TEXTURE_0 = 0x1C,
			TEXTURE_SRC_CODE_TEXTURE_1 = 0x1D,
			TEXTURE_SRC_CODE_TEXTURE_2 = 0x1E,
			TEXTURE_SRC_CODE_TEXTURE_3 = 0x1F,
			TEXTURE_SRC_CODE_IMPACT_MASK_DEPRECATED = 0x20,
			TEXTURE_SRC_CODE_UI3D = 0x21,
			TEXTURE_SRC_CODE_MISSILE_CAM = 0x22,
			TEXTURE_SRC_CODE_MISSILE_CAM_0 = 0x23,
			TEXTURE_SRC_CODE_MISSILE_CAM_1 = 0x24,
			TEXTURE_SRC_CODE_MISSILE_CAM_2 = 0x25,
			TEXTURE_SRC_CODE_MISSILE_CAM_3 = 0x26,
			TEXTURE_SRC_CODE_COMPOSITE_RESULT = 0x27,
			TEXTURE_SRC_CODE_HEATMAP = 0x28,
			TEXTURE_SRC_CODE_SONAR_COLOR = 0x29,
			TEXTURE_SRC_CODE_SONAR_DEPTH = 0x2A,
			TEXTURE_SRC_CODE_QRCODE_0 = 0x2B,
			TEXTURE_SRC_CODE_QRCODE_1 = 0x2C,
			TEXTURE_SRC_CODE_QRCODE_2 = 0x2D,
			TEXTURE_SRC_CODE_QRCODE_3 = 0x2E,
			TEXTURE_SRC_CODE_QRCODE_4 = 0x2F,
			TEXTURE_SRC_CODE_QRCODE_5 = 0x30,
			TEXTURE_SRC_CODE_QRCODE_6 = 0x31,
			TEXTURE_SRC_CODE_QRCODE_7 = 0x32,
			TEXTURE_SRC_CODE_QRCODE_8 = 0x33,
			TEXTURE_SRC_CODE_QRCODE_9 = 0x34,
			TEXTURE_SRC_CODE_QRCODE_10 = 0x35,
			TEXTURE_SRC_CODE_QRCODE_11 = 0x36,
			TEXTURE_SRC_CODE_COUNT = 0x37,
		};

		enum MaterialTechniqueType
		{
			TECHNIQUE_DEPTH_PREPASS = 0x0,
			TECHNIQUE_BUILD_SHADOWMAP_DEPTH = 0x1,
			TECHNIQUE_UNLIT = 0x2,
			TECHNIQUE_EMISSIVE = 0x3,
			TECHNIQUE_LIT_BEGIN = 0x4,
			TECHNIQUE_LIT = 0x4,
			TECHNIQUE_LIT_SUN = 0x5,
			TECHNIQUE_LIT_SUN_SHADOW = 0x6,
			TECHNIQUE_LIT_SPOT = 0x7,
			TECHNIQUE_LIT_SPOT_SHADOW = 0x8,
			TECHNIQUE_LIT_SPOT_SQUARE = 0x9,
			TECHNIQUE_LIT_SPOT_SQUARE_SHADOW = 0xA,
			TECHNIQUE_LIT_SPOT_ROUND = 0xB,
			TECHNIQUE_LIT_SPOT_ROUND_SHADOW = 0xC,
			TECHNIQUE_LIT_OMNI = 0xD,
			TECHNIQUE_LIT_OMNI_SHADOW = 0xE,
			TECHNIQUE_LIT_DLIGHT_GLIGHT = 0xF,
			TECHNIQUE_LIT_SUN_DLIGHT_GLIGHT = 0x10,
			TECHNIQUE_LIT_SUN_SHADOW_DLIGHT_GLIGHT = 0x11,
			TECHNIQUE_LIT_SPOT_DLIGHT_GLIGHT = 0x12,
			TECHNIQUE_LIT_SPOT_SHADOW_DLIGHT_GLIGHT = 0x13,
			TECHNIQUE_LIT_SPOT_SQUARE_DLIGHT_GLIGHT = 0x14,
			TECHNIQUE_LIT_SPOT_SQUARE_SHADOW_DLIGHT_GLIGHT = 0x15,
			TECHNIQUE_LIT_SPOT_ROUND_DLIGHT_GLIGHT = 0x16,
			TECHNIQUE_LIT_SPOT_ROUND_SHADOW_DLIGHT_GLIGHT = 0x17,
			TECHNIQUE_LIT_OMNI_DLIGHT_GLIGHT = 0x18,
			TECHNIQUE_LIT_OMNI_SHADOW_DLIGHT_GLIGHT = 0x19,
			TECHNIQUE_LIT_END = 0x1A,
			TECHNIQUE_LIGHT_SPOT = 0x1A,
			TECHNIQUE_LIGHT_OMNI = 0x1B,
			TECHNIQUE_FAKELIGHT_NORMAL = 0x1C,
			TECHNIQUE_FAKELIGHT_VIEW = 0x1D,
			TECHNIQUE_SUNLIGHT_PREVIEW = 0x1E,
			TECHNIQUE_CASE_TEXTURE = 0x1F,
			TECHNIQUE_WIREFRAME_SOLID = 0x20,
			TECHNIQUE_WIREFRAME_SHADED = 0x21,
			TECHNIQUE_DEBUG_BUMPMAP = 0x22,
			TECHNIQUE_DEBUG_PERFORMANCE = 0x23,
			TECHNIQUE_COUNT = 0x24,
			TECHNIQUE_TOTAL_COUNT = 0x25,
			TECHNIQUE_CUSTOM_01 = 0x2,
			TECHNIQUE_CUSTOM_02 = 0x3,
			TECHNIQUE_CUSTOM_03 = 0x4,
			TECHNIQUE_CUSTOM_04 = 0x5,
			TECHNIQUE_CUSTOM_05 = 0x6,
			TECHNIQUE_CUSTOM_06 = 0x7,
			TECHNIQUE_CUSTOM_07 = 0x8,
			TECHNIQUE_CUSTOM_08 = 0x9,
			TECHNIQUE_CUSTOM_09 = 0xA,
			TECHNIQUE_CUSTOM_10 = 0xB,
			TECHNIQUE_CUSTOM_11 = 0xC,
			TECHNIQUE_CUSTOM_12 = 0xD,
			TECHNIQUE_CUSTOM_13 = 0xE,
			TECHNIQUE_CUSTOM_14 = 0xF,
			TECHNIQUE_CUSTOM_15 = 0x10,
			TECHNIQUE_CUSTOM_16 = 0x11,
			TECHNIQUE_CUSTOM_17 = 0x12,
			TECHNIQUE_CUSTOM_18 = 0x13,
			TECHNIQUE_CUSTOM_19 = 0x14,
			TECHNIQUE_CUSTOM_20 = 0x15,
			TECHNIQUE_CUSTOM_21 = 0x16,
			TECHNIQUE_CUSTOM_22 = 0x17,
			TECHNIQUE_CUSTOM_23 = 0x18,
			TECHNIQUE_CUSTOM_24 = 0x19,
			TECHNIQUE_CUSTOM_25 = 0x1A,
			TECHNIQUE_CUSTOM_26 = 0x1B,
			TECHNIQUE_CUSTOM_27 = 0x1C,
			TECHNIQUE_CUSTOM_28 = 0x1D,
			TECHNIQUE_CUSTOM_29 = 0x1E,
			TECHNIQUE_CUSTOM_30 = 0x1F,
			TECHNIQUE_CUSTOM_COUNT = 0x20,
			TECHNIQUE_NONE = 0xFF,
		};


		/* 1982 */
		union $86326EB2200266E247E1D0AD3A411D3A
		{
			MaterialTechniqueSet* localTechniqueSet;
			MaterialTechniqueSet* techniqueSet;
		};

		/* 1983 */
		union $DE6FD6455E5F44B1854F855CE54D14E8
		{
			MaterialConstantDef* localConstantTable;
			MaterialConstantDef* constantTable;
		};

		struct __declspec(align(8)) MaterialInfo
		{
			const char* name;
			unsigned int gameFlags;
			char pad;
			char sortKey;
			char textureAtlasRowCount;
			char textureAtlasColumnCount;
			GfxDrawSurf drawSurf;
			unsigned int surfaceTypeBits;
			unsigned int layeredSurfaceTypes;
			unsigned __int16 hashIndex;
			int surfaceFlags;
			int contents;
		};


		/* 1981 */
		struct Material
		{
			union
			{
				const char* name;
				MaterialInfo info;
			};
			unsigned char stateBitsEntry[36];
			char textureCount;
			char constantCount;
			char stateBitsCount;
			char stateFlags;
			char cameraRegion;
			char probeMipBits;
			MaterialTechniqueSet* techniqueSet;
			MaterialTextureDef* textureTable;
			MaterialConstantDef* constantTable;
			GfxStateBits* stateBitsTable;
			Material* thermalMaterial;
		};

		//int MaterialSize = sizeof(Material);

		/* 1959 */
		struct GfxPixelShaderLoadDef
		{
			unsigned __int8* program;
			unsigned int programSize;
		};

		/* 1960 */
		struct MaterialPixelShaderProgram
		{
			ID3D11PixelShader *ps;
			GfxPixelShaderLoadDef loadDef;
		};

		/* 1961 */
		struct MaterialPixelShader
		{
			const char* name;
			MaterialPixelShaderProgram prog;
		};

		struct GfxVertexShaderLoadDef
		{
			char* program;
			unsigned int programSize;
		};

		struct MaterialVertexShaderProgram
		{
			ID3D11VertexShader* vs;
			GfxVertexShaderLoadDef loadDef;
		};

		/* 1958 */
		struct MaterialVertexShader
		{
			const char* name;
			MaterialVertexShaderProgram prog;
		};

		/* 1971 */
		struct MaterialTechniqueSet
		{
			const char* name;
			unsigned __int8 worldVertFormat;
			MaterialTechnique* techniques[36];
		};

		/* 1973 */
		union GfxTexture
		{
			//ID3D11ShaderResourceView *basemap;
			GfxImageLoadDef* loadDef;
		};

		/* 1974 */
		struct Picmip
		{
			unsigned __int8 platform[2];
		};

		/* 1975 */
		struct CardMemory
		{
			int platform[2];
		};

		/* 1976 */
		struct GfxStreamedPartInfo
		{
			unsigned int levelCountAndSize;
			unsigned int hash;
			unsigned __int16 width;
			unsigned __int16 height;
			unsigned __int32 offset : 32;
			unsigned __int32 size : 28;
			unsigned __int32 ipakIndex : 4;
			unsigned __int32 adjacentLeft : 15;
			unsigned __int32 adjacentRight : 15;
			unsigned __int32 compressed : 1;
			unsigned __int32 valid : 1;
		};

		/* 1977 */
		struct GfxImage
		{
			GfxTexture texture;
			unsigned __int8 mapType;
			unsigned __int8 semantic;
			unsigned __int8 category;
			bool delayLoadPixels;
			Picmip picmip;
			bool noPicmip;
			unsigned __int8 track;
			CardMemory cardMemory;
			unsigned __int16 width;
			unsigned __int16 height;
			unsigned __int16 depth;
			unsigned __int8 levelCount;
			unsigned __int8 streaming;
			unsigned int baseSize;
			unsigned __int8* pixels;
			GfxStreamedPartInfo streamedParts[1];
			unsigned __int8 streamedPartCount;
			unsigned int loadedSize;
			unsigned __int8 skippedMipLevels;
			const char* name;
			unsigned int hash;
		};

		/* 3739 */
		struct SndAssetBankHeader
		{
			unsigned int magic;
			unsigned int version;
			unsigned int entrySize;
			unsigned int checksumSize;
			unsigned int dependencySize;
			unsigned int entryCount;
			unsigned int dependencyCount;
			unsigned int pad32;
			__int64 fileSize;
			__int64 entryOffset;
			__int64 checksumOffset;
			unsigned __int8 checksumChecksum[16];
			char dependencies[512];
			char padding[1464];
		};

		/* 3740 */
		struct __declspec(align(2)) SndRuntimeAssetBank
		{
			const char* zone;
			const char* language;
			int fileHandle;
			SndAssetBankHeader header;
			unsigned int entryOffset;
			unsigned __int8 linkTimeChecksum[16];
			char filename[256];
			bool indicesLoaded;
			bool indicesAllocated;
		};

		/* 3742 */
		struct SndLoadedAssets
		{
			const char* zone;
			const char* language;
			unsigned int loadedCount;
			unsigned int entryCount;
			SndAssetBankEntry* entries;
			unsigned int dataSize;
			char* data;
		};

		/* 3744 */
		struct __declspec(align(2)) SndBank
		{
			const char* name;
			unsigned int aliasCount;
			SndAliasList* alias;
			SndIndexEntry* aliasIndex;
			unsigned int radverbCount;
			SndRadverb* radverbs;
			unsigned int duckCount;
			SndDuck* ducks;
			SndRuntimeAssetBank streamAssetBank;
			SndRuntimeAssetBank loadAssetBank;
			SndLoadedAssets loadedAssets;
			unsigned int scriptIdLookupCount;
			SndDialogScriptIdLookup* scriptIdLookups;
			SndBankState state;
			int streamRequestId;
			bool pendingIo;
			bool ioError;
			bool runtimeAssetLoad;
		};

		/* 3745 */
		struct SndPatch
		{
			char* name;
			unsigned int elementCount;
			unsigned int* elements;
		};

		/* 2907 */
		struct ClipInfo
		{
			int planeCount;
			cplane_s* planes;
			unsigned int numMaterials;
			ClipMaterial* materials;
			unsigned int numBrushSides;
			cbrushside_t* brushsides;
			unsigned int leafbrushNodesCount;
			cLeafBrushNode_s* leafbrushNodes;
			unsigned int numLeafBrushes;
			unsigned __int16* leafbrushes;
			unsigned int numBrushVerts;
			vec3_t* brushVerts;
			unsigned int nuinds;
			unsigned __int16* uinds;
			unsigned __int16 numBrushes;
			cbrush_t* brushes;
			Bounds* brushBounds;
			int* brushContents;
		};

		/* 2908 */
		struct __declspec(align(4)) cLeaf_s
		{
			unsigned __int16 firstCollAabbIndex;
			unsigned __int16 collAabbCount;
			int brushContents;
			int terrainContents;
			vec3_t mins;
			vec3_t maxs;
			int leafBrushNode;
			__int16 cluster;
		};

		/* 2909 */
		struct cmodel_t
		{
			vec3_t mins;
			vec3_t maxs;
			float radius;
			ClipInfo* info;
			cLeaf_s leaf;
		};

		/* 3760 */
		struct clipMap_t
		{
			const char* name;
			int isInUse;
			ClipInfo info;
			ClipInfo* pInfo;
			unsigned int numStaticModels;
			cStaticModel_s* staticModelList;
			unsigned int numNodes;
			cNode_t* nodes;
			unsigned int numLeafs;
			cLeaf_s* leafs;
			unsigned int vertCount;
			vec3_t* verts;
			int triCount;
			short* triIndices; //unsigned __int16* triIndices;
			char* triEdgeIsWalkable; //unsigned __int8* triEdgeIsWalkable;
			int partitionCount;
			CollisionPartition* partitions;
			int aabbTreeCount;
			CollisionAabbTree* aabbTrees;
			unsigned int numSubModels;
			cmodel_t* cmodels;
			int numClusters;
			int clusterBytes;
			unsigned __int8* visibility;
			int vised;
			MapEnts* mapEnts;
			cbrush_t* box_brush;
			cmodel_t box_model;
			unsigned __int16 originalDynEntCount;
			unsigned __int16 dynEntCount[4];
			DynEntityDef* dynEntDefList[2];
			DynEntityPose* dynEntPoseList[2];
			DynEntityClient* dynEntClientList[2];
			DynEntityServer* dynEntServerList[2];
			DynEntityColl* dynEntCollList[4];
			int num_constraints;
			PhysConstraint* constraints;
			int max_ropes;
			rope_t* ropes;
			unsigned int checksum;
		};

		/* 3761 */
		struct ComPrimaryLight
		{
			unsigned __int8 type;
			unsigned __int8 canUseShadowMap;
			unsigned __int8 exponent;
			char priority;
			__int16 cullDist;
			unsigned __int8 useCookie;
			unsigned __int8 shadowmapVolume;
			float color[3];
			float dir[3];
			float origin[3];
			float radius;
			float cosHalfFovOuter;
			float cosHalfFovInner;
			float cosHalfFovExpanded;
			float rotationLimit;
			float translationLimit;
			float mipDistance;
			float dAttenuation;
			float roundness;
			vec4_t diffuseColor;
			vec4_t falloff;
			vec4_t angle;
			vec4_t aAbB;
			vec4_t cookieControl0;
			vec4_t cookieControl1;
			vec4_t cookieControl2;
			const char* defName;

			Json ToJson(bool fromT6 = true)
			{
				Json data;
				JSON_STRING(defName);

				JSON_FIELD_ARR(color, 3);
				JSON_FIELD_ARR(dir, 3);
				//JSON_FIELD_ARR(up, 3);
				JSON_FIELD_ARR(origin, 3);

				JSON_FIELD(type);
				JSON_FIELD(canUseShadowMap);
				JSON_FIELD(exponent);
				//JSON_FIELD(unused);
				JSON_FIELD(radius);
				JSON_FIELD(cosHalfFovOuter);
				JSON_FIELD(cosHalfFovInner);
				JSON_FIELD(cosHalfFovExpanded);
				JSON_FIELD(rotationLimit);
				JSON_FIELD(translationLimit);

				return data;
			}
		};

		/* 3762 */
		struct ComWorld
		{
			const char* name;
			int isInUse;
			unsigned int primaryLightCount;
			ComPrimaryLight* primaryLights;
		};

		/* 3767 */
		struct PathData
		{
			unsigned int nodeCount;
			unsigned int originalNodeCount;
			pathnode_t* nodes;
			pathbasenode_t* basenodes;
			int visBytes;
			unsigned __int8* pathVis;
			int smoothBytes;
			unsigned __int8* smoothCache;
			int nodeTreeCount;
			pathnode_tree_t* nodeTree;
		};

		/* 3768 */
		struct GameWorldSp
		{
			const char* name;
			PathData path;
		};

		/* 3769 */
		struct GameWorldMp
		{
			const char* name;
			PathData path;
		};

		/* 3750 */
		struct MapTriggers
		{
			unsigned int modelCount;
			TriggerModel* models;
			unsigned int hullCount;
			TriggerHull* hulls;
			unsigned int slabCount;
			TriggerSlab* slabs;
		};

		/* 3751 */
		struct MapEnts
		{
			const char* name;
			char* entityString;
			int numEntityChars;
			MapTriggers trigger;
		};

		/* 3771 */
		struct GfxWorldStreamInfo
		{
			int aabbTreeCount;
			GfxStreamingAabbTree* aabbTrees;
			int leafRefCount;
			int* leafRefs;
		};

		/* 3772 */
		struct GfxWorldSun
		{
			unsigned int control;
			vec3_t angles;
			vec4_t ambientColor;
			vec4_t sunCd;
			vec4_t sunCs;
			vec4_t skyColor;
			float exposure;
		};

		/* 3333 */
		struct GfxWorldFog
		{
			float baseDist;
			float halfDist;
			float baseHeight;
			float halfHeight;
			float sunFogPitch;
			float sunFogYaw;
			float sunFogInner;
			float sunFogOuter;
			vec3_t fogColor;
			float fogOpacity;
			vec3_t sunFogColor;
			float sunFogOpacity;
		};

		/* 3773 */
		struct SunLightParseParams
		{
			char name[64];
			GfxWorldSun initWorldSun[1];
			float fogTransitionTime;
			GfxWorldFog initWorldFog[1];
		};

		/* 3781 */
		struct GfxSkyDynamicIntensity
		{
			float angle0;
			float angle1;
			float factor0;
			float factor1;
		};

		/* 3782 */
		struct GfxWorldDpvsPlanes
		{
			int cellCount;
			cplane_s* planes;
			unsigned __int16* nodes;
			unsigned int* sceneEntCellBits;
		};

		/* 3792 */
		union $3BC312025CD50A0FB428B510FA0626C4
		{
			GfxReflectionProbe* localReflectionProbes;
			GfxReflectionProbe* reflectionProbes;
		};

		/* 3793 */
		union $7A514813D698CAA1912C998763BF35C8
		{
			GfxTexture* localReflectionProbeTextures;
			GfxTexture* reflectionProbeTextures;
		};

		/* 3794 */
		union $EC8AA23CFF1943ADE1BC23208707E45F
		{
			GfxLightmapArray* localLightmaps;
			GfxLightmapArray* lightmaps;
		};

		/* 3795 */
		union $C8B124FFEF69FDAD0359CC4F043058AC
		{
			GfxTexture* localLightmapPrimaryTextures;
			GfxTexture* lightmapPrimaryTextures;
		};

		/* 3796 */
		union $2DC06D0E31E15D1F20205FF26D955939
		{
			GfxTexture* localLightmapSecondaryTextures;
			GfxTexture* lightmapSecondaryTextures;
		};

		/* 3790 */
		struct GfxWorldVertexData0
		{
			unsigned __int8* data;
			char pad[4]; //ID3D11Buffer *vb;
		};

		/* 3791 */
		struct GfxWorldVertexData1
		{
			unsigned __int8* data;
			char pad[4]; //ID3D11Buffer *vb;
		};

		/* 3797 */
		struct GfxWorldDraw
		{
			unsigned int reflectionProbeCount;
			$3BC312025CD50A0FB428B510FA0626C4 ___u1;
			$7A514813D698CAA1912C998763BF35C8 ___u2;
			int lightmapCount;
			$EC8AA23CFF1943ADE1BC23208707E45F ___u4;
			$C8B124FFEF69FDAD0359CC4F043058AC ___u5;
			$2DC06D0E31E15D1F20205FF26D955939 ___u6;
			unsigned int vertexCount;
			unsigned int vertexDataSize0;
			GfxWorldVertexData0 vd0;
			unsigned int vertexDataSize1;
			GfxWorldVertexData1 vd1;
			int indexCount;
			unsigned __int16* indices;
			char pad[4]; //ID3D11Buffer *indexBuffer;
		};

		/* 3802 */
		struct GfxLightGrid
		{
			unsigned int sunPrimaryLightIndex;
			unsigned __int16 mins[3];
			unsigned __int16 maxs[3];
			float offset;
			unsigned int rowAxis;
			unsigned int colAxis;
			unsigned __int16* rowDataStart;
			unsigned int rawRowDataSize;
			unsigned __int8* rawRowData;
			unsigned int entryCount;
			GfxLightGridEntry* entries;
			unsigned int colorCount;
			GfxCompressedLightGridColors* colors;
			unsigned int coeffCount;
			GfxCompressedLightGridCoeffs* coeffs;
			unsigned int skyGridVolumeCount;
			GfxSkyGridVolume* skyGridVolumes;
		};

		/* 3806 */
		struct sunflare_t
		{
			bool hasValidData;
			Material* spriteMaterial;
			Material* flareMaterial;
			float spriteSize;
			float flareMinSize;
			float flareMinDot;
			float flareMaxSize;
			float flareMaxDot;
			float flareMaxAlpha;
			int flareFadeInTime;
			int flareFadeOutTime;
			float blindMinDot;
			float blindMaxDot;
			float blindMaxDarken;
			int blindFadeInTime;
			int blindFadeOutTime;
			float glareMinDot;
			float glareMaxDot;
			float glareMaxLighten;
			int glareFadeInTime;
			int glareFadeOutTime;
			vec3_t sunFxPosition;
		};

		/* 3835 */
		struct GfxWorldDpvsStatic
		{
			unsigned int smodelCount;
			unsigned int staticSurfaceCount;
			unsigned int litSurfsBegin;
			unsigned int litSurfsEnd;
			unsigned int litTransSurfsBegin;
			unsigned int litTransSurfsEnd;
			unsigned int emissiveOpaqueSurfsBegin;
			unsigned int emissiveOpaqueSurfsEnd;
			unsigned int emissiveTransSurfsBegin;
			unsigned int emissiveTransSurfsEnd;
			unsigned int smodelVisDataCount;
			unsigned int surfaceVisDataCount;
			unsigned __int8* smodelVisData[3];
			unsigned __int8* surfaceVisData[3];
			unsigned __int8* smodelVisDataCameraSaved;
			unsigned __int8* surfaceVisDataCameraSaved;
			unsigned __int16* sortedSurfIndex;
			GfxStaticModelInst* smodelInsts;
			GfxSurface* surfaces;
			GfxStaticModelDrawInst* smodelDrawInsts;
			GfxDrawSurf* surfaceMaterials;
			unsigned __int8* surfaceCastsSunShadow;
			unsigned __int8* surfaceCastsShadow;
			unsigned __int8* smodelCastsShadow;
			volatile int usageCount;
		};

		/* 3836 */
		struct GfxWorldDpvsDynamic
		{
			unsigned int dynEntClientWordCount[2];
			unsigned int dynEntClientCount[2];
			unsigned int* dynEntCellBits[2];
			unsigned __int8* dynEntVisData[2][3];
			volatile int usageCount;
		};

		/* 3837 */
		struct GfxWaterBuffer
		{
			unsigned int bufferSize;
			vec4_t* buffer;
		};

		/* 3842 */
		struct GfxWorld
		{
			const char* name;
			const char* baseName;
			int planeCount;
			int nodeCount;
			int surfaceCount;
			GfxWorldStreamInfo streamInfo;
			const char* skyBoxModel;
			SunLightParseParams sunParse;
			GfxLight* sunLight;
			unsigned int sunPrimaryLightIndex;
			unsigned int primaryLightCount;
			unsigned int coronaCount;
			GfxLightCorona* coronas;
			unsigned int shadowMapVolumeCount;
			GfxShadowMapVolume* shadowMapVolumes;
			unsigned int shadowMapVolumePlaneCount;
			GfxVolumePlane* shadowMapVolumePlanes;
			unsigned int exposureVolumeCount;
			GfxExposureVolume* exposureVolumes;
			unsigned int exposureVolumePlaneCount;
			GfxVolumePlane* exposureVolumePlanes;
			unsigned int worldFogVolumeCount;
			GfxWorldFogVolume* worldFogVolumes;
			unsigned int worldFogVolumePlaneCount;
			GfxVolumePlane* worldFogVolumePlanes;
			unsigned int worldFogModifierVolumeCount;
			GfxWorldFogModifierVolume* worldFogModifierVolumes;
			unsigned int worldFogModifierVolumePlaneCount;
			GfxVolumePlane* worldFogModifierVolumePlanes;
			unsigned int lutVolumeCount;
			GfxLutVolume* lutVolumes;
			unsigned int lutVolumePlaneCount;
			GfxVolumePlane* lutVolumePlanes;
			GfxSkyDynamicIntensity skyDynIntensity;
			GfxWorldDpvsPlanes dpvsPlanes;
			int cellBitsCount;
			GfxCell* cells;
			GfxWorldDraw draw;
			GfxLightGrid lightGrid;
			int modelCount;
			GfxBrushModel* models;
			vec3_t mins;
			vec3_t maxs;
			unsigned int checksum;
			int materialMemoryCount;
			MaterialMemory* materialMemory;
			sunflare_t sun;
			vec4_t outdoorLookupMatrix[4];
			GfxImage* outdoorImage;
			unsigned int* cellCasterBits;
			GfxSceneDynModel* sceneDynModel;
			GfxSceneDynBrush* sceneDynBrush;
			unsigned int* primaryLightEntityShadowVis;
			unsigned int* primaryLightDynEntShadowVis[2];
			unsigned int numSiegeSkinInsts;
			SSkinInstance* siegeSkinInsts;
			GfxShadowGeometry* shadowGeom;
			GfxLightRegion* lightRegion;
			GfxWorldDpvsStatic dpvs;
			GfxWorldDpvsDynamic dpvsDyn;
			float waterDirection;
			GfxWaterBuffer waterBuffers[2];
			const Material* waterMaterial;
			const Material* coronaMaterial;
			const Material* ropeMaterial;
			const Material* lutMaterial;
			unsigned int numOccluders;
			Occluder* occluders;
			unsigned int numOutdoorBounds;
			GfxOutdoorBounds* outdoorBounds;
			unsigned int heroLightCount;
			unsigned int heroLightTreeCount;
			GfxHeroLight* heroLights;
			GfxHeroLightTree* heroLightTree;
			unsigned int lightingFlags;
			int lightingQuality;
		};

		/* 2799 */
		struct __declspec(align(4)) GfxLightImage
		{
			GfxImage* image;
			unsigned __int8 samplerState;
		};

		/* 2800 */
		struct GfxLightDef
		{
			const char* name;
			GfxLightImage attenuation;
			int lmapLookupStart;
		};

		/* 1996 */
		struct Glyph
		{
			unsigned __int16 letter;
			char x0;
			char y0;
			unsigned __int8 dx;
			unsigned __int8 pixelWidth;
			unsigned __int8 pixelHeight;
			float s0;
			float t0;
			float s1;
			float t1;
		};

		/* 1997 */
		struct KerningPairs
		{
			unsigned __int16 wFirst;
			unsigned __int16 wSecond;
			int iKernAmount;
		};

		/* 1998 */
		struct Font_s
		{
			const char* fontName;
			int pixelHeight;
			int isScalingAllowed;
			int glyphCount;
			int kerningPairsCount;
			Material* material;
			Material* glowMaterial;
			Glyph* glyphs;
			KerningPairs* kerningPairs;
		};

		/* 3843 */
		struct FontIconName
		{
			const char* string;
			int hash;
		};

		/* 3844 */
		struct FontIconEntry
		{
			FontIconName fontIconName;
			Material* fontIconMaterialHandle;
			int fontIconSize;
			float xScale;
			float yScale;
		};

		/* 3845 */
		struct FontIconAlias
		{
			int aliasHash;
			int buttonHash;
		};

		/* 3846 */
		struct FontIcon
		{
			const char* name;
			int numEntries;
			int numAliasEntries;
			FontIconEntry* fontIconEntry;
			FontIconAlias* fontIconAlias;
		};

		/* 3847 */
		struct MenuList
		{
			const char* name;
			int menuCount;
			menuDef_t** menus;
		};

		/* 1999 */
		struct rectDef_s
		{
			float x;
			float y;
			float w;
			float h;
			int horzAlign;
			int vertAlign;
		};

		/* 2323 */
		struct windowDef_t
		{
			const char* name;
			rectDef_s rect;
			rectDef_s rectClient;
			const char* group;
			unsigned __int8 style;
			unsigned __int8 border;
			unsigned __int8 modal;
			unsigned __int8 frameSides;
			float frameTexSize;
			float frameSize;
			int ownerDraw;
			int ownerDrawFlags;
			float borderSize;
			int staticFlags;
			int dynamicFlags[1];
			int nextTime;
			vec4_t foreColor;
			vec4_t backColor;
			vec4_t borderColor;
			vec4_t outlineColor;
			float rotation;
			Material* background;
		};

		/* 2151 */
		struct ExpressionStatement
		{
			char* filename;
			int line;
			int numRpn;
			expressionRpn* rpn;
		};

		/* 2339 */
		struct __declspec(align(8)) menuDef_t
		{
			windowDef_t window;
			const char* font;
			int fullScreen;
			int ui3dWindowId;
			int itemCount;
			int fontIndex;
			int cursorItem[1];
			int fadeCycle;
			int priority;
			float fadeClamp;
			float fadeAmount;
			float fadeInAmount;
			float blurRadius;
			int openSlideSpeed;
			int closeSlideSpeed;
			int openSlideDirection;
			int closeSlideDirection;
			rectDef_s initialRectInfo;
			int openFadingTime;
			int closeFadingTime;
			int fadeTimeCounter;
			int slideTimeCounter;
			GenericEventHandler* onEvent;
			ItemKeyHandler* onKey;
			ExpressionStatement visibleExp;
			unsigned __int64 showBits;
			unsigned __int64 hideBits;
			const char* allowedBinding;
			const char* soundName;
			int imageTrack;
			int control;
			vec4_t focusColor;
			vec4_t disableColor;
			ExpressionStatement rectXExp;
			ExpressionStatement rectYExp;
			itemDef_s** items;
		};

		/* 3848 */
		struct LocalizeEntry
		{
			const char* value;
			const char* name;
		};

		/* 2818 */
		struct WeaponVariantDef
		{
			const char* szInternalName;
			int iVariantCount;
			WeaponDef* weapDef;
			const char* szDisplayName;
			const char* szAltWeaponName;
			const char* szAttachmentUnique;
			WeaponAttachment* attachments; //WeaponAttachment** attachments;
			WeaponAttachmentUnique* attachmentUniques; //WeaponAttachmentUnique** attachmentUniques;
			const char** szXAnims;
			unsigned __int16* hideTags;
			XModel* attachViewModel; //XModel** attachViewModel;
			XModel* attachWorldModel; //XModel** attachWorldModel;
			const char* attachViewModelTag; //const char** attachViewModelTag;
			const char* attachWorldModelTag; //const char** attachWorldModelTag;
			float attachViewModelOffsets[24];
			float attachWorldModelOffsets[24];
			float attachViewModelRotations[24];
			float attachWorldModelRotations[24];
			float stowedModelOffsets[3];
			float stowedModelRotations[3];
			unsigned int altWeaponIndex;
			int iAttachments;
			bool bIgnoreAttachments;
			int iClipSize;
			int iReloadTime;
			int iReloadEmptyTime;
			int iReloadQuickTime;
			int iReloadQuickEmptyTime;
			int iAdsTransInTime;
			int iAdsTransOutTime;
			int iAltRaiseTime;
			const char* szAmmoDisplayName;
			const char* szAmmoName;
			int iAmmoIndex;
			const char* szClipName;
			int iClipIndex;
			float fAimAssistRangeAds;
			float fAdsSwayHorizScale;
			float fAdsSwayVertScale;
			float fAdsViewKickCenterSpeed;
			float fHipViewKickCenterSpeed;
			float fAdsZoomFov1;
			float fAdsZoomFov2;
			float fAdsZoomFov3;
			float fAdsZoomInFrac;
			float fAdsZoomOutFrac;
			float fOverlayAlphaScale;
			float fOOPosAnimLength[2];
			bool bSilenced;
			bool bDualMag;
			bool bInfraRed;
			bool bTVGuided;
			unsigned int perks[2];
			bool bAntiQuickScope;
			Material* overlayMaterial;
			Material* overlayMaterialLowRes;
			Material* dpadIcon;
			weaponIconRatioType_t dpadIconRatio;
			bool noAmmoOnDpadIcon;
			bool mmsWeapon;
			bool mmsInScope;
			float mmsFOV;
			float mmsAspect;
			float mmsMaxDist;
			//float ikLeftHandIdlePos[3];
			float ikLeftHandIdlePosForward;
			float ikLeftHandIdlePosRight;
			float ikLeftHandIdlePosUp;
			//float ikLeftHandOffset[3];
			float ikLeftHandOffsetForward;
			float ikLeftHandOffsetRight;
			float ikLeftHandOffsetUp;
			//float ikLeftHandRotation[3];
			float ikLeftHandRotationPitch;
			float ikLeftHandRotationYaw;
			float ikLeftHandRotationRoll;
			bool bUsingLeftHandProneIK;
			//float ikLeftHandProneOffset[3];
			float ikLeftHandProneOffsetForward;
			float ikLeftHandProneOffsetRight;
			float ikLeftHandProneOffsetUp;
			//float ikLeftHandProneRotation[3];
			float ikLeftHandProneRotationPitch;
			float ikLeftHandProneRotationYaw;
			float ikLeftHandProneRotationRoll;
			//float ikLeftHandUiViewerOffset[3];
			float ikLeftHandUiViewerOffsetForward;
			float ikLeftHandUiViewerOffsetRight;
			float ikLeftHandUiViewerOffsetUp;
			//float ikLeftHandUiViewerRotation[3];
			float ikLeftHandUiViewerRotationPitch;
			float ikLeftHandUiViewerRotationYaw;
			float ikLeftHandUiViewerRotationRoll;
		};

		/* 2577 */
		struct WeaponAttachment
		{
			const char* szInternalName;
			const char* szDisplayName;
			eAttachment attachmentType;
			eAttachmentPoint attachmentPoint;
			PenetrateType penetrateType;
			weapFireType_t fireType;
			int firstRaisePriority;
			float fHipIdleAmount;
			bool bAltWeaponAdsOnly;
			bool bAltWeaponDisableSwitching;
			float altScopeADSTransInTime;
			float altScopeADSTransOutTime;
			bool bSilenced;
			bool bDualMag;
			bool laserSight;
			bool bInfraRed;
			bool bUseAsMelee;
			bool bDualWield;
			bool sharedAmmo;
			float fDamageRangeScale;
			float fAdsZoomFov1;
			float fAdsZoomFov2;
			float fAdsZoomFov3;
			float fAdsZoomInFrac;
			float fAdsZoomOutFrac;
			float fAdsTransInTimeScale;
			float fAdsTransOutTimeScale;
			float fAdsRecoilReductionRate;
			float fAdsRecoilReductionLimit;
			float fAdsViewKickCenterSpeedScale;
			float fAdsIdleAmountScale;
			bool swayOverride;
			float swayMaxAngle;
			float swayLerpSpeed;
			float swayPitchScale;
			float swayYawScale;
			float swayHorizScale;
			float swayVertScale;
			bool adsSwayOverride;
			float adsSwayMaxAngle;
			float adsSwayLerpSpeed;
			float adsSwayPitchScale;
			float adsSwayYawScale;
			float fAdsSwayHorizScale;
			float fAdsSwayVertScale;
			float adsMoveSpeedScale;
			float fHipSpreadMinScale;
			float fHipSpreadMaxScale;
			float strafeRotR;
			float standMoveF;
			vec3_t vStandRot;
			float fFireTimeScale;
			float fReloadTimeScale;
			float fReloadEmptyTimeScale;
			float fReloadAddTimeScale;
			float fReloadQuickTimeScale;
			float fReloadQuickEmptyTimeScale;
			float fReloadQuickAddTimeScale;
			bool mmsWeapon;
			bool mmsInScope;
			float mmsFOV;
			float mmsAspect;
			float mmsMaxDist;
			float clipSizeScale;
			int iClipSize;
			unsigned int stackFire;
			float stackFireSpread;
			float stackFireAccuracyDecay;
			unsigned int perks[2];
			float customFloat0;
			float customFloat1;
			float customFloat2;
			int customBool0;
			int customBool1;
			int customBool2;
		};

		/* 2817 */
		struct WeaponAttachmentUnique
		{
			const char* szInternalName;
			eAttachment attachmentType;
			int siblingLink;
			int childLink;
			int combinedAttachmentTypeMask;
			const char* szAltWeaponName;
			unsigned int altWeaponIndex;
			const char* szDualWieldWeaponName;
			unsigned int dualWieldWeaponIndex;
			unsigned __int16* hideTags;
			XModel* viewModel;
			XModel* viewModelAdditional;
			XModel* viewModelADS;
			XModel* worldModel;
			XModel* worldModelAdditional;
			const char* viewModelTag;
			const char* worldModelTag;
			vec3_t viewModelOffsets;
			vec3_t worldModelOffsets;
			vec3_t viewModelRotations;
			vec3_t worldModelRotations;
			vec3_t viewModelAddOffsets;
			vec3_t worldModelAddOffsets;
			vec3_t viewModelAddRotations;
			vec3_t worldModelAddRotations;
			WeaponCamo* weaponCamo;
			bool disableBaseWeaponAttachment;
			bool disableBaseWeaponClip;
			bool overrideBaseWeaponAttachmentOffsets;
			vec3_t viewModelOffsetBaseAttachment;
			vec3_t worldModelOffsetBaseAttachment;
			Material* overlayMaterial;
			Material* overlayMaterialLowRes;
			weapOverlayReticle_t overlayReticle;
			int iFirstRaiseTime;
			int iAltRaiseTime;
			int iAltDropTime;
			int iReloadAmmoAdd;
			int iReloadStartAdd;
			bool bSegmentedReload;
			const char** szXAnims;
			int animationOverrides[3];
			float* locationDamageMultipliers;
			int soundOverrides;
			const char* fireSound;
			const char* fireSoundPlayer;
			const char* fireLoopSound;
			const char* fireLoopSoundPlayer;
			const char* fireLoopEndSound;
			const char* fireLoopEndSoundPlayer;
			const char* fireStartSound;
			const char* fireStopSound;
			const char* fireStartSoundPlayer;
			const char* fireStopSoundPlayer;
			const char* fireLastSound;
			const char* fireLastSoundPlayer;
			const char* fireKillcamSound;
			const char* fireKillcamSoundPlayer;
			int effectOverrides;
			const FxEffectDef* viewFlashEffect;
			const FxEffectDef* worldFlashEffect;
			TracerDef* tracerType;
			TracerDef* enemyTracerType;
			float adsDofStart;
			float adsDofEnd;
			int iAmmoIndex;
			int iClipIndex;
			bool bOverrideLeftHandIK;
			bool bOverrideLeftHandProneIK;
			vec3_t ikLeftHandOffset;
			vec3_t ikLeftHandRotation;
			vec3_t ikLeftHandProneOffset;
			vec3_t ikLeftHandProneRotation;
			float customFloat0;
			float customFloat1;
			float customFloat2;
			int customBool0;
			int customBool1;
			int customBool2;
		};

		/* 2815 */
		struct WeaponCamo
		{
			const char* name;
			GfxImage* solidBaseImage;
			GfxImage* patternBaseImage;
			WeaponCamoSet* camoSets;
			unsigned int numCamoSets;
			WeaponCamoMaterialSet* camoMaterials;
			unsigned int numCamoMaterials;
		};

		/* 3856 */
		struct SndDriverGlobals
		{
			const char* name;
			unsigned int groupCount;
			SndVolumeGroup* groups;
			unsigned int curveCount;
			SndCurve* curves;
			unsigned int panCount;
			SndPan* pans;
			unsigned int duckGroupCount;
			SndDuckGroup* duckGroups;
			unsigned int contextCount;
			SndContext* contexts;
			unsigned int masterCount;
			SndMaster* masters;
			unsigned int voiceDuckCount;
			SndSidechainDuck* voiceDucks;
			unsigned int futzCount;
			SndFutz* futzes;
		};

		/* 2813 */
		union $B23106E4967054A4E77C4B3E322525DE
		{
			const FxElemDef* elemDefs;
			const FxElemDef* localElemDefs;
		};

		/* 2789 */
		struct FxFloatRange
		{
			float base;
			float amplitude;
		};

		/* 2797 */
		const struct FxEffectDef
		{
			const char* name;
			unsigned __int16 flags;
			unsigned __int8 efPriority;
			__int16 elemDefCountLooping;
			__int16 elemDefCountOneShot;
			__int16 elemDefCountEmission;
			int totalSize;
			int msecLoopingLife;
			int msecNonLoopingLife;
			$B23106E4967054A4E77C4B3E322525DE ___u9;
			vec3_t boundingBoxDim;
			vec3_t boundingBoxCentre;
			float occlusionQueryDepthBias;
			int occlusionQueryFadeIn;
			int occlusionQueryFadeOut;
			FxFloatRange occlusionQueryScaleRange;
		};

		/* 3735 */
		struct FxImpactTable
		{
			const char* name;
			FxImpactEntry* table;
		};

		/* 3857 */
		struct RawFile
		{
			const char* name;
			int len;
			const char* buffer;
		};

		/* 3734 */
		struct StringTable
		{
			const char* name;
			int columns;
			int rows;
			StringTableCell* strings;
			__int16* cellIndex;
		};

		/* 3858 */
		struct LbColumnDef
		{
			const char* name;
			int colId;
			int dwColIndex;
			bool hidden;
			const char* statName;
			LbColType type;
			int precision;
			LbAggType agg;
			const char* localization;
			int uiCalColX;
			int uiCalColY;
			Json ToJson()
			{
				Json data;

				JSON_STRING(name);
				JSON_STRING(statName);

				JSON_FIELD(colId);
				JSON_FIELD(dwColIndex);
				JSON_FIELD(hidden);
				JSON_FIELD(type);
				JSON_FIELD(precision);
				JSON_FIELD(agg);
				JSON_FIELD(localization);
				JSON_FIELD(uiCalColX);
				JSON_FIELD(uiCalColY);

				return data;
			}
		};

		/* 3859 */
		struct LeaderboardDef
		{
			const char* name;
			unsigned int id;
			int columnCount;
			int dwColumnCount;
			int xpColId;
			int prestigeColId;
			LbColumnDef* columns;
			LbUpdateType updateType;
			int trackTypes;
		};

		/* 3860 */
		struct gump_info_t
		{
			char* name;
			int size;
		};

		/* 3861 */
		struct overlay_info_t
		{
			char* name;
			int size;
		};

		/* 3862 */
		struct XGlobals
		{
			const char* name;
			int xanimStreamBufferSize;
			int cinematicMaxWidth;
			int cinematicMaxHeight;
			int extracamResolution;
			vec4_t screenClearColor;
			int bigestGumpSize;
			int gumpsCount;
			gump_info_t gumps[32];
			int bigestOverlaySize;
			int overlayCount;
			overlay_info_t overlays[32];
		};

		/* 3506 */
		struct ddlMemberDef_t
		{
			const char* name;
			int size;
			int offset;
			int type;
			int externalIndex;
			unsigned int rangeLimit;
			unsigned int serverDelta;
			unsigned int clientDelta;
			int arraySize;
			int enumIndex;
			int permission;
		};

		/* 3430 */
		struct ddlHash_t
		{
			int hash;
			int index;
		};

		/* 3507 */
		struct ddlStructDef_t
		{
			const char* name;
			int size;
			int memberCount;
			ddlMemberDef_t* members;
			ddlHash_t* hashTable;
		};

		/* 3431 */
		struct ddlEnumDef_t
		{
			const char* name;
			int memberCount;
			const char** members;
			ddlHash_t* hashTable;
		};

		/* 3508 */
		struct ddlDef_t
		{
			int version;
			int size;
			ddlStructDef_t* structList;
			int structCount;
			ddlEnumDef_t* enumList;
			int enumCount;
			ddlDef_t* next;
		};

		/* 3733 */
		struct ddlRoot_t
		{
			const char* name;
			ddlDef_t* ddlDef;
		};

		/* 3863 */
		struct Glasses
		{
			const char* name;
			unsigned int numGlasses;
			Glass* glasses;
			unsigned __int8* workMemory;
			unsigned int workMemorySize;
			unsigned int smallAllocatorBlocks;
			unsigned int maxGroups;
			unsigned int maxShards;
			unsigned int maxPhysics;
			unsigned int shardMemorySize;
			unsigned int maxFreeCmd;
			unsigned int numSlots;
			unsigned int numVerts;
			unsigned int numIndices;
		};

		/* 3864 */
		struct EmblemLayer
		{
			int cost;
			int unlockLevel;
			int unlockPLevel;
		};

		/* 3865 */
		struct EmblemCategory
		{
			const char* name;
			const char* description;
		};

		/* 3866 */
		struct EmblemIconType
		{
			const char* name;
			const char* description;
		};

		/* 3867 */
		struct EmblemBGCategory
		{
			const char* name;
			const char* description;
		};

		/* 3868 */
		struct EmblemIcon
		{
			GfxImage* image;
			const char* description;
			float outlineSize;
			int cost;
			int unlockLevel;
			int unlockPLevel;
			int unclassifyAt;
			int sortKey;
			unsigned __int16 iconType;
			unsigned __int16 category;
		};

		/* 3869 */
		struct EmblemBackground
		{
			Material* material;
			const char* description;
			int cost;
			int unlockLevel;
			int unlockPLevel;
			int unclassifyAt;
			int sortKey;
			unsigned __int16 bgCategory;
			int mtxIndex;
		};

		/* 3870 */
		struct EmblemSet
		{
			int colorCount;
			int layerCount;
			EmblemLayer layers[32];
			int categoryCount;
			EmblemCategory categories[16];
			int iconTypeCount;
			EmblemIconType iconTypes[16];
			int bgCategoryCount;
			EmblemBGCategory bgCategories[16];
			int iconCount;
			EmblemIcon icons[1024];
			int backgroundCount;
			EmblemBackground backgrounds[550];
			int backgroundLookupCount;
			__int16 backgroundLookup[550];
		};

		/* 3871 */
		struct ScriptParseTree
		{
			const char* name;
			int len;
			unsigned __int8* buffer;
		};

		/* 3873 */
		struct KeyValuePairs
		{
			const char* name;
			int numVariables;
			KeyValuePair* keyValuePairs;
		};

		/* 3130 */
		struct view_limits_t
		{
			float horizSpanLeft;
			float horizSpanRight;
			float vertSpanUp;
			float vertSpanDown;
			float horizResistLeft;
			float horizResistRight;
			float vertResistUp;
			float vertResistDown;
		};

		/* 2983 */
		struct VehicleParameter
		{
			float m_speed_max;
			float m_accel_max;
			float m_reverse_scale;
			float m_steer_angle_max;
			float m_steer_angle_speed_scale;
			float m_steer_speed;
			float m_wheel_radius;
			float m_susp_spring_k;
			float m_susp_damp_k;
			float m_susp_adj;
			float m_susp_hard_limit;
			float m_susp_min_height;
			float m_tire_fric_fwd;
			float m_tire_fric_side;
			float m_tire_fric_brake;
			float m_tire_fric_hand_brake;
			float m_body_mass;
			float m_roll_stability;
			float m_pitch_stability;
			float m_pitch_roll_resistance;
			float m_yaw_resistance;
			float m_upright_strength;
			float m_tilt_fakey;
			float m_peel_out_max_speed;
			float m_tire_damp_coast;
			float m_tire_damp_brake;
			float m_tire_damp_hand;
			float m_auto_hand_brake_min_speed;
			TractionType m_traction_type;
			const char* m_name;
			float m_bbox_min[3];
			float m_bbox_max[3];
			float m_mass_center_offset[3];
			float m_buoyancybox_min[3];
			float m_buoyancybox_max[3];
			float m_water_speed_max;
			float m_water_accel_max;
			float m_water_turn_accel;
			float m_water_turn_speed_max;
			float m_water_ebrake_power;
			float m_boat_motor_offset[3];
			float m_boat_speed_rise;
			float m_boat_speed_tilt;
			float m_boat_side_fric_scale;
			float m_boat_forward_fric_scale;
			float m_boat_vertical_fric_scale;
			float m_jump_force;
			float m_tire_fric_side_max;
			bool m_drive_on_walls;
			float m_linear_drag_scale;
			float m_angular_drag_scale;
		};

		/* 3133 */
		struct VehicleDriveBySound
		{
			int apex;
			const char* name;
			unsigned int alias;
		};

		/* 3134 */
		struct VehicleEngineSound
		{
			const char* name;
			unsigned int alias;
			float params[5];
		};

		/* 3135 */
		struct VehicleGearData
		{
			float minRPM;
			float maxRPM;
			float ratio;
		};

		/* 3136 */
		struct VehicleEngine
		{
			float idleRpms;
			float maxRpms;
			float maxTorque;
			float brakingCoeff;
			float loadFadeParams[4];
			float loadScale;
			float loadSmoothing;
			float throttleLag;
			float pitchScale;
			VehicleEngineSound onload[5];
			VehicleEngineSound offload[5];
			int numGears;
			int loopLastGear;
			VehicleGearData gears[10];
		};

		/* 3137 */
		struct VehicleAntenna
		{
			float springK;
			float damp;
			float length;
			float gravity;
		};

		struct cspField_t
		{
			const char* szName;
			int iOffset;
			int iFieldType;
		};

		/* 3138 */
		const struct VehicleDef
		{
			const char* name;
			__int16 type;
			int remoteControl;
			int bulletDamage;
			int armorPiercingDamage;
			int grenadeDamage;
			int projectileDamage;
			int projectileSplashDamage;
			int heavyExplosiveDamage;
			__int16 cameraMode;
			int autoRecenterOnAccel;
			int thirdPersonDriver;
			int thirdPersonUseVehicleRoll;
			int thirdPersonCameraPitchVehicleRelative;
			int thirdPersonCameraHeightWorldRelative;
			float thirdPersonCameraRange;
			float thirdPersonCameraMinPitchClamp;
			float thirdPersonCameraMaxPitchClamp;
			float thirdPersonCameraHeight[2];
			float thirdPersonCameraPitch[2];
			int cameraAlwaysAutoCenter;
			float cameraAutoCenterLerpRate;
			float cameraAutoCenterMaxLerpRate;
			float thirdPersonCameraSpringDistance;
			float thirdPersonCameraSpringTime;
			float thirdPersonCameraHandbrakeTurnRateInc;
			float cameraFOV;
			float cameraRollFraction;
			float tagPlayerOffset[3];
			int killcamCollision;
			float killcamDist;
			float killcamZDist;
			float killcamMinDist;
			float killcamZTargetOffset;
			float killcamFOV;
			float killcamNearBlur;
			float killcamNearBlurStart;
			float killcamNearBlurEnd;
			float killcamFarBlur;
			float killcamFarBlurStart;
			float killcamFarBlurEnd;
			int isDrivable;
			int numberOfSeats;
			int numberOfGunners;
			int seatSwitchOrder[11];
			int driverControlledGunPos;
			float entryPointRadius[5];
			float texScrollScale;
			float wheelRotRate;
			float extraWheelRotScale;
			int wheelChildTakesSteerYaw;
			float maxSpeed;
			float maxSpeedVertical;
			float accel;
			float accelVertical;
			float rotRate;
			float rotAccel;
			float maxBodyPitch;
			float maxBodyRoll;
			float collisionDamage;
			float collisionSpeed;
			float suspensionTravel;
			float heliCollisionScalar;
			float viewPitchOffset;
			float viewInfluence;
			float tiltFromAcceleration[2];
			float tiltFromDeceleration[2];
			float tiltFromVelocity[2];
			float tiltSpeed[2];
			const char* turretWeapon;
			view_limits_t turretViewLimits;
			float turretRotRate;
			int turretClampPlayerView;
			int turretLockTurretToPlayerView;
			const char* gunnerWeapon[4];
			unsigned __int16 gunnerWeaponIndex[4];
			float gunnerRotRate;
			float gunnerRestAngles[4][2];
			view_limits_t passengerViewLimits[6];
			const char* sndNames[2];
			unsigned int sndIndices[2];
			const char* sndMaterialNames[3];
			float skidSpeedMin;
			float skidSpeedMax;
			const char* futzName;
			float futzBlend;
			int animType;
			const char* animSet;
			int scriptedAnimationEntry;
			float mantleAngles[4];
			unsigned __int16 extraWheelTags[4];
			unsigned __int16 driverHideTag;
			XModel* attachmentModels[4];
			unsigned __int16 attachmentTags[4];
			XModel* deathAttachmentModels[4];
			unsigned __int16 deathAttachmentTags[4];
			float tracerOffset[2];
			XModel* model;
			XModel* viewModel;
			XModel* deathModel;
			XModel* enemyModel;
			float modelSwapDelay;
			const FxEffectDef* exhaustFx;
			int oneExhaust;
			const FxEffectDef* treadFx[32];
			const FxEffectDef* deathFx;
			unsigned __int16 deathFxTag;
			const char* deathFxSound;
			const FxEffectDef* lightFx[4];
			unsigned __int16 lightFxTag[4];
			const FxEffectDef* friendlyLightFx;
			unsigned __int16 friendlyLightFxTag;
			const FxEffectDef* enemyLightFx;
			unsigned __int16 enemyLightFxTag;
			float radiusDamageMin;
			float radiusDamageMax;
			float radiusDamageRadius;
			const char* shootShock;
			const char* shootRumble;
			float deathQuakeScale;
			float deathQuakeDuration;
			float deathQuakeRadius;
			const char* rumbleType;
			float rumbleScale;
			float rumbleDuration;
			float rumbleRadius;
			float rumbleBaseTime;
			float rumbleAdditionalTime;
			int healthDefault;
			int healthMin;
			int healthMax;
			int eTeam;
			int boostAccelMultiplier;
			float boostDuration;
			float boostSpeedIncrease;
			int addToCompass;
			int addToCompassEnemy;
			const char* compassIcon;
			Material* compassIconMaterial;
			const char* gasButtonName;
			int gasButton;
			const char* reverseBrakeButtonName;
			int reverseBrakeButton;
			const char* handBrakeButtonName;
			int handBrakeButton;
			const char* attackButtonName;
			int attackButton;
			const char* attackSecondaryButtonName;
			int attackSecondaryButton;
			const char* boostButtonName;
			int boostButton;
			const char* moveUpButtonName;
			int moveUpButton;
			const char* moveDownButtonName;
			int moveDownButton;
			const char* switchSeatButtonName;
			int switchSeatButton;
			const char* steerGraphName; //4A0
			GraphFloat* steerGraph;
			const char* accelGraphName;
			GraphFloat* accelGraph;
			int isNitrous;
			int isFourWheelSteering;
			int useCollmap;
			float radius;
			float minHeight;
			float maxHeight;
			float max_fric_tilt_angle;
			float max_fric_tilt;
			int noDirectionalDamage;
			int fakeBodyStabilizer;
			float vehHelicopterBoundsRadius;
			float vehHelicopterDecelerationFwd;
			float vehHelicopterDecelerationSide;
			float vehHelicopterDecelerationUp;
			float vehHelicopterTiltFromControllerAxes;
			float vehHelicopterTiltFromFwdAndYaw;
			float vehHelicopterTiltFromFwdAndYaw_VelAtMaxTilt;
			float vehHelicopterTiltMomentum;
			int vehHelicopterQuadRotor;
			int vehHelicopterAccelTwardsView;
			float maxRotorArmMovementAngle;
			float maxRotorArmRotationAngle;
			int vehHelicopterMaintainHeight;
			int vehHelicopterMaintainMaxHeight;
			float vehHelicopterMaintainHeightLimit;
			float vehHelicopterMaintainHeightAccel;
			float vehHelicopterMaintainHeightMinimum;
			float vehHelicopterMaintainHeightMaximum;
			float vehHelicopterMaintainCeilingMinimum;
			int joltVehicle;
			int joltVehicleDriver;
			float joltMaxTime;
			float joltTime;
			float joltWaves;
			float joltIntensity;
			VehicleParameter nitrousVehParams;
			float driveBySoundRadius[2];
			VehicleDriveBySound driveBySounds[40];
			int doFootSteps;
			int isSentient;
			VehicleEngine engine;
			VehicleAntenna antenna[2];
			char* csvInclude;
			float customFloat0;
			float customFloat1;
			float customFloat2;
			int customBool0;
			int customBool1;
			int customBool2;
		};

		/* 3874 */
		struct MemoryBlock
		{
			const char* name;
			bool mainMem;
			bool videoMem;
			bool streamMem;
			unsigned int size;
			unsigned int alignment;
			unsigned __int8* data;
		};

		/* 3875 */
		struct AddonMapEnts
		{
			const char* name;
			char* entityString;
			int numEntityChars;
			MapTriggers trigger;
			ClipInfo* info;
			unsigned int numSubModels;
			cmodel_t* cmodels;
			GfxBrushModel* models;
		};

		/* 2511 */
		struct TracerDef
		{
			const char* name;
			tracerType_t type;
			Material* material;
			unsigned int drawInterval;
			float speed;
			float beamLength;
			float beamWidth;
			float screwRadius;
			float screwDist;
			float fadeTime;
			float fadeScale;
			float texRepeatRate;
			float colors[5][4];
		};

		/* 3876 */
		struct SkinnedVertsDef
		{
			const char* name;
			unsigned int maxSkinnedVerts;
		};

		/* 3877 */
		struct Qdb
		{
			const char* name;
			int len;
			unsigned __int8* buffer;
		};

		/* 3878 */
		struct Slug
		{
			const char* name;
			int len;
			unsigned __int8* buffer;
		};

		/* 3879 */
		struct FootstepTableDef
		{
			const char* name;
			unsigned int sndAliasTable[32][7];
		};

		/* 3880 */
		struct FootstepFXTableDef
		{
			const char* name;
			const FxEffectDef* footstepFX[32];
		};

		/* 3272 */
		struct ZBarrierBoard
		{
			XModel* pBoardModel;
			XModel* pAlternateBoardModel;
			XModel* pUpgradedBoardModel;
			const char* pTearAnim;
			const char* pBoardAnim;
			const FxEffectDef* repairEffect1;
			const FxEffectDef* repairEffect2;
			float repairEffect1Offset[3];
			float repairEffect2Offset[3];
			unsigned int boardRepairSound;
			unsigned int boardRepairHoverSound;
			unsigned int pauseAndRepeatRepSound;
			float minPause;
			float maxPause;
			unsigned __int16 zombieBoardTearStateName;
			unsigned __int16 zombieBoardTearSubStateName;
			unsigned int numRepsToPullProBoard;
		};

		/* 3273 */
		struct ZBarrierDef
		{
			const char* name;
			unsigned int generalRepairSound1;
			unsigned int generalRepairSound2;
			unsigned int upgradedGeneralRepairSound1;
			unsigned int upgradedGeneralRepairSound2;
			unsigned int delayBetweenRepSounds;
			float delayBetweenRepSoundsDuration;
			unsigned int earthquakeOnRepair;
			float earthquakeMinScale;
			float earthquakeMaxScale;
			float earthquakeMinDuration;
			float earthquakeMaxDuration;
			float earthquakeRadius;
			int numBoardsInBarrier;
			unsigned int autoHideOpenPieces;
			unsigned int taunts;
			unsigned int reachThroughAttacks;
			unsigned __int16 zombieTauntAnimState;
			unsigned __int16 zombieReachThroughAnimState;
			int numAttackSlots;
			float attackSpotHorzOffset;
			ZBarrierBoard boards[6];
		};

		/* 3484 */
		struct XModelPiece
		{
			XModel* model;
			vec3_t offset;
		};

		/* 3062 */
		struct DestructibleStage
		{
			unsigned __int16 showBone;
			float breakHealth;
			float maxTime;
			unsigned int flags;
			const FxEffectDef* breakEffect;
			const char* breakSound;
			const char* breakNotify;
			const char* loopSound;
			XModel* spawnModel[3];
			PhysPreset* physPreset;
		};

		/* 3063 */
		struct DestructiblePiece
		{
			DestructibleStage stages[5];
			unsigned __int8 parentPiece;
			float parentDamagePercent;
			float bulletDamageScale;
			float explosiveDamageScale;
			float meleeDamageScale;
			float impactDamageScale;
			float entityDamageTransfer;
			PhysConstraints* physConstraints;
			int health;
			const char* damageSound;
			const FxEffectDef* burnEffect;
			const char* burnSound;
			unsigned __int16 enableLabel;
			int hideBones[5];
		};

		/* 3169 */
		struct XAnimNotifyInfo
		{
			unsigned __int16 name;
			float time;
		};

		/* 3181 */
		struct XAnimDeltaPart
		{
			XAnimPartTrans* trans;
			XAnimDeltaPartQuat2* quat2;
			XAnimDeltaPartQuat* quat;
		};

		/* 2664 */
		struct DObjAnimMat
		{
			vec4_t quat;
			vec3_t trans;
			float transWeight;
		};

		/* 2665 */
		struct XSurfaceVertexInfo
		{
			__int16 vertCount[4];
			unsigned __int16* vertsBlend;
			float* tensionData;
		};

		/* 2675 */
		union $AE6C164B5DE82B33A09A25BB561EDF66
		{
			XRigidVertList* vertList;
			XRigidVertList* vertListEA;
		};

		/* 2676 */
		struct __declspec(align(16)) XSurface
		{
			unsigned __int8 tileMode;
			unsigned __int8 vertListCount;
			unsigned __int16 flags;
			unsigned __int16 vertCount;
			unsigned __int16 triCount;
			unsigned __int16 baseVertIndex;
			unsigned __int16* triIndices;
			XSurfaceVertexInfo vertInfo;
			GfxPackedVertex* verts0;
			ID3D11Buffer* vb0;
			XRigidVertList* vertList;
			ID3D11Buffer* indexBuffer;
			int partBits[5];
		};

		/* 2173 */
		struct XModelCollSurf_s
		{
			XModelCollTri_s* tris;
			int numCollTris;
			vec3_t mins;
			vec3_t maxs;
			int boneIdx;
			int contents;
			int surfFlags;
		};

		/* 2197 */
		struct __declspec(align(4)) XBoneInfo
		{
			vec3_t bounds[2];
			vec3_t offset;
			float radiusSquared;
			unsigned __int8 collmap;
		};

		/* 2488 */
		struct Collmap
		{
			unsigned int count;
			PhysGeomInfo* geoms;
			int contents;
		};

		/* 1978 */
		struct MaterialTextureDef
		{
			unsigned int nameHash;
			char nameStart;
			char nameEnd;
			unsigned __int8 samplerState;
			unsigned __int8 semantic;
			unsigned __int8 isMatureContent;
			unsigned __int8 pad[3];
			GfxImage* image;
		};

		/* 1979 */
		struct MaterialConstantDef
		{
			unsigned int nameHash;
			char name[12];
			vec4_t literal;
		};

		/* 1980 */
		struct GfxStateBits
		{
			unsigned int loadBits[2];
			ID3D11BlendState *blendState;
			ID3D11DepthStencilState *depthStencilState;
			ID3D11RasterizerState *rasterizerState;
		};

		/* 1967 */
		union $1DE63AEDF7054C077EFD704A704D8955
		{
			MaterialPixelShader* pixelShader;
			MaterialPixelShader* localPixelShader;
		};

		/* 1968 */
		union $029AC51ADF28C243FF41D15E843FFF95
		{
			MaterialShaderArgument* localArgs;
			MaterialShaderArgument* args;
		};

		/* 1969 */
		struct MaterialPass
		{
			MaterialVertexDeclaration* vertexDecl;
			MaterialVertexShader* vertexShader;
			MaterialPixelShader* pixelShader;
			unsigned __int8 perPrimArgCount;
			unsigned __int8 perObjArgCount;
			unsigned __int8 stableArgCount;
			unsigned __int8 customSamplerFlags;
			unsigned __int8 precompiledIndex;
			unsigned __int8 materialType;
			MaterialShaderArgument* args;
		};

		/* 1970 */
		struct MaterialTechnique
		{
			const char* name;
			unsigned __int16 flags;
			unsigned __int16 passCount;
			MaterialPass passArray[1];
		};

		/* 1972 */
		struct __declspec(align(4)) GfxImageLoadDef
		{
			unsigned __int8 levelCount;
			unsigned __int8 flags;
			int format;
			int resourceSize;
			unsigned __int8 data[1];
		};

		/* 2189 */
		struct SndAliasList
		{
			const char* name;
			unsigned int id;
			SndAlias* head;
			int count;
			int sequence;
		};

		/* 3736 */
		struct SndIndexEntry
		{
			unsigned __int16 value;
			unsigned __int16 next;
		};

		/* 3737 */
		struct SndRadverb
		{
			char name[32];
			unsigned int id;
			float smoothing;
			float earlyTime;
			float lateTime;
			float earlyGain;
			float lateGain;
			float returnGain;
			float earlyLpf;
			float lateLpf;
			float inputLpf;
			float dampLpf;
			float wallReflect;
			float dryGain;
			float earlySize;
			float lateSize;
			float diffusion;
			float returnHighpass;
		};

		/* 3738 */
		struct SndDuck
		{
			char name[32];
			unsigned int id;
			float fadeIn;
			float fadeOut;
			float startDelay;
			float distance;
			float length;
			unsigned int fadeInCurve;
			unsigned int fadeOutCurve;
			float* attenuation;
			float* filter;
			int updateWhilePaused;
		};

		/* 3741 */
		struct SndAssetBankEntry
		{
			unsigned int id;
			unsigned int size;
			unsigned int offset;
			unsigned int frameCount;
			unsigned __int8 frameRateIndex;
			unsigned __int8 channelCount;
			unsigned __int8 looping;
			unsigned __int8 format;
		};

		/* 3743 */
		struct SndDialogScriptIdLookup
		{
			unsigned int scriptId;
			unsigned int aliasId;
		};

		/* 1850 */
		struct cplane_s
		{
			vec3_t normal;
			float dist;
			unsigned __int8 type;
			unsigned __int8 signbits;
			unsigned __int8 pad[2];
		};

		/* 2905 */
		struct ClipMaterial
		{
			const char* name;
			int surfaceFlags;
			int contentFlags;
		};

		/* 1851 */
		struct cbrushside_t
		{
			cplane_s* plane;
			int cflags;
			int sflags;
		};

		/* 2826 */
		struct cLeafBrushNodeLeaf_t
		{
			unsigned __int16* brushes;
		};

		/* 2827 */
		struct cLeafBrushNodeChildren_t
		{
			float dist;
			float range;
			unsigned __int16 childOffset[2];
		};

		/* 2828 */
		union cLeafBrushNodeData_t
		{
			cLeafBrushNodeLeaf_t leaf;
			cLeafBrushNodeChildren_t children;
		};

		/* 2829 */
		struct cLeafBrushNode_s
		{
			unsigned __int8 axis;
			__int16 leafBrushCount;
			int contents;
			cLeafBrushNodeData_t data;
		};

		/* 2904 */
		struct __declspec(align(8)) cbrush_t
		{
			vec3_t mins;
			int contents;
			vec3_t maxs;
			unsigned int numsides;
			cbrushside_t* sides;
			int axial_cflags[2][3];
			int axial_sflags[2][3];
			unsigned int numverts;
			vec3_t* verts;
		};

		/* 2687 */
		struct cStaticModelWritable
		{
			unsigned __int16 nextModelInWorldSector;
		};

		/* 2688 */
		struct cStaticModel_s
		{
			cStaticModelWritable writable;
			XModel* xmodel;
			int contents;
			vec3_t origin;
			vec3_t invScaledAxis[3];
			vec3_t absmin;
			vec3_t absmax;
		};

		/* 3746 */
		struct cNode_t
		{
			cplane_s* plane;
			__int16 children[2];
		};

		/* 3493 */
		const struct CollisionPartition
		{
			unsigned __int8 triCount;
			int firstTri;
			int nuinds;
			int fuind;
		};

		/* 2926 */
		union CollisionAabbTreeIndex
		{
			int firstChildIndex;
			int partitionIndex;
		};

		/* 2927 */
		const struct CollisionAabbTree
		{
			vec3_t origin;
			unsigned __int16 materialIndex;
			unsigned __int16 childCount;
			vec3_t halfSize;
			CollisionAabbTreeIndex u;
		};



		/* 3483 */
		struct GfxPlacement
		{
			vec4_t quat;
			vec3_t origin;
		};

		/* 3486 */
		struct DynEntityDef
		{
			DynEntityType type;
			GfxPlacement pose;
			XModel* xModel;
			XModel* destroyedxModel;
			unsigned __int16 brushModel;
			unsigned __int16 physicsBrushModel;
			const FxEffectDef* destroyFx;
			unsigned int destroySound;
			XModelPieces* destroyPieces;
			PhysPreset* physPreset;
			__int16 physConstraints[4];
			int health;
			int flags;
			int contents;
			unsigned __int16 targetname;
			unsigned __int16 target;
		};

		/* 3752 */
		struct DynEntityPose
		{
			GfxPlacement pose;
			float radius;
		};

		/* 3753 */
		struct DynEntityClient
		{
			int physObjId;
			unsigned __int16 flags;
			unsigned __int16 lightingHandle;
			int health;
			unsigned __int16 burnTime;
			unsigned __int16 fadeTime;
			int physicsStartTime;
		};

		/* 3754 */
		struct DynEntityServer
		{
			unsigned __int16 flags;
			int health;
		};

		/* 3755 */
		struct DynEntityColl
		{
			unsigned __int16 sector;
			unsigned __int16 nextEntInSector;
			vec3_t linkMins;
			vec3_t linkMaxs;
			int contents;
		};

		/* 3756 */
		struct par_t
		{
			vec3_t p;
			vec3_t p0;
			vec3_t p_prev;
			int flags;
		};

		/* 3561 */
		struct __declspec(align(4)) constraint_t
		{
			vec3_t p;
			rope_constraint_e type;
			int entity_index;
			int bone_name_hash;
			unsigned __int8 pi1;
			unsigned __int8 pi2;
		};

		/* 3757 */
		struct rope_frame_verts_t
		{
			int num_verts;
			vec3_t v[50];
		};

		/* 3758 */
		struct rope_client_verts_t
		{
			rope_frame_verts_t frame_verts[2];
			unsigned int frame_index;
		};

		/* 3759 */
		struct __declspec(align(4)) rope_t
		{
			par_t m_particles[25];
			constraint_t m_constraints[30];
			int m_entity_anchors[3];
			int m_num_particles;
			int m_num_constraints;
			int m_num_entity_anchors;
			int m_num_draw_verts;
			rope_client_verts_t m_client_verts;
			vec3_t m_min;
			vec3_t m_max;
			vec3_t m_start;
			vec3_t m_end;
			int m_in_use;
			int m_visible;
			int m_dist_constraint;
			int m_flags;
			const Material* m_material;
			float m_seglen;
			float m_length;
			float m_width;
			float m_scale;
			float m_force_scale;
			int m_health;
			int m_frame;
			int m_stable_count;
			int m_static_rope;
			unsigned __int16 m_lightingHandle;
		};

		/* 3052 */
		struct pathnode_constant_t
		{
			nodeType type;
			int spawnflags;
			unsigned __int16 targetname;
			unsigned __int16 script_linkName;
			unsigned __int16 script_noteworthy;
			unsigned __int16 target;
			unsigned __int16 animscript;
			int animscriptfunc;
			vec3_t vOrigin;
			float fAngle;
			vec2_t forward;
			float fRadius;
			float minUseDistSq;
			__int16 wOverlapNode[2];
			unsigned __int16 totalLinkCount;
			pathlink_s* Links;
		};

		/* 3053 */
		struct SentientHandle
		{
			unsigned __int16 number;
			unsigned __int16 infoIndex;
		};

		/* 3054 */
		struct __declspec(align(4)) pathnode_dynamic_t
		{
			SentientHandle pOwner;
			int iFreeTime;
			int iValidTime[3];
			int dangerousNodeTime[3];
			int inPlayerLOSTime;
			__int16 wLinkCount;
			__int16 wOverlapCount;
			__int16 turretEntNumber;
			__int16 userCount;
			bool hasBadPlaceLink;
		};

		/* 3056 */
		union $73F238679C0419BE2C31C6559E8604FC
		{
			float nodeCost;
			int linkIndex;
		};

		/* 3057 */
		struct pathnode_transient_t
		{
			int iSearchFrame;
			pathnode_t* pNextOpen;
			pathnode_t* pPrevOpen;
			pathnode_t* pParent;
			float fCost;
			float fHeuristic;
			$73F238679C0419BE2C31C6559E8604FC ___u6;
		};

		/* 3055 */
		struct pathnode_t
		{
			pathnode_constant_t constant;
			pathnode_dynamic_t dynamic;
			pathnode_transient_t transient;
		};

		/* 3763 */
		struct pathbasenode_t
		{
			vec3_t vOrigin;
			unsigned int type;
		};

		/* 3765 */
		struct pathnode_tree_nodes_t
		{
			int nodeCount;
			unsigned __int16* nodes;
		};

		/* 3766 */
		union pathnode_tree_info_t
		{
			pathnode_tree_t* child[2];
			pathnode_tree_nodes_t s;
		};

		/* 3764 */
		struct pathnode_tree_t
		{
			int axis;
			float dist;
			pathnode_tree_info_t u;
		};

		/* 3747 */
		struct TriggerModel
		{
			int contents;
			unsigned __int16 hullCount;
			unsigned __int16 firstHull;
		};

		/* 3748 */
		struct TriggerHull
		{
			Bounds bounds;
			int contents;
			unsigned __int16 slabCount;
			unsigned __int16 firstSlab;
		};

		/* 3749 */
		struct TriggerSlab
		{
			vec3_t dir;
			float midPoint;
			float halfSize;
		};

		/* 3770 */
		struct GfxStreamingAabbTree
		{
			vec4_t mins;
			vec4_t maxs;
			float maxStreamingDistance;
			unsigned __int16 firstItem;
			unsigned __int16 itemCount;
			unsigned __int16 firstChild;
			unsigned __int16 childCount;
			unsigned __int16 smodelCount;
			unsigned __int16 surfaceCount;
		};

		/* 3329 */
		union $7662B16DD98C70549FD687041DB8E303
		{
			vec4_t m[4];
			float member[16];
		};

		/* 3330 */
		struct float44
		{
			$7662B16DD98C70549FD687041DB8E303 ___u0;
		};

		/* 3331 */
		struct GfxLight
		{
			unsigned __int8 type;
			unsigned __int8 canUseShadowMap;
			unsigned __int8 shadowmapVolume;
			__int16 cullDist;
			vec3_t color;
			vec3_t dir;
			vec3_t origin;
			float radius;
			float cosHalfFovOuter;
			float cosHalfFovInner;
			int exponent;
			unsigned int spotShadowIndex;
			float dAttenuation;
			float roundness;
			vec3_t angles;
			float spotShadowHiDistance;
			vec4_t diffuseColor;
			vec4_t shadowColor;
			vec4_t falloff;
			vec4_t aAbB;
			vec4_t cookieControl0;
			vec4_t cookieControl1;
			vec4_t cookieControl2;
			__declspec(align(16)) float44 viewMatrix;
			float44 projMatrix;
			GfxLightDef* def;
		};

		/* 3774 */
		struct GfxLightCorona
		{
			vec3_t origin;
			float radius;
			vec3_t color;
			float intensity;
		};

		/* 3775 */
		struct GfxShadowMapVolume
		{
			unsigned int control;
			unsigned int padding1;
			unsigned int padding2;
			unsigned int padding3;
		};

		/* 3776 */
		struct GfxVolumePlane
		{
			vec4_t plane;
		};

		/* 3777 */
		struct GfxExposureVolume
		{
			unsigned int control;
			float exposure;
			float luminanceIncreaseScale;
			float luminanceDecreaseScale;
			float featherRange;
			float featherAdjust;
		};

		/* 3778 */
		struct GfxWorldFogVolume
		{
			vec3_t mins;
			unsigned int control;
			vec3_t maxs;
			float fogTransitionTime;
			unsigned int controlEx;
			GfxWorldFog volumeWorldFog[1];
		};

		/* 3779 */
		struct GfxWorldFogModifierVolume
		{
			unsigned int control;
			unsigned __int16 minX;
			unsigned __int16 minY;
			unsigned __int16 minZ;
			unsigned __int16 maxX;
			unsigned __int16 maxY;
			unsigned __int16 maxZ;
			unsigned int controlEx;
			float transitionTime;
			float depthScale;
			float heightScale;
			vec4_t colorAdjust;
		};

		/* 3780 */
		struct GfxLutVolume
		{
			vec3_t mins;
			unsigned int control;
			vec3_t maxs;
			float lutTransitionTime;
			unsigned int lutIndex;
		};

		/* 3786 */
		struct GfxCell
		{
			vec3_t mins;
			vec3_t maxs;
			int aabbTreeCount;
			GfxAabbTree* aabbTree;
			int portalCount;
			GfxPortal* portals;
			unsigned __int8 reflectionProbeCount;
			unsigned __int8* reflectionProbes;
		};

		/* 3663 */
		struct GfxLightingSH
		{
			vec4_t V0;
			vec4_t V1;
			vec4_t V2;
		};

		/* 3788 */
		struct GfxReflectionProbe
		{
			vec3_t origin;
			GfxLightingSH lightingSH;
			GfxImage* reflectionImage;
			GfxReflectionProbeVolumeData* probeVolumes;
			unsigned int probeVolumeCount;
			float mipLodBias;
		};

		/* 3789 */
		struct GfxLightmapArray
		{
			GfxImage* primary;
			GfxImage* secondary;
		};

		/* 3798 */
		struct GfxLightGridEntry
		{
			unsigned __int16 colorsIndex;
			unsigned __int8 primaryLightIndex;
			unsigned __int8 visibility;
		};

		/* 3799 */
		struct GfxCompressedLightGridColors
		{
			unsigned __int8 rgb[56][3];
		};

		/* 3800 */
		struct GfxCompressedLightGridCoeffs
		{
			unsigned __int16 coeffs[9][3];
		};

		/* 3801 */
		struct GfxSkyGridVolume
		{
			vec3_t mins;
			vec3_t maxs;
			vec3_t lightingOrigin;
			unsigned __int16 colorsIndex;
			unsigned __int8 primaryLightIndex;
			unsigned __int8 visibility;
		};

		/* 3803 */
		struct GfxBrushModelWritable
		{
			vec3_t mins;
			float padding1;
			vec3_t maxs;
			float padding2;
		};

		/* 3804 */
		struct GfxBrushModel
		{
			GfxBrushModelWritable writable;
			vec3_t bounds[2];
			unsigned int surfaceCount;
			unsigned int startSurfIndex;
		};

		/* 3805 */
		struct MaterialMemory
		{
			Material* material;
			int memory;
		};

		/* 3807 */
		struct $35B54DACFB25E6634BE25ECADBFCE1AF
		{
			unsigned __int8 state;
			char lod;
			unsigned __int16 surfId;
		};

		/* 3808 */
		union XModelDrawInfo
		{
			$35B54DACFB25E6634BE25ECADBFCE1AF __s0;
			unsigned int packed;
		};

		/* 3809 */
		struct GfxSceneDynModel
		{
			XModelDrawInfo info;
			unsigned __int16 dynEntId;
			unsigned __int8 primaryLightIndex;
			unsigned __int8 reflectionProbeIndex;
		};

		/* 3810 */
		struct BModelDrawInfo
		{
			unsigned __int16 surfId;
		};

		/* 3811 */
		struct GfxSceneDynBrush
		{
			BModelDrawInfo info;
			unsigned __int16 dynEntId;
		};

		/* 2419 */
		union __m128
		{
			float m128_f32[4];
			unsigned __int64 m128_u64[2];
			char m128_i8[16];
			__int16 m128_i16[8];
			int m128_i32[4];
			__int64 m128_i64[2];
			unsigned __int8 m128_u8[16];
			unsigned __int16 m128_u16[8];
			unsigned int m128_u32[4];
		};

		/* 2513 */
		struct vector3
		{
			__m128 x;
			__m128 y;
			__m128 z;
		};

		/* 2514 */
		struct vector4 : vector3
		{
			__m128 w;
		};

		/* 3823 */
		union $B5E027B7D20F40C7F8A3EE8A561758AC
		{
			vector4 mat;
			float disk_mat[16];
		};

		/* 3824 */
		struct SSkinInstance
		{
			$B5E027B7D20F40C7F8A3EE8A561758AC ___u0;
			SSkinShaders* shaders;
			SSkinModel* model;
			SSkinAnim* anim;
			SSkinVert* instVerts;
			float frame;
			int pad1;
			int pad2;
			int pad3;
		};

		/* 3825 */
		struct GfxShadowGeometry
		{
			unsigned __int16 surfaceCount;
			unsigned __int16 smodelCount;
			unsigned __int16* sortedSurfIndex;
			unsigned __int16* smodelIndex;
		};

		/* 3828 */
		struct GfxLightRegion
		{
			unsigned int hullCount;
			GfxLightRegionHull* hulls;
		};

		/* 3829 */
		struct GfxStaticModelInst
		{
			vec3_t mins;
			vec3_t maxs;
			vec3_t lightingOrigin;
		};

		/* 3830 */
		struct srfTriangles_t
		{
			vec3_t mins;
			int vertexDataOffset0;
			vec3_t maxs;
			int vertexDataOffset1;
			int firstVertex;
			float himipRadiusInvSq;
			unsigned __int16 vertexCount;
			unsigned __int16 triCount;
			int baseIndex;
		};

		/* 3831 */
		struct GfxSurface
		{
			srfTriangles_t tris;
			Material* material;
			unsigned __int8 lightmapIndex;
			unsigned __int8 reflectionProbeIndex;
			unsigned __int8 primaryLightIndex;
			unsigned __int8 flags;
			vec3_t bounds[2];
		};

		/* 3832 */
		struct GfxPackedPlacement
		{
			vec3_t origin;
			vec3_t axis[3];
			float scale;
		};

		/* 3668 */
		struct GfxLightingSHQuantized
		{
			unsigned __int16 V0[4];
			unsigned __int16 V1[4];
			unsigned __int16 V2[4];
		};

		/* 3833 */
		struct __declspec(align(4)) GfxStaticModelLmapVertexInfo
		{
			unsigned int* lmapVertexColors;
			char pad[4]; //ID3D11Buffer *lmapVertexColorsVB;
			unsigned __int16 numLmapVertexColors;
		};

		/* 3834 */
		struct GfxStaticModelDrawInst
		{
			float cullDist;
			GfxPackedPlacement placement;
			XModel* model;
			int flags;
			float invScaleSq;
			unsigned __int16 lightingHandle;
			unsigned __int16 colorsIndex;
			GfxLightingSHQuantized lightingSH;
			unsigned __int8 primaryLightIndex;
			unsigned __int8 visibility;
			unsigned __int8 reflectionProbeIndex;
			unsigned int smid;
			GfxStaticModelLmapVertexInfo lmapVertexInfo[4];
		};

		/* 3838 */
		struct Occluder
		{
			unsigned int flags;
			char name[16];
			vec3_t points[4];
		};

		/* 3839 */
		struct GfxOutdoorBounds
		{
			vec3_t bounds[2];
		};

		/* 3840 */
		struct GfxHeroLight
		{
			unsigned __int8 type;
			unsigned __int8 unused[3];
			vec3_t color;
			vec3_t dir;
			vec3_t origin;
			float radius;
			float cosHalfFovOuter;
			float cosHalfFovInner;
			int exponent;
		};

		/* 3841 */
		struct GfxHeroLightTree
		{
			vec3_t mins;
			vec3_t maxs;
			int leftNode;
			int rightNode;
		};

		/* 2153 */
		struct GenericEventHandler
		{
			const char* name;
			GenericEventScript* eventScript;
			GenericEventHandler* next;
		};

		/* 2329 */
		struct ItemKeyHandler
		{
			int key;
			GenericEventScript* keyScript;
			ItemKeyHandler* next;
		};

		/* 1847 */
		union operandInternalDataUnion
		{
			int intVal;
			float floatVal;
			const char* string;
			const dvar_t* dvar;
		};

		/* 1994 */
		struct Operand
		{
			expDataType dataType;
			operandInternalDataUnion internals;
		};

		/* 2091 */
		union expressionRpnDataUnion
		{
			Operand constant;
			void* cmd;
			int cmdIdx;
		};

		/* 2150 */
		struct expressionRpn
		{
			int type;
			expressionRpnDataUnion data;
		};

		/* 2338 */
		union itemDefData_t
		{
			textDef_s* textDef;
			imageDef_s* imageDef;
			focusItemDef_s* blankButtonDef;
			ownerDrawDef_s* ownerDrawDef;
			void* data;
		};

		/* 2342 */
		struct __declspec(align(8)) itemDef_s
		{
			windowDef_t window;
			int type;
			int dataType;
			int imageTrack;
			const char* dvar;
			const char* profileVar;
			const char* dvarTest;
			const char* enableDvar;
			int dvarFlags;
			itemDefData_t typeData;
			menuDef_t* parent;
			rectData_s* rectExpData;
			ExpressionStatement visibleExp;
			unsigned __int64 showBits;
			unsigned __int64 hideBits;
			ExpressionStatement forecolorAExp;
			int ui3dWindowId;
			GenericEventHandler* onEvent;
			UIAnimInfo* animInfo;
		};

		/* 2816 */
		struct WeaponDef
		{
			const char* szOverlayName;
			XModel** gunXModel;
			XModel* handXModel;
			const char* szModeName;
			unsigned __int16* notetrackSoundMapKeys;
			unsigned __int16* notetrackSoundMapValues;
			int playerAnimType;
			weapType_t weapType;
			weapClass_t weapClass;
			PenetrateType penetrateType;
			ImpactType impactType;
			weapInventoryType_t inventoryType;
			weapFireType_t fireType;
			weapClipType_t clipType;
			barrelType_t barrelType;
			int itemIndex;
			const char* parentWeaponName;
			int iJamFireTime;
			int overheatWeapon;
			float overheatRate;
			float cooldownRate;
			float overheatEndVal;
			bool coolWhileFiring;
			bool fuelTankWeapon;
			int iTankLifeTime;
			OffhandClass offhandClass;
			OffhandSlot offhandSlot;
			weapStance_t stance;
			const FxEffectDef* viewFlashEffect;
			const FxEffectDef* worldFlashEffect;
			const FxEffectDef* barrelCooldownEffect;
			int barrelCooldownMinCount;
			float vViewFlashOffset[3];
			float vWorldFlashOffset[3];
			const char* pickupSound;
			const char* pickupSoundPlayer;
			const char* ammoPickupSound;
			const char* ammoPickupSoundPlayer;
			const char* projectileSound;
			const char* pullbackSound;
			const char* pullbackSoundPlayer;
			const char* fireSound;
			const char* fireSoundPlayer;
			const char* fireLoopSound;
			const char* fireLoopSoundPlayer;
			const char* fireLoopEndSound;
			const char* fireLoopEndSoundPlayer;
			const char* fireStartSound;
			const char* fireStopSound;
			const char* fireKillcamSound;
			const char* fireStartSoundPlayer;
			const char* fireStopSoundPlayer;
			const char* fireKillcamSoundPlayer;
			const char* fireLastSound;
			const char* fireLastSoundPlayer;
			const char* emptyFireSound;
			const char* emptyFireSoundPlayer;
			const char* crackSound;
			const char* whizbySound;
			const char* meleeSwipeSound;
			const char* meleeSwipeSoundPlayer;
			const char* meleeHitSound;
			const char* meleeMissSound;
			const char* rechamberSound;
			const char* rechamberSoundPlayer;
			const char* reloadSound;
			const char* reloadSoundPlayer;
			const char* reloadEmptySound;
			const char* reloadEmptySoundPlayer;
			const char* reloadStartSound;
			const char* reloadStartSoundPlayer;
			const char* reloadEndSound;
			const char* reloadEndSoundPlayer;
			const char* rotateLoopSound;
			const char* rotateLoopSoundPlayer;
			const char* rotateStopSound;
			const char* rotateStopSoundPlayer;
			const char* deploySound;
			const char* deploySoundPlayer;
			const char* finishDeploySound;
			const char* finishDeploySoundPlayer;
			const char* breakdownSound;
			const char* breakdownSoundPlayer;
			const char* finishBreakdownSound;
			const char* finishBreakdownSoundPlayer;
			const char* detonateSound;
			const char* detonateSoundPlayer;
			const char* nightVisionWearSound;
			const char* nightVisionWearSoundPlayer;
			const char* nightVisionRemoveSound;
			const char* nightVisionRemoveSoundPlayer;
			const char* altSwitchSound;
			const char* altSwitchSoundPlayer;
			const char* raiseSound;
			const char* raiseSoundPlayer;
			const char* firstRaiseSound;
			const char* firstRaiseSoundPlayer;
			const char* adsRaiseSoundPlayer;
			const char* adsLowerSoundPlayer;
			const char* putawaySound;
			const char* putawaySoundPlayer;
			const char* overheatSound;
			const char* overheatSoundPlayer;
			const char* adsZoomSound;
			const char* shellCasing;
			const char* shellCasingPlayer;
			const char* bounceSound; //const char** bounceSound;
			const char* standMountedWeapdef;
			const char* crouchMountedWeapdef;
			const char* proneMountedWeapdef;
			int standMountedIndex;
			int crouchMountedIndex;
			int proneMountedIndex;
			const FxEffectDef* viewShellEjectEffect;
			const FxEffectDef* worldShellEjectEffect;
			const FxEffectDef* viewLastShotEjectEffect;
			const FxEffectDef* worldLastShotEjectEffect;
			float vViewShellEjectOffset[3];
			float vWorldShellEjectOffset[3];
			float vViewShellEjectRotation[3];
			float vWorldShellEjectRotation[3];
			Material* reticleCenter;
			Material* reticleSide;
			int iReticleCenterSize;
			int iReticleSideSize;
			int iReticleMinOfs;
			activeReticleType_t activeReticleType;
			float vStandMove[3];
			float vStandRot[3];
			float vDuckedOfs[3];
			float vDuckedMove[3];
			float vDuckedSprintOfs[3];
			float vDuckedSprintRot[3];
			float vDuckedSprintBob[2];
			float fDuckedSprintCycleScale;
			float vSprintOfs[3];
			float vSprintRot[3];
			float vSprintBob[2];
			float fSprintCycleScale;
			float vLowReadyOfs[3];
			float vLowReadyRot[3];
			float vRideOfs[3];
			float vRideRot[3];
			float vDtpOfs[3];
			float vDtpRot[3];
			float vDtpBob[2];
			float fDtpCycleScale;
			float vMantleOfs[3];
			float vMantleRot[3];
			float vSlideOfs[3];
			float vSlideRot[3];
			float vDuckedRot[3];
			float vProneOfs[3];
			float vProneMove[3];
			float vProneRot[3];
			float vStrafeMove[3];
			float vStrafeRot[3];
			float fPosMoveRate;
			float fPosProneMoveRate;
			float fStandMoveMinSpeed;
			float fDuckedMoveMinSpeed;
			float fProneMoveMinSpeed;
			float fPosRotRate;
			float fPosProneRotRate;
			float fStandRotMinSpeed;
			float fDuckedRotMinSpeed;
			float fProneRotMinSpeed;
			XModel** worldModel;
			XModel* worldClipModel;
			XModel* rocketModel;
			XModel* mountedModel;
			XModel* additionalMeleeModel;
			Material* fireTypeIcon;
			Material* hudIcon;
			weaponIconRatioType_t hudIconRatio;
			Material* indicatorIcon;
			weaponIconRatioType_t indicatorIconRatio;
			Material* ammoCounterIcon;
			weaponIconRatioType_t ammoCounterIconRatio;
			ammoCounterClipType_t ammoCounterClip;
			int iStartAmmo;
			int iMaxAmmo;
			int shotCount;
			const char* szSharedAmmoCapName;
			int iSharedAmmoCapIndex;
			int iSharedAmmoCap;
			bool unlimitedAmmo;
			bool ammoCountClipRelative;
			int damage[6];
			float damageRange[6];
			int minPlayerDamage;
			float damageDuration;
			float damageInterval;
			int playerDamage;
			int iMeleeDamage;
			int iDamageType;
			unsigned __int16 explosionTag;
			int iFireDelay;
			int iMeleeDelay;
			int meleeChargeDelay;
			int iDetonateDelay;
			int iSpinUpTime;
			int iSpinDownTime;
			float spinRate;
			const char* spinLoopSound;
			const char* spinLoopSoundPlayer;
			const char* startSpinSound;
			const char* startSpinSoundPlayer;
			const char* stopSpinSound;
			const char* stopSpinSoundPlayer;
			bool applySpinPitch;
			int iFireTime;
			int iLastFireTime;
			int iRechamberTime;
			int iRechamberBoltTime;
			int iHoldFireTime;
			int iDetonateTime;
			int iMeleeTime;
			int iBurstDelayTime;
			int meleeChargeTime;
			int iReloadTimeRight;
			int iReloadTimeLeft;
			int reloadShowRocketTime;
			int iReloadEmptyTimeLeft;
			int iReloadAddTime;
			int iReloadEmptyAddTime;
			int iReloadQuickAddTime;
			int iReloadQuickEmptyAddTime;
			int iReloadStartTime;
			int iReloadStartAddTime;
			int iReloadEndTime;
			int iDropTime;
			int iRaiseTime;
			int iAltDropTime;
			int quickDropTime;
			int quickRaiseTime;
			int iFirstRaiseTime;
			int iEmptyRaiseTime;
			int iEmptyDropTime;
			int sprintInTime;
			int sprintLoopTime;
			int sprintOutTime;
			int lowReadyInTime;
			int lowReadyLoopTime;
			int lowReadyOutTime;
			int contFireInTime;
			int contFireLoopTime;
			int contFireOutTime;
			int dtpInTime;
			int dtpLoopTime;
			int dtpOutTime;
			int crawlInTime;
			int crawlForwardTime;
			int crawlBackTime;
			int crawlRightTime;
			int crawlLeftTime;
			int crawlOutFireTime;
			int crawlOutTime;
			int slideInTime;
			int deployTime;
			int breakdownTime;
			int iFlourishTime;
			int nightVisionWearTime;
			int nightVisionWearTimeFadeOutEnd;
			int nightVisionWearTimePowerUp;
			int nightVisionRemoveTime;
			int nightVisionRemoveTimePowerDown;
			int nightVisionRemoveTimeFadeInStart;
			int fuseTime;
			int aiFuseTime;
			int lockOnRadius;
			int lockOnSpeed;
			bool requireLockonToFire;
			bool noAdsWhenMagEmpty;
			bool avoidDropCleanup;
			unsigned int stackFire;
			float stackFireSpread;
			float stackFireAccuracyDecay;
			const char* stackSound;
			float autoAimRange;
			float aimAssistRange;
			bool mountableWeapon;
			float aimPadding;
			float enemyCrosshairRange;
			bool crosshairColorChange;
			float moveSpeedScale;
			float adsMoveSpeedScale;
			float sprintDurationScale;
			weapOverlayReticle_t overlayReticle;
			WeapOverlayInteface_t overlayInterface;
			float overlayWidth;
			float overlayHeight;
			float fAdsBobFactor;
			float fAdsViewBobMult;
			bool bHoldBreathToSteady;
			float fHipSpreadStandMin;
			float fHipSpreadDuckedMin;
			float fHipSpreadProneMin;
			float hipSpreadStandMax;
			float hipSpreadDuckedMax;
			float hipSpreadProneMax;
			float fHipSpreadDecayRate;
			float fHipSpreadFireAdd;
			float fHipSpreadTurnAdd;
			float fHipSpreadMoveAdd;
			float fHipSpreadDuckedDecay;
			float fHipSpreadProneDecay;
			float fHipReticleSidePos;
			float fAdsIdleAmount;
			float fHipIdleAmount;
			float adsIdleSpeed;
			float hipIdleSpeed;
			float fIdleCrouchFactor;
			float fIdleProneFactor;
			float fGunMaxPitch;
			float fGunMaxYaw;
			float swayMaxAngle;
			float swayLerpSpeed;
			float swayPitchScale;
			float swayYawScale;
			float swayHorizScale;
			float swayVertScale;
			float swayShellShockScale;
			float adsSwayMaxAngle;
			float adsSwayLerpSpeed;
			float adsSwayPitchScale;
			float adsSwayYawScale;
			bool sharedAmmo;
			bool bRifleBullet;
			bool armorPiercing;
			bool bAirburstWeapon;
			bool bBoltAction;
			bool bUseAltTagFlash;
			bool bUseAntiLagRewind;
			bool bIsCarriedKillstreakWeapon;
			bool aimDownSight;
			bool bRechamberWhileAds;
			bool bReloadWhileAds;
			float adsViewErrorMin;
			float adsViewErrorMax;
			bool bCookOffHold;
			bool bClipOnly;
			bool bCanUseInVehicle;
			bool bNoDropsOrRaises;
			bool adsFireOnly;
			bool cancelAutoHolsterWhenEmpty;
			bool suppressAmmoReserveDisplay;
			bool laserSight;
			bool laserSightDuringNightvision;
			bool bHideThirdPerson;
			bool bHasBayonet;
			bool bDualWield;
			bool bExplodeOnGround;
			bool bThrowBack;
			bool bRetrievable;
			bool bDieOnRespawn;
			bool bNoThirdPersonDropsOrRaises;
			bool bContinuousFire;
			bool bNoPing;
			bool bForceBounce;
			bool bUseDroppedModelAsStowed;
			bool bNoQuickDropWhenEmpty;
			bool bKeepCrosshairWhenADS;
			bool bUseOnlyAltWeaoponHideTagsInAltMode;
			bool bAltWeaponAdsOnly;
			bool bAltWeaponDisableSwitching;
			Material* killIcon;
			weaponIconRatioType_t killIconRatio;
			bool flipKillIcon;
			bool bNoPartialReload;
			bool bSegmentedReload;
			bool bNoADSAutoReload;
			int iReloadAmmoAdd;
			int iReloadStartAdd;
			const char* szSpawnedGrenadeWeaponName;
			const char* szDualWieldWeaponName;
			unsigned int dualWieldWeaponIndex;
			int iDropAmmoMin;
			int iDropAmmoMax;
			int iDropClipAmmoMin;
			int iDropClipAmmoMax;
			int iShotsBeforeRechamber;
			bool blocksProne;
			bool bShowIndicator;
			int isRollingGrenade;
			int useBallisticPrediction;
			int isValuable;
			int isTacticalInsertion;
			bool isReviveWeapon;
			bool bUseRigidBodyOnVehicle;
			int iExplosionRadius;
			int iExplosionRadiusMin;
			int iIndicatorRadius;
			int iExplosionInnerDamage;
			int iExplosionOuterDamage;
			float damageConeAngle;
			int iProjectileSpeed;
			int iProjectileSpeedUp;
			int iProjectileSpeedRelativeUp;
			int iProjectileSpeedForward;
			float fProjectileTakeParentVelocity;
			int iProjectileActivateDist;
			float projLifetime;
			float timeToAccelerate;
			float projectileCurvature;
			XModel* projectileModel;
			weapProjExposion_t projExplosion;
			const FxEffectDef* projExplosionEffect;
			bool projExplosionEffectForceNormalUp;
			const FxEffectDef* projExplosionEffect2;
			bool projExplosionEffect2ForceNormalUp;
			const FxEffectDef* projExplosionEffect3;
			bool projExplosionEffect3ForceNormalUp;
			const FxEffectDef* projExplosionEffect4;
			bool projExplosionEffect4ForceNormalUp;
			const FxEffectDef* projExplosionEffect5;
			bool projExplosionEffect5ForceNormalUp;
			const FxEffectDef* projDudEffect;
			const char* projExplosionSound;
			const char* projDudSound;
			const char* mortarShellSound;
			const char* tankShellSound;
			bool bProjImpactExplode;
			bool bProjSentientImpactExplode;
			bool bProjExplodeWhenStationary;
			bool bBulletImpactExplode;
			WeapStickinessType stickiness;
			WeapRotateType rotateType;
			bool plantable;
			bool hasDetonator;
			bool timedDetonation;
			bool bNoCrumpleMissile;
			bool rotate;
			bool bKeepRolling;
			bool holdButtonToThrow;
			bool offhandHoldIsCancelable;
			bool freezeMovementWhenFiring;
			float lowAmmoWarningThreshold;
			bool bDisallowAtMatchStart;
			float meleeChargeRange;
			bool bUseAsMelee;
			bool isCameraSensor;
			bool isAcousticSensor;
			bool isLaserSensor;
			bool isHoldUseGrenade;
			float parallelBounce; //float* parallelBounce;
			float perpendicularBounce; //float* perpendicularBounce;
			const FxEffectDef* projTrailEffect;
			float vProjectileColor[3];
			guidedMissileType_t guidedMissileType;
			float maxSteeringAccel;
			int projIgnitionDelay;
			const FxEffectDef* projIgnitionEffect;
			const char* projIgnitionSound;
			float fAdsAimPitch;
			float fAdsCrosshairInFrac;
			float fAdsCrosshairOutFrac;
			int adsGunKickReducedKickBullets;
			float adsGunKickReducedKickPercent;
			float fAdsGunKickPitchMin;
			float fAdsGunKickPitchMax;
			float fAdsGunKickYawMin;
			float fAdsGunKickYawMax;
			float fAdsGunKickAccel;
			float fAdsGunKickSpeedMax;
			float fAdsGunKickSpeedDecay;
			float fAdsGunKickStaticDecay;
			float fAdsViewKickPitchMin;
			float fAdsViewKickPitchMax;
			float fAdsViewKickMinMagnitude;
			float fAdsViewKickYawMin;
			float fAdsViewKickYawMax;
			float fAdsRecoilReductionRate;
			float fAdsRecoilReductionLimit;
			float fAdsRecoilReturnRate;
			float fAdsViewScatterMin;
			float fAdsViewScatterMax;
			float fAdsSpread;
			int hipGunKickReducedKickBullets;
			float hipGunKickReducedKickPercent;
			float fHipGunKickPitchMin;
			float fHipGunKickPitchMax;
			float fHipGunKickYawMin;
			float fHipGunKickYawMax;
			float fHipGunKickAccel;
			float fHipGunKickSpeedMax;
			float fHipGunKickSpeedDecay;
			float fHipGunKickStaticDecay;
			float fHipViewKickPitchMin;
			float fHipViewKickPitchMax;
			float fHipViewKickMinMagnitude;
			float fHipViewKickYawMin;
			float fHipViewKickYawMax;
			float fHipViewScatterMin;
			float fHipViewScatterMax;
			float fAdsViewKickCenterDuckedScale;
			float fAdsViewKickCenterProneScale;
			float fAntiQuickScopeTime;
			float fAntiQuickScopeScale;
			float fAntiQuickScopeSpreadMultiplier;
			float fAntiQuickScopeSpreadMax;
			float fAntiQuickScopeSwayFactor;
			float fightDist;
			float maxDist;
			const char* accuracyGraphName[2];
			float* accuracyGraphKnots[2][2];
			float* originalAccuracyGraphKnots[2][2];
			int accuracyGraphKnotCount[2];
			int originalAccuracyGraphKnotCount[2];
			int iPositionReloadTransTime;
			float leftArc;
			float rightArc;
			float topArc;
			float bottomArc;
			float accuracy;
			float aiSpread;
			float playerSpread;
			float minTurnSpeed[2];
			float maxTurnSpeed[2];
			float pitchConvergenceTime;
			float yawConvergenceTime;
			float suppressTime;
			float maxRange;
			float fAnimHorRotateInc;
			float fPlayerPositionDist;
			const char* szUseHintString;
			const char* dropHintString;
			int iUseHintStringIndex;
			int dropHintStringIndex;
			float horizViewJitter;
			float vertViewJitter;
			float cameraShakeScale;
			int cameraShakeDuration;
			int cameraShakeRadius;
			float explosionCameraShakeScale;
			int explosionCameraShakeDuration;
			int explosionCameraShakeRadius;
			const char* szScript;
			float destabilizationRateTime;
			float destabilizationCurvatureMax;
			int destabilizeDistance;
			float* locationDamageMultipliers;
			const char* fireRumble;
			const char* meleeImpactRumble;
			const char* reloadRumble;
			const char* explosionRumble;
			TracerDef* tracerType;
			TracerDef* enemyTracerType;
			float adsDofStart;
			float adsDofEnd;
			float hipDofStart;
			float hipDofEnd;
			float scanSpeed;
			float scanAccel;
			int scanPauseTime;
			const char* flameTableFirstPerson;
			const char* flameTableThirdPerson;
			flameTable* flameTableFirstPersonPtr;
			flameTable* flameTableThirdPersonPtr;
			const FxEffectDef* tagFx_preparationEffect;
			const FxEffectDef* tagFlash_preparationEffect;
			bool doGibbing;
			float maxGibDistance;
			float altScopeADSTransInTime;
			float altScopeADSTransOutTime;
			int iIntroFireTime;
			int iIntroFireLength;
			const FxEffectDef* meleeSwipeEffect;
			const FxEffectDef* meleeImpactEffect;
			const FxEffectDef* meleeImpactNoBloodEffect;
			const char* throwBackType;
			WeaponCamo* weaponCamo;
			float customFloat0;
			float customFloat1;
			float customFloat2;
			int customBool0;
			int customBool1;
			int customBool2;
		};

		/* 2814 */
		struct WeaponCamoSet
		{
			GfxImage* solidCamoImage;
			GfxImage* patternCamoImage;
			vec2_t patternOffset;
			float patternScale;
		};

		/* 2353 */
		struct WeaponCamoMaterialSet
		{
			unsigned int numMaterials;
			WeaponCamoMaterial* materials;
		};

		/* 3432 */
		struct SndVolumeGroup
		{
			char name[32];
			char parentName[32];
			unsigned int id;
			int parentIndex;
			SndMenuCategory category;
			unsigned __int16 attenuationSp;
			unsigned __int16 attenuationMp;
		};

		/* 3849 */
		struct SndCurve
		{
			char name[32];
			unsigned int id;
			vec2_t points[8];
		};

		/* 3850 */
		struct SndPan
		{
			char name[32];
			unsigned int id;
			float front;
			float back;
			float center;
			float lfe;
			float left;
			float right;
		};

		/* 3851 */
		struct SndDuckGroup
		{
			char name[32];
			unsigned int id;
		};

		/* 3852 */
		struct SndContext
		{
			unsigned int type;
			unsigned int values[8];
		};

		/* 3853 */
		struct SndMaster
		{
			char name[32];
			unsigned int id;
			int lowE;
			float lowG;
			float lowF;
			float lowQ;
			int peak1E;
			float peak1G;
			float peak1F;
			float peak1Q;
			int peak2E;
			float peak2G;
			float peak2F;
			float peak2Q;
			int hiE;
			float hiG;
			float hiF;
			float hiQ;
			float eqG;
			int compE;
			float compPG;
			float compMG;
			float compT;
			float compR;
			float compTA;
			float compTR;
			int limitE;
			float limitPG;
			float limitMG;
			float limitT;
			float limitR;
			float limitTA;
			float limitTR;
			float busReverbG;
			float busFxG;
			float busVoiceG;
			float busPfutzG;
			float busHdrfxG;
			float busUiG;
			float busMusicG;
			float busMovieG;
			float busVcsG;
			int busReverbE;
			int busFxE;
			int busVoiceE;
			int busPfutzE;
			int busHdrfxE;
			int busUiE;
			int busMusicE;
			int busMovieE;
			int hdrfxCompE;
			int voiceEqE;
			int voiceCompE;
		};

		/* 3854 */
		struct SndSidechainDuck
		{
			char name[32];
			unsigned int id;
			float g;
			float f;
			float q;
			float ta;
			float tr;
			float tf;
		};

		/* 3855 */
		struct SndFutz
		{
			char name[32];
			unsigned int id;
			float bpfF;
			float bpfQ;
			float lsG;
			float lsF;
			float lsQ;
			float dist;
			float preG;
			float postG;
			float th;
			float tg;
			float clippre;
			float clippost;
			float blend;
			unsigned int startAliasId;
			unsigned int stopAliasId;
			unsigned int loopAliasId;
		};

		/* 2785 */
		struct FxSpawnDefLooping
		{
			int intervalMsec;
			int count;
		};

		/* 2786 */
		struct FxIntRange
		{
			int base;
			int amplitude;
		};

		/* 2787 */
		struct FxSpawnDefOneShot
		{
			FxIntRange count;
		};

		/* 2788 */
		union FxSpawnDef
		{
			FxSpawnDefLooping looping;
			FxSpawnDefOneShot oneShot;
		};

		/* 2790 */
		struct FxElemAtlas
		{
			unsigned __int8 behavior;
			unsigned __int8 index;
			unsigned __int8 fps;
			unsigned __int8 loopCount;
			unsigned __int8 colIndexBits;
			unsigned __int8 rowIndexBits;
			unsigned __int16 entryCountAndIndexRange;
		};

		/* 2798 */
		union FxEffectDefRef
		{
			const FxEffectDef* handle;
			const char* name;
		};

		/* 2801 */
		union FxElemVisuals
		{
			const void* anonymous;
			Material* material;
			XModel* model;
			FxEffectDefRef effectDef;
			const char* soundName;
			GfxLightDef* lightDef;
		};

		/* 2802 */
		union FxElemDefVisuals
		{
			FxElemMarkVisuals* markArray;
			FxElemVisuals* array;
			FxElemVisuals instance;
		};

		/* 2808 */
		union FxElemExtendedDefPtr
		{
			FxTrailDef* localTrailDef;
			FxTrailDef* trailDef;
			FxSpotLightDef* localSpotLightDef;
			FxSpotLightDef* spotLightDef;
			void* unknownDef;
		};

		/* 2809 */
		struct FxBillboardTrim
		{
			float topWidth;
			float bottomWidth;
		};

		/* 2810 */
		union FxElemDefUnion
		{
			FxBillboardTrim billboard;
			FxIntRange cloudDensityRange;
		};

		/* 2811 */
		struct FxElemSpawnSound
		{
			const char* spawnSound;
		};

		/* 2812 */
		const struct FxElemDef
		{
			int flags;
			FxSpawnDef spawn;
			FxFloatRange spawnRange;
			FxFloatRange fadeInRange;
			FxFloatRange fadeOutRange;
			float spawnFrustumCullRadius;
			FxIntRange spawnDelayMsec;
			FxIntRange lifeSpanMsec;
			FxFloatRange spawnOrigin[3];
			FxFloatRange spawnOffsetRadius;
			FxFloatRange spawnOffsetHeight;
			FxFloatRange spawnAngles[3];
			FxFloatRange angularVelocity[3];
			FxFloatRange initialRotation;
			unsigned int rotationAxis;
			FxFloatRange gravity;
			FxFloatRange reflectionFactor;
			FxElemAtlas atlas;
			float windInfluence;
			unsigned __int8 elemType;
			unsigned __int8 visualCount;
			unsigned __int8 velIntervalCount;
			unsigned __int8 visStateIntervalCount;
			const FxElemVelStateSample* velSamples;
			const FxElemVisStateSample* visSamples;
			FxElemDefVisuals visuals;
			vec3_t collMins;
			vec3_t collMaxs;
			FxEffectDefRef effectOnImpact;
			FxEffectDefRef effectOnDeath;
			FxEffectDefRef effectEmitted;
			FxFloatRange emitDist;
			FxFloatRange emitDistVariance;
			FxEffectDefRef effectAttached;
			FxElemExtendedDefPtr extended;
			unsigned __int8 sortOrder;
			unsigned __int8 lightingFrac;
			unsigned __int8 unused[2];
			unsigned __int16 alphaFadeTimeMsec;
			unsigned __int16 maxWindStrength;
			unsigned __int16 spawnIntervalAtMaxWind;
			unsigned __int16 lifespanAtMaxWind;
			FxElemDefUnion u;
			FxElemSpawnSound spawnSound;
			vec2_t billboardPivot;
		};

		/* 3730 */
		struct FxImpactEntry
		{
			const FxEffectDef* nonflesh[32];
			const FxEffectDef* flesh[4];
		};

		/* 3731 */
		struct StringTableCell
		{
			const char* string;
			int hash;
		};

		/* 3519 */
		struct Glass
		{
			unsigned int numCellIndices;
			unsigned __int16 cellIndices[6];
			GlassDef* glassDef;
			unsigned int index;
			unsigned int brushModel;
			vec3_t origin;
			vec3_t angles;
			vec3_t absmin;
			vec3_t absmax;
			bool isPlanar;
			unsigned __int8 numOutlineVerts;
			unsigned __int8 binormalSign;
			vec2_t* outline;
			vec3_t outlineAxis[3];
			vec3_t outlineOrigin;
			float uvScale;
			float thickness;
		};

		/* 3872 */
		struct KeyValuePair
		{
			int keyHash;
			int namespaceHash;
			const char* value;
		};

		/* 3132 */
		struct GraphFloat
		{
			char name[64];
			float knots[32][2];
			int knotCount;
			float scale;
		};

		/* 3170 */
		union XAnimDynamicFrames
		{
			unsigned __int8(*_1)[3];
			unsigned __int16(*_2)[3];
		};

		/* 3171 */
		union XAnimDynamicIndices
		{
			unsigned __int8 _1[1];
			unsigned __int16 _2[1];
		};

		/* 3172 */
		struct __declspec(align(4)) XAnimPartTransFrames
		{
			vec3_t mins;
			vec3_t size;
			XAnimDynamicFrames frames;
			XAnimDynamicIndices indices;
		};

		/* 3173 */
		union XAnimPartTransData
		{
			XAnimPartTransFrames frames;
			vec3_t frame0;
		};

		/* 3174 */
		struct XAnimPartTrans
		{
			unsigned __int16 size;
			unsigned __int8 smallTrans;
			XAnimPartTransData u;
		};

		/* 3175 */
		struct __declspec(align(4)) XAnimDeltaPartQuatDataFrames2
		{
			__int16(*frames)[2];
			XAnimDynamicIndices indices;
		};

		/* 3176 */
		union XAnimDeltaPartQuatData2
		{
			XAnimDeltaPartQuatDataFrames2 frames;
			__int16 frame0[2];
		};

		/* 3177 */
		struct XAnimDeltaPartQuat2
		{
			unsigned __int16 size;
			XAnimDeltaPartQuatData2 u;
		};

		/* 3178 */
		struct __declspec(align(4)) XAnimDeltaPartQuatDataFrames
		{
			__int16(*frames)[4];
			XAnimDynamicIndices indices;
		};

		/* 3179 */
		union XAnimDeltaPartQuatData
		{
			XAnimDeltaPartQuatDataFrames frames;
			__int16 frame0[4];
		};

		/* 3180 */
		struct XAnimDeltaPartQuat
		{
			unsigned __int16 size;
			XAnimDeltaPartQuatData u;
		};

		/* 2666 */
		union GfxColor
		{
			unsigned int packed;
			unsigned __int8 array[4];
		};

		/* 2667 */
		union PackedTexCoords
		{
			unsigned int packed;
		};

		/* 2668 */
		union PackedUnitVec
		{
			unsigned int packed;
			unsigned char array[4];
		};

		/* 2669 */
		struct GfxPackedVertex
		{
			vec3_t xyz;
			float binormalSign;
			GfxColor color;
			PackedTexCoords texCoord;
			PackedUnitVec normal;
			PackedUnitVec tangent;
		};

		/* 2674 */
		struct XRigidVertList
		{
			unsigned __int16 boneOffset;
			unsigned __int16 vertCount;
			unsigned __int16 triOffset;
			unsigned __int16 triCount;
			XSurfaceCollisionTree* collisionTree;
		};

		/* 2133 */
		struct XModelCollTri_s
		{
			vec4_t plane;
			vec4_t svec;
			vec4_t tvec;
		};

		/* 1855 */
		struct MaterialStreamRouting
		{
			unsigned __int8 source;
			unsigned __int8 dest;
		};

		/* 1954 */
		struct MaterialVertexStreamRouting
		{
			MaterialStreamRouting data[16];
			ID3D11InputLayout *decl[20];
		};

		/* 1955 */
		struct MaterialVertexDeclaration
		{
			unsigned __int8 streamCount;
			bool hasOptionalSource;
			bool isLoaded;
			MaterialVertexStreamRouting routing;
		};

		/* 1962 */
		struct $8514BDC4D443D0EE4E7523DDD937FABD
		{
			unsigned __int8 textureIndex;
			unsigned __int8 samplerIndex;
		};

		/* 1963 */
		union MaterialArgumentLocation
		{
			unsigned __int16 offset;
			$8514BDC4D443D0EE4E7523DDD937FABD __s1;
		};

		/* 1964 */
		struct MaterialArgumentCodeConst
		{
			unsigned __int16 index;
			unsigned __int8 firstRow;
			unsigned __int8 rowCount;
		};

		/* 1965 */
		union MaterialArgumentDef
		{
			const float* literalConst;
			MaterialArgumentCodeConst codeConst;
			unsigned int codeSampler;
			unsigned int nameHash;
		};

		/* 1966 */
		struct MaterialShaderArgument
		{
			unsigned __int16 type;
			MaterialArgumentLocation location;
			unsigned __int16 size;
			unsigned __int16 buffer;
			MaterialArgumentDef u;
		};

		/* 2188 */
		struct SndAlias
		{
			const char* name;
			unsigned int id;
			const char* subtitle;
			const char* secondaryname;
			unsigned int assetId;
			const char* assetFileName;
			unsigned int flags0;
			unsigned int flags1;
			unsigned int duck;
			unsigned int contextType;
			unsigned int contextValue;
			unsigned int stopOnPlay;
			unsigned int futzPatch;
			unsigned __int16 fluxTime;
			unsigned __int16 startDelay;
			unsigned __int16 reverbSend;
			unsigned __int16 centerSend;
			unsigned __int16 volMin;
			unsigned __int16 volMax;
			unsigned __int16 pitchMin;
			unsigned __int16 pitchMax;
			unsigned __int16 distMin;
			unsigned __int16 distMax;
			unsigned __int16 distReverbMax;
			unsigned __int16 envelopMin;
			unsigned __int16 envelopMax;
			unsigned __int16 envelopPercentage;
			__int16 fadeIn;
			__int16 fadeOut;
			__int16 dopplerScale;
			unsigned __int8 minPriorityThreshold;
			unsigned __int8 maxPriorityThreshold;
			unsigned __int8 probability;
			unsigned __int8 occlusionLevel;
			unsigned __int8 minPriority;
			unsigned __int8 maxPriority;
			unsigned __int8 pan;
			unsigned __int8 limitCount;
			unsigned __int8 entityLimitCount;
			unsigned __int8 duckGroup;
		};

		/* 3051 */
		struct __declspec(align(4)) pathlink_s
		{
			float fDist;
			unsigned __int16 nodeNum;
			unsigned __int8 disconnectCount;
			unsigned __int8 negotiationLink;
			unsigned __int8 flags;
			unsigned __int8 ubBadPlaceCount[5];
		};

		/* 3627 */
		struct GfxAabbTree
		{
			vec3_t mins;
			vec3_t maxs;
			unsigned __int16 childCount;
			unsigned __int16 surfaceCount;
			unsigned __int16 startSurfIndex;
			unsigned __int16 smodelIndexCount;
			unsigned __int16* smodelIndexes;
			int childrenOffset;
		};

		/* 3784 */
		struct GfxPortalWritable
		{
			bool isQueued;
			bool isAncestor;
			unsigned __int8 recursionDepth;
			unsigned __int8 hullPointCount;
			vec2_t* hullPoints;
			GfxPortal* queuedParent;
		};

		/* 3785 */
		struct DpvsPlane
		{
			vec4_t coeffs;
			unsigned __int8 side[3];
			unsigned __int8 pad;
		};

		/* 3783 */
		struct GfxPortal
		{
			GfxPortalWritable writable;
			DpvsPlane plane;
			GfxCell* cell;
			vec3_t* vertices;
			unsigned __int8 vertexCount;
			vec3_t hullAxis[2];
			vec3_t bounds[2];
		};

		/* 3787 */
		struct GfxReflectionProbeVolumeData
		{
			vec4_t volumePlanes[6];
		};

		/* 3812 */
		struct SSkinShaders
		{
			char* pixelShaderAlign;
			char* pixelShader;
			char* vertexShader;
			int pixelShaderSize;
			int vertexShaderSize;
		};

		/* 3821 */
		struct SSkinModel
		{
			int numVerts;
			int numTris;
			SSkinVert* verts;
			unsigned __int16* tris;
		};

		/* 3822 */
		struct SSkinAnim
		{
			int bones;
			int width;
			int frames;
			float* data;
		};

		/* 3813 */
		struct half
		{
			unsigned __int16 v;
		};

		/* 3814 */
		struct $C2CA3046C1337BB758100744CC3D6CDE
		{
			half x;
			half y;
			half z;
			half w;
		};

		/* 3815 */
		union $9DC32350A7A9F3CE9C868F22F4F335C1
		{
			$C2CA3046C1337BB758100744CC3D6CDE __s0;
			unsigned __int64 v;
		};

		/* 3816 */
		struct half4
		{
			$9DC32350A7A9F3CE9C868F22F4F335C1 ___u0;
		};

		/* 3817 */
		struct $4E89931BECE0A5CA2FEFDCB20746F804
		{
			half x;
			half y;
		};

		/* 3818 */
		union $29553E9C032D5E9BB4124D238FBED407
		{
			$4E89931BECE0A5CA2FEFDCB20746F804 __s0;
			unsigned int v;
		};

		/* 3819 */
		struct half2
		{
			$29553E9C032D5E9BB4124D238FBED407 ___u0;
		};

		/* 3820 */
		struct SSkinVert
		{
			half4 pos_bone;
			PackedUnitVec normal;
			half2 uv;
		};

		/* 3827 */
		struct GfxLightRegionHull
		{
			float kdopMidPoint[9];
			float kdopHalfSize[9];
			unsigned int axisCount;
			GfxLightRegionAxis* axis;
		};

		/* 2152 */
		struct GenericEventScript
		{
			ScriptCondition* prerequisites;
			ExpressionStatement condition;
			int type;
			bool fireOnTrue;
			const char* action;
			int blockID;
			int constructID;
			GenericEventScript* next;
		};

		/* 1838 */
		union DvarValue
		{
			bool enabled;
			int integer;
			unsigned int unsignedInt;
			__int64 integer64;
			unsigned __int64 unsignedInt64;
			float value;
			vec4_t vector;
			const char* string;
			unsigned __int8 color[4];
		};

		/* 1839 */
		struct DvarLimits_enum
		{
			int stringCount;
			const char** strings;
		};

		/* 1840 */
		struct DvarLimits_int
		{
			int min;
			int max;
		};

		/* 1841 */
		struct DvarLimits_int64
		{
			__int64 min;
			__int64 max;
		};

		/* 1842 */
		struct DvarLimits_value
		{
			float min;
			float max;
		};

		/* 1843 */
		struct DvarLimits_vector
		{
			float min;
			float max;
		};

		/* 1844 */
		union DvarLimits
		{
			DvarLimits_enum enumeration;
			DvarLimits_int integer;
			DvarLimits_int64 integer64;
			DvarLimits_value value;
			DvarLimits_vector vector;
		};

		/* 1845 */
		struct __declspec(align(8)) dvar_t
		{
			const char* name;
			const char* description;
			int hash;
			unsigned int flags;
			dvarType_t type;
			bool modified;
			DvarValue current;
			DvarValue latched;
			DvarValue reset;
			DvarLimits domain;
			dvar_t* hashNext;
		};

		/* 2334 */
		union textDefData_t
		{
			focusItemDef_s* focusItemDef;
			gameMsgDef_s* gameMsgDef;
			void* data;
		};

		/* 2335 */
		struct textDef_s
		{
			rectDef_s textRect[1];
			int alignment;
			int fontEnum;
			int itemFlags;
			int textAlignMode;
			float textalignx;
			float textaligny;
			float textscale;
			float textpadding;
			int textStyle;
			vec4_t fontColor;
			vec4_t glowColor;
			vec4_t shadowColor;
			float fontBlur;
			float glowSize;
			float shadowSize;
			vec2_t shadowOffset;
			const char* text;
			textExp_s* textExpData;
			textDefData_t textTypeData;
		};

		/* 2336 */
		struct imageDef_s
		{
			ExpressionStatement materialExp;
		};

		/* 2332 */
		union focusDefData_t
		{
			listBoxDef_s* listBox;
			multiDef_s* multi;
			profileMultiDef_s* profileMulti;
			editFieldDef_s* editField;
			enumDvarDef_s* enumDvar;
			void* data;
		};

		/* 2333 */
		struct focusItemDef_s
		{
			const char* mouseEnterText;
			const char* mouseExitText;
			const char* mouseEnter;
			const char* mouseExit;
			ItemKeyHandler* onKey;
			focusDefData_t focusTypeData;
		};

		/* 2337 */
		struct ownerDrawDef_s
		{
			ExpressionStatement dataExp;
		};

		/* 2340 */
		struct rectData_s
		{
			ExpressionStatement rectXExp;
			ExpressionStatement rectYExp;
			ExpressionStatement rectWExp;
			ExpressionStatement rectHExp;
		};

		/* 2154 */
		struct animParamsDef_t
		{
			const char* name;
			rectDef_s rectClient;
			float borderSize;
			vec4_t foreColor;
			vec4_t backColor;
			vec4_t borderColor;
			vec4_t outlineColor;
			float textScale;
			float rotation;
			GenericEventHandler* onEvent;
		};

		/* 2341 */
		struct UIAnimInfo
		{
			int animStateCount;
			animParamsDef_t** animStates;
			animParamsDef_t currentAnimState;
			animParamsDef_t nextAnimState;
			int animating;
			int animStartTime;
			int animDuration;
		};

		/* 2685 */
		struct flameTable
		{
			float flameVar_streamChunkGravityStart;
			float flameVar_streamChunkGravityEnd;
			float flameVar_streamChunkMaxSize;
			float flameVar_streamChunkStartSize;
			float flameVar_streamChunkEndSize;
			float flameVar_streamChunkStartSizeRand;
			float flameVar_streamChunkEndSizeRand;
			float flameVar_streamChunkDistScalar;
			float flameVar_streamChunkDistSwayScale;
			float flameVar_streamChunkDistSwayVelMax;
			float flameVar_streamChunkSpeed;
			float flameVar_streamChunkDecel;
			float flameVar_streamChunkVelocityAddScale;
			float flameVar_streamChunkDuration;
			float flameVar_streamChunkDurationScaleMaxVel;
			float flameVar_streamChunkDurationVelScalar;
			float flameVar_streamChunkSizeSpeedScale;
			float flameVar_streamChunkSizeAgeScale;
			float flameVar_streamChunkSpawnFireIntervalStart;
			float flameVar_streamChunkSpawnFireIntervalEnd;
			float flameVar_streamChunkSpawnFireMinLifeFrac;
			float flameVar_streamChunkSpawnFireMaxLifeFrac;
			float flameVar_streamChunkFireMinLifeFrac;
			float flameVar_streamChunkFireMinLifeFracStart;
			float flameVar_streamChunkFireMinLifeFracEnd;
			float flameVar_streamChunkDripsMinLifeFrac;
			float flameVar_streamChunkDripsMinLifeFracStart;
			float flameVar_streamChunkDripsMinLifeFracEnd;
			float flameVar_streamChunkRotationRange;
			float flameVar_streamSizeRandSinWave;
			float flameVar_streamSizeRandCosWave;
			float flameVar_streamDripsChunkInterval;
			float flameVar_streamDripsChunkMinFrac;
			float flameVar_streamDripsChunkRandFrac;
			float flameVar_streamSmokeChunkInterval;
			float flameVar_streamSmokeChunkMinFrac;
			float flameVar_streamSmokeChunkRandFrac;
			float flameVar_streamChunkCullDistSizeFrac;
			float flameVar_streamChunkCullMinLife;
			float flameVar_streamChunkCullMaxLife;
			float flameVar_streamFuelSizeStart;
			float flameVar_streamFuelSizeEnd;
			float flameVar_streamFuelLength;
			float flameVar_streamFuelNumSegments;
			float flameVar_streamFuelAnimLoopTime;
			float flameVar_streamFlameSizeStart;
			float flameVar_streamFlameSizeEnd;
			float flameVar_streamFlameLength;
			float flameVar_streamFlameNumSegments;
			float flameVar_streamFlameAnimLoopTime;
			float flameVar_streamPrimaryLightRadius;
			float flameVar_streamPrimaryLightRadiusFlutter;
			float flameVar_streamPrimaryLightR;
			float flameVar_streamPrimaryLightG;
			float flameVar_streamPrimaryLightB;
			float flameVar_streamPrimaryLightFlutterR;
			float flameVar_streamPrimaryLightFlutterG;
			float flameVar_streamPrimaryLightFlutterB;
			float flameVar_fireLife;
			float flameVar_fireLifeRand;
			float flameVar_fireSpeedScale;
			float flameVar_fireSpeedScaleRand;
			float flameVar_fireVelocityAddZ;
			float flameVar_fireVelocityAddZRand;
			float flameVar_fireVelocityAddSideways;
			float flameVar_fireGravity;
			float flameVar_fireGravityEnd;
			float flameVar_fireMaxRotVel;
			float flameVar_fireFriction;
			float flameVar_fireEndSizeAdd;
			float flameVar_fireStartSizeScale;
			float flameVar_fireEndSizeScale;
			float flameVar_fireBrightness;
			float flameVar_dripsLife;
			float flameVar_dripsLifeRand;
			float flameVar_dripsSpeedScale;
			float flameVar_dripsSpeedScaleRand;
			float flameVar_dripsVelocityAddZ;
			float flameVar_dripsVelocityAddZRand;
			float flameVar_dripsVelocityAddSideways;
			float flameVar_dripsGravity;
			float flameVar_dripsGravityEnd;
			float flameVar_dripsMaxRotVel;
			float flameVar_dripsFriction;
			float flameVar_dripsEndSizeAdd;
			float flameVar_dripsStartSizeScale;
			float flameVar_dripsEndSizeScale;
			float flameVar_dripsBrightness;
			float flameVar_smokeLife;
			float flameVar_smokeLifeRand;
			float flameVar_smokeSpeedScale;
			float flameVar_smokeVelocityAddZ;
			float flameVar_smokeGravity;
			float flameVar_smokeGravityEnd;
			float flameVar_smokeMaxRotation;
			float flameVar_smokeMaxRotVel;
			float flameVar_smokeFriction;
			float flameVar_smokeEndSizeAdd;
			float flameVar_smokeStartSizeAdd;
			float flameVar_smokeOriginSizeOfsZScale;
			float flameVar_smokeOriginOfsZ;
			float flameVar_smokeFadein;
			float flameVar_smokeFadeout;
			float flameVar_smokeMaxAlpha;
			float flameVar_smokeBrightness;
			float flameVar_smokeOriginOffset;
			float flameVar_collisionSpeedScale;
			float flameVar_collisionVolumeScale;
			const char* name;
			Material* fire;
			Material* smoke;
			Material* heat;
			Material* drips;
			Material* streamFuel;
			Material* streamFuel2;
			Material* streamFlame;
			Material* streamFlame2;
			const char* flameOffLoopSound;
			const char* flameIgniteSound;
			const char* flameOnLoopSound;
			const char* flameCooldownSound;
		};

		/* 2205 */
		struct WeaponCamoMaterial
		{
			unsigned __int16 replaceFlags;
			unsigned __int16 numBaseMaterials;
			Material** baseMaterials;
			Material** camoMaterials;
			float shaderConsts[8];
		};

		/* 2791 */
		struct FxElemVec3Range
		{
			vec3_t base;
			vec3_t amplitude;
		};

		/* 2792 */
		struct FxElemVelStateInFrame
		{
			FxElemVec3Range velocity;
			FxElemVec3Range totalDelta;
		};

		/* 2793 */
		const struct FxElemVelStateSample
		{
			FxElemVelStateInFrame local;
			FxElemVelStateInFrame world;
		};

		/* 2794 */
		struct FxElemVisualState
		{
			unsigned __int8 color[4];
			float rotationDelta;
			float rotationTotal;
			float size[2];
			float scale;
		};

		/* 2795 */
		const struct FxElemVisStateSample
		{
			FxElemVisualState base;
			FxElemVisualState amplitude;
		};

		/* 2796 */
		struct FxElemMarkVisuals
		{
			Material* materials[2];
		};

		/* 2804 */
		union $56A7BE17DB462AC7F4A20E9987950A4D
		{
			FxTrailVertex* vertsEA;
			FxTrailVertex* verts;
		};

		/* 2805 */
		union $D31B43AE1E46D24CA4A859818848B5E1
		{
			unsigned __int16* indsEA;
			unsigned __int16* inds;
		};

		/* 2806 */
		struct FxTrailDef
		{
			int scrollTimeMsec;
			int repeatDist;
			int splitDist;
			int vertCount;
			$56A7BE17DB462AC7F4A20E9987950A4D ___u4;
			int indCount;
			$D31B43AE1E46D24CA4A859818848B5E1 ___u6;
		};

		/* 2807 */
		struct FxSpotLightDef
		{
			float fovInnerFraction;
			float startRadius;
			float endRadius;
		};

		/* 3518 */
		struct GlassDef
		{
			const char* name;
			int maxHealth;
			float thickness;
			float minShardSize;
			float maxShardSize;
			float shardLifeProbablility;
			int maxShards;
			Material* pristineMaterial;
			Material* crackedMaterial;
			Material* shardMaterial;
			const char* crackSound;
			const char* shatterShound;
			const char* autoShatterShound;
			const FxEffectDef* crackEffect;
			const FxEffectDef* shatterEffect;
		};

		/* 2673 */
		struct XSurfaceCollisionTree
		{
			vec3_t trans;
			vec3_t scale;
			unsigned int nodeCount;
			XSurfaceCollisionNode* nodes;
			unsigned int leafCount;
			XSurfaceCollisionLeaf* leafs;
		};

		/* 2172 */
		struct PhysGeomInfo
		{
			BrushWrapper* brush;
			int type;
			vec3_t orientation[3];
			vec3_t offset;
			vec3_t halfLengths;
		};

		/* 1 */
		struct _GUID
		{
			unsigned int Data1;
			unsigned __int16 Data2;
			unsigned __int16 Data3;
			unsigned __int8 Data4[8];
		};

		/* 3826 */
		struct GfxLightRegionAxis
		{
			vec3_t dir;
			float midPoint;
			float halfSize;
		};

		/* 2141 */
		struct ScriptCondition
		{
			bool fireOnTrue;
			int constructID;
			int blockID;
			ScriptCondition* next;
		};

		/* 2322 */
		struct textExp_s
		{
			ExpressionStatement textExp;
		};

		/* 2176 */
		struct gameMsgDef_s
		{
			int gameMsgWindowIndex;
			int gameMsgWindowMode;
		};

		/* 2000 */
		struct columnInfo_s
		{
			int elementStyle;
			int maxChars;
			rectDef_s rect;
		};

		/* 2146 */
		struct listBoxDef_s
		{
			int mousePos;
			int cursorPos[1];
			int startPos[1];
			int endPos[1];
			int drawPadding;
			float elementWidth;
			float elementHeight;
			int numColumns;
			float special;
			columnInfo_s columnInfo[16];
			int notselectable;
			int noScrollBars;
			int usePaging;
			vec4_t selectBorder;
			vec4_t disableColor;
			vec4_t focusColor;
			vec4_t elementHighlightColor;
			vec4_t elementBackgroundColor;
			Material* selectIcon;
			Material* backgroundItemListbox;
			Material* highlightTexture;
			int noBlinkingHighlight;
			MenuRow* rows;
			int maxRows;
			int rowCount;
		};

		/* 2005 */
		struct multiDef_s
		{
			const char* dvarList[32];
			const char* dvarStr[32];
			float dvarValue[32];
			int count;
			int actionOnEnterPressOnly;
			int strDef;
		};

		/* 2161 */
		struct profileMultiDef_s
		{
			const char* profileVarList[32];
			const char* profileVarStr[32];
			float profileVarValue[32];
			int count;
			int actionOnEnterPressOnly;
			int strDef;
		};

		/* 2199 */
		struct editFieldDef_s
		{
			int cursorPos[1];
			float minVal;
			float maxVal;
			float defVal;
			float range;
			int maxChars;
			int maxCharsGotoNext;
			int maxPaintChars;
			int paintOffset;
		};

		/* 2159 */
		struct enumDvarDef_s
		{
			const char* enumDvarName;
		};

		/* 2803 */
		struct FxTrailVertex
		{
			vec2_t pos;
			vec2_t normal;
			float texCoord;
		};

		/* 2670 */
		struct XSurfaceCollisionAabb
		{
			unsigned __int16 mins[3];
			unsigned __int16 maxs[3];
		};

		/* 2671 */
		struct XSurfaceCollisionNode
		{
			XSurfaceCollisionAabb aabb;
			unsigned __int16 childBeginIndex;
			unsigned __int16 childCount;
		};

		/* 2672 */
		struct XSurfaceCollisionLeaf
		{
			unsigned __int16 triangleBeginIndex;
		};

		/* 2004 */
		struct BrushWrapper
		{
			vec3_t mins;
			int contents;
			vec3_t maxs;
			unsigned int numsides;
			cbrushside_t* sides;
			int axial_cflags[2][3];
			int axial_sflags[2][3];
			unsigned int numverts;
			vec3_t* verts;
			cplane_s* planes;
		};

		/* 1995 */
		struct MenuRow
		{
			MenuCell* cells;
			char* eventName;
			char* onFocusEventName;
			bool disableArg;
			int status;
			int name;
		};

		/* 1846 */
		struct MenuCell
		{
			int type;
			int maxChars;
			char* stringValue;
		};



		union XAssetHeader
		{
			//XModelPieces* xmodelPieces;
			PhysPreset* physPreset; //TODO
			//PhysConstraints* physConstraints;
			//DestructibleDef* destructibleDef;
			XAnimParts* parts; //TODO
			XModel* model; //TODO
			Material* material; //TODO
			MaterialPixelShader* pixelShader; //TODO
			MaterialVertexShader* vertexShader; //TODO
			MaterialTechniqueSet* techniqueSet; //TODO
			GfxImage* image;
			//SndBank* sound;
			//SndPatch* soundPatch;
			clipMap_t* clipMap; //TODO
			ComWorld* comWorld; //TODO
			GameWorldSp* gameWorldSp;
			GameWorldMp* gameWorldMp; //TODO
			MapEnts* mapEnts;
			//GfxWorld* gfxWorld;
			//GfxLightDef* lightDef;
			Font_s* font;
			//FontIcon* fontIcon;
			//MenuList* menuList;
			//menuDef_t* menu;
			LocalizeEntry* localize; //TODO
			WeaponVariantDef* weapon; //TODO
			//WeaponAttachment* attachment;
			//WeaponAttachmentUnique* attachmentUnique;
			//WeaponCamo* weaponCamo;
			//SndDriverGlobals* sndDriverGlobals;
			FxEffectDef* fx; //TODO
			//FxImpactTable* impactFx;
			RawFile* rawfile;
			StringTable* stringTable;
			LeaderboardDef* leaderboardDef;
			XGlobals* xGlobals; //No equivalent in IW5 //TODO
			//ddlRoot_t* ddlRoot;
			//Glasses* glasses;
			//TextureList* textureList;
			//EmblemSet* emblemSet;
			ScriptParseTree* scriptParseTree;
			KeyValuePairs* keyValuePairs; //No equivalent in IW5 //TODO
			VehicleDef* vehicleDef; //TODO
			MemoryBlock* memoryBlock; //No equivalent in IW5 //TODO
			AddonMapEnts* addonMapEnts;
			//TracerDef* tracerDef;
			//SkinnedVertsDef* skinnedVertsDef;
			Qdb* qdb; //No equivalent in IW5 //Unused
			Slug* slug; //No equivalent in IW5 //Unused
			FootstepTableDef* footstepTableDef; //No equivalent in IW5
			//FootstepFXTableDef* footstepFXTableDef;
			ZBarrierDef* zbarrierDef; //No equivalent in IW5 //TODO
			void* data;
		};
	}
}
