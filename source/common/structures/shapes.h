#ifndef __SHAPES_H__
#define __SHAPES_H__
#pragma once

#include "types/types.h"
#include "structures/vector.h"

template<typename t_type, typename t_scalar, t_type k_default_epsilon>
struct s_aabb_2d
{
	c_vector_2d<t_type, t_scalar, k_default_epsilon> min;
	c_vector_2d<t_type, t_scalar, k_default_epsilon> max;

	s_aabb_2d<t_type, t_scalar, k_default_epsilon> add(const s_aabb_2d<t_type, t_scalar, k_default_epsilon>& other) const
	{
		s_aabb_2d out = *this;
		real32 other_half_width = other.width() * 0.5f;
		real32 other_half_height = other.height() * 0.5;
		out.min.x() -= other_half_width;
		out.min.y() -= other_half_height;
		out.max.x() += other_half_width;
		out.max.y() += other_half_height;
		return out;
	}

	bool is_valid() const
	{
		return max.x() > min.x() && max.y() > min.y();
	}

	real32 width() const { return max.x() - min.x(); }
	real32 height() const { return max.y() - min.y(); }

	bool contains_point(const c_vector_2d<t_type, t_scalar, k_default_epsilon>& point) const
	{
		ASSERT(is_valid());
		return
			in_range_inclusive<t_type>(min.x(), max.x(), point.x()) &&
			in_range_inclusive<t_type>(min.y(), max.y(), point.y());
	}

	bool contains_plane_x(t_type y) const
	{
		ASSERT(is_valid());
		return in_range_inclusive<t_type>(min.y(), max.y(), y);
	}

	bool contains_plane_y(t_type x) const
	{
		ASSERT(is_valid());
		return in_range_inclusive<t_type>(min.x(), max.x(), x);
	}

	bool overlaps_other(const s_aabb_2d<t_type, t_scalar, k_default_epsilon>& other) const
	{
		ASSERT(is_valid());
		ASSERT(other.is_valid());
		return
			max.x() >= other.min.x() && other.max.x() >= min.x() &&
			max.y() >= other.min.y() && other.max.y() >= min.y();
	}
};

using t_aabb_2d_real32 = s_aabb_2d<real32, real32, k_default_epsilon_real32>;
using t_aabb_2d_int32 = s_aabb_2d<int32, real32, k_default_epsilon_int32>;

template<typename t_type, typename t_scalar, t_type k_default_epsilon>
struct s_rect_2d
{
	t_type x;
	t_type y;
	t_type width;
	t_type height;

	void set(t_type x, t_type y, t_type width, t_type height)
	{
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
	}

	c_vector_2d<t_type, t_scalar, k_default_epsilon> xy() const
	{
		return c_vector_2d<t_type, t_scalar, k_default_epsilon>(x, y);
	}

	c_vector_2d<t_type, t_scalar, k_default_epsilon> wh() const
	{
		return c_vector_2d<t_type, t_scalar, k_default_epsilon>(width, height);
	}

	c_vector_2d<t_type, t_scalar, k_default_epsilon> center() const
	{
		c_vector_2d<t_type, t_scalar, k_default_epsilon> out;
		out.x() = x + width * 0.5f;
		out.y() = y + height * 0.5f;
		return out;
	}

	s_aabb_2d<t_type, t_scalar, k_default_epsilon> to_aabb() const
	{
		s_aabb_2d<t_type, t_scalar, k_default_epsilon> out;
		out.min.set(x, y);
		out.max.set(x + width, y + height);
		return out;
	}
};

using t_rect_2d_real32 = s_rect_2d<real32, real32, k_default_epsilon_real32>;
using t_rect_2d_int32 = s_rect_2d<int32, real32, k_default_epsilon_int32>;

template<typename t_type, typename t_scalar, t_type k_default_epsilon>
struct s_plane_3d
{
	using t_vector_4d = c_vector_4d<t_type, t_scalar, k_default_epsilon>;

	t_vector_4d abcd;

	t_type& a() { return abcd.x(); }
	t_type& b() { return abcd.y(); }
	t_type& c() { return abcd.z(); }
	t_type& d() { return abcd.w(); }

	const t_type& a() const { return abcd.x(); }
	const t_type& b() const { return abcd.y(); }
	const t_type& c() const { return abcd.z(); }
	const t_type& d() const { return abcd.w(); }

	static_member_function s_plane_3d from_normal_and_distance(const t_vector_4d& normal, t_type distance)
	{
		s_plane_3d out;
		out.abcd = normal;
		out.d() = distance;
		return out;
	}

	static_member_function s_plane_3d from_point_and_normal(const t_vector_4d& point, const t_vector_4d& normal)
	{
		t_vector_4d normalized = normal.normal();
		s_plane_3d out { normalized };
		out.d() = -1 * point.dot(normalized);
		return out;
	}

	t_vector_4d normal() const
	{
		t_vector_4d out = abcd;
		out.w() = 0;
		return out;
	}

	t_type distance_from_origin() { return abcd.w(); }

	t_type distance_to_point_signed(t_vector_4d& point) const
	{
		return a() * point.x() + b() * point.y() + c() * point.z() + d();
	}

	t_type distance_to_point_unsigned(t_vector_4d& point) const
	{
		return math_abs(distance_to_point_signed);
	}

	t_vector_4d closest_point_on_plane(t_vector_4d& point) const
	{
		t_vector_4d out = point - (normal() * distance_to_point_signed(point));
		return out;
	}
};

using t_plane_3d_real32 = s_plane_3d<real32, real32, k_default_epsilon_real32>;
using t_plane_3d_int32 = s_plane_3d<int32, real32, k_default_epsilon_int32>;

template<typename t_type, typename t_scalar, t_type k_default_epsilon>
struct s_aabb_3d
{
	using t_vector_4d = c_vector_4d<t_type, t_scalar, k_default_epsilon>;

	t_vector_4d min;
	t_vector_4d max;

	s_aabb_3d grow_by_other(const s_aabb_3d& other) const
	{
		s_aabb_3d out = *this;
		t_type other_half_width = other.width() * 0.5f;
		t_type other_half_height = other.height() * 0.5f;
		t_type other_half_depth = other.depth() * 0.5f;
		out.min.x() -= other_half_width;
		out.min.y() -= other_half_height;
		out.min.z() -= other_half_depth;
		out.max.x() += other_half_width;
		out.max.y() += other_half_height;
		out.max.z() += other_half_depth;
		ASSERT(out.is_valid());
		return out;
	}

	s_aabb_3d minkowski_sum(const s_aabb_3d& other) const
	{
		s_aabb_3d out;
		out.min = min + other.min;
		out.max = max + other.max;
		out.min.w() = 1;
		out.max.w() = 1;
		ASSERT(out.is_valid());
		return out;
	}

	s_aabb_3d minkowski_difference(const s_aabb_3d& other) const
	{
		s_aabb_3d out;
		out.min = min - other.max;
		out.max = max - other.min;
		out.min.w() = 1;
		out.max.w() = 1;
		ASSERT(out.is_valid());
		return out;
	}

	bool is_valid() const
	{
		return max.x() > min.x() && max.y() > min.y() && max.z() >= min.z() && min.w() == 1 && max.w() == 1;
	}

	t_type width() const { return max.x() - min.x(); }
	t_type height() const { return max.y() - min.y(); }
	t_type depth() const { return max.z() - min.z(); }

	t_vector_4d center() const
	{
		t_vector_4d out = min;
		out += half_extents();
		return out;
	}

	t_scalar radius() const
	{
		return math_max<t_scalar>(math_max<t_scalar>(width() * 0.5f, height() * 0.5f), depth() * 0.5f);
	}

	t_vector_4d half_extents() const
	{
		return { width() * 0.5f, height() * 0.5f, depth() * 0.5f, 0 };
	}

	void get_extents(c_array<t_vector_4d> out_extents) const
	{
		ASSERT(out_extents.capacity() >= 8);

		out_extents[0] = min;
		out_extents[1] = { min.x(), max.y(), min.z(), 1 };
		out_extents[2] = { min.x(), min.y(), max.z(), 1 };
		out_extents[3] = { min.x(), max.y(), max.z(), 1 };
		out_extents[4] = { max.x(), min.y(), min.z(), 1 };
		out_extents[5] = { max.x(), max.y(), min.z(), 1 };
		out_extents[6] = { max.x(), min.y(), max.z(), 1 };
		out_extents[7] = max;
	}

	bool contains_point(const t_vector_4d& point) const
	{
		ASSERT(is_valid());
		return
			in_range_inclusive<t_type>(min.x(), max.x(), point.x()) &&
			in_range_inclusive<t_type>(min.y(), max.y(), point.y()) &&
			in_range_inclusive<t_type>(min.z(), max.z(), point.z()) ;
	}

	t_vector_4d get_closest_point_on_bounds(t_vector_4d& point) const
	{
		// project the point past the bounds (even if it already is outside)
		t_vector_4d c = center();
		t_vector_4d direction = point - c;
		direction *= radius();
		t_vector_4d projection = c + direction;

		// clamp the projected point to the bounds
		t_type x = math_pin<t_type>(min.x(), max.x(), projection.x());
		t_type y = math_pin<t_type>(min.y(), max.y(), projection.y());
		t_type z = math_pin<t_type>(min.z(), max.z(), projection.z());
		return { x, y, z, 1 };
	}

	bool overlaps_plane_xy_at_z(t_type z) const
	{
		ASSERT(is_valid());
		return in_range_inclusive<t_type>(min.z(), max.z(), z);
	}

	bool overlaps_plane_yz_at_x(t_type x) const
	{
		ASSERT(is_valid());
		return in_range_inclusive<t_type>(min.x(), max.x(), x);
	}

	bool overlaps_plane_xz_at_y(t_type y) const
	{
		ASSERT(is_valid());
		return in_range_inclusive<t_type>(min.y(), max.y(), y);
	}

	bool overlaps_other(const s_aabb_3d& other) const
	{
		ASSERT(is_valid()); 
		ASSERT(other.is_valid());

		return
			max.x() >= other.min.x() && other.max.x() >= min.x() &&
			max.y() >= other.min.y() && other.max.y() >= min.y() &&
			max.z() >= other.min.z() && other.max.z() >= min.z();
	}
};

using t_aabb_3d_real32 = s_aabb_3d<real32, real32, k_default_epsilon_real32>;
using t_aabb_3d_int32 = s_aabb_3d<int32, real32, k_default_epsilon_int32>;

template<typename t_type, typename t_scalar, t_type k_default_epsilon>
struct s_rect_3d
{
	using t_vector_4d = c_vector_4d<t_type, t_scalar, k_default_epsilon>;

	t_type x;
	t_type y;
	t_type z;
	t_type width;
	t_type height;
	t_type depth;

	void set(
		t_type x, t_type y, t_type z,
		t_type width, t_type height, t_type depth)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->width = width;
		this->height = height;
		this->depth = depth;
	}

	t_vector_4d xyz() const
	{
		return t_vector_4d(x, y, z, 1);
	}

	t_vector_4d whd() const
	{
		return t_vector_4d(width, height, depth, 0);
	}

	t_vector_4d center() const
	{
		t_vector_4d out;
		out.x() = x + width * 0.5f;
		out.y() = y + height * 0.5f;
		out.z() = z + depth * 0.5f;
		out.w() = 1;
		return out;
	}

	s_aabb_3d<t_type, t_scalar, k_default_epsilon> to_aabb() const
	{
		s_aabb_3d<t_type, t_scalar, k_default_epsilon> out;
		out.min = xyz();
		out.max = xyz() + whd();
		ASSERT(out.is_valid());
		return out;
	}
};

using t_rect_3d_real32 = s_rect_3d<real32, real32, k_default_epsilon_real32>;
using t_rect_3d_int32 = s_rect_3d<int32, real32, k_default_epsilon_int32>;

template<typename t_type, typename t_scalar, t_type k_default_epsilon>
struct s_sphere_3d
{
	using t_vector_4d = c_vector_4d<t_type, t_scalar, k_default_epsilon>;

	t_vector_4d center;
	t_type radius;

	static_member_function s_sphere_3d from_aabb(s_aabb_3d<t_type, t_scalar, k_default_epsilon>& aabb)
	{
		s_sphere_3d out;
		t_vector_4d half_extents = aabb.half_extents();
		out.center = aabb.center();
		out.radius = math_max(math_max(half_extents.x(), half_extents.y()), half_extents.z());
		return out;
	}

	void enclose_point(t_vector_4d& point)
	{
		radius = math_max(radius, (point - center).magnitude());
	}

	bool contains_point(t_vector_4d& point)
	{
		t_scalar distance_to_point = (point - center).magnitude_squared();
		return distance_to_point <= radius;
	}
};

using t_sphere_3d_real32 = s_sphere_3d<real32, real32, k_default_epsilon_real32>;
using t_sphere_3d_int32 = s_sphere_3d<int32, real32, k_default_epsilon_int32>;

#endif //__SHAPES_H__