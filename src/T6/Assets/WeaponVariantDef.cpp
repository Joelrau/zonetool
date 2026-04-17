// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"
#include <zlib.h>

#define ZONETOOL_BRANDING "Compiled using ZoneTool by RektInator."

namespace ZoneTool
{
	namespace T6
	{
#define WEAPON_DUMP_FIELD(__field__) \
	data[#__field__] = asset->__field__

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

		Json dump_weapondef(WeaponDef* asset)
		{
			Json data;

			//Looking at a previously dumped weapon file 16 seems to be correct.
			for (int i = 0; i < 16; i++)
			{
				if (asset->gunXModel && asset->gunXModel[i])
				{
					data["gunXModel"][i] = asset->gunXModel[i]->name;
				}
				else
				{
					data["gunXModel"][i] = "";
				}

				if (asset->worldModel && asset->worldModel[i])
				{
					data["worldXModel"][i] = asset->worldModel[i]->name;
				}
				else
				{
					data["worldXModel"][i] = "";
				}
			}
			printf("0.1\n");
			//These aren't present in the previously dumped weapon files so 24 may not be correct
			for (int i = 0; i < 24; i++)
			{
				if (asset->notetrackSoundMapKeys && asset->notetrackSoundMapKeys[i])
				{
					data["notetrackSoundMapKeys"][i] = SL_ConvertToString(asset->notetrackSoundMapKeys[i]);
				}
				else
				{
					data["notetrackSoundMapKeys"][i] = "";
				}

				if (asset->notetrackSoundMapValues && asset->notetrackSoundMapValues[i])
				{
					data["notetrackSoundMapValues"][i] = SL_ConvertToString(asset->notetrackSoundMapValues[i]);
				}
				else
				{
					data["notetrackSoundMapValues"][i] = "";
				}
			}
			printf("0.2\n");
			WEAPON_DUMP_ASSET(handXModel);
			WEAPON_DUMP_FIELD(szModeName);
			WEAPON_DUMP_FIELD(playerAnimType);
			WEAPON_DUMP_FIELD(weapType);
			WEAPON_DUMP_FIELD(weapClass);
			WEAPON_DUMP_FIELD(penetrateType);
			WEAPON_DUMP_FIELD(impactType);
			WEAPON_DUMP_FIELD(inventoryType);
			WEAPON_DUMP_FIELD(fireType);
			WEAPON_DUMP_FIELD(clipType);
			WEAPON_DUMP_FIELD(barrelType);
			printf("0.3\n");
			WEAPON_DUMP_FIELD(itemIndex);
			WEAPON_DUMP_FIELD(parentWeaponName);
			WEAPON_DUMP_FIELD(iJamFireTime);
			WEAPON_DUMP_FIELD(overheatWeapon);
			WEAPON_DUMP_FIELD(overheatRate);
			WEAPON_DUMP_FIELD(cooldownRate);
			WEAPON_DUMP_FIELD(overheatEndVal);
			WEAPON_DUMP_FIELD(coolWhileFiring);
			WEAPON_DUMP_FIELD(fuelTankWeapon);
			WEAPON_DUMP_FIELD(iTankLifeTime);
			WEAPON_DUMP_FIELD(offhandClass);
			WEAPON_DUMP_FIELD(offhandSlot);
			WEAPON_DUMP_FIELD(stance);
			WEAPON_DUMP_ASSET(viewFlashEffect);
			WEAPON_DUMP_ASSET(worldFlashEffect);
			WEAPON_DUMP_ASSET(barrelCooldownEffect);
			WEAPON_DUMP_FIELD(barrelCooldownMinCount);
			WEAPON_DUMP_FIELD_ARR(vViewFlashOffset, 3);
			WEAPON_DUMP_FIELD_ARR(vWorldFlashOffset, 3);
			WEAPON_DUMP_FIELD(pickupSound);
			WEAPON_DUMP_FIELD(pickupSoundPlayer);
			WEAPON_DUMP_FIELD(ammoPickupSound);
			WEAPON_DUMP_FIELD(ammoPickupSoundPlayer);
			WEAPON_DUMP_FIELD(projectileSound);
			WEAPON_DUMP_FIELD(pullbackSound);
			WEAPON_DUMP_FIELD(pullbackSoundPlayer);
			WEAPON_DUMP_FIELD(fireSound);
			WEAPON_DUMP_FIELD(fireSoundPlayer);
			WEAPON_DUMP_FIELD(fireLoopSound);
			WEAPON_DUMP_FIELD(fireLoopSoundPlayer);
			WEAPON_DUMP_FIELD(fireLoopEndSound);
			WEAPON_DUMP_FIELD(fireLoopEndSoundPlayer);
			WEAPON_DUMP_FIELD(fireStartSound);
			WEAPON_DUMP_FIELD(fireStopSound);
			WEAPON_DUMP_FIELD(fireKillcamSound);
			WEAPON_DUMP_FIELD(fireStartSoundPlayer);
			WEAPON_DUMP_FIELD(fireStopSoundPlayer);
			WEAPON_DUMP_FIELD(fireKillcamSoundPlayer);
			WEAPON_DUMP_FIELD(fireLastSound);
			WEAPON_DUMP_FIELD(fireLastSoundPlayer);
			WEAPON_DUMP_FIELD(emptyFireSound);
			WEAPON_DUMP_FIELD(emptyFireSoundPlayer);
			WEAPON_DUMP_FIELD(crackSound);
			WEAPON_DUMP_FIELD(whizbySound);
			WEAPON_DUMP_FIELD(meleeSwipeSound);
			WEAPON_DUMP_FIELD(meleeSwipeSoundPlayer);
			WEAPON_DUMP_FIELD(meleeHitSound);
			WEAPON_DUMP_FIELD(meleeMissSound);
			WEAPON_DUMP_FIELD(rechamberSound);
			WEAPON_DUMP_FIELD(rechamberSoundPlayer);
			WEAPON_DUMP_FIELD(reloadSound);
			WEAPON_DUMP_FIELD(reloadSoundPlayer);
			WEAPON_DUMP_FIELD(reloadEmptySound);
			WEAPON_DUMP_FIELD(reloadEmptySoundPlayer);
			WEAPON_DUMP_FIELD(reloadStartSound);
			WEAPON_DUMP_FIELD(reloadStartSoundPlayer);
			WEAPON_DUMP_FIELD(reloadEndSound);
			WEAPON_DUMP_FIELD(reloadEndSoundPlayer);
			WEAPON_DUMP_FIELD(rotateLoopSound);
			WEAPON_DUMP_FIELD(rotateLoopSoundPlayer);
			WEAPON_DUMP_FIELD(rotateStopSound);
			WEAPON_DUMP_FIELD(rotateStopSoundPlayer);
			WEAPON_DUMP_FIELD(deploySound);
			WEAPON_DUMP_FIELD(deploySoundPlayer);
			WEAPON_DUMP_FIELD(finishDeploySound);
			WEAPON_DUMP_FIELD(finishDeploySoundPlayer);
			WEAPON_DUMP_FIELD(breakdownSound);
			WEAPON_DUMP_FIELD(breakdownSoundPlayer);
			WEAPON_DUMP_FIELD(finishBreakdownSound);
			WEAPON_DUMP_FIELD(finishBreakdownSoundPlayer);
			WEAPON_DUMP_FIELD(detonateSound);
			WEAPON_DUMP_FIELD(detonateSoundPlayer);
			WEAPON_DUMP_FIELD(nightVisionWearSound);
			WEAPON_DUMP_FIELD(nightVisionWearSoundPlayer);
			WEAPON_DUMP_FIELD(nightVisionRemoveSound);
			WEAPON_DUMP_FIELD(nightVisionRemoveSoundPlayer);
			WEAPON_DUMP_FIELD(altSwitchSound);
			WEAPON_DUMP_FIELD(altSwitchSoundPlayer);
			WEAPON_DUMP_FIELD(raiseSound);
			WEAPON_DUMP_FIELD(raiseSoundPlayer);
			WEAPON_DUMP_FIELD(firstRaiseSound);
			WEAPON_DUMP_FIELD(firstRaiseSoundPlayer);
			WEAPON_DUMP_FIELD(adsRaiseSoundPlayer);
			WEAPON_DUMP_FIELD(adsLowerSoundPlayer);
			WEAPON_DUMP_FIELD(putawaySound);
			WEAPON_DUMP_FIELD(putawaySoundPlayer);
			WEAPON_DUMP_FIELD(overheatSound);
			WEAPON_DUMP_FIELD(overheatSoundPlayer);
			WEAPON_DUMP_FIELD(adsZoomSound);
			WEAPON_DUMP_FIELD(shellCasing);
			WEAPON_DUMP_FIELD(shellCasingPlayer);
			printf("0.31\n");
			if (asset->bounceSound != nullptr)
			{
				WEAPON_DUMP_FIELD(bounceSound);
			}
			else
			{
				data["bounceSound"] = "";
			}
			//data["bounceSound"] = *asset->bounceSound;
			printf("0.4\n");
			//WEAPON_DUMP_FIELD(bounceSound);
			WEAPON_DUMP_FIELD(standMountedWeapdef);
			WEAPON_DUMP_FIELD(crouchMountedWeapdef);
			WEAPON_DUMP_FIELD(proneMountedWeapdef);
			WEAPON_DUMP_FIELD(standMountedIndex);
			WEAPON_DUMP_FIELD(crouchMountedIndex);
			WEAPON_DUMP_FIELD(proneMountedIndex);
			WEAPON_DUMP_ASSET(viewShellEjectEffect);
			printf("0.45\n");
			WEAPON_DUMP_ASSET(worldShellEjectEffect);
			WEAPON_DUMP_ASSET(viewLastShotEjectEffect);
			WEAPON_DUMP_ASSET(worldLastShotEjectEffect);
			WEAPON_DUMP_FIELD_ARR(vViewShellEjectOffset, 3);
			WEAPON_DUMP_FIELD_ARR(vWorldShellEjectOffset, 3);
			WEAPON_DUMP_FIELD_ARR(vViewShellEjectRotation, 3);
			WEAPON_DUMP_FIELD_ARR(vWorldShellEjectRotation, 3);
			WEAPON_DUMP_ASSET(reticleCenter);
			WEAPON_DUMP_ASSET(reticleSide);
			//data["reticleCenter"] = asset->reticleCenter->name;
			//data["reticleSide"] = asset->reticleSide->name;
			printf("0.5\n");
			WEAPON_DUMP_FIELD(iReticleCenterSize);
			WEAPON_DUMP_FIELD(iReticleSideSize);
			WEAPON_DUMP_FIELD(iReticleMinOfs);
			WEAPON_DUMP_FIELD(activeReticleType);
			WEAPON_DUMP_FIELD_ARR(vStandMove, 3);
			WEAPON_DUMP_FIELD_ARR(vStandRot, 3);
			WEAPON_DUMP_FIELD_ARR(vDuckedOfs, 3);
			WEAPON_DUMP_FIELD_ARR(vDuckedMove, 3);
			WEAPON_DUMP_FIELD_ARR(vDuckedSprintOfs, 3);
			WEAPON_DUMP_FIELD_ARR(vDuckedSprintRot, 3);
			WEAPON_DUMP_FIELD_ARR(vDuckedSprintBob, 2);
			WEAPON_DUMP_FIELD(fDuckedSprintCycleScale);
			WEAPON_DUMP_FIELD_ARR(vSprintOfs, 3);
			WEAPON_DUMP_FIELD_ARR(vSprintRot, 3);
			WEAPON_DUMP_FIELD_ARR(vSprintBob, 2);
			WEAPON_DUMP_FIELD(fSprintCycleScale);
			WEAPON_DUMP_FIELD_ARR(vLowReadyOfs, 3);
			WEAPON_DUMP_FIELD_ARR(vLowReadyRot, 3);
			WEAPON_DUMP_FIELD_ARR(vRideOfs, 3);
			WEAPON_DUMP_FIELD_ARR(vRideRot, 3);
			WEAPON_DUMP_FIELD_ARR(vDtpOfs, 3);
			WEAPON_DUMP_FIELD_ARR(vDtpRot, 3);
			WEAPON_DUMP_FIELD_ARR(vDtpBob, 2);
			WEAPON_DUMP_FIELD(fDtpCycleScale);
			WEAPON_DUMP_FIELD_ARR(vMantleOfs, 3);
			WEAPON_DUMP_FIELD_ARR(vMantleRot, 3);
			WEAPON_DUMP_FIELD_ARR(vSlideOfs, 3);
			WEAPON_DUMP_FIELD_ARR(vSlideRot, 3);
			WEAPON_DUMP_FIELD_ARR(vDuckedRot, 3);
			WEAPON_DUMP_FIELD_ARR(vProneOfs, 3);
			WEAPON_DUMP_FIELD_ARR(vProneMove, 3);
			WEAPON_DUMP_FIELD_ARR(vProneRot, 3);
			WEAPON_DUMP_FIELD_ARR(vStrafeMove, 3);
			WEAPON_DUMP_FIELD_ARR(vStrafeRot, 3);
			WEAPON_DUMP_FIELD(fPosMoveRate);
			WEAPON_DUMP_FIELD(fPosProneMoveRate);
			WEAPON_DUMP_FIELD(fStandMoveMinSpeed);
			WEAPON_DUMP_FIELD(fDuckedMoveMinSpeed);
			WEAPON_DUMP_FIELD(fProneMoveMinSpeed);
			WEAPON_DUMP_FIELD(fPosRotRate);
			WEAPON_DUMP_FIELD(fPosProneRotRate);
			WEAPON_DUMP_FIELD(fStandRotMinSpeed);
			WEAPON_DUMP_FIELD(fDuckedRotMinSpeed);
			WEAPON_DUMP_FIELD(fProneRotMinSpeed);
			printf("0.56\n");
			WEAPON_DUMP_ASSET(worldClipModel);
			WEAPON_DUMP_ASSET(rocketModel);
			WEAPON_DUMP_ASSET(mountedModel);
			WEAPON_DUMP_ASSET(additionalMeleeModel);
			WEAPON_DUMP_ASSET(fireTypeIcon);
			//data["fireTypeIcon"] = asset->fireTypeIcon->name;
			WEAPON_DUMP_ASSET(hudIcon);
			//data["hudIcon"] = asset->hudIcon->name;
			WEAPON_DUMP_FIELD(hudIconRatio);
			WEAPON_DUMP_ASSET(indicatorIcon);
			//data["indicatorIcon"] = asset->indicatorIcon->name;
			WEAPON_DUMP_FIELD(indicatorIconRatio);
			WEAPON_DUMP_ASSET(ammoCounterIcon);
			//data["ammoCounterIcon"] = asset->ammoCounterIcon->name;
			printf("0.6\n");
			WEAPON_DUMP_FIELD(ammoCounterIconRatio);
			WEAPON_DUMP_FIELD(ammoCounterClip);
			WEAPON_DUMP_FIELD(iStartAmmo);
			WEAPON_DUMP_FIELD(iMaxAmmo);
			WEAPON_DUMP_FIELD(shotCount);
			WEAPON_DUMP_FIELD(szSharedAmmoCapName);
			WEAPON_DUMP_FIELD(iSharedAmmoCapIndex);
			WEAPON_DUMP_FIELD(iSharedAmmoCap);
			WEAPON_DUMP_FIELD(unlimitedAmmo);
			WEAPON_DUMP_FIELD(ammoCountClipRelative);
			WEAPON_DUMP_FIELD_ARR(damage, 6);
			WEAPON_DUMP_FIELD_ARR(damageRange, 6);
			printf("0.67\n");
			WEAPON_DUMP_FIELD(minPlayerDamage);
			WEAPON_DUMP_FIELD(damageDuration);
			WEAPON_DUMP_FIELD(damageInterval);
			WEAPON_DUMP_FIELD(playerDamage);
			WEAPON_DUMP_FIELD(iMeleeDamage);
			WEAPON_DUMP_FIELD(iDamageType);
			WEAPON_DUMP_FIELD(explosionTag);
			WEAPON_DUMP_FIELD(iFireDelay);
			WEAPON_DUMP_FIELD(iMeleeDelay);
			WEAPON_DUMP_FIELD(meleeChargeDelay);
			WEAPON_DUMP_FIELD(iDetonateDelay);
			WEAPON_DUMP_FIELD(iSpinUpTime);
			WEAPON_DUMP_FIELD(iSpinDownTime);
			WEAPON_DUMP_FIELD(spinRate);
			WEAPON_DUMP_FIELD(spinLoopSound);
			WEAPON_DUMP_FIELD(spinLoopSoundPlayer);
			WEAPON_DUMP_FIELD(startSpinSound);
			WEAPON_DUMP_FIELD(startSpinSoundPlayer);
			WEAPON_DUMP_FIELD(stopSpinSound);
			WEAPON_DUMP_FIELD(stopSpinSoundPlayer);
			WEAPON_DUMP_FIELD(applySpinPitch);
			WEAPON_DUMP_FIELD(iFireTime);
			WEAPON_DUMP_FIELD(iLastFireTime);
			WEAPON_DUMP_FIELD(iRechamberTime);
			WEAPON_DUMP_FIELD(iRechamberBoltTime);
			WEAPON_DUMP_FIELD(iHoldFireTime);
			WEAPON_DUMP_FIELD(iDetonateTime);
			WEAPON_DUMP_FIELD(iMeleeTime);
			WEAPON_DUMP_FIELD(iBurstDelayTime);
			WEAPON_DUMP_FIELD(meleeChargeTime);
			WEAPON_DUMP_FIELD(iReloadTimeRight);
			WEAPON_DUMP_FIELD(iReloadTimeLeft);
			WEAPON_DUMP_FIELD(reloadShowRocketTime);
			WEAPON_DUMP_FIELD(iReloadEmptyTimeLeft);
			WEAPON_DUMP_FIELD(iReloadAddTime);
			WEAPON_DUMP_FIELD(iReloadEmptyAddTime);
			WEAPON_DUMP_FIELD(iReloadQuickAddTime);
			WEAPON_DUMP_FIELD(iReloadQuickEmptyAddTime);
			WEAPON_DUMP_FIELD(iReloadStartTime);
			WEAPON_DUMP_FIELD(iReloadStartAddTime);
			WEAPON_DUMP_FIELD(iReloadEndTime);
			WEAPON_DUMP_FIELD(iDropTime);
			WEAPON_DUMP_FIELD(iRaiseTime);
			WEAPON_DUMP_FIELD(iAltDropTime);
			WEAPON_DUMP_FIELD(quickDropTime);
			WEAPON_DUMP_FIELD(quickRaiseTime);
			WEAPON_DUMP_FIELD(iFirstRaiseTime);
			WEAPON_DUMP_FIELD(iEmptyRaiseTime);
			WEAPON_DUMP_FIELD(iEmptyDropTime);
			WEAPON_DUMP_FIELD(sprintInTime);
			WEAPON_DUMP_FIELD(sprintLoopTime);
			WEAPON_DUMP_FIELD(sprintOutTime);
			WEAPON_DUMP_FIELD(lowReadyInTime);
			WEAPON_DUMP_FIELD(lowReadyLoopTime);
			WEAPON_DUMP_FIELD(lowReadyOutTime);
			WEAPON_DUMP_FIELD(contFireInTime);
			WEAPON_DUMP_FIELD(contFireLoopTime);
			WEAPON_DUMP_FIELD(contFireOutTime);
			WEAPON_DUMP_FIELD(dtpInTime);
			WEAPON_DUMP_FIELD(dtpLoopTime);
			WEAPON_DUMP_FIELD(dtpOutTime);
			WEAPON_DUMP_FIELD(crawlInTime);
			WEAPON_DUMP_FIELD(crawlForwardTime);
			WEAPON_DUMP_FIELD(crawlBackTime);
			WEAPON_DUMP_FIELD(crawlRightTime);
			WEAPON_DUMP_FIELD(crawlLeftTime);
			WEAPON_DUMP_FIELD(crawlOutFireTime);
			WEAPON_DUMP_FIELD(crawlOutTime);
			WEAPON_DUMP_FIELD(slideInTime);
			WEAPON_DUMP_FIELD(deployTime);
			WEAPON_DUMP_FIELD(breakdownTime);
			WEAPON_DUMP_FIELD(iFlourishTime);
			WEAPON_DUMP_FIELD(nightVisionWearTime);
			WEAPON_DUMP_FIELD(nightVisionWearTimeFadeOutEnd);
			WEAPON_DUMP_FIELD(nightVisionWearTimePowerUp);
			WEAPON_DUMP_FIELD(nightVisionRemoveTime);
			WEAPON_DUMP_FIELD(nightVisionRemoveTimePowerDown);
			WEAPON_DUMP_FIELD(nightVisionRemoveTimeFadeInStart);
			WEAPON_DUMP_FIELD(fuseTime);
			WEAPON_DUMP_FIELD(aiFuseTime);
			WEAPON_DUMP_FIELD(lockOnRadius);
			WEAPON_DUMP_FIELD(lockOnSpeed);
			WEAPON_DUMP_FIELD(requireLockonToFire);
			WEAPON_DUMP_FIELD(noAdsWhenMagEmpty);
			WEAPON_DUMP_FIELD(avoidDropCleanup);
			WEAPON_DUMP_FIELD(stackFire);
			WEAPON_DUMP_FIELD(stackFireSpread);
			WEAPON_DUMP_FIELD(stackFireAccuracyDecay);
			WEAPON_DUMP_FIELD(stackSound);
			WEAPON_DUMP_FIELD(autoAimRange);
			WEAPON_DUMP_FIELD(aimAssistRange);
			WEAPON_DUMP_FIELD(mountableWeapon);
			WEAPON_DUMP_FIELD(aimPadding);
			WEAPON_DUMP_FIELD(enemyCrosshairRange);
			WEAPON_DUMP_FIELD(crosshairColorChange);
			WEAPON_DUMP_FIELD(moveSpeedScale);
			WEAPON_DUMP_FIELD(adsMoveSpeedScale);
			WEAPON_DUMP_FIELD(sprintDurationScale);
			WEAPON_DUMP_FIELD(overlayReticle);
			WEAPON_DUMP_FIELD(overlayInterface);
			WEAPON_DUMP_FIELD(overlayWidth);
			WEAPON_DUMP_FIELD(overlayHeight);
			WEAPON_DUMP_FIELD(fAdsBobFactor);
			WEAPON_DUMP_FIELD(fAdsViewBobMult);
			WEAPON_DUMP_FIELD(bHoldBreathToSteady);
			WEAPON_DUMP_FIELD(fHipSpreadStandMin);
			WEAPON_DUMP_FIELD(fHipSpreadDuckedMin);
			WEAPON_DUMP_FIELD(fHipSpreadProneMin);
			WEAPON_DUMP_FIELD(hipSpreadStandMax);
			WEAPON_DUMP_FIELD(hipSpreadDuckedMax);
			WEAPON_DUMP_FIELD(hipSpreadProneMax);
			WEAPON_DUMP_FIELD(fHipSpreadDecayRate);
			WEAPON_DUMP_FIELD(fHipSpreadFireAdd);
			WEAPON_DUMP_FIELD(fHipSpreadTurnAdd);
			WEAPON_DUMP_FIELD(fHipSpreadMoveAdd);
			WEAPON_DUMP_FIELD(fHipSpreadDuckedDecay);
			WEAPON_DUMP_FIELD(fHipSpreadProneDecay);
			WEAPON_DUMP_FIELD(fHipReticleSidePos);
			WEAPON_DUMP_FIELD(fAdsIdleAmount);
			WEAPON_DUMP_FIELD(fHipIdleAmount);
			WEAPON_DUMP_FIELD(adsIdleSpeed);
			WEAPON_DUMP_FIELD(hipIdleSpeed);
			WEAPON_DUMP_FIELD(fIdleCrouchFactor);
			WEAPON_DUMP_FIELD(fIdleProneFactor);
			WEAPON_DUMP_FIELD(fGunMaxPitch);
			WEAPON_DUMP_FIELD(fGunMaxYaw);
			WEAPON_DUMP_FIELD(swayMaxAngle);
			WEAPON_DUMP_FIELD(swayLerpSpeed);
			WEAPON_DUMP_FIELD(swayPitchScale);
			WEAPON_DUMP_FIELD(swayYawScale);
			WEAPON_DUMP_FIELD(swayHorizScale);
			WEAPON_DUMP_FIELD(swayVertScale);
			WEAPON_DUMP_FIELD(swayShellShockScale);
			WEAPON_DUMP_FIELD(adsSwayMaxAngle);
			WEAPON_DUMP_FIELD(adsSwayLerpSpeed);
			WEAPON_DUMP_FIELD(adsSwayPitchScale);
			WEAPON_DUMP_FIELD(adsSwayYawScale);
			WEAPON_DUMP_FIELD(sharedAmmo);
			WEAPON_DUMP_FIELD(bRifleBullet);
			WEAPON_DUMP_FIELD(armorPiercing);
			WEAPON_DUMP_FIELD(bAirburstWeapon);
			WEAPON_DUMP_FIELD(bBoltAction);
			WEAPON_DUMP_FIELD(bUseAltTagFlash);
			WEAPON_DUMP_FIELD(bUseAntiLagRewind);
			WEAPON_DUMP_FIELD(bIsCarriedKillstreakWeapon);
			WEAPON_DUMP_FIELD(aimDownSight);
			WEAPON_DUMP_FIELD(bRechamberWhileAds);
			WEAPON_DUMP_FIELD(bReloadWhileAds);
			WEAPON_DUMP_FIELD(adsViewErrorMin);
			WEAPON_DUMP_FIELD(adsViewErrorMax);
			WEAPON_DUMP_FIELD(bCookOffHold);
			WEAPON_DUMP_FIELD(bClipOnly);
			WEAPON_DUMP_FIELD(bCanUseInVehicle);
			WEAPON_DUMP_FIELD(bNoDropsOrRaises);
			WEAPON_DUMP_FIELD(adsFireOnly);
			WEAPON_DUMP_FIELD(cancelAutoHolsterWhenEmpty);
			WEAPON_DUMP_FIELD(suppressAmmoReserveDisplay);
			WEAPON_DUMP_FIELD(laserSight);
			WEAPON_DUMP_FIELD(laserSightDuringNightvision);
			WEAPON_DUMP_FIELD(bHideThirdPerson);
			WEAPON_DUMP_FIELD(bHasBayonet);
			WEAPON_DUMP_FIELD(bDualWield);
			WEAPON_DUMP_FIELD(bExplodeOnGround);
			WEAPON_DUMP_FIELD(bThrowBack);
			WEAPON_DUMP_FIELD(bRetrievable);
			WEAPON_DUMP_FIELD(bDieOnRespawn);
			WEAPON_DUMP_FIELD(bNoThirdPersonDropsOrRaises);
			WEAPON_DUMP_FIELD(bContinuousFire);
			WEAPON_DUMP_FIELD(bNoPing);
			WEAPON_DUMP_FIELD(bForceBounce);
			WEAPON_DUMP_FIELD(bUseDroppedModelAsStowed);
			WEAPON_DUMP_FIELD(bNoQuickDropWhenEmpty);
			WEAPON_DUMP_FIELD(bKeepCrosshairWhenADS);
			WEAPON_DUMP_FIELD(bUseOnlyAltWeaoponHideTagsInAltMode);
			WEAPON_DUMP_FIELD(bAltWeaponAdsOnly);
			WEAPON_DUMP_FIELD(bAltWeaponDisableSwitching);
			printf("0.68\n");
			WEAPON_DUMP_ASSET(killIcon);
			//data["killIcon"] = asset->killIcon->name;
			printf("0.681\n");
			WEAPON_DUMP_FIELD(killIconRatio);
			WEAPON_DUMP_FIELD(flipKillIcon);
			WEAPON_DUMP_FIELD(bNoPartialReload);
			WEAPON_DUMP_FIELD(bSegmentedReload);
			WEAPON_DUMP_FIELD(bNoADSAutoReload);
			WEAPON_DUMP_FIELD(iReloadAmmoAdd);
			WEAPON_DUMP_FIELD(iReloadStartAdd);
			WEAPON_DUMP_FIELD(szSpawnedGrenadeWeaponName);
			WEAPON_DUMP_FIELD(szDualWieldWeaponName);
			WEAPON_DUMP_FIELD(dualWieldWeaponIndex);
			WEAPON_DUMP_FIELD(iDropAmmoMin);
			WEAPON_DUMP_FIELD(iDropAmmoMax);
			WEAPON_DUMP_FIELD(iDropClipAmmoMin);
			WEAPON_DUMP_FIELD(iDropClipAmmoMax);
			WEAPON_DUMP_FIELD(iShotsBeforeRechamber);
			WEAPON_DUMP_FIELD(blocksProne);
			WEAPON_DUMP_FIELD(bShowIndicator);
			WEAPON_DUMP_FIELD(isRollingGrenade);
			WEAPON_DUMP_FIELD(useBallisticPrediction);
			WEAPON_DUMP_FIELD(isValuable);
			WEAPON_DUMP_FIELD(isTacticalInsertion);
			WEAPON_DUMP_FIELD(isReviveWeapon);
			WEAPON_DUMP_FIELD(bUseRigidBodyOnVehicle);
			WEAPON_DUMP_FIELD(iExplosionRadius);
			WEAPON_DUMP_FIELD(iExplosionRadiusMin);
			WEAPON_DUMP_FIELD(iIndicatorRadius);
			WEAPON_DUMP_FIELD(iExplosionInnerDamage);
			WEAPON_DUMP_FIELD(iExplosionOuterDamage);
			WEAPON_DUMP_FIELD(damageConeAngle);
			WEAPON_DUMP_FIELD(iProjectileSpeed);
			WEAPON_DUMP_FIELD(iProjectileSpeedUp);
			WEAPON_DUMP_FIELD(iProjectileSpeedRelativeUp);
			WEAPON_DUMP_FIELD(iProjectileSpeedForward);
			WEAPON_DUMP_FIELD(fProjectileTakeParentVelocity);
			WEAPON_DUMP_FIELD(iProjectileActivateDist);
			WEAPON_DUMP_FIELD(projLifetime);
			WEAPON_DUMP_FIELD(timeToAccelerate);
			WEAPON_DUMP_FIELD(projectileCurvature);
			WEAPON_DUMP_ASSET(projectileModel);
			printf("0.682\n");
			WEAPON_DUMP_FIELD(projExplosion);
			WEAPON_DUMP_ASSET(projExplosionEffect);
			WEAPON_DUMP_FIELD(projExplosionEffectForceNormalUp);
			WEAPON_DUMP_ASSET(projExplosionEffect2);
			WEAPON_DUMP_FIELD(projExplosionEffect2ForceNormalUp);
			WEAPON_DUMP_ASSET(projExplosionEffect3);
			WEAPON_DUMP_FIELD(projExplosionEffect3ForceNormalUp);
			WEAPON_DUMP_ASSET(projExplosionEffect4);
			WEAPON_DUMP_FIELD(projExplosionEffect4ForceNormalUp);
			WEAPON_DUMP_ASSET(projExplosionEffect5);
			WEAPON_DUMP_FIELD(projExplosionEffect5ForceNormalUp);
			WEAPON_DUMP_ASSET(projDudEffect);
			WEAPON_DUMP_FIELD(projExplosionSound);
			WEAPON_DUMP_FIELD(projDudSound);
			WEAPON_DUMP_FIELD(mortarShellSound);
			WEAPON_DUMP_FIELD(tankShellSound);
			WEAPON_DUMP_FIELD(bProjImpactExplode);
			WEAPON_DUMP_FIELD(bProjSentientImpactExplode);
			WEAPON_DUMP_FIELD(bProjExplodeWhenStationary);
			WEAPON_DUMP_FIELD(bBulletImpactExplode);
			WEAPON_DUMP_FIELD(stickiness);
			WEAPON_DUMP_FIELD(rotateType);
			printf("0.683\n");
			WEAPON_DUMP_FIELD(plantable);
			WEAPON_DUMP_FIELD(hasDetonator);
			WEAPON_DUMP_FIELD(timedDetonation);
			WEAPON_DUMP_FIELD(bNoCrumpleMissile);
			WEAPON_DUMP_FIELD(rotate);
			WEAPON_DUMP_FIELD(bKeepRolling);
			WEAPON_DUMP_FIELD(holdButtonToThrow);
			WEAPON_DUMP_FIELD(offhandHoldIsCancelable);
			WEAPON_DUMP_FIELD(freezeMovementWhenFiring);
			WEAPON_DUMP_FIELD(lowAmmoWarningThreshold);
			WEAPON_DUMP_FIELD(bDisallowAtMatchStart);
			WEAPON_DUMP_FIELD(meleeChargeRange);
			WEAPON_DUMP_FIELD(bUseAsMelee);
			WEAPON_DUMP_FIELD(isCameraSensor);
			WEAPON_DUMP_FIELD(isAcousticSensor);
			WEAPON_DUMP_FIELD(isLaserSensor);
			WEAPON_DUMP_FIELD(isHoldUseGrenade);
			printf("0.69\n");
			//data["parallelBounce"] = *asset->parallelBounce;
			WEAPON_DUMP_FIELD(parallelBounce);
			//data["perpendicularBounce"] = *asset->perpendicularBounce;
			WEAPON_DUMP_FIELD(perpendicularBounce);
			printf("0.7\n");
			WEAPON_DUMP_ASSET(projTrailEffect);
			WEAPON_DUMP_FIELD_ARR(vProjectileColor, 3);
			WEAPON_DUMP_FIELD(guidedMissileType);
			WEAPON_DUMP_FIELD(maxSteeringAccel);
			WEAPON_DUMP_FIELD(projIgnitionDelay);
			WEAPON_DUMP_ASSET(projIgnitionEffect);
			WEAPON_DUMP_FIELD(projIgnitionSound);
			WEAPON_DUMP_FIELD(fAdsAimPitch);
			WEAPON_DUMP_FIELD(fAdsCrosshairInFrac);
			WEAPON_DUMP_FIELD(fAdsCrosshairOutFrac);
			WEAPON_DUMP_FIELD(adsGunKickReducedKickBullets);
			WEAPON_DUMP_FIELD(adsGunKickReducedKickPercent);
			WEAPON_DUMP_FIELD(fAdsGunKickPitchMin);
			WEAPON_DUMP_FIELD(fAdsGunKickPitchMax);
			WEAPON_DUMP_FIELD(fAdsGunKickYawMin);
			WEAPON_DUMP_FIELD(fAdsGunKickYawMax);
			WEAPON_DUMP_FIELD(fAdsGunKickAccel);
			WEAPON_DUMP_FIELD(fAdsGunKickSpeedMax);
			WEAPON_DUMP_FIELD(fAdsGunKickSpeedDecay);
			WEAPON_DUMP_FIELD(fAdsGunKickStaticDecay);
			WEAPON_DUMP_FIELD(fAdsViewKickPitchMin);
			WEAPON_DUMP_FIELD(fAdsViewKickPitchMax);
			WEAPON_DUMP_FIELD(fAdsViewKickMinMagnitude);
			WEAPON_DUMP_FIELD(fAdsViewKickYawMin);
			WEAPON_DUMP_FIELD(fAdsViewKickYawMax);
			WEAPON_DUMP_FIELD(fAdsRecoilReductionRate);
			WEAPON_DUMP_FIELD(fAdsRecoilReductionLimit);
			WEAPON_DUMP_FIELD(fAdsRecoilReturnRate);
			WEAPON_DUMP_FIELD(fAdsViewScatterMin);
			WEAPON_DUMP_FIELD(fAdsViewScatterMax);
			WEAPON_DUMP_FIELD(fAdsSpread);
			WEAPON_DUMP_FIELD(hipGunKickReducedKickBullets);
			WEAPON_DUMP_FIELD(hipGunKickReducedKickPercent);
			WEAPON_DUMP_FIELD(fHipGunKickPitchMin);
			WEAPON_DUMP_FIELD(fHipGunKickPitchMax);
			WEAPON_DUMP_FIELD(fHipGunKickYawMin);
			WEAPON_DUMP_FIELD(fHipGunKickYawMax);
			WEAPON_DUMP_FIELD(fHipGunKickAccel);
			WEAPON_DUMP_FIELD(fHipGunKickSpeedMax);
			WEAPON_DUMP_FIELD(fHipGunKickSpeedDecay);
			WEAPON_DUMP_FIELD(fHipGunKickStaticDecay);
			WEAPON_DUMP_FIELD(fHipViewKickPitchMin);
			WEAPON_DUMP_FIELD(fHipViewKickPitchMax);
			WEAPON_DUMP_FIELD(fHipViewKickMinMagnitude);
			WEAPON_DUMP_FIELD(fHipViewKickYawMin);
			WEAPON_DUMP_FIELD(fHipViewKickYawMax);
			WEAPON_DUMP_FIELD(fHipViewScatterMin);
			WEAPON_DUMP_FIELD(fHipViewScatterMax);
			WEAPON_DUMP_FIELD(fAdsViewKickCenterDuckedScale);
			WEAPON_DUMP_FIELD(fAdsViewKickCenterProneScale);
			WEAPON_DUMP_FIELD(fAntiQuickScopeTime);
			WEAPON_DUMP_FIELD(fAntiQuickScopeScale);
			WEAPON_DUMP_FIELD(fAntiQuickScopeSpreadMultiplier);
			WEAPON_DUMP_FIELD(fAntiQuickScopeSpreadMax);
			WEAPON_DUMP_FIELD(fAntiQuickScopeSwayFactor);
			WEAPON_DUMP_FIELD(fightDist);
			WEAPON_DUMP_FIELD(maxDist);
			WEAPON_DUMP_FIELD_ARR(accuracyGraphName, 2);
			printf("0.8\n");
			//WEAPON_DUMP_FIELD(accuracyGraphKnots);
			//WEAPON_DUMP_FIELD(originalAccuracyGraphKnots);
			WEAPON_DUMP_FIELD_ARR(accuracyGraphKnotCount, 2);
			WEAPON_DUMP_FIELD_ARR(originalAccuracyGraphKnotCount, 2);
			WEAPON_DUMP_FIELD(iPositionReloadTransTime);
			WEAPON_DUMP_FIELD(leftArc);
			WEAPON_DUMP_FIELD(rightArc);
			WEAPON_DUMP_FIELD(topArc);
			WEAPON_DUMP_FIELD(bottomArc);
			WEAPON_DUMP_FIELD(accuracy);
			WEAPON_DUMP_FIELD(aiSpread);
			WEAPON_DUMP_FIELD(playerSpread);
			printf("0.81\n");
			WEAPON_DUMP_FIELD_ARR(minTurnSpeed, 2);
			WEAPON_DUMP_FIELD_ARR(maxTurnSpeed, 2);
			WEAPON_DUMP_FIELD(pitchConvergenceTime);
			WEAPON_DUMP_FIELD(yawConvergenceTime);
			WEAPON_DUMP_FIELD(suppressTime);
			WEAPON_DUMP_FIELD(maxRange);
			printf("0.82\n");
			WEAPON_DUMP_FIELD(fAnimHorRotateInc);
			printf("0.821\n");
			WEAPON_DUMP_FIELD(fPlayerPositionDist);
			printf("0.822\n");
			if (asset->szUseHintString != nullptr)
			{
				WEAPON_DUMP_FIELD(szUseHintString);
			}
			else
			{
				data["szUseHintString"] = "";
			}
			printf("0.823\n");
			if (asset->dropHintString != nullptr)
			{
				WEAPON_DUMP_FIELD(dropHintString);
			}
			else
			{
				data["dropHintString"] = "";
			}
			printf("0.824\n");
			WEAPON_DUMP_FIELD(iUseHintStringIndex);
			printf("0.83\n");
			WEAPON_DUMP_FIELD(dropHintStringIndex);
			WEAPON_DUMP_FIELD(horizViewJitter);
			WEAPON_DUMP_FIELD(vertViewJitter);
			WEAPON_DUMP_FIELD(cameraShakeScale);
			printf("0.84\n");
			WEAPON_DUMP_FIELD(cameraShakeDuration);
			WEAPON_DUMP_FIELD(cameraShakeRadius);
			WEAPON_DUMP_FIELD(explosionCameraShakeScale);
			WEAPON_DUMP_FIELD(explosionCameraShakeDuration);
			WEAPON_DUMP_FIELD(explosionCameraShakeRadius);
			printf("0.85\n");
			WEAPON_DUMP_FIELD(szScript);
			WEAPON_DUMP_FIELD(destabilizationRateTime);
			WEAPON_DUMP_FIELD(destabilizationCurvatureMax);
			WEAPON_DUMP_FIELD(destabilizeDistance);
			printf("0.9\n");
			WEAPON_DUMP_FIELD_ARR(locationDamageMultipliers, 20);
			printf("0.91\n");
			if (asset->fireRumble != nullptr)
			{
				WEAPON_DUMP_FIELD(fireRumble);
			}
			else
			{
				data["fireRumble"] = "";
			}
			printf("0.911\n");
			if (asset->meleeImpactRumble != nullptr)
			{
				WEAPON_DUMP_FIELD(meleeImpactRumble);
			}
			else
			{
				data["meleeImpactRumble"] = "";
			}
			printf("0.912\n");
			if (asset->reloadRumble != nullptr)
			{
				WEAPON_DUMP_FIELD(reloadRumble);
			}
			else
			{
				data["reloadRumble"] = "";
			}
			printf("0.913\n");
			if (asset->explosionRumble != nullptr)
			{
				WEAPON_DUMP_FIELD(explosionRumble);
			}
			else
			{
				data["explosionRumble"] = "";
			}
			printf("0.914\n");
			WEAPON_DUMP_ASSET(tracerType);
			printf("0.915\n");
			WEAPON_DUMP_ASSET(enemyTracerType);
			printf("0.916\n");
			WEAPON_DUMP_FIELD(adsDofStart);
			printf("0.917\n");
			WEAPON_DUMP_FIELD(adsDofEnd);
			printf("0.92\n");
			WEAPON_DUMP_FIELD(hipDofStart);
			printf("0.921\n");
			WEAPON_DUMP_FIELD(hipDofEnd);
			printf("0.922\n");
			WEAPON_DUMP_FIELD(scanSpeed);
			printf("0.923\n");
			WEAPON_DUMP_FIELD(scanAccel);
			printf("0.924\n");
			WEAPON_DUMP_FIELD(scanPauseTime);
			printf("0.925\n");
			if (asset->flameTableFirstPerson != nullptr)
			{
				WEAPON_DUMP_FIELD(flameTableFirstPerson);
			}
			else
			{
				data["flameTableFirstPerson"] = "";
			}
			printf("0.926\n");
			if (asset->flameTableThirdPerson != nullptr)
			{
				WEAPON_DUMP_FIELD(flameTableThirdPerson);
			}
			else
			{
				data["flameTableThirdPerson"] = "";
			}
			printf("0.927\n");
			WEAPON_DUMP_ASSET(flameTableFirstPersonPtr);
			printf("0.928\n");
			WEAPON_DUMP_ASSET(flameTableThirdPersonPtr);
			printf("0.929\n");
			WEAPON_DUMP_ASSET(tagFx_preparationEffect);
			printf("0.9210\n");
			WEAPON_DUMP_ASSET(tagFlash_preparationEffect);
			printf("0.9211\n");
			WEAPON_DUMP_FIELD(doGibbing);
			printf("0.93\n");
			WEAPON_DUMP_FIELD(maxGibDistance);
			printf("0.931\n");
			WEAPON_DUMP_FIELD(altScopeADSTransInTime);
			printf("0.932\n");
			WEAPON_DUMP_FIELD(altScopeADSTransOutTime);
			printf("0.933\n");
			WEAPON_DUMP_FIELD(iIntroFireTime);
			printf("0.934\n");
			WEAPON_DUMP_FIELD(iIntroFireLength);
			printf("0.935\n");
			WEAPON_DUMP_ASSET(meleeSwipeEffect);
			printf("0.936\n");
			WEAPON_DUMP_ASSET(meleeImpactEffect);
			printf("0.937\n");
			WEAPON_DUMP_ASSET(meleeImpactNoBloodEffect);
			printf("0.938\n");
			if (asset->throwBackType != nullptr)
			{
				WEAPON_DUMP_FIELD(throwBackType);
			}
			else
			{
				data["throwBackType"] = "";
			}
			printf("0.94\n");
			WEAPON_DUMP_ASSET(weaponCamo);
			WEAPON_DUMP_FIELD(customFloat0);
			WEAPON_DUMP_FIELD(customFloat1);
			WEAPON_DUMP_FIELD(customFloat2);
			WEAPON_DUMP_FIELD(customBool0);
			WEAPON_DUMP_FIELD(customBool1);
			printf("0.95\n");
			WEAPON_DUMP_FIELD(customBool2);
			return data;
		}

		Json dump_complete(WeaponVariantDef* asset)
		{
			Json data;
			data["baseAsset"] = asset->szInternalName;
			WEAPON_DUMP_FIELD(iVariantCount);
			for (int i = 0; i < 42; i++)
			{
				if (asset->szXAnims && asset->szXAnims[i])
				{
					data["szXAnims"][i] = asset->szXAnims[i];
				}
				else
				{
					data["szXAnims"][i] = "";
				}
			}
			for (int i = 0; i < 32; i++)
			{
				if (asset->hideTags && asset->hideTags[i])
				{
					data["hideTags"][i] = asset->hideTags[i];
				}
				else
				{
					data["hideTags"][i] = "";
				}
			}
			data["weapDef"] = dump_weapondef(asset->weapDef);
			printf("1\n");
			WEAPON_DUMP_FIELD(szInternalName);
			WEAPON_DUMP_FIELD(szAltWeaponName);
			WEAPON_DUMP_FIELD(szAttachmentUnique);
			printf("1.1\n");
			for (int i = 0; i < 63; i++)
			{
				if (asset->attachments && asset->attachments[i].szInternalName)
				{
					data["attachment"][i] = asset->attachments[i].szInternalName;
				}
				else
				{
					data["attachment"][i] = "";
				}
			}
			for (int i = 0; i < 95; i++)
			{
				if (asset->attachmentUniques && asset->attachmentUniques[i].szInternalName)
				{
					data["uniqueAttachment"][i] = asset->attachmentUniques[i].szInternalName;
				}
				else
				{
					data["uniqueAttachment"][i] = "";
				}
			}
			printf("2\n");
			//WEAPON_DUMP_ASSET(attachViewModel);
			if (asset->attachViewModel != nullptr && asset->attachViewModel->name != nullptr)
			{
				data["attachViewModel"] = asset->attachViewModel->name;
			}
			else
			{
				data["attachViewModel"] = "";
			}
			//auto attachViewModel = *asset->attachViewModel;
			//data["attachViewModel"] = attachViewModel->name;
			printf("2.01\n");
			//WEAPON_DUMP_ASSET(attachWorldModel);
			if (asset->attachWorldModel != nullptr && asset->attachWorldModel->name != nullptr)
			{
				data["attachWorldModel"] = asset->attachWorldModel->name;
			}
			else
			{
				data["attachWorldModel"] = "";
			}
			//auto attachWorldModel = *asset->attachWorldModel;
			//data["attachWorldModel"] = attachWorldModel->name;
			printf("2.02\n");
			WEAPON_DUMP_FIELD(attachViewModelTag);
			//data["attachViewModelTag"] = *asset->attachViewModelTag;
			
			WEAPON_DUMP_FIELD(attachWorldModelTag);
			printf("2.03\n");
			//data["attachWorldModelTag"] = *asset->attachWorldModelTag;
			printf("2.1\n");
			
			WEAPON_DUMP_FIELD_ARR(attachViewModelOffsets, 24);
			WEAPON_DUMP_FIELD_ARR(attachWorldModelOffsets, 24);
			WEAPON_DUMP_FIELD_ARR(attachViewModelRotations, 24);
			WEAPON_DUMP_FIELD_ARR(attachWorldModelRotations, 24);
			WEAPON_DUMP_FIELD_ARR(stowedModelOffsets, 3);
			WEAPON_DUMP_FIELD_ARR(stowedModelRotations, 3);
			WEAPON_DUMP_FIELD(altWeaponIndex);
			WEAPON_DUMP_FIELD(iAttachments);
			WEAPON_DUMP_FIELD(bIgnoreAttachments);
			WEAPON_DUMP_FIELD(iClipSize);
			WEAPON_DUMP_FIELD(iReloadTime);
			WEAPON_DUMP_FIELD(iReloadEmptyTime);
			WEAPON_DUMP_FIELD(iReloadQuickTime);
			WEAPON_DUMP_FIELD(iAdsTransInTime);
			WEAPON_DUMP_FIELD(iAdsTransOutTime);
			WEAPON_DUMP_FIELD(iAltRaiseTime);
			WEAPON_DUMP_FIELD(szAmmoDisplayName);
			WEAPON_DUMP_FIELD(szAmmoName);
			WEAPON_DUMP_FIELD(iAmmoIndex);
			WEAPON_DUMP_FIELD(szClipName);
			WEAPON_DUMP_FIELD(iClipIndex);
			WEAPON_DUMP_FIELD(fAimAssistRangeAds);
			WEAPON_DUMP_FIELD(fAdsSwayHorizScale);
			WEAPON_DUMP_FIELD(fAdsSwayVertScale);
			WEAPON_DUMP_FIELD(fAdsViewKickCenterSpeed);
			WEAPON_DUMP_FIELD(fHipViewKickCenterSpeed);
			WEAPON_DUMP_FIELD(fAdsZoomFov1);
			WEAPON_DUMP_FIELD(fAdsZoomFov2);
			WEAPON_DUMP_FIELD(fAdsZoomFov3);
			WEAPON_DUMP_FIELD(fAdsZoomInFrac);
			WEAPON_DUMP_FIELD(fAdsZoomOutFrac);
			WEAPON_DUMP_FIELD(fOverlayAlphaScale);
			printf("3\n");
			WEAPON_DUMP_FIELD_ARR(fOOPosAnimLength, 2);
			WEAPON_DUMP_FIELD(bSilenced);
			WEAPON_DUMP_FIELD(bDualMag);
			WEAPON_DUMP_FIELD(bInfraRed);
			WEAPON_DUMP_FIELD(bTVGuided);
			WEAPON_DUMP_FIELD_ARR(perks, 2);
			//data["overlayMaterial"] = asset->overlayMaterial->name;
			WEAPON_DUMP_ASSET(overlayMaterial);
			//data["overlayMaterialLowRes"] = asset->overlayMaterialLowRes->name;
			WEAPON_DUMP_ASSET(overlayMaterialLowRes);
			//data["dpadIcon"] = asset->dpadIcon->name;
			WEAPON_DUMP_ASSET(dpadIcon);
			WEAPON_DUMP_FIELD(dpadIconRatio);
			WEAPON_DUMP_FIELD(noAmmoOnDpadIcon);
			WEAPON_DUMP_FIELD(mmsWeapon);
			WEAPON_DUMP_FIELD(mmsInScope);
			WEAPON_DUMP_FIELD(mmsFOV);

			WEAPON_DUMP_FIELD(mmsAspect);
			WEAPON_DUMP_FIELD(mmsMaxDist);
			WEAPON_DUMP_FIELD(ikLeftHandIdlePosForward);
			WEAPON_DUMP_FIELD(ikLeftHandIdlePosRight);
			WEAPON_DUMP_FIELD(ikLeftHandIdlePosUp);
			WEAPON_DUMP_FIELD(ikLeftHandOffsetForward);
			WEAPON_DUMP_FIELD(ikLeftHandOffsetRight);
			WEAPON_DUMP_FIELD(ikLeftHandOffsetUp);
			WEAPON_DUMP_FIELD(ikLeftHandRotationPitch);
			WEAPON_DUMP_FIELD(ikLeftHandRotationYaw);
			WEAPON_DUMP_FIELD(ikLeftHandRotationRoll);
			WEAPON_DUMP_FIELD(bUsingLeftHandProneIK);
			WEAPON_DUMP_FIELD(ikLeftHandProneOffsetForward);
			WEAPON_DUMP_FIELD(ikLeftHandProneOffsetRight);
			WEAPON_DUMP_FIELD(ikLeftHandProneOffsetUp);
			WEAPON_DUMP_FIELD(ikLeftHandProneRotationPitch);
			WEAPON_DUMP_FIELD(ikLeftHandProneRotationYaw);
			WEAPON_DUMP_FIELD(ikLeftHandProneRotationRoll);
			WEAPON_DUMP_FIELD(ikLeftHandUiViewerOffsetForward);
			WEAPON_DUMP_FIELD(ikLeftHandUiViewerOffsetRight);
			WEAPON_DUMP_FIELD(ikLeftHandUiViewerOffsetUp);
			WEAPON_DUMP_FIELD(ikLeftHandUiViewerRotationPitch);
			WEAPON_DUMP_FIELD(ikLeftHandUiViewerRotationYaw);
			WEAPON_DUMP_FIELD(ikLeftHandUiViewerRotationRoll);
			printf("4\n");
			return data;
		}

		void IWeaponVariantDef::dump(WeaponVariantDef* asset)
		{
			std::string path = "weapons/"s + asset->szInternalName;
			std::string json = dump_complete(asset).dump(4);

			auto file = FileSystem::FileOpen(path, "w"s);
			fwrite(json.data(), json.size(), 1, file);
			FileSystem::FileClose(file);
		}
	}
}
