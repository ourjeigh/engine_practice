#include "game_api.h"
#include "engine_api.h"
#include "types/render_types.h"
#include "types/asset_types.h"
#include "structures/vector.h"
#include "structures/matrix.h"

const s_asset_definition k_click_audio_asset_def = { "sound_click_01", asset_scope_global, asset_type_wav, "C:\\Users\\RJ\\git\\simm_engine\\assets\\click_16_44k.wav" };
const s_asset_definition k_test_bmp_asset_def = { "test_bmp", asset_scope_global, asset_type_bitmap, R"(C:\Users\RJ\git\simm_engine\assets\test\dude.bmp)" };

struct s_rect_2d
{
	t_vector_2d_real32 bottom_left;
	t_vector_2d_real32 top_right;

	real32 width() const
	{
		return top_right.x() - bottom_left.x();
	}

	real32 height() const
	{
		return top_right.y() - bottom_left.y();
	}
};

class c_player
{
public:
	t_vector_4d_real32 position;

private:
};

class c_camera_2d
{
public:
	t_vector_4d_real32 position;
	s_rect_2d bounds;
	s_render_shape_rect render_space;

	t_vector_4d_real32 get_inverse() const
	{
		t_vector_4d_real32 out = position;
		out.flip();
		return out;
	}

	t_render_shape_point get_render_point_from_position(const t_vector_4d_real32& position) const
	{
		t_render_shape_point out;

		real32 percent_x = (position.x() - bounds.bottom_left.x()) / bounds.width();
		real32 percent_y = (position.y() - bounds.bottom_left.y()) / bounds.height();


		out.set(render_space.width * percent_x, render_space.height * percent_y);

		return out;
	}
};

struct s_game_state
{
	c_player player;
	c_camera_2d camera;

	s_bitmap_asset* g_test_bmp;
	s_wav_asset* g_test_sound;
};

s_game_state* g_game_state;

void asset_loaded_callback(c_string_id asset_id, s_asset* asset, void* object)
{
	if (asset_id == k_test_bmp_asset_def.id)
	{
		g_game_state->g_test_bmp = static_cast<s_bitmap_asset*>(asset);
		ASSERT(g_game_state->g_test_bmp != nullptr);
	}
	else if (asset_id == k_click_audio_asset_def.id)
	{
		g_game_state->g_test_sound = static_cast<s_wav_asset*>(asset);
		ASSERT(g_game_state->g_test_sound != nullptr);
	}
}

void HACK_old_demo_update(const s_input_state const_ptr input_state, real32 dt)
{
	static_local_variable bool was_mouse_down = false;

	engine_render_fill_screen(0xFF202020);

	//const s_mouse_state* mouse_state = engine_input_get_mouse_state();
	int32 mouse_x = input_state->get_mouse_state().position.x;
	int32 mouse_y = input_state->get_mouse_state().position.y;

	const bool mouse_down = input_state->get_key_state(input_mouse_left).is_down;
	const uint32 color = mouse_down ?
		k_color_red_uint32 :
		k_color_blue_uint32;

	s_render_shape_rect mouse_box(mouse_x - 25, mouse_y - 25, 50, 50);
	engine_render_draw_line(engine_get_screen_center(), t_render_shape_point(mouse_x, mouse_y), k_color_green_uint32);

	s_render_shape_circle circle;
	circle.center = t_render_shape_point(mouse_x, mouse_y);
	circle.radius = 38;

	if (g_game_state->g_test_bmp)
	{
		int32 x = mouse_x - (g_game_state->g_test_bmp->width / 2);
		int32 y = mouse_y - (g_game_state->g_test_bmp->height / 2);
		s_render_shape_rect rect;
		rect.x = x;
		rect.y = y;
		rect.width = g_game_state->g_test_bmp->width;
		rect.height = g_game_state->g_test_bmp->height;
		engine_render_bitmap(*g_game_state->g_test_bmp, rect, render_layer_main);
	}

	if (mouse_down)
	{
		engine_render_draw_rect(mouse_box, color);
		engine_render_draw_circle(circle, color, false);
	}
	else
	{
		engine_render_draw_circle(circle, color, true);
	}

	if (mouse_down && !was_mouse_down)
	{
		const s_wav_asset* click_asset = static_cast<const s_wav_asset*>(engine_get_asset(k_click_audio_asset_def.id));
		if (click_asset != nullptr)
		{
			engine_audio_play_sound(*click_asset);
		}
	}

	was_mouse_down = mouse_down;
}

extern "C"
{
	GAME_API void game_init(s_game_engine_context& engine_context)
	{
		g_engine_ptr = engine_context.engine;
		g_assert_handler = engine_context.assert_hook;
		g_game_state = reinterpret_cast<s_game_state*>(engine_context.memory.data);
		ASSERT(g_game_state != nullptr);

		engine_load_asset(&k_click_audio_asset_def, asset_loaded_callback, nullptr);
		engine_load_asset(&k_test_bmp_asset_def, asset_loaded_callback, nullptr);

		g_game_state->player.position.zero();
		g_game_state->camera.position.zero();
		g_game_state->camera.bounds.bottom_left.set(-720, -360);
		g_game_state->camera.bounds.top_right.set(720, 360);
		g_game_state->camera.render_space.x = 0;
		g_game_state->camera.render_space.y = 0;
		g_game_state->camera.render_space.width = 1440;
		g_game_state->camera.render_space.height = 720;

		engine_log_verbose("game: game initialized");
	}

	GAME_API void game_update(const s_input_state const_ptr input_state, real32 dt)
	{
		engine_render_fill_screen(0xFF202020);
		const real32 player_speed_pixels_hack = 5.0f;
		const int32 player_speed_shift_modifier = input_state->get_key_state(input_key_special_shift).is_down ? 2 : 1;

		t_vector_4d_real32 player_delta;
		player_delta.zero();

		if (input_state->get_key_state(input_key_arrow_up).is_down)
		{
			player_delta.y() -= 1;
		}

		if (input_state->get_key_state(input_key_arrow_down).is_down)
		{
			player_delta.y() += 1;
		}

		if (input_state->get_key_state(input_key_arrow_left).is_down)
		{
			player_delta.x() -= 1;
		}

		if (input_state->get_key_state(input_key_arrow_right).is_down)
		{
			player_delta.x() += 1;
		}

		player_delta.normalize();
		player_delta *= player_speed_pixels_hack * player_speed_shift_modifier;
		g_game_state->player.position += player_delta;


		if (g_game_state->g_test_bmp != nullptr)
		{
			t_vector_4d_real32 world_position(
				g_game_state->player.position.x() - g_game_state->g_test_bmp->width / 2,
				g_game_state->player.position.y() - g_game_state->g_test_bmp->height / 2,
				0, 0);

			t_render_shape_point render_pos = g_game_state->camera.get_render_point_from_position(world_position);

			s_render_shape_rect rect;
			rect.x = render_pos.x();
			rect.y = render_pos.y();
			rect.width = 50;// g_game_state->g_test_bmp->width;
			rect.height = 50; // g_game_state->g_test_bmp->height;
			engine_render_bitmap(
				*g_game_state->g_test_bmp, 
				rect,
				render_layer_main);
		}

		static_local_variable bool bad = false;
		if (input_state->get_key_state(input_key_special_tab).is_down)
		{
			if (!bad && g_game_state->g_test_sound != nullptr)
			{
				bad = true;
				engine_audio_play_sound(*g_game_state->g_test_sound);
			}
		}
		else
		{
			bad = false;
		}
	}

#ifdef HOT_RELOAD
	GAME_API void game_reload(s_game_engine_context& engine_context)
	{
		g_engine_ptr = engine_context.engine;
		g_assert_handler = engine_context.assert_hook;
		g_game_state = reinterpret_cast<s_game_state*>(engine_context.memory.data);
		ASSERT(g_game_state != nullptr);
	
		engine_log_verbose("game: game reloaded");
	}
#endif //HOT_RELOAD
}