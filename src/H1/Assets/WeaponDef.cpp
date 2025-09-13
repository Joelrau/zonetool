#include "stdafx.hpp"

namespace ZoneTool::H1
{
	const char* get_anim_name_from_index(weapAnimFiles_t index)
	{
		return anim_names[index];
	}

	const char* get_anim_name_from_index(int index)
	{
		return anim_names[index];
	}

	weapAnimFiles_t get_anim_index_from_name(const char* name)
	{
		for (unsigned int i = 0; i < NUM_WEAP_ANIMS; i++)
		{
			const char* anim_name = anim_names[i];
			if (!_stricmp(anim_name, name))
			{
				return static_cast<weapAnimFiles_t>(i);
			}
		}

		ZONETOOL_FATAL("Invalid animation index returned for name: \"%s\"", name);
		//return WEAP_ANIM_INVALID;
	}

#define WEAPON_DUMP_FIELD(__field__) \
	data[#__field__] = asset->__field__

#define WEAPON_DUMP_STRING(__field__) \
	static_assert(std::is_same_v<decltype(asset->__field__), const char PTR64>, "Field is not of type const char*"); \
	asset->__field__ ? data[#__field__] = asset->__field__ : data[#__field__] = nullptr;

#define WEAPON_DUMP_FIELD_ARR(__field__, __size__) \
	for (auto idx##__field__ = 0; idx##__field__ < __size__; idx##__field__++) \
	{ \
		data[#__field__][idx##__field__] = asset->__field__[idx##__field__]; \
	}

#define WEAPON_DUMP_FIELD_ARR_STR(__field__, __size__) \
	for (auto idx##__field__ = 0; idx##__field__ < __size__; idx##__field__++) \
	{ \
		if (asset->__field__[idx##__field__] != nullptr) \
			data[#__field__][idx##__field__] = asset->__field__[idx##__field__]; \
		else \
			data[#__field__][idx##__field__] = ""; \
	}

#define WEAPON_DUMP_ASSET(__field__) \
	if (asset->__field__) \
	{ \
		data[#__field__] = asset->__field__->name; \
	} \
	else \
	{ \
		data[#__field__] = ""; \
	}

#define WEAPON_DUMP_ASSET_ARR(__field__, __size__) \
	if (asset->__field__ && __size__) \
	{ \
		for (auto idx##__field__ = 0; idx##__field__ < __size__; idx##__field__++) \
		{ \
			if (asset->__field__[idx##__field__]) \
			{ \
				data[#__field__][idx##__field__] = asset->__field__[idx##__field__]->name; \
			} \
			else \
			{ \
				data[#__field__][idx##__field__] = ""; \
			} \
		} \
	} \
	else \
	{ \
		data[#__field__] = nullptr; \
	}

#define WEAPON_DUMP_ANIM_ARR(__field__, __size__) \
	if (asset->__field__ && __size__) \
	{ \
		for (auto idx##__field__ = 0; idx##__field__ < __size__; idx##__field__++) \
		{ \
			auto name##__field__ = get_anim_name_from_index(idx##__field__); \
			if (asset->__field__[idx##__field__]) \
			{ \
				data[#__field__][name##__field__] = asset->__field__[idx##__field__]->name; \
			} \
			else \
			{ \
				data[#__field__][name##__field__] = ""; \
			} \
		} \
	} \
	else \
	{ \
		data[#__field__] = nullptr; \
	}

#define WEAPON_DUMP_SOUND(__field__) \
	if (asset->__field__) \
	{ \
		data["sounds"][#__field__] = asset->__field__->name; \
	} \
	else \
	{ \
		data["sounds"][#__field__] = ""; \
	}

	ordered_json dump_overlay(ADSOverlay* asset)
	{
		ordered_json data;

		WEAPON_DUMP_ASSET(shader);
		WEAPON_DUMP_ASSET(shaderLowRes);
		WEAPON_DUMP_ASSET(shaderEMP);
		WEAPON_DUMP_ASSET(shaderEMPLowRes);
		WEAPON_DUMP_FIELD(reticle);
		WEAPON_DUMP_FIELD(width);
		WEAPON_DUMP_FIELD(height);
		WEAPON_DUMP_FIELD(widthSplitscreen);
		WEAPON_DUMP_FIELD(heightSplitscreen);

		return data;
	}

	json dump_turret_hydraulic_settings(TurretHydraulicSettings* settings)
	{
		json data;

		data["minVelocity"] = settings->minVelocity;
		data["maxVelocity"] = settings->maxVelocity;
		data["verticalSound"] = settings->verticalSound ? settings->verticalSound->name : "";
		data["verticalStopSound"] = settings->verticalStopSound ? settings->verticalStopSound->name : "";
		data["horizontalSound"] = settings->horizontalSound ? settings->horizontalSound->name : "";
		data["horizontalStopSound"] = settings->horizontalStopSound ? settings->horizontalStopSound->name : "";

		return data;
	}

	json dump_accuracy_graph(WeaponDef* asset)
	{
		json data;

		for (auto i = 0; i < 2; i++)
		{
			if (asset->accuracyGraphName[i])
			{
				data["accuracyGraphName"][i] = asset->accuracyGraphName[i];
			}
			else
			{
				data["accuracyGraphName"][i] = "";
			}

			for (auto o = 0; o < asset->accuracyGraphKnotCount[i]; o++)
			{
				if (asset->accuracyGraphKnots[i])
				{
					data["accuracyGraphKnots"][i][o][0] = asset->accuracyGraphKnots[i][o][0];
					data["accuracyGraphKnots"][i][o][1] = asset->accuracyGraphKnots[i][o][1];
				}
				else
				{
					data["accuracyGraphKnots"] = nullptr;
				}
				if (asset->accuracyGraphKnots[i])
				{
					data["originalAccuracyGraphKnots"][i][o][0] = asset->originalAccuracyGraphKnots[i][o][0];
					data["originalAccuracyGraphKnots"][i][o][1] = asset->originalAccuracyGraphKnots[i][o][1];
				}
				else
				{
					data["originalAccuracyGraphKnots"] = nullptr;
				}
			}
		}

		return data;
	}

	json dump_statetimers(StateTimers* asset)
	{
		json data;

		WEAPON_DUMP_FIELD(fireDelay);
		WEAPON_DUMP_FIELD(meleeDelay);
		WEAPON_DUMP_FIELD(meleeChargeDelay);
		WEAPON_DUMP_FIELD(detonateDelay);
		WEAPON_DUMP_FIELD(fireTime);
		WEAPON_DUMP_FIELD(rechamberTime);
		WEAPON_DUMP_FIELD(rechamberTimeOneHanded);
		WEAPON_DUMP_FIELD(rechamberBoltTime);
		WEAPON_DUMP_FIELD(holdFireTime);
		WEAPON_DUMP_FIELD(grenadePrimeReadyToThrowTime);
		WEAPON_DUMP_FIELD(detonateTime);
		WEAPON_DUMP_FIELD(meleeTime);
		WEAPON_DUMP_FIELD(meleeChargeTime);
		WEAPON_DUMP_FIELD(reloadTime);
		WEAPON_DUMP_FIELD(reloadShowRocketTime);
		WEAPON_DUMP_FIELD(reloadEmptyTime);
		WEAPON_DUMP_FIELD(reloadAddTime);
		WEAPON_DUMP_FIELD(reloadEmptyAddTime);
		WEAPON_DUMP_FIELD(reloadStartTime);
		WEAPON_DUMP_FIELD(reloadStartAddTime);
		WEAPON_DUMP_FIELD(reloadEndTime);
		WEAPON_DUMP_FIELD(reloadTimeDualWield);
		WEAPON_DUMP_FIELD(reloadAddTimeDualWield);
		WEAPON_DUMP_FIELD(reloadEmptyDualMag);
		WEAPON_DUMP_FIELD(reloadEmptyAddTimeDualMag);
		WEAPON_DUMP_FIELD(speedReloadTime);
		WEAPON_DUMP_FIELD(speedReloadAddTime);
		WEAPON_DUMP_FIELD(dropTime);
		WEAPON_DUMP_FIELD(raiseTime);
		WEAPON_DUMP_FIELD(altDropTime);
		WEAPON_DUMP_FIELD(altRaiseTime);
		WEAPON_DUMP_FIELD(quickDropTime);
		WEAPON_DUMP_FIELD(quickRaiseTime);
		WEAPON_DUMP_FIELD(firstRaiseTime);
		WEAPON_DUMP_FIELD(breachRaiseTime);
		WEAPON_DUMP_FIELD(emptyRaiseTime);
		WEAPON_DUMP_FIELD(emptyDropTime);
		WEAPON_DUMP_FIELD(sprintInTime);
		WEAPON_DUMP_FIELD(sprintLoopTime);
		WEAPON_DUMP_FIELD(sprintOutTime);
		WEAPON_DUMP_FIELD(stunnedTimeBegin);
		WEAPON_DUMP_FIELD(stunnedTimeLoop);
		WEAPON_DUMP_FIELD(stunnedTimeEnd);
		WEAPON_DUMP_FIELD(nightVisionWearTime);
		WEAPON_DUMP_FIELD(nightVisionWearTimeFadeOutEnd);
		WEAPON_DUMP_FIELD(nightVisionWearTimePowerUp);
		WEAPON_DUMP_FIELD(nightVisionRemoveTime);
		WEAPON_DUMP_FIELD(nightVisionRemoveTimePowerDown);
		WEAPON_DUMP_FIELD(nightVisionRemoveTimeFadeInStart);
		WEAPON_DUMP_FIELD(aiFuseTime);
		WEAPON_DUMP_FIELD(fuseTime);
		WEAPON_DUMP_FIELD(missileTime);
		WEAPON_DUMP_FIELD(primeTime);
		WEAPON_DUMP_FIELD(bHoldFullPrime);
		WEAPON_DUMP_FIELD(bHoldFullPrime);
		WEAPON_DUMP_FIELD(blastRightTime);
		WEAPON_DUMP_FIELD(blastBackTime);
		WEAPON_DUMP_FIELD(blastLeftTime);
		WEAPON_DUMP_FIELD(slideInTime);
		WEAPON_DUMP_FIELD(slideLoopTime);
		WEAPON_DUMP_FIELD(slideOutTime);
		WEAPON_DUMP_FIELD(highJumpInTime);
		WEAPON_DUMP_FIELD(highJumpDropInTime);
		WEAPON_DUMP_FIELD(highJumpDropLoopTime);
		WEAPON_DUMP_FIELD(highJumpDropLandTime);
		WEAPON_DUMP_FIELD(dodgeTime);
		WEAPON_DUMP_FIELD(landDipTime);
		WEAPON_DUMP_FIELD(hybridSightInTime);
		WEAPON_DUMP_FIELD(hybridSightOutTime);
		WEAPON_DUMP_FIELD(offhandSwitchTime);
		WEAPON_DUMP_FIELD(heatCooldownInTime);
		WEAPON_DUMP_FIELD(heatCooldownOutTime);
		WEAPON_DUMP_FIELD(heatCooldownOutReadyTime);
		WEAPON_DUMP_FIELD(overheatOutTime);
		WEAPON_DUMP_FIELD(overheatOutReadyTime);

		return data;
	}

	void IWeaponDef::dump(WeaponDef* asset, const std::function<const char* (std::uint16_t)>& SL_ConvertToString)
	{
		const auto path = "weapons\\"s + asset->name + ".json"s;

		ordered_json data;

		WEAPON_DUMP_STRING(szInternalName);
		WEAPON_DUMP_STRING(szDisplayName);
		WEAPON_DUMP_STRING(szAltWeaponName);

		WEAPON_DUMP_ASSET_ARR(gunModel, 2);
		WEAPON_DUMP_ASSET_ARR(worldModel, 2);

		WEAPON_DUMP_ASSET_ARR(reticleViewModels, 64);

		WEAPON_DUMP_ASSET(handModel);
		WEAPON_DUMP_ASSET(persistentArmXModel);
		WEAPON_DUMP_ASSET(worldClipModel);
		WEAPON_DUMP_ASSET(rocketModel);
		WEAPON_DUMP_ASSET(knifeModel);
		WEAPON_DUMP_ASSET(worldKnifeModel);

		WEAPON_DUMP_ANIM_ARR(szXAnimsRightHanded, 190);
		WEAPON_DUMP_ANIM_ARR(szXAnimsLeftHanded, 190);
		WEAPON_DUMP_ANIM_ARR(szXAnims, 190);

		for (auto i = 0; i < 32; i++)
		{
			if (asset->hideTags && asset->hideTags[i])
			{
				data["hideTags"][i] = SL_ConvertToString(asset->hideTags[i]);
			}
			else
			{
				data["hideTags"][i] = "";
			}
		}

		WEAPON_DUMP_ASSET_ARR(attachments, asset->numAttachments);

		for (auto i = 0u; i < asset->numAnimOverrides; i++)
		{
			data["animOverrides"][i]["altmodeAnim"] = (asset->animOverrides[i].altmodeAnim)
				? asset->animOverrides[i].altmodeAnim->name
				: "";
			data["animOverrides"][i]["overrideAnim"] = (asset->animOverrides[i].overrideAnim)
				? asset->animOverrides[i].overrideAnim->name
				: "";
			data["animOverrides"][i]["attachment1"] = asset->animOverrides[i].attachment1;
			data["animOverrides"][i]["attachment2"] = asset->animOverrides[i].attachment2;
			data["animOverrides"][i]["altTime"] = asset->animOverrides[i].altTime;
			data["animOverrides"][i]["animTime"] = asset->animOverrides[i].animTime;
			data["animOverrides"][i]["animTreeType"] = asset->animOverrides[i].animTreeType;
			data["animOverrides"][i]["animHand"] = asset->animOverrides[i].animHand;
		}

		for (auto i = 0u; i < asset->numSoundOverrides; i++)
		{
			data["soundOverrides"][i]["altmodeSound"] = (asset->soundOverrides[i].altmodeSound)
				? asset->soundOverrides[i].altmodeSound->name
				: "";
			data["soundOverrides"][i]["attachment1"] = asset->soundOverrides[i].attachment1;
			data["soundOverrides"][i]["attachment2"] = asset->soundOverrides[i].attachment2;
			data["soundOverrides"][i]["overrideSound"] = (asset->soundOverrides[i].overrideSound)
				? asset->soundOverrides[i].overrideSound->name
				: "";
			data["soundOverrides"][i]["soundType"] = asset->soundOverrides[i].soundType;
		}

		for (auto i = 0u; i < asset->numFXOverrides; i++)
		{
			data["fxOverrides"][i]["altmodeFX"] = (asset->fxOverrides[i].altmodeFX)
				? asset->fxOverrides[i].altmodeFX->name
				: "";
			data["fxOverrides"][i]["attachment1"] = asset->fxOverrides[i].attachment1;
			data["fxOverrides"][i]["attachment2"] = asset->fxOverrides[i].attachment2;
			data["fxOverrides"][i]["fxType"] = asset->fxOverrides[i].fxType;
			data["fxOverrides"][i]["overrideFX"] = (asset->fxOverrides[i].overrideFX)
				? asset->fxOverrides[i].overrideFX->name
				: "";
		}

		for (auto i = 0u; i < asset->numReloadStateTimerOverrides; i++)
		{
			data["reloadOverrides"][i]["attachment"] = asset->reloadOverrides[i].attachment;
			data["reloadOverrides"][i]["reloadAddTime"] = asset->reloadOverrides[i].reloadAddTime;
			data["reloadOverrides"][i]["reloadEmptyAddTime"] = asset->reloadOverrides[i].reloadEmptyAddTime;
			data["reloadOverrides"][i]["reloadStartAddTime"] = asset->reloadOverrides[i].reloadStartAddTime;
		}

		for (auto i = 0u; i < asset->numNotetrackOverrides; i++)
		{
			data["notetrackOverrides"][i]["attachment"] = asset->notetrackOverrides[i].attachment;

			for (auto j = 0u; j < 36; j++)
			{
				data["notetrackOverrides"][i]["notetrackSoundMap"][j]["Key"] = (asset->notetrackOverrides[i].notetrackSoundMapKeys[j])
					? SL_ConvertToString(asset->notetrackOverrides[i].notetrackSoundMapKeys[j])
					: "";

				data["notetrackOverrides"][i]["notetrackSoundMap"][j]["Value"] = (asset->notetrackOverrides[i].notetrackSoundMapValues[j])
					? SL_ConvertToString(asset->notetrackOverrides[i].notetrackSoundMapValues[j])
					: "";
			}
		}

		for (auto i = 0; i < 36; i++)
		{
			data["notetrackSoundMap"][i]["Key"] = asset->notetrackSoundMapKeys && asset->notetrackSoundMapKeys[i]
				? SL_ConvertToString(asset->notetrackSoundMapKeys[i])
				: "";

			data["notetrackSoundMap"][i]["Value"] = asset->notetrackSoundMapValues && asset->notetrackSoundMapValues[i]
				? SL_ConvertToString(asset->notetrackSoundMapValues[i])
				: "";
		}

		for (auto i = 0; i < 16; i++)
		{
			data["notetrackRumbleMap"][i]["Key"] = asset->notetrackRumbleMapKeys && asset->notetrackRumbleMapKeys[i]
				? SL_ConvertToString(asset->notetrackRumbleMapKeys[i])
				: "";

			data["notetrackRumbleMap"][i]["Value"] = asset->notetrackRumbleMapValues && asset->notetrackRumbleMapValues[i]
				? SL_ConvertToString(asset->notetrackRumbleMapValues[i])
				: "";
		}

		for (auto i = 0; i < 16; i++)
		{
			data["notetrackFXMap"][i]["Key"] = asset->notetrackFXMapKeys && asset->notetrackFXMapKeys[i]
				? SL_ConvertToString(asset->notetrackFXMapKeys[i])
				: "";

			data["notetrackFXMap"][i]["Value"] = asset->notetrackFXMapValues && asset->notetrackFXMapValues[i]
				? asset->notetrackFXMapValues[i]->name
				: "";

			data["notetrackFXMap"][i]["Tag"] = asset->notetrackFXMapTagValues && asset->notetrackFXMapTagValues[i]
				? SL_ConvertToString(asset->notetrackFXMapTagValues[i])
				: "";
		}

		for (auto i = 0; i < 16; i++)
		{
			data["notetrackHideTag"][i]["Key"] = asset->notetrackHideTagKeys && asset->notetrackHideTagKeys[i]
				? SL_ConvertToString(asset->notetrackHideTagKeys[i])
				: "";

			data["notetrackHideTag"][i]["Value"] = asset->notetrackHideTagValues && asset->notetrackHideTagValues[i]
				? asset->notetrackHideTagValues[i]
				: false;

			data["notetrackHideTag"][i]["Tag"] = asset->notetrackHideTagTagValues && asset->notetrackHideTagTagValues[i]
				? SL_ConvertToString(asset->notetrackHideTagTagValues[i])
				: "";
		}

		WEAPON_DUMP_STRING(lobWorldModelName);
		WEAPON_DUMP_STRING(szAdsrBaseSetting);
		WEAPON_DUMP_STRING(szAmmoName);
		WEAPON_DUMP_STRING(szClipName);
		WEAPON_DUMP_STRING(szSharedAmmoCapName);

		WEAPON_DUMP_ASSET(viewFlashEffect);
		WEAPON_DUMP_ASSET(viewBodyFlashEffect);
		WEAPON_DUMP_ASSET(worldFlashEffect);
		WEAPON_DUMP_ASSET(viewFlashADSEffect);
		WEAPON_DUMP_ASSET(viewBodyFlashADSEffect);
		WEAPON_DUMP_ASSET(signatureViewFlashEffect);
		WEAPON_DUMP_ASSET(signatureViewBodyFlashEffect);
		WEAPON_DUMP_ASSET(signatureWorldFlashEffect);
		WEAPON_DUMP_ASSET(signatureViewFlashADSEffect);
		WEAPON_DUMP_ASSET(signatureViewBodyFlashADSEffect);
		WEAPON_DUMP_ASSET(meleeHitEffect);
		WEAPON_DUMP_ASSET(meleeMissEffect);

		WEAPON_DUMP_SOUND(pickupSound);
		WEAPON_DUMP_SOUND(pickupSoundPlayer);
		WEAPON_DUMP_SOUND(ammoPickupSound);
		WEAPON_DUMP_SOUND(ammoPickupSoundPlayer);
		WEAPON_DUMP_SOUND(projectileSound);
		WEAPON_DUMP_SOUND(pullbackSound);
		WEAPON_DUMP_SOUND(pullbackSoundPlayer);
		WEAPON_DUMP_SOUND(pullbackSoundQuick);
		WEAPON_DUMP_SOUND(pullbackSoundQuickPlayer);
		WEAPON_DUMP_SOUND(fireSound);
		WEAPON_DUMP_SOUND(fireSoundPlayer);
		WEAPON_DUMP_SOUND(fireSoundPlayerAkimbo);
		WEAPON_DUMP_SOUND(fireMedSound);
		WEAPON_DUMP_SOUND(fireMedSoundPlayer);
		WEAPON_DUMP_SOUND(fireHighSound);
		WEAPON_DUMP_SOUND(fireHighSoundPlayer);
		WEAPON_DUMP_SOUND(fireLoopSound);
		WEAPON_DUMP_SOUND(fireLoopSoundPlayer);
		WEAPON_DUMP_SOUND(fireMedLoopSound);
		WEAPON_DUMP_SOUND(fireMedLoopSoundPlayer);
		WEAPON_DUMP_SOUND(fireHighLoopSound);
		WEAPON_DUMP_SOUND(fireHighLoopSoundPlayer);
		WEAPON_DUMP_SOUND(fireLoopEndPointSound);
		WEAPON_DUMP_SOUND(fireLoopEndPointSoundPlayer);
		WEAPON_DUMP_SOUND(fireStopSound);
		WEAPON_DUMP_SOUND(fireStopSoundPlayer);
		WEAPON_DUMP_SOUND(fireMedStopSound);
		WEAPON_DUMP_SOUND(fireMedStopSoundPlayer);
		WEAPON_DUMP_SOUND(fireHighStopSound);
		WEAPON_DUMP_SOUND(fireHighStopSoundPlayer);
		WEAPON_DUMP_SOUND(fireLastSound);
		WEAPON_DUMP_SOUND(fireLastSoundPlayer);
		WEAPON_DUMP_SOUND(fireFirstSound);
		WEAPON_DUMP_SOUND(fireFirstSoundPlayer);
		WEAPON_DUMP_SOUND(fireCustomSound);
		WEAPON_DUMP_SOUND(fireCustomSoundPlayer);
		WEAPON_DUMP_SOUND(emptyFireSound);
		WEAPON_DUMP_SOUND(emptyFireSoundPlayer);
		WEAPON_DUMP_SOUND(adsRequiredFireSoundPlayer);
		WEAPON_DUMP_SOUND(meleeSwipeSound);
		WEAPON_DUMP_SOUND(meleeSwipeSoundPlayer);
		WEAPON_DUMP_SOUND(meleeHitSound);
		WEAPON_DUMP_SOUND(meleeHitSoundPlayer);
		WEAPON_DUMP_SOUND(meleeMissSound);
		WEAPON_DUMP_SOUND(meleeMissSoundPlayer);
		WEAPON_DUMP_SOUND(rechamberSound);
		WEAPON_DUMP_SOUND(rechamberSoundPlayer);
		WEAPON_DUMP_SOUND(reloadSound);
		WEAPON_DUMP_SOUND(reloadSoundPlayer);
		WEAPON_DUMP_SOUND(reloadEmptySound);
		WEAPON_DUMP_SOUND(reloadEmptySoundPlayer);
		WEAPON_DUMP_SOUND(reloadStartSound);
		WEAPON_DUMP_SOUND(reloadStartSoundPlayer);
		WEAPON_DUMP_SOUND(reloadEndSound);
		WEAPON_DUMP_SOUND(reloadEndSoundPlayer);
		WEAPON_DUMP_SOUND(detonateSound);
		WEAPON_DUMP_SOUND(detonateSoundPlayer);
		WEAPON_DUMP_SOUND(nightVisionWearSound);
		WEAPON_DUMP_SOUND(nightVisionWearSoundPlayer);
		WEAPON_DUMP_SOUND(nightVisionRemoveSound);
		WEAPON_DUMP_SOUND(nightVisionRemoveSoundPlayer);
		WEAPON_DUMP_SOUND(raiseSound);
		WEAPON_DUMP_SOUND(raiseSoundPlayer);
		WEAPON_DUMP_SOUND(firstRaiseSound);
		WEAPON_DUMP_SOUND(firstRaiseSoundPlayer);
		WEAPON_DUMP_SOUND(altSwitchSound);
		WEAPON_DUMP_SOUND(altSwitchSoundPlayer);
		WEAPON_DUMP_SOUND(putawaySound);
		WEAPON_DUMP_SOUND(putawaySoundPlayer);
		WEAPON_DUMP_SOUND(scanSound);
		WEAPON_DUMP_SOUND(changeVariableZoomSound);
		WEAPON_DUMP_SOUND(adsUpSound);
		WEAPON_DUMP_SOUND(adsDownSound);
		WEAPON_DUMP_SOUND(adsCrosshairEnemySound);

		WEAPON_DUMP_ASSET_ARR(bounceSound, 53);
		WEAPON_DUMP_ASSET_ARR(rollingSound, 53);

		WEAPON_DUMP_ASSET(viewShellEjectEffect);
		WEAPON_DUMP_ASSET(worldShellEjectEffect);
		WEAPON_DUMP_ASSET(viewLastShotEjectEffect);
		WEAPON_DUMP_ASSET(worldLastShotEjectEffect);
		WEAPON_DUMP_ASSET(viewMagEjectEffect);

		WEAPON_DUMP_ASSET(reticleCenter);
		WEAPON_DUMP_ASSET(reticleSide);

		WEAPON_DUMP_ASSET(hudIcon);
		WEAPON_DUMP_ASSET(pickupIcon);
		WEAPON_DUMP_ASSET(minimapIconFriendly);
		WEAPON_DUMP_ASSET(minimapIconEnemy);
		WEAPON_DUMP_ASSET(minimapIconNeutral);
		WEAPON_DUMP_ASSET(ammoCounterIcon);

		WEAPON_DUMP_ASSET(physCollmap);
		WEAPON_DUMP_ASSET(physPreset);

		WEAPON_DUMP_STRING(szUseHintString);
		WEAPON_DUMP_STRING(dropHintString);

		WEAPON_DUMP_FIELD_ARR(locationDamageMultipliers, 22);

		WEAPON_DUMP_STRING(fireRumble);
		WEAPON_DUMP_STRING(fireMedRumble);
		WEAPON_DUMP_STRING(fireHighRumble);
		WEAPON_DUMP_STRING(meleeImpactRumble);

		WEAPON_DUMP_ASSET(tracerType);
		WEAPON_DUMP_ASSET(signatureTracerType);

		WEAPON_DUMP_ASSET(laserType);

		WEAPON_DUMP_ASSET(turretOverheatSound);
		WEAPON_DUMP_ASSET(turretOverheatEffect);
		WEAPON_DUMP_STRING(turretBarrelSpinRumble);
		WEAPON_DUMP_ASSET(turretBarrelSpinMaxSnd);
		if (asset->turretBarrelSpinUpSnd && 4) {
			for (auto idxturretBarrelSpinUpSnd = 0; idxturretBarrelSpinUpSnd < 4; idxturretBarrelSpinUpSnd++) {
				if (asset->turretBarrelSpinUpSnd[idxturretBarrelSpinUpSnd]) {
					data["turretBarrelSpinUpSnd"][idxturretBarrelSpinUpSnd] = asset->turretBarrelSpinUpSnd[idxturretBarrelSpinUpSnd]->name;
				}
				else {
					data["turretBarrelSpinUpSnd"][idxturretBarrelSpinUpSnd] = "";
				}
			}
		}
		else {
			data["turretBarrelSpinUpSnd"] = nullptr;
		};
		if (asset->turretBarrelSpinDownSnd && 4) {
			for (auto idxturretBarrelSpinDownSnd = 0; idxturretBarrelSpinDownSnd < 4; idxturretBarrelSpinDownSnd++) {
				if (asset->turretBarrelSpinDownSnd[idxturretBarrelSpinDownSnd]) {
					data["turretBarrelSpinDownSnd"][idxturretBarrelSpinDownSnd] = asset->turretBarrelSpinDownSnd[idxturretBarrelSpinDownSnd]->name;
				}
				else {
					data["turretBarrelSpinDownSnd"][idxturretBarrelSpinDownSnd] = "";
				}
			}
		}
		else {
			data["turretBarrelSpinDownSnd"] = nullptr;
		};
		if (asset->missileConeSoundAlias) {
			data["missileConeSoundAlias"] = asset->missileConeSoundAlias->name;
		}
		else {
			data["missileConeSoundAlias"] = "";
		};
		WEAPON_DUMP_ASSET(missileConeSoundAliasAtBase);

		WEAPON_DUMP_ASSET(stowOffsetModel);

		WEAPON_DUMP_ASSET(killIcon);
		WEAPON_DUMP_ASSET(dpadIcon);
		WEAPON_DUMP_ASSET(hudProximityWarningIcon);

		WEAPON_DUMP_STRING(projectileName);
		WEAPON_DUMP_ASSET(projectileModel);
		WEAPON_DUMP_ASSET(projExplosionEffect);
		WEAPON_DUMP_ASSET(projDudEffect);
		WEAPON_DUMP_ASSET(projExplosionSound);
		WEAPON_DUMP_ASSET(projDudSound);
		WEAPON_DUMP_ASSET(projTrailEffect);
		WEAPON_DUMP_ASSET(projBeaconEffect);
		WEAPON_DUMP_ASSET(projIgnitionEffect);
		WEAPON_DUMP_ASSET(projIgnitionSound);

		WEAPON_DUMP_STRING(szScript);

		if (asset->turretHydraulicSettings)
		{
			data["turretHydraulicSettings"] = dump_turret_hydraulic_settings(asset->turretHydraulicSettings);
		}

		data["stateTimers"] = dump_statetimers(&asset->stateTimers);
		data["stateTimersAkimbo"] = dump_statetimers(&asset->akimboStateTimers);

		data["overlay"] = dump_overlay(&asset->overlay);

		data["accuracy_graph"] = dump_accuracy_graph(asset);

		data["stowTag"] = SL_ConvertToString(asset->stowTag) ? SL_ConvertToString(asset->stowTag) : "";

		WEAPON_DUMP_FIELD(altWeapon);
		WEAPON_DUMP_FIELD(playerAnimType);
		WEAPON_DUMP_FIELD(weapType);
		WEAPON_DUMP_FIELD(weapClass);
		WEAPON_DUMP_FIELD(penetrateType);
		WEAPON_DUMP_FIELD(penetrateDepth);
		WEAPON_DUMP_FIELD(impactType);
		WEAPON_DUMP_FIELD(inventoryType);
		WEAPON_DUMP_FIELD(fireType);
		WEAPON_DUMP_FIELD(fireBarrels);
		WEAPON_DUMP_FIELD(adsFireMode);
		WEAPON_DUMP_FIELD(burstFireCooldown);
		WEAPON_DUMP_FIELD(greebleType);
		WEAPON_DUMP_FIELD(autoReloadType);
		WEAPON_DUMP_FIELD(autoHolsterType);
		WEAPON_DUMP_FIELD(offhandClass);
		WEAPON_DUMP_FIELD(stance);
		WEAPON_DUMP_FIELD(reticleCenterSize);
		WEAPON_DUMP_FIELD(reticleSideSize);
		WEAPON_DUMP_FIELD(reticleMinOfs);
		WEAPON_DUMP_FIELD(activeReticleType);
		WEAPON_DUMP_FIELD_ARR(standMove, 3);
		WEAPON_DUMP_FIELD_ARR(standRot, 3);
		WEAPON_DUMP_FIELD_ARR(strafeMove, 3);
		WEAPON_DUMP_FIELD_ARR(strafeRot, 3);
		WEAPON_DUMP_FIELD_ARR(duckedOfs, 3);
		WEAPON_DUMP_FIELD_ARR(duckedMove, 3);
		WEAPON_DUMP_FIELD_ARR(duckedRot, 3);
		WEAPON_DUMP_FIELD_ARR(proneOfs, 3);
		WEAPON_DUMP_FIELD_ARR(proneMove, 3);
		WEAPON_DUMP_FIELD_ARR(proneRot, 3);
		WEAPON_DUMP_FIELD(posMoveRate);
		WEAPON_DUMP_FIELD(posProneMoveRate);
		WEAPON_DUMP_FIELD(standMoveMinSpeed);
		WEAPON_DUMP_FIELD(duckedMoveMinSpeed);
		WEAPON_DUMP_FIELD(proneMoveMinSpeed);
		WEAPON_DUMP_FIELD(posRotRate);
		WEAPON_DUMP_FIELD(posProneRotRate);
		WEAPON_DUMP_FIELD(hudIconRatio);
		WEAPON_DUMP_FIELD(pickupIconRatio);
		WEAPON_DUMP_FIELD(ammoCounterIconRatio);
		WEAPON_DUMP_FIELD(ammoCounterClip);
		WEAPON_DUMP_FIELD(startAmmo);
		//WEAPON_DUMP_FIELD(ammoIndex); // runtime
		//WEAPON_DUMP_FIELD(clipIndex); // runtime
		WEAPON_DUMP_FIELD(maxAmmo);
		WEAPON_DUMP_FIELD(minAmmoReq);
		WEAPON_DUMP_FIELD(clipSize);
		WEAPON_DUMP_FIELD(shotCount);
		//WEAPON_DUMP_FIELD(sharedAmmoCapIndex); // runtime
		WEAPON_DUMP_FIELD(sharedAmmoCap);
		WEAPON_DUMP_FIELD(damage);
		WEAPON_DUMP_FIELD(playerDamage);
		WEAPON_DUMP_FIELD(meleeDamage);
		WEAPON_DUMP_FIELD(damageType);
		WEAPON_DUMP_FIELD(autoAimRange);
		WEAPON_DUMP_FIELD(aimAssistRange);
		WEAPON_DUMP_FIELD(aimAssistRangeAds);
		WEAPON_DUMP_FIELD(aimPadding);
		WEAPON_DUMP_FIELD(enemyCrosshairRange);
		WEAPON_DUMP_FIELD(moveSpeedScale);
		WEAPON_DUMP_FIELD(adsMoveSpeedScale);
		WEAPON_DUMP_FIELD(sprintDurationScale);
		WEAPON_DUMP_FIELD(adsZoomFov);
		WEAPON_DUMP_FIELD(adsZoomInFrac);
		WEAPON_DUMP_FIELD(adsZoomOutFrac);
		WEAPON_DUMP_FIELD(adsSceneBlurStrength);
		WEAPON_DUMP_FIELD(adsSceneBlurPhysicalScale);
		//WEAPON_DUMP_FIELD(pad3);
		WEAPON_DUMP_FIELD(adsBobFactor);
		WEAPON_DUMP_FIELD(adsViewBobMult);
		WEAPON_DUMP_FIELD(hipSpreadStandMin);
		WEAPON_DUMP_FIELD(hipSpreadDuckedMin);
		WEAPON_DUMP_FIELD(hipSpreadProneMin);
		WEAPON_DUMP_FIELD(hipSpreadStandMax);
		WEAPON_DUMP_FIELD(hipSpreadSprintMax);
		WEAPON_DUMP_FIELD(hipSpreadSlideMax);
		WEAPON_DUMP_FIELD(hipSpreadDuckedMax);
		WEAPON_DUMP_FIELD(hipSpreadProneMax);
		WEAPON_DUMP_FIELD(hipSpreadDecayRate);
		WEAPON_DUMP_FIELD(hipSpreadFireAdd);
		WEAPON_DUMP_FIELD(hipSpreadTurnAdd);
		WEAPON_DUMP_FIELD(hipSpreadMoveAdd);
		WEAPON_DUMP_FIELD(hipSpreadDuckedDecay);
		WEAPON_DUMP_FIELD(hipSpreadProneDecay);
		WEAPON_DUMP_FIELD(hipReticleSidePos);
		WEAPON_DUMP_FIELD(adsIdleAmount);
		WEAPON_DUMP_FIELD(hipIdleAmount);
		WEAPON_DUMP_FIELD(adsIdleSpeed);
		WEAPON_DUMP_FIELD(hipIdleSpeed);
		WEAPON_DUMP_FIELD(idleCrouchFactor);
		WEAPON_DUMP_FIELD(idleProneFactor);
		WEAPON_DUMP_FIELD(gunMaxPitch);
		WEAPON_DUMP_FIELD(gunMaxYaw);
		WEAPON_DUMP_FIELD(adsIdleLerpStartTime);
		WEAPON_DUMP_FIELD(adsIdleLerpTime);
		WEAPON_DUMP_FIELD(adsTransInTime);
		WEAPON_DUMP_FIELD(adsTransInFromSprintTime);
		WEAPON_DUMP_FIELD(adsTransOutTime);
		WEAPON_DUMP_FIELD(swayMaxAngleSteadyAim);
		WEAPON_DUMP_FIELD(swayMaxAngle);
		WEAPON_DUMP_FIELD(swayLerpSpeed);
		WEAPON_DUMP_FIELD(swayPitchScale);
		WEAPON_DUMP_FIELD(swayYawScale);
		WEAPON_DUMP_FIELD(swayVertScale);
		WEAPON_DUMP_FIELD(swayHorizScale);
		WEAPON_DUMP_FIELD(swayShellShockScale);
		WEAPON_DUMP_FIELD(adsSwayMaxAngle);
		WEAPON_DUMP_FIELD(adsSwayLerpSpeed);
		WEAPON_DUMP_FIELD(adsSwayPitchScale);
		WEAPON_DUMP_FIELD(adsSwayYawScale);
		WEAPON_DUMP_FIELD(adsSwayHorizScale);
		WEAPON_DUMP_FIELD(adsSwayVertScale);
		WEAPON_DUMP_FIELD(adsViewErrorMin);
		WEAPON_DUMP_FIELD(adsViewErrorMax);
		WEAPON_DUMP_FIELD(adsFireAnimFrac);
		WEAPON_DUMP_FIELD(dualWieldViewModelOffset);
		WEAPON_DUMP_FIELD(scopeDriftDelay);
		WEAPON_DUMP_FIELD(scopeDriftLerpInTime);
		WEAPON_DUMP_FIELD(scopeDriftSteadyTime);
		WEAPON_DUMP_FIELD(scopeDriftLerpOutTime);
		WEAPON_DUMP_FIELD(scopeDriftSteadyFactor);
		WEAPON_DUMP_FIELD(scopeDriftUnsteadyFactor);
		WEAPON_DUMP_FIELD(bobVerticalFactor);
		WEAPON_DUMP_FIELD(bobHorizontalFactor);
		WEAPON_DUMP_FIELD(bobViewVerticalFactor);
		WEAPON_DUMP_FIELD(bobViewHorizontalFactor);
		WEAPON_DUMP_FIELD(stationaryZoomFov);
		WEAPON_DUMP_FIELD(stationaryZoomDelay);
		WEAPON_DUMP_FIELD(stationaryZoomLerpInTime);
		WEAPON_DUMP_FIELD(stationaryZoomLerpOutTime);
		WEAPON_DUMP_FIELD(adsDofStart);
		WEAPON_DUMP_FIELD(adsDofEnd);
		//WEAPON_DUMP_FIELD(pad1);
		WEAPON_DUMP_FIELD(killIconRatio);
		WEAPON_DUMP_FIELD(dpadIconRatio);
		WEAPON_DUMP_FIELD(fireAnimLength);
		WEAPON_DUMP_FIELD(fireAnimLengthAkimbo);
		WEAPON_DUMP_FIELD(inspectAnimTime);
		WEAPON_DUMP_FIELD(reloadAmmoAdd);
		WEAPON_DUMP_FIELD(reloadStartAdd);
		WEAPON_DUMP_FIELD(ammoDropStockMin);
		WEAPON_DUMP_FIELD(ammoDropStockMax);
		WEAPON_DUMP_FIELD(ammoDropClipPercentMin);
		WEAPON_DUMP_FIELD(ammoDropClipPercentMax);
		WEAPON_DUMP_FIELD(explosionRadius);
		WEAPON_DUMP_FIELD(explosionRadiusMin);
		WEAPON_DUMP_FIELD(explosionInnerDamage);
		WEAPON_DUMP_FIELD(explosionOuterDamage);
		WEAPON_DUMP_FIELD(damageConeAngle);
		WEAPON_DUMP_FIELD(bulletExplDmgMult);
		WEAPON_DUMP_FIELD(bulletExplRadiusMult);
		WEAPON_DUMP_FIELD(projectileSpeed);
		WEAPON_DUMP_FIELD(projectileSpeedUp);
		WEAPON_DUMP_FIELD(projectileSpeedForward);
		WEAPON_DUMP_FIELD(projectileActivateDist);
		WEAPON_DUMP_FIELD(projLifetime);
		WEAPON_DUMP_FIELD(timeToAccelerate);
		WEAPON_DUMP_FIELD(projectileCurvature);
		//WEAPON_DUMP_FIELD(pad2);
		WEAPON_DUMP_FIELD(projExplosion);
		WEAPON_DUMP_FIELD(stickiness);
		WEAPON_DUMP_FIELD(lowAmmoWarningThreshold);
		WEAPON_DUMP_FIELD(ricochetChance);
		WEAPON_DUMP_FIELD(riotShieldHealth);
		WEAPON_DUMP_FIELD(riotShieldDamageMult);
		WEAPON_DUMP_FIELD_ARR(parallelBounce, 53);
		WEAPON_DUMP_FIELD_ARR(perpendicularBounce, 53);
		WEAPON_DUMP_FIELD_ARR(projectileColor, 3);
		WEAPON_DUMP_FIELD(guidedMissileType);
		WEAPON_DUMP_FIELD(maxSteeringAccel);
		WEAPON_DUMP_FIELD(projIgnitionDelay);
		WEAPON_DUMP_FIELD(adsAimPitch);
		WEAPON_DUMP_FIELD(adsCrosshairInFrac);
		WEAPON_DUMP_FIELD(adsCrosshairOutFrac);
		WEAPON_DUMP_FIELD(adsGunKickReducedKickBullets);
		WEAPON_DUMP_FIELD(adsGunKickReducedKickPercent);
		WEAPON_DUMP_FIELD(adsGunKickPitchMin);
		WEAPON_DUMP_FIELD(adsGunKickPitchMax);
		WEAPON_DUMP_FIELD(adsGunKickYawMin);
		WEAPON_DUMP_FIELD(adsGunKickYawMax);
		WEAPON_DUMP_FIELD(adsGunKickMagMin);
		WEAPON_DUMP_FIELD(adsGunKickAccel);
		WEAPON_DUMP_FIELD(adsGunKickSpeedMax);
		WEAPON_DUMP_FIELD(adsGunKickSpeedDecay);
		WEAPON_DUMP_FIELD(adsGunKickStaticDecay);
		WEAPON_DUMP_FIELD(adsViewKickPitchMin);
		WEAPON_DUMP_FIELD(adsViewKickPitchMax);
		WEAPON_DUMP_FIELD(adsViewKickYawMin);
		WEAPON_DUMP_FIELD(adsViewKickYawMax);
		WEAPON_DUMP_FIELD(adsViewKickMagMin);
		WEAPON_DUMP_FIELD(adsViewKickCenterSpeed);
		WEAPON_DUMP_FIELD(adsViewScatterMin);
		WEAPON_DUMP_FIELD(adsViewScatterMax);
		WEAPON_DUMP_FIELD(adsSpread);
		WEAPON_DUMP_FIELD(hipGunKickReducedKickBullets);
		WEAPON_DUMP_FIELD(hipGunKickReducedKickPercent);
		WEAPON_DUMP_FIELD(hipGunKickPitchMin);
		WEAPON_DUMP_FIELD(hipGunKickPitchMax);
		WEAPON_DUMP_FIELD(hipGunKickYawMin);
		WEAPON_DUMP_FIELD(hipGunKickYawMax);
		WEAPON_DUMP_FIELD(hipGunKickMagMin);
		WEAPON_DUMP_FIELD(hipGunKickAccel);
		WEAPON_DUMP_FIELD(hipGunKickSpeedMax);
		WEAPON_DUMP_FIELD(hipGunKickSpeedDecay);
		WEAPON_DUMP_FIELD(hipGunKickStaticDecay);
		WEAPON_DUMP_FIELD(hipViewKickPitchMin);
		WEAPON_DUMP_FIELD(hipViewKickPitchMax);
		WEAPON_DUMP_FIELD(hipViewKickYawMin);
		WEAPON_DUMP_FIELD(hipViewKickYawMax);
		WEAPON_DUMP_FIELD(hipViewKickMagMin);
		WEAPON_DUMP_FIELD(hipViewKickCenterSpeed);
		WEAPON_DUMP_FIELD(hipViewScatterMin);
		WEAPON_DUMP_FIELD(hipViewScatterMax);
		WEAPON_DUMP_FIELD(viewKickScale);
		WEAPON_DUMP_FIELD(positionReloadTransTime);
		WEAPON_DUMP_FIELD(fightDist);
		WEAPON_DUMP_FIELD(maxDist);
		WEAPON_DUMP_FIELD(leftArc);
		WEAPON_DUMP_FIELD(rightArc);
		WEAPON_DUMP_FIELD(topArc);
		WEAPON_DUMP_FIELD(bottomArc);
		WEAPON_DUMP_FIELD(accuracy);
		WEAPON_DUMP_FIELD(aiSpread);
		WEAPON_DUMP_FIELD(playerSpread);
		WEAPON_DUMP_FIELD_ARR(minTurnSpeed, 2);
		WEAPON_DUMP_FIELD_ARR(maxTurnSpeed, 2);
		WEAPON_DUMP_FIELD(pitchConvergenceTime);
		WEAPON_DUMP_FIELD(yawConvergenceTime);
		WEAPON_DUMP_FIELD(suppressTime);
		WEAPON_DUMP_FIELD(maxRange);
		WEAPON_DUMP_FIELD(animHorRotateInc);
		WEAPON_DUMP_FIELD(playerPositionDist);
		//WEAPON_DUMP_FIELD(iUseHintStringIndex);
		//WEAPON_DUMP_FIELD(dropHintStringIndex);
		WEAPON_DUMP_FIELD(horizViewJitter);
		WEAPON_DUMP_FIELD(vertViewJitter);
		WEAPON_DUMP_FIELD(scanSpeed);
		WEAPON_DUMP_FIELD(scanAccel);
		WEAPON_DUMP_FIELD(scanPauseTime);
		WEAPON_DUMP_FIELD(minDamage);
		WEAPON_DUMP_FIELD(midDamage);
		WEAPON_DUMP_FIELD(minPlayerDamage);
		WEAPON_DUMP_FIELD(midPlayerDamage);
		WEAPON_DUMP_FIELD(maxDamageRange);
		WEAPON_DUMP_FIELD(minDamageRange);
		WEAPON_DUMP_FIELD(signatureAmmoInClip);
		WEAPON_DUMP_FIELD(signatureDamage);
		WEAPON_DUMP_FIELD(signatureMidDamage);
		WEAPON_DUMP_FIELD(signatureMinDamage);
		WEAPON_DUMP_FIELD(signatureMaxDamageRange);
		WEAPON_DUMP_FIELD(signatureMinDamageRange);
		WEAPON_DUMP_FIELD(destabilizationRateTime);
		WEAPON_DUMP_FIELD(destabilizationCurvatureMax);
		WEAPON_DUMP_FIELD(destabilizeDistance);
		WEAPON_DUMP_FIELD(turretADSTime);
		WEAPON_DUMP_FIELD(turretFov);
		WEAPON_DUMP_FIELD(turretFovADS);
		WEAPON_DUMP_FIELD(turretScopeZoomRate);
		WEAPON_DUMP_FIELD(turretScopeZoomMin);
		WEAPON_DUMP_FIELD(turretScopeZoomMax);
		WEAPON_DUMP_FIELD(overheatUpRate);
		WEAPON_DUMP_FIELD(overheatDownRate);
		WEAPON_DUMP_FIELD(overheatCooldownRate);
		WEAPON_DUMP_FIELD(overheatPenalty);
		WEAPON_DUMP_FIELD(turretBarrelSpinSpeed);
		WEAPON_DUMP_FIELD(turretBarrelSpinUpTime);
		WEAPON_DUMP_FIELD(turretBarrelSpinDownTime);
		WEAPON_DUMP_FIELD(missileConeSoundRadiusAtTop);
		WEAPON_DUMP_FIELD(missileConeSoundRadiusAtBase);
		WEAPON_DUMP_FIELD(missileConeSoundHeight);
		WEAPON_DUMP_FIELD(missileConeSoundOriginOffset);
		WEAPON_DUMP_FIELD(missileConeSoundVolumescaleAtCore);
		WEAPON_DUMP_FIELD(missileConeSoundVolumescaleAtEdge);
		WEAPON_DUMP_FIELD(missileConeSoundVolumescaleCoreSize);
		WEAPON_DUMP_FIELD(missileConeSoundPitchAtTop);
		WEAPON_DUMP_FIELD(missileConeSoundPitchAtBottom);
		WEAPON_DUMP_FIELD(missileConeSoundPitchTopSize);
		WEAPON_DUMP_FIELD(missileConeSoundPitchBottomSize);
		WEAPON_DUMP_FIELD(missileConeSoundCrossfadeTopSize);
		WEAPON_DUMP_FIELD(missileConeSoundCrossfadeBottomSize);
		WEAPON_DUMP_FIELD(aim_automelee_lerp);
		WEAPON_DUMP_FIELD(aim_automelee_range);
		WEAPON_DUMP_FIELD(aim_automelee_region_height);
		WEAPON_DUMP_FIELD(aim_automelee_region_width);
		WEAPON_DUMP_FIELD(player_meleeHeight);
		WEAPON_DUMP_FIELD(player_meleeRange);
		WEAPON_DUMP_FIELD(player_meleeWidth);
		WEAPON_DUMP_FIELD(changedFireTime);
		WEAPON_DUMP_FIELD(changedFireTimeNumBullets);
		WEAPON_DUMP_FIELD(fireTimeInterpolationType);
		WEAPON_DUMP_FIELD(generateAmmo);
		WEAPON_DUMP_FIELD(ammoPerShot);
		WEAPON_DUMP_FIELD(explodeCount);
		WEAPON_DUMP_FIELD(batteryDischargeRate);
		WEAPON_DUMP_FIELD(extendedBattery);
		WEAPON_DUMP_FIELD(bulletsPerTag);
		WEAPON_DUMP_FIELD(maxTags);
		WEAPON_DUMP_FIELD(rattleSoundType);
		WEAPON_DUMP_FIELD(adsShouldShowCrosshair);
		WEAPON_DUMP_FIELD(adsCrosshairShouldScale);
		WEAPON_DUMP_FIELD(turretADSEnabled);
		WEAPON_DUMP_FIELD(knifeAttachTagLeft);
		WEAPON_DUMP_FIELD(knifeAlwaysAttached);
		WEAPON_DUMP_FIELD(meleeOverrideValues);
		WEAPON_DUMP_FIELD(riotShieldEnableDamage);
		WEAPON_DUMP_FIELD(allowPrimaryWeaponPickup);
		WEAPON_DUMP_FIELD(sharedAmmo);
		WEAPON_DUMP_FIELD(lockonSupported);
		WEAPON_DUMP_FIELD(requireLockonToFire);
		WEAPON_DUMP_FIELD(isAirburstWeapon);
		WEAPON_DUMP_FIELD(bigExplosion);
		WEAPON_DUMP_FIELD(noAdsWhenMagEmpty);
		WEAPON_DUMP_FIELD(avoidDropCleanup);
		WEAPON_DUMP_FIELD(inheritsPerks);
		WEAPON_DUMP_FIELD(crosshairColorChange);
		WEAPON_DUMP_FIELD(rifleBullet);
		WEAPON_DUMP_FIELD(armorPiercing);
		WEAPON_DUMP_FIELD(boltAction);
		WEAPON_DUMP_FIELD(aimDownSight);
		WEAPON_DUMP_FIELD(canHoldBreath);
		WEAPON_DUMP_FIELD(meleeOnly);
		WEAPON_DUMP_FIELD(quickMelee);
		WEAPON_DUMP_FIELD(bU_086);
		WEAPON_DUMP_FIELD(canVariableZoom);
		WEAPON_DUMP_FIELD(rechamberWhileAds);
		WEAPON_DUMP_FIELD(bulletExplosiveDamage);
		WEAPON_DUMP_FIELD(cookOffHold);
		WEAPON_DUMP_FIELD(useBattery);
		WEAPON_DUMP_FIELD(reticleSpin45);
		WEAPON_DUMP_FIELD(clipOnly);
		WEAPON_DUMP_FIELD(noAmmoPickup);
		WEAPON_DUMP_FIELD(disableSwitchToWhenEmpty);
		WEAPON_DUMP_FIELD(suppressAmmoReserveDisplay);
		WEAPON_DUMP_FIELD(motionTracker);
		WEAPON_DUMP_FIELD(markableViewmodel);
		WEAPON_DUMP_FIELD(noDualWield);
		WEAPON_DUMP_FIELD(flipKillIcon);
		WEAPON_DUMP_FIELD(actionSlotShowAmmo);
		WEAPON_DUMP_FIELD(noPartialReload);
		WEAPON_DUMP_FIELD(segmentedReload);
		WEAPON_DUMP_FIELD(multipleReload);
		WEAPON_DUMP_FIELD(blocksProne);
		WEAPON_DUMP_FIELD(silenced);
		WEAPON_DUMP_FIELD(isRollingGrenade);
		WEAPON_DUMP_FIELD(projExplosionEffectForceNormalUp);
		WEAPON_DUMP_FIELD(projExplosionEffectInheritParentDirection);
		WEAPON_DUMP_FIELD(projImpactExplode);
		WEAPON_DUMP_FIELD(projTrajectoryEvents);
		WEAPON_DUMP_FIELD(projWhizByEnabled);
		WEAPON_DUMP_FIELD(stickToPlayers);
		WEAPON_DUMP_FIELD(stickToVehicles);
		WEAPON_DUMP_FIELD(stickToTurrets);
		WEAPON_DUMP_FIELD(thrownSideways);
		WEAPON_DUMP_FIELD(hasDetonatorEmptyThrow);
		WEAPON_DUMP_FIELD(hasDetonatorDoubleTap);
		WEAPON_DUMP_FIELD(disableFiring);
		WEAPON_DUMP_FIELD(timedDetonation);
		WEAPON_DUMP_FIELD(noCrumpleMissile);
		WEAPON_DUMP_FIELD(fuseLitAfterImpact);
		WEAPON_DUMP_FIELD(rotate);
		WEAPON_DUMP_FIELD(holdButtonToThrow);
		WEAPON_DUMP_FIELD(freezeMovementWhenFiring);
		WEAPON_DUMP_FIELD(thermalScope);
		WEAPON_DUMP_FIELD(thermalToggle);
		WEAPON_DUMP_FIELD(outlineEnemies);
		WEAPON_DUMP_FIELD(altModeSameWeapon);
		WEAPON_DUMP_FIELD(turretBarrelSpinEnabled);
		WEAPON_DUMP_FIELD(missileConeSoundEnabled);
		WEAPON_DUMP_FIELD(missileConeSoundPitchshiftEnabled);
		WEAPON_DUMP_FIELD(missileConeSoundCrossfadeEnabled);
		WEAPON_DUMP_FIELD(offhandHoldIsCancelable);
		WEAPON_DUMP_FIELD(doNotAllowAttachmentsToOverrideSpread);
		WEAPON_DUMP_FIELD(useFastReloadAnims);
		WEAPON_DUMP_FIELD(dualMagReloadSupported);
		WEAPON_DUMP_FIELD(reloadStopsAlt);
		WEAPON_DUMP_FIELD(useScopeDrift);
		WEAPON_DUMP_FIELD(alwaysShatterGlassOnImpact);
		WEAPON_DUMP_FIELD(oldWeapon);
		WEAPON_DUMP_FIELD(raiseToHold);
		WEAPON_DUMP_FIELD(notifyOnPlayerImpact);
		WEAPON_DUMP_FIELD(decreasingKick);
		WEAPON_DUMP_FIELD(counterSilencer);
		WEAPON_DUMP_FIELD(projSuppressedByEMP);
		WEAPON_DUMP_FIELD(projDisabledByEMP);
		WEAPON_DUMP_FIELD(autosimDisableVariableRate);
		WEAPON_DUMP_FIELD(projPlayTrailEffectForOwnerOnly);
		WEAPON_DUMP_FIELD(projPlayBeaconEffectForOwnerOnly);
		WEAPON_DUMP_FIELD(projKillTrailEffectOnDeath);
		WEAPON_DUMP_FIELD(projKillBeaconEffectOnDeath);
		WEAPON_DUMP_FIELD(reticleDetonateHide);
		WEAPON_DUMP_FIELD(cloaked);
		WEAPON_DUMP_FIELD(adsHideWeapon);
		WEAPON_DUMP_FIELD(adsHideHands);
		WEAPON_DUMP_FIELD(bU_108);
		WEAPON_DUMP_FIELD(adsSceneBlur);
		WEAPON_DUMP_FIELD(usesSniperScope);
		WEAPON_DUMP_FIELD(hasTransientModels);
		WEAPON_DUMP_FIELD(signatureAmmoAlternate);
		WEAPON_DUMP_FIELD(useScriptCallbackForHit);
		WEAPON_DUMP_FIELD(useBulletTagSystem);
		WEAPON_DUMP_FIELD(hideBulletTags);
		WEAPON_DUMP_FIELD(adsDofPhysicalFstop);
		WEAPON_DUMP_FIELD(adsDofPhysicalFocusDistance);
		WEAPON_DUMP_FIELD(autosimSpeedScale);
		WEAPON_DUMP_FIELD(reactiveMotionRadiusScale);
		WEAPON_DUMP_FIELD(reactiveMotionFrequencyScale);
		WEAPON_DUMP_FIELD(reactiveMotionAmplitudeScale);
		WEAPON_DUMP_FIELD(reactiveMotionFalloff);
		WEAPON_DUMP_FIELD(reactiveMotionLifetime);

		WEAPON_DUMP_FIELD_ARR(fU_3604, 3);

		std::string json = data.dump(4);

		auto file = filesystem::file(path);
		file.open("wb");
		file.write(json.data(), json.size(), 1);
		file.close();
	}
}