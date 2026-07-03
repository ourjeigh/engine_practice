#include "enhanced_game.h"
#include "debug/debug_helpers.h"
#include "engine_api.h"
#include "input/input_utilities.h"
#include "player.h"
#include "types/asset_types.h" 
#include "state/state_machine.h"
#include "enhanced_game/enhanced_game_state_machine.h"

struct s_game_state
{
	
	s_game_flow_state_machine_data game_flow_state_machine_data;
};

static_global c_game_flow_state_machine_enhanced g_game_flow;
static_global s_game_state* g_game_state;

void c_enhanced_game::init(const s_game_memory& game_memory)
{
	ASSERT(game_memory.size >= sizeof(s_game_state));
	g_game_state = reinterpret_cast<s_game_state*>(game_memory.data);
	ASSERT(g_game_state != nullptr);

#define SKIP_INTRO

#ifndef SKIP_INTRO
	g_game_flow.init(&g_game_state->game_flow_state_machine_data);
#else
	// feels a bit off to get the static id member directly...
	g_game_flow.init_with_state(
		&g_game_state->game_flow_state_machine_data, 
		c_game_flow_state_gameplay::id);
#endif //SKIP_INTRO
}

void c_enhanced_game::update(const s_input_state const_ptr input_state, real32 dt)
{
	bool still_running = true;
	g_game_flow.update(&g_game_state->game_flow_state_machine_data, dt, still_running);

	if (!still_running)
	{
		// handle player requested exit here
	}
}

#ifdef HOT_RELOAD
void c_enhanced_game::reload(const s_game_memory& game_memory)
{
	g_game_state = reinterpret_cast<s_game_state*>(game_memory.data);
	ASSERT(g_game_state != nullptr);
}
#endif // HOT_RELOAD