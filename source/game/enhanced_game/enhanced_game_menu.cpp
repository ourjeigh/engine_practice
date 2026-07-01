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
	engine_render_draw_string(title, 200, 200, 4, k_color_white, render_layer_main);

	auto menu_items = make_static_array<t_string_128>({ 
		"NEW GAME",
		"EXIT" });

	if (state_data->show_popup)
	{
		t_render_shape_point center = engine_get_screen_center();
		s_screen_dimensions dimensions = engine_get_screen_dimensions();
		uint32 width = 800;
		uint32 height = 400;
		t_render_shape_rect popup(center.x() - width / 2, center.y() - height / 2, width, height);
		c_color color = k_color_grey;
		color.set_alpha(0.95f);
		engine_render_draw_rect(popup, color, render_layer_ui);

		t_string_128 blurb("Try the new\nAI Enhanced Mode!");
		engine_render_draw_string(blurb, popup.x + 100, popup.y + 20, 3, k_color_white, render_layer_ui);

		t_render_shape_rect ok_rect(center.x() - 30, center.y() - 15, 60, 30);
		engine_render_draw_rect(ok_rect, k_color_black, render_layer_ui);
		
		t_string_128 ok("ok");
		engine_render_draw_string(ok, center.x()-20, center.y()-10, 3, k_color_white, render_layer_ui);

		if (engine_input_get_key_state(input_key_special_esc).is_down)
		{
			state_data->show_popup = false;
		}
		// enter from New Game selection applies to quickly and takes us into gameplay here
		/*else if (engine_input_get_key_state(input_key_special_return).is_down)
		{
			out_continue = false;
		}*/
	}
	else
	{
		menu_items[state_data->selection].append("-");

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
	}

	const int32 x = 225;
	int32 y = 300;
	for (int32 item_index = 0; item_index < menu_items.capacity(); item_index++)
	{
		engine_render_draw_string(menu_items[item_index], x, y, 3, k_color_white, render_layer_main);
		y += 30;
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