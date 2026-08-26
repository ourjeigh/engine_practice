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

enum e_demo_game_menu_selection
{
	demo_game_menu_selection_camera_scene,
	demo_game_menu_selection_collision_scene,
	demo_game_menu_selection_exit,

	k_demo_game_menu_selection_count,
};
ENUM_MATH(e_demo_game_menu_selection, k_demo_game_menu_selection_count);

struct s_demo_game_state_menu_scene
{
	e_demo_game_menu_selection selection;
};

class c_demo_game_flow_menu_scene: public c_game_state_machine_state<s_demo_game_state_menu_scene>
{
public:
	real32 post_exit_wait_seconds() { return 0.5f; }

	void on_enter(s_demo_game_state_menu_scene* state_data, real32 dt, bool& out_continue);
	void on_update(s_demo_game_state_menu_scene* state_data, real32 dt, bool& out_continue);
	void on_exit(s_demo_game_state_menu_scene* state_data, real32 dt, bool& out_continue);

	c_string_id state_id() const { return id; }
	const char* debug_state_name() const { return id.get_debug_string(); };
	static_member_data constexpr c_string_id id = "demo_game_flow_state::menu";
};

struct s_demo_game_state_camera_scene
{
	c_player player;
	c_camera_2d camera;
};

class c_demo_game_flow_camera_scene : public c_game_state_machine_state<s_demo_game_state_camera_scene>
{
public:
	void on_enter(s_demo_game_state_camera_scene* state_data, real32 dt, bool& out_continue);
	void on_update(s_demo_game_state_camera_scene* state_data, real32 dt, bool& out_continue);
	void on_exit(s_demo_game_state_camera_scene* state_data, real32 dt, bool& out_continue);

	c_string_id state_id() const { return id; }
	const char* debug_state_name() const { return id.get_debug_string(); };
	static_member_data constexpr c_string_id id = "demo_game_flow_state::camera_scene";
};

struct s_demo_game_state_collision_scene
{
	c_player player;
	c_camera_2d camera;
	c_static_stack<c_object, 8> scene_objects;
};

class c_demo_game_flow_collision_scene : public c_game_state_machine_state<s_demo_game_state_collision_scene>
{
public:
	void on_enter(s_demo_game_state_collision_scene* state_data, real32 dt, bool& out_continue);
	void on_update(s_demo_game_state_collision_scene* state_data, real32 dt, bool& out_continue);
	void on_exit(s_demo_game_state_collision_scene* state_data, real32 dt, bool& out_continue);

	c_string_id state_id() const { return id; }
	const char* debug_state_name() const { return id.get_debug_string(); };
	static_member_data constexpr c_string_id id = "demo_game_flow_state::collision_scene";
};

class c_demo_game_flow_state_machine : public c_state_machine_fsm
{
public:
	void set_state_id(s_game_flow_state_machine_data* data, c_string_id state_id)
	{
		data->current_state_id = state_id;
	}

	c_string_id get_next_state_id(const s_game_flow_state_machine_data* data) const
	{
		if (data->current_state_id == c_game_flow_state_logo::id)
		{
			return m_state_menu.id;
		}

		if (data->current_state_id == c_demo_game_flow_menu_scene::id)
		{
			const s_demo_game_state_menu_scene* menu_state_data = 
				reinterpret_cast<const s_demo_game_state_menu_scene*>(data->current_state_data);

			ASSERT(menu_state_data != nullptr);

			switch (menu_state_data->selection)
			{
			case demo_game_menu_selection_camera_scene:
				return m_state_camera_scene.id;
			case demo_game_menu_selection_collision_scene:
				return m_state_collision_scene.id;
			default:
				HALT_UNIMPLEMENTED();
			}

			return m_state_menu.id;
		}

		if (data->current_state_id == c_demo_game_flow_camera_scene::id)
		{
			return m_state_menu.id;
		}

		if (data->current_state_id == c_demo_game_flow_collision_scene::id)
		{
			return m_state_menu.id;
		}

		return m_state_logo.id;
	}

	c_game_state_machine_state_base* get_state(const c_string_id& state_id)
	{
		if (state_id == c_game_flow_state_logo::id)
		{
			return &m_state_logo;
		}

		if (state_id == c_demo_game_flow_menu_scene::id)
		{
			return &m_state_menu;
		}

		if (state_id == c_demo_game_flow_camera_scene::id)
		{
			return &m_state_camera_scene;
		}

		if (state_id == c_demo_game_flow_collision_scene::id)
		{
			return &m_state_collision_scene;
		}

		return &m_state_logo;
	}


private:
	c_game_flow_state_logo m_state_logo;
	c_demo_game_flow_menu_scene m_state_menu;
	c_demo_game_flow_camera_scene m_state_camera_scene;
	c_demo_game_flow_collision_scene m_state_collision_scene;
};


#endif // __ENHANCED_GAME_STATE_MACHINE_H__