#ifndef __LOGO_STATE_H__
#define __LOGO_STATE_H__
#pragma once

#include <types/types.h>
#include "state/state_machine.h"

struct s_flow_state_logo_data
{
	bool assets_loaded;
	real32 fade_in_value;
	bool is_fading_in;
};

class c_game_flow_state_logo : public c_game_state_machine_state<s_flow_state_logo_data>
{
public:
	real32 pre_enter_wait_seconds() { return 0.5f; }
	real32 post_exit_wait_seconds() { return 0.75f; }

	void on_enter(s_flow_state_logo_data* state_data, real32 dt, bool& out_continue);
	void on_update(s_flow_state_logo_data* state_data, real32 dt, bool& out_continue);
	void on_exit(s_flow_state_logo_data* state_data, real32 dt, bool& out_continue);

	c_string_id state_id() const { return id; }
	const char* debug_state_name() const { return id.get_debug_string(); };
	static_member_data constexpr c_string_id id = "enhanced_game_flow_state::logo";
};

#endif // !__LOGO_STATE_H__
