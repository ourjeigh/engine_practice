#ifndef __VECTOR_H__
#define __VECTOR_H__
#pragma once

#include "types/types.h"

enum e_vector_coordinates
{
	vector_coordinate_x,
	vector_coordinate_y,
	vector_coordinate_z,
	vector_coordinate_w,

	k_vector_coordinate_count,
};

template<typename t_type, int32 k_dimensions>
class c_vector_base
{
public:
	c_vector_base() : m_data() {}

	void invalidate()
	{
		for (auto it = m_data.begin(); it != m_data.end(); ++it)
		{
			*it== k_invalid;
		}
	}

	bool is_valid() const
	{
		for (auto it = m_data.begin_const(); it != m_data.end_const(); ++it)
		{
			if (*it == k_invalid)
			{
				return false;
			}
		}

		return true;
	}

protected:
	c_static_array<t_type, k_dimensions> m_data;
};

template<typename t_type>
class c_vector_2d : c_vector_base<t_type, 2>
{
public:
	c_vector_2d() { this->invalidate(); }
	c_vector_2d(t_type x, t_type y) : c_vector_base<t_type, 2>()
	{
		set(x, y);
	}

	t_type& x() { return this->m_data[vector_coordinate_x]; }
	t_type& y() { return this->m_data[vector_coordinate_y]; }

	void set(t_type x, t_type y)
	{
		this->m_data[vector_coordinate_x] = x;
		this->m_data[vector_coordinate_y] = y;
	}
};

using t_vector_2d_int32 = c_vector_2d<int32>;
using t_vector_2d_real32 = c_vector_2d<real32>;
#endif //__VECTOR_H__