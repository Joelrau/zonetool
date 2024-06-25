#include "stdafx.hpp"
#include "IW5/Assets/Sound.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		IW5::SoundChannel iw5_conversion_table[IW4::SND_CHANNEL_COUNT]
		{
			IW5::SND_CHANNEL_PHYSICS,
			IW5::SND_CHANNEL_AMBDIST1,
			IW5::SND_CHANNEL_AMBDIST2,
			IW5::SND_CHANNEL_AUTO,
			IW5::SND_CHANNEL_AUTO2,
			IW5::SND_CHANNEL_AUTODOG,
			IW5::SND_CHANNEL_BULLETIMPACT,
			IW5::SND_CHANNEL_BULLETWHIZBY,
			IW5::SND_CHANNEL_EXPLOSIVEIMPACT,
			IW5::SND_CHANNEL_ELEMENT,
			IW5::SND_CHANNEL_AUTO2D,
			IW5::SND_CHANNEL_VEHICLE,
			IW5::SND_CHANNEL_VEHICLELIMITED,
			IW5::SND_CHANNEL_MENU,
			IW5::SND_CHANNEL_BODY,
			IW5::SND_CHANNEL_BODY2D,
			IW5::SND_CHANNEL_RELOAD,
			IW5::SND_CHANNEL_RELOAD2D,
			IW5::SND_CHANNEL_ITEM,
			IW5::SND_CHANNEL_EFFECTS1,
			IW5::SND_CHANNEL_EFFECTS2,
			IW5::SND_CHANNEL_WEAPON,
			IW5::SND_CHANNEL_WEAPON2D,
			IW5::SND_CHANNEL_NONSHOCK,
			IW5::SND_CHANNEL_VOICE,
			IW5::SND_CHANNEL_LOCAL,
			IW5::SND_CHANNEL_LOCAL2,
			IW5::SND_CHANNEL_LOCAL3,
			IW5::SND_CHANNEL_AMBIENT,
			IW5::SND_CHANNEL_HURT,
			IW5::SND_CHANNEL_PLAYER1,
			IW5::SND_CHANNEL_PLAYER2,
			IW5::SND_CHANNEL_MUSIC,
			IW5::SND_CHANNEL_MUSICNOPAUSE,
			IW5::SND_CHANNEL_MISSION,
			IW5::SND_CHANNEL_ANNOUNCER,
			IW5::SND_CHANNEL_SHELLSHOCK,
		};

		IW5::SoundChannel channel_to_iw5(IW4::SoundChannel snd_channel)
		{
			return iw5_conversion_table[snd_channel];
		}

		IW5::snd_alias_list_t* generate_sound(snd_alias_list_t* asset, allocator& mem)
		{
			auto* iw5_asset = mem.allocate<IW5::snd_alias_list_t>();
			memset(iw5_asset, 0, sizeof IW5::snd_alias_list_t);

			iw5_asset->aliasName = asset->name;
			iw5_asset->count = asset->count;

			iw5_asset->head = mem.allocate<IW5::snd_alias_t>(iw5_asset->count);
			memset(iw5_asset->head, 0, sizeof(IW5::snd_alias_t) * iw5_asset->count);

			for (auto i = 0; i < iw5_asset->count; i++)
			{
				auto* current_iw4 = &asset->head[i];
				auto* current_iw5 = &iw5_asset->head[i];

				memcpy(current_iw5, &asset->head[i], 36);
				current_iw5->volModIndex = IW5::SoundVolMod::SND_VOLMOD_DEFAULT;
				memcpy(&current_iw5->pitchMin, &current_iw4->pitchMin, 20);

				IW4::SoundAliasFlags iw4_flags;
				iw4_flags.intValue = current_iw4->flags;

				IW5::SoundAliasFlags iw5_flags;
				iw5_flags.intValue = 0;

				iw5_flags.looping = iw4_flags.looping;
				iw5_flags.isMaster = iw4_flags.isMaster;
				iw5_flags.isSlave = iw4_flags.isSlave;
				iw5_flags.fullDryLevel = iw4_flags.fullDryLevel;
				iw5_flags.noWetLevel = iw4_flags.noWetLevel;
				iw5_flags.unknown = iw4_flags.unknown1;
				iw5_flags.unk_is3D = iw4_flags.unknown2;
				iw5_flags.type = iw4_flags.type;
				iw5_flags.channel = channel_to_iw5(IW4::SoundChannel(iw4_flags.channel));

				current_iw5->flags.intValue = iw5_flags.intValue;

				current_iw5->masterPriority = 50;
				current_iw5->masterPercentage = current_iw4->___u15.masterPercentage;
				current_iw5->slavePercentage = current_iw4->___u15.slavePercentage;
				memcpy(&current_iw5->probability, &current_iw4->probability, 36);
			}

			return iw5_asset;
		}

		void ISound::dump(snd_alias_list_t* asset, const std::function<std::string(const char* filename)>& get_streamed_sound_data)
		{
			allocator allocator;
			auto* iw5_asset = generate_sound(asset, allocator);

			IW5::ISound::dump(iw5_asset, get_streamed_sound_data);
		}
	}
}