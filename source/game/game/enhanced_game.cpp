#include "enhanced_game.h"
#include "camera.h"
#include "debug/debug_helpers.h"
#include "engine_api.h"
#include "input/input_utilities.h"
#include "player.h"
#include "types/asset_types.h"

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

class c_game_state_machine_state
{
public:
	virtual real32 pre_enter_wait_seconds() = 0;
	virtual void enter() = 0;
	virtual void update(real32 dt, bool& out_continue) = 0;
	virtual void exit() = 0;
	virtual real32 post_exit_wait_seconds() = 0;

	virtual c_string_id state_id() const = 0;
};

class c_game_flow_state_logo : public c_game_state_machine_state
{
public:
	real32 pre_enter_wait_seconds() { return 0.0f; }
	real32 post_exit_wait_seconds() { return 0.5f; }

	void enter()
	{
		m_fade_in_value = 0.0f;
		m_fade_in = true;
		engine_load_asset(&k_simm_logo_bmp_asset_def, nullptr, nullptr);
	}
	
	void update(real32 dt, bool& out_continue)
	{
		const s_asset* asset= engine_get_asset(k_simm_logo_bmp_asset_def.id);
		if (asset == nullptr)
		{
			out_continue = true;
			return;
		}
		else
		{
			const real32 fade_in_time_seconds = 2.5f;
			const real32 fade_out_time_seconds = 1.3f;

			real32 fade_time_seonds = m_fade_in ? fade_in_time_seconds : -fade_out_time_seconds;
			real32 delta =  dt / fade_time_seonds;
			m_fade_in_value += delta;

			const s_bitmap_asset* bitmap = static_cast<const s_bitmap_asset*>(asset);
			ASSERT(bitmap != nullptr);

			s_screen_dimensions dimensions = engine_get_screen_dimensions();
			t_render_shape_rect rect;
			rect.x = 0;
			rect.y = 0;
			rect.height = dimensions.height;
			rect.width = dimensions.width;
			engine_render_bitmap(*bitmap, rect, render_layer_background);

			render_full_screen_fade(m_fade_in_value);

			if (m_fade_in_value >= 1.0f)
			{
				m_fade_in = false;
			}
			else if (m_fade_in_value < 0.0f && !m_fade_in)
			{
				out_continue = false;
			}
		}
	}
	
	void exit()
	{
		// unload asset
	}

	c_string_id state_id() const { return id; };
	static_member_function c_string_id static_id() { return id; };

private:
	real32 m_fade_in_value;
	bool m_fade_in;

	static constexpr c_string_id id = "enhanced_game_flow_state::logo";
};

class c_game_flow_state_main_menu : public c_game_state_machine_state
{
public:
	real32 pre_enter_wait_seconds() { return 0.0f; }
	real32 post_exit_wait_seconds() { return 0.2f; }

	void enter()
	{
	}
	
	void update(real32 dt, bool& out_continue)
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

	void exit()
	{
	}

	c_string_id state_id() const { return id; };
	static_member_function c_string_id static_id() { return id; };

private:
	static constexpr c_string_id id = "enhanced_game_flow_state::main_menu";
};

class c_game_flow_state_gameplay : public c_game_state_machine_state
{
public:
	real32 pre_enter_wait_seconds() { return 0.0f; }
	real32 post_exit_wait_seconds() { return 0.0f; }

	void enter()
	{
	}

	void update(real32 dt, bool& out_continue)
	{
		engine_render_fill_screen(k_color_black.to_uint32());

		t_string_128 title("Gameplay!");
		engine_render_draw_string(title, 600, 300, 5, k_color_white);

		if (engine_input_get_key_state(input_key_special_return).is_down)
		{
			out_continue = false;
		}
	}

	void exit()
	{
	}

	c_string_id state_id() const { return id; };
	static_member_function c_string_id static_id() { return id; };

private:
	static constexpr c_string_id id = "enhanced_game_flow_state::gameplay";
};

enum e_state_machine_phase
{
	state_machine_phase_pre_enter,
	state_machine_phase_enter,
	state_machine_phase_in_state,
	state_machine_phase_exit,
	state_machine_phase_post_exit,
	state_machine_phase_finished,

	k_state_machine_phase_count
};

class c_game_flow_state_machine 
{
public:
	void init()
	{
		m_phase = state_machine_phase_pre_enter;
		m_wait = 0.0f;
		m_current_state = get_next_state(m_current_state);
	}

	void init_with_state(c_string_id state_id)
	{
		init();
		set_state(state_id);
	}

	void set_state(c_string_id state_id)
	{
		if (state_id == m_state_logo.state_id())
		{
			m_current_state = &m_state_logo;
		}
		else if (state_id == m_state_main_menu.state_id())
		{
			m_current_state = &m_state_main_menu;
		}
	}

	c_game_state_machine_state* get_next_state(const c_game_state_machine_state* current_state)
	{
		if (current_state == nullptr)
		{
			return &m_state_logo;
		}

		if (current_state->state_id() == c_game_flow_state_logo::static_id())
		{
			return &m_state_main_menu;
		}

		if (current_state->state_id() == c_game_flow_state_main_menu::static_id())
		{
			return &m_state_gameplay;
		}

		if (current_state->state_id() == c_game_flow_state_gameplay::static_id())
		{
			return &m_state_main_menu;
		}

		HALT_UNIMPLEMENTED();
		return nullptr;
	}

	void update(real32 dt, bool& out_continue)
	{
		bool state_continue = true;
		if (m_current_state)
		{
			switch (m_phase)
			{
			case state_machine_phase_pre_enter:
			{
				real32 wait_time_seconds = m_current_state->pre_enter_wait_seconds();
				
				if (wait_time_seconds > 0)
				{
					real32 delta = dt / wait_time_seconds;
					m_wait += delta;
				}

				if (m_wait >= wait_time_seconds)
				{
					m_wait = 0.0f;
					m_phase = state_machine_phase_enter;
				}

				break;
			}
			case state_machine_phase_enter:
			{
				m_current_state->enter();
				m_phase = state_machine_phase_in_state;
				break;
			}
			case state_machine_phase_in_state:
			{
				m_current_state->update(dt, state_continue);
				if (!state_continue)
				{
					m_phase = state_machine_phase_exit;
				}
				break;
			}
			case state_machine_phase_exit:
			{
				m_current_state->exit();
				m_phase = state_machine_phase_post_exit;
				break;
			}
			case state_machine_phase_post_exit:
			{
				real32 wait_time_seconds = m_current_state->post_exit_wait_seconds();
				
				if (wait_time_seconds > 0)
				{
					real32 delta = dt / wait_time_seconds;
					m_wait += delta;
				}

				if (m_wait >= wait_time_seconds)
				{
					m_wait = 0.0f;
					m_phase = state_machine_phase_finished;
				}
				break;
			}
			case state_machine_phase_finished:
			{
				m_current_state = get_next_state(m_current_state);
				m_phase = state_machine_phase_pre_enter;
				break;
			}
			}
		}
		else
		{
			out_continue = false;
		}
	}

	void term()
	{
	}

private:
	c_game_state_machine_state* m_current_state;
	e_state_machine_phase m_phase;
	real32 m_wait;

	// probably bad
	c_game_flow_state_logo m_state_logo;
	c_game_flow_state_main_menu m_state_main_menu;
	c_game_flow_state_gameplay m_state_gameplay;
};

// //TEMP MOVE
/////////////////////////////////////////////////////

struct s_game_state
{
	c_player player;
	c_camera_2d camera;

};

static_global c_game_flow_state_machine g_game_flow;
static_global s_game_state* g_game_state;

void c_enhanced_game::init(const s_game_memory& game_memory)
{
	ASSERT(game_memory.size >= sizeof(s_game_state));
	g_game_state = reinterpret_cast<s_game_state*>(game_memory.data);
	ASSERT(g_game_state != nullptr);

	g_game_state->player.m_transform.reset();
	g_game_state->camera.set_transform(s_transform::default_values());
	g_game_state->camera.set_zoom(1.0f);
	g_game_state->camera.set_width(10.0f);
	g_game_state->camera.set_screen_dimensions(engine_get_screen_dimensions());

	g_game_flow.init();
	//g_game_flow.init_with_state(c_game_flow_state_main_menu::state_id());
}

void c_enhanced_game::update(const s_input_state const_ptr input_state, real32 dt)
{
	// enhanced game here!
	bool still_running = true;
	g_game_flow.update(dt, still_running);

	if (!still_running)
	{
		still_running = true;
	}

#ifdef CONFIG_DEBUG
	//draw_debug_world_grid(20, g_game_state->camera);

	const int32 align_x = 1170;
	{
		t_string_128 player_position_string;
		player_position_string.printf("Player: {f6.2}, {f6.2}, {f6.2}",
			g_game_state->player.m_transform.position.x(),
			g_game_state->player.m_transform.position.y(),
			g_game_state->player.m_transform.position.z());

		engine_render_draw_string(player_position_string, align_x, 5, 1, k_color_white);
	}
	{
		t_string_128 camera_position_string;
		camera_position_string.printf("Camera: {f6.2}, {f6.2}, {f6.2}",
			g_game_state->camera.get_transform().position.x(),
			g_game_state->camera.get_transform().position.y(),
			g_game_state->camera.get_transform().position.z());

		engine_render_draw_string(camera_position_string, align_x, 15, 1, k_color_white);
	}
	{
		t_string_128 camera_viewport_title("Camera Viewport");
		engine_render_draw_string(camera_viewport_title, align_x, 25, 1, k_color_white);
		const int32 view_align_x = align_x + 65;
		t_string_128 camera_viewport_string;
		g_game_state->camera.get_viewport_debug_string(camera_viewport_string);
		engine_render_draw_string(camera_viewport_string, view_align_x, 35, 1, k_color_white);
	}
#endif // CONFIG_DEBUG
}

#ifdef HOT_RELOAD
void c_enhanced_game::reload(const s_game_memory& game_memory)
{
	g_game_state = reinterpret_cast<s_game_state*>(game_memory.data);
	ASSERT(g_game_state != nullptr);
}
#endif // HOT_RELOAD