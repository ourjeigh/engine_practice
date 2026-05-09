#ifndef __HASHING_INL__
#define __HASHING_INL__
#pragma once

#include "debug/asserts.h"

//https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function

// constants
const uint64 k_hash_fnv_32_offset = 2166136261;
const uint64 k_hash_fnv_32_prime = 16777619;
const uint64 k_hash_fnv_64_offset = 14695981039346656037;
const uint64 k_hash_fnv_64_prime = 1099511628211;

// private declarations
template<typename t_type>
constexpr t_type fnv1_hash_internal(const void* input, uint64 size, t_type offset, t_type prime);

template<typename t_type>
constexpr t_type fnv1a_hash_internal(const void* input, uint64 size, t_type offset, t_type prime);

template<typename t_type>
constexpr t_type fnv1_string_hash_internal(const char* input, uint64 size, t_type offset, t_type prime);

template<typename t_type>
constexpr t_type fnv1a_string_hash_internal(const char* input, uint64 size, t_type offset, t_type prime);


// public implementations
constexpr uint32 fnv1_hash_32(const void* input, uint64 size)
{
	return fnv1_hash_internal<uint32>(input, size, k_hash_fnv_32_offset, k_hash_fnv_32_prime);
}

constexpr uint32 fnv1a_hash_32(const void* input, uint64 size)
{
	return fnv1a_hash_internal<uint32>(input, size, k_hash_fnv_32_offset, k_hash_fnv_32_prime);
}

constexpr uint64 fnv1_hash_64(const void* input, uint64 size)
{
	return fnv1_hash_internal<uint64>(input, size, k_hash_fnv_64_offset, k_hash_fnv_64_prime);
}

constexpr uint64 fnv1a_hash_64(const void* input, uint64 size)
{
	return fnv1a_hash_internal<uint64>(input, size, k_hash_fnv_64_offset, k_hash_fnv_64_prime);
}

// strings
constexpr uint32 fnv1_string_hash_32(const char* input, uint64 size)
{
	return fnv1_string_hash_internal<uint32>(input, size, k_hash_fnv_32_offset, k_hash_fnv_32_prime);
}

constexpr uint32 fnv1a_string_hash_32(const char* input, uint64 size)
{
	return fnv1a_string_hash_internal<uint32>(input, size, k_hash_fnv_32_offset, k_hash_fnv_32_prime);
}

constexpr uint64 fnv1_string_hash_64(const char* input, uint64 size)
{
	return fnv1_string_hash_internal<uint64>(input, size, k_hash_fnv_64_offset, k_hash_fnv_64_prime);
}

constexpr uint64 fnv1a_string_hash_64(const char* input, uint64 size)
{
	return fnv1a_string_hash_internal<uint64>(input, size, k_hash_fnv_64_offset, k_hash_fnv_64_prime);
}

// private implementations
template<typename t_type>
constexpr t_type fnv1_hash_internal(const void* input, uint64 size, t_type offset, t_type prime)
{
	ASSERT(input != nullptr);

	const byte* input_bytes = static_cast<const byte*>(input);
	t_type out = offset;

	for (uint64 i = 0; i < size; i++)
	{
		out *= prime;
		out ^= input_bytes[i];
	}

	return out;
}

template<typename t_type>
constexpr t_type fnv1a_hash_internal(const void* input, uint64 size, t_type offset, t_type prime)
{
	ASSERT(input != nullptr);

	const byte* input_bytes = static_cast<const byte*>(input);
	t_type out = offset;

	for (uint64 i = 0; i < size; i++)
	{
		out ^= input_bytes[i];
		out *= prime;
	}

	return out;
}

template<typename t_type>
constexpr t_type fnv1_string_hash_internal(const char* input, uint64 size, t_type offset, t_type prime)
{
	ASSERT(input != nullptr);

	t_type out = offset;

	for (uint64 i = 0; i < size; i++)
	{
		out *= prime;
		out ^= static_cast<byte>(input[i]);
	}

	return out;
}

template<typename t_type>
constexpr t_type fnv1a_string_hash_internal(const char* input, uint64 size, t_type offset, t_type prime)
{
	ASSERT(input != nullptr);

	t_type out = offset;

	for (uint64 i = 0; i < size; i++)
	{
		out ^= static_cast<byte>(input[i]);
		out *= prime;
	}

	return out;
}

#endif //__HASHING_INL__