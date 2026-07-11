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

	bool contains_point(const t_vector_4d& point) const
	{
		ASSERT(is_valid());
		return
			in_range_inclusive<t_type>(min.x(), max.x(), point.x()) &&
			in_range_inclusive<t_type>(min.y(), max.y(), point.y()) &&
			in_range_inclusive<t_type>(min.z(), max.z(), point.z()) ;
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

#endif //__SHAPES_H__