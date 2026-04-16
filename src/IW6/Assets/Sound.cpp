#include "stdafx.hpp"

namespace ZoneTool::IW6
{
	std::array<const char*, SND_VOLMOD_COUNT> volume_mod_groups =
	{
		// OLD AND TO BE DELETED
		"foley",
		// searched ready to remove
		"wpnai",
		"wpnplyr",
		// User interface
		"hud",
		"interface",
		"interface_music",
		// Music
		"music",
		"music_emitter",
		// Ambience
		"ambience",
		"ambience_dist",
		"element",
		"emitter",
		"physics",
		// Character sounds
		"bodyfall",
		"foley_plr",
		"foleymp_plr",
		"foley_npc",
		"foleymp_npc",
		"foley_wpn_plr",
		"foley_wpn_npc",
		"footstep_plr",
		"footstep_npc",
		"footstepmp_plr",
		"footstepmp_npc",
		"melee_plr",
		"melee_npc",
		// Dialogue
		"chatteral",
		"chatterax",
		"reactional",
		"reactionax",
		"voiceover",
		"voiceover_radio",
		"voiceover_critical",
		"voiceover_amb",
		// Explosions & Destruction
		"destruct",
		"explosion",
		"explosion_grenade",
		"explosion_flashbang",
		"explosion_rocket",
		"explosion_car",
		"rex_emitters",
		// Bullet Impacts & Whizbys
		"impact",
		"impact_plr",
		"impact_npc",
		"impactmp",
		"impactmp_plr",
		"impactmp_npc",
		"whizby",
		"whizbymp",
		// Vehicle sounds
		"vehicle_plr",
		"vehicle_npc",
		"vehicle_wpn_plr",
		"vehicle_wpn_npc",
		"vehicle",
		// Weapons
		"grenadebounce",
		"grenadebouncemp",
		"shellcasings",
		"shellcasingsmp",
		"wpn_plr",
		"wpnmp_plr",
		"wpn_npc",
		"wpnmp_npc",
		"wpn_projectile",
		"wpnmp_projectile",
		// Special use
		"na",
		"max",
		"scripted1",
		"scripted2",
		"scripted3",
		"scripted4",
		"scripted5",
		"fullvolume",
		// Level specific and MP perks
		"perkmp_quiet",
		"deathsdoor",
		// Default if left blank
		"default",
	};

	std::array<const char*, SND_CHANNEL_COUNT> channels =
	{
		"physics",
		"ambdist1",
		"ambdist2",
		"alarm",
		"auto",
		"auto2",
		"auto2d",
		"autodog",
		"explosiondist1",
		"explosiondist2",
		"explosiveimpact",
		"element",
		"element_int",
		"element_ext",
		"foley_plr_mvmt",
		"foley_plr_weap",
		"foley_npc_mvmt",
		"foley_npc_weap",
		"foley_dog_mvmt",
		"element_lim",
		"element2d",
		"voice_dog_dist",
		"bulletflesh1npc_npc",
		"bulletflesh2npc_npc",
		"bulletimpact",
		"bulletflesh1npc",
		"bulletflesh2npc",
		"bulletflesh1",
		"bulletflesh2",
		"vehicle",
		"vehiclelimited",
		"menu",
		"menulim1",
		"menulim2",
		"menulim2",
		"bulletwhizbyout",
		"body",
		"body2d",
		"reload",
		"reload2d",
		"foley_plr_step",
		"foley_plr_step_unres",
		"foley_npc_step",
		"foley_dog_step",
		"item",
		"weapon_drone",
		"explosion1",
		"explosion2",
		"explosion3",
		"explosion4",
		"explosion5",
		"effects1",
		"effects2",
		"effects3",
		"effects2d1",
		"effects2d2",
		"norestrict",
		"norestrict2d",
		"aircraft",
		"vehicle2d",
		"weapon_dist",
		"weapon_mid",
		"weapon",
		"weapon2d",
		"nonshock",
		"nonshock2",
		"effects2dlim",
		"voice_dog",
		"music_emitter",
		"voice_dog_attack",
		"voice",
		"local",
		"local2",
		"local3",
		"ambient",
		"plr_weap_fire_2d",
		"plr_weap_mech_2d",
		"hurt",
		"player1",
		"player2",
		"music",
		"musicnopause",
		"mission",
		"missionfx",
		"announcer",
		"shellshock",
	};

	const char* get_vol_mod_name(int index)
	{
		return volume_mod_groups[index];
	}

	const char* get_channel_name(int index)
	{
		return channels[index];
	}

	int get_vol_mod_index_from_name(const char* name)
	{
		for (int i = 0; i < volume_mod_groups.size(); i++)
		{
			const char* vol_mod = volume_mod_groups[i];
			if (!_stricmp(vol_mod, name))
			{
				return i;
			}
		}
		return -1;
	}

	int get_channel_index_from_name(const char* name)
	{
		for (int i = 0; i < channels.size(); i++)
		{
			const char* channel = channels[i];
			if (!_stricmp(channel, name))
			{
				return i;
			}
		}
		return -1;
	}

#define SOUND_DUMP_SUBASSET(entry) \
	if (asset->entry) sound[#entry] = asset->entry->name;	\
	else sound[#entry] = nullptr;
#define SOUND_DUMP_STRING(entry) \
	if (asset->entry) sound[#entry] = std::string(asset->entry); \
	else sound[#entry] = nullptr;
#define SOUND_DUMP_CHAR(entry) \
	sound[#entry] = (char)asset->entry;
#define SOUND_DUMP_SHORT(entry) \
	sound[#entry] = (short)asset->entry;
#define SOUND_DUMP_INT(entry) \
	sound[#entry] = (int)asset->entry;
#define SOUND_DUMP_FLOAT(entry) \
	sound[#entry] = (float)asset->entry;

	void ISound::json_dump_snd_alias(ordered_json& sound, snd_alias_t* asset)
	{
		SOUND_DUMP_STRING(aliasName);
		SOUND_DUMP_STRING(secondaryAliasName);
		SOUND_DUMP_STRING(chainAliasName);
		SOUND_DUMP_STRING(subtitle);
		SOUND_DUMP_STRING(mixerGroup);

		// soundfile shit
		if (asset->soundFile)
		{
			sound["soundfile"]["type"] = asset->soundFile->type;
			sound["soundfile"]["exists"] = asset->soundFile->exists;

			auto insert_loaded = [&]()
			{
				sound["soundfile"]["name"] = asset->soundFile->u.loadSnd->name ? asset->soundFile->u.loadSnd->name : "";
			};

			auto insert_streamed = [&]()
			{
				sound["soundfile"]["totalMsec"] = asset->soundFile->u.streamSnd.totalMsec;
				sound["soundfile"]["isLocalized"] = asset->soundFile->u.streamSnd.filename.isLocalized;
				sound["soundfile"]["fileIndex"] = asset->soundFile->u.streamSnd.filename.fileIndex;

				sound["soundfile"]["packed"]["offset"] = 0;
				sound["soundfile"]["packed"]["length"] = 0;
				sound["soundfile"]["raw"]["dir"] = "";
				sound["soundfile"]["raw"]["name"] = "";

				if (asset->soundFile->u.streamSnd.filename.fileIndex)
				{
					sound["soundfile"]["packed"]["offset"] = asset->soundFile->u.streamSnd.filename.info.packed.offset;
					sound["soundfile"]["packed"]["length"] = asset->soundFile->u.streamSnd.filename.info.packed.length;
				}
				else
				{
					sound["soundfile"]["raw"]["dir"] = asset->soundFile->u.streamSnd.filename.info.raw.dir
						? asset->soundFile->u.streamSnd.filename.info.raw.dir
						: "";
					sound["soundfile"]["raw"]["name"] = asset->soundFile->u.streamSnd.filename.info.raw.name
						? asset->soundFile->u.streamSnd.filename.info.raw.name
						: "";
				}
			};

			auto insert_primed = [&]()
			{
				sound["soundfile"]["isLocalized"] = asset->soundFile->u.primedSnd.filename.isLocalized;
				sound["soundfile"]["fileIndex"] = asset->soundFile->u.primedSnd.filename.fileIndex;

				sound["soundfile"]["packed"]["offset"] = 0;
				sound["soundfile"]["packed"]["length"] = 0;
				sound["soundfile"]["raw"]["dir"] = "";
				sound["soundfile"]["raw"]["name"] = "";

				if (asset->soundFile->u.primedSnd.filename.fileIndex)
				{
					sound["soundfile"]["packed"]["offset"] = asset->soundFile->u.primedSnd.filename.info.packed.offset;
					sound["soundfile"]["packed"]["length"] = asset->soundFile->u.primedSnd.filename.info.packed.length;
				}
				else
				{
					sound["soundfile"]["raw"]["dir"] = asset->soundFile->u.primedSnd.filename.info.raw.dir
						? asset->soundFile->u.primedSnd.filename.info.raw.dir
						: "";
					sound["soundfile"]["raw"]["name"] = asset->soundFile->u.primedSnd.filename.info.raw.name
						? asset->soundFile->u.primedSnd.filename.info.raw.name
						: "";
				}

				if (asset->soundFile->u.primedSnd.loadedPart)
				{
					sound["soundfile"]["name"] = asset->soundFile->u.primedSnd.loadedPart->name ? asset->soundFile->u.primedSnd.loadedPart->name : "";
				}

				sound["soundfile"]["dataOffset"] = asset->soundFile->u.primedSnd.dataOffset;
				sound["soundfile"]["totalSize"] = asset->soundFile->u.primedSnd.totalSize;
				sound["soundfile"]["primedCrc"] = asset->soundFile->u.primedSnd.primedCrc;
			};

			if (asset->soundFile->type == SAT_LOADED)
			{
				insert_loaded();
			}
			else if (asset->soundFile->type == SAT_STREAMED)
			{
				insert_streamed();
			}
			else if (asset->soundFile->type == SAT_PRIMED)
			{
				insert_primed();
			}
		}

		SoundAliasFlags flags{};
		flags.intValue = asset->flags;

		sound["flags"] = {};
		sound["flags"]["looping"] = static_cast<int>(flags.packed.looping);
		sound["flags"]["isMaster"] = static_cast<int>(flags.packed.isMaster);
		sound["flags"]["isSlave"] = static_cast<int>(flags.packed.isSlave);
		sound["flags"]["fullDryLevel"] = static_cast<int>(flags.packed.fullDryLevel);
		sound["flags"]["noWetLevel"] = static_cast<int>(flags.packed.noWetLevel);
		sound["flags"]["randomLooping"] = static_cast<int>(flags.packed.randomLooping);
		sound["flags"]["spatialize"] = static_cast<int>(flags.packed.spatialize);
		sound["flags"]["type"] = static_cast<int>(flags.packed.type);
		sound["flags"]["channel"] = get_channel_name(flags.packed.channel);

		sound["volMod"] = get_vol_mod_name(asset->volModIndex); //SOUND_DUMP_SHORT(volModIndex);
		SOUND_DUMP_FLOAT(volMin);
		SOUND_DUMP_FLOAT(volMax);
		SOUND_DUMP_FLOAT(pitchMin);
		SOUND_DUMP_FLOAT(pitchMax);
		SOUND_DUMP_FLOAT(distMin);
		SOUND_DUMP_FLOAT(distMax);
		SOUND_DUMP_FLOAT(velocityMin);
		SOUND_DUMP_FLOAT(probability);
		SOUND_DUMP_INT(sequence);
		SOUND_DUMP_INT(startDelay);

		SOUND_DUMP_CHAR(masterPriority);
		SOUND_DUMP_FLOAT(masterPercentage);
		SOUND_DUMP_FLOAT(slavePercentage);

		SOUND_DUMP_FLOAT(lfePercentage);
		SOUND_DUMP_FLOAT(centerPercentage);

		SOUND_DUMP_FLOAT(envelopMin);
		SOUND_DUMP_FLOAT(envelopMax);
		SOUND_DUMP_FLOAT(envelopPercentage);

		SOUND_DUMP_FLOAT(wetMixOverride);

		SOUND_DUMP_SUBASSET(volumeFalloffCurve);
		SOUND_DUMP_SUBASSET(lpfCurve);
		SOUND_DUMP_SUBASSET(reverbSendCurve);

		sound["speakerMap"] = nullptr;
		if (asset->speakerMap)
		{
			json speakerMap;
			speakerMap["name"] = asset->speakerMap->name;
			speakerMap["isDefault"] = asset->speakerMap->isDefault;

			json channelMaps;
			for (char x = 0; x < 2; x++)
			{
				for (char y = 0; y < 2; y++)
				{
					json channelMap;
					channelMap["speakerCount"] = asset->speakerMap->channelMaps[x][y].speakerCount;

					json speakers;
					for (int speaker = 0; speaker < asset->speakerMap->channelMaps[x][y].speakerCount; speaker++)
					{
						json jspeaker;

						jspeaker["speaker"] = asset->speakerMap->channelMaps[x][y].speakers[speaker].speaker;
						jspeaker["numLevels"] = asset->speakerMap->channelMaps[x][y].speakers[speaker].numLevels;
						jspeaker["levels0"] = asset->speakerMap->channelMaps[x][y].speakers[speaker].levels[0];
						jspeaker["levels1"] = asset->speakerMap->channelMaps[x][y].speakers[speaker].levels[1];

						speakers[speaker] = jspeaker;
					}

					channelMap["speakers"] = speakers;

					channelMaps[(x & 0x01) << 1 | y & 0x01] = channelMap;
				}
			}

			speakerMap["channelMaps"] = channelMaps;

			sound["speakerMap"] = speakerMap;
		}

		SOUND_DUMP_CHAR(allowDoppler);
		SOUND_DUMP_SUBASSET(dopplerPreset);
	}

	void ISound::json_dump(snd_alias_list_t* asset)
	{
		const auto path = "sounds\\"s + asset->name + ".json"s;

		ordered_json sound;
		ordered_json aliases;
		ordered_json unknownArray;

		SOUND_DUMP_STRING(aliasName);
		SOUND_DUMP_CHAR(count);

		for (unsigned char i = 0; i < asset->count; i++)
		{
			ordered_json alias;
			json_dump_snd_alias(alias, &asset->head[i]);
			sound["head"][i] = alias;
		}

		std::string assetstr = sound.dump(4);

		auto file = filesystem::file(path);
		file.open();
		file.write(assetstr.data(), assetstr.size(), 1);
		file.close();
	}

	void ISound::dump(snd_alias_list_t* asset)
	{
		json_dump(asset);
	}
}