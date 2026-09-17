#include "stdafx.hpp"

namespace ZoneTool::IW7
{
	void IScriptableDef::dump_scriptable_event(assetmanager::dumper& dump, ScriptableEventDef* data)
	{
		dump.dump_string(data->base.name);

		const auto dump_event_base = [&]()
		{
			dump.dump_single(data->data.anonymous.base);
			if (data->data.anonymous.base)
			{
				dump.dump_string(data->data.anonymous.base->name);
			}
		};

		const auto dump_part_reference = [&](ScriptablePartReference* data_)
		{
			dump.dump_string(data_->base.name);

			const auto dump_part_reference_base = [&]()
			{
				dump.dump_single(data_->u.__0.base);
				if (data_->u.__0.base)
				{
					dump.dump_string(data_->u.__0.base->name);
				}
			};

			switch (data_->type)
			{
			case 0:
			case 1:
			case 2:
				dump_part_reference_base();
				break;
			case 3:
				dump_part_reference_base();
				dump.dump_array(data_->u.__3.val, data_->u.__3.count);
				break;
			}
		};

		switch (data->type)
		{
		case Scriptable_EventType_StateChange:
			dump_event_base();
			dump_part_reference(&data->data.stateChange.partReference);
			break;
		case Scriptable_EventType_Wait:
			dump_event_base();
			break;
		case Scriptable_EventType_Random:
			dump_event_base();
			dump.dump_array(data->data.random.eventsA, data->data.random.eventACount);
			for (unsigned int i = 0; i < data->data.random.eventACount; i++)
			{
				dump_scriptable_event(dump, &data->data.random.eventsA[i]);
			}
			dump.dump_array(data->data.random.eventsB, data->data.random.eventBCount);
			for (unsigned int i = 0; i < data->data.random.eventBCount; i++)
			{
				dump_scriptable_event(dump, &data->data.random.eventsB[i]);
			}
			break;
		case Scriptable_EventType_Script:
			dump_event_base();
			dump.dump_string(data->data.script.notification);
			dump.dump_string(SL_ConvertToString(data->data.script.scrNotification));
			break;
		case Scriptable_EventType_Model:
			dump_event_base();
			dump.dump_asset(data->data.model.model);
			break;
		case Scriptable_EventType_Collision:
			dump_event_base();
			dump.dump_string(data->data.collision.collmapName);
			break;
		case Scriptable_EventType_Animation:
			dump_event_base();
			dump.dump_asset(data->data.animation.animation);
			dump.dump_array(data->data.animation.eventsAtEnd, data->data.animation.eventAtEndCount);
			for (unsigned int i = 0; i < data->data.animation.eventAtEndCount; i++)
			{
				dump_scriptable_event(dump, &data->data.animation.eventsAtEnd[i]);
			}
			break;
		case Scriptable_EventType_HideShowBone:
			dump_event_base();
			dump.dump_string(data->data.hideShowBone.tagName);
			dump.dump_string(SL_ConvertToString(data->data.hideShowBone.scrTagName));
			break;
		case Scriptable_EventType_NoteTrack:
			dump_event_base();
			dump.dump_array(data->data.noteTrack.noteTracks, data->data.noteTrack.noteTrackCount);
			for (unsigned int i = 0; i < data->data.noteTrack.noteTrackCount; i++)
			{
				dump.dump_string(data->data.noteTrack.noteTracks[i].noteTrackName);
				dump.dump_string(SL_ConvertToString(data->data.noteTrack.noteTracks[i].scrNoteTrackName));
				dump.dump_array(data->data.noteTrack.noteTracks[i].events, data->data.noteTrack.noteTracks[i].numEvents);
				for (unsigned int j = 0; j < data->data.noteTrack.noteTracks[i].numEvents; j++)
				{
					dump_scriptable_event(dump, &data->data.noteTrack.noteTracks[i].events[j]);
				}
			}
			break;
		case Scriptable_EventType_ChunkDynent:
			dump_event_base();
			dump_part_reference(&data->data.chunkDynent.partReference);
			break;
		case Scriptable_EventType_SpawnDynent:
			dump_event_base();
			dump.dump_asset(data->data.spawnDynent.model);
			dump.dump_string(data->data.spawnDynent.tagName);
			dump.dump_string(SL_ConvertToString(data->data.spawnDynent.scrTagName));
			break;
		case Scriptable_EventType_PFX:
			dump_event_base();
			dump.dump_array(data->data.particleFX.scrTagNames, data->data.particleFX.scrTagCount);
			for (unsigned int i = 0; i < data->data.particleFX.scrTagCount; i++)
			{
				dump.dump_string(SL_ConvertToString(data->data.particleFX.scrTagNames[i]));
			}
			dump.dump_array(data->data.particleFX.scrEndTagNames, data->data.particleFX.scrEndTagCount);
			for (unsigned int i = 0; i < data->data.particleFX.scrEndTagCount; i++)
			{
				dump.dump_string(SL_ConvertToString(data->data.particleFX.scrEndTagNames[i]));
			}
			dump.dump_string(data->data.particleFX.effectAlias);
			dump.dump_asset(data->data.particleFX.effectDef.u.vfx);
			break;
		case Scriptable_EventType_Sound:
			dump_event_base();
			dump.dump_string(data->data.sound.tagName);
			dump.dump_string(SL_ConvertToString(data->data.sound.scrTagName));
			dump.dump_string(data->data.sound.soundAlias);
			dump.dump_string(data->data.sound.soundAliasCache);
			break;
		case Scriptable_EventType_Explosion:
			dump_event_base();
			dump.dump_string(data->data.explosion.tagName);
			dump.dump_string(SL_ConvertToString(data->data.explosion.scrTagName));
			break;
		case Scriptable_EventType_Light:
			dump_event_base();
			dump.dump_string(data->data.light.name);
			dump.dump_string(SL_ConvertToString(data->data.light.scrName));
			break;
		case Scriptable_EventType_Sun:
			dump_event_base();
			break;
		case Scriptable_EventType_Rumble:
			dump_event_base();
			dump.dump_string(data->data.rumble.tagName);
			dump.dump_string(SL_ConvertToString(data->data.rumble.scrTagName));
			dump.dump_string(data->data.rumble.rumble);
			dump.dump_asset(data->data.rumble.rumbleAsset);
			break;
		case Scriptable_EventType_Screenshake:
			dump_event_base();
			dump.dump_string(data->data.screenshake.tagName);
			dump.dump_string(SL_ConvertToString(data->data.screenshake.scrTagName));
			break;
		case Scriptable_EventType_PartDamage:
			dump_event_base();
			dump_part_reference(&data->data.partDamage.partReference);
			break;
		case Scriptable_EventType_SetMayhem:
			dump_event_base();
			dump.dump_asset(data->data.setMayhem.mayhem);
			break;
		case Scriptable_EventType_PlayMayhem:
			dump_event_base();
			break;
		case Scriptable_EventType_ViewmodelShaderParam:
			dump_event_base();
			break;
		case Scriptable_EventType_ViewmodelChangeImage:
			dump_event_base();
			break;
		case Scriptable_EventType_ClientViewSelector:
			dump_event_base();
			dump.dump_array(data->data.clientViewSelector.events1p, data->data.clientViewSelector.event1pCount);
			for (unsigned int i = 0; i < data->data.clientViewSelector.event1pCount; i++)
			{
				dump_scriptable_event(dump, &data->data.clientViewSelector.events1p[i]);
			}
			dump.dump_array(data->data.clientViewSelector.events3p, data->data.clientViewSelector.event3pCount);
			for (unsigned int i = 0; i < data->data.clientViewSelector.event3pCount; i++)
			{
				dump_scriptable_event(dump, &data->data.clientViewSelector.events3p[i]);
			}
			break;
		case Scriptable_EventType_TeamSelector:
			dump_event_base();
			dump.dump_array(data->data.teamSelector.eventsPass, data->data.teamSelector.eventPassCount);
			for (unsigned int i = 0; i < data->data.teamSelector.eventPassCount; i++)
			{
				dump_scriptable_event(dump, &data->data.teamSelector.eventsPass[i]);
			}
			dump.dump_array(data->data.teamSelector.eventsFail, data->data.teamSelector.eventFailCount);
			for (unsigned int i = 0; i < data->data.teamSelector.eventFailCount; i++)
			{
				dump_scriptable_event(dump, &data->data.teamSelector.eventsFail[i]);
			}
			break;
		case Scriptable_EventType_AddModel:
			dump_event_base();
			dump.dump_string(data->data.addModel.tagName);
			dump.dump_string(SL_ConvertToString(data->data.addModel.scrTagName));
			dump.dump_asset(data->data.addModel.model);
			break;
		case Scriptable_EventType_ApplyForce:
			dump_event_base();
			break;
		case Scriptable_EventType_CompassIcon:
			dump_event_base();
			dump.dump_asset(data->data.compassIcon.friendlyArrow);
			dump.dump_asset(data->data.compassIcon.friendlyFiring);
			dump.dump_asset(data->data.compassIcon.friendlyChatting);
			dump.dump_asset(data->data.compassIcon.friendlyYelling);
			dump.dump_asset(data->data.compassIcon.partyArrow);
			dump.dump_asset(data->data.compassIcon.partyFiring);
			dump.dump_asset(data->data.compassIcon.partyChatting);
			dump.dump_asset(data->data.compassIcon.partyYelling);
			dump.dump_asset(data->data.compassIcon.squadArrow);
			dump.dump_asset(data->data.compassIcon.squadFiring);
			for (auto i = 0; i < 3; i++)
			{
				dump.dump_asset(data->data.compassIcon.enemyCompassIconQuiet[i]);
			}
			for (auto i = 0; i < 3; i++)
			{
				dump.dump_asset(data->data.compassIcon.enemyCompassIconFiring[i]);
			}
			dump.dump_asset(data->data.compassIcon.enemyCompassIconDirectional);
			break;
		case Scriptable_EventType_MaterialOverride:
			dump_event_base();
			dump.dump_asset(data->data.materialOverride.material);
			break;
		}
	}

	void IScriptableDef::dump_state_base(assetmanager::dumper& dump, ScriptableStateBaseDef* data)
	{
		dump.dump_string(data->name);
		dump.dump_array(data->events, data->numEvents);
		for (unsigned int i = 0; i < data->numEvents; i++)
		{
			dump_scriptable_event(dump, &data->events[i]);
		}
	}

	void IScriptableDef::dump_state(assetmanager::dumper& dump, ScriptableStateDef* data)
	{
		dump_state_base(dump, &data->base);
		if (data->type == Scriptable_StateType_Simple)
		{
			dump.dump_single(data->data.simple.base);
			if (data->data.simple.base)
			{
				dump_state_base(dump, data->data.simple.base);
			}
		}
		else if (data->type == Scriptable_StateType_Health)
		{
			dump.dump_single(data->data.health.base);
			if (data->data.health.base)
			{
				dump_state_base(dump, data->data.health.base);
			}
			dump.dump_string(data->data.health.script_id);
			dump.dump_string(SL_ConvertToString(data->data.health.scrScript_id));
		}
		else if (data->type == Scriptable_StateType_Scripted)
		{
			dump.dump_single(data->data.scripted.base);
			if (data->data.scripted.base)
			{
				dump_state_base(dump, data->data.scripted.base);
			}
			dump.dump_string(data->data.scripted.script_id);
			dump.dump_string(SL_ConvertToString(data->data.scripted.scrScript_id));
		}
	}

	void IScriptableDef::dump_part(assetmanager::dumper& dump, ScriptablePartDef* data)
	{
		dump.dump_string(data->name);
		dump.dump_string(SL_ConvertToString(data->scrName));
		dump.dump_string(data->tagName);
		dump.dump_string(SL_ConvertToString(data->scrTagName));

		dump.dump_array(data->states, data->numStates);
		for (unsigned int i = 0; i < data->numStates; i++)
		{
			dump_state(dump, &data->states[i]);
		}

		dump.dump_array(data->childParts, data->numChildParts);
		for (unsigned int i = 0; i < data->numChildParts; i++)
		{
			dump_part(dump, &data->childParts[i]);
		}

		dump.dump_array(data->damageTagOverrides, data->numDamageTagOverrides);
		for (unsigned int i = 0; i < data->numDamageTagOverrides; i++)
		{
			dump.dump_string(data->damageTagOverrides[i].tag);
			dump.dump_string(SL_ConvertToString(data->damageTagOverrides[i].scrTag));
		}
	}

	void IScriptableDef::dump(ScriptableDef* asset)
	{
		const auto path = "scriptable\\"s + asset->name;

		assetmanager::dumper dump;
		if (!dump.open(path))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);

		dump.dump_asset(asset->nextScriptableDef);

		dump.dump_array(asset->parts, asset->numParts);
		for (unsigned int i = 0; i < asset->numParts; i++)
		{
			dump_part(dump, &asset->parts[i]);
		}

		dump.dump_string(SL_ConvertToString(asset->animationTreeName));

		dump.dump_array(asset->models, asset->numXModels);
		for (unsigned int i = 0; i < asset->numXModels; i++)
		{
			dump.dump_asset(asset->models[i]);
		}

		dump.close();
	}
}