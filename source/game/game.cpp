#include "game_api.h"
#include "engine_api.h"
#include "types/render_types.h"
#include "types/asset_types.h"
#include "structures/vector.h"
#include "camera.h"
#include "../engine/platform/platform.h"

const s_asset_definition k_click_audio_asset_def = { "sound_click_01", asset_scope_global, asset_type_wav, "C:\\Users\\RJ\\git\\simm_engine\\assets\\click_16_44k.wav" };
const s_asset_definition k_test_bmp_asset_def = { "test_bmp", asset_scope_global, asset_type_bitmap, R"(C:\Users\RJ\git\simm_engine\assets\test\dude.bmp)" };

class c_object
{
public:
	s_transform m_transform;
};

class c_player : public c_object
{
public:
	c_string_id m_bitmap_asset_id;
};

struct s_game_state
{
	c_player player;
	c_camera_2d camera;

	s_wav_asset* g_test_sound;
};

s_game_state* g_game_state;

void asset_loaded_callback(c_string_id asset_id, s_asset* asset, void* object)
{
	if (asset_id == k_click_audio_asset_def.id)
	{
		g_game_state->g_test_sound = static_cast<s_wav_asset*>(asset);
		ASSERT(g_game_state->g_test_sound != nullptr);
	}
}

void get_arrow_key_move_delta(const s_input_state const_ptr input_state, t_vector_4d_real32& out_move_delta)
{
	out_move_delta.zero();

	if (input_state->get_key_state(input_key_arrow_up).is_down)
	{
		out_move_delta.y() += 1;
	}

	if (input_state->get_key_state(input_key_arrow_down).is_down)
	{
		out_move_delta.y() -= 1;
	}

	if (input_state->get_key_state(input_key_arrow_left).is_down)
	{
		out_move_delta.x() -= 1;
	}

	if (input_state->get_key_state(input_key_arrow_right).is_down)
	{
		out_move_delta.x() += 1;
	}

	out_move_delta.normalize();
}

void get_wads_key_move_delta(const s_input_state const_ptr input_state, t_vector_4d_real32& out_move_delta)
{
	out_move_delta.zero();

	if (input_state->get_key_state(input_key_char_w).is_down)
	{
		out_move_delta.y() += 1;
	}

	if (input_state->get_key_state(input_key_char_s).is_down)
	{
		out_move_delta.y() -= 1;
	}

	if (input_state->get_key_state(input_key_char_a).is_down)
	{
		out_move_delta.x() -= 1;
	}

	if (input_state->get_key_state(input_key_char_d).is_down)
	{
		out_move_delta.x() += 1;
	}

	out_move_delta.normalize();
}

void draw_debug_world_grid()
{
	const int32 grid_size = 20;
	const int32 start_x = -grid_size/2;
	const int32 end_x = grid_size / 2;
	const int32 start_y = -grid_size / 2;
	const int32 end_y = grid_size / 2;

	for (int32 x = start_x; x <= end_x; x++)
	{
		t_vector_4d_real32 vertical_start(x, start_y, 0, 1);
		t_vector_4d_real32 vertical_end(x, end_y, 0, 1);

		engine_render_draw_line(
			g_game_state->camera.world_to_screen_space(vertical_start),
			g_game_state->camera.world_to_screen_space(vertical_end),
			k_color_green_uint32,
			render_layer_debug);
	}

	for (int32 y = start_y; y <= end_y; y++)
	{
		t_vector_4d_real32 horizontal_start(start_x, y, 0, 1);
		t_vector_4d_real32 horizontal_end(end_x, y, 0, 1);

		engine_render_draw_line(
			g_game_state->camera.world_to_screen_space(horizontal_start),
			g_game_state->camera.world_to_screen_space(horizontal_end),
			k_color_green_uint32,
			render_layer_debug);
	}
}

extern "C"
{
	GAME_API void game_init(s_game_engine_context& engine_context)
	{
		g_engine_ptr = engine_context.engine;
		g_assert_handler = engine_context.assert_hook;
		ASSERT(engine_context.memory.size >= sizeof(s_game_state));
		g_game_state = reinterpret_cast<s_game_state*>(engine_context.memory.data);
		ASSERT(g_game_state != nullptr);

		engine_load_asset(&k_click_audio_asset_def, asset_loaded_callback, nullptr);
		engine_load_asset(&k_test_bmp_asset_def, asset_loaded_callback, nullptr);

		g_game_state->player.m_transform.reset();
		g_game_state->player.m_bitmap_asset_id = k_test_bmp_asset_def.id;
		g_game_state->camera.set_transform(s_transform::default_values());
		g_game_state->camera.set_zoom(1.0f);
		g_game_state->camera.set_width(10.0f);
		g_game_state->camera.set_screen_dimensions(engine_get_screen_dimensions());

		engine_log_verbose("game: game initialized");
	}

	GAME_API void game_update(const s_input_state const_ptr input_state, real32 dt)
	{
		engine_render_fill_screen(0xFF202020);

		

		//if (input_state->get_key_state(input_key_special_shift).is_down)
		{
			real32 zoom = g_game_state->camera.get_zoom();
			if (zoom < 1.0f)
			{
				zoom *= 1.05f;
				g_game_state->camera.set_zoom(zoom);
			}

			t_vector_4d_real32 move_delta;
			get_wads_key_move_delta(input_state, move_delta);
			const real32 camera_speed_meters_per_second = 1.5f;
			move_delta *= camera_speed_meters_per_second * dt;
			s_transform camera_transfom = g_game_state->camera.get_transform();
			camera_transfom.position += move_delta;
			g_game_state->camera.set_transform(camera_transfom);
		}
		//else
		{
			t_vector_4d_real32 move_delta;
			get_arrow_key_move_delta(input_state, move_delta);
			const real32 player_speed_meters_per_second = 2.0f;
			move_delta *= player_speed_meters_per_second * dt;
			g_game_state->player.m_transform.position += move_delta;
		}

		const s_bitmap_asset* player_bitmap = static_cast<const s_bitmap_asset*>(engine_get_asset(g_game_state->player.m_bitmap_asset_id));
		if (player_bitmap != nullptr)
		{
			// todo: make position a well defined type with w hardcoded to 1
			g_game_state->player.m_transform.position.w() = 1.0f;

			t_render_shape_point render_pos = g_game_state->camera.world_to_screen_space(g_game_state->player.m_transform.position);
			t_render_shape_rect rect;
			rect.x = render_pos.x() - player_bitmap->width * g_game_state->camera.get_zoom() / 2;
			rect.y = render_pos.y() - player_bitmap->height * g_game_state->camera.get_zoom() / 2;
			rect.width = player_bitmap->width * g_game_state->camera.get_zoom();
			rect.height = player_bitmap->height * g_game_state->camera.get_zoom();
			engine_render_bitmap(
				*player_bitmap,
				rect,
				render_layer_main);
		}

#ifdef CONFIG_DEBUG
		draw_debug_world_grid();

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