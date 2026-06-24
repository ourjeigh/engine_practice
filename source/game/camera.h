#ifndef __CAMERA_H__
#define __CAMERA_H__
#pragma once

#include "structures/vector.h"
#include "structures/shapes.h"
#include "types/render_types.h"
#include "transform.h"
#include "structures/matrix.h"
#include "mmath.h"

class c_camera_2d
{
public:
	const s_transform& get_transform() { return m_transform; }
	const t_render_shape_rect& get_screen_dimensions() { return m_screen_dimensions; }
	const s_matrix_3x3_real32& get_viewport() { return m_viewport; }
	real32 get_zoom() { return m_zoom; }

	void set_transform(const s_transform& transform)
	{
		m_transform = transform;
		update_view_matrix();
	}

	void set_screen_dimensions(real32 width, real32 height)
	{
		m_screen_dimensions.set(0, 0, width, height);
		update_view_matrix();
	}

	void set_zoom(const real32 zoom)
	{
		m_zoom = zoom;
		update_view_matrix();
	}

	t_render_shape_point world_to_screen_space(const t_vector_4d_real32& position) const
	{
		t_vector_2d_real32 view = position.xy() - m_transform.position.xy();
		t_vector_2d_real32 projected = view * m_zoom;
		auto blah = m_screen_dimensions.wh();
		auto blah2 = blah * 0.5f;
		t_vector_2d_real32 screen_position = projected + blah2;
		screen_position.y() = m_screen_dimensions.height - screen_position.y();
		t_render_shape_point out(screen_position.x(), screen_position.y());
		return out;
	}

	t_render_shape_point world_to_screen_space2(const t_vector_4d_real32& position) const
	{
		t_render_shape_point out;
		// temp, once we're using 4x4 matrices z will be real and w will be used to denote a point vector
		t_vector_4d_real32 p_hack = position;
		p_hack.z() = 1;
		t_vector_4d_real32 world_space = m_viewport * p_hack;
		out.set(world_space.x(), world_space.y());
		return out;
	}

private:
	void update_view_matrix()
	{
		s_matrix_3x3_real32 translation =
		{
			{
				{1, 0, 0},
				{0, 1, 0},
				{-m_transform.position.x(), -m_transform.position.y(), 1}
			}
		};

		real32 sin_z = math_sin(m_transform.rotation.z());
		real32 cos_z = math_cos(m_transform.rotation.z());
		s_matrix_3x3_real32 rotation =
		{
			{
				{cos_z, -sin_z, 0},
				{sin_z, cos_z, 0},
				{0,		 0,		1 }
			}
		};

		s_matrix_3x3_real32 view_matrix = translation * rotation;

		real32 width = m_screen_dimensions.width;
		real32 height = m_screen_dimensions.height;

		s_matrix_3x3_real32 projection_matrix =
		{
			{
				{(m_zoom * 2 / width), 					   0, 0},
				{					0, (m_zoom * 2 / height), 0},
				{					0,					   0, 1}
			}
		};

		s_matrix_3x3_real32 viewport_matrix =
		{
			{
				{(width / 2),			  0, 0},
				{		  0,  (-height / 2), 0},
				{(width / 2),  (height / 2), 1}
			}
		};

		m_viewport = view_matrix * projection_matrix * viewport_matrix;
	}

	s_transform m_transform;
	t_render_shape_rect m_screen_dimensions;
	real32 m_zoom = 1.0f;
	s_matrix_3x3_real32 m_viewport;
};

#endif //__CAMERA_H__