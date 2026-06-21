#include "game_api.h"
#include "engine_api.h"
#include "types/render_types.h"
#include "types/asset_types.h"
#include "structures/vector.h"
#include "camera.h"

const s_asset_definition k_click_audio_asset_def = { "sound_click_01", asset_scope_global, asset_type_wav, "C:\\Users\\RJ\\git\\simm_engine\\assets\\click_16_44k.wav" };
const s_asset_definition k_test_bmp_asset_def = { "test_bmp", asset_scope_global, asset_type_bitmap, R"(C:\Users\RJ\git\simm_engine\assets\test\dude.bmp)" };

struct s_transform
{
	t_vector_4d_real32 position;
	t_vector_4d_real32 rotation;
	t_vector_4d_real32 scale;

	void set(
		real32 position_x, real32 position_y, real32 position_z,
		real32 rotation_x, real32 rotation_y, real32 rotation_z,
		real32 scale_x, real32 scale_y, real32 scale_z)
	{
		position.set(position_x, position_y, position_z, 1);
		rotation.set(rotation_x, rotation_y, rotation_z, 0);
		scale.set(scale_x, scale_y, scale_z, 0);
	}
};

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

		g_game_state->player.m_transform.position.zero();
		g_game_state->player.m_bitmap_asset_id = k_test_bmp_asset_def.id;
		g_game_state->camera.m_position.zero();
		g_game_state->camera.m_zoom = 1;
		//g_game_state->camera.m_bounds.set(-720, -360, 1440, 720);
		g_game_state->camera.m_render_space.set(0, 0, 1440, 720);

		engine_log_verbose("game: game initialized");
	}

	GAME_API void game_update(const s_input_state const_ptr input_state, real32 dt)
	{
		engine_render_fill_screen(0xFF202020);

		t_vector_4d_real32 move_delta;
		get_arrow_key_move_delta(input_state, move_delta);

		if (input_state->get_key_state(input_key_special_shift).is_down)
		{
			g_game_state->camera.m_position += move_delta;
		}
		else
		{
			const real32 player_speed_pixels_hack = 5.0f;
			move_delta *= player_speed_pixels_hack;
			g_game_state->player.m_transform.position += move_delta;
		}

		const s_bitmap_asset* player_bitmap = static_cast<const s_bitmap_asset*>(engine_get_asset(g_game_state->player.m_bitmap_asset_id));
		if (player_bitmap != nullptr)
		{
			t_vector_4d_real32 world_position(
				g_game_state->player.m_transform.position.x() - player_bitmap->width / 2,
				g_game_state->player.m_transform.position.y() - player_bitmap->height / 2,
				0, 0);

			t_render_shape_point render_pos = g_game_state->camera.world_to_screen_space(world_position);

			t_render_shape_rect rect;
			rect.x = render_pos.x();
			rect.y = render_pos.y();
			rect.width = player_bitmap->width * g_game_state->camera.m_zoom;
			rect.height = player_bitmap->height * g_game_state->camera.m_zoom;
			engine_render_bitmap(
				*player_bitmap,
				rect,
				render_layer_main);
		}

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
				g_game_state->camera.m_position.x(),
				g_game_state->camera.m_position.y(),
				g_game_state->camera.m_position.z());

			engine_render_draw_string(camera_position_string, align_x, 15, 1, k_color_white);
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