// if this is named math.h included windows files will freak out because they cant find
// their own methods. maybe someday i'll implement them all and can make it math.h again
#ifndef __MMATH_H__
#define __MMATH_H__
#pragma once

#include <types/types.h>

const real32 k_math_real32_pi = 3.14159265358979323846f;
const real32 k_math_real32_sixth_pi = k_math_real32_pi / 6.0f;
const real32 k_math_real32_quarter_pi = k_math_real32_pi / 4.0f;
const real32 k_math_real32_third_pi = k_math_real32_pi / 3.0f;
const real32 k_math_real32_half_pi = k_math_real32_pi / 2.0f;
const real32 k_math_real32_two_pi = k_math_real32_pi * 2.0f;

const real64 k_math_real64_pi = 3.14159265358979323846264338327950288;
const real64 k_math_real64_half_pi = 1.570796326794896619231321691639751442;
const real64 k_math_real64_two_pi = 6.283185307179586476925286766559005768;

template<typename t_type>
t_type math_min(t_type a, t_type b)
{
	return a < b ? a : b;
}

template<typename t_type>
t_type math_max(t_type a, t_type b)
{
	return a > b ? a : b;
}

template<typename t_type>
t_type math_pin(t_type min, t_type max, t_type val)
{
	return math_min(max, math_max(min, val));
}

inline int32 math_pin_int32(int32 min, int32 max, int32 val)
{
	return math_pin(min, max, val);
}

template<typename t_type>
constexpr t_type math_abs(t_type in)
{
	return in > 0 ? in : -in;
}

template<typename t_type>
constexpr t_type math_pow(t_type base, int32 exp)
{
	// TODO: create fractional power that returns float
	ASSERT(exp >= 0);

	if (exp == 0)
	{
		return 1;
	}

	t_type temp;
	temp = math_pow(base, exp / 2);
	
	if ((exp % 2) == 0)
	{
		return temp * temp;
	}
	else
	{
		if (exp > 0)
			return base * temp * temp;
		else
			return (temp * temp) / base;
	}
}

template<typename t_type>
t_type math_floor(t_type x)
{
	return static_cast<t_type>(static_cast<int64>(x));
}

template<typename t_type>
t_type math_ceil(t_type x)
{
	return math_floor(x) + static_cast<t_type>(1);
}

constexpr int64 math_fact(int64 in)
{
	int64 out= in;
	while (--in > 0)
	{
		out *= in;
	}

	return out;
}

constexpr real64 math_sin(real64 x)
{
	// shift to [-2*pi, 2*pi]
	int64 shift = static_cast<int64>(x / k_math_real64_two_pi);
	x -= shift * k_math_real64_two_pi;

	// shift to [-pi/2, pi/2]
	if (x > k_math_real64_half_pi)
	{
		x = k_math_real64_pi - x;
	}
	else if (x < -k_math_real64_half_pi)
	{
		x = -k_math_real64_pi - x;  
	}

	real64 x_squared = x * x;

	const real64 component_3 = -1.0f / 6.0f;	// -1/3!
	const real64 component_5 = 1.0f / 120.0f;	//  1/5!
	const real64 component_7 = -1.0f / 5040.0f;	// -1/7!

	return x * (1 + x_squared * (component_3 + x_squared * (component_5 + x_squared * component_7)));
}

constexpr real64 math_cos(real64 x)
{
	return math_sin(x + k_math_real32_half_pi);
}

constexpr real32 math_sin(real32 x)
{
	return static_cast<real32>(math_sin(static_cast<real64>(x)));
}

constexpr real32 math_cos(real32 x)
{
	return static_cast<real32>(math_cos(static_cast<real64>(x)));
}

template<typename t_type>
constexpr int32 math_digit_count(t_type x)
{
	int32 count = 0;
	while (x)
	{
		count++;
		x /= 10;
	}

	return count;
}

#endif //__MAMTH_H__
