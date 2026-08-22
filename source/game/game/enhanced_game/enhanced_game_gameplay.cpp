#include "enhanced_game_state_machine.h"


void c_game_flow_state_gameplay::on_enter(s_flow_state_gameplay* state_data, real32 dt, bool& out_continue)
{
	state_data->player.m_transform.reset();
	state_data->player.m_transform.position.y() = 2;
	state_data->player.m_transform.scale *= 0.5f;
	
	state_data->camera.set_transform(s_transform::default_values());
	state_data->camera.set_position({ 0, 2, 0, 1 });
	state_data->camera.set_rotation(k_vector_4d_rotation_forward);
	state_data->camera.set_zoom(1.0f);
	state_data->camera.set_width(10.0f);
	state_data->camera.set_screen_dimensions(engine_get_screen_dimensions());

	out_continue = false;
}





void c_game_flow_state_gameplay::on_update(s_flow_state_gameplay* state_data, real32 dt, bool& out_continue)
{
	engine_render_fill_screen(k_color_black);

	t_string_128 title("Gameplay!");
	engine_render_draw_string(title, 600, 300, 5, k_color_white, render_layer_ui);
}

void c_game_flow_state_gameplay::on_exit(s_flow_state_gameplay* state_data, real32 dt, bool& out_continue)
{
	out_continue = false;
}