#include "enhanced_game_state_machine.h"

void c_game_flow_state_main_menu::on_enter(s_flow_state_main_menu* state_data, real32 dt, bool& out_continue)
{
	out_continue = false;
}

void c_game_flow_state_main_menu::on_update(s_flow_state_main_menu* state_data, real32 dt, bool& out_continue)
{
	out_continue = true;

	engine_render_fill_screen(k_color_black.to_uint32());

	t_string_128 title("TITLE");
	engine_render_draw_string(title, 200, 200, 4, k_color_white);

	if (engine_input_get_key_state(input_key_special_return).is_down)
	{
		out_continue = false;
	}
}

void c_game_flow_state_main_menu::on_exit(s_flow_state_main_menu* state_data, real32 dt, bool& out_continue)
{
	out_continue = false;
}