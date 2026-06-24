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
	const s_screen_dimensions& get_screen_dimensions() { return m_screen_dimensions; }
	const s_matrix_3x3_real32& get_viewport() { return m_viewport; }
	real32 get_zoom() { return m_zoom; }
	real32 get_width() { return m_width; }

	void set_transform(const s_transform& transform)
	{
		m_transform = transform;
		update_view_matrix();
	}

	void set_screen_dimensions(s_screen_dimensions dimensions)
	{
		m_screen_dimensions = dimensions;
		update_view_matrix();
	}

	void set_width(const real32 width)
	{
		m_width = width;
		update_view_matrix();
	}

	void set_zoom(const real32 zoom)
	{
		m_zoom = zoom;
		update_view_matrix();
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

	bool is_valid()
	{
		return m_screen_dimensions.width > 0 && m_screen_dimensions.height > 0 && m_width > 0;
	}

private:
	void update_view_matrix()
	{
		if (!is_valid()) return;

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
				{0, 0, 1}
			}
		};

		s_matrix_3x3_real32 view_matrix = translation * rotation;

		real32 view_ratio = m_screen_dimensions.width / m_screen_dimensions.height;
		real32 world_width = m_width;
		real32 world_height = world_width / view_ratio;

		s_matrix_3x3_real32 projection_matrix =
		{
			{
				{(m_zoom * 2 / world_width), 0, 0},
				{0, (m_zoom * 2 / world_height), 0},
				{0, 0, 1}
			}
		};

		real32 screen_width = m_screen_dimensions.width;
		real32 screen_height = m_screen_dimensions.height;

		s_matrix_3x3_real32 viewport_matrix =
		{
			{
				{(screen_width / 2), 0, 0},
				{0, (-screen_height / 2), 0},
				{(screen_width / 2), (screen_height / 2), 1}
			}
		};

		m_viewport = view_matrix * projection_matrix * viewport_matrix;
	}

	s_transform m_transform;
	real32 m_zoom;
	real32 m_width;
	s_screen_dimensions m_screen_dimensions;
	s_matrix_3x3_real32 m_viewport;
};

#endif //__CAMERA_H__