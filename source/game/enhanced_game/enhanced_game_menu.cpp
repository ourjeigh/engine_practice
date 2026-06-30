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

	auto menu_items = make_static_array<t_string_128>({ 
		"NEW GAME",
		"EXIT" });

	menu_items[state_data->selection].append("-");

	const int32 x = 225;
	int32 y = 300;
	for (int32 item_index = 0; item_index < menu_items.capacity(); item_index++)
	{
		engine_render_draw_string(menu_items[item_index], x, y, 3, k_color_white);
		y += 30;
	}

	if (engine_input_get_key_state(input_key_arrow_up).is_down)
	{
		state_data->selection--;
	}
	else if (engine_input_get_key_state(input_key_arrow_down).is_down)
	{
		state_data->selection++;
	}

	if (state_data->selection < 0)
	{
		state_data->selection = main_menu_selection_new_game;
	}
	else if (state_data->selection >= k_main_menu_selection_count)
	{
		state_data->selection = main_menu_selection_exit;
	}

	if (state_data->show_popup)
	{
		t_render_shape_rect popup(300, 200, 300, 300);
		engine_render_draw_rect(popup, k_color_blue);
	}

	if (engine_input_get_key_state(input_key_special_return).is_down)
	{
		switch (state_data->selection)
		{
		case main_menu_selection_new_game:
			state_data->show_popup = true;
			//out_continue = false;
			break;
		case main_menu_selection_exit:
			engine_request_exit();
			break;
		}
	}
}

void c_game_flow_state_main_menu::on_exit(s_flow_state_main_menu* state_data, real32 dt, bool& out_continue)
{
	out_continue = false;
}