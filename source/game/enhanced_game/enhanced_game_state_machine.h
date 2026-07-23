#ifndef __ENHANCED_GAME_STATE_MACHINE_H__
#define __ENHANCED_GAME_STATE_MACHINE_H__
#pragma once

#include "camera.h"
#include "mmath.h"
#include "engine_api.h"
#include "state/state_machine.h"
#include "types/types.h"
#include "player.h"
#include "state/logo_state.h"

enum e_main_menu_selection
{
	main_menu_selection_new_game,
	main_menu_selection_exit,
	k_main_menu_selection_count,
};
ENUM_MATH(e_main_menu_selection, k_main_menu_selection_count);

struct s_flow_state_main_menu
{
	e_main_menu_selection selection;
	bool show_popup;
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
	c_static_stack<c_object, 8> scene_objects;
};

class c_game_flow_state_gameplay : public c_game_state_machine_state<s_flow_state_gameplay>
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
	void set_state_id(s_game_flow_state_machine_data* data, c_string_id state_id)
	{
		data->current_state_id = state_id;
	}

	c_string_id get_next_state_id(const c_string_id& current_state_id)
	{
		if (current_state_id == c_game_flow_state_logo::id)
		{
			return m_state_main_menu.id;
		}

		if (current_state_id == c_game_flow_state_main_menu::id)
		{
			return m_state_gameplay.id;
		}

		if (current_state_id == c_game_flow_state_gameplay::id)
		{
			return m_state_main_menu.id;
		}

		return m_state_logo.id;
	}

	c_game_state_machine_state_base* get_state(const c_string_id& state_id)
	{
		if (state_id == c_game_flow_state_logo::id)
		{
			return &m_state_logo;
		}

		if (state_id == c_game_flow_state_main_menu::id)
		{
			return &m_state_main_menu;
		}

		if (state_id == c_game_flow_state_gameplay::id)
		{
			return &m_state_gameplay;
		}

		return &m_state_logo;
	}


private:
	c_game_flow_state_logo m_state_logo;
	c_game_flow_state_main_menu m_state_main_menu;
	c_game_flow_state_gameplay m_state_gameplay;
};


#endif // __ENHANCED_GAME_STATE_MACHINE_H__