#ifndef __SHAPES_H__
#define __SHAPES_H__
#pragma once

#include "types/types.h"
#include "structures/vector.h"

template<typename t_type, typename t_scalar, t_type k_default_epsilon>
struct s_aabb_2d
{
	c_vector_2d<t_type, t_scalar, k_default_epsilon> bottom_left;
	c_vector_2d<t_type, t_scalar, k_default_epsilon> top_right;

	bool contains_point(const c_vector_2d<t_type, t_scalar, k_default_epsilon>& point) const
	{
		return
			in_range_inclusive<t_type>(bottom_left.x(), top_right.x(), point.x()) &&
			in_range_inclusive<t_type>(bottom_left.y(), top_right.y(), point.y());
	}

	bool contains_plane_x(t_type y) const
	{
		return in_range_inclusive<t_type>(bottom_left.y(), top_right.y(), y);
	}

	bool contains_plane_y(t_type x) const
	{
		return in_range_inclusive<t_type>(bottom_left.x(), top_right.x(), x);
	}

	bool overlaps_other(const s_aabb_2d<t_type, t_scalar, k_default_epsilon>& other) const
	{
		bool contains_x = contains_plane_x(other.bottom_left.y()) || contains_plane_x(other.top_right.y());
		bool contains_y = contains_plane_x(other.bottom_left.x()) || contains_plane_x(other.top_right.x());
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
		out.bottom_left.set(x, y);
		out.top_right.set(x + width, y + height);
		return out;
	}
};

using t_rect_2d_real32 = s_rect_2d<real32, real32, k_default_epsilon_real32>;
using t_rect_2d_int32 = s_rect_2d<int32, real32, k_default_epsilon_int32>;

#endif //__SHAPES_H__