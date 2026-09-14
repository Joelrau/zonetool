#include "stdafx.hpp"
#include "IW4/Assets/GameWorldMp.hpp"
#include "IW4/Assets/FxWorld.hpp"

namespace ZoneTool
{
	namespace IW3
	{
		void IGameWorldMp::dump(GameWorldMp* asset)
		{
			allocator allocator;

			// lol, GameWorldMp contains no data in IW3
			auto* iw4_gameworld = allocator.allocate<IW4::GameWorldMp>();
			memset(iw4_gameworld, 0, sizeof(IW4::GameWorldMp));

			iw4_gameworld->g_glassData = allocator.allocate<IW4::G_GlassData>();
			memset(iw4_gameworld->g_glassData, 0, sizeof(IW4::G_GlassData));

			// The piece array stays empty. A dummy piece was tried here to stop IW7's
			// collision filter faulting on a null piece array, and it moved the crash
			// rather than fixing it: the client glass geometry array (stride 56) has no
			// record behind a synthesised piece, so the first shot that damaged it faulted
			// in the sound-alias lookup instead. A map with no glass carries no pieces;
			// CONTENTS_GLASS is stripped in havok_builder so nothing ever asks about one.
			iw4_gameworld->name = asset->name;

			IW4::IGameWorldMp::dump(iw4_gameworld);

			// dump fx_map here too
			auto* h1_fxworld = allocator.allocate<IW4::FxWorld>();
			memset(h1_fxworld, 0, sizeof(IW4::FxWorld));

			h1_fxworld->name = asset->name;
			IW4::IFxWorld::dump(h1_fxworld);

			// Both of those are assets IW3 does not have: they are synthesised here out of
			// game_map_mp, so the game never logs them and the csv never gets a line for them.
			// Without the lines the linker packs neither, and a zone with no GlassWorld crashes
			// the moment anything asks about glass: G_InitGlass resolves the asset with
			// createDefault set, the default's piece array is null, and the first query through
			// a script builtin dereferences it (0xC0000005 in sub_140B157B0). Stock IW7 zones
			// carry both rows -- mp_frontend has fx_map and glass_map next to its gfx_map.
			zonetool::filesystem::csv_buffer_line("glass_map", asset->name);
			zonetool::filesystem::csv_buffer_line("fx_map", asset->name);
		}
	}
}