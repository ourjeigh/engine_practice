#ifndef __ENHANCED_GAME_STATE_MACHINE_H__
#define __ENHANCED_GAME_STATE_MACHINE_H__
#pragma once

#include "camera.h"
#include "engine_api.h"
#include "state/state_machine.h"
#include "types/types.h"
#include "player.h"
#include "state/logo_state.h"




struct s_flow_state_main_menu
{
};

class c_game_flow_state_main_menu : public c_game_state_machine_state<s_flow_state_main_menu>
{
public:
	real32 pre_enter_wait_seconds() { return 0.0f; }
	real32 post_exit_wait_seconds() { return 0.2f; }

	void on_enter(s_flow_state_main_menu* state_data, real32 dt, bool& out_continue);
	void on_update(s_flow_state_main_menu* state_data, real32 dt, bool& out_continue);
	void on_exit(s_flow_state_main_menu* state_data, real32 dt, bool& out_continue);

	c_string_id state_id() const { return id; }
	const char* debug_state_name() const { return id.get_debug_string(); };
	static_member_data constexpr c_string_id id = "enhanced_game_flow_state::main_menu";
};

struct s_flow_state_gameplay
{
	c_player player;
	c_camera_2d camera;
};

class c_game_flow_state_gameplay : public c_game_state_machine_state< s_flow_state_gameplay>
{
public:
	real32 pre_enter_wait_seconds() { return 0.0f; }
	real32 post_exit_wait_seconds() { return 0.0f; }

	void on_enter(s_flow_state_gameplay* state_data, real32 dt, bool& out_continue);
	void on_update(s_flow_state_gameplay* state_data, real32 dt, bool& out_continue);
	void on_exit(s_flow_state_gameplay* state_data, real32 dt, bool& out_continue);

	c_string_id state_id() const { return id; }
	const char* debug_state_name() const { return id.get_debug_string(); };
	static_member_data constexpr c_string_id id = "enhanced_game_flow_state::gameplay";
};

class c_game_flow_state_machine_enhanced : public c_state_machine_fsm
{
public:
	void set_state(s_game_flow_state_machine_data* data, c_string_id state_id)
	{
		if (state_id == m_state_logo.state_id())
		{
			data->current_state = &m_state_logo;
		}
		else if (state_id == m_state_main_menu.state_id())
		{
			data->current_state = &m_state_main_menu;
		}
	}

	c_game_state_machine_state_base* get_next_state(const c_game_state_machine_state_base* current_state)
	{
		if (current_state == nullptr)
		{
			return &m_state_logo;
		}

		if (current_state->is_type<c_game_flow_state_logo>())
		{
			return &m_state_main_menu;
		}

		if (current_state->is_type<c_game_flow_state_main_menu>())
		{
			return &m_state_gameplay;
		}

		if (current_state->is_type<c_game_flow_state_gameplay>())
		{
			return &m_state_main_menu;
		}

		HALT_UNIMPLEMENTED();
		return nullptr;
	}

private:
	c_game_flow_state_logo m_state_logo;
	c_game_flow_state_main_menu m_state_main_menu;
	c_game_flow_state_gameplay m_state_gameplay;
};


#endif // __ENHANCED_GAME_STATE_MACHINE_H__