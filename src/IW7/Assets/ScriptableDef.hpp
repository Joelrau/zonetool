#pragma once

namespace ZoneTool::IW7
{
	class IScriptableDef
	{
	private:
		static void dump_scriptable_event(assetmanager::dumper& dump, ScriptableEventDef* data);
		static void dump_state_base(assetmanager::dumper& dump, ScriptableStateBaseDef* data);
		static void dump_state(assetmanager::dumper& dump, ScriptableStateDef* data);
		static void dump_part(assetmanager::dumper& dump, ScriptablePartDef* data);
	public:
		static void dump(ScriptableDef* asset);
	};
}