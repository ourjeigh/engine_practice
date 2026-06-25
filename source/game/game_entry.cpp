#include "game_api.h"
#include "engine_api.h"
#include "game/enhanced_game.h"
#include "game/demo_game.h"

#define CONFIG_DEMO_GAME

#ifdef CONFIG_DEMO_GAME
static_global c_demo_game g_game;
#else
static_global c_enhanced_game g_game;
#endif //CONFIG_DEMO_GAME
extern "C"
{
	GAME_API void game_init(s_game_engine_context& engine_context)
	{
		g_engine_ptr = engine_context.engine;
		g_assert_handler = engine_context.assert_hook;

		g_game.init(engine_context.memory);


		engine_log_verbose("game: game initialized");
	}

	GAME_API void game_update(const s_input_state const_ptr input_state, real32 dt)
	{
		g_game.update(input_state, dt);
	}

#ifdef HOT_RELOAD
	GAME_API void game_reload(s_game_engine_context& engine_context)
	{
		g_engine_ptr = engine_context.engine;
		g_assert_handler = engine_context.assert_hook;
		
		g_game.reload(engine_context.memory);
	
		engine_log_verbose("game: game reloaded");
	}
#endif //HOT_RELOAD
}