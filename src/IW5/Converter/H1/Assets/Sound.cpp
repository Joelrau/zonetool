#include "stdafx.hpp"
#include "../Include.hpp"

#include "Sound.hpp"

#define STREAMED_TO_LOADED_IW5

namespace
{
	std::string remove_extension(const std::string& filename)
	{
		size_t lastdot = filename.find_last_of(".");
		if (lastdot == std::string::npos) return filename;
		return filename.substr(0, lastdot);
	};
}

namespace ZoneTool::IW5
{
	namespace H1Converter
	{
		bool channel_is_3d[IW5::SoundChannel::SND_CHANNEL_COUNT]
		{
			true,	//SND_CHANNEL_PHYSICS,
			true,	//SND_CHANNEL_AMBDIST1,
			true,	//SND_CHANNEL_AMBDIST2,
			true,	//SND_CHANNEL_ALARM,
			true,	//SND_CHANNEL_AUTO,
			true,	//SND_CHANNEL_AUTO2,
			false,	//SND_CHANNEL_AUTO2D,
			true,	//SND_CHANNEL_AUTODOG,
			true,	//SND_CHANNEL_EXPLOSIONDIST1,
			true,	//SND_CHANNEL_EXPLOSIONDIST2,
			true,	//SND_CHANNEL_EXPLOSIVEIMPACT,
			true,	//SND_CHANNEL_ELEMENT,
			true,	//SND_CHANNEL_ELEMENT_INT,
			true,	//SND_CHANNEL_ELEMENT_EXT,
			true,	//SND_CHANNEL_BULLETIMPACT,
			true,	//SND_CHANNEL_BULLETFLESH1,
			true,	//SND_CHANNEL_BULLETFLESH2,
			true,	//SND_CHANNEL_BULLETWHIZBY,
			true,	//SND_CHANNEL_VEHICLE,
			true,	//SND_CHANNEL_VEHICLELIMITED,
			false,	//SND_CHANNEL_MENU,
			true,	//SND_CHANNEL_BODY,
			false,	//SND_CHANNEL_BODY2D,
			true,	//SND_CHANNEL_RELOAD,
			false,	//SND_CHANNEL_RELOAD2D,
			true,	//SND_CHANNEL_ITEM,
			true,	//SND_CHANNEL_EXPLOSION1,
			true,	//SND_CHANNEL_EXPLOSION2,
			true,	//SND_CHANNEL_EXPLOSION3,
			true,	//SND_CHANNEL_EXPLOSION4,
			true,	//SND_CHANNEL_EXPLOSION5,
			true,	//SND_CHANNEL_EFFECTS1,
			true,	//SND_CHANNEL_EFFECTS2,
			true,	//SND_CHANNEL_EFFECTS3,
			false,	//SND_CHANNEL_EFFECTS2D1,
			false,	//SND_CHANNEL_EFFECTS2D2,
			true,	//SND_CHANNEL_NORESTRICT,
			false,	//SND_CHANNEL_NORESTRICT2D,
			true,	//SND_CHANNEL_AIRCRAFT,
			false,	//SND_CHANNEL_VEHICLE2D,
			true,	//SND_CHANNEL_WEAPON_DIST,
			true,	//SND_CHANNEL_WEAPON_MID,
			true,	//SND_CHANNEL_WEAPON,
			false,	//SND_CHANNEL_WEAPON2D,
			true,	//SND_CHANNEL_NONSHOCK,
			true,	//SND_CHANNEL_NONSHOCK2,
			true,	//SND_CHANNEL_GRONDO3D,
			false,	//SND_CHANNEL_GRONDO2D,
			true,		//SND_CHANNEL_VOICE,
			false,	//SND_CHANNEL_LOCAL,
			false,	//SND_CHANNEL_LOCAL2,
			false,	//SND_CHANNEL_LOCAL3,
			false,	//SND_CHANNEL_AMBIENT,
			false,	//SND_CHANNEL_HURT,
			false,	//SND_CHANNEL_PLAYER1,
			false,	//SND_CHANNEL_PLAYER2,
			false,		//SND_CHANNEL_MUSIC,
			false,		//SND_CHANNEL_MUSICNOPAUSE,
			true,	//SND_CHANNEL_MISSION,
			true,	//SND_CHANNEL_CRITICAL,
			false,	//SND_CHANNEL_ANNOUNCER,
			false,	//SND_CHANNEL_SHELLSHOCK,
		};

		H1::SoundDspBus channel_to_dspbus_index[IW5::SoundChannel::SND_CHANNEL_COUNT]
		{
			H1::SoundDspBus::SND_DSPBUS_PHYSICS,	//SND_CHANNEL_PHYSICS,
			H1::SoundDspBus::SND_DSPBUS_AMBIENCE,	//SND_CHANNEL_AMBDIST1,
			H1::SoundDspBus::SND_DSPBUS_AMBIENCE,	//SND_CHANNEL_AMBDIST2,
			H1::SoundDspBus::SND_DSPBUS_FOLEY,	//SND_CHANNEL_ALARM,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_CHANNEL_AUTO,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_CHANNEL_AUTO2,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_CHANNEL_AUTO2D,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_CHANNEL_AUTODOG,
			H1::SoundDspBus::SND_DSPBUS_EXPLOSIONS,	//SND_CHANNEL_EXPLOSIONDIST1,
			H1::SoundDspBus::SND_DSPBUS_EXPLOSIONS,	//SND_CHANNEL_EXPLOSIONDIST2,
			H1::SoundDspBus::SND_DSPBUS_EXPLOSIONS,	//SND_CHANNEL_EXPLOSIVEIMPACT,
			H1::SoundDspBus::SND_DSPBUS_EMITTERS,	//SND_CHANNEL_ELEMENT,
			H1::SoundDspBus::SND_DSPBUS_EMITTERS,	//SND_CHANNEL_ELEMENT_INT,
			H1::SoundDspBus::SND_DSPBUS_EMITTERS,	//SND_CHANNEL_ELEMENT_EXT,
			H1::SoundDspBus::SND_DSPBUS_WEAPONS,	//SND_CHANNEL_BULLETIMPACT,
			H1::SoundDspBus::SND_DSPBUS_PHYSICS,	//SND_CHANNEL_BULLETFLESH1,
			H1::SoundDspBus::SND_DSPBUS_PHYSICS,	//SND_CHANNEL_BULLETFLESH2,
			H1::SoundDspBus::SND_DSPBUS_WHIZBYS,	//SND_CHANNEL_BULLETWHIZBY,
			H1::SoundDspBus::SND_DSPBUS_VEHICLES,	//SND_CHANNEL_VEHICLE,
			H1::SoundDspBus::SND_DSPBUS_VEHICLES,	//SND_CHANNEL_VEHICLELIMITED,
			H1::SoundDspBus::SND_DSPBUS_INTERFACE,	//SND_CHANNEL_MENU,
			H1::SoundDspBus::SND_DSPBUS_FOLEY,	//SND_CHANNEL_BODY,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_CHANNEL_BODY2D,
			H1::SoundDspBus::SND_DSPBUS_WEAPONS,	//SND_CHANNEL_RELOAD,
			H1::SoundDspBus::SND_DSPBUS_WEAPONS,	//SND_CHANNEL_RELOAD2D,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_CHANNEL_ITEM,
			H1::SoundDspBus::SND_DSPBUS_EXPLOSIONS,	//SND_CHANNEL_EXPLOSION1,
			H1::SoundDspBus::SND_DSPBUS_EXPLOSIONS,	//SND_CHANNEL_EXPLOSION2,
			H1::SoundDspBus::SND_DSPBUS_EXPLOSIONS,	//SND_CHANNEL_EXPLOSION3,
			H1::SoundDspBus::SND_DSPBUS_EXPLOSIONS,	//SND_CHANNEL_EXPLOSION4,
			H1::SoundDspBus::SND_DSPBUS_EXPLOSIONS,	//SND_CHANNEL_EXPLOSION5,
			H1::SoundDspBus::SND_DSPBUS_EMITTERS,	//SND_CHANNEL_EFFECTS1,
			H1::SoundDspBus::SND_DSPBUS_EMITTERS,	//SND_CHANNEL_EFFECTS2,
			H1::SoundDspBus::SND_DSPBUS_EMITTERS,	//SND_CHANNEL_EFFECTS3,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_CHANNEL_EFFECTS2D1,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_CHANNEL_EFFECTS2D2,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_CHANNEL_NORESTRICT,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_CHANNEL_NORESTRICT2D,
			H1::SoundDspBus::SND_DSPBUS_AIR_VEHICLES,	//SND_CHANNEL_AIRCRAFT,
			H1::SoundDspBus::SND_DSPBUS_VEHICLES,	//SND_CHANNEL_VEHICLE2D,
			H1::SoundDspBus::SND_DSPBUS_WEAPONS_DIST,	//SND_CHANNEL_WEAPON_DIST,
			H1::SoundDspBus::SND_DSPBUS_WEAPONS,	//SND_CHANNEL_WEAPON_MID,
			H1::SoundDspBus::SND_DSPBUS_WEAPONS,	//SND_CHANNEL_WEAPON,
			H1::SoundDspBus::SND_DSPBUS_WEAPONS,	//SND_CHANNEL_WEAPON2D,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_CHANNEL_NONSHOCK,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_CHANNEL_NONSHOCK2,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_CHANNEL_GRONDO3D,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_CHANNEL_GRONDO2D,
			H1::SoundDspBus::SND_DSPBUS_VOICES,		//SND_CHANNEL_VOICE,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_CHANNEL_LOCAL,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_CHANNEL_LOCAL2,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_CHANNEL_LOCAL3,
			H1::SoundDspBus::SND_DSPBUS_AMBIENCE,	//SND_CHANNEL_AMBIENT,
			H1::SoundDspBus::SND_DSPBUS_FOLEY,	//SND_CHANNEL_HURT,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_CHANNEL_PLAYER1,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_CHANNEL_PLAYER2,
			H1::SoundDspBus::SND_DSPBUS_MUSIC,		//SND_CHANNEL_MUSIC,
			H1::SoundDspBus::SND_DSPBUS_MUSIC,		//SND_CHANNEL_MUSICNOPAUSE,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_CHANNEL_MISSION,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_CHANNEL_CRITICAL,
			H1::SoundDspBus::SND_DSPBUS_BATTLECHATTER,	//SND_CHANNEL_ANNOUNCER,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_CHANNEL_SHELLSHOCK,
		};

		H1::SoundDspBus volmod_to_dspbus[IW5::SoundVolMod::SND_VOLMOD_COUNT]
		{
			H1::SoundDspBus::SND_DSPBUS_FOLEY,	//SND_VOLMOD_FOLEY,
			H1::SoundDspBus::SND_DSPBUS_WEAPONS,	//SND_VOLMOD_WPNAI,
			H1::SoundDspBus::SND_DSPBUS_WEAPONS,	//SND_VOLMOD_WPNPLYR,
			H1::SoundDspBus::SND_DSPBUS_INTERFACE,	//SND_VOLMOD_HUD,
			H1::SoundDspBus::SND_DSPBUS_INTERFACE,	//SND_VOLMOD_INTERFACE,
			H1::SoundDspBus::SND_DSPBUS_INTERFACE,	//SND_VOLMOD_INTERFACE_MUSIC,
			H1::SoundDspBus::SND_DSPBUS_MUSIC,	//SND_VOLMOD_MUSIC,
			H1::SoundDspBus::SND_DSPBUS_MUSIC,	//SND_VOLMOD_MUSIC_EMITTER,
			H1::SoundDspBus::SND_DSPBUS_AMBIENCE,	//SND_VOLMOD_AMBIENCE,
			H1::SoundDspBus::SND_DSPBUS_AMBIENCE,	//SND_VOLMOD_AMBIENCE_DIST,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_VOLMOD_ELEMENT,
			H1::SoundDspBus::SND_DSPBUS_EMITTERS,	//SND_VOLMOD_EMITTER,
			H1::SoundDspBus::SND_DSPBUS_PHYSICS,	//SND_VOLMOD_PHYSICS,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_VOLMOD_BODYFALL,
			H1::SoundDspBus::SND_DSPBUS_FOLEY,	//SND_VOLMOD_FOLEY_PLR,
			H1::SoundDspBus::SND_DSPBUS_FOLEY,	//SND_VOLMOD_FOLEYMP_PLR,
			H1::SoundDspBus::SND_DSPBUS_FOLEY,	//SND_VOLMOD_FOLEY_NPC,
			H1::SoundDspBus::SND_DSPBUS_FOLEY,	//SND_VOLMOD_FOLEYMP_NPC,
			H1::SoundDspBus::SND_DSPBUS_FOLEY,	//SND_VOLMOD_FOLEY_WPN_PLR,
			H1::SoundDspBus::SND_DSPBUS_FOLEY,	//SND_VOLMOD_FOLEY_WPN_NPC,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_VOLMOD_FOOTSTEPS_PLR,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_VOLMOD_FOOTSTEPS_NPC,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_VOLMOD_FOOTSTEPMP_PLR,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_VOLMOD_FOOTSTEPMP_NPC,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_VOLMOD_MELEE_PLR,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_VOLMOD_MELEE_NPC,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_VOLMOD_CHATTERAL,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_VOLMOD_CHATTERRAX,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_VOLMOD_REACTIONAL,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_VOLMOD_REACTIONALMAX,
			H1::SoundDspBus::SND_DSPBUS_VOICES,	//SND_VOLMOD_VOICEOVER,
			H1::SoundDspBus::SND_DSPBUS_VOICES,	//SND_VOLMOD_VOICEOVER_RADIO,
			H1::SoundDspBus::SND_DSPBUS_VOICES,	//SND_VOLMOD_VOICEOVER_CRITICAL,
			H1::SoundDspBus::SND_DSPBUS_VOICES,	//SND_VOLMOD_VOICEOVER_AMB,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_VOLMOD_DESTRUCT,
			H1::SoundDspBus::SND_DSPBUS_EXPLOSIONS,	//SND_VOLMOD_EXPLOSION,
			H1::SoundDspBus::SND_DSPBUS_EXPLOSIONS,	//SND_VOLMOD_EXPLOSION_GRENADE,
			H1::SoundDspBus::SND_DSPBUS_EXPLOSIONS,	//SND_VOLMOD_EXPLOSION_FLASHBANG,
			H1::SoundDspBus::SND_DSPBUS_EXPLOSIONS,	//SND_VOLMOD_EXPLOSION_ROCKET,
			H1::SoundDspBus::SND_DSPBUS_EXPLOSIONS,	//SND_VOLMOD_EXPLOSION_CAR,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_VOLMOD_IMPACT,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_VOLMOD_IMPACT_PLR,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_VOLMOD_IMPACT_NPC,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_VOLMOD_IMPACTMP,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_VOLMOD_IMPACTMP_PLR,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_VOLMOD_IMPACTMP_NPC,
			H1::SoundDspBus::SND_DSPBUS_WHIZBYS,	//SND_VOLMOD_WHIZBY,
			H1::SoundDspBus::SND_DSPBUS_WHIZBYS,	//SND_VOLMOD_WHIZBYMP,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_VOLMOD_VEHICLE_PLR,
			H1::SoundDspBus::SND_DSPBUS_VEHICLES,	//SND_VOLMOD_VEHICLE_NPC,
			H1::SoundDspBus::SND_DSPBUS_VEHICLES,	//SND_VOLMOD_VEHICLE_WPN_PLR,
			H1::SoundDspBus::SND_DSPBUS_VEHICLES,	//SND_VOLMOD_VEHICLE_WPN_NPC,
			H1::SoundDspBus::SND_DSPBUS_VEHICLES,	//SND_VOLMOD_VEHICLE,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_VOLMOD_GRENADEBOUNCE,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_VOLMOD_GRENADEBOUNCEMP,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_VOLMOD_SHELLCASINGS,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_VOLMOD_SHELLCASINGSMP,
			H1::SoundDspBus::SND_DSPBUS_WEAPONS,	//SND_VOLMOD_WPN_PLR,
			H1::SoundDspBus::SND_DSPBUS_WEAPONS,	//SND_VOLMOD_WPNMP_PLR,
			H1::SoundDspBus::SND_DSPBUS_WEAPONS,	//SND_VOLMOD_WPN_NPC,
			H1::SoundDspBus::SND_DSPBUS_WEAPONS,	//SND_VOLMOD_WPNMP_NPC,
			H1::SoundDspBus::SND_DSPBUS_WEAPONS,	//SND_VOLMOD_WPN_PROJECTILE,
			H1::SoundDspBus::SND_DSPBUS_WEAPONS,	//SND_VOLMOD_WPNMP_PROJECTILE,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_VOLMOD_NA,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_VOLMOD_MAX,
			H1::SoundDspBus::SND_DSPBUS_SCRIPTED1,	//SND_VOLMOD_SCRIPTED1,
			H1::SoundDspBus::SND_DSPBUS_SCRIPTED2,	//SND_VOLMOD_SCRIPTED2,
			H1::SoundDspBus::SND_DSPBUS_SCRIPTED3,	//SND_VOLMOD_SCRIPTED3,
			H1::SoundDspBus::SND_DSPBUS_SCRIPTED4,	//SND_VOLMOD_SCRIPTED4,
			H1::SoundDspBus::SND_DSPBUS_SCRIPTED5,	//SND_VOLMOD_SCRIPTED5,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_VOLMOD_FULLVOLUME,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_VOLMOD_PERKMP_QUIET,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_VOLMOD_LEVEL_AC130,
			H1::SoundDspBus::SND_DSPBUS_DEFAULT,	//SND_VOLMOD_DEFAULT,
		};

		H1::SoundVolMod volmod_to_volmod_index[IW5::SoundVolMod::SND_VOLMOD_COUNT]
		{
			H1::SoundVolMod::SND_VOLMOD_DEFAULT,	//SND_VOLMOD_FOLEY,
			H1::SoundVolMod::SND_VOLMOD_DEFAULT,	//SND_VOLMOD_WPNAI,
			H1::SoundVolMod::SND_VOLMOD_WPN_PLR,	//SND_VOLMOD_WPNPLYR,
			H1::SoundVolMod::SND_VOLMOD_HUD,		//SND_VOLMOD_HUD,
			H1::SoundVolMod::SND_VOLMOD_FRONTEND_SFX,	//SND_VOLMOD_INTERFACE,
			H1::SoundVolMod::SND_VOLMOD_FRONTEND_MUSIC,	//SND_VOLMOD_INTERFACE_MUSIC,
			H1::SoundVolMod::SND_VOLMOD_MUSIC,	//SND_VOLMOD_MUSIC,
			H1::SoundVolMod::SND_VOLMOD_MUSIC_EMITTER,	//SND_VOLMOD_MUSIC_EMITTER,
			H1::SoundVolMod::SND_VOLMOD_AMB_QUAD,	//SND_VOLMOD_AMBIENCE,
			H1::SoundVolMod::SND_VOLMOD_AMB_DIST,	//SND_VOLMOD_AMBIENCE_DIST,
			H1::SoundVolMod::SND_VOLMOD_DEFAULT,	//SND_VOLMOD_ELEMENT,
			H1::SoundVolMod::SND_VOLMOD_AMB_EMITTER,	//SND_VOLMOD_EMITTER,
			H1::SoundVolMod::SND_VOLMOD_PHYSICS,	//SND_VOLMOD_PHYSICS,
			H1::SoundVolMod::SND_VOLMOD_BODYFALL,	//SND_VOLMOD_BODYFALL,
			H1::SoundVolMod::SND_VOLMOD_FOLEY_PLR,	//SND_VOLMOD_FOLEY_PLR,
			H1::SoundVolMod::SND_VOLMOD_FOLEY_PLR,	//SND_VOLMOD_FOLEYMP_PLR,
			H1::SoundVolMod::SND_VOLMOD_FOLEY_NPC,	//SND_VOLMOD_FOLEY_NPC,
			H1::SoundVolMod::SND_VOLMOD_FOLEYMP_NPC,	//SND_VOLMOD_FOLEYMP_NPC,
			H1::SoundVolMod::SND_VOLMOD_WPN_FOLEY_PLR,	//SND_VOLMOD_FOLEY_WPN_PLR,
			H1::SoundVolMod::SND_VOLMOD_WPN_FOLEY_NPC,	//SND_VOLMOD_FOLEY_WPN_NPC,
			H1::SoundVolMod::SND_VOLMOD_FOOTSTEP_PLR,	//SND_VOLMOD_FOOTSTEPS_PLR,
			H1::SoundVolMod::SND_VOLMOD_FOOTSTEP_NPC,	//SND_VOLMOD_FOOTSTEPS_NPC,
			H1::SoundVolMod::SND_VOLMOD_MP_FOOTSTEP_PLR,	//SND_VOLMOD_FOOTSTEPMP_PLR,
			H1::SoundVolMod::SND_VOLMOD_MP_FOOTSTEP_NPC,	//SND_VOLMOD_FOOTSTEPMP_NPC,
			H1::SoundVolMod::SND_VOLMOD_MELEE_PLR,	//SND_VOLMOD_MELEE_PLR,
			H1::SoundVolMod::SND_VOLMOD_MELEE_NPC,	//SND_VOLMOD_MELEE_NPC,
			H1::SoundVolMod::SND_VOLMOD_DEFAULT,	//SND_VOLMOD_CHATTERAL,
			H1::SoundVolMod::SND_VOLMOD_DEFAULT,	//SND_VOLMOD_CHATTERRAX,
			H1::SoundVolMod::SND_VOLMOD_DEFAULT,	//SND_VOLMOD_REACTIONAL,
			H1::SoundVolMod::SND_VOLMOD_DEFAULT,	//SND_VOLMOD_REACTIONALMAX,
			H1::SoundVolMod::SND_VOLMOD_DEFAULT,	//SND_VOLMOD_VOICEOVER,
			H1::SoundVolMod::SND_VOLMOD_DEFAULT,	//SND_VOLMOD_VOICEOVER_RADIO,
			H1::SoundVolMod::SND_VOLMOD_DEFAULT,	//SND_VOLMOD_VOICEOVER_CRITICAL,
			H1::SoundVolMod::SND_VOLMOD_DEFAULT,	//SND_VOLMOD_VOICEOVER_AMB,
			H1::SoundVolMod::SND_VOLMOD_DESTRUCT,	//SND_VOLMOD_DESTRUCT,
			H1::SoundVolMod::SND_VOLMOD_EXPLOSION,	//SND_VOLMOD_EXPLOSION,
			H1::SoundVolMod::SND_VOLMOD_EXPLOSION_GRENADE,	//SND_VOLMOD_EXPLOSION_GRENADE,
			H1::SoundVolMod::SND_VOLMOD_EXPLOSION_FLASHBANG,	//SND_VOLMOD_EXPLOSION_FLASHBANG,
			H1::SoundVolMod::SND_VOLMOD_EXPLOSION_ROCKET,	//SND_VOLMOD_EXPLOSION_ROCKET,
			H1::SoundVolMod::SND_VOLMOD_EXPLOSION_CAR,	//SND_VOLMOD_EXPLOSION_CAR,
			H1::SoundVolMod::SND_VOLMOD_BULLET_IMPACT_GEO,	//SND_VOLMOD_IMPACT,
			H1::SoundVolMod::SND_VOLMOD_BULLET_IMPACT_PLR,	//SND_VOLMOD_IMPACT_PLR,
			H1::SoundVolMod::SND_VOLMOD_BULLET_IMPACT_NPC,	//SND_VOLMOD_IMPACT_NPC,
			H1::SoundVolMod::SND_VOLMOD_BULLET_IMPACT_GEO,	//SND_VOLMOD_IMPACTMP,
			H1::SoundVolMod::SND_VOLMOD_BULLET_IMPACT_PLR,	//SND_VOLMOD_IMPACTMP_PLR,
			H1::SoundVolMod::SND_VOLMOD_BULLET_IMPACT_NPC,	//SND_VOLMOD_IMPACTMP_NPC,
			H1::SoundVolMod::SND_VOLMOD_BULLET_WHIZBY,	//SND_VOLMOD_WHIZBY,
			H1::SoundVolMod::SND_VOLMOD_MP_BULLET_WHIZBY,	//SND_VOLMOD_WHIZBYMP,
			H1::SoundVolMod::SND_VOLMOD_VEHICLE_PLR,	//SND_VOLMOD_VEHICLE_PLR,
			H1::SoundVolMod::SND_VOLMOD_VEHICLE_NPC,	//SND_VOLMOD_VEHICLE_NPC,
			H1::SoundVolMod::SND_VOLMOD_WPN_VEHICLE_PLR,	//SND_VOLMOD_VEHICLE_WPN_PLR,
			H1::SoundVolMod::SND_VOLMOD_WPN_VEHICLE_NPC,	//SND_VOLMOD_VEHICLE_WPN_NPC,
			H1::SoundVolMod::SND_VOLMOD_VEHICLE,	//SND_VOLMOD_VEHICLE,
			H1::SoundVolMod::SND_VOLMOD_GRENADEBOUNCE,	//SND_VOLMOD_GRENADEBOUNCE,
			H1::SoundVolMod::SND_VOLMOD_MP_GRENADEBOUNCE,	//SND_VOLMOD_GRENADEBOUNCEMP,
			H1::SoundVolMod::SND_VOLMOD_SHELLCASINGS,	//SND_VOLMOD_SHELLCASINGS,
			H1::SoundVolMod::SND_VOLMOD_MP_SHELLCASINGS,	//SND_VOLMOD_SHELLCASINGSMP,
			H1::SoundVolMod::SND_VOLMOD_WPN_PLR,	//SND_VOLMOD_WPN_PLR,
			H1::SoundVolMod::SND_VOLMOD_MP_WPN_PLR,	//SND_VOLMOD_WPNMP_PLR,
			H1::SoundVolMod::SND_VOLMOD_WPN_NPC,	//SND_VOLMOD_WPN_NPC,
			H1::SoundVolMod::SND_VOLMOD_MP_WPN_NPC,	//SND_VOLMOD_WPNMP_NPC,
			H1::SoundVolMod::SND_VOLMOD_WPN_PROJECTILE,	//SND_VOLMOD_WPN_PROJECTILE,
			H1::SoundVolMod::SND_VOLMOD_MP_WPN_PROJECTILE,	//SND_VOLMOD_WPNMP_PROJECTILE,
			H1::SoundVolMod::SND_VOLMOD_MW3_DEPRECATED_NA,	//SND_VOLMOD_NA,
			H1::SoundVolMod::SND_VOLMOD_MW3_DEPRECATED_MAX,	//SND_VOLMOD_MAX,
			H1::SoundVolMod::SND_VOLMOD_SCRIPTED1,	//SND_VOLMOD_SCRIPTED1,
			H1::SoundVolMod::SND_VOLMOD_SCRIPTED2,	//SND_VOLMOD_SCRIPTED2,
			H1::SoundVolMod::SND_VOLMOD_SCRIPTED3,	//SND_VOLMOD_SCRIPTED3,
			H1::SoundVolMod::SND_VOLMOD_SCRIPTED4,	//SND_VOLMOD_SCRIPTED4,
			H1::SoundVolMod::SND_VOLMOD_SCRIPTED5,	//SND_VOLMOD_SCRIPTED5,
			H1::SoundVolMod::SND_VOLMOD_FULLVOLUME,	//SND_VOLMOD_FULLVOLUME,
			H1::SoundVolMod::SND_VOLMOD_MP_PERK_QUIET,	//SND_VOLMOD_PERKMP_QUIET,
			H1::SoundVolMod::SND_VOLMOD_DEFAULT,	//SND_VOLMOD_LEVEL_AC130,
			H1::SoundVolMod::SND_VOLMOD_DEFAULT,	//SND_VOLMOD_DEFAULT,
		};

		void GenerateH1SoundAlias(snd_alias_t* alias, H1::snd_alias_t* h1_alias, int index, allocator& mem, const std::function<std::string(const char* filename)>& get_streamed_sound_data)
		{
			h1_alias->aliasName = alias->aliasName;
			h1_alias->subtitle = alias->subtitle;
			h1_alias->secondaryAliasName = alias->secondaryAliasName;
			h1_alias->chainAliasName = alias->chainAliasName;

			h1_alias->soundFile;
			h1_alias->soundFile = mem.allocate<H1::SoundFile>();

			h1_alias->soundFile->exists = alias->soundFile->exists;
			h1_alias->soundFile->type = static_cast<H1::snd_alias_type_t>(alias->soundFile->type);

			switch (h1_alias->soundFile->type)
			{
			case H1::snd_alias_type_t::SAT_LOADED:
			{
				const std::string s_name = alias->soundFile->u.loadSnd->name;
				const std::string s_name_no_ext = remove_extension(s_name);

				h1_alias->soundFile->u.loadSnd = mem.allocate<H1::LoadedSound>();
				h1_alias->soundFile->u.loadSnd->name = mem.duplicate_string(s_name_no_ext);
				break;
			}
			case H1::snd_alias_type_t::SAT_STREAMED:
			{
#ifdef STREAMED_TO_LOADED_IW5
				h1_alias->soundFile->type = H1::snd_alias_type_t::SAT_LOADED;
				const std::string s_dir = alias->soundFile->u.streamSnd.dir;
				const std::string s_name = alias->soundFile->u.streamSnd.name;
				const auto path = std::filesystem::path(s_dir + "/" + s_name);

				std::string noext_path = remove_extension(path.string());

				const auto is_mp3 = path.string().find(".mp3") != std::string::npos;
				//ZONETOOL_INFO("SAT_STREAMED->SAT_LOADED (%s)", alias->aliasName/*, path.string().data()*/);

				if (!is_mp3)
				{
					const auto dest_path = va("loaded_sound/%s", path.string().data());
					const auto internal_path = va("sound/%s", path.string().data());

					auto data = get_streamed_sound_data(internal_path.data());

					if (data.size())
					{
						filesystem::file dest_file(dest_path);
						dest_file.open("wb");
						dest_file.write(data.data(), data.size(), 1);
						dest_file.close();
					}
				}
				else
				{
					ZONETOOL_INFO("manual conversion needed for file: %s", path.string().data());
				}

				h1_alias->soundFile->u.loadSnd = mem.allocate<H1::LoadedSound>();
				h1_alias->soundFile->u.loadSnd->name = mem.duplicate_string(noext_path);
#else
				h1_alias->soundFile->u.streamSnd.filename.isLocalized = false;
				h1_alias->soundFile->u.streamSnd.filename.isStreamed = false;
				h1_alias->soundFile->u.streamSnd.filename.fileIndex = 0;

				h1_alias->soundFile->u.streamSnd.filename.info.raw.name = alias->soundFile->sound.streamSnd.name;
				h1_alias->soundFile->u.streamSnd.filename.info.raw.dir = alias->soundFile->sound.streamSnd.dir;

				h1_alias->soundFile->u.streamSnd.totalMsec = 0;
#endif
				break;
			}
			default:
				__debugbreak();
				break;
			}

			h1_alias->squelchName = nullptr;

			h1_alias->polyCount = 1;
			h1_alias->polyGlobalCount = 174;
			h1_alias->polyEntityType = 0;
			h1_alias->polyGlobalType = 0;

			h1_alias->dspBusIndex = 0; // default;
			h1_alias->priority = 1;

			h1_alias->playCount = 0;
			h1_alias->polyClass = 0;
			h1_alias->unk = 0;

			h1_alias->volMin = alias->volMin;
			h1_alias->volMax = alias->volMax;
			h1_alias->volModIndex = 0; // default

			h1_alias->pitchMin = alias->pitchMin;
			h1_alias->pitchMax = alias->pitchMax;

			h1_alias->distMin = alias->distMin;
			h1_alias->distMax = alias->distMax;

			h1_alias->velocityMin = alias->velocityMin;

			h1_alias->masterPriority = alias->masterPriority;
			h1_alias->masterPercentage = alias->masterPercentage;

			h1_alias->slavePercentage = alias->slavePercentage;

			h1_alias->playbackPercentage = 100;
			h1_alias->probability = alias->probability;

			h1_alias->sndContext = nullptr;

			h1_alias->sequence = index + 1; // alias->sequence;
			h1_alias->lfePercentage = alias->lfePercentage;
			h1_alias->centerPercentage = alias->centerPercentage;
			h1_alias->startDelay = alias->startDelay;

			auto* default_sndcurve = mem.allocate<H1::SndCurve>();
			default_sndcurve->name = "$default";
			default_sndcurve->isDefault = 1;

			H1::SndCurve* sndcurve = nullptr;
			if (alias->volumeFalloffCurve && alias->volumeFalloffCurve->filename != ""s)
			{
				sndcurve = mem.allocate<H1::SndCurve>();
				sndcurve->filename = alias->volumeFalloffCurve->filename;
				sndcurve->isDefault = 0;
				sndcurve->knotCount = alias->volumeFalloffCurve->knotCount;
				memcpy(sndcurve->knots, alias->volumeFalloffCurve->knots, sizeof(float[16][2]));
			}
			else
			{
				sndcurve = default_sndcurve;
			}

			h1_alias->volumeFalloffCurve = sndcurve;
			h1_alias->lpfCurve = default_sndcurve;
			h1_alias->reverbSendCurve = default_sndcurve;

			h1_alias->speakerMap = nullptr;
			if (alias->speakerMap)
			{
				h1_alias->speakerMap = mem.allocate<H1::SpeakerMap>();
				h1_alias->speakerMap->isDefault = alias->speakerMap->isDefault;
				h1_alias->speakerMap->name = alias->speakerMap->name;
				h1_alias->speakerMap->orientation = 0;

				for (char x = 0; x < 2; x++)
				{
					for (char y = 0; y < 2; y++)
					{
						h1_alias->speakerMap->channelMaps[x][y].speakerCount = alias->speakerMap->channelMaps[x][y].speakerCount;
						for (char i = 0; i < 6; i++)
						{
							h1_alias->speakerMap->channelMaps[x][y].speakers[i].speaker = alias->speakerMap->channelMaps[x][y].speakers[i].speaker;
							h1_alias->speakerMap->channelMaps[x][y].speakers[i].numLevels = alias->speakerMap->channelMaps[x][y].speakers[i].numLevels;
							h1_alias->speakerMap->channelMaps[x][y].speakers[i].levels[0] = alias->speakerMap->channelMaps[x][y].speakers[i].levels[0];
							h1_alias->speakerMap->channelMaps[x][y].speakers[i].levels[1] = alias->speakerMap->channelMaps[x][y].speakers[i].levels[1];
						}
					}
				}
			}

			h1_alias->envelopMin = alias->envelopMin;
			h1_alias->envelopMax = alias->envelopMax;

			h1_alias->wetMixOverride = 0.0f;
			h1_alias->focusPercentage = 1.0f;
			h1_alias->smartpanDistance2d = 0.0f;
			h1_alias->smartpanDistance3d = 0.0f;
			h1_alias->smartpanAttenuation3d = 1.0f;
			h1_alias->minSmartpan2dContribution = 0.0f;

			h1_alias->stereo3DAngle = 0;
			h1_alias->stereo3DStart = 0.0f;
			h1_alias->stereo3DEnd = 0.0f;

			h1_alias->allowDoppler = 0;
			h1_alias->dopplerPreset = nullptr;

			h1_alias->variationType = 4;
			h1_alias->threshold = 0.5011872053146362f;
			h1_alias->lockedLoopTime = 0.0f;

			// convert flags
			H1::SoundAliasFlags h1_flags{ 0 };
			IW5::SoundAliasFlags iw5_flags{ 0 };

			h1_flags.intValue = 0;
			iw5_flags.intValue = alias->flags.intValue;

			h1_flags.packed.looping = iw5_flags.looping;
			h1_flags.packed.isMaster = iw5_flags.isMaster;
			h1_flags.packed.isSlave = iw5_flags.isSlave;
			h1_flags.packed.fullDryLevel = iw5_flags.fullDryLevel;
			h1_flags.packed.noWetLevel = iw5_flags.noWetLevel;
			h1_flags.packed.randomLooping = iw5_flags.randomLooping;
			h1_flags.packed.spatializedRangeCheck = iw5_flags.unk_is3D;
			h1_flags.packed.spatializedIs3D = !channel_is_3d[iw5_flags.channel]; // dont ask...
			h1_flags.packed.unk9 = 1;
			h1_flags.packed.inheritPitch = 0;
			h1_flags.packed.inheritVolume = 0;
			h1_flags.packed.useContextList = 0;
			h1_flags.packed.useNoPanning2D = 0;
			h1_flags.packed.useOldPanning = 0;
			h1_flags.packed.useNoPanning3D = 0;
			h1_flags.packed.type = h1_alias->soundFile->type; //iw5_flags.type;
			h1_flags.packed.unused = 0;

			h1_alias->flags = h1_flags.intValue;

			h1_alias->volModIndex = volmod_to_volmod_index[alias->volModIndex];
			h1_alias->dspBusIndex = channel_to_dspbus_index[iw5_flags.channel];
			if (h1_alias->dspBusIndex == H1::SND_DSPBUS_DEFAULT)
			{
				h1_alias->dspBusIndex = volmod_to_dspbus[alias->volModIndex];
			}
		}

		H1::snd_alias_list_t* GenerateH1Sound(snd_alias_list_t* asset, allocator& mem, const std::function<std::string(const char* filename)>& get_streamed_sound_data)
		{
			auto* h1_asset = mem.allocate<H1::snd_alias_list_t>();
			h1_asset->aliasName = asset->aliasName;
			
			h1_asset->count = asset->count;

			h1_asset->head = mem.allocate<H1::snd_alias_t>(h1_asset->count);
			for (unsigned char i = 0; i < h1_asset->count; i++)
			{
				GenerateH1SoundAlias(&asset->head[i], &h1_asset->head[i], i, mem, get_streamed_sound_data);
			}

			h1_asset->contextList = nullptr;
			h1_asset->contextListCount = 0;

			return h1_asset;
		}

		H1::snd_alias_list_t* convert(snd_alias_list_t* asset, allocator& allocator, const std::function<std::string(const char* filename)>& get_streamed_sound_data)
		{
			// generate h1 asset
			return GenerateH1Sound(asset, allocator, get_streamed_sound_data);
		}
	}
}

#undef STREAMED_TO_LOADED_IW5