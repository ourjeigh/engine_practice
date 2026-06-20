#ifndef __CAMERA_H__
#define __CAMERA_H__
#pragma once

#include "structures/vector.h"
#include "structures/shapes.h"
#include "types/render_types.h"

class c_camera_2d
{
public:
	t_vector_4d_real32 m_position;
	// it feels like we could get rid of this, but for now it helps with the screen space conversion
	t_rect_2d_real32 m_bounds;
	t_render_shape_rect m_render_space;
	real32 m_zoom = 1.0f;

	t_render_shape_point world_to_screen_space(const t_vector_4d_real32& position) const
	{
		t_vector_2d_real32 world_xy = position.xy();
		t_vector_2d_real32 cam_xy = m_position.xy();
		t_vector_2d_real32 view = world_xy - cam_xy;
		t_vector_2d_real32 projected = view * m_zoom;
		t_vector_2d_real32 screen_position = projected + (m_bounds.wh() * 0.5f);

		t_render_shape_point out(screen_position.x(), screen_position.y());
		return out;
	}
};

#endif //__CAMERA_H__