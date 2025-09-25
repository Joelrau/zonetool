#include "stdafx.hpp"
#include "../Include.hpp"

#include "WeaponAttachment.hpp"

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

		H1::WeaponAttachment* GenerateH1Attachment(WeaponAttachment* asset, allocator& mem)
		{
			const auto h1_asset = mem.allocate<H1::WeaponAttachment>();

			REINTERPRET_CAST(szInternalName);
			REINTERPRET_CAST(szDisplayName);
			h1_asset->type = static_cast<H1::AttachmentType>(asset->type > ATTACHMENT_UNDERBARREL ? asset->type + 1 : asset->type);
			COPY_FIELD_CAST(weaponType); // same enum values
			COPY_FIELD_CAST(weapClass); // same enum values
			h1_asset->greebleType = H1::WEAPON_GREEBLE_NONE;

			if (asset->worldModels)
			{
				h1_asset->worldModels = mem.allocate<H1::XModel PTR64>(2);
				h1_asset->worldModels[0] = reinterpret_cast<H1::XModel*>(asset->worldModels[0]);
				h1_asset->worldModels[1] = nullptr;
			}

			if (asset->viewModels)
			{
				h1_asset->viewModels = mem.allocate<H1::XModel PTR64>(2);
				h1_asset->viewModels[0] = reinterpret_cast<H1::XModel*>(asset->viewModels[0]);
				h1_asset->viewModels[1] = nullptr;
			}

			if (asset->reticleViewModels)
			{
				h1_asset->reticleViewModels = mem.allocate<H1::XModel PTR64>(64);
				for (auto i = 0; i < 8; i++)
				{
					h1_asset->reticleViewModels[i] = reinterpret_cast<H1::XModel*>(asset->reticleViewModels[i]);
				}
			}

			h1_asset->bounceSounds = nullptr;
			h1_asset->rollingSounds = nullptr;

			h1_asset->chargeInfo = nullptr;
			h1_asset->hybridSettings = nullptr;

			h1_asset->hideTags = nullptr;
			h1_asset->showTags = nullptr;

			COPY_FIELD(loadIndex);
			h1_asset->unused1 = 0;
			h1_asset->unused2 = 0;

			h1_asset->isAlternateAmmo = false;
			COPY_FIELD(hideIronSightsWithThisAttachment);
			h1_asset->showMasterRail = false;
			h1_asset->showSideRail = false;
			COPY_FIELD(shareAmmoWithAlt);
			h1_asset->knifeAlwaysAttached = false;
			h1_asset->riotShield = false;
			h1_asset->automaticAttachment = false;
			COPY_FIELD(hideIronSightsWithThisAttachment);

			struct WeaponAttachmentField
			{
				H1::WAField field;
				unsigned short offset;
			};
			std::vector<WeaponAttachmentField> fields;
			const auto addField = [&](H1::WAFieldCode code, H1::WAFieldType type, H1::WAFieldParm parm, unsigned short offset, unsigned char index = 0)
			{
				WeaponAttachmentField f{};
				f.field.code = code;
				f.field.type = type;
				f.field.parm = parm;
				f.field.index = index;
				f.offset = offset;
				fields.push_back(f);
			};

			float ammunitionScale = 1.0f;
			float damageScale = 1.0f;
			float damageScaleMin = 1.0f;
			float idleSettingsScale = 1.0f;
			float adsSettingsScale = 1.0f;
			float adsSettingsScaleMain = 1.0f;
			float hipSpreadScale = 1.0f;
			float gunKickScale = 1.0f;
			float viewKickScale = 1.0f;
			float viewCenterScale = 1.0f;

			if (asset->ammunitionScale != 0.0f)
			{
				ammunitionScale = asset->ammunitionScale;
			}
			if (asset->damageScale != 0.0f)
			{
				damageScale = asset->damageScale;
			}
			if (asset->damageScaleMin != 0.0f)
			{
				damageScaleMin = asset->damageScaleMin;
			}
			if (asset->idleSettingsScale != 0.0f)
			{
				idleSettingsScale = asset->idleSettingsScale;
			}
			if (asset->adsSettingsScale != 0.0f)
			{
				adsSettingsScale = asset->adsSettingsScale;
			}
			if (asset->adsSettingsScaleMain != 0.0f)
			{
				adsSettingsScaleMain = asset->adsSettingsScaleMain;
			}
			if (asset->hipSpreadScale != 0.0f)
			{
				hipSpreadScale = asset->hipSpreadScale;
			}
			if (asset->gunKickScale != 0.0f)
			{
				gunKickScale = asset->gunKickScale;
			}
			if (asset->viewKickScale != 0.0f)
			{
				viewKickScale = asset->viewKickScale;
			}
			if (asset->viewKickScale != 0.0f)
			{
				viewKickScale = asset->viewKickScale;
			}
			if (asset->viewCenterScale != 0.0f)
			{
				viewCenterScale = asset->viewCenterScale;
			}

			if (asset->ammoGeneral)
			{
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_INT, { .p_int = asset->ammoGeneral->penetrateType }, offsetof(H1::WeaponDef, penetrateType));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->ammoGeneral->penetrateMultiplier }, offsetof(H1::WeaponDef, penetrateDepth));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_INT,
					{ .p_int = asset->ammoGeneral->impactType > 6 ? asset->ammoGeneral->impactType + 1 : asset->ammoGeneral->impactType }, offsetof(H1::WeaponDef, impactType));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_INT,
					{ .p_int = asset->ammoGeneral->fireType > 4 ? asset->ammoGeneral->fireType + 1 : asset->ammoGeneral->fireType }, offsetof(H1::WeaponDef, fireType));
				if (asset->ammoGeneral->tracerType)
				{
					addField(H1::FIELD_OP_STRING_SET, H1::WAFIELD_TYPE_TRACER, { .string = asset->ammoGeneral->tracerType->name }, offsetof(H1::WeaponDef, tracerType));
				}
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_BOOL, { .p_bool = asset->ammoGeneral->rifleBullet }, offsetof(H1::WeaponDef, rifleBullet));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_BOOL, { .p_bool = asset->ammoGeneral->armorPiercing }, offsetof(H1::WeaponDef, armorPiercing));
			}

			if (asset->sight)
			{
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_BOOL, { .p_bool = asset->sight->aimDownSight }, offsetof(H1::WeaponDef, aimDownSight));
				//addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_BOOL, { .p_bool = asset->sight->adsFire }, offsetof(H1::WeaponDef, adsFireOnly));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_BOOL, { .p_bool = asset->sight->rechamberWhileAds }, offsetof(H1::WeaponDef, rechamberWhileAds));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_BOOL, { .p_bool = asset->sight->noAdsWhenMagEmpty }, offsetof(H1::WeaponDef, noAdsWhenMagEmpty));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_BOOL, { .p_bool = asset->sight->canHoldBreath }, offsetof(H1::WeaponDef, canHoldBreath));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_BOOL, { .p_bool = asset->sight->canVariableZoom }, offsetof(H1::WeaponDef, canVariableZoom));
				h1_asset->showSideRail = asset->sight->hideRailWithThisScope == false;
				h1_asset->showMasterRail = asset->sight->hideRailWithThisScope == false;
			}

			if (asset->reload)
			{
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_BOOL, { .p_bool = asset->reload->noPartialReload }, offsetof(H1::WeaponDef, noPartialReload));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_BOOL, { .p_bool = asset->reload->segmentedReload }, offsetof(H1::WeaponDef, segmentedReload));
			}

			if (asset->addOns)
			{
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_BOOL, { .p_bool = asset->addOns->motionTracker }, offsetof(H1::WeaponDef, motionTracker));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_BOOL, { .p_bool = asset->addOns->silenced }, offsetof(H1::WeaponDef, silenced));
			}

			if (asset->general)
			{
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_BOOL, { .p_bool = asset->general->boltAction }, offsetof(H1::WeaponDef, boltAction));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_BOOL, { .p_bool = asset->general->inheritsPerks }, offsetof(H1::WeaponDef, inheritsPerks));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->general->enemyCrosshairRange }, offsetof(H1::WeaponDef, enemyCrosshairRange));
				if (asset->general->reticleCenter)
				{
					addField(H1::FIELD_OP_STRING_SET, H1::WAFIELD_TYPE_MATERIAL, { .string = asset->general->reticleCenter->name }, offsetof(H1::WeaponDef, reticleCenter));
				}
				if (asset->general->reticleSide)
				{
					addField(H1::FIELD_OP_STRING_SET, H1::WAFIELD_TYPE_MATERIAL, { .string = asset->general->reticleSide->name }, offsetof(H1::WeaponDef, reticleSide));
				}
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_INT, { .p_int = asset->general->reticleCenterSize }, offsetof(H1::WeaponDef, reticleCenterSize));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_INT, { .p_int = asset->general->reticleSideSize }, offsetof(H1::WeaponDef, reticleSideSize));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->general->moveSpeedScale }, offsetof(H1::WeaponDef, moveSpeedScale));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->general->adsMoveSpeedScale }, offsetof(H1::WeaponDef, adsMoveSpeedScale));
			}

			if (asset->aimAssist)
			{
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->aimAssist->autoAimRange }, offsetof(H1::WeaponDef, autoAimRange));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->aimAssist->aimAssistRange }, offsetof(H1::WeaponDef, aimAssistRange));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->aimAssist->aimAssistRangeAds }, offsetof(H1::WeaponDef, aimAssistRangeAds));
			}

			if (asset->ammunition)
			{
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_INT, { .p_int = (int)std::round(asset->ammunition->maxAmmo * ammunitionScale) }, offsetof(H1::WeaponDef, maxAmmo));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_INT, { .p_int = (int)std::round(asset->ammunition->startAmmo * ammunitionScale) }, offsetof(H1::WeaponDef, startAmmo));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_INT, { .p_int = (int)std::round(asset->ammunition->clipSize * ammunitionScale) }, offsetof(H1::WeaponDef, clipSize));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_INT, { .p_int = asset->ammunition->shotCount }, offsetof(H1::WeaponDef, shotCount));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_INT, { .p_int = (int)std::round(asset->ammunition->reloadAmmoAdd * ammunitionScale) }, offsetof(H1::WeaponDef, reloadAmmoAdd));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_INT, { .p_int = asset->ammunition->reloadStartAdd }, offsetof(H1::WeaponDef, reloadStartAdd));
			}

			if (asset->damage)
			{
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_INT, { .p_int = asset->damage->damage }, offsetof(H1::WeaponDef, damage));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_INT, { .p_int = asset->damage->minDamage }, offsetof(H1::WeaponDef, minDamage));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_INT, { .p_int = asset->damage->meleeDamage }, offsetof(H1::WeaponDef, meleeDamage));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->damage->maxDamageRange * damageScale }, offsetof(H1::WeaponDef, maxDamageRange));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->damage->minDamageRange * damageScaleMin }, offsetof(H1::WeaponDef, minDamageRange));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_INT, { .p_int = asset->damage->playerDamage }, offsetof(H1::WeaponDef, playerDamage));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_INT, { .p_int = asset->damage->minPlayerDamage }, offsetof(H1::WeaponDef, minPlayerDamage));
			}

			if (asset->locationDamage)
			{
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->locationDamage->locNone }, offsetof(H1::WeaponDef, locationDamageMultipliers), H1::HITLOC_NONE);
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->locationDamage->locHelmet }, offsetof(H1::WeaponDef, locationDamageMultipliers), H1::HITLOC_HELMET);
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->locationDamage->locHead }, offsetof(H1::WeaponDef, locationDamageMultipliers), H1::HITLOC_HEAD);
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->locationDamage->locNeck }, offsetof(H1::WeaponDef, locationDamageMultipliers), H1::HITLOC_NECK);
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->locationDamage->locTorsoUpper }, offsetof(H1::WeaponDef, locationDamageMultipliers), H1::HITLOC_TORSO_UPR);
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->locationDamage->locTorsoLower }, offsetof(H1::WeaponDef, locationDamageMultipliers), H1::HITLOC_TORSO_LWR);
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->locationDamage->locRightArmUpper }, offsetof(H1::WeaponDef, locationDamageMultipliers), H1::HITLOC_R_ARM_UPR);
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->locationDamage->locRightArmLower }, offsetof(H1::WeaponDef, locationDamageMultipliers), H1::HITLOC_L_ARM_UPR);
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->locationDamage->locRightHand }, offsetof(H1::WeaponDef, locationDamageMultipliers), H1::HITLOC_R_ARM_LWR);
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->locationDamage->locLeftArmUpper }, offsetof(H1::WeaponDef, locationDamageMultipliers), H1::HITLOC_L_ARM_LWR);
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->locationDamage->locLeftArmLower }, offsetof(H1::WeaponDef, locationDamageMultipliers), H1::HITLOC_R_HAND);
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->locationDamage->locLeftHand }, offsetof(H1::WeaponDef, locationDamageMultipliers), H1::HITLOC_L_HAND);
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->locationDamage->locRightLegUpper }, offsetof(H1::WeaponDef, locationDamageMultipliers), H1::HITLOC_R_LEG_UPR);
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->locationDamage->locRightLegLower }, offsetof(H1::WeaponDef, locationDamageMultipliers), H1::HITLOC_L_LEG_UPR);
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->locationDamage->locRightFoot }, offsetof(H1::WeaponDef, locationDamageMultipliers), H1::HITLOC_R_LEG_LWR);
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->locationDamage->locLeftLegUpper }, offsetof(H1::WeaponDef, locationDamageMultipliers), H1::HITLOC_L_LEG_LWR);
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->locationDamage->locLeftLegLower }, offsetof(H1::WeaponDef, locationDamageMultipliers), H1::HITLOC_R_FOOT);
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->locationDamage->locLeftFoot }, offsetof(H1::WeaponDef, locationDamageMultipliers), H1::HITLOC_L_FOOT);
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->locationDamage->locGun }, offsetof(H1::WeaponDef, locationDamageMultipliers), H1::HITLOC_GUN);
			}

			if (asset->idleSettings)
			{
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->idleSettings->hipIdleAmount }, offsetof(H1::WeaponDef, hipIdleAmount));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->idleSettings->hipIdleSpeed }, offsetof(H1::WeaponDef, hipIdleSpeed));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->idleSettings->idleCrouchFactor * idleSettingsScale }, offsetof(H1::WeaponDef, idleCrouchFactor));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->idleSettings->idleProneFactor * idleSettingsScale }, offsetof(H1::WeaponDef, idleProneFactor));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->idleSettings->adsIdleLerpStartTime * idleSettingsScale }, offsetof(H1::WeaponDef, adsIdleLerpStartTime));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->idleSettings->adsIdleLerpTime * idleSettingsScale }, offsetof(H1::WeaponDef, adsIdleLerpTime));
			}

			if (!asset->shareAmmoWithAlt)
			{
				if (asset->adsSettings)
				{
					addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->adsSettings->adsSpread * adsSettingsScaleMain }, offsetof(H1::WeaponDef, adsSpread));
					addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->adsSettings->adsAimPitch * adsSettingsScaleMain }, offsetof(H1::WeaponDef, adsAimPitch));
					addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT32, { .p_float = asset->adsSettings->adsTransInTime * adsSettingsScaleMain }, offsetof(H1::WeaponDef, adsTransInTime));
					addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT32, { .p_float = asset->adsSettings->adsTransOutTime * adsSettingsScaleMain }, offsetof(H1::WeaponDef, adsTransOutTime));
					addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_INT, { .p_int = (int)std::round(asset->adsSettings->adsReloadTransTime * adsSettingsScaleMain) }, offsetof(H1::WeaponDef, positionReloadTransTime));
					addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->adsSettings->adsCrosshairInFrac }, offsetof(H1::WeaponDef, adsCrosshairInFrac));
					addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->adsSettings->adsCrosshairOutFrac }, offsetof(H1::WeaponDef, adsCrosshairOutFrac));
					addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->adsSettings->adsZoomFov * adsSettingsScaleMain }, offsetof(H1::WeaponDef, adsZoomFov));
					addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->adsSettings->adsZoomInFrac * adsSettingsScaleMain }, offsetof(H1::WeaponDef, adsZoomInFrac));
					addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->adsSettings->adsZoomOutFrac * adsSettingsScaleMain }, offsetof(H1::WeaponDef, adsZoomOutFrac));
					addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->adsSettings->adsBobFactor * adsSettingsScaleMain }, offsetof(H1::WeaponDef, adsBobFactor));
					addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->adsSettings->adsViewBobMult * adsSettingsScaleMain }, offsetof(H1::WeaponDef, adsViewBobMult));
					addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->adsSettings->adsViewErrorMin }, offsetof(H1::WeaponDef, adsViewErrorMin));
					addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->adsSettings->adsViewErrorMax }, offsetof(H1::WeaponDef, adsViewErrorMax));
				}
			}
			else
			{
				if (asset->adsSettingsMain)
				{
					addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->adsSettingsMain->adsSpread * adsSettingsScaleMain }, offsetof(H1::WeaponDef, adsSpread));
					addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->adsSettingsMain->adsAimPitch * adsSettingsScaleMain }, offsetof(H1::WeaponDef, adsAimPitch));
					addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->adsSettingsMain->adsTransInTime * adsSettingsScaleMain }, offsetof(H1::WeaponDef, adsTransInTime));
					addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->adsSettingsMain->adsTransOutTime * adsSettingsScaleMain }, offsetof(H1::WeaponDef, adsTransOutTime));
					addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_INT, { .p_int = (int)std::round(asset->adsSettingsMain->adsReloadTransTime * adsSettingsScaleMain) }, offsetof(H1::WeaponDef, positionReloadTransTime));
					addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->adsSettingsMain->adsCrosshairInFrac }, offsetof(H1::WeaponDef, adsCrosshairInFrac));
					addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->adsSettingsMain->adsCrosshairOutFrac }, offsetof(H1::WeaponDef, adsCrosshairOutFrac));
					addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->adsSettingsMain->adsZoomFov * adsSettingsScaleMain }, offsetof(H1::WeaponDef, adsZoomFov));
					addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->adsSettingsMain->adsZoomInFrac * adsSettingsScaleMain }, offsetof(H1::WeaponDef, adsZoomInFrac));
					addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->adsSettingsMain->adsZoomOutFrac * adsSettingsScaleMain }, offsetof(H1::WeaponDef, adsZoomOutFrac));
					addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->adsSettingsMain->adsBobFactor * adsSettingsScaleMain }, offsetof(H1::WeaponDef, adsBobFactor));
					addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->adsSettingsMain->adsViewBobMult * adsSettingsScaleMain }, offsetof(H1::WeaponDef, adsViewBobMult));
					addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->adsSettingsMain->adsViewErrorMin }, offsetof(H1::WeaponDef, adsViewErrorMin));
					addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->adsSettingsMain->adsViewErrorMax }, offsetof(H1::WeaponDef, adsViewErrorMax));
				}
			}

			if (asset->hipSpread)
			{
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->hipSpread->hipSpreadStandMin * hipSpreadScale }, offsetof(H1::WeaponDef, hipSpreadStandMin));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->hipSpread->hipSpreadDuckedMin * hipSpreadScale }, offsetof(H1::WeaponDef, hipSpreadDuckedMin));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->hipSpread->hipSpreadProneMin * hipSpreadScale }, offsetof(H1::WeaponDef, hipSpreadProneMin));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->hipSpread->hipSpreadMax * hipSpreadScale }, offsetof(H1::WeaponDef, hipSpreadStandMax));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->hipSpread->hipSpreadDuckedMax * hipSpreadScale }, offsetof(H1::WeaponDef, hipSpreadDuckedMax));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->hipSpread->hipSpreadProneMax * hipSpreadScale }, offsetof(H1::WeaponDef, hipSpreadProneMax));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->hipSpread->hipSpreadFireAdd * hipSpreadScale }, offsetof(H1::WeaponDef, hipSpreadFireAdd));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->hipSpread->hipSpreadTurnAdd * hipSpreadScale }, offsetof(H1::WeaponDef, hipSpreadTurnAdd));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->hipSpread->hipSpreadMoveAdd * hipSpreadScale }, offsetof(H1::WeaponDef, hipSpreadMoveAdd));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->hipSpread->hipSpreadDecayRate * hipSpreadScale }, offsetof(H1::WeaponDef, hipSpreadDecayRate));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->hipSpread->hipSpreadDuckedDecay * hipSpreadScale }, offsetof(H1::WeaponDef, hipSpreadDuckedDecay));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->hipSpread->hipSpreadProneDecay * hipSpreadScale }, offsetof(H1::WeaponDef, hipSpreadProneDecay));
			}

			if (asset->gunKick)
			{
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_INT, { .p_int = (int)std::round(asset->gunKick->hipGunKickReducedKickBullets * gunKickScale) }, offsetof(H1::WeaponDef, hipGunKickReducedKickBullets));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->gunKick->hipGunKickReducedKickPercent * gunKickScale }, offsetof(H1::WeaponDef, hipGunKickReducedKickPercent));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->gunKick->hipGunKickPitchMin * gunKickScale }, offsetof(H1::WeaponDef, hipGunKickPitchMin));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->gunKick->hipGunKickPitchMax * gunKickScale }, offsetof(H1::WeaponDef, hipGunKickPitchMax));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->gunKick->hipGunKickYawMin * gunKickScale }, offsetof(H1::WeaponDef, hipGunKickYawMin));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->gunKick->hipGunKickYawMax * gunKickScale }, offsetof(H1::WeaponDef, hipGunKickYawMax));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->gunKick->hipGunKickAccel * gunKickScale }, offsetof(H1::WeaponDef, hipGunKickAccel));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->gunKick->hipGunKickSpeedMax * gunKickScale }, offsetof(H1::WeaponDef, hipGunKickSpeedMax));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->gunKick->hipGunKickSpeedDecay * gunKickScale }, offsetof(H1::WeaponDef, hipGunKickSpeedDecay));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->gunKick->hipGunKickStaticDecay * gunKickScale }, offsetof(H1::WeaponDef, hipGunKickStaticDecay));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_INT, { .p_int = (int)std::round(asset->gunKick->adsGunKickReducedKickBullets * gunKickScale) }, offsetof(H1::WeaponDef, adsGunKickReducedKickBullets));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->gunKick->adsGunKickReducedKickPercent * gunKickScale }, offsetof(H1::WeaponDef, adsGunKickReducedKickPercent));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->gunKick->adsGunKickPitchMin * gunKickScale }, offsetof(H1::WeaponDef, adsGunKickPitchMin));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->gunKick->adsGunKickPitchMax * gunKickScale }, offsetof(H1::WeaponDef, adsGunKickPitchMax));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->gunKick->adsGunKickYawMin * gunKickScale }, offsetof(H1::WeaponDef, adsGunKickYawMin));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->gunKick->adsGunKickYawMax * gunKickScale }, offsetof(H1::WeaponDef, adsGunKickYawMax));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->gunKick->adsGunKickAccel * gunKickScale }, offsetof(H1::WeaponDef, adsGunKickAccel));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->gunKick->adsGunKickSpeedMax * gunKickScale }, offsetof(H1::WeaponDef, adsGunKickSpeedMax));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->gunKick->adsGunKickSpeedDecay * gunKickScale }, offsetof(H1::WeaponDef, adsGunKickSpeedDecay));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->gunKick->adsGunKickStaticDecay * gunKickScale }, offsetof(H1::WeaponDef, adsGunKickStaticDecay));
			}

			if (asset->viewKick)
			{
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->viewKick->hipViewKickPitchMin * viewKickScale }, offsetof(H1::WeaponDef, hipViewKickPitchMin));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->viewKick->hipViewKickPitchMax * viewKickScale }, offsetof(H1::WeaponDef, hipViewKickPitchMax));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->viewKick->hipViewKickYawMin * viewKickScale }, offsetof(H1::WeaponDef, hipViewKickYawMin));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->viewKick->hipViewKickYawMax * viewKickScale }, offsetof(H1::WeaponDef, hipViewKickYawMax));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->viewKick->hipViewKickCenterSpeed * viewCenterScale }, offsetof(H1::WeaponDef, hipViewKickCenterSpeed));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->viewKick->adsViewKickPitchMin * viewKickScale }, offsetof(H1::WeaponDef, adsViewKickPitchMin));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->viewKick->adsViewKickPitchMax * viewKickScale }, offsetof(H1::WeaponDef, adsViewKickPitchMax));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->viewKick->adsViewKickYawMin * viewKickScale }, offsetof(H1::WeaponDef, adsViewKickYawMin));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->viewKick->adsViewKickYawMax * viewKickScale }, offsetof(H1::WeaponDef, adsViewKickYawMax));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->viewKick->adsViewKickCenterSpeed * viewCenterScale }, offsetof(H1::WeaponDef, adsViewKickCenterSpeed));
			}

			if (asset->adsOverlay)
			{
				if (asset->adsOverlay->overlay.shader)
				{
					addField(H1::FIELD_OP_STRING_SET, H1::WAFIELD_TYPE_MATERIAL, { .string = asset->adsOverlay->overlay.shader->name }, offsetof(H1::WeaponDef, overlay.shader));
				}
				if (asset->adsOverlay->overlay.shaderLowRes)
				{
					addField(H1::FIELD_OP_STRING_SET, H1::WAFIELD_TYPE_MATERIAL, { .string = asset->adsOverlay->overlay.shaderLowRes->name }, offsetof(H1::WeaponDef, overlay.shaderLowRes));
				}
				if (asset->adsOverlay->overlay.shaderEMP)
				{
					addField(H1::FIELD_OP_STRING_SET, H1::WAFIELD_TYPE_MATERIAL, { .string = asset->adsOverlay->overlay.shaderEMP->name }, offsetof(H1::WeaponDef, overlay.shaderEMP));
				}
				if (asset->adsOverlay->overlay.shaderEMPLowRes)
				{
					addField(H1::FIELD_OP_STRING_SET, H1::WAFIELD_TYPE_MATERIAL, { .string = asset->adsOverlay->overlay.shaderEMPLowRes->name }, offsetof(H1::WeaponDef, overlay.shaderEMPLowRes));
				}
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_INT, { .p_int = asset->adsOverlay->overlay.reticle }, offsetof(H1::WeaponDef, overlay.reticle));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->adsOverlay->overlay.width }, offsetof(H1::WeaponDef, overlay.width));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->adsOverlay->overlay.height }, offsetof(H1::WeaponDef, overlay.height));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->adsOverlay->overlay.widthSplitscreen }, offsetof(H1::WeaponDef, overlay.widthSplitscreen));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->adsOverlay->overlay.heightSplitscreen }, offsetof(H1::WeaponDef, overlay.heightSplitscreen));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_BOOL, { .p_bool = asset->adsOverlay->thermalScope }, offsetof(H1::WeaponDef, thermalScope));
			}

			if (asset->ui)
			{
				if (asset->ui->dpadIcon)
				{
					addField(H1::FIELD_OP_STRING_SET, H1::WAFIELD_TYPE_MATERIAL, { .string = asset->ui->dpadIcon->name }, offsetof(H1::WeaponDef, dpadIcon));
				}
				if (asset->ui->ammoCounterIcon)
				{
					addField(H1::FIELD_OP_STRING_SET, H1::WAFIELD_TYPE_MATERIAL, { .string = asset->ui->ammoCounterIcon->name }, offsetof(H1::WeaponDef, ammoCounterIcon));
				}
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_INT, { .p_int = asset->ui->dpadIconRatio }, offsetof(H1::WeaponDef, dpadIconRatio));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_INT, { .p_int = asset->ui->ammoCounterIconRatio }, offsetof(H1::WeaponDef, ammoCounterIconRatio));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_INT, { .p_int = asset->ui->ammoCounterClip }, offsetof(H1::WeaponDef, ammoCounterClip));
			}

			if (asset->rumbles)
			{
				if (asset->rumbles->fireRumble)
				{
					addField(H1::FIELD_OP_STRING_SET, H1::WAFIELD_TYPE_STRING, { .string = asset->rumbles->fireRumble }, offsetof(H1::WeaponDef, fireRumble));
				}
				if (asset->rumbles->meleeImpactRumble)
				{
					addField(H1::FIELD_OP_STRING_SET, H1::WAFIELD_TYPE_STRING, { .string = asset->rumbles->meleeImpactRumble }, offsetof(H1::WeaponDef, meleeImpactRumble));
				}
			}

			if (asset->projectile)
			{
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_INT, { .p_int = asset->projectile->explosionRadius }, offsetof(H1::WeaponDef, explosionRadius));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_INT, { .p_int = asset->projectile->explosionInnerDamage }, offsetof(H1::WeaponDef, explosionInnerDamage));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_INT, { .p_int = asset->projectile->explosionOuterDamage }, offsetof(H1::WeaponDef, explosionOuterDamage));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->projectile->damageConeAngle }, offsetof(H1::WeaponDef, damageConeAngle));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_INT, { .p_int = asset->projectile->projectileSpeed }, offsetof(H1::WeaponDef, projectileSpeed));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_INT, { .p_int = asset->projectile->projectileSpeedUp }, offsetof(H1::WeaponDef, projectileSpeedUp));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_INT, { .p_int = asset->projectile->projectileActivateDist }, offsetof(H1::WeaponDef, projectileActivateDist));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->projectile->projectileLifetime }, offsetof(H1::WeaponDef, projLifetime));
				if (asset->projectile->projectileModel)
				{
					addField(H1::FIELD_OP_STRING_SET, H1::WAFIELD_TYPE_MODEL, { .string = asset->projectile->projectileModel->name }, offsetof(H1::WeaponDef, projectileModel));
				}
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_INT, { .p_int = asset->projectile->projExplosionType }, offsetof(H1::WeaponDef, projExplosion));
				if (asset->projectile->projExplosionEffect)
				{
					addField(H1::FIELD_OP_STRING_SET, H1::WAFIELD_TYPE_FX, { .string = asset->projectile->projExplosionEffect->name }, offsetof(H1::WeaponDef, projExplosionEffect));
				}
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_BOOL, { .p_bool = asset->projectile->projExplosionEffectForceNormalUp }, offsetof(H1::WeaponDef, projExplosionEffectForceNormalUp));
				if (asset->projectile->projExplosionSound.sound)
				{
					addField(H1::FIELD_OP_STRING_SET, H1::WAFIELD_TYPE_SOUND, { .string = asset->projectile->projExplosionSound.sound->aliasName }, offsetof(H1::WeaponDef, projExplosionSound));
				}
				if (asset->projectile->projDudEffect)
				{
					addField(H1::FIELD_OP_STRING_SET, H1::WAFIELD_TYPE_FX, { .string = asset->projectile->projDudEffect->name }, offsetof(H1::WeaponDef, projDudEffect));
				}
				if (asset->projectile->projDudSound.sound)
				{
					addField(H1::FIELD_OP_STRING_SET, H1::WAFIELD_TYPE_SOUND, { .string = asset->projectile->projDudSound.sound->aliasName }, offsetof(H1::WeaponDef, projDudSound));
				}
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_BOOL, { .p_bool = asset->projectile->projImpactExplode }, offsetof(H1::WeaponDef, projImpactExplode));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->projectile->destabilizationRateTime }, offsetof(H1::WeaponDef, destabilizationRateTime));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->projectile->destabilizationCurvatureMax }, offsetof(H1::WeaponDef, destabilizationCurvatureMax));
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_INT, { .p_int = asset->projectile->destabilizeDistance }, offsetof(H1::WeaponDef, destabilizeDistance));
				if (asset->projectile->projTrailEffect)
				{
					addField(H1::FIELD_OP_STRING_SET, H1::WAFIELD_TYPE_FX, { .string = asset->projectile->projTrailEffect->name }, offsetof(H1::WeaponDef, projTrailEffect));
				}
				addField(H1::FIELD_OP_NUMBER_SET, H1::WAFIELD_TYPE_INT, { .p_int = asset->projectile->projIgnitionDelay }, offsetof(H1::WeaponDef, projIgnitionDelay));
				if (asset->projectile->projIgnitionEffect)
				{
					addField(H1::FIELD_OP_STRING_SET, H1::WAFIELD_TYPE_FX, { .string = asset->projectile->projIgnitionEffect->name }, offsetof(H1::WeaponDef, projIgnitionEffect));
				}
				if (asset->projectile->projIgnitionSound.sound)
				{
					addField(H1::FIELD_OP_STRING_SET, H1::WAFIELD_TYPE_SOUND, { .string = asset->projectile->projIgnitionSound.sound->aliasName }, offsetof(H1::WeaponDef, projIgnitionSound));
				}
			}

			if (ammunitionScale != 0.0f && ammunitionScale != 1.0f && asset->ammunition == nullptr)
			{
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = ammunitionScale }, offsetof(H1::WeaponDef, clipSize));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = ammunitionScale }, offsetof(H1::WeaponDef, maxAmmo));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = ammunitionScale }, offsetof(H1::WeaponDef, reloadAmmoAdd));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = ammunitionScale }, offsetof(H1::WeaponDef, startAmmo));
			}
			if (damageScale != 0.0f && damageScale != 1.0f && asset->damage == nullptr)
			{
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = damageScale }, offsetof(H1::WeaponDef, maxDamageRange));
			}
			if (damageScaleMin != 0.0f && damageScaleMin != 1.0f && asset->damage == nullptr)
			{
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = damageScaleMin }, offsetof(H1::WeaponDef, minDamageRange));
			}
			if (asset->stateTimersScale != 0.0f && asset->stateTimersScale != 1.0f)
			{
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->stateTimersScale }, offsetof(H1::WeaponDef, stateTimers.meleeTime));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->stateTimersScale }, offsetof(H1::WeaponDef, stateTimers.meleeDelay));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->stateTimersScale }, offsetof(H1::WeaponDef, stateTimers.meleeChargeTime));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->stateTimersScale }, offsetof(H1::WeaponDef, stateTimers.meleeChargeDelay));
			}
			if (asset->fireTimersScale != 0.0f && asset->fireTimersScale != 1.0f)
			{
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = asset->fireTimersScale }, offsetof(H1::WeaponDef, stateTimers.fireTime));
			}
			if (idleSettingsScale != 0.0f && idleSettingsScale != 1.0f)
			{
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = idleSettingsScale }, offsetof(H1::WeaponDef, adsIdleAmount));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = idleSettingsScale }, offsetof(H1::WeaponDef, adsIdleSpeed));
			}
			if (idleSettingsScale != 0.0f && idleSettingsScale != 1.0f && asset->idleSettings == nullptr)
			{
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = idleSettingsScale }, offsetof(H1::WeaponDef, idleCrouchFactor));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = idleSettingsScale }, offsetof(H1::WeaponDef, idleProneFactor));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = idleSettingsScale }, offsetof(H1::WeaponDef, adsIdleLerpStartTime));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = idleSettingsScale }, offsetof(H1::WeaponDef, adsIdleLerpTime));
			}
			if (!asset->shareAmmoWithAlt)
			{
				if (adsSettingsScale != 0.0f && adsSettingsScale != 1.0f && asset->adsSettings == nullptr)
				{
					addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = adsSettingsScale }, offsetof(H1::WeaponDef, adsAimPitch));
					addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = adsSettingsScale }, offsetof(H1::WeaponDef, adsBobFactor));
					addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = adsSettingsScale }, offsetof(H1::WeaponDef, adsFireAnimFrac));
					addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = adsSettingsScale }, offsetof(H1::WeaponDef, positionReloadTransTime));
					addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = adsSettingsScale }, offsetof(H1::WeaponDef, adsViewBobMult));
					addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = adsSettingsScale }, offsetof(H1::WeaponDef, adsZoomFov));
					addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = adsSettingsScale }, offsetof(H1::WeaponDef, adsTransInTime));
					addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = adsSettingsScale }, offsetof(H1::WeaponDef, adsTransOutTime));
					addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = adsSettingsScale }, offsetof(H1::WeaponDef, adsZoomInFrac));
					addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = adsSettingsScale }, offsetof(H1::WeaponDef, adsZoomOutFrac));
					addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = adsSettingsScale }, offsetof(H1::WeaponDef, adsSpread));
				}
			}
			else
			{
				if (adsSettingsScaleMain != 0.0f && adsSettingsScaleMain != 1.0f && asset->adsSettingsMain == nullptr)
				{
					addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = adsSettingsScaleMain }, offsetof(H1::WeaponDef, adsAimPitch));
					addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = adsSettingsScaleMain }, offsetof(H1::WeaponDef, adsBobFactor));
					addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = adsSettingsScaleMain }, offsetof(H1::WeaponDef, adsFireAnimFrac));
					addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = adsSettingsScaleMain }, offsetof(H1::WeaponDef, positionReloadTransTime));
					addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = adsSettingsScaleMain }, offsetof(H1::WeaponDef, adsViewBobMult));
					addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = adsSettingsScaleMain }, offsetof(H1::WeaponDef, adsZoomFov));
					addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = adsSettingsScaleMain }, offsetof(H1::WeaponDef, adsTransInTime));
					addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = adsSettingsScaleMain }, offsetof(H1::WeaponDef, adsTransOutTime));
					addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = adsSettingsScaleMain }, offsetof(H1::WeaponDef, adsZoomInFrac));
					addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = adsSettingsScaleMain }, offsetof(H1::WeaponDef, adsZoomOutFrac));
					addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = adsSettingsScaleMain }, offsetof(H1::WeaponDef, adsSpread));
				}
			}
			
			if (hipSpreadScale != 0.0f && hipSpreadScale != 1.0f && asset->hipSpread == nullptr)
			{
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = hipSpreadScale }, offsetof(H1::WeaponDef, hipSpreadStandMin));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = hipSpreadScale }, offsetof(H1::WeaponDef, hipSpreadStandMax));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = hipSpreadScale }, offsetof(H1::WeaponDef, hipSpreadDuckedMin));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = hipSpreadScale }, offsetof(H1::WeaponDef, hipSpreadDuckedMax));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = hipSpreadScale }, offsetof(H1::WeaponDef, hipSpreadProneMin));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = hipSpreadScale }, offsetof(H1::WeaponDef, hipSpreadProneMax));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = hipSpreadScale }, offsetof(H1::WeaponDef, hipSpreadDecayRate));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = hipSpreadScale }, offsetof(H1::WeaponDef, hipSpreadProneDecay));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = hipSpreadScale }, offsetof(H1::WeaponDef, hipSpreadDuckedDecay));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = hipSpreadScale }, offsetof(H1::WeaponDef, hipSpreadTurnAdd));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = hipSpreadScale }, offsetof(H1::WeaponDef, hipSpreadMoveAdd));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = hipSpreadScale }, offsetof(H1::WeaponDef, hipSpreadFireAdd));
			}
			if (gunKickScale != 0.0f && gunKickScale != 1.0f && asset->gunKick == nullptr)
			{
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = gunKickScale }, offsetof(H1::WeaponDef, adsGunKickReducedKickBullets));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = gunKickScale }, offsetof(H1::WeaponDef, adsGunKickReducedKickPercent));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = gunKickScale }, offsetof(H1::WeaponDef, hipGunKickAccel));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = gunKickScale }, offsetof(H1::WeaponDef, hipGunKickSpeedMax));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = gunKickScale }, offsetof(H1::WeaponDef, hipGunKickSpeedDecay));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = gunKickScale }, offsetof(H1::WeaponDef, hipGunKickStaticDecay));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = gunKickScale }, offsetof(H1::WeaponDef, adsGunKickAccel));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = gunKickScale }, offsetof(H1::WeaponDef, adsGunKickSpeedMax));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = gunKickScale }, offsetof(H1::WeaponDef, adsGunKickSpeedDecay));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = gunKickScale }, offsetof(H1::WeaponDef, adsGunKickStaticDecay));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = gunKickScale }, offsetof(H1::WeaponDef, hipGunKickReducedKickBullets));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = gunKickScale }, offsetof(H1::WeaponDef, hipGunKickReducedKickPercent));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = gunKickScale }, offsetof(H1::WeaponDef, hipGunKickPitchMin));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = gunKickScale }, offsetof(H1::WeaponDef, hipGunKickPitchMax));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = gunKickScale }, offsetof(H1::WeaponDef, hipGunKickYawMin));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = gunKickScale }, offsetof(H1::WeaponDef, hipGunKickYawMax));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = gunKickScale }, offsetof(H1::WeaponDef, hipGunKickMagMin));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = gunKickScale }, offsetof(H1::WeaponDef, adsGunKickPitchMin));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = gunKickScale }, offsetof(H1::WeaponDef, adsGunKickPitchMax));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = gunKickScale }, offsetof(H1::WeaponDef, adsGunKickYawMin));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = gunKickScale }, offsetof(H1::WeaponDef, adsGunKickYawMax));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = gunKickScale }, offsetof(H1::WeaponDef, adsGunKickMagMin));
			}
			if (viewKickScale != 0.0f && viewKickScale != 1.0f && asset->viewKick == nullptr)
			{
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = viewKickScale }, offsetof(H1::WeaponDef, hipViewKickPitchMin));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = viewKickScale }, offsetof(H1::WeaponDef, hipViewKickPitchMax));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = viewKickScale }, offsetof(H1::WeaponDef, hipViewKickYawMin));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = viewKickScale }, offsetof(H1::WeaponDef, hipViewKickYawMax));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = viewKickScale }, offsetof(H1::WeaponDef, adsViewKickPitchMin));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = viewKickScale }, offsetof(H1::WeaponDef, adsViewKickPitchMax));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = viewKickScale }, offsetof(H1::WeaponDef, adsViewKickYawMin));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = viewKickScale }, offsetof(H1::WeaponDef, adsViewKickYawMax));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = viewKickScale }, offsetof(H1::WeaponDef, hipViewKickMagMin));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = viewKickScale }, offsetof(H1::WeaponDef, adsViewKickMagMin));
			}
			if (viewCenterScale != 0.0f && viewCenterScale != 1.0f && asset->viewKick == nullptr)
			{
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = viewCenterScale }, offsetof(H1::WeaponDef, adsViewKickCenterSpeed));
				addField(H1::FIELD_OP_NUMBER_MULTIPLY, H1::WAFIELD_TYPE_FLOAT, { .p_float = viewCenterScale }, offsetof(H1::WeaponDef, hipViewKickCenterSpeed));
			}

			std::sort(fields.begin(), fields.end(), [](const auto a, const auto b)
			{
				if (a.offset == b.offset)
				{
					return a.field.index < b.field.index;
				}

				return a.offset < b.offset;
			});

			h1_asset->numFields = static_cast<unsigned int>(fields.size());
			h1_asset->fields = mem.allocate<H1::WAField>(h1_asset->numFields);
			h1_asset->fieldOffsets = mem.allocate<unsigned short>(h1_asset->numFields);
			for (size_t i = 0; i < fields.size(); i++)
			{
				h1_asset->fields[i] = fields[i].field;
				h1_asset->fieldOffsets[i] = fields[i].offset;
			}

			return h1_asset;
		}

		H1::WeaponAttachment* convert(WeaponAttachment* asset, allocator& allocator)
		{
			// generate h1 attachment
			return GenerateH1Attachment(asset, allocator);
		}
	}
}