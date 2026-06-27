#ifndef __ENHANCED_GAME_STATE_MACHINE_H__
#define __ENHANCED_GAME_STATE_MACHINE_H__
#pragma once

#include "types/types.h"
#include "state/state_machine.h"
#include "engine_api.h"

const s_asset_definition k_simm_logo_bmp_asset_def = { "logo_bmp", asset_scope_global, asset_type_bitmap, R"(C:\Users\RJ\git\simm_engine\assets\simm_logo.bmp)" };

/////////////////////////////////////////////////////
// TEMP MOVE
void render_full_screen_fade(real32 fade_value)
{
	s_screen_dimensions dimensions = engine_get_screen_dimensions();
	t_render_shape_rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.height = dimensions.height;
	rect.width = dimensions.width;

	c_color color = k_color_black;
	real32 alpha = math_pin(0.0f, 1.0f, (1.0f - fade_value));
	color.set_alpha(alpha);

	engine_render_draw_rect(rect, color.to_uint32());
}
// //TEMP MOVE
/////////////////////////////////////////////////////


struct s_flow_state_logo_data
{
	real32 fade_in_value;
	bool is_fading_in;
};

class c_game_flow_state_logo : public c_game_state_machine_state<s_flow_state_logo_data>
{
public:
	real32 pre_enter_wait_seconds() { return 0.0f; }
	real32 post_exit_wait_seconds() { return 0.5f; }

	void on_enter(s_flow_state_logo_data* state_data)
	{
		state_data->fade_in_value = 0.0f;
		state_data->is_fading_in = true;
		engine_load_asset(&k_simm_logo_bmp_asset_def, nullptr, nullptr);
	}

	void on_update(s_flow_state_logo_data* state_data, real32 dt, bool& out_continue)
	{
		const s_asset* asset = engine_get_asset(k_simm_logo_bmp_asset_def.id);
		if (asset == nullptr)
		{
			out_continue = true;
			return;
		}
		else
		{
			const real32 fade_in_time_seconds = 2.5f;
			const real32 fade_out_time_seconds = 1.3f;

			real32 fade_time_seonds = state_data->is_fading_in ? fade_in_time_seconds : -fade_out_time_seconds;
			real32 delta = dt / fade_time_seonds;
			state_data->fade_in_value += delta;

			const s_bitmap_asset* bitmap = static_cast<const s_bitmap_asset*>(asset);
			ASSERT(bitmap != nullptr);

			s_screen_dimensions dimensions = engine_get_screen_dimensions();
			t_render_shape_rect rect;
			rect.x = 0;
			rect.y = 0;
			rect.height = dimensions.height;
			rect.width = dimensions.width;
			engine_render_bitmap(*bitmap, rect, render_layer_background);

			render_full_screen_fade(state_data->fade_in_value);

			if (state_data->fade_in_value >= 1.0f)
			{
				state_data->is_fading_in = false;
			}
			else if (state_data->fade_in_value < 0.0f && !state_data->is_fading_in)
			{
				out_continue = false;
			}
		}
	}

	void on_exit(s_flow_state_logo_data* state_data)
	{
		// unload asset
	}

	c_string_id state_id() const { return id; }
	const char* debug_state_name() const { return id.get_debug_string(); };
	static_member_data constexpr c_string_id id = "enhanced_game_flow_state::logo";
};

struct s_flow_state_main_menu
{
};

class c_game_flow_state_main_menu : public c_game_state_machine_state<s_flow_state_main_menu>
{
public:
	real32 pre_enter_wait_seconds() { return 0.0f; }
	real32 post_exit_wait_seconds() { return 0.2f; }

	void on_enter(s_flow_state_main_menu* state_data)
	{
	}

	void on_update(s_flow_state_main_menu* state_data, real32 dt, bool& out_continue)
	{
		out_continue = true;

		engine_render_fill_screen(k_color_black.to_uint32());

		t_string_128 title("Enhanced");
		engine_render_draw_string(title, 600, 300, 2, k_color_white);

		if (engine_input_get_key_state(input_key_special_return).is_down)
		{
			out_continue = false;
		}
	}

	void on_exit(s_flow_state_main_menu* state_data)
	{
	}

	c_string_id state_id() const { return id; }
	const char* debug_state_name() const { return id.get_debug_string(); };
	static_member_data constexpr c_string_id id = "enhanced_game_flow_state::main_menu";

private:
};

struct s_flow_state_gameplay
{
};


class c_game_flow_state_gameplay : public c_game_state_machine_state< s_flow_state_gameplay>
{
public:
	real32 pre_enter_wait_seconds() { return 0.0f; }
	real32 post_exit_wait_seconds() { return 0.0f; }

	void on_enter(s_flow_state_gameplay* state_data)
	{
	}

	void on_update(s_flow_state_gameplay* state_data, real32 dt, bool& out_continue)
	{
		engine_render_fill_screen(k_color_black.to_uint32());

		t_string_128 title("Gameplay!");
		engine_render_draw_string(title, 600, 300, 5, k_color_white);

		if (engine_input_get_key_state(input_key_special_return).is_down)
		{
			out_continue = false;
		}
	}

	void on_exit(s_flow_state_gameplay* state_data)
	{
	}

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