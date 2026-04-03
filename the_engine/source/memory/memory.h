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

// move
//class c_handle
//{
//public:
//	c_handle() { invalidate(); }
//	~c_handle() { invalidate(); }
//
//	c_handle(c_handle&& other);
//	c_handle& operator=(c_handle&& other);
//
//	bool operator==(const c_handle& other) { return m_pointer == other.m_pointer; }
//	bool operator!=(const c_handle& other) { return !(*this == other); }
//
//	bool is_valid() { return  m_pointer != k_invalid; }
//	void invalidate() { m_pointer = k_invalid; };
//
//	void* get_pointer();
//	const void* get_pointer_const();
//
//private:
//	// no copying
//	c_handle(const c_handle& other) = delete;
//	c_handle& operator=(const c_handle& other) = delete;
//
//	uint64 m_pointer;
//};

//template<typename t_type>
//class c_typed_handle : public c_handle
//{
//public:
//	t_type* get_pointer();
//	const t_type* get_pointer_const();
//};



// using memory_zero will wipe out template constants (eg c_static_array::k_max_size)
#define zero_object(obj) obj = {}

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

template<typename t_type>
bool memory_has_nonzero_padding_bytes(const t_type const_ptr obj)
{
	t_type temp;
	memory_set(&temp, k_int8_zero, sizeof(temp));

	// this doesn't seem to be guaranteed to only copy members, it seems to 
	// copy padding if an assignment operator is not defined. at least with msvc
	temp = *obj;

	return memory_compare(obj, &temp, sizeof(t_type)) != 0;
}

#endif//__MEMORY_H__