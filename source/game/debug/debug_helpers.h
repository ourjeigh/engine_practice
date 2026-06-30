#ifndef __DEBUG_HELPERS_H__
#define __DEBUG_HELPERS_H__
#pragma once

#include "../camera.h"
#include "engine_api.h"

inline void draw_debug_world_grid(int32 size_meters, const c_camera_2d& camera)
{
	const int32 start_x = -size_meters / 2;
	const int32 end_x = size_meters / 2;
	const int32 start_y = -size_meters / 2;
	const int32 end_y = size_meters / 2;

	for (int32 x = start_x; x <= end_x; x++)
	{
		t_vector_4d_real32 vertical_start(x, start_y, 0, 1);
		t_vector_4d_real32 vertical_end(x, end_y, 0, 1);

		engine_render_draw_line(
			camera.world_to_screen_space(vertical_start),
			camera.world_to_screen_space(vertical_end),
			k_color_green,
			render_layer_debug);
	}

	for (int32 y = start_y; y <= end_y; y++)
	{
		t_vector_4d_real32 horizontal_start(start_x, y, 0, 1);
		t_vector_4d_real32 horizontal_end(end_x, y, 0, 1);

		engine_render_draw_line(
			camera.world_to_screen_space(horizontal_start),
			camera.world_to_screen_space(horizontal_end),
			k_color_green,
			render_layer_debug);
	}
}

#endif // !__DEBUG_HELPERS_H__
