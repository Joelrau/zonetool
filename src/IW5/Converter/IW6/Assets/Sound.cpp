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
	namespace IW6Converter
	{
		IW6::SoundChannel channel_to_channel_index[IW5::SoundChannel::SND_CHANNEL_COUNT]
		{
			IW6::SoundChannel::SND_CHANNEL_PHYSICS,	//SND_CHANNEL_PHYSICS,
			IW6::SoundChannel::SND_CHANNEL_AMBDIST1,	//SND_CHANNEL_AMBDIST1,
			IW6::SoundChannel::SND_CHANNEL_AMBDIST2,	//SND_CHANNEL_AMBDIST2,
			IW6::SoundChannel::SND_CHANNEL_ALARM,	//SND_CHANNEL_ALARM,
			IW6::SoundChannel::SND_CHANNEL_AUTO,	//SND_CHANNEL_AUTO,
			IW6::SoundChannel::SND_CHANNEL_AUTO2,	//SND_CHANNEL_AUTO2,
			IW6::SoundChannel::SND_CHANNEL_AUTO2D,	//SND_CHANNEL_AUTO2D,
			IW6::SoundChannel::SND_CHANNEL_AUTODOG,	//SND_CHANNEL_AUTODOG,
			IW6::SoundChannel::SND_CHANNEL_EXPLOSIONDIST1,	//SND_CHANNEL_EXPLOSIONDIST1,
			IW6::SoundChannel::SND_CHANNEL_EXPLOSIONDIST2,	//SND_CHANNEL_EXPLOSIONDIST2,
			IW6::SoundChannel::SND_CHANNEL_EXPLOSIVEIMPACT,	//SND_CHANNEL_EXPLOSIVEIMPACT,
			IW6::SoundChannel::SND_CHANNEL_ELEMENT,	//SND_CHANNEL_ELEMENT,
			IW6::SoundChannel::SND_CHANNEL_ELEMENT_INT,	//SND_CHANNEL_ELEMENT_INT,
			IW6::SoundChannel::SND_CHANNEL_ELEMENT_EXT,	//SND_CHANNEL_ELEMENT_EXT,
			IW6::SoundChannel::SND_CHANNEL_BULLETIMPACT,	//SND_CHANNEL_BULLETIMPACT,
			IW6::SoundChannel::SND_CHANNEL_BULLETFLESH1,	//SND_CHANNEL_BULLETFLESH1,
			IW6::SoundChannel::SND_CHANNEL_BULLETFLESH2,	//SND_CHANNEL_BULLETFLESH2,
			IW6::SoundChannel::SND_CHANNEL_AUTO,	//SND_CHANNEL_BULLETWHIZBY,
			IW6::SoundChannel::SND_CHANNEL_VEHICLE,	//SND_CHANNEL_VEHICLE,
			IW6::SoundChannel::SND_CHANNEL_VEHICLELIMITED,	//SND_CHANNEL_VEHICLELIMITED,
			IW6::SoundChannel::SND_CHANNEL_MENU,	//SND_CHANNEL_MENU,
			IW6::SoundChannel::SND_CHANNEL_BODY,	//SND_CHANNEL_BODY,
			IW6::SoundChannel::SND_CHANNEL_BODY2D,	//SND_CHANNEL_BODY2D,
			IW6::SoundChannel::SND_CHANNEL_RELOAD,	//SND_CHANNEL_RELOAD,
			IW6::SoundChannel::SND_CHANNEL_RELOAD2D,	//SND_CHANNEL_RELOAD2D,
			IW6::SoundChannel::SND_CHANNEL_ITEM,	//SND_CHANNEL_ITEM,
			IW6::SoundChannel::SND_CHANNEL_EXPLOSION1,	//SND_CHANNEL_EXPLOSION1,
			IW6::SoundChannel::SND_CHANNEL_EXPLOSION2,	//SND_CHANNEL_EXPLOSION2,
			IW6::SoundChannel::SND_CHANNEL_EXPLOSION3,	//SND_CHANNEL_EXPLOSION3,
			IW6::SoundChannel::SND_CHANNEL_EXPLOSION4,	//SND_CHANNEL_EXPLOSION4,
			IW6::SoundChannel::SND_CHANNEL_EXPLOSION5,	//SND_CHANNEL_EXPLOSION5,
			IW6::SoundChannel::SND_CHANNEL_EFFECTS1,	//SND_CHANNEL_EFFECTS1,
			IW6::SoundChannel::SND_CHANNEL_EFFECTS2,	//SND_CHANNEL_EFFECTS2,
			IW6::SoundChannel::SND_CHANNEL_EFFECTS3,	//SND_CHANNEL_EFFECTS3,
			IW6::SoundChannel::SND_CHANNEL_EFFECTS2D1,	//SND_CHANNEL_EFFECTS2D1,
			IW6::SoundChannel::SND_CHANNEL_EFFECTS2D2,	//SND_CHANNEL_EFFECTS2D2,
			IW6::SoundChannel::SND_CHANNEL_NORESTRICT,	//SND_CHANNEL_NORESTRICT,
			IW6::SoundChannel::SND_CHANNEL_NORESTRICT2D,	//SND_CHANNEL_NORESTRICT2D,
			IW6::SoundChannel::SND_CHANNEL_AIRCRAFT,	//SND_CHANNEL_AIRCRAFT,
			IW6::SoundChannel::SND_CHANNEL_VEHICLE2D,	//SND_CHANNEL_VEHICLE2D,
			IW6::SoundChannel::SND_CHANNEL_WEAPON_DIST,	//SND_CHANNEL_WEAPON_DIST,
			IW6::SoundChannel::SND_CHANNEL_WEAPON_MID,	//SND_CHANNEL_WEAPON_MID,
			IW6::SoundChannel::SND_CHANNEL_WEAPON,	//SND_CHANNEL_WEAPON,
			IW6::SoundChannel::SND_CHANNEL_WEAPON2D,	//SND_CHANNEL_WEAPON2D,
			IW6::SoundChannel::SND_CHANNEL_NONSHOCK,	//SND_CHANNEL_NONSHOCK,
			IW6::SoundChannel::SND_CHANNEL_NONSHOCK2,	//SND_CHANNEL_NONSHOCK2,
			IW6::SoundChannel::SND_CHANNEL_AUTO,	//SND_CHANNEL_GRONDO3D,
			IW6::SoundChannel::SND_CHANNEL_AUTO,	//SND_CHANNEL_GRONDO2D,
			IW6::SoundChannel::SND_CHANNEL_VOICE,		//SND_CHANNEL_VOICE,
			IW6::SoundChannel::SND_CHANNEL_LOCAL,	//SND_CHANNEL_LOCAL,
			IW6::SoundChannel::SND_CHANNEL_LOCAL2,	//SND_CHANNEL_LOCAL2,
			IW6::SoundChannel::SND_CHANNEL_LOCAL3,	//SND_CHANNEL_LOCAL3,
			IW6::SoundChannel::SND_CHANNEL_AMBIENT,	//SND_CHANNEL_AMBIENT,
			IW6::SoundChannel::SND_CHANNEL_HURT,	//SND_CHANNEL_HURT,
			IW6::SoundChannel::SND_CHANNEL_PLAYER1,	//SND_CHANNEL_PLAYER1,
			IW6::SoundChannel::SND_CHANNEL_PLAYER2,	//SND_CHANNEL_PLAYER2,
			IW6::SoundChannel::SND_CHANNEL_MUSIC,		//SND_CHANNEL_MUSIC,
			IW6::SoundChannel::SND_CHANNEL_MUSICNOPAUSE,		//SND_CHANNEL_MUSICNOPAUSE,
			IW6::SoundChannel::SND_CHANNEL_MISSION,	//SND_CHANNEL_MISSION,
			IW6::SoundChannel::SND_CHANNEL_AUTO,	//SND_CHANNEL_CRITICAL,
			IW6::SoundChannel::SND_CHANNEL_ANNOUNCER,	//SND_CHANNEL_ANNOUNCER,
			IW6::SoundChannel::SND_CHANNEL_SHELLSHOCK,	//SND_CHANNEL_SHELLSHOCK,
		};

		IW6::SoundVolMod volmod_to_volmod_index[IW5::SoundVolMod::SND_VOLMOD_COUNT]
		{
			IW6::SoundVolMod::SND_VOLMOD_FOLEY,	//SND_VOLMOD_FOLEY,
			IW6::SoundVolMod::SND_VOLMOD_WPNAI,	//SND_VOLMOD_WPNAI,
			IW6::SoundVolMod::SND_VOLMOD_WPNPLYR,	//SND_VOLMOD_WPNPLYR,
			IW6::SoundVolMod::SND_VOLMOD_HUD,		//SND_VOLMOD_HUD,
			IW6::SoundVolMod::SND_VOLMOD_INTERFACE,	//SND_VOLMOD_INTERFACE,
			IW6::SoundVolMod::SND_VOLMOD_INTERFACE_MUSIC,	//SND_VOLMOD_INTERFACE_MUSIC,
			IW6::SoundVolMod::SND_VOLMOD_MUSIC,	//SND_VOLMOD_MUSIC,
			IW6::SoundVolMod::SND_VOLMOD_MUSIC_EMITTER,	//SND_VOLMOD_MUSIC_EMITTER,
			IW6::SoundVolMod::SND_VOLMOD_AMBIENCE,	//SND_VOLMOD_AMBIENCE,
			IW6::SoundVolMod::SND_VOLMOD_AMBIENCE_DIST,	//SND_VOLMOD_AMBIENCE_DIST,
			IW6::SoundVolMod::SND_VOLMOD_ELEMENT,	//SND_VOLMOD_ELEMENT,
			IW6::SoundVolMod::SND_VOLMOD_EMITTER,	//SND_VOLMOD_EMITTER,
			IW6::SoundVolMod::SND_VOLMOD_PHYSICS,	//SND_VOLMOD_PHYSICS,
			IW6::SoundVolMod::SND_VOLMOD_BODYFALL,	//SND_VOLMOD_BODYFALL,
			IW6::SoundVolMod::SND_VOLMOD_FOLEY_PLR,	//SND_VOLMOD_FOLEY_PLR,
			IW6::SoundVolMod::SND_VOLMOD_FOLEYMP_PLR,	//SND_VOLMOD_FOLEYMP_PLR,
			IW6::SoundVolMod::SND_VOLMOD_FOLEY_NPC,	//SND_VOLMOD_FOLEY_NPC,
			IW6::SoundVolMod::SND_VOLMOD_FOLEYMP_NPC,	//SND_VOLMOD_FOLEYMP_NPC,
			IW6::SoundVolMod::SND_VOLMOD_FOLEY_WPN_PLR,	//SND_VOLMOD_FOLEY_WPN_PLR,
			IW6::SoundVolMod::SND_VOLMOD_FOLEY_WPN_NPC,	//SND_VOLMOD_FOLEY_WPN_NPC,
			IW6::SoundVolMod::SND_VOLMOD_FOOTSTEP_PLR,	//SND_VOLMOD_FOOTSTEPS_PLR,
			IW6::SoundVolMod::SND_VOLMOD_FOOTSTEP_NPC,	//SND_VOLMOD_FOOTSTEPS_NPC,
			IW6::SoundVolMod::SND_VOLMOD_FOOTSTEPMP_PLR,	//SND_VOLMOD_FOOTSTEPMP_PLR,
			IW6::SoundVolMod::SND_VOLMOD_FOOTSTEPMP_NPC,	//SND_VOLMOD_FOOTSTEPMP_NPC,
			IW6::SoundVolMod::SND_VOLMOD_MELEE_PLR,	//SND_VOLMOD_MELEE_PLR,
			IW6::SoundVolMod::SND_VOLMOD_MELEE_NPC,	//SND_VOLMOD_MELEE_NPC,
			IW6::SoundVolMod::SND_VOLMOD_CHATTERAL,	//SND_VOLMOD_CHATTERAL,
			IW6::SoundVolMod::SND_VOLMOD_CHATTERAL,	//SND_VOLMOD_CHATTERRAX,
			IW6::SoundVolMod::SND_VOLMOD_REACTIONAL,	//SND_VOLMOD_REACTIONAL,
			IW6::SoundVolMod::SND_VOLMOD_REACTIONAL,	//SND_VOLMOD_REACTIONALMAX,
			IW6::SoundVolMod::SND_VOLMOD_VOICEOVER,	//SND_VOLMOD_VOICEOVER,
			IW6::SoundVolMod::SND_VOLMOD_VOICEOVER_RADIO,	//SND_VOLMOD_VOICEOVER_RADIO,
			IW6::SoundVolMod::SND_VOLMOD_VOICEOVER_CRITICAL,	//SND_VOLMOD_VOICEOVER_CRITICAL,
			IW6::SoundVolMod::SND_VOLMOD_VOICEOVER_AMB,	//SND_VOLMOD_VOICEOVER_AMB,
			IW6::SoundVolMod::SND_VOLMOD_DESTRUCT,	//SND_VOLMOD_DESTRUCT,
			IW6::SoundVolMod::SND_VOLMOD_EXPLOSION,	//SND_VOLMOD_EXPLOSION,
			IW6::SoundVolMod::SND_VOLMOD_EXPLOSION_GRENADE,	//SND_VOLMOD_EXPLOSION_GRENADE,
			IW6::SoundVolMod::SND_VOLMOD_EXPLOSION_FLASHBANG,	//SND_VOLMOD_EXPLOSION_FLASHBANG,
			IW6::SoundVolMod::SND_VOLMOD_EXPLOSION_ROCKET,	//SND_VOLMOD_EXPLOSION_ROCKET,
			IW6::SoundVolMod::SND_VOLMOD_EXPLOSION_CAR,	//SND_VOLMOD_EXPLOSION_CAR,
			IW6::SoundVolMod::SND_VOLMOD_IMPACT,	//SND_VOLMOD_IMPACT,
			IW6::SoundVolMod::SND_VOLMOD_IMPACT_PLR,	//SND_VOLMOD_IMPACT_PLR,
			IW6::SoundVolMod::SND_VOLMOD_IMPACT_NPC,	//SND_VOLMOD_IMPACT_NPC,
			IW6::SoundVolMod::SND_VOLMOD_IMPACTMP,	//SND_VOLMOD_IMPACTMP,
			IW6::SoundVolMod::SND_VOLMOD_IMPACTMP_PLR,	//SND_VOLMOD_IMPACTMP_PLR,
			IW6::SoundVolMod::SND_VOLMOD_IMPACTMP_NPC,	//SND_VOLMOD_IMPACTMP_NPC,
			IW6::SoundVolMod::SND_VOLMOD_WHIZBY,	//SND_VOLMOD_WHIZBY,
			IW6::SoundVolMod::SND_VOLMOD_WHIZBYMP,	//SND_VOLMOD_WHIZBYMP,
			IW6::SoundVolMod::SND_VOLMOD_VEHICLE_PLR,	//SND_VOLMOD_VEHICLE_PLR,
			IW6::SoundVolMod::SND_VOLMOD_VEHICLE_NPC,	//SND_VOLMOD_VEHICLE_NPC,
			IW6::SoundVolMod::SND_VOLMOD_VEHICLE_WPN_PLR,	//SND_VOLMOD_VEHICLE_WPN_PLR,
			IW6::SoundVolMod::SND_VOLMOD_VEHICLE_WPN_NPC,	//SND_VOLMOD_VEHICLE_WPN_NPC,
			IW6::SoundVolMod::SND_VOLMOD_VEHICLE,	//SND_VOLMOD_VEHICLE,
			IW6::SoundVolMod::SND_VOLMOD_GRENADEBOUNCE,	//SND_VOLMOD_GRENADEBOUNCE,
			IW6::SoundVolMod::SND_VOLMOD_GRENADEBOUNCEMP,	//SND_VOLMOD_GRENADEBOUNCEMP,
			IW6::SoundVolMod::SND_VOLMOD_SHELLCASINGS,	//SND_VOLMOD_SHELLCASINGS,
			IW6::SoundVolMod::SND_VOLMOD_SHELLCASINGSMP,	//SND_VOLMOD_SHELLCASINGSMP,
			IW6::SoundVolMod::SND_VOLMOD_WPN_PLR,	//SND_VOLMOD_WPN_PLR,
			IW6::SoundVolMod::SND_VOLMOD_WPNMP_PLR,	//SND_VOLMOD_WPNMP_PLR,
			IW6::SoundVolMod::SND_VOLMOD_WPN_NPC,	//SND_VOLMOD_WPN_NPC,
			IW6::SoundVolMod::SND_VOLMOD_WPNMP_NPC,	//SND_VOLMOD_WPNMP_NPC,
			IW6::SoundVolMod::SND_VOLMOD_WPN_PROJECTILE,	//SND_VOLMOD_WPN_PROJECTILE,
			IW6::SoundVolMod::SND_VOLMOD_WPNMP_PROJECTILE,	//SND_VOLMOD_WPNMP_PROJECTILE,
			IW6::SoundVolMod::SND_VOLMOD_NA,	//SND_VOLMOD_NA,
			IW6::SoundVolMod::SND_VOLMOD_MAX,	//SND_VOLMOD_MAX,
			IW6::SoundVolMod::SND_VOLMOD_SCRIPTED1,	//SND_VOLMOD_SCRIPTED1,
			IW6::SoundVolMod::SND_VOLMOD_SCRIPTED2,	//SND_VOLMOD_SCRIPTED2,
			IW6::SoundVolMod::SND_VOLMOD_SCRIPTED3,	//SND_VOLMOD_SCRIPTED3,
			IW6::SoundVolMod::SND_VOLMOD_SCRIPTED4,	//SND_VOLMOD_SCRIPTED4,
			IW6::SoundVolMod::SND_VOLMOD_SCRIPTED5,	//SND_VOLMOD_SCRIPTED5,
			IW6::SoundVolMod::SND_VOLMOD_FULLVOLUME,	//SND_VOLMOD_FULLVOLUME,
			IW6::SoundVolMod::SND_VOLMOD_PERKMP_QUIET,	//SND_VOLMOD_PERKMP_QUIET,
			IW6::SoundVolMod::SND_VOLMOD_DEFAULT,	//SND_VOLMOD_LEVEL_AC130,
			IW6::SoundVolMod::SND_VOLMOD_DEFAULT,	//SND_VOLMOD_DEFAULT,
		};

		void GenerateIW6SoundAlias(snd_alias_t* alias, IW6::snd_alias_t* iw6_alias, allocator& mem, const std::function<std::string(const char* filename)>& get_streamed_sound_data)
		{
			iw6_alias->aliasName = alias->aliasName;
			iw6_alias->subtitle = alias->subtitle;
			iw6_alias->secondaryAliasName = alias->secondaryAliasName;
			iw6_alias->chainAliasName = alias->chainAliasName;

			// todo
			iw6_alias->soundFile;
			iw6_alias->soundFile = mem.allocate<IW6::SoundFile>();

			iw6_alias->soundFile->exists = alias->soundFile->exists;
			iw6_alias->soundFile->type = static_cast<IW6::snd_alias_type_t>(alias->soundFile->type);

			switch (iw6_alias->soundFile->type)
			{
			case IW6::snd_alias_type_t::SAT_LOADED:
			{
				const std::string s_name = alias->soundFile->u.loadSnd->name;
				const std::string s_name_no_ext = remove_extension(s_name);

				iw6_alias->soundFile->u.loadSnd = mem.allocate<IW6::LoadedSound>();
				iw6_alias->soundFile->u.loadSnd->name = mem.duplicate_string(s_name_no_ext);
				break;
			}
			case IW6::snd_alias_type_t::SAT_STREAMED:
			{
#ifdef STREAMED_TO_LOADED_IW5
				iw6_alias->soundFile->type = IW6::snd_alias_type_t::SAT_LOADED;
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

				iw6_alias->soundFile->u.loadSnd = mem.allocate<IW6::LoadedSound>();
				iw6_alias->soundFile->u.loadSnd->name = mem.duplicate_string(noext_path);
#else
				iw6_alias->soundFile->u.streamSnd.filename.isLocalized = false;
				iw6_alias->soundFile->u.streamSnd.filename.isStreamed = false;
				iw6_alias->soundFile->u.streamSnd.filename.fileIndex = 0;

				iw6_alias->soundFile->u.streamSnd.filename.info.raw.name = alias->soundFile->sound.streamSnd.name;
				iw6_alias->soundFile->u.streamSnd.filename.info.raw.dir = alias->soundFile->sound.streamSnd.dir;

				iw6_alias->soundFile->u.streamSnd.totalMsec = 0;
#endif
				break;
			}
			}

			iw6_alias->mixerGroup = nullptr;

			iw6_alias->volMin = alias->volMin;
			iw6_alias->volMax = alias->volMax;
			iw6_alias->volModIndex = volmod_to_volmod_index[alias->volModIndex];

			iw6_alias->pitchMin = alias->pitchMin;
			iw6_alias->pitchMax = alias->pitchMax;

			iw6_alias->distMin = alias->distMin;
			iw6_alias->distMax = alias->distMax;

			iw6_alias->velocityMin = alias->velocityMin;

			iw6_alias->masterPriority = alias->masterPriority;
			iw6_alias->masterPercentage = alias->masterPercentage;

			iw6_alias->slavePercentage = alias->slavePercentage;

			iw6_alias->probability = alias->probability;

			iw6_alias->sequence = alias->sequence;
			iw6_alias->lfePercentage = alias->lfePercentage;
			iw6_alias->centerPercentage = alias->centerPercentage;
			iw6_alias->startDelay = alias->startDelay;

			auto* default_sndcurve = mem.allocate<IW6::SndCurve>();
			default_sndcurve->name = "$default";
			default_sndcurve->isDefault = 1;

			IW6::SndCurve* sndcurve = nullptr;
			if (alias->volumeFalloffCurve && alias->volumeFalloffCurve->filename != ""s)
			{
				sndcurve = mem.allocate<IW6::SndCurve>();
				sndcurve->filename = alias->volumeFalloffCurve->filename;
				sndcurve->isDefault = 0;
				sndcurve->knotCount = alias->volumeFalloffCurve->knotCount;
				memcpy(sndcurve->knots, alias->volumeFalloffCurve->knots, sizeof(float[16][2]));
			}
			else
			{
				sndcurve = default_sndcurve;
			}

			iw6_alias->volumeFalloffCurve = sndcurve;
			iw6_alias->lpfCurve = default_sndcurve;
			iw6_alias->reverbSendCurve = default_sndcurve;

			iw6_alias->speakerMap = nullptr;
			if (alias->speakerMap)
			{
				iw6_alias->speakerMap = mem.allocate<IW6::SpeakerMap>();
				iw6_alias->speakerMap->isDefault = alias->speakerMap->isDefault;
				iw6_alias->speakerMap->name = alias->speakerMap->name;

				for (char x = 0; x < 2; x++)
				{
					for (char y = 0; y < 2; y++)
					{
						iw6_alias->speakerMap->channelMaps[x][y].speakerCount = alias->speakerMap->channelMaps[x][y].speakerCount;
						for (char i = 0; i < 6; i++)
						{
							iw6_alias->speakerMap->channelMaps[x][y].speakers[i].speaker = alias->speakerMap->channelMaps[x][y].speakers[i].speaker;
							iw6_alias->speakerMap->channelMaps[x][y].speakers[i].numLevels = alias->speakerMap->channelMaps[x][y].speakers[i].numLevels;
							iw6_alias->speakerMap->channelMaps[x][y].speakers[i].levels[0] = alias->speakerMap->channelMaps[x][y].speakers[i].levels[0];
							iw6_alias->speakerMap->channelMaps[x][y].speakers[i].levels[1] = alias->speakerMap->channelMaps[x][y].speakers[i].levels[1];
						}
					}
				}
			}

			iw6_alias->wetMixOverride = 0.0f;

			iw6_alias->envelopMin = alias->envelopMin;
			iw6_alias->envelopMax = alias->envelopMax;

			iw6_alias->envelopPercentage = alias->envelopPercentage;

			iw6_alias->allowDoppler = 0;
			iw6_alias->dopplerPreset = nullptr;

			// convert flags
			IW6::SoundAliasFlags iw6_flags{ 0 };
			IW5::SoundAliasFlags iw5_flags{ 0 };

			iw6_flags.intValue = 0;
			iw5_flags.intValue = alias->flags.intValue;

			// IW6 struct may be wrong?
			iw6_flags.packed.looping = iw5_flags.looping;
			iw6_flags.packed.isMaster = iw5_flags.isMaster;
			iw6_flags.packed.isSlave = iw5_flags.isSlave;
			iw6_flags.packed.fullDryLevel = iw5_flags.fullDryLevel;
			iw6_flags.packed.noWetLevel = iw5_flags.noWetLevel;
			iw6_flags.packed.type = iw6_alias->soundFile->type; //iw5_flags.type;
			iw6_flags.packed.channel = channel_to_channel_index[iw5_flags.channel];
			iw6_flags.packed.randomLooping = iw5_flags.randomLooping;
			iw6_flags.packed.spatialize = iw5_flags.spatialize;

			iw6_alias->flags = iw6_flags.intValue;
		}

		IW6::snd_alias_list_t* GenerateIW6Sound(snd_alias_list_t* asset, allocator& mem, const std::function<std::string(const char* filename)>& get_streamed_sound_data)
		{
			auto* iw6_asset = mem.allocate<IW6::snd_alias_list_t>();
			iw6_asset->aliasName = asset->aliasName;
			
			iw6_asset->count = asset->count;
			iw6_asset->head = mem.allocate<IW6::snd_alias_t>(iw6_asset->count);
			for (unsigned char i = 0; i < iw6_asset->count; i++)
			{
				GenerateIW6SoundAlias(&asset->head[i], &iw6_asset->head[i], mem, get_streamed_sound_data);
			}

			return iw6_asset;
		}

		IW6::snd_alias_list_t* convert(snd_alias_list_t* asset, allocator& allocator, const std::function<std::string(const char* filename)>& get_streamed_sound_data)
		{
			// generate IW6 asset
			return GenerateIW6Sound(asset, allocator, get_streamed_sound_data);
		}
	}
}

#undef STREAMED_TO_LOADED_IW5