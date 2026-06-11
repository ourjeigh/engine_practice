#ifndef __VECTOR_H__
#define __VECTOR_H__
#pragma once

#include "assert.h"
#include "types/types.h"
#include "memory/memory.h"
#include "structures/array.h"

template<typename t_type, typename t_scalar_type>
class c_vector_2d
{
public:
	c_vector_2d() 
	{
		set(0, 0);
	}

	c_vector_2d(t_type x, t_type y)
	{
		set(x, y);
	}

	t_type& x() { return m_x; }
	t_type& y() { return m_y; }

	const t_type& x() const { return m_x; }
	const t_type& y() const { return m_y; }

	void set(t_type x, t_type y)
	{
		m_x = x;
		m_y = y;
	}

	c_vector_2d& zero()
	{
		set(0, 0);
		return *this;
	}

	c_vector_2d operator+(const c_vector_2d& other) const
	{
		c_vector_2d out = *this;
		return out.add(other);
	}

	c_vector_2d operator-(const c_vector_2d& other) const
	{
		c_vector_2d out = *this;
		return out.subtract(other);
	}

	c_vector_2d operator*(const t_type value) const
	{
		c_vector_2d out = *this;
		return out.multiply_scalar(value);
	}

	c_vector_2d operator/(const t_type value) const
	{
		c_vector_2d out = *this;
		return out.divide_scalar(value);
	}

	c_vector_2d operator-() const
	{
		c_vector_2d out = *this;
		out.flip();
		return out;
	}

	c_vector_2d& operator+=(const c_vector_2d& other)
	{
		return add(other);
	}

	c_vector_2d& operator-=(const c_vector_2d& other)
	{
		return subtract(other);
	}

	c_vector_2d& operator*=(const t_type value)
	{
		return multiply_scalar(value);
	}

	c_vector_2d& operator/=(const t_type value)
	{
		return divide_scalar(value);
	}

	bool is_zero(t_type epsilon = 0) const
	{
		return math_abs(m_x) <= epsilon && math_abs(m_y) <= epsilon;
	}

	c_vector_2d& add(const c_vector_2d& other)
	{
		m_x += other.m_x;
		m_y += other.m_y;
		return *this;
	}

	c_vector_2d& subtract(const c_vector_2d& other)
	{
		m_x -= other.m_x;
		m_y -= other.m_y;
		return *this;
	}

	c_vector_2d& multiply_scalar(const t_type value)
	{
		return multiply_scalar_real(value);
	}

	c_vector_2d& divide_scalar(const t_type value)
	{
		ASSERT(value != 0.0f);
		const t_scalar_type inv = 1 / static_cast<t_scalar_type>(value);
		return multiply_scalar_real(inv);
	}

	c_vector_2d& flip()
	{
		return multiply_scalar(-1);
	}

	t_scalar_type magnitude_squared() const
	{
		t_scalar_type x = static_cast<t_scalar_type>(m_x);
		t_scalar_type y = static_cast<t_scalar_type>(m_y);
		t_scalar_type out = (x * x) + (y * y);

		return out;
	}

	t_scalar_type magnitude() const
	{
		return math_sqrt(magnitude_squared());
	}

	c_vector_2d normal() const
	{
		c_vector_2d out = *this;
		out.normalize();
		return out;
	}

	c_vector_2d& normalize()
	{
		if (is_zero())
		{
			return *this;
		}

		return divide_scalar(magnitude());
	}

	t_scalar_type* dot(const c_vector_2d& other)
	{
		t_scalar_type x = static_cast<t_scalar_type>(m_x);
		t_scalar_type y = static_cast<t_scalar_type>(m_y);
		return (x * other.m_x) + (y * other.m_y);
	}

private:
	c_vector_2d& multiply_scalar_real(const t_scalar_type value)
	{
		m_x *= value;
		m_y *= value;

		return *this;
	}

	t_type m_x;
	t_type m_y;
};

using t_vector_2d_int32 = c_vector_2d<int32, real32>;
using t_vector_2d_real32 = c_vector_2d<real32, real32>;
#endif //__VECTOR_H__