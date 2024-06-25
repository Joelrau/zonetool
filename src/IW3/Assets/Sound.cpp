#include "stdafx.hpp"
#include "IW4/Assets/Sound.hpp"

namespace ZoneTool
{
	namespace IW3
	{
		IW4::SoundChannel iw4_conversion_table[IW3::SND_CHANNEL_COUNT]
		{
			IW4::SND_CHANNEL_PHYSICS,
			IW4::SND_CHANNEL_AUTO,
			IW4::SND_CHANNEL_AUTO2,
			IW4::SND_CHANNEL_AUTODOG,
			IW4::SND_CHANNEL_BULLETIMPACT,
			IW4::SND_CHANNEL_BULLETWHIZBY,
			IW4::SND_CHANNEL_ELEMENT,
			IW4::SND_CHANNEL_AUTO2D,
			IW4::SND_CHANNEL_VEHICLE,
			IW4::SND_CHANNEL_VEHICLELIMITED,
			IW4::SND_CHANNEL_MENU,
			IW4::SND_CHANNEL_BODY,
			IW4::SND_CHANNEL_BODY2D,
			IW4::SND_CHANNEL_RELOAD,
			IW4::SND_CHANNEL_RELOAD2D,
			IW4::SND_CHANNEL_ITEM,
			IW4::SND_CHANNEL_EFFECTS1,
			IW4::SND_CHANNEL_EFFECTS2,
			IW4::SND_CHANNEL_WEAPON,
			IW4::SND_CHANNEL_WEAPON2D,
			IW4::SND_CHANNEL_NONSHOCK,
			IW4::SND_CHANNEL_VOICE,
			IW4::SND_CHANNEL_LOCAL,
			IW4::SND_CHANNEL_LOCAL2,
			IW4::SND_CHANNEL_AMBIENT,
			IW4::SND_CHANNEL_HURT,
			IW4::SND_CHANNEL_PLAYER1,
			IW4::SND_CHANNEL_PLAYER2,
			IW4::SND_CHANNEL_MUSIC,
			IW4::SND_CHANNEL_MUSICNOPAUSE,
			IW4::SND_CHANNEL_MISSION,
			IW4::SND_CHANNEL_ANNOUNCER,
			IW4::SND_CHANNEL_SHELLSHOCK,
		};

		static IW4::SoundChannel channel_to_iw4(IW3::SoundChannel snd_channel)
		{
			return iw4_conversion_table[snd_channel];
		}

		IW4::snd_alias_list_t* generate_sound(snd_alias_list_t* asset, allocator& mem)
		{
			const auto iw4_asset = mem.allocate<IW4::snd_alias_list_t>(); // new IW4::snd_alias_list_t;
			memcpy(iw4_asset, asset, sizeof snd_alias_list_t);

			iw4_asset->head = mem.allocate<IW4::snd_alias_t>(iw4_asset->count);
			memset(iw4_asset->head, 0, sizeof IW4::snd_alias_t * iw4_asset->count);

			for (auto i = 0; i < asset->count; i++)
			{
				memcpy(&iw4_asset->head[i], &asset->head[i], 16);
				memcpy(&iw4_asset->head[i].soundFile, &asset->head[i].soundFile, sizeof snd_alias_t - 16 - 20);
				memcpy(&iw4_asset->head[i].volumeFalloffCurve, &asset->head[i].volumeFalloffCurve, 20);

				IW3::SoundAliasFlags iw3_flags;
				iw3_flags.intValue = asset->head[i].flags;

				IW4::SoundAliasFlags iw4_flags;
				iw4_flags.intValue = 0;

				iw4_flags.looping = iw3_flags.looping;
				iw4_flags.isMaster = iw3_flags.isMaster;
				iw4_flags.isSlave = iw3_flags.isSlave;
				iw4_flags.fullDryLevel = iw3_flags.fullDryLevel;
				iw4_flags.noWetLevel = iw3_flags.noWetLevel;
				iw4_flags.unknown1 = iw3_flags.unknown1;
				iw4_flags.unknown2 = 0;
				iw4_flags.type = iw3_flags.type;
				iw4_flags.channel = channel_to_iw4(IW3::SoundChannel(iw3_flags.channel));

				iw4_asset->head[i].flags = iw4_flags.intValue;
			}

			return iw4_asset;
		}

		void ISound::dump(snd_alias_list_t* asset)
		{
			allocator allocator;
			auto* iw4_asset = generate_sound(asset, allocator);

			IW4::ISound::dump(iw4_asset, filesystem_read_big_file);
		}
	}
}