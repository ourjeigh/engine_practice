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

		const s_mouse_state* mouse_state = engine_input_get_mouse_state();
		int32 mouse_x = mouse_state->position.x;
		int32 mouse_y = mouse_state->position.y;

		const bool mouse_down = engine_input_get_key_state(input_mouse_left).is_down;
		const uint32 color = mouse_down ?
			k_color_red_uint32 :
			k_color_blue_uint32;

		s_render_shape_rect mouse_box(mouse_x - 25, mouse_y - 25, 50, 50);
		engine_render_draw_line(engine_get_screen_center(), s_render_shape_point(mouse_x, mouse_y), k_color_green_uint32);

		s_render_shape_circle circle;
		circle.center = s_render_shape_point(mouse_x, mouse_y);
		circle.radius = 38;

		if (mouse_down)
		{
			engine_render_draw_rect(mouse_box, color);
			engine_render_draw_circle(circle, color, false);
		}
		else
		{
			engine_render_draw_circle(circle, color, true);
		}

	}
};
