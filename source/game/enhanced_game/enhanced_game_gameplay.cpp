#include "enhanced_game_state_machine.h"
#include "debug/debug_helpers.h"
#include "input/input_utilities.h"

void c_game_flow_state_gameplay::on_enter(s_flow_state_gameplay* state_data, real32 dt, bool& out_continue)
{
	state_data->player.m_transform.reset();
	c_object& dummy = state_data->scene_objects.push();
	dummy.m_transform.reset();
	dummy.m_transform.position.set(1, 1, 0, 1);
	state_data->camera.set_transform(s_transform::default_values());
	state_data->camera.set_zoom(1.0f);
	state_data->camera.set_width(10.0f);
	state_data->camera.set_screen_dimensions(engine_get_screen_dimensions());
	out_continue = false;
}

void c_game_flow_state_gameplay::on_update(s_flow_state_gameplay* state_data, real32 dt, bool& out_continue)
{
	engine_render_fill_screen(k_color_black.to_uint32());

	t_string_128 title("Gameplay!");
	engine_render_draw_string(title, 600, 300, 5, k_color_white, render_layer_ui);
	
	t_rect_2d_real32 game_rect(-0.9, 0.9, 1.2, 1.2);

	auto input_state = engine_input_get_input_state();
	t_vector_4d_real32 player_move_delta;
	get_arrow_key_move_delta(input_state, player_move_delta);

	const real32 player_speed_meters_per_second = 2.0f;
	player_move_delta *= player_speed_meters_per_second * dt;
	state_data->player.move(player_move_delta);


#ifdef CONFIG_DEBUG
	engine_render_draw_rect(
		state_data->camera.world_rect_to_screen_space(state_data->player.get_collision_rect()),
		k_color_red,
		false,
		render_layer_debug);

	for (const auto& object : state_data->scene_objects)
	{
		engine_render_draw_rect(
			state_data->camera.world_rect_to_screen_space(object.get_collision_rect()),
			k_color_blue,
			false,
			render_layer_debug);
	}

	draw_debug_world_grid(20, state_data->camera);

	const int32 align_x = 1170;
	{
		t_string_128 player_position_string;
		player_position_string.printf("Player: {f6.2}, {f6.2}, {f6.2}",
			state_data->player.m_transform.position.x(),
			state_data->player.m_transform.position.y(),
			state_data->player.m_transform.position.z());

		engine_render_draw_string(player_position_string, align_x, 5, 1, k_color_white, render_layer_debug);
	}
	{
		t_string_128 camera_position_string;
		camera_position_string.printf("Camera: {f6.2}, {f6.2}, {f6.2}",
			state_data->camera.get_transform().position.x(),
			state_data->camera.get_transform().position.y(),
			state_data->camera.get_transform().position.z());

		engine_render_draw_string(camera_position_string, align_x, 15, 1, k_color_white, render_layer_debug);
	}
	{
		t_string_128 camera_viewport_title("Camera Viewport");
		engine_render_draw_string(camera_viewport_title, align_x, 25, 1, k_color_white, render_layer_debug);
		const int32 view_align_x = align_x + 65;
		t_string_128 camera_viewport_string;
		state_data->camera.get_viewport_debug_string(camera_viewport_string);
		engine_render_draw_string(camera_viewport_string, view_align_x, 35, 1, k_color_white, render_layer_debug);
	}
#endif // CONFIG_DEBUG
}

void c_game_flow_state_gameplay::on_exit(s_flow_state_gameplay* state_data, real32 dt, bool& out_continue)
{
	out_continue = false;
}