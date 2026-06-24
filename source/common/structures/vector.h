#ifndef __VECTOR_H__
#define __VECTOR_H__
#pragma once

#include "debug/asserts.h"
#include "types/types.h"
#include "structures/array.h"

const real32 k_default_epsilon_real32 = 1e-5f;
const int32 k_default_epsilon_int32 = 0;

template<typename t_type, typename t_scalar_type, t_type k_default_epsilon>
class c_vector_2d
{
public:
	c_vector_2d() 
	{
		zero();
	}

	c_vector_2d(t_type x, t_type y)
	{
		set(x, y);
	}

	c_vector_2d(const c_vector_2d& other)
	{
		set(other.x(), other.y());
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

	bool operator==(const c_vector_2d& other) const
	{
		return is_equal(other);
	}

	bool is_equal(const c_vector_2d& other, t_type epsilon = k_default_epsilon) const
	{
		return math_abs(other.m_x - m_x) <= k_default_epsilon && math_abs(other.m_y - m_y) <= k_default_epsilon;
	}

	c_vector_2d& operator=(const c_vector_2d& other)
	{
		set(other.x(), other.y());
		return *this;
	}
	
	template<typename t_other, typename t_other_scalar, t_other other_epsilon>
	c_vector_2d operator+(const c_vector_2d<t_other, t_other_scalar, other_epsilon>& other) const
	{
		c_vector_2d out = *this;
		return out.add(other);
	}

	template<typename t_other, typename t_other_scalar, t_other other_epsilon>
	c_vector_2d operator-(const c_vector_2d<t_other, t_other_scalar, other_epsilon>& other) const
	{
		c_vector_2d out = *this;
		return out.subtract(other);
	}

	c_vector_2d operator*(const t_scalar_type value) const
	{
		c_vector_2d out = *this;
		return out.multiply_scalar(value);
	}

	c_vector_2d operator/(const t_scalar_type value) const
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

	template<typename t_other, typename t_other_scalar, t_other other_epsilon>
	c_vector_2d& operator+=(const c_vector_2d<t_other, t_other_scalar, other_epsilon>& other)
	{
		return add(other);
	}

	template<typename t_other, typename t_other_scalar, t_other other_epsilon>
	c_vector_2d& operator-=(const c_vector_2d<t_other, t_other_scalar, other_epsilon>& other)
	{
		return subtract(other);
	}

	c_vector_2d& operator*=(const t_scalar_type value)
	{
		return multiply_scalar(value);
	}

	c_vector_2d& operator/=(const t_scalar_type value)
	{
		return divide_scalar(value);
	}

	bool is_zero(t_type epsilon = k_default_epsilon) const
	{
		return math_abs(m_x) <= epsilon && math_abs(m_y) <= epsilon;
	}

	template<typename t_other, typename t_other_scalar, t_other other_epsilon>
	c_vector_2d& add(const c_vector_2d<t_other, t_other_scalar, other_epsilon>& other)
	{
		m_x += other.x();
		m_y += other.y();
		return *this;
	}

	template<typename t_other, typename t_other_scalar, t_other other_epsilon>
	c_vector_2d& subtract(const c_vector_2d<t_other, t_other_scalar, other_epsilon>& other)
	{
		m_x -= other.m_x;
		m_y -= other.m_y;
		return *this;
	}

	c_vector_2d& multiply_scalar(const t_scalar_type value)
	{
		return multiply_scalar_real(value);
	}

	c_vector_2d& divide_scalar(const t_scalar_type value)
	{
		ASSERT(value != 0.0f);
		const t_scalar_type inv = 1 / (value);
		return multiply_scalar_real(inv);
	}

	c_vector_2d& flip()
	{
		return multiply_scalar(-1);
	}

	c_vector_2d& flip_x()
	{
		m_x *= -1;
		return *this;
	}

	c_vector_2d& flip_y()
	{
		m_y *= -1;
		return *this;
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

	t_scalar_type dot(const c_vector_2d& other)
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

using t_vector_2d_int32 = c_vector_2d<int32, real32, k_default_epsilon_int32>;
using t_vector_2d_real32 = c_vector_2d<real32, real32, k_default_epsilon_real32>;


template<typename t_type, typename t_scalar_type, t_type k_default_epsilon>
class c_vector_4d
{
public:
	c_vector_4d()
	{
		zero();
	}

	c_vector_4d(t_type x, t_type y, t_type z, t_type w)
	{
		set(x, y, z, w);
	}

	t_type& x() { return m_x; }
	t_type& y() { return m_y; }
	t_type& z() { return m_z; }
	t_type& w() { return m_w; }

	const t_type& x() const { return m_x; }
	const t_type& y() const { return m_y; }
	const t_type& z() const { return m_z; }
	const t_type& w() const { return m_w; }

	c_vector_2d<t_type, t_scalar_type, k_default_epsilon> xy() const
	{
		c_vector_2d<t_type, t_scalar_type, k_default_epsilon> out(x(), y());
		return out;
	}

	void set(t_type x, t_type y, t_type z, t_type w)
	{
		m_x = x;
		m_y = y;
		m_z = z;
		m_w = w;
	}

	c_vector_4d& zero()
	{
		set(0, 0, 0, 0);
		return *this;
	}

	bool operator==(const c_vector_4d& other) const
	{
		return equals(other);
	}

	bool equals(const c_vector_4d& other, t_type epsilon = k_default_epsilon) const
	{
		return
			math_abs(other.m_x - m_x) <= k_default_epsilon &&
			math_abs(other.m_y - m_y) <= k_default_epsilon &&
			math_abs(other.m_z - m_z) <= k_default_epsilon &&
			math_abs(other.m_w - m_w) <= k_default_epsilon;
	}

	c_vector_4d operator+(const c_vector_4d& other) const
	{
		c_vector_4d out = *this;
		return out.add(other);
	}

	c_vector_4d operator-(const c_vector_4d& other) const
	{
		c_vector_4d out = *this;
		return out.subtract(other);
	}

	c_vector_4d operator*(const t_type value) const
	{
		c_vector_4d out = *this;
		return out.multiply_scalar(value);
	}

	c_vector_4d operator/(const t_type value) const
	{
		c_vector_4d out = *this;
		return out.divide_scalar(value);
	}

	c_vector_4d operator-() const
	{
		c_vector_4d out = *this;
		out.flip();
		return out;
	}

	c_vector_4d& operator+=(const c_vector_4d& other)
	{
		return add(other);
	}

	c_vector_4d& operator-=(const c_vector_4d& other)
	{
		return subtract(other);
	}

	c_vector_4d& operator*=(const t_type value)
	{
		return multiply_scalar(value);
	}

	c_vector_4d& operator/=(const t_type value)
	{
		return divide_scalar(value);
	}

	bool is_zero(t_type epsilon = k_default_epsilon) const
	{
		return 
			math_abs(m_x) <= epsilon &&
			math_abs(m_y) <= epsilon &&
			math_abs(m_z) <= epsilon &&
			math_abs(m_w) <= epsilon;
	}

	c_vector_4d& add(const c_vector_4d& other)
	{
		m_x += other.m_x;
		m_y += other.m_y;
		m_z += other.m_z;
		m_w += other.m_w;
		return *this;
	}

	c_vector_4d& subtract(const c_vector_4d& other)
	{
		m_x -= other.m_x;
		m_y -= other.m_y;
		m_z -= other.m_z;
		m_w -= other.m_w;
		return *this;
	}

	c_vector_4d& multiply_scalar(const t_type value)
	{
		return multiply_scalar_real(value);
	}

	c_vector_4d& divide_scalar(const t_type value)
	{
		ASSERT(value != 0.0f);
		const t_scalar_type inv = 1 / static_cast<t_scalar_type>(value);
		return multiply_scalar_real(inv);
	}

	c_vector_4d& flip()
	{
		return multiply_scalar(-1);
	}

	t_scalar_type magnitude_squared() const
	{
		t_scalar_type x = static_cast<t_scalar_type>(m_x);
		t_scalar_type y = static_cast<t_scalar_type>(m_y);
		t_scalar_type z = static_cast<t_scalar_type>(m_z);
		t_scalar_type w = static_cast<t_scalar_type>(m_w);
		t_scalar_type out = (x * x) + (y * y) + (z * z) + (w * w);

		return out;
	}

	t_scalar_type magnitude() const
	{
		return math_sqrt(magnitude_squared());
	}

	c_vector_4d normal() const
	{
		c_vector_4d out = *this;
		out.normalize();
		return out;
	}

	c_vector_4d& normalize()
	{
		if (is_zero())
		{
			return *this;
		}

		return divide_scalar(magnitude());
	}

	t_scalar_type* dot(const c_vector_4d& other)
	{
		t_scalar_type x = static_cast<t_scalar_type>(m_x);
		t_scalar_type y = static_cast<t_scalar_type>(m_y);
		t_scalar_type z = static_cast<t_scalar_type>(m_z);
		t_scalar_type w = static_cast<t_scalar_type>(m_w);
		return (x * other.m_x) + (y * other.m_y) + (z * other.m_z) + (w + other.m_w);
	}

private:
	c_vector_4d& multiply_scalar_real(const t_scalar_type value)
	{
		m_x *= value;
		m_y *= value;
		m_z *= value;
		m_w *= value;

		return *this;
	}

	t_type m_x;
	t_type m_y;
	t_type m_z;
	t_type m_w;
};

using t_vector_4d_int32 = c_vector_4d<int32, real32, k_default_epsilon_int32>;
using t_vector_4d_real32 = c_vector_4d<real32, real32, k_default_epsilon_real32>;

#endif //_