#include "enhanced_game.h"
#include "camera.h"
#include "debug/debug_helpers.h"
#include "engine_api.h"
#include "input/input_utilities.h"
#include "player.h"
#include "types/asset_types.h"

struct s_game_state
{
	c_player player;
	c_camera_2d camera;
};

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
}

void c_enhanced_game::update(const s_input_state const_ptr input_state, real32 dt)
{
	// enhanced game here!

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