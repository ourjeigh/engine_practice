#ifndef __CAMERA_H__
#define __CAMERA_H__
#pragma once

#include "config.h"
#include "mmath.h"
#include "structures/matrix.h"
#include "structures/vector.h"
#include "structures/shapes.h"
#include "structures/string/string.h"
#include "types/render_types.h"
#include "transform.h"

class c_camera_2d
{
public:
	const s_transform& get_transform() { return m_transform; }
	const s_screen_dimensions& get_screen_dimensions() { return m_screen_dimensions; }
	const s_matrix_4x4_real32& get_viewport() { return m_viewport; }
	real32 get_zoom() { return m_zoom; }
	real32 get_width() { return m_width; }

#ifdef CONFIG_DEBUG
	void get_viewport_debug_string(c_string& out_string)
	{
		out_string.printf(
			"{f6.2}, {f6.2}, {f6.2}, {f6.2}\n"
			"{f6.2}, {f6.2}, {f6.2}, {f6.2}\n"
			"{f6.2}, {f6.2}, {f6.2}, {f6.2}\n"
			"{f6.2}, {f6.2}, {f6.2}, {f6.2}",
			m_viewport[0][0], m_viewport[0][1], m_viewport[0][2], m_viewport[0][3],
			m_viewport[1][0], m_viewport[1][1], m_viewport[1][2], m_viewport[1][3],
			m_viewport[2][0], m_viewport[2][1], m_viewport[2][2], m_viewport[2][3],
			m_viewport[3][0], m_viewport[3][1], m_viewport[3][2], m_viewport[3][3]);
	}
#endif // CONFIG_DEBUG

	void set_transform(const s_transform& transform)
	{
		// we can only support z-axis rotation until we are ready for much more complicated rendering
		ASSERT(transform.rotation.x() == 0.0f && transform.rotation.y() == 0.0f);
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

	t_render_shape_point world_to_screen_space(const t_vector_4d_real32& position) const
	{
		t_render_shape_point out;
		t_vector_4d_real32 world_space = position * m_viewport;
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

		s_matrix_4x4_real32 translation =
		{{
			{1, 0, 0, 0},
			{0, 1, 0, 0},
			{0, 0, 1, 0},
			{-m_transform.position.x(), -m_transform.position.y(), -m_transform.position.z(), 1 }
		}};

		real32 sin_z = math_sin(m_transform.rotation.z());
		real32 cos_z = math_cos(m_transform.rotation.z());
		s_matrix_4x4_real32 rotation =
		{{
			{cos_z, sin_z, 0, 0},
			{-sin_z, cos_z, 0, 0},
			{0, 0, 1, 0},
			{0, 0, 0, 1},
		}};

		s_matrix_4x4_real32 view_matrix = translation * rotation;

		real32 view_ratio = m_screen_dimensions.width / m_screen_dimensions.height;
		real32 world_width = m_width;
		real32 world_height = world_width / view_ratio;

		real32 left = -world_width / 2;
		real32 right = world_width / 2;
		real32 top = world_height / 2;
		real32 bottom = -world_height / 2;
		real32 near = -1;
		real32 far = 1;

		s_matrix_4x4_real32 projection_matrix =
		{{
			{2/(right - left), 0, 0, 0},
			{0, 2/(top - bottom), 0 , 0},
			{0, 0, -2/(far - near), 0},
			{-(right + left) / (right - left), -(top + bottom) / (top - bottom), -(far + near) / (far - near), 1}
		}};

		real32 screen_width = m_screen_dimensions.width;
		real32 screen_height = m_screen_dimensions.height;

		s_matrix_4x4_real32 viewport_matrix =
		{{
			{(screen_width / 2), 0, 0, 0},
			{0, (-screen_height / 2), 0, 0},
			{0, 0, 1, 0},
			{(screen_width / 2), (screen_height / 2), 0, 1}
		}};

		m_viewport = view_matrix * projection_matrix * viewport_matrix;
	}

	s_transform m_transform;
	real32 m_zoom;
	real32 m_width;
	s_screen_dimensions m_screen_dimensions;
	s_matrix_4x4_real32 m_viewport;
};

#endif //__CAMERA_H__