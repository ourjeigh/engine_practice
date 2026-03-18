#include <asserts.h>
#ifndef __TYPES_H__
#define __TYPES_H__
#pragma once

typedef wchar_t wchar;
#define WIDE(x) L##x

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
template<typename t_type> inline constexpr bool operator==(const t_type& lhs, const e_invalid& rhs) { return static_cast<t_type>(k_invalid) == lhs; }
template<typename t_type> inline constexpr bool operator==(const e_invalid& lhs, const t_type& rhs) { return static_cast<t_type>(k_invalid) == rhs; }
template<typename t_type> inline constexpr bool operator!=(const t_type& lhs, const e_invalid& rhs) { return !(lhs == rhs); }
template<typename t_type> inline constexpr bool operator!=(const e_invalid& lhs, const t_type& rhs) { return !(lhs == rhs); }


const char k_char_max = 0x7F;
const byte k_byte_max = 0xFF;

const int8 k_int8_max = 0x7F;
const int16 k_int16_max = 0x7FFF;
const int32 k_int32_max = 0x7FFFFFFF;
const int64 k_int64_max = 0x7FFFFFFFFFFFFFFF;

const int8 k_int8_min = static_cast<int8>(0x80);
const int16 k_int16_min = static_cast<int16>(0x8000);
const int32 k_int32_min = static_cast<int32>(0x80000000);
const int64 k_int64_min = static_cast<int64>(0x8000000000000000);

const uint8 k_uint8_max = 0xFF;
const uint16 k_uint16_max = 0xFFFF;
const uint32 k_uint32_max = 0xFFFFFFFF;
const uint64 k_uint64_max = 0xFFFFFFFFFFFFFFFF;

const real32 k_real32_min = 1.175494351e-38F;
const real32 k_real32_max = 3.402823466e+38f;
const real64 k_real64_min = 2.2250738585072014e-308;
const real64 k_real64_max = 1.7976931348623158e+308;


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

inline uint32 uint64_to_uint32(uint64 in)
{
	ASSERT(in <= static_cast<uint64>(k_uint32_max));
	return static_cast<uint32>(in);
}

inline int32 uint32_to_int32(uint32 in)
{
	ASSERT(in <= static_cast<uint32>(k_int32_max));
	return static_cast<int32>(in);
}

inline uint32 int32_to_uint32(int32 in)
{
	ASSERT(in >= 0);
	return static_cast<uint32>(in);
}

inline uint64 int64_to_uint64(int64 in)
{
	ASSERT(in >= 0);
	return static_cast<uint64>(in);
}

inline int64 uint64_to_int64(uint64 in)
{
	ASSERT(in <= static_cast<uint64>(k_int64_max));
	return static_cast<int64>(in);
}

inline int32 uint64_to_int32(uint64 in)
{
	ASSERT(in <= static_cast<uint64>(k_int32_max));
	return static_cast<int32>(in);
}

inline uint32 int64_to_uint32(int64 in)
{
	ASSERT(in >= 0);
	ASSERT(in <= static_cast<int64>(k_uint32_max));
	return static_cast<uint32>(in);
}

inline uint32 real32_to_uint32(real32 in)
{
	ASSERT(in >= 0.0f);
	ASSERT(in <= static_cast<real32>(k_uint32_max));
	return static_cast<uint32>(in);
}

inline uint32 real64_to_uint32(real64 in)
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

// this feels like it belongs somewhere else

template<typename t_type>
bool in_range(t_type min, t_type max, t_type value)
{
	return min <= value && value <= max;
}

inline bool in_range_int32(int32 min, int32 max, int32 value)
{
	return in_range(min, max, value);
}

#endif //__TYPES_H__