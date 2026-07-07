#ifndef __SHAPES_H__
#define __SHAPES_H__
#pragma once

#include "types/types.h"
#include "structures/vector.h"

template<typename t_type, typename t_scalar, t_type k_default_epsilon>
struct s_aabb_2d
{
	c_vector_2d<t_type, t_scalar, k_default_epsilon> min_xy;
	c_vector_2d<t_type, t_scalar, k_default_epsilon> max_xy;

	s_aabb_2d<t_type, t_scalar, k_default_epsilon> add(const s_aabb_2d<t_type, t_scalar, k_default_epsilon>& other) const
	{
		s_aabb_2d out = *this;
		real32 other_half_width = other.width() * 0.5f;
		real32 other_half_height = other.height() * 0.5;
		out.min_xy.x() -= other_half_width;
		out.min_xy.y() -= other_half_height;
		out.max_xy.x() += other_half_width;
		out.max_xy.y() += other_half_height;
		return out;
	}

	bool is_valid() const
	{
		return max_xy.x() > min_xy.x() && max_xy.y() > min_xy.y();
	}

	real32 width() const { return max_xy.x() - min_xy.x(); }
	real32 height() const { return max_xy.y() - min_xy.y(); }

	bool contains_point(const c_vector_2d<t_type, t_scalar, k_default_epsilon>& point) const
	{
		ASSERT(is_valid());
		return
			in_range_inclusive<t_type>(min_xy.x(), max_xy.x(), point.x()) &&
			in_range_inclusive<t_type>(min_xy.y(), max_xy.y(), point.y());
	}

	bool contains_plane_x(t_type y) const
	{
		ASSERT(is_valid());
		return in_range_inclusive<t_type>(min_xy.y(), max_xy.y(), y);
	}

	bool contains_plane_y(t_type x) const
	{
		ASSERT(is_valid());
		return in_range_inclusive<t_type>(min_xy.x(), max_xy.x(), x);
	}

	bool overlaps_other(const s_aabb_2d<t_type, t_scalar, k_default_epsilon>& other) const
	{
		ASSERT(is_valid());
		bool contains_x = contains_plane_x(other.min_xy.y()) || contains_plane_x(other.max_xy.y());
		bool contains_y = contains_plane_y(other.min_xy.x()) || contains_plane_y(other.max_xy.x());
		return contains_x && contains_y;
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
		out.min_xy.set(x, y);
		out.max_xy.set(x + width, y + height);
		return out;
	}
};

using t_rect_2d_real32 = s_rect_2d<real32, real32, k_default_epsilon_real32>;
using t_rect_2d_int32 = s_rect_2d<int32, real32, k_default_epsilon_int32>;

#endif //__SHAPES_H__