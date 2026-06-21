#ifndef __SHAPES_H__
#define __SHAPES_H__
#pragma once

#include "types/types.h"
#include "structures/vector.h"

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
};

using t_rect_2d_real32 = s_rect_2d<real32, real32, k_default_epsilon_real32>;
using t_rect_2d_int32 = s_rect_2d<int32, int32, k_default_epsilon_int32>;

#endif //__SHAPES_H__