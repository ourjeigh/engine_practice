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
	real32 get_width() { return m_world_width; }

	bool is_valid()
	{
		return m_screen_dimensions.width > 0 && m_screen_dimensions.height > 0 && m_world_width > 0;
	}

	void set_transform(const s_transform& transform)
	{
		m_transform = transform;
		update_view_matrix();
	}

	void set_position(const t_vector_4d_real32 position)
	{
		m_transform.position = position;
		update_view_matrix();
	}

	void set_rotation(const t_vector_4d_real32 rotation)
	{
		m_transform.rotation = rotation;
		update_view_matrix();
	}

	void set_scale(const t_vector_4d_real32 scale)
	{
		m_transform.scale = scale;
		update_view_matrix();
	}

	void set_screen_dimensions(s_screen_dimensions dimensions)
	{
		m_screen_dimensions = dimensions;
		update_view_matrix();
	}

	void set_width(const real32 width)
	{
		m_world_width = width;
		update_view_matrix();
	}

	void set_zoom(const real32 zoom)
	{
		m_zoom = zoom;
		update_view_matrix();
	}

	t_render_shape_point world_position_to_screen_space(const t_vector_4d_real32& position) const
	{
		t_render_shape_point out;
		t_vector_4d_real32 world_space = position * m_viewport;
		out.set(world_space.x(), world_space.y());
		return out;
	}

	// remove
	t_render_shape_rect world_rect_to_screen_space_2d(const t_rect_2d_real32& rect) const
	{
		// converting from world to screen space involves flipping top/bottom
		t_vector_4d_real32 world_top_left(rect.x, rect.y , 0, 1);
		t_vector_4d_real32 world_bottom_right(rect.x + rect.width, rect.y + rect.height, 0, 1);

		t_render_shape_point screen_top_left = world_position_to_screen_space(world_top_left);
		t_render_shape_point screen_bottom_right = world_position_to_screen_space(world_bottom_right);

		int32 width = screen_bottom_right.x() - screen_top_left.x();
		int32 height = screen_top_left.y() - screen_bottom_right.y();

		t_render_shape_rect out(screen_top_left.x(), screen_bottom_right.y(), width, height);
		return out;
	}
	
	t_render_shape_rect world_rect_to_screen_space(const t_rect_3d_real32& rect) const
	{
		t_aabb_3d_real32 aabb = rect.to_aabb();
		t_render_shape_point min = world_position_to_screen_space(aabb.min);
		t_render_shape_point max = world_position_to_screen_space(aabb.max);

		int32 left = min.x();
		int32 top = min.y();
		int32 height = max.y() - min.y();
		int32 width = max.x() - min.x();

		if (min.x() > max.x())
		{
			left = max.x();
			width *= -1;
		}

		if (min.y() > max.y())
		{
			top = max.y();
			height *= -1;
		}

		t_render_shape_rect out(left, top, width, height);
		return out;
	}

	s_render_shape_line world_plane_to_screen_space(const t_plane_3d_real32& plane) const
	{
		// this would just fill the whole screen
		ASSERT(plane.abcd.z() == 0);

		const real32 view_ratio = m_screen_dimensions.width / m_screen_dimensions.height;
		const real32 world_width = m_world_width;
		const real32 world_height = world_width / view_ratio;

		const real32 left = m_transform.position.x() - world_width / 2;
		const real32 right = m_transform.position.x() + world_width / 2;
		const real32 top = m_transform.position.y() + world_height / 2;
		const real32 bottom = m_transform.position.y() - world_height / 2;

		t_vector_4d_real32 bottom_left = { left, bottom, 0, 1 };
		t_vector_4d_real32 top_right = { right, top, 0, 1 };

		t_vector_4d_real32 p1 = plane.closest_point_on_plane(bottom_left);
		t_vector_4d_real32 p2 = plane.closest_point_on_plane(top_right);

		s_render_shape_line out;
		out.p1 = world_position_to_screen_space(p1);
		out.p2 = world_position_to_screen_space(p2);

		return out;
	}

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

private:
	void update_view_matrix()
	{
		if (!is_valid()) return;

		const real32 pos_x = m_transform.position.x();
		const real32 pos_y = m_transform.position.y();
		const real32 pos_z = m_transform.position.z();
		const real32 rot_sin_x = math_sin(m_transform.rotation.x());
		const real32 rot_cos_x = math_cos(m_transform.rotation.x());
		const real32 rot_sin_y = math_sin(m_transform.rotation.y());
		const real32 rot_cos_y = math_cos(m_transform.rotation.y());
		const real32 rot_sin_z = math_sin(m_transform.rotation.z());
		const real32 rot_cos_z = math_cos(m_transform.rotation.z());

		s_matrix_4x4_real32 rotation_matrix =
		{{
			{ (rot_cos_y * rot_cos_z),  (rot_cos_x * rot_sin_z + rot_sin_x * rot_sin_y * rot_cos_z), (rot_sin_x * rot_sin_z - rot_cos_x * rot_sin_y * rot_cos_z), 0 },
			{ (-rot_cos_y * rot_sin_z), (rot_cos_x * rot_cos_z - rot_sin_x * rot_sin_y * rot_sin_z), (rot_sin_x * rot_cos_z + rot_cos_x * rot_sin_y * rot_sin_z), 0 },
			{ rot_sin_y, (-rot_sin_x * rot_cos_y), (rot_cos_x * rot_cos_y), 0 },
			{ 0, 0, 0, 1 },
		}};
		
		s_matrix_4x4_real32 translation_matrix =
		{{
			{ 1, 0, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 1, 0 },
			{ -pos_x, -pos_y, -pos_z, 1 },
		}};

		s_matrix_4x4_real32 view_matrix = translation_matrix * rotation_matrix;

		const real32 view_ratio = m_screen_dimensions.width / m_screen_dimensions.height;
		const real32 world_width = m_world_width;
		const real32 world_height = world_width / view_ratio;

		const real32 left = -world_width * 0.5f;
		const real32 right = world_width * 0.5f;
		const real32 top = world_height * 0.5f;
		const real32 bottom = -world_height * 0.5f;
		const real32 near = 0;
		const real32 far = 10;

		s_matrix_4x4_real32 projection_matrix =
		{{
			{ 2/(right - left), 0, 0, 0 },
			{ 0, 2/(top - bottom), 0 , 0 },
			{ 0, 0, -2/(far - near), 0 },
			{ -(right + left) / (right - left), -(top + bottom) / (top - bottom), -(far + near) / (far - near), 1 }
		}};

		const real32 half_screen_width = m_screen_dimensions.width * 0.5f;
		const real32 half_screen_height = m_screen_dimensions.height * 0.5f;

		s_matrix_4x4_real32 viewport_matrix =
		{{
			{ half_screen_width, 0, 0, 0 },
			{ 0, -half_screen_height, 0, 0 },
			{ 0, 0, 1, 0 },
			{ half_screen_width, half_screen_height, 0, 1 }
		}};

		m_viewport = view_matrix * projection_matrix * viewport_matrix;
	}

	s_transform m_transform;
	real32 m_zoom;
	real32 m_world_width;
	s_screen_dimensions m_screen_dimensions;
	s_matrix_4x4_real32 m_viewport;
};

#endif //__CAMERA_H__