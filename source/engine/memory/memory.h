#ifndef __MEMORY_H__
#define __MEMORY_H__
#pragma once

#include <types/types.h>
#include <mmath.h>

const uint32 k_byte_kb = 1000;
const uint32 k_byte_mb = math_pow(k_byte_kb, 2);
const uint32 k_byte_gb = math_pow(k_byte_kb, 3);
const uint32 k_byte_tb = math_pow(k_byte_kb, 4);
const uint32 k_byte_kib = 1024;
const uint32 k_byte_mib = math_pow(k_byte_kib, 2);
const uint32 k_byte_gib = math_pow(k_byte_kib, 3);
const uint32 k_byte_tib = math_pow(k_byte_kib, 4);


// using zero_object will wipe out template constants (eg c_static_array::k_max_size)
#define zero_object(obj) memory_zero(&obj, sizeof(obj))

template<typename t_type>
inline void memory_swap(t_type* left, t_type* right)
{
	t_type temp = *left;
	*left = *right;
	*right = temp;
}

void memory_zero(void* obj, size_t size);

template<typename t_type>
void memory_set(void* dest, t_type val, size_t size)
{
	COMPILE_ASSERT(sizeof(t_type) <= 8);
	ASSERT(size % sizeof(t_type) == 0);

	const size_t count = size / sizeof(t_type);
	t_type* dest_ptr = static_cast<t_type*>(dest);

	for (int i = 0; i < count; i++)
	{
		dest_ptr[i] = val;
	}
}

// non-overlapping
void memory_copy(void* dest, void const* src, size_t size);

// overlapping allowed
void memory_move(void* dest, void const* src, size_t size);

// returns 0 if equal
int32 memory_compare(const void* left, const void* right, size_t size);
#endif//__MEMORY_H__