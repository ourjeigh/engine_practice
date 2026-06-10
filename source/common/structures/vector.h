#ifndef __VECTOR_H__
#define __VECTOR_H__
#pragma once

#include "assert.h"
#include "types/types.h"
#include "memory/memory.h"
#include "structures/array.h"

enum e_vector_coordinates
{
	vector_coordinate_x,
	vector_coordinate_y,
	vector_coordinate_z,
	vector_coordinate_w,

	k_vector_coordinate_count,
};

// we'll likely want to specialize all these implementations for speed later
// but for now this makes standing up different types and testing them easy.
template<typename t_type, int32 k_dimensions>
class c_vector_base
{
public:
	c_vector_base() : m_data() {}
	
	c_vector_base& operator+(const c_vector_base& other)
	{
		return add(other);
	}

	c_vector_base& operator-(const c_vector_base& other)
	{
		return subtract(other);
	}

	c_vector_base& operator*(const t_type value)
	{
		return multiply_scalar(value);
	}
	
	c_vector_base& operator/(const t_type value)
	{
		return divide_scalar(value);
	}

	c_vector_base& operator-()
	{
		return flip();
	}

	void invalidate()
	{
		memory_set(m_data.data(), k_invalid, sizeof(t_type) * m_data.capacity());
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

	bool is_unit() const
	{
		HALT_UNIMPLEMENTED();
	}

	c_vector_base get_unit() const
	{
		c_vector_base out;
		HALT_UNIMPLEMENTED();
		return out;
	}



	c_vector_base& add(const c_vector_base& other)
	{
		ASSERT(m_data.capacity() == other.m_data.capacity());

		for (int32 i = 0; i < m_data.capacity(); i++)
		{
			m_data[i] += other.m_data[i];
		}

		return *this;
	}

	c_vector_base& subtract(const c_vector_base& other)
	{
		ASSERT(m_data.capacity() == other.m_data.capacity());

		for (int32 i = 0; i < m_data.capacity(); i++)
		{
			m_data[i] -= other.m_data[i];
		}

		return *this;
	}

	c_vector_base& multiply_scalar(const t_type value)
	{
		return multiply_scalar_real(value);
	}

	c_vector_base& multiply_scalar_real(const real64 value)
	{
		for (int32 i = 0; i < m_data.capacity(); i++)
		{
			m_data[i] *= value;
		}

		return *this;
	}

	c_vector_base& divide_scalar(const t_type value)
	{
		const real64 inv = 1 / static_cast<real64>(value);
		return multiply_scalar_real(inv);
	}

	c_vector_base& flip()
	{
		return multiply_scalar(-1);
	}

protected:
	c_static_array<t_type, k_dimensions> m_data;
};

template<typename t_type>
class c_vector_2d : public c_vector_base<t_type, 2>
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

template<typename t_type>
class c_vector_3d : public c_vector_base<t_type, 3>
{
public:
	c_vector_3d() { this->invalidate(); }
	c_vector_3d(t_type x, t_type y) : c_vector_base<t_type, 3>()
	{
		set(x, y, z);
	}

	t_type& x() { return this->m_data[vector_coordinate_x]; }
	t_type& y() { return this->m_data[vector_coordinate_y]; }
	t_type& z() { return this->m_data[vector_coordinate_z]; }

	void set(t_type x, t_type y, t_type z)
	{
		this->m_data[vector_coordinate_x] = x;
		this->m_data[vector_coordinate_y] = y;
		this->m_data[vector_coordinate_z] = z;
	}
};

using t_vector_2d_int32 = c_vector_2d<int32>;
using t_vector_2d_real32 = c_vector_2d<real32>;
#endif //__VECTOR_H__