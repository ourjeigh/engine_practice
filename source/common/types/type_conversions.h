#ifndef __TYPE_CONVERSIONS_H__
#define __TYPE_CONVERSIONS_H__
#pragma once

#include "debug/asserts.h"
#include "types/types.h"

template<typename t_type>
inline constexpr bool in_range_inclusive(t_type min, t_type max, t_type value)
{
	return min <= value && value <= max;
}

inline constexpr bool in_range_inclusive_int32(int32 min, int32 max, int32 value)
{
	return in_range_inclusive(min, max, value);
}

template<typename t_type>
inline constexpr bool in_range_exclusive(t_type min, t_type max, t_type value)
{
	return min < value && value < max;
}

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
	ASSERT(in_range_inclusive(static_cast<real32>(k_int32_min), static_cast<real32>(k_int32_max), in));
	return static_cast<int32>(in);
}

inline constexpr uint32 real32_to_uint32(real32 in)
{
	ASSERT(in >= 0.0f);
	ASSERT(in <= static_cast<real32>(k_uint32_max));
	return static_cast<uint32>(in);
}

inline constexpr int32 real64_to_int32(real64 in)
{
	ASSERT(in_range_inclusive(static_cast<real64>(k_int32_min), static_cast<real64>(k_int32_max), in));
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
#endif //__TYPE_CONVERSIONS_H__