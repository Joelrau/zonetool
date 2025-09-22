#include "stdafx.hpp"
#include "../Include.hpp"

#include "Weapon.hpp"

namespace ZoneTool::IW5
{
	namespace H1Converter
	{
		// Overloads
#define COPY_FIELD_1(_field) \
    h1_asset->_field = asset->_field;

#define COPY_FIELD_2(_field, _field2) \
    h1_asset->_field = asset->_field2;

// Chooser + forced expansion (MSVC needs this)
#define _COPY_FIELD_GET_MACRO(_1, _2, NAME, ...) NAME
#define _EXPAND(x) x
#define COPY_FIELD(...) \
    _EXPAND(_COPY_FIELD_GET_MACRO(__VA_ARGS__, COPY_FIELD_2, COPY_FIELD_1)(__VA_ARGS__))


// Helper: destination field's plain type (no ref, no cv)
#define _FIELD_TYPE(_field) std::remove_cv_t<std::remove_reference_t<decltype(h1_asset->_field)>>

// Overloads
#define COPY_FIELD_CAST_1(_field) \
    (h1_asset->_field) = static_cast<_FIELD_TYPE(_field)>((asset->_field))

#define COPY_FIELD_CAST_2(_field, _field2) \
    (h1_asset->_field) = static_cast<_FIELD_TYPE(_field)>((asset->_field2))

// Chooser + forced expansion (MSVC friendly)
#define _COPY_FIELD_CAST_GET_MACRO(_1, _2, NAME, ...) NAME
#define _EXPAND(x) x
#define COPY_FIELD_CAST(...) \
    _EXPAND(_COPY_FIELD_CAST_GET_MACRO(__VA_ARGS__, COPY_FIELD_CAST_2, COPY_FIELD_CAST_1)(__VA_ARGS__))


#define REINTERPRET_CAST_1(_field) \
	h1_asset->_field = reinterpret_cast<decltype(h1_asset->_field)>(asset->_field);

#define REINTERPRET_CAST_2(_field, _field2) \
	h1_asset->_field = reinterpret_cast<decltype(h1_asset->_field)>(asset->_field2);

		// Chooser + forced expansion (MSVC needs this)
#define _REINTERPRET_CAST_GET_MACRO(_1, _2, NAME, ...) NAME
#define _EXPAND(x) x
#define REINTERPRET_CAST(...) \
    _EXPAND(_REINTERPRET_CAST_GET_MACRO(__VA_ARGS__, REINTERPRET_CAST_2, REINTERPRET_CAST_1)(__VA_ARGS__))


#define COPY_ARR(_field, _field2) \
	static_assert(sizeof(h1_asset->_field) == sizeof(asset->_field2)); \
	std::memcpy(&h1_asset->_field, &asset->_field2, sizeof(h1_asset->_field));


#define MEMBER_SPAN_SIZE_T(T, first, last) \
    (offsetof(T, last) + sizeof(((T*)0)->last) - offsetof(T, first))

		H1::weapAnimFiles_t getAnim(weapAnimFiles_t iw5_anim)
		{
			static std::unordered_map< weapAnimFiles_t, ZoneTool::H1::weapAnimFiles_t> mapped_anims =
			{
				{ WEAP_ANIM_ROOT, H1::WEAP_ANIM_ROOT },
				{ WEAP_ANIM_IDLE, H1::WEAP_ANIM_IDLE },
				{ WEAP_ANIM_EMPTY_IDLE, H1::WEAP_ANIM_EMPTY_IDLE },
				{ WEAP_ANIM_FIRE, H1::WEAP_ANIM_FIRE },
				{ WEAP_ANIM_HOLD_FIRE, H1::WEAP_ANIM_HOLD_FIRE },
				{ WEAP_ANIM_LASTSHOT, H1::WEAP_ANIM_LASTSHOT },
				{ WEAP_ANIM_RECHAMBER, H1::WEAP_ANIM_RECHAMBER },
				{ WEAP_ANIM_MELEE, H1::WEAP_ANIM_MELEE_SWIPE },
				{ WEAP_ANIM_MELEE_CHARGE, H1::WEAP_ANIM_MELEE_FATAL },
				{ WEAP_ANIM_RELOAD, H1::WEAP_ANIM_RELOAD },
				{ WEAP_ANIM_RELOAD_EMPTY, H1::WEAP_ANIM_RELOAD_EMPTY },
				{ WEAP_ANIM_RELOAD_START, H1::WEAP_ANIM_RELOAD_START },
				{ WEAP_ANIM_RELOAD_END, H1::WEAP_ANIM_RELOAD_END },
				{ WEAP_ANIM_RAISE, H1::WEAP_ANIM_RAISE },
				{ WEAP_ANIM_FIRST_RAISE, H1::WEAP_ANIM_FIRST_RAISE },
				{ WEAP_ANIM_BREACH_RAISE, H1::WEAP_ANIM_BREACH_RAISE },
				{ WEAP_ANIM_DROP, H1::WEAP_ANIM_DROP },
				{ WEAP_ANIM_ALT_RAISE, H1::WEAP_ANIM_ALT_RAISE },
				{ WEAP_ANIM_ALT_DROP, H1::WEAP_ANIM_ALT_DROP },
				{ WEAP_ANIM_QUICK_RAISE, H1::WEAP_ANIM_QUICK_RAISE },
				{ WEAP_ANIM_QUICK_DROP, H1::WEAP_ANIM_QUICK_DROP },
				{ WEAP_ANIM_EMPTY_RAISE, H1::WEAP_ANIM_EMPTY_RAISE },
				{ WEAP_ANIM_EMPTY_DROP, H1::WEAP_ANIM_EMPTY_DROP },
				{ WEAP_ANIM_SPRINT_IN, H1::WEAP_ANIM_SPRINT_IN },
				{ WEAP_ANIM_SPRINT_LOOP, H1::WEAP_ANIM_SPRINT_LOOP },
				{ WEAP_ANIM_SPRINT_OUT, H1::WEAP_ANIM_SPRINT_OUT },
				{ WEAP_ANIM_STUNNED_START, H1::WEAP_ANIM_STUNNED_START },
				{ WEAP_ANIM_STUNNED_LOOP, H1::WEAP_ANIM_STUNNED_LOOP },
				{ WEAP_ANIM_STUNNED_END, H1::WEAP_ANIM_STUNNED_END },
				{ WEAP_ANIM_DETONATE, H1::WEAP_ANIM_DETONATE },
				{ WEAP_ANIM_NIGHTVISION_WEAR, H1::WEAP_ANIM_NIGHTVISION_WEAR },
				{ WEAP_ANIM_NIGHTVISION_REMOVE, H1::WEAP_ANIM_NIGHTVISION_REMOVE },
				{ WEAP_ANIM_ADS_FIRE, H1::WEAP_ANIM_ADS_FIRE },
				{ WEAP_ANIM_ADS_LASTSHOT, H1::WEAP_ANIM_ADS_LASTSHOT },
				{ WEAP_ANIM_ADS_RECHAMBER, H1::WEAP_ANIM_ADS_RECHAMBER },
				{ WEAP_ANIM_BLAST_FRONT, H1::WEAP_ANIM_BLAST_FRONT },
				{ WEAP_ANIM_BLAST_RIGHT, H1::WEAP_ANIM_BLAST_RIGHT },
				{ WEAP_ANIM_BLAST_BACK, H1::WEAP_ANIM_BLAST_BACK },
				{ WEAP_ANIM_BLAST_LEFT, H1::WEAP_ANIM_BLAST_LEFT },
				{ WEAP_ANIM_ADS_UP, H1::WEAP_ANIM_ADS_UP },
				{ WEAP_ANIM_ADS_DOWN, H1::WEAP_ANIM_ADS_DOWN },
				{ WEAP_ALT_ANIM_ADJUST, H1::WEAP_ALT_ANIM_ADJUST },
			};

			if (mapped_anims.contains(iw5_anim))
			{
				return mapped_anims[iw5_anim];
			}

			__debugbreak();
			return H1::WEAP_ANIM_INVALID;
		}

		void convertAnims(H1::XAnimParts PTR64 PTR64 h1_anims, const char** anims, allocator& mem)
		{
			for (auto i = 0; i < WEAP_ANIM_COUNT; i++)
			{
				const auto iw5_anim = static_cast<weapAnimFiles_t>(i);
				const auto h1_anim = getAnim(iw5_anim);

				if (anims[iw5_anim])
				{
					h1_anims[h1_anim] = mem.manual_allocate<H1::XAnimParts>(sizeof(const char PTR64));
					h1_anims[h1_anim]->name = anims[iw5_anim];
				}
			}
		}

		H1::PhysPreset* getPhysPreset(weapClass_t weapon_class, allocator& mem)
		{
			std::string phys_preset_name = "default";
			switch (weapon_class)
			{
				case WEAPCLASS_PISTOL:
					phys_preset_name = "weapon_light";
					break;
				case WEAPCLASS_RIFLE:
				case WEAPCLASS_SNIPER:
				case WEAPCLASS_MG:
				case WEAPCLASS_SMG:
				case WEAPCLASS_SPREAD:
				case WEAPCLASS_GRENADE:
				case WEAPCLASS_TURRET:
					phys_preset_name = "weapon_heavy";
					break;
				case WEAPCLASS_THROWINGKNIFE:
					phys_preset_name = "weapon_clip_empty";
					break;
				default:
					phys_preset_name = "default";
					break;
			}

			auto* asset = mem.manual_allocate<H1::PhysPreset>(sizeof(const char PTR64));
			asset->name = mem.duplicate_string(phys_preset_name);
			return asset;
		}

		H1::playerAnimType_t convertPlayerAnim(playerAnimType_t animType)
		{
			static std::unordered_map< playerAnimType_t, ZoneTool::H1::playerAnimType_t> mapped_anims =
			{
				{ PLAYERANIMTYPE_NONE, H1::PLAYERANIMTYPE_NONE },
				{ PLAYERANIMTYPE_OTHER, H1::PLAYERANIMTYPE_OTHER },
				{ PLAYERANIMTYPE_PISTOL, H1::PLAYERANIMTYPE_PISTOL },
				{ PLAYERANIMTYPE_SMG, H1::PLAYERANIMTYPE_SMG },
				{ PLAYERANIMTYPE_AUTORIFLE, H1::PLAYERANIMTYPE_AUTORIFLE },
				{ PLAYERANIMTYPE_MG, H1::PLAYERANIMTYPE_AUTORIFLE },
				{ PLAYERANIMTYPE_SNIPER, H1::PLAYERANIMTYPE_SNIPER },
				{ PLAYERANIMTYPE_ROCKETLAUNCHER, H1::PLAYERANIMTYPE_ROCKET_LAUNCHER },
				{ PLAYERANIMTYPE_EXPLOSIVE, H1::PLAYERANIMTYPE_GRENADE },
				{ PLAYERANIMTYPE_GRENADE, H1::PLAYERANIMTYPE_GRENADE },
				{ PLAYERANIMTYPE_TURRET, H1::PLAYERANIMTYPE_HOLD },
				{ PLAYERANIMTYPE_C4, H1::PLAYERANIMTYPE_HOLD },
				{ PLAYERANIMTYPE_M203, H1::PLAYERANIMTYPE_M203 },
				{ PLAYERANIMTYPE_HOLD, H1::PLAYERANIMTYPE_HOLD },
				{ PLAYERANIMTYPE_BRIEFCASE, H1::PLAYERANIMTYPE_BRIEFCASE },
				{ PLAYERANIMTYPE_RIOTSHIELD, H1::PLAYERANIMTYPE_RIOTSHIELD },
				{ PLAYERANIMTYPE_LAPTOP, H1::PLAYERANIMTYPE_LAPTOP },
				{ PLAYERANIMTYPE_THROWINGKNIFE, H1::PLAYERANIMTYPE_THROWINGKNIFE },
			};

			if (mapped_anims.contains(animType))
			{
				return mapped_anims[animType];
			}

			__debugbreak();
			return H1::PLAYERANIMTYPE_NONE;
		}

		H1::OffhandClass convertOffhandClass(OffhandClass offhandClass)
		{
			static std::unordered_map< OffhandClass, ZoneTool::H1::OffhandClass> mapped =
			{
				{ OFFHAND_CLASS_NONE, H1::OFFHAND_CLASS_NONE },
				{ OFFHAND_CLASS_FRAG_GRENADE, H1::OFFHAND_CLASS_FRAG_GRENADE },
				{ OFFHAND_CLASS_SMOKE_GRENADE, H1::OFFHAND_CLASS_SMOKE_GRENADE },
				{ OFFHAND_CLASS_FLASH_GRENADE, H1::OFFHAND_CLASS_FLASH_GRENADE },
				{ OFFHAND_CLASS_THROWINGKNIFE, H1::OFFHAND_CLASS_THROWINGKNIFE },
				{ OFFHAND_CLASS_OTHER, H1::OFFHAND_CLASS_OTHER },
			};

			if (mapped.contains(offhandClass))
			{
				return mapped[offhandClass];
			}

			__debugbreak();
			return H1::OFFHAND_CLASS_NONE;
		}

		void copyStateTimer(H1::StateTimers* h1_asset, StateTimers* asset, WeaponCompleteDef* def, bool akimbo)
		{
			COPY_FIELD(fireDelay, iFireDelay);
			COPY_FIELD(meleeDelay, iMeleeDelay);
			COPY_FIELD(meleeChargeDelay, meleeChargeDelay);
			COPY_FIELD(detonateDelay, iDetonateDelay);
			h1_asset->fireTime = akimbo ? def->iFireTimeAkimbo : def->iFireTime;

			COPY_FIELD(rechamberTime, iRechamberTime);
			COPY_FIELD(rechamberTimeOneHanded, rechamberTimeOneHanded);
			COPY_FIELD(rechamberBoltTime, iRechamberBoltTime);
			COPY_FIELD(holdFireTime, iHoldFireTime);
			h1_asset->grenadePrimeReadyToThrowTime = def->weapDef->weapClass == WEAPCLASS_GRENADE ? 100 : 0;

			COPY_FIELD(detonateTime, iDetonateTime);
			COPY_FIELD(meleeTime, iMeleeTime);
			COPY_FIELD(meleeChargeTime, meleeChargeTime);

			COPY_FIELD(reloadTime, iReloadTime);
			COPY_FIELD(reloadShowRocketTime, reloadShowRocketTime);
			COPY_FIELD(reloadEmptyTime, iReloadEmptyTime);
			COPY_FIELD(reloadAddTime, iReloadAddTime);
			h1_asset->reloadEmptyAddTime = 0;

			COPY_FIELD(reloadStartTime, iReloadStartTime);
			COPY_FIELD(reloadStartAddTime, iReloadStartAddTime);
			COPY_FIELD(reloadEndTime, iReloadEndTime);
			h1_asset->reloadTimeDualWield = 0;
			h1_asset->reloadAddTimeDualWield = 0;
			h1_asset->reloadEmptyDualMag = 2000;
			h1_asset->reloadEmptyAddTimeDualMag = 0;
			h1_asset->speedReloadTime = def->weapDef->weapClass == WEAPCLASS_GRENADE ? 0 : 2000;
			h1_asset->speedReloadAddTime = 0;

			COPY_FIELD(dropTime, iDropTime);
			COPY_FIELD(raiseTime, iRaiseTime);
			COPY_FIELD(altDropTime, iAltDropTime);
			h1_asset->altRaiseTime = akimbo ? def->iAltRaiseTime : def->iAltRaiseTimeAkimbo;

			COPY_FIELD(quickDropTime, quickDropTime);
			COPY_FIELD(quickRaiseTime, quickRaiseTime);
			h1_asset->firstRaiseTime = akimbo ? def->iFirstRaiseTime : def->iFirstRaiseTimeAkimbo;
			COPY_FIELD(breachRaiseTime, iBreachRaiseTime);
			COPY_FIELD(emptyRaiseTime, iEmptyRaiseTime);
			COPY_FIELD(emptyDropTime, iEmptyDropTime);

			COPY_FIELD(sprintInTime, sprintInTime);
			COPY_FIELD(sprintLoopTime, sprintLoopTime);
			COPY_FIELD(sprintOutTime, sprintOutTime);

			COPY_FIELD(stunnedTimeBegin, stunnedTimeBegin);
			COPY_FIELD(stunnedTimeLoop, stunnedTimeLoop);
			COPY_FIELD(stunnedTimeEnd, stunnedTimeEnd);

			COPY_FIELD(nightVisionWearTime, nightVisionWearTime);
			COPY_FIELD(nightVisionWearTimeFadeOutEnd, nightVisionWearTimeFadeOutEnd);
			COPY_FIELD(nightVisionWearTimePowerUp, nightVisionWearTimePowerUp);
			COPY_FIELD(nightVisionRemoveTime, nightVisionRemoveTime);
			COPY_FIELD(nightVisionRemoveTimePowerDown, nightVisionRemoveTimePowerDown);
			COPY_FIELD(nightVisionRemoveTimeFadeInStart, nightVisionRemoveTimeFadeInStart);

			COPY_FIELD(aiFuseTime, aiFuseTime);
			COPY_FIELD(fuseTime, fuseTime);
			h1_asset->missileTime = def->weapDef->weapClass == WEAPCLASS_ROCKETLAUNCHER ? 4000 : 0;
			h1_asset->primeTime = 0;
			h1_asset->bHoldFullPrime = false;

			COPY_FIELD(blastFrontTime, blastFrontTime);
			COPY_FIELD(blastRightTime, blastRightTime);
			COPY_FIELD(blastBackTime, blastBackTime);
			COPY_FIELD(blastLeftTime, blastLeftTime);

			h1_asset->slideInTime = 500;
			h1_asset->slideLoopTime = 500;
			h1_asset->slideOutTime = 500;

			h1_asset->highJumpInTime = 600;
			h1_asset->highJumpDropInTime = 100;
			h1_asset->highJumpDropLoopTime = 1000;
			h1_asset->highJumpDropLandTime = 1000;

			h1_asset->dodgeTime = 500;
			h1_asset->landDipTime = 600;

			h1_asset->hybridSightInTime = 0;
			h1_asset->hybridSightOutTime = 0;

			h1_asset->offhandSwitchTime = 0;

			h1_asset->heatCooldownInTime = 500;
			h1_asset->heatCooldownOutTime = 500;
			h1_asset->heatCooldownOutReadyTime = 500;

			h1_asset->overheatOutTime = 500;
			h1_asset->overheatOutReadyTime = 500;
		}

		bool isGun(weapClass_t weaponClass)
		{
			if (weaponClass <= WEAPCLASS_PISTOL || weaponClass == WEAPCLASS_ROCKETLAUNCHER) // is a gun
			{
				return true;
			}
			return false;
		}

		bool isExplosive(weapClass_t weaponClass)
		{
			if (weaponClass == WEAPCLASS_GRENADE || weaponClass == WEAPCLASS_ROCKETLAUNCHER) // is explosive
			{
				return true;
			}
			return false;
		}

		void convertSurfaceSounds(H1::snd_alias_list_t PTR64 PTR64 h1_sounds, SndAliasCustom* sounds, allocator& mem)
		{
			static std::unordered_map<H1::materialSurfType_t, materialSurfType_t> mapped =
			{
				{ H1::SURF_TYPE_DEFAULT, SURF_TYPE_DEFAULT },
				{ H1::SURF_TYPE_BARK, SURF_TYPE_BARK },
				{ H1::SURF_TYPE_BRICK, SURF_TYPE_BRICK },
				{ H1::SURF_TYPE_CARPET, SURF_TYPE_CARPET },
				{ H1::SURF_TYPE_CLOTH, SURF_TYPE_CLOTH },
				{ H1::SURF_TYPE_CONCRETE, SURF_TYPE_CONCRETE },
				{ H1::SURF_TYPE_DIRT, SURF_TYPE_DIRT },
				{ H1::SURF_TYPE_FLESH, SURF_TYPE_FLESH },
				{ H1::SURF_TYPE_FOLIAGE_DEBRIS, SURF_TYPE_FOLIAGE },
				{ H1::SURF_TYPE_GLASS, SURF_TYPE_GLASS },
				{ H1::SURF_TYPE_GRASS, SURF_TYPE_GRASS },
				{ H1::SURF_TYPE_GRAVEL, SURF_TYPE_GRAVEL },
				{ H1::SURF_TYPE_ICE, SURF_TYPE_ICE },
				{ H1::SURF_TYPE_METAL_SOLID, SURF_TYPE_METAL },
				{ H1::SURF_TYPE_METAL_GRATE, SURF_TYPE_METAL },
				{ H1::SURF_TYPE_MUD, SURF_TYPE_MUD },
				{ H1::SURF_TYPE_PAPER, SURF_TYPE_PAPER },
				{ H1::SURF_TYPE_PLASTER, SURF_TYPE_PLASTER },
				{ H1::SURF_TYPE_ROCK, SURF_TYPE_ROCK },
				{ H1::SURF_TYPE_SAND, SURF_TYPE_SAND },
				{ H1::SURF_TYPE_SNOW, SURF_TYPE_SNOW },
				{ H1::SURF_TYPE_WATER_WAIST, SURF_TYPE_WATER },
				{ H1::SURF_TYPE_WOOD_SOLID, SURF_TYPE_WOOD },
				{ H1::SURF_TYPE_ASPHALT, SURF_TYPE_ASPHALT },
				{ H1::SURF_TYPE_CERAMIC, SURF_TYPE_CERAMIC },
				{ H1::SURF_TYPE_PLASTIC_SOLID, SURF_TYPE_PLASTIC },
				{ H1::SURF_TYPE_RUBBER, SURF_TYPE_RUBBER },
				{ H1::SURF_TYPE_FRUIT, SURF_TYPE_FRUIT },
				{ H1::SURF_TYPE_PAINTEDMETAL, SURF_TYPE_PAINTED_METAL },
				{ H1::SURF_TYPE_RIOTSHIELD, SURF_TYPE_RIOT_SHIELD },
				{ H1::SURF_TYPE_SLUSH, SURF_TYPE_SLUSH },
				{ H1::SURF_TYPE_ASPHALT_WET, SURF_TYPE_ASPHALT },
				{ H1::SURF_TYPE_ASPHALT_DEBRIS, SURF_TYPE_ASPHALT },
				{ H1::SURF_TYPE_CONCRETE_WET, SURF_TYPE_CONCRETE },
				{ H1::SURF_TYPE_CONCRETE_DEBRIS, SURF_TYPE_CONCRETE },
				{ H1::SURF_TYPE_FOLIAGE_VEGETATION, SURF_TYPE_FOLIAGE },
				{ H1::SURF_TYPE_FOLIAGE_LEAVES, SURF_TYPE_FOLIAGE },
				{ H1::SURF_TYPE_GRASS_TALL, SURF_TYPE_GRASS },
				{ H1::SURF_TYPE_METAL_HOLLOW, SURF_TYPE_METAL },
				{ H1::SURF_TYPE_METAL_VEHICLE, SURF_TYPE_METAL },
				{ H1::SURF_TYPE_METAL_THIN, SURF_TYPE_METAL },
				{ H1::SURF_TYPE_METAL_WET, SURF_TYPE_METAL },
				{ H1::SURF_TYPE_METAL_DEBRIS, SURF_TYPE_METAL },
				{ H1::SURF_TYPE_PLASTIC_HOLLOW, SURF_TYPE_PLASTIC },
				{ H1::SURF_TYPE_PLASTIC_TARP, SURF_TYPE_PLASTIC },
				{ H1::SURF_TYPE_ROCK_WET, SURF_TYPE_ROCK },
				{ H1::SURF_TYPE_ROCK_DEBRIS, SURF_TYPE_ROCK },
				{ H1::SURF_TYPE_WATER_ANKLE, SURF_TYPE_WATER },
				{ H1::SURF_TYPE_WATER_KNEE, SURF_TYPE_WATER },
				{ H1::SURF_TYPE_WOOD_HOLLOW, SURF_TYPE_WOOD },
				{ H1::SURF_TYPE_WOOD_WET, SURF_TYPE_WOOD },
				{ H1::SURF_TYPE_WOOD_DEBRIS, SURF_TYPE_WOOD },
				{ H1::SURF_TYPE_CUSHION, SURF_TYPE_CUSHION },
			};

			for (auto& [h1_type, iw5_type] : mapped)
			{
				if (sounds[iw5_type].sound)
				{
					h1_sounds[h1_type] = mem.manual_allocate<H1::snd_alias_list_t>(sizeof(const char PTR64));
					h1_sounds[h1_type]->name = mem.duplicate_string(sounds[iw5_type].sound->aliasName);
				}
			}
		}

		H1::WeaponDef* GenerateWeaponDef(WeaponCompleteDef* asset, allocator& mem)
		{
			// allocate H1 WeaponDef structure
			const auto h1_asset = mem.allocate<H1::WeaponDef>();

			COPY_FIELD(szInternalName);
			COPY_FIELD(szDisplayName);
			COPY_FIELD(szAltWeaponName);
			if (asset->weapDef->gunXModel)
			{
				h1_asset->gunModel = mem.allocate<H1::XModel PTR64>(2);
				h1_asset->gunModel[0] = reinterpret_cast<H1::XModel*>(asset->weapDef->gunXModel[0]);
				h1_asset->gunModel[1] = nullptr;
			}
			REINTERPRET_CAST(handModel, weapDef->handXModel);
			// persistentArmXModel
			// reticleViewModels
			// lobWorldModelName
			if (asset->weapDef->szXAnimsRightHanded)
			{
				h1_asset->szXAnimsRightHanded = mem.allocate<H1::XAnimParts PTR64>(H1::NUM_WEAP_ANIMS);
				convertAnims(h1_asset->szXAnimsRightHanded, asset->weapDef->szXAnimsRightHanded, mem);
			}
			if (asset->weapDef->szXAnimsLeftHanded)
			{
				h1_asset->szXAnimsLeftHanded = mem.allocate<H1::XAnimParts PTR64>(H1::NUM_WEAP_ANIMS);
				convertAnims(h1_asset->szXAnimsLeftHanded, asset->weapDef->szXAnimsLeftHanded, mem);
			}
			REINTERPRET_CAST(hideTags);

			// attachments
			const auto getScopes = [](WeaponCompleteDef* def, std::vector<WeaponAttachment*>& attachmentsOut)
			{
				if (def->scopes)
				{
					for (auto i = 0; i < 6; i++)
					{
						if (def->scopes[i])
						{
							attachmentsOut.push_back(def->scopes[i]);
						}
					}
				}
			};
			const auto getUnderBarrels = [](WeaponCompleteDef* def, std::vector<WeaponAttachment*>& attachmentsOut)
			{
				if (def->underBarrels)
				{
					for (auto i = 0; i < 3; i++)
					{
						if (def->underBarrels[i])
						{
							attachmentsOut.push_back(def->underBarrels[i]);
						}
					}
				}
			};
			const auto getOthers = [](WeaponCompleteDef* def, std::vector<WeaponAttachment*>& attachmentsOut)
			{
				if (def->others)
				{
					for (auto i = 0; i < 4; i++)
					{
						if (def->others[i])
						{
							attachmentsOut.push_back(def->others[i]);
						}
					}
				}
			};
			const auto getAllAttachments = [&](WeaponCompleteDef* def, std::vector<WeaponAttachment*>& attachmentsOut)
			{
				getScopes(def, attachmentsOut);
				getUnderBarrels(def, attachmentsOut);
				getOthers(def, attachmentsOut);
			};
			const auto getNumScopes = [&](WeaponCompleteDef* def)
			{
				std::vector <WeaponAttachment*> attachments;
				getScopes(def, attachments);
				return attachments.size();
			};
			const auto getNumUnderBarrels = [&](WeaponCompleteDef* def)
			{
				std::vector <WeaponAttachment*> attachments;
				getUnderBarrels(def, attachments);
				return attachments.size();
			};
			const auto getNumOthers = [&](WeaponCompleteDef* def)
			{
				std::vector <WeaponAttachment*> attachments;
				getOthers(def, attachments);
				return attachments.size();
			};

			std::vector<WeaponAttachment*> attachments;
			getAllAttachments(asset, attachments);
			assert(attachments.size() < 0xFF);

			h1_asset->numAttachments = static_cast<unsigned char>(attachments.size());
			h1_asset->attachments = mem.allocate<H1::WeaponAttachment PTR64>(h1_asset->numAttachments);
			for (auto i = 0; i < h1_asset->numAttachments; i++)
			{
				h1_asset->attachments[i] = mem.manual_allocate<H1::WeaponAttachment>(sizeof(const char PTR64));
				h1_asset->attachments[i]->szInternalName = mem.duplicate_string(attachments[i]->szInternalName);
			}

			if (asset->szXAnims)
			{
				h1_asset->szXAnims = mem.allocate<H1::XAnimParts PTR64>(H1::NUM_WEAP_ANIMS);
				convertAnims(h1_asset->szXAnims, asset->szXAnims, mem);
			}

			const auto convertAttachmentValue = [&](WeaponAttachmentCombination value) -> unsigned char
			{
				if (value.fields == 0)
				{
					return 0; // unused
				}

				if (value.scope)
				{
					return value.scope;
				}
				else if (value.underBarrel)
				{
					return value.underBarrel + getNumScopes(asset);
				}
				else if (value.other)
				{
					return value.other + getNumScopes(asset) + getNumUnderBarrels(asset);
				}

				__debugbreak();
				return 0;
			};

			h1_asset->numAnimOverrides = asset->numAnimOverrides;
			h1_asset->animOverrides = mem.allocate<H1::AnimOverrideEntry>(h1_asset->numAnimOverrides);
			for (auto i = 0; i < h1_asset->numAnimOverrides; i++)
			{
				h1_asset->animOverrides[i].animHand = 0;

				h1_asset->animOverrides[i].attachment1 = convertAttachmentValue(asset->animOverrides[i].attachment1);
				h1_asset->animOverrides[i].attachment2 = convertAttachmentValue(asset->animOverrides[i].attachment2);

				const auto anim = getAnim(asset->animOverrides[i].animTreeType);
				assert(anim != H1::WEAP_ANIM_INVALID);
				h1_asset->animOverrides[i].animTreeType = static_cast<unsigned char>(anim);

				if (asset->animOverrides[i].overrideAnim)
				{
					h1_asset->animOverrides[i].overrideAnim = mem.allocate<H1::XAnimParts>(sizeof(const char PTR64));
					h1_asset->animOverrides[i].overrideAnim->name = asset->animOverrides[i].overrideAnim;
				}

				if (asset->animOverrides[i].altmodeAnim)
				{
					h1_asset->animOverrides[i].altmodeAnim = mem.allocate<H1::XAnimParts>(sizeof(const char PTR64));
					h1_asset->animOverrides[i].altmodeAnim->name = asset->animOverrides[i].altmodeAnim;
				}

				COPY_FIELD_CAST(animOverrides[i].animTime);
				COPY_FIELD_CAST(animOverrides[i].altTime);
			}

			h1_asset->numSoundOverrides = asset->numSoundOverrides;
			h1_asset->soundOverrides = mem.allocate<H1::SoundOverrideEntry>(h1_asset->numSoundOverrides);
			for (auto i = 0; i < h1_asset->numSoundOverrides; i++)
			{
				h1_asset->soundOverrides[i].attachment1 = convertAttachmentValue(asset->soundOverrides[i].attachment1);
				h1_asset->soundOverrides[i].attachment2 = convertAttachmentValue(asset->soundOverrides[i].attachment2);
				COPY_FIELD_CAST(soundOverrides[i].soundType);
				REINTERPRET_CAST(soundOverrides[i].overrideSound, soundOverrides[i].overrideSound.sound);
				REINTERPRET_CAST(soundOverrides[i].altmodeSound, soundOverrides[i].altmodeSound.sound);
			}

			h1_asset->numFXOverrides = asset->numFxOverrides;
			h1_asset->fxOverrides = mem.allocate<H1::FXOverrideEntry>(h1_asset->numFXOverrides);
			for (auto i = 0; i < h1_asset->numFXOverrides; i++)
			{
				h1_asset->fxOverrides[i].attachment1 = convertAttachmentValue(asset->fxOverrides[i].attachment1);
				h1_asset->fxOverrides[i].attachment2 = convertAttachmentValue(asset->fxOverrides[i].attachment2);
				COPY_FIELD_CAST(fxOverrides[i].fxType);
				REINTERPRET_CAST(fxOverrides[i].overrideFX, fxOverrides[i].overrideFx);
				REINTERPRET_CAST(fxOverrides[i].altmodeFX, fxOverrides[i].altmodeFx);
			}

			h1_asset->numReloadStateTimerOverrides = asset->numReloadStateTimerOverrides;
			h1_asset->reloadOverrides = mem.allocate<H1::ReloadStateTimerEntry>(h1_asset->numReloadStateTimerOverrides);
			for (auto i = 0; i < h1_asset->numReloadStateTimerOverrides; i++)
			{
				h1_asset->reloadOverrides[i].attachment = convertAttachmentValue(asset->reloadOverrides[i].attachment);
				COPY_FIELD(reloadOverrides[i].reloadAddTime);
				h1_asset->reloadOverrides[i].reloadEmptyAddTime = 0;
				COPY_FIELD(reloadOverrides[i].reloadStartAddTime);
			}

			h1_asset->numNotetrackOverrides = asset->numNotetrackOverrides;
			h1_asset->notetrackOverrides = mem.allocate<H1::NoteTrackToSoundEntry>(h1_asset->numNotetrackOverrides);
			for (auto i = 0; i < h1_asset->numNotetrackOverrides; i++)
			{
				h1_asset->notetrackOverrides[i].attachment = convertAttachmentValue(asset->notetrackOverrides[i].attachment);
				h1_asset->notetrackOverrides[i].notetrackSoundMapKeys = mem.allocate<H1::scr_string_t>(36);
				h1_asset->notetrackOverrides[i].notetrackSoundMapValues = mem.allocate<H1::scr_string_t>(36);
				for (auto j = 0; j < 36; j++)
				{
					h1_asset->notetrackOverrides[i].notetrackSoundMapKeys[j] = asset->notetrackOverrides[i].notetrackSoundMapKeys[j];
					h1_asset->notetrackOverrides[i].notetrackSoundMapValues[j] = asset->notetrackOverrides[i].notetrackSoundMapValues[j];
				}
				//memcpy(h1_asset->notetrackOverrides[i].notetrackSoundMapKeys, asset->notetrackOverrides[i].notetrackSoundMapKeys, sizeof(scr_string_t) * 24);
				//memcpy(h1_asset->notetrackOverrides[i].notetrackSoundMapValues, asset->notetrackOverrides[i].notetrackSoundMapValues, sizeof(scr_string_t) * 24);
			}

			// animOverrides
			// soundOverrides
			// fxOverrides
			// reloadOverrides
			// notetrackOverrides
			h1_asset->notetrackSoundMapKeys = mem.allocate<H1::scr_string_t>(36);
			h1_asset->notetrackSoundMapValues = mem.allocate<H1::scr_string_t>(36);
			for (auto i = 0; i < 36; i++)
			{
				h1_asset->notetrackSoundMapKeys[i] = asset->weapDef->notetrackSoundMapKeys[i];
				h1_asset->notetrackSoundMapValues[i] = asset->weapDef->notetrackSoundMapValues[i];
			}
			//memcpy(h1_asset->notetrackSoundMapKeys, asset->weapDef->notetrackSoundMapKeys, sizeof(scr_string_t) * 24);
			//memcpy(h1_asset->notetrackSoundMapValues, asset->weapDef->notetrackSoundMapValues, sizeof(scr_string_t) * 24);
			h1_asset->notetrackRumbleMapKeys = mem.allocate<H1::scr_string_t>(16);
			h1_asset->notetrackRumbleMapValues = mem.allocate<H1::scr_string_t>(16);
			for (auto i = 0; i < 16; i++)
			{
				h1_asset->notetrackRumbleMapKeys[i] = asset->weapDef->notetrackRumbleMapKeys[i];
				h1_asset->notetrackRumbleMapValues[i] = asset->weapDef->notetrackRumbleMapValues[i];
			}
			//memcpy(h1_asset->notetrackRumbleMapKeys, asset->weapDef->notetrackRumbleMapKeys, sizeof(scr_string_t) * 16);
			//memcpy(h1_asset->notetrackRumbleMapValues, asset->weapDef->notetrackRumbleMapValues, sizeof(scr_string_t) * 16);
			// notetrackFXMapKeys
			// notetrackFXMapValues
			// notetrackFXMapTagValues
			// notetrackHideTagKeys
			// notetrackHideTagValues
			// notetrackHideTagTagValues
			h1_asset->szAdsrBaseSetting = nullptr;
			REINTERPRET_CAST(viewFlashEffect, weapDef->viewFlashEffect);
			h1_asset->viewBodyFlashEffect = nullptr;
			REINTERPRET_CAST(worldFlashEffect, weapDef->worldFlashEffect);
			h1_asset->viewFlashADSEffect = nullptr;
			// signatureViewFlashEffect
			// signatureViewBodyFlashEffect
			// signatureWorldFlashEffect
			// signatureViewFlashADSEffect
			// signatureViewBodyFlashADSEffect
			h1_asset->meleeHitEffect = nullptr;
			h1_asset->meleeMissEffect = nullptr;
			REINTERPRET_CAST(pickupSound, weapDef->pickupSound.sound);
			REINTERPRET_CAST(pickupSoundPlayer, weapDef->pickupSoundPlayer.sound);
			REINTERPRET_CAST(ammoPickupSound, weapDef->ammoPickupSound.sound);
			REINTERPRET_CAST(ammoPickupSoundPlayer, weapDef->ammoPickupSoundPlayer.sound);
			REINTERPRET_CAST(projectileSound, weapDef->projectileSound.sound);
			REINTERPRET_CAST(pullbackSound, weapDef->pullbackSound.sound);
			REINTERPRET_CAST(pullbackSoundPlayer, weapDef->pullbackSoundPlayer.sound);
			//REINTERPRET_CAST(pullbackSoundQuick, weapDef->pullbackSoundQuick.sound);
			//REINTERPRET_CAST(pullbackSoundQuickPlayer, weapDef->pullbackSoundQuickPlayer.sound);
			REINTERPRET_CAST(fireSound, weapDef->fireSound.sound);
			REINTERPRET_CAST(fireSoundPlayer, weapDef->fireSoundPlayer.sound);
			REINTERPRET_CAST(fireSoundPlayerAkimbo, weapDef->fireSoundPlayerAkimbo.sound);
			//REINTERPRET_CAST(fireMedSound, weapDef->fireMedSound.sound);
			//REINTERPRET_CAST(fireMedSoundPlayer, weapDef->fireMedSoundPlayer.sound);
			//REINTERPRET_CAST(fireHighSound, weapDef->fireHighSound.sound);
			//REINTERPRET_CAST(fireHighSoundPlayer, weapDef->fireHighSoundPlayer.sound);
			REINTERPRET_CAST(fireLoopSound, weapDef->fireLoopSound.sound);
			REINTERPRET_CAST(fireLoopSoundPlayer, weapDef->fireLoopSoundPlayer.sound);
			//REINTERPRET_CAST(fireMedLoopSound, weapDef->fireMedLoopSound.sound);
			//REINTERPRET_CAST(fireMedLoopSoundPlayer, weapDef->fireMedLoopSoundPlayer.sound);
			//REINTERPRET_CAST(fireHighLoopSound, weapDef->fireHighLoopSound.sound);
			//REINTERPRET_CAST(fireHighLoopSoundPlayer, weapDef->fireHighLoopSoundPlayer.sound);
			//REINTERPRET_CAST(fireLoopEndPointSound, weapDef->fireLoopEndPointSound.sound);
			//REINTERPRET_CAST(fireLoopEndPointSoundPlayer, weapDef->fireLoopEndPointSoundPlayer.sound);
			REINTERPRET_CAST(fireStopSound, weapDef->fireStopSound.sound);
			REINTERPRET_CAST(fireStopSoundPlayer, weapDef->fireStopSoundPlayer.sound);
			//REINTERPRET_CAST(fireMedStopSound, weapDef->fireMedStopSound.sound);
			//REINTERPRET_CAST(fireMedStopSoundPlayer, weapDef->fireMedStopSoundPlayer.sound);
			//REINTERPRET_CAST(fireHighStopSound, weapDef->fireHighStopSound.sound);
			//REINTERPRET_CAST(fireHighStopSoundPlayer, weapDef->fireHighStopSoundPlayer.sound);
			REINTERPRET_CAST(fireLastSound, weapDef->fireLastSound.sound);
			REINTERPRET_CAST(fireLastSoundPlayer, weapDef->fireLastSoundPlayer.sound);
			//REINTERPRET_CAST(fireFirstSound, weapDef->fireFirstSound.sound);
			//REINTERPRET_CAST(fireFirstSoundPlayer, weapDef->fireFirstSoundPlayer.sound);
			//REINTERPRET_CAST(fireCustomSound, weapDef->fireCustomSound.sound);
			//REINTERPRET_CAST(fireCustomSoundPlayer, weapDef->fireCustomSoundPlayer.sound);
			REINTERPRET_CAST(emptyFireSound, weapDef->emptyFireSound.sound);
			REINTERPRET_CAST(emptyFireSoundPlayer, weapDef->emptyFireSoundPlayer.sound);
			//REINTERPRET_CAST(adsRequiredFireSoundPlayer, weapDef->adsRequiredFireSoundPlayer.sound);
			REINTERPRET_CAST(meleeSwipeSound, weapDef->meleeSwipeSound.sound);
			REINTERPRET_CAST(meleeSwipeSoundPlayer, weapDef->meleeSwipeSoundPlayer.sound);
			REINTERPRET_CAST(meleeHitSound, weapDef->meleeHitSound.sound);
			//REINTERPRET_CAST(meleeHitSoundPlayer, weapDef->meleeHitSoundPlayer.sound);
			REINTERPRET_CAST(meleeMissSound, weapDef->meleeMissSound.sound);
			//REINTERPRET_CAST(meleeMissSoundPlayer, weapDef->meleeMissSoundPlayer.sound);
			REINTERPRET_CAST(rechamberSound, weapDef->rechamberSound.sound);
			REINTERPRET_CAST(rechamberSoundPlayer, weapDef->rechamberSoundPlayer.sound);
			REINTERPRET_CAST(reloadSound, weapDef->reloadSound.sound);
			REINTERPRET_CAST(reloadSoundPlayer, weapDef->reloadSoundPlayer.sound);
			REINTERPRET_CAST(reloadEmptySound, weapDef->reloadEmptySound.sound);
			REINTERPRET_CAST(reloadEmptySoundPlayer, weapDef->reloadEmptySoundPlayer.sound);
			REINTERPRET_CAST(reloadStartSound, weapDef->reloadStartSound.sound);
			REINTERPRET_CAST(reloadStartSoundPlayer, weapDef->reloadStartSoundPlayer.sound);
			REINTERPRET_CAST(reloadEndSound, weapDef->reloadEndSound.sound);
			REINTERPRET_CAST(reloadEndSoundPlayer, weapDef->reloadEndSoundPlayer.sound);
			REINTERPRET_CAST(detonateSound, weapDef->detonateSound.sound);
			REINTERPRET_CAST(detonateSoundPlayer, weapDef->detonateSoundPlayer.sound);
			REINTERPRET_CAST(nightVisionWearSound, weapDef->nightVisionWearSound.sound);
			REINTERPRET_CAST(nightVisionWearSoundPlayer, weapDef->nightVisionWearSoundPlayer.sound);
			REINTERPRET_CAST(nightVisionRemoveSound, weapDef->nightVisionRemoveSound.sound);
			REINTERPRET_CAST(nightVisionRemoveSoundPlayer, weapDef->nightVisionRemoveSoundPlayer.sound);
			REINTERPRET_CAST(raiseSound, weapDef->raiseSound.sound);
			REINTERPRET_CAST(raiseSoundPlayer, weapDef->raiseSoundPlayer.sound);
			REINTERPRET_CAST(firstRaiseSound, weapDef->firstRaiseSound.sound);
			REINTERPRET_CAST(firstRaiseSoundPlayer, weapDef->firstRaiseSoundPlayer.sound);
			REINTERPRET_CAST(altSwitchSound, weapDef->altSwitchSound.sound);
			REINTERPRET_CAST(altSwitchSoundPlayer, weapDef->altSwitchSoundPlayer.sound);
			REINTERPRET_CAST(putawaySound, weapDef->putawaySound.sound);
			REINTERPRET_CAST(putawaySoundPlayer, weapDef->putawaySoundPlayer.sound);
			REINTERPRET_CAST(scanSound, weapDef->scanSound.sound);
			REINTERPRET_CAST(changeVariableZoomSound, weapDef->changeVariableZoomSound.sound);
			//REINTERPRET_CAST(adsUpSound, weapDef->adsUpSound.sound);
			//REINTERPRET_CAST(adsDownSound, weapDef->adsDownSound.sound);
			//REINTERPRET_CAST(adsCrosshairEnemySound, weapDef->adsCrosshairEnemySound.sound);

			if (asset->weapDef->bounceSound)
			{
				h1_asset->bounceSound = mem.allocate<H1::snd_alias_list_t PTR64>(53);
				convertSurfaceSounds(h1_asset->bounceSound, asset->weapDef->bounceSound, mem);
			}
			if (asset->weapDef->rollingSound)
			{
				h1_asset->rollingSound = mem.allocate<H1::snd_alias_list_t PTR64>(53);
				convertSurfaceSounds(h1_asset->rollingSound, asset->weapDef->rollingSound, mem);
			}

			REINTERPRET_CAST(viewShellEjectEffect, weapDef->viewShellEjectEffect);
			REINTERPRET_CAST(worldShellEjectEffect, weapDef->worldShellEjectEffect);
			REINTERPRET_CAST(viewLastShotEjectEffect, weapDef->viewLastShotEjectEffect);
			REINTERPRET_CAST(worldLastShotEjectEffect, weapDef->worldLastShotEjectEffect);
			// viewMagEjectEffect
			REINTERPRET_CAST(reticleCenter, weapDef->reticleCenter);
			REINTERPRET_CAST(reticleSide, weapDef->reticleSide);
			if (asset->weapDef->worldModel)
			{
				h1_asset->worldModel = mem.allocate<H1::XModel PTR64>(2);
				h1_asset->worldModel[0] = reinterpret_cast<H1::XModel*>(asset->weapDef->worldModel[0]);
				h1_asset->worldModel[1] = nullptr;
			}
			REINTERPRET_CAST(worldClipModel, weapDef->worldClipModel);
			REINTERPRET_CAST(rocketModel, weapDef->rocketModel);
			REINTERPRET_CAST(knifeModel, weapDef->knifeModel);
			REINTERPRET_CAST(worldKnifeModel, weapDef->worldKnifeModel);
			REINTERPRET_CAST(hudIcon, weapDef->hudIcon);
			REINTERPRET_CAST(pickupIcon, weapDef->pickupIcon);
			//REINTERPRET_CAST(minimapIconFriendly, weapDef->minimapIconFriendly);
			//REINTERPRET_CAST(minimapIconEnemy, weapDef->minimapIconEnemy);
			//REINTERPRET_CAST(minimapIconNeutral, weapDef->minimapIconNeutral);
			REINTERPRET_CAST(ammoCounterIcon, weapDef->ammoCounterIcon);
			REINTERPRET_CAST(szAmmoName, weapDef->szAmmoName);
			REINTERPRET_CAST(szClipName, weapDef->szClipName);
			REINTERPRET_CAST(szSharedAmmoCapName, weapDef->szSharedAmmoCapName);
			REINTERPRET_CAST(physCollmap, weapDef->physCollmap);
			h1_asset->physPreset = getPhysPreset(asset->weapDef->weapClass, mem);
			REINTERPRET_CAST(szUseHintString, weapDef->szUseHintString);
			REINTERPRET_CAST(dropHintString, weapDef->dropHintString);

			// locationDamageMultipliers
			h1_asset->locationDamageMultipliers = mem.allocate<float>(22);
			std::fill_n(h1_asset->locationDamageMultipliers, 22, 1.0f);
			for (auto i = 0; i < 20; i++)
			{
				h1_asset->locationDamageMultipliers[i] = asset->weapDef->locationDamageMultipliers[i];
			}
			//memcpy(h1_asset->locationDamageMultipliers, asset->weapDef->locationDamageMultipliers, sizeof(float) * 20);

			REINTERPRET_CAST(fireRumble, weapDef->fireRumble);
			// fireMedRumble
			// fireHighRumble
			REINTERPRET_CAST(meleeImpactRumble, weapDef->meleeImpactRumble);
			REINTERPRET_CAST(tracerType, weapDef->tracerType);
			// signatureTracerType
			// laserType
			REINTERPRET_CAST(turretOverheatSound, weapDef->turretOverheatSound.sound);
			REINTERPRET_CAST(turretOverheatEffect, weapDef->turretOverheatEffect);
			REINTERPRET_CAST(turretBarrelSpinRumble, weapDef->turretBarrelSpinRumble);
			REINTERPRET_CAST(turretBarrelSpinMaxSnd, weapDef->turretBarrelSpinMaxSnd.sound);
			memcpy(h1_asset->turretBarrelSpinUpSnd, asset->weapDef->turretBarrelSpinUpSnd, sizeof(asset->weapDef->turretBarrelSpinUpSnd));
			memcpy(h1_asset->turretBarrelSpinDownSnd, asset->weapDef->turretBarrelSpinDownSnd, sizeof(asset->weapDef->turretBarrelSpinDownSnd));
			REINTERPRET_CAST(missileConeSoundAlias, weapDef->missileConeSoundAlias.sound);
			REINTERPRET_CAST(missileConeSoundAliasAtBase, weapDef->missileConeSoundAliasAtBase.sound);
			REINTERPRET_CAST(stowOffsetModel, weapDef->stowOffsetModel);
			// turretHydraulicSettings

			COPY_FIELD(altWeapon);
			// numAttachments
			// numAnimOverrides
			// numSoundOverrides
			// numFXOverrides
			// numReloadStateTimerOverrides
			// numNotetrackOverrides

			h1_asset->playerAnimType = convertPlayerAnim(asset->weapDef->playerAnimType);
			COPY_FIELD_CAST(weapType, weapDef->weapType); // same enum values
			COPY_FIELD_CAST(weapClass, weapDef->weapClass); // same enum values up to a certain point
			COPY_FIELD_CAST(penetrateType, weapDef->penetrateType); // same enum values
			h1_asset->penetrateDepth = asset->weapDef->penetrateType == PENETRATE_TYPE_NONE ? 0.0f : 1.0f;
			COPY_FIELD_CAST(impactType, impactType); // same enum values
			h1_asset->impactType = static_cast<H1::ImpactType>(asset->impactType > 6 ? asset->impactType + 1 : asset->impactType);
			COPY_FIELD_CAST(inventoryType, weapDef->inventoryType); // same enum values
			h1_asset->fireType = static_cast<H1::weapFireType_t>(asset->weapDef->fireType > 4 ? asset->weapDef->fireType + 1 : asset->weapDef->fireType);
			h1_asset->fireBarrels = asset->weapDef->fireType == WEAPON_FIRETYPE_DOUBLEBARREL ? H1::WEAPON_FIREBARREL_DOUBLE : H1::WEAPON_FIREBARREL_SINGLE;
			h1_asset->adsFireMode = H1::WEAPADSFIREMODE_DEFAULT;
			h1_asset->burstFireCooldown = asset->weapDef->fireType == WEAPON_FIRETYPE_BURSTFIRE2 || asset->weapDef->fireType == WEAPON_FIRETYPE_BURSTFIRE3 || asset->weapDef->fireType == WEAPON_FIRETYPE_BURSTFIRE4 ? 200.0f : 0.0f;
			h1_asset->greebleType = H1::WEAPON_GREEBLE_NONE;
			h1_asset->autoReloadType = H1::WEAPON_AUTORELOAD_ALWAYS;
			h1_asset->autoHolsterType = H1::WEAPON_AUTOHOLSTER_ALWAYS;
			h1_asset->offhandClass = convertOffhandClass(asset->weapDef->offhandClass);
			COPY_FIELD_CAST(stance, weapDef->stance); // same enum values

			COPY_FIELD(reticleCenterSize, weapDef->iReticleCenterSize);
			COPY_FIELD(reticleSideSize, weapDef->iReticleSideSize);
			COPY_FIELD(reticleMinOfs, weapDef->iReticleMinOfs);
			COPY_FIELD_CAST(activeReticleType, weapDef->activeReticleType); // same enum values
			
			constexpr std::size_t size = MEMBER_SPAN_SIZE_T(WeaponDef, vStandMove, fPosProneRotRate);
			std::memcpy(&h1_asset->standMove, &asset->weapDef->vStandMove, size);

			COPY_FIELD_CAST(hudIconRatio, weapDef->hudIconRatio); // same enum values
			COPY_FIELD_CAST(pickupIconRatio, weapDef->pickupIconRatio); // ^
			COPY_FIELD_CAST(ammoCounterIconRatio, weapDef->ammoCounterIconRatio); // ^
			COPY_FIELD_CAST(ammoCounterClip, weapDef->ammoCounterClip); // ^
			COPY_FIELD(startAmmo, weapDef->iStartAmmo);
			// iAmmoIndex (runtime)
			// iClipIndex (runtime)
			COPY_FIELD(maxAmmo, weapDef->iMaxAmmo);
			h1_asset->minAmmoReq = 1;
			COPY_FIELD(clipSize, iClipSize);
			COPY_FIELD(shotCount, weapDef->shotCount);
			// sharedAmmoCapIndex (runtime)
			COPY_FIELD(sharedAmmoCap, weapDef->iSharedAmmoCap);
			COPY_FIELD(damage, weapDef->damage);
			COPY_FIELD(playerDamage, weapDef->playerDamage);
			COPY_FIELD(meleeDamage, weapDef->iMeleeDamage);
			COPY_FIELD(damageType, weapDef->iDamageType);

			copyStateTimer(&h1_asset->stateTimers, &asset->weapDef->stateTimers, asset, false);
			copyStateTimer(&h1_asset->akimboStateTimers, &asset->weapDef->stateTimers, asset, true);

			COPY_FIELD(autoAimRange, weapDef->autoAimRange);
			COPY_FIELD(aimAssistRange, weapDef->aimAssistRange);
			COPY_FIELD(aimAssistRangeAds, weapDef->aimAssistRangeAds);
			COPY_FIELD(aimPadding, weapDef->aimPadding);
			COPY_FIELD(enemyCrosshairRange, weapDef->enemyCrosshairRange);
			COPY_FIELD(moveSpeedScale, weapDef->moveSpeedScale);
			COPY_FIELD(adsMoveSpeedScale, weapDef->adsMoveSpeedScale);
			COPY_FIELD(sprintDurationScale, weapDef->sprintDurationScale);
			h1_asset->adsZoomFov = 65.0f;
			COPY_FIELD(adsZoomInFrac, weapDef->fAdsZoomInFrac);
			COPY_FIELD(adsZoomOutFrac, weapDef->fAdsZoomOutFrac);
			h1_asset->adsSceneBlurStrength = 0.0f;
			if (isGun(asset->weapDef->weapClass))
			{
				h1_asset->adsSceneBlurPhysicalScale = 1.0f;
			}
			// pad3

			REINTERPRET_CAST(overlay.shader, weapDef->overlay.shader);
			REINTERPRET_CAST(overlay.shaderLowRes, weapDef->overlay.shaderLowRes);
			REINTERPRET_CAST(overlay.shaderEMP, weapDef->overlay.shaderEMP);
			REINTERPRET_CAST(overlay.shaderEMPLowRes, weapDef->overlay.shaderEMPLowRes);
			COPY_FIELD_CAST(overlay.reticle, weapDef->overlay.reticle);
			COPY_FIELD(overlay.width, weapDef->overlay.width);
			COPY_FIELD(overlay.height, weapDef->overlay.height);
			COPY_FIELD(overlay.widthSplitscreen, weapDef->overlay.widthSplitscreen);
			COPY_FIELD(overlay.heightSplitscreen, weapDef->overlay.heightSplitscreen);

			COPY_FIELD(adsBobFactor, weapDef->fAdsBobFactor);
			COPY_FIELD(adsViewBobMult, weapDef->fAdsViewBobMult);
			COPY_FIELD(hipSpreadStandMin, weapDef->fHipSpreadStandMin);
			COPY_FIELD(hipSpreadDuckedMin, weapDef->fHipSpreadDuckedMin);
			COPY_FIELD(hipSpreadProneMin, weapDef->fHipSpreadProneMin);
			COPY_FIELD(hipSpreadStandMax, weapDef->hipSpreadStandMax);
			h1_asset->hipSpreadSprintMax = 0.0f;
			h1_asset->hipSpreadSlideMax = 0.0f;
			COPY_FIELD(hipSpreadDuckedMax, weapDef->hipSpreadDuckedMax);
			COPY_FIELD(hipSpreadProneMax, weapDef->hipSpreadProneMax);
			COPY_FIELD(hipSpreadDecayRate, weapDef->fHipSpreadDecayRate);
			COPY_FIELD(hipSpreadFireAdd, weapDef->fHipSpreadFireAdd);
			COPY_FIELD(hipSpreadTurnAdd, weapDef->fHipSpreadTurnAdd);
			COPY_FIELD(hipSpreadMoveAdd, weapDef->fHipSpreadMoveAdd);
			COPY_FIELD(hipSpreadDuckedDecay, weapDef->fHipSpreadDuckedDecay);
			COPY_FIELD(hipSpreadProneDecay, weapDef->fHipSpreadProneDecay);
			COPY_FIELD(hipReticleSidePos, weapDef->fHipReticleSidePos);
			COPY_FIELD(adsIdleAmount, weapDef->fAdsIdleAmount);
			COPY_FIELD(hipIdleAmount, weapDef->fHipIdleAmount);
			COPY_FIELD(adsIdleSpeed, weapDef->adsIdleSpeed);
			COPY_FIELD(hipIdleSpeed, weapDef->hipIdleSpeed);
			COPY_FIELD(idleCrouchFactor, weapDef->fIdleCrouchFactor);
			COPY_FIELD(idleProneFactor, weapDef->fIdleProneFactor);
			COPY_FIELD(gunMaxPitch, weapDef->fGunMaxPitch);
			COPY_FIELD(gunMaxYaw, weapDef->fGunMaxYaw);
			COPY_FIELD(adsIdleLerpStartTime, weapDef->adsIdleLerpStartTime);
			COPY_FIELD(adsIdleLerpTime, weapDef->adsIdleLerpTime);
			COPY_FIELD(adsTransInTime, iAdsTransInTime);
			h1_asset->adsTransInFromSprintTime = asset->iAdsTransInTime;
			COPY_FIELD(adsTransOutTime, iAdsTransOutTime);
			h1_asset->swayMaxAngleSteadyAim = 1.0f;
			COPY_FIELD(swayMaxAngle, weapDef->swayMaxAngle);
			COPY_FIELD(swayLerpSpeed, weapDef->swayLerpSpeed);
			COPY_FIELD(swayPitchScale, weapDef->swayPitchScale);
			COPY_FIELD(swayYawScale, weapDef->swayYawScale);
			COPY_FIELD(swayVertScale, weapDef->swayVertScale);
			COPY_FIELD(swayHorizScale, weapDef->swayHorizScale);
			COPY_FIELD(swayShellShockScale, weapDef->swayShellShockScale);
			COPY_FIELD(adsSwayMaxAngle, weapDef->adsSwayMaxAngle);
			COPY_FIELD(adsSwayLerpSpeed, weapDef->adsSwayLerpSpeed);
			COPY_FIELD(adsSwayPitchScale, weapDef->adsSwayPitchScale);
			COPY_FIELD(adsSwayYawScale, weapDef->adsSwayYawScale);
			COPY_FIELD(adsSwayHorizScale, weapDef->adsSwayHorizScale);
			COPY_FIELD(adsSwayVertScale, weapDef->adsSwayVertScale);
			COPY_FIELD(adsViewErrorMin, weapDef->adsViewErrorMin);
			COPY_FIELD(adsViewErrorMax, weapDef->adsViewErrorMax);
			COPY_FIELD(dualWieldViewModelOffset, weapDef->dualWieldViewModelOffset);
			if (isGun(asset->weapDef->weapClass))
			{
				h1_asset->adsFireAnimFrac = 0.75f;
				h1_asset->scopeDriftDelay = 0.5f;
				h1_asset->scopeDriftLerpInTime = 1.5f;
				h1_asset->scopeDriftSteadyTime = 8.0f;
				h1_asset->scopeDriftLerpOutTime = 1.5f;
				h1_asset->scopeDriftSteadyFactor = 0.5f;
				h1_asset->scopeDriftUnsteadyFactor = 1.5f;
			}
			h1_asset->bobVerticalFactor = 1.0f;
			h1_asset->bobHorizontalFactor = 1.0f;
			h1_asset->bobViewVerticalFactor = 1.0f;
			h1_asset->bobViewHorizontalFactor = 1.0f;
			if (isGun(asset->weapDef->weapClass))
			{
				h1_asset->stationaryZoomFov = 0.0f;
				h1_asset->stationaryZoomDelay = 1.0f;
				h1_asset->stationaryZoomLerpInTime = 2.0f;
				h1_asset->stationaryZoomLerpOutTime = 0.5f;
			}
			COPY_FIELD(adsDofStart);
			COPY_FIELD(adsDofEnd);
			
			REINTERPRET_CAST(killIcon);
			REINTERPRET_CAST(dpadIcon);
			h1_asset->hudProximityWarningIcon = nullptr;

			COPY_FIELD_CAST(killIconRatio, weapDef->killIconRatio);
			COPY_FIELD_CAST(dpadIconRatio);
			COPY_FIELD(fireAnimLength);
			COPY_FIELD(fireAnimLengthAkimbo);
			h1_asset->inspectAnimTime = 0;
			COPY_FIELD(reloadAmmoAdd, weapDef->iReloadAmmoAdd);
			COPY_FIELD(reloadStartAdd, weapDef->iReloadStartAdd);
			COPY_FIELD(ammoDropStockMin, weapDef->ammoDropStockMin);
			COPY_FIELD(ammoDropStockMax, ammoDropStockMax);
			COPY_FIELD(ammoDropClipPercentMin, weapDef->ammoDropClipPercentMin);
			COPY_FIELD(ammoDropClipPercentMax, weapDef->ammoDropClipPercentMax);
			COPY_FIELD(explosionRadius, weapDef->iExplosionRadius);
			COPY_FIELD(explosionRadiusMin, weapDef->iExplosionRadiusMin);
			COPY_FIELD(explosionInnerDamage, weapDef->iExplosionInnerDamage);
			COPY_FIELD(explosionOuterDamage, weapDef->iExplosionOuterDamage);
			COPY_FIELD(damageConeAngle, weapDef->damageConeAngle);
			COPY_FIELD(bulletExplDmgMult, weapDef->bulletExplDmgMult);
			COPY_FIELD(bulletExplRadiusMult, weapDef->bulletExplRadiusMult);
			COPY_FIELD(projectileSpeed, weapDef->iProjectileSpeed);
			COPY_FIELD(projectileSpeedUp, weapDef->iProjectileSpeedUp);
			COPY_FIELD(projectileSpeedForward, weapDef->iProjectileSpeedForward);
			COPY_FIELD(projectileActivateDist, weapDef->iProjectileActivateDist);
			COPY_FIELD(projLifetime, weapDef->projLifetime);
			COPY_FIELD(timeToAccelerate, weapDef->timeToAccelerate);
			COPY_FIELD(projectileCurvature, weapDef->projectileCurvature);
			h1_asset->projectileName = nullptr;
			REINTERPRET_CAST(projectileModel, weapDef->projectileModel);
			REINTERPRET_CAST(projExplosionEffect, weapDef->projExplosionEffect);
			REINTERPRET_CAST(projDudEffect, weapDef->projDudEffect);
			REINTERPRET_CAST(projExplosionSound, weapDef->projExplosionSound.sound);
			REINTERPRET_CAST(projDudSound, weapDef->projDudSound.sound);
			COPY_FIELD_CAST(projExplosion, weapDef->projExplosion); // same enum values
			COPY_FIELD_CAST(stickiness, weapDef->stickiness); // same enum values
			COPY_FIELD(lowAmmoWarningThreshold, weapDef->lowAmmoWarningThreshold);
			COPY_FIELD(ricochetChance, weapDef->ricochetChance);
			COPY_FIELD(riotShieldHealth, weapDef->riotShieldHealth);
			COPY_FIELD(riotShieldDamageMult, weapDef->riotShieldDamageMult);

			std::vector<float> parallelBounceArray = 
			{
				0.5,
				0.6000000238418579,
				0.6000000238418579,
				0.6000000238418579,
				0.20000000298023224,
				0.6000000238418579,
				0.44999998807907104,
				0.20000000298023224,
				0.05000000074505806,
				0.4000000059604645,
				0.3499999940395355,
				0.5,
				0.6000000238418579,
				0.6000000238418579,
				0.5,
				0.20000000298023224,
				0.20000000298023224,
				0.5,
				0.6000000238418579,
				0.30000001192092896,
				0.20000000298023224,
				0.20000000298023224,
				0.6000000238418579,
				0.6000000238418579,
				0.5,
				0.5,
				0.5,
				0.5,
				0.5,
				0.5,
				0.5,
				0.6000000238418579,
				0.6000000238418579,
				0.6000000238418579,
				0.6000000238418579,
				0.05000000074505806,
				0.05000000074505806,
				0.3499999940395355,
				0.6000000238418579,
				0.6000000238418579,
				0.6000000238418579,
				0.6000000238418579,
				0.6000000238418579,
				0.5,
				0.5,
				0.6000000238418579,
				0.6000000238418579,
				0.20000000298023224,
				0.20000000298023224,
				0.6000000238418579,
				0.6000000238418579,
				0.6000000238418579,
				0.0
			};

			std::vector<float> perpendicularBounceArray =
			{
				0.25,
				0.25,
				0.25,
				0.25,
				0.20000000298023224,
				0.25,
				0.22499999403953552,
				0.20000000298023224,
				0.05000000074505806,
				0.20000000298023224,
				0.15000000596046448,
				0.25,
				0.30000001192092896,
				0.25,
				0.5,
				0.10000000149011612,
				0.20000000298023224,
				0.25,
				0.25,
				0.10000000149011612,
				0.10000000149011612,
				0.20000000298023224,
				0.25,
				0.25,
				0.5,
				0.5,
				0.5,
				0.5,
				0.5,
				0.5,
				0.5,
				0.25,
				0.25,
				0.25,
				0.25,
				0.05000000074505806,
				0.05000000074505806,
				0.15000000596046448,
				0.25,
				0.25,
				0.25,
				0.25,
				0.25,
				0.5,
				0.5,
				0.25,
				0.25,
				0.20000000298023224,
				0.20000000298023224,
				0.25,
				0.25,
				0.25,
				0.0
			};

			h1_asset->parallelBounce = mem.allocate<float>(53);
			if (asset->weapDef->weapClass == WEAPCLASS_ROCKETLAUNCHER)
			{
				std::fill_n(h1_asset->parallelBounce, 52, 0.5f);
			}
			else if (asset->weapDef->weapClass == WEAPCLASS_GRENADE || asset->weapDef->weapClass == WEAPCLASS_THROWINGKNIFE)
			{
				std::copy_n(parallelBounceArray.data(), std::min<std::size_t>(52, parallelBounceArray.size()), h1_asset->parallelBounce);
			}

			h1_asset->perpendicularBounce = mem.allocate<float>(53);
			if (asset->weapDef->weapClass == WEAPCLASS_ROCKETLAUNCHER)
			{
				std::fill_n(h1_asset->perpendicularBounce, 52, 0.5f);
			}
			else if (asset->weapDef->weapClass == WEAPCLASS_GRENADE || asset->weapDef->weapClass == WEAPCLASS_THROWINGKNIFE)
			{
				std::copy_n(perpendicularBounceArray.data(), std::min<std::size_t>(52, perpendicularBounceArray.size()), h1_asset->perpendicularBounce);
			}

			REINTERPRET_CAST(projTrailEffect, weapDef->projTrailEffect);
			REINTERPRET_CAST(projBeaconEffect, weapDef->projBeaconEffect);
			COPY_ARR(projectileColor, weapDef->vProjectileColor);
			COPY_FIELD_CAST(guidedMissileType, weapDef->guidedMissileType);
			COPY_FIELD(maxSteeringAccel, weapDef->maxSteeringAccel);
			COPY_FIELD(projIgnitionDelay, weapDef->projIgnitionDelay);
			REINTERPRET_CAST(projIgnitionEffect, weapDef->projIgnitionEffect);
			REINTERPRET_CAST(projIgnitionSound, weapDef->projIgnitionSound.sound);

			COPY_FIELD(adsAimPitch, weapDef->fAdsAimPitch);
			COPY_FIELD(adsCrosshairInFrac, weapDef->fAdsCrosshairInFrac);
			COPY_FIELD(adsCrosshairOutFrac, weapDef->fAdsCrosshairOutFrac);
			COPY_FIELD(adsGunKickReducedKickBullets, weapDef->adsGunKickReducedKickBullets);
			COPY_FIELD(adsGunKickReducedKickPercent, weapDef->adsGunKickReducedKickPercent);
			COPY_FIELD(adsGunKickPitchMin, weapDef->fAdsGunKickPitchMin);
			COPY_FIELD(adsGunKickPitchMax, weapDef->fAdsGunKickPitchMax);
			COPY_FIELD(adsGunKickYawMin, weapDef->fAdsGunKickYawMin);
			COPY_FIELD(adsGunKickYawMax, weapDef->fAdsGunKickYawMax);
			h1_asset->adsGunKickMagMin = 0.0f;
			COPY_FIELD(adsGunKickAccel, weapDef->fAdsGunKickAccel);
			COPY_FIELD(adsGunKickSpeedMax, weapDef->fAdsGunKickSpeedMax);
			COPY_FIELD(adsGunKickSpeedDecay, weapDef->fAdsGunKickSpeedDecay);
			COPY_FIELD(adsGunKickStaticDecay, weapDef->fAdsGunKickStaticDecay);
			COPY_FIELD(adsViewKickPitchMin, weapDef->fAdsViewKickPitchMin);
			COPY_FIELD(adsViewKickPitchMax, weapDef->fAdsViewKickPitchMax);
			COPY_FIELD(adsViewKickYawMin, weapDef->fAdsViewKickYawMin);
			COPY_FIELD(adsViewKickYawMax, weapDef->fAdsViewKickYawMax);
			h1_asset->adsViewKickMagMin = 0.0f;
			if (isGun(asset->weapDef->weapClass))
			{
				h1_asset->adsViewKickCenterSpeed = 1500.0f;
			}
			COPY_FIELD(adsViewScatterMin, weapDef->fAdsViewScatterMin);
			COPY_FIELD(adsViewScatterMax, weapDef->fAdsViewScatterMax);
			COPY_FIELD(adsSpread, weapDef->fAdsSpread);
			COPY_FIELD(hipGunKickReducedKickBullets, weapDef->hipGunKickReducedKickBullets);
			COPY_FIELD(hipGunKickReducedKickPercent, weapDef->hipGunKickReducedKickPercent);
			COPY_FIELD(hipGunKickPitchMin, weapDef->fHipGunKickPitchMin);
			COPY_FIELD(hipGunKickPitchMax, weapDef->fHipGunKickPitchMax);
			COPY_FIELD(hipGunKickYawMin, weapDef->fHipGunKickYawMin);
			COPY_FIELD(hipGunKickYawMax, weapDef->fHipGunKickYawMax);
			h1_asset->hipGunKickMagMin = 0.0f;
			COPY_FIELD(hipGunKickAccel, weapDef->fHipGunKickAccel);
			COPY_FIELD(hipGunKickSpeedMax, weapDef->fHipGunKickSpeedMax);
			COPY_FIELD(hipGunKickSpeedDecay, weapDef->fHipGunKickSpeedDecay);
			COPY_FIELD(hipGunKickStaticDecay, weapDef->fHipGunKickStaticDecay);
			COPY_FIELD(hipViewKickPitchMin, weapDef->fHipViewKickPitchMin);
			COPY_FIELD(hipViewKickPitchMax, weapDef->fHipViewKickPitchMax);
			COPY_FIELD(hipViewKickYawMin, weapDef->fHipViewKickYawMin);
			COPY_FIELD(hipViewKickYawMax, weapDef->fHipViewKickYawMax);
			h1_asset->hipViewKickMagMin = 0.0f;
			if (isGun(asset->weapDef->weapClass))
			{
				h1_asset->hipViewKickCenterSpeed = 1500.0f;
			}
			COPY_FIELD(hipViewScatterMin, weapDef->fHipViewScatterMin);
			COPY_FIELD(hipViewScatterMax, weapDef->fHipViewScatterMax);
			if (isGun(asset->weapDef->weapClass))
			{
				h1_asset->viewKickScale = 1.0f;
			}
			COPY_FIELD(positionReloadTransTime, weapDef->iPositionReloadTransTime);
			COPY_FIELD(fightDist, weapDef->fightDist);
			COPY_FIELD(maxDist, weapDef->maxDist);
			REINTERPRET_CAST(accuracyGraphName[0], weapDef->aiVsAiAccuracyGraphName);
			REINTERPRET_CAST(accuracyGraphName[1], weapDef->aiVsPlayerAccuracyGraphName);
			REINTERPRET_CAST(accuracyGraphKnots[0], weapDef->originalAiVsAiAccuracyGraphKnots);
			REINTERPRET_CAST(accuracyGraphKnots[1], weapDef->originalAiVsPlayerAccuracyGraphKnots);
			h1_asset->originalAccuracyGraphKnots[0] = h1_asset->accuracyGraphKnots[0];
			h1_asset->originalAccuracyGraphKnots[1] = h1_asset->accuracyGraphKnots[1];
			COPY_FIELD(accuracyGraphKnotCount[0], weapDef->originalAiVsAiAccuracyGraphKnotCount);
			COPY_FIELD(accuracyGraphKnotCount[1], weapDef->originalAiVsPlayerAccuracyGraphKnotCount);
			COPY_FIELD(leftArc, weapDef->leftArc);
			COPY_FIELD(rightArc, weapDef->rightArc);
			COPY_FIELD(topArc, weapDef->topArc);
			COPY_FIELD(bottomArc, weapDef->bottomArc);
			COPY_FIELD(accuracy, weapDef->accuracy);
			COPY_FIELD(aiSpread, weapDef->aiSpread);
			COPY_FIELD(playerSpread, weapDef->playerSpread);
			COPY_ARR(minTurnSpeed, weapDef->minTurnSpeed);
			COPY_ARR(maxTurnSpeed, weapDef->maxTurnSpeed);
			COPY_FIELD(pitchConvergenceTime, weapDef->pitchConvergenceTime);
			COPY_FIELD(yawConvergenceTime, weapDef->yawConvergenceTime);
			COPY_FIELD(suppressTime, weapDef->suppressTime);
			COPY_FIELD(maxRange, weapDef->maxRange);
			COPY_FIELD(animHorRotateInc, weapDef->fAnimHorRotateInc);
			COPY_FIELD(playerPositionDist, weapDef->fPlayerPositionDist);
			// useHintStringIndex (runtime)
			// dropHintStringIndex (runtime)
			COPY_FIELD(horizViewJitter, weapDef->horizViewJitter);
			COPY_FIELD(vertViewJitter, weapDef->vertViewJitter);
			COPY_FIELD(scanSpeed, weapDef->scanSpeed);
			COPY_FIELD(scanAccel, weapDef->scanAccel);
			COPY_FIELD(scanPauseTime, weapDef->scanPauseTime);
			REINTERPRET_CAST(szScript, weapDef->szScript);
			COPY_FIELD(minDamage, weapDef->minDamage);
			COPY_FIELD(midDamage, weapDef->minDamage); // midDamage doesn't exist, using minDamage as a placeholder
			COPY_FIELD(minPlayerDamage, weapDef->minPlayerDamage);
			COPY_FIELD(midPlayerDamage, weapDef->minPlayerDamage); // midPlayerDamage doesn't exist, using minPlayerDamage as a placeholder
			COPY_FIELD(maxDamageRange, weapDef->fMaxDamageRange);
			COPY_FIELD(minDamageRange, weapDef->fMinDamageRange);
			h1_asset->signatureAmmoInClip = 0;
			if (isGun(asset->weapDef->weapClass))
			{
				h1_asset->signatureDamage = 30;
				h1_asset->signatureMidDamage = 30;
				h1_asset->signatureMinDamage = 30;
				h1_asset->signatureMaxDamageRange = 15000.0f;
				h1_asset->signatureMinDamageRange = 16000.0f;
			}
			COPY_FIELD(destabilizationRateTime, weapDef->destabilizationRateTime);
			COPY_FIELD(destabilizationCurvatureMax, weapDef->destabilizationCurvatureMax);
			COPY_FIELD(destabilizeDistance, weapDef->destabilizeDistance);

			COPY_FIELD(turretADSTime, weapDef->turretADSTime);
			COPY_FIELD(turretFov, weapDef->turretFov);
			COPY_FIELD(turretFovADS, weapDef->turretFovADS);
			COPY_FIELD(turretScopeZoomRate, weapDef->turretScopeZoomRate);
			COPY_FIELD(turretScopeZoomMin, weapDef->turretScopeZoomMin);
			COPY_FIELD(turretScopeZoomMax, weapDef->turretScopeZoomMax);
			COPY_FIELD(overheatUpRate, weapDef->turretOverheatUpRate);
			COPY_FIELD(overheatDownRate, weapDef->turretOverheatDownRate);
			h1_asset->overheatCooldownRate = 0.0f; // check this later for turrets
			COPY_FIELD(overheatPenalty, weapDef->turretOverheatPenalty);
			COPY_FIELD(turretBarrelSpinSpeed, weapDef->turretBarrelSpinSpeed);
			COPY_FIELD(turretBarrelSpinUpTime, weapDef->turretBarrelSpinUpTime);
			COPY_FIELD(turretBarrelSpinDownTime, weapDef->turretBarrelSpinDownTime);
			COPY_FIELD(missileConeSoundRadiusAtTop, weapDef->missileConeSoundRadiusAtTop);
			COPY_FIELD(missileConeSoundRadiusAtBase, weapDef->missileConeSoundRadiusAtBase);
			COPY_FIELD(missileConeSoundHeight, weapDef->missileConeSoundHeight);
			COPY_FIELD(missileConeSoundOriginOffset, weapDef->missileConeSoundOriginOffset);
			COPY_FIELD(missileConeSoundVolumescaleAtCore, weapDef->missileConeSoundVolumescaleAtCore);
			COPY_FIELD(missileConeSoundVolumescaleAtEdge, weapDef->missileConeSoundVolumescaleAtEdge);
			COPY_FIELD(missileConeSoundVolumescaleCoreSize, weapDef->missileConeSoundVolumescaleCoreSize);
			COPY_FIELD(missileConeSoundPitchAtTop, weapDef->missileConeSoundPitchAtTop);
			COPY_FIELD(missileConeSoundPitchAtBottom, weapDef->missileConeSoundPitchAtBottom);
			COPY_FIELD(missileConeSoundPitchTopSize, weapDef->missileConeSoundPitchTopSize);
			COPY_FIELD(missileConeSoundPitchBottomSize, weapDef->missileConeSoundPitchBottomSize);
			COPY_FIELD(missileConeSoundCrossfadeTopSize, weapDef->missileConeSoundCrossfadeTopSize);
			COPY_FIELD(missileConeSoundCrossfadeBottomSize, weapDef->missileConeSoundCrossfadeBottomSize);
			h1_asset->aim_automelee_lerp = 40.0f;
			h1_asset->aim_automelee_range = 128.0f;
			h1_asset->aim_automelee_region_height = 240.0f;
			h1_asset->aim_automelee_region_width = 320.0f;
			h1_asset->player_meleeHeight = 10.0f;
			h1_asset->player_meleeRange = 64.0f;
			h1_asset->player_meleeWidth = 10.0f;
			h1_asset->changedFireTime = 0.0f;
			h1_asset->changedFireTimeNumBullets = 0;
			h1_asset->fireTimeInterpolationType = H1::WEAPON_FIRETIME_INTERPOLATION_NONE;
			h1_asset->generateAmmo = 0;
			h1_asset->ammoPerShot = 1;
			if (isExplosive(asset->weapDef->weapClass))
			{
				h1_asset->explodeCount = 1;
			}
			h1_asset->batteryDischargeRate = 0;
			h1_asset->extendedBattery = 0;
			h1_asset->bulletsPerTag = 0;
			h1_asset->maxTags = 1;
			COPY_FIELD_CAST(stowTag, weapDef->stowTag);
			h1_asset->rattleSoundType = 0;
			h1_asset->adsShouldShowCrosshair = false;
			h1_asset->adsCrosshairShouldScale = true;
			COPY_FIELD(turretADSEnabled, weapDef->turretADSEnabled);
			h1_asset->knifeAttachTagLeft = false;
			h1_asset->knifeAlwaysAttached = false;
			h1_asset->meleeOverrideValues = false;
			COPY_FIELD(riotShieldEnableDamage, weapDef->riotShieldEnableDamage);
			h1_asset->allowPrimaryWeaponPickup = false;
			COPY_FIELD(sharedAmmo, weapDef->sharedAmmo);
			COPY_FIELD(lockonSupported, weapDef->lockonSupported);
			COPY_FIELD(requireLockonToFire, weapDef->requireLockonToFire);
			COPY_FIELD(isAirburstWeapon, weapDef->isAirburstWeapon);
			COPY_FIELD(bigExplosion, weapDef->bigExplosion);
			COPY_FIELD(noAdsWhenMagEmpty, weapDef->noAdsWhenMagEmpty);
			COPY_FIELD(avoidDropCleanup, weapDef->avoidDropCleanup);
			COPY_FIELD(inheritsPerks, weapDef->inheritsPerks);
			COPY_FIELD(crosshairColorChange, weapDef->crosshairColorChange);
			COPY_FIELD(rifleBullet, weapDef->bRifleBullet);
			COPY_FIELD(armorPiercing, weapDef->armorPiercing);
			COPY_FIELD(boltAction, weapDef->bBoltAction);
			COPY_FIELD(aimDownSight, weapDef->aimDownSight);
			COPY_FIELD(canHoldBreath, weapDef->canHoldBreath);
			h1_asset->meleeOnly = false;
			h1_asset->quickMelee = false;
			h1_asset->bU_086 = false;
			COPY_FIELD(canVariableZoom, weapDef->canVariableZoom);
			COPY_FIELD(rechamberWhileAds, weapDef->bRechamberWhileAds);
			COPY_FIELD(bulletExplosiveDamage, weapDef->bBulletExplosiveDamage);
			COPY_FIELD(cookOffHold, weapDef->bCookOffHold);
			h1_asset->useBattery = false;
			h1_asset->reticleSpin45 = false;
			COPY_FIELD(clipOnly, weapDef->bClipOnly);
			COPY_FIELD(noAmmoPickup, weapDef->noAmmoPickup);
			COPY_FIELD(disableSwitchToWhenEmpty, weapDef->disableSwitchToWhenEmpty);
			COPY_FIELD(suppressAmmoReserveDisplay, weapDef->suppressAmmoReserveDisplay);
			COPY_FIELD(motionTracker, motionTracker);
			COPY_FIELD(markableViewmodel, weapDef->markableViewmodel);
			COPY_FIELD(noDualWield, weapDef->noDualWield);
			COPY_FIELD(flipKillIcon, weapDef->flipKillIcon);
			h1_asset->actionSlotShowAmmo = true;
			COPY_FIELD(noPartialReload, weapDef->bNoPartialReload);
			COPY_FIELD(segmentedReload, weapDef->bSegmentedReload);
			h1_asset->multipleReload = false;
			COPY_FIELD(blocksProne, weapDef->blocksProne);
			COPY_FIELD(silenced, weapDef->silenced);
			COPY_FIELD(isRollingGrenade, weapDef->isRollingGrenade);
			COPY_FIELD(projExplosionEffectForceNormalUp, weapDef->projExplosionEffectForceNormalUp);
			h1_asset->projExplosionEffectInheritParentDirection = false;
			COPY_FIELD(projImpactExplode, weapDef->bProjImpactExplode);
			h1_asset->projTrajectoryEvents = false;
			h1_asset->projWhizByEnabled = false;
			COPY_FIELD(stickToPlayers, weapDef->stickToPlayers);
			COPY_FIELD(stickToVehicles, weapDef->stickToVehicles);
			COPY_FIELD(stickToTurrets, weapDef->stickToTurrets);
			h1_asset->thrownSideways = false;
			h1_asset->hasDetonatorEmptyThrow = false; // figure this out? should be true for c4
			h1_asset->hasDetonatorDoubleTap = false; // figure this out? should be true for c4
			COPY_FIELD(disableFiring, weapDef->disableFiring);
			COPY_FIELD(timedDetonation, weapDef->timedDetonation);
			h1_asset->noCrumpleMissile = false;
			h1_asset->fuseLitAfterImpact = false;
			COPY_FIELD(rotate, weapDef->rotate);
			COPY_FIELD(holdButtonToThrow, weapDef->holdButtonToThrow);
			COPY_FIELD(freezeMovementWhenFiring, weapDef->freezeMovementWhenFiring);
			COPY_FIELD(thermalScope, weapDef->thermalScope);
			h1_asset->thermalToggle = false;
			h1_asset->outlineEnemies = false;
			COPY_FIELD(altModeSameWeapon, weapDef->altModeSameWeapon);
			COPY_FIELD(turretBarrelSpinEnabled, weapDef->turretBarrelSpinEnabled);
			COPY_FIELD(missileConeSoundEnabled, weapDef->missileConeSoundEnabled);
			COPY_FIELD(missileConeSoundPitchshiftEnabled, weapDef->missileConeSoundPitchshiftEnabled);
			COPY_FIELD(missileConeSoundCrossfadeEnabled, weapDef->missileConeSoundCrossfadeEnabled);
			COPY_FIELD(offhandHoldIsCancelable, weapDef->offhandHoldIsCancelable);
			COPY_FIELD(doNotAllowAttachmentsToOverrideSpread, weapDef->doNotAllowAttachmentsToOverrideSpread);
			h1_asset->useFastReloadAnims = false;
			h1_asset->dualMagReloadSupported = false;
			h1_asset->reloadStopsAlt = false;
			h1_asset->useScopeDrift = false;
			h1_asset->alwaysShatterGlassOnImpact = false;
			h1_asset->oldWeapon = false; // true or false? what does this even do?
			h1_asset->raiseToHold = false;
			h1_asset->notifyOnPlayerImpact = false;
			h1_asset->decreasingKick = false;
			h1_asset->counterSilencer = false;
			h1_asset->projSuppressedByEMP = false;
			h1_asset->projDisabledByEMP = false;
			h1_asset->autosimDisableVariableRate = false;
			h1_asset->projPlayTrailEffectForOwnerOnly = false;
			h1_asset->projPlayBeaconEffectForOwnerOnly = false;
			h1_asset->projKillTrailEffectOnDeath = false;
			h1_asset->projKillBeaconEffectOnDeath = false;
			h1_asset->reticleDetonateHide = false;
			h1_asset->cloaked = false;
			h1_asset->adsHideWeapon = false; // should we hide these for snipers?
			h1_asset->adsHideHands = false; // should we hide these for snipers?
			h1_asset->bU_108 = false;
			h1_asset->adsSceneBlur = false; // should we enable this for snipers?
			h1_asset->usesSniperScope = false; // should we enable this for snipers?
			h1_asset->hasTransientModels = false; // modify this later if needed
			h1_asset->signatureAmmoAlternate = false;
			h1_asset->useScriptCallbackForHit = false;
			h1_asset->useBulletTagSystem = false;
			h1_asset->hideBulletTags = false;
			h1_asset->adsDofPhysicalFstop = 45.0f;
			h1_asset->adsDofPhysicalFocusDistance = 10.0f;
			h1_asset->autosimSpeedScale = 1.0f;
			h1_asset->reactiveMotionRadiusScale = 0.0f;
			h1_asset->reactiveMotionFrequencyScale = 0.0f;
			h1_asset->reactiveMotionAmplitudeScale = 0.0f;
			h1_asset->reactiveMotionFalloff = 0.0f;
			h1_asset->reactiveMotionLifetime = 0.0f;
			h1_asset->fU_3604[0] = 30.0f;
			h1_asset->fU_3604[1] = 0.0f;
			h1_asset->fU_3604[2] = 0.5899999737739563f;

			return h1_asset;
		}

		H1::WeaponDef* convert(WeaponCompleteDef* asset, allocator& allocator)
		{
			// generate h1 weapon
			return GenerateWeaponDef(asset, allocator);
		}
	}
}