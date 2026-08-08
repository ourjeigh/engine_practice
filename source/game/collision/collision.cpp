#include "collision.h"

bool ray_aabb_intersect_test_2d(
	const t_vector_2d_real32& origin,
	const t_vector_2d_real32& direction,
	const t_aabb_2d_real32& static_aabb,
	s_collision_info& out_collision_info)
{
	const real32 top_y = static_aabb.max.y();
	const real32 bottom_y = static_aabb.min.y();
	const real32 left_x = static_aabb.min.x();
	const real32 right_x = static_aabb.max.x();

	real32 t_min = 1.0f;

	{
		// top/bottom
		real32 t_top = safe_divide(top_y - origin.y(), direction.y(), k_real32_max);
		real32 t_bottom = safe_divide(bottom_y - origin.y(), direction.y(), k_real32_max);
		real32 t = math_min(t_top, t_bottom);
		if (in_range_inc_l_exc_r(0.0f, t_min, t))
		{
			t_vector_2d_real32 collision_position = origin + (direction * t);
			if (in_range_inclusive(left_x, right_x, collision_position.x()))
			{
				t_min = t;
			}
		}
	}
	{
		// left/right
		real32 t_left = safe_divide(left_x - origin.x(), direction.x(), k_real32_max);
		real32 t_right = safe_divide(right_x - origin.x(), direction.x(), k_real32_max);
		real32 t = math_min(t_left, t_right);
		if (in_range_inc_l_exc_r(0.0f, t_min, t))
		{
			t_vector_2d_real32 collision_position = origin + (direction * t);
			if (in_range_inclusive(bottom_y, top_y, collision_position.y()))
			{
				t_min = t;
			}
		}
	}

	bool collides = false;
	if (in_range_inc_l_exc_r(0.0f, 1.0f, t_min))
	{
		collides = true;
		t_vector_2d_real32 hit_position = origin + (direction * t_min);
		out_collision_info.position.set(hit_position.x(), hit_position.y(), 0, 1);
		out_collision_info.t = t_min;
	}

	return collides;
}

bool aabb_intersect_aabb_test_2d(
	const t_vector_2d_real32& origin,
	const t_vector_2d_real32& direction,
	const t_aabb_2d_real32& moving_aabb,
	const t_aabb_2d_real32& static_aabb,
	s_collision_info& out_collision_info)
{
	t_aabb_2d_real32 aabb_sum = static_aabb.add(moving_aabb);
	return ray_aabb_intersect_test_2d(origin, direction, aabb_sum, out_collision_info);
}

bool ray_intersect_aabb_test_3d(
	const t_vector_4d_real32& origin,
	const t_vector_4d_real32& direction,
	const t_aabb_3d_real32& static_aabb,
	s_collision_info& out_collision_info)
{
	const real32 bottom_y = static_aabb.min.y();
	const real32 top_y = static_aabb.max.y();
	const real32 left_x = static_aabb.min.x();
	const real32 right_x = static_aabb.max.x();
	const real32 front_z = static_aabb.min.z();
	const real32 back_z = static_aabb.max.z();
	
	real32 t_min = 1.0f;
	t_vector_4d_real32 normal = t_vector_4d_real32::zero();

	{
		// top/bottom
		real32 t_top = safe_divide(top_y - origin.y(), direction.y(), k_real32_max);
		real32 t_bottom = safe_divide(bottom_y - origin.y(), direction.y(), k_real32_max);
		real32 t = math_min(t_top, t_bottom);
		if (in_range_inc_l_exc_r(0.0f, t_min, t))
		{
			t_vector_4d_real32 collision_position = origin + (direction * t);
			if (in_range_inclusive(left_x, right_x, collision_position.x()))
			{
				t_min = t;
				normal = t == t_top ? k_vector_4d_direction_up : k_vector_4d_direction_down;
			}
		}
	}
	{
		// left/right
		real32 t_left = safe_divide(left_x - origin.x(), direction.x(), k_real32_max);
		real32 t_right = safe_divide(right_x - origin.x(), direction.x(), k_real32_max);
		real32 t = math_min(t_left, t_right);
		if (in_range_inc_l_exc_r(0.0f, t_min, t))
		{
			t_vector_4d_real32 collision_position = origin + (direction * t);
			if (in_range_inclusive(bottom_y, top_y, collision_position.y()))
			{
				t_min = t;
				normal = t == t_left ? k_vector_4d_direction_left : k_vector_4d_direction_right;
			}
		}
	}
	{
		// front/back
		real32 t_back = safe_divide(back_z - origin.z(), direction.z(), k_real32_max);
		real32 t_front = safe_divide(front_z - origin.z(), direction.z(), k_real32_max);
		real32 t = math_min(t_back, t_front);
		if (in_range_inc_l_exc_r(0.0f, t_min, t))
		{
			t_vector_4d_real32 collision_position = origin + (direction * t);
			if (in_range_inclusive(front_z, back_z, collision_position.z()))
			{
				t_min = t;
				normal = t == t_front ? k_vector_4d_direction_forward : k_vector_4d_direction_backward;
			}
		}
	}

	if (in_range_inc_l_exc_r(0.0f, 1.0f, t_min))
	{
		out_collision_info.position = origin + (direction * t_min);
		out_collision_info.t = t_min;
		out_collision_info.normal = normal;
		return true;
	}

	return false;
}

bool aabb_intersect_aabb_test_3d(
	const t_vector_4d_real32& origin,
	const t_vector_4d_real32& direction,
	const t_aabb_3d_real32& moving_aabb,
	const t_aabb_3d_real32& static_aabb,
	s_collision_info& out_collision_info)
{
	t_aabb_3d_real32 aabb_sum = static_aabb.grow_by_other(moving_aabb);
	bool collision = ray_intersect_aabb_test_3d(origin, direction, aabb_sum, out_collision_info);
	
	if (collision)
	{
		auto blah = out_collision_info.position - out_collision_info.normal;
		out_collision_info.position = moving_aabb.get_closest_point_on_bounds(blah);
	}
	
	return collision;
}

bool ray_intersect_plane_test_3d(
	const t_vector_4d_real32& origin,
	const t_vector_4d_real32& direction,
	const t_plane_3d_real32& plane,
	s_collision_info& out_collision_info)
{
	ASSERT(direction.w() == 0);
	if (direction.is_zero())
	{
		return false;
	}

	t_vector_4d_real32 plane_normal = plane.normal();
	real32 denom = direction.dot(plane_normal);

	if (math_abs(denom) < 0.001f)
	{
		return false;
	}

	real32 t = (plane.d() + origin.dot(plane_normal)) / -denom;

	if (in_range_inclusive<real32>(0, 1, t))
	{
		out_collision_info.position = origin + (direction * t);
		out_collision_info.t = t;
		out_collision_info.normal = denom < 0 ? plane_normal : plane_normal.flip();
		return true;
	}
	return false;
}

bool aabb_intersect_plane_test_3d(
	const t_vector_4d_real32& origin,
	const t_vector_4d_real32& direction,
	const t_aabb_3d_real32& aabb,
	const t_plane_3d_real32& plane,
	s_collision_info& out_collision_info)
{
	ASSERT(direction.w() == 0);
	if (direction.is_zero())
	{
		return false;
	}

	c_static_array<t_vector_4d_real32, 8> extents;
	aabb.get_extents(extents);

	real32 t_min = k_real32_max;

	for (auto& extent : extents)
	{
		s_collision_info collision_info;
		if (ray_intersect_plane_test_3d(extent, direction, plane, collision_info) &&
			collision_info.t < t_min)
		{
			out_collision_info = collision_info;
			t_min = collision_info.t;
		}
	}

	return t_min <= 1.0;
}