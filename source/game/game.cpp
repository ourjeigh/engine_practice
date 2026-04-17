#include "game_api.h"
#include "engine_api.h"
#include "types/render_types.h"

class c_game_test : public c_game_base
{
public:
	void init()
	{
		engine_log();
	}

	void term() {}

	void update() 
	{
		engine_render_fill_screen(0xFF202020);

		s_render_shape_circle circle;
		circle.center = engine_get_screen_center();
		circle.radius = 50;

		engine_render_draw_circle(circle, k_color_red_uint32, true);

		/*const c_mouse_state* mouse_state = input_system_get_mouse_state();
		int32 mouse_x = mouse_state->position.x;
		int32 mouse_y = mouse_state->position.y;

		const bool mouse_down = input_system_get_key_state(input_mouse_left)->is_down();
		const uint32 color = mouse_down ?
			k_color_red_uint32 :
			k_color_blue_uint32;

		s_render_shape_rect mouse_box(mouse_x - 25, mouse_y - 25, 50, 50);
		draw_line(get_screen_center(), s_render_shape_point(mouse_x, mouse_y), k_color_green_uint32);

		s_render_shape_circle circle;
		circle.center = s_render_shape_point(mouse_x, mouse_y);
		circle.radius = 38;

		if (mouse_down)
		{
			draw_rect(mouse_box, color);
			draw_circle(circle, color, false);
		}
		else
		{
			draw_circle(circle, color, true);
		}*/

	}
};
