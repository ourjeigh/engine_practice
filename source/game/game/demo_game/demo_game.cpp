#include "demo_game.h"
#include "player.h"
#include "camera.h"
#include "debug/debug_helpers.h"
#include "input/input_utilities.h"
#include "types/asset_types.h"
#include "engine_api.h"
#include <state/state_machine.h>
#include "demo_game_state_machine.h"

struct s_demo_game_state
{
	s_game_flow_state_machine_data game_flow_state_machine_data;
};

static_global c_demo_game_flow_state_machine g_game_flow;
static_global s_demo_game_state* g_game_state;

void c_demo_game::init(const s_game_memory& game_memory)
{
	ASSERT(game_memory.size >= sizeof(s_demo_game_state));
	g_game_state = reinterpret_cast<s_demo_game_state*>(game_memory.data);
	ASSERT(g_game_state != nullptr);
	
	g_game_flow.init(&g_game_state->game_flow_state_machine_data);
	/*g_game_flow.init_with_state(
		&g_game_state->game_flow_state_machine_data,
		"demo_game_flow_state::collision_scene");*/
}

void c_demo_game::update(const s_input_state const_ptr input_state, real32 dt)
{
	bool should_continue;
	g_game_flow.update(
		&g_game_state->game_flow_state_machine_data,
		dt,
		should_continue);
}

#ifdef HOT_RELOAD
void c_demo_game::reload(const s_game_memory& game_memory)
{
	g_game_state = reinterpret_cast<s_demo_game_state*>(game_memory.data);
	ASSERT(g_game_state != nullptr);
}
#endif // HOT_RELOAD