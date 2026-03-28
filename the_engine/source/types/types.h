#include <asserts.h>
#ifndef __TYPES_H__
#define __TYPES_H__
#pragma once

#define const_ptr *const

#define static_global static
#define static_function static
#define static_local static

#define WIDE(x) L##x

// types
typedef wchar_t wchar;

typedef unsigned char byte;

typedef char int8;
typedef short int16;
typedef long int32;
typedef long long int64;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;
typedef unsigned long long uint64;

typedef float real32;
typedef double real64;

enum e_invalid
{
	k_invalid = -1
};

template<typename t_type> inline constexpr bool operator==(const t_type& lhs, const e_invalid& rhs)
{ 
	return static_cast<t_type>(k_invalid) == lhs;
}

template<typename t_type> inline constexpr bool operator==(const e_invalid& lhs, const t_type& rhs)
{
	return static_cast<t_type>(k_invalid) == rhs; 
}

template<typename t_type> inline constexpr bool operator!=(const t_type& lhs, const e_invalid& rhs)
{
	return !(lhs == rhs);
}

template<typename t_type> inline constexpr bool operator!=(const e_invalid& lhs, const t_type& rhs)
{
	return !(lhs == rhs);
}

// limits
static_global const char k_char_max = 0x7F;
static_global const byte k_byte_max = 0xFF;

static_global const int8 k_int8_max = 0x7F;
static_global const int16 k_int16_max = 0x7FFF;
static_global const int32 k_int32_max = 0x7FFFFFFF;
static_global const int64 k_int64_max = 0x7FFFFFFFFFFFFFFF;

static_global const int8 k_int8_min = static_cast<int8>(0x80);
static_global const int16 k_int16_min = static_cast<int16>(0x8000);
static_global const int32 k_int32_min = static_cast<int32>(0x80000000);
static_global const int64 k_int64_min = static_cast<int64>(0x8000000000000000);

#define number_zero 0
static_global const int8 k_int8_zero = number_zero;
static_global const int16 k_int16_zero = number_zero;
static_global const int32 k_int32_zero = number_zero;
static_global const int64 k_int64_zero = number_zero;

static_global const uint8 k_uint8_max = 0xFF;
static_global const uint16 k_uint16_max = 0xFFFF;
static_global const uint32 k_uint32_max = 0xFFFFFFFF;
static_global const uint64 k_uint64_max = 0xFFFFFFFFFFFFFFFF;

static_global const real32 k_real32_min = 1.175494351e-38F;
static_global const real32 k_real32_max = 3.402823466e+38f;
static_global const real64 k_real64_min = 2.2250738585072014e-308;
static_global const real64 k_real64_max = 1.7976931348623158e+308;


// this feels like it belongs somewhere else
template<typename t_type>
inline constexpr bool in_range(t_type min, t_type max, t_type value)
{
	return min <= value && value <= max;
}

inline constexpr bool in_range_int32(int32 min, int32 max, int32 value)
{
	return in_range(min, max, value);
}

// conversions
inline constexpr uint8 int32_to_uint8(int32 in)
{
	ASSERT(in >= 0);
	ASSERT(in <= static_cast<int32>(k_uint8_max));
	return static_cast<uint8>(in);
}

inline constexpr int64 int32_to_int64(int32 in)
{
	return in;
}

inline constexpr int8 int64_to_int8(int64 in)
{
	ASSERT(in <= static_cast<int64>(k_int8_max));
	return static_cast<int8>(in);
}

inline constexpr int32 int64_to_int32(int64 in)
{
	ASSERT(in <= static_cast<int64>(k_int32_max));
	return static_cast<int32>(in);
}

inline constexpr real32 int64_to_real32(int64 in)
{
	return static_cast<real32>(in);
}

inline constexpr uint32 uint64_to_uint32(uint64 in)
{
	ASSERT(in <= static_cast<uint64>(k_uint32_max));
	return static_cast<uint32>(in);
}

inline constexpr int32 uint32_to_int32(uint32 in)
{
	ASSERT(in <= static_cast<uint32>(k_int32_max));
	return static_cast<int32>(in);
}

inline constexpr uint32 int32_to_uint32(int32 in)
{
	ASSERT(in >= 0);
	return static_cast<uint32>(in);
}

inline constexpr uint64 int64_to_uint64(int64 in)
{
	ASSERT(in >= 0);
	return static_cast<uint64>(in);
}

inline constexpr int64 uint64_to_int64(uint64 in)
{
	ASSERT(in <= static_cast<uint64>(k_int64_max));
	return static_cast<int64>(in);
}

inline constexpr int32 uint64_to_int32(uint64 in)
{
	ASSERT(in <= static_cast<uint64>(k_int32_max));
	return static_cast<int32>(in);
}

inline constexpr uint32 int64_to_uint32(int64 in)
{
	ASSERT(in >= 0);
	ASSERT(in <= static_cast<int64>(k_uint32_max));
	return static_cast<uint32>(in);
}

inline constexpr int32 real32_to_int32(real32 in)
{
	ASSERT(in_range(static_cast<real32>(k_int32_min), static_cast<real32>(k_int32_max), in));
	return static_cast<int32>(in);
}

inline constexpr uint32 real32_to_uint32(real32 in)
{
	ASSERT(in >= 0.0f);
	ASSERT(in <= static_cast<real32>(k_uint32_max));
	return static_cast<uint32>(in);
}

inline constexpr uint32 real64_to_uint32(real64 in)
{
	ASSERT(in >= 0.0);
	ASSERT(in <= static_cast<real64>(k_uint32_max));
	return static_cast<uint32>(in);
}

inline char wchar_to_char(const wchar in)
{
	ASSERT(in <= k_char_max);
	return static_cast<char>(in);
}
#endif //__TYPES_H__