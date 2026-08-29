#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__
#pragma once

#include "config.h"
#include "structures/array.h"
#include "types/types.h"

#include <new>

#define ALLOCATE_NEW(type, allocator, ...) \
    new ((allocator).allocate(sizeof(type), alignof(type))) type(__VA_ARGS__)

#define ALLOCATE_NO_CONSTRUCTOR(type, allocator) static_cast<type*>((allocator).allocate(sizeof(type), alignof(type)))

#ifdef MEMORY_TRACKING_ENABLED
struct s_memory_tracker
{
	const char* system_name;
	const char* subsystem_name;
	const char* file;
	const char* function;
	uint32 line;
};
#endif

class i_allocator
{
public:
	virtual void* allocate(uint64 size, uint64 align) = 0;
	virtual void free(void* memory) = 0;
};

typedef uint64 t_stack_allocator_marker;
class c_stack_allocator : i_allocator
{
public:
	c_stack_allocator();
	c_stack_allocator(void* memory, uint64 size) { set_memory(memory, size); }
	void set_memory(const void* memory, uint64 size);
	void* get_base() { return reinterpret_cast<void*>(m_base); }
	const void* get_base_const() { return reinterpret_cast<const void*>(m_base); }
	uint64 get_capacity() { return m_end - m_base; }

	void* allocate(uint64 size, uint64 align) override;
	void* allocate(uint64 size, uint64 align, t_stack_allocator_marker& out_marker);
	bool is_marker_valid(t_stack_allocator_marker marker) const;
	void free(void* memory) override {} // NOP
	void free_to_marker(t_stack_allocator_marker marker);
	void clear();

private:
	uint64 m_top;
	uint64 m_base;
	uint64 m_end;
};

template<uint64 k_size>
class c_static_stack_allocator : public c_stack_allocator
{
public:
	c_static_stack_allocator() : c_stack_allocator(m_data, k_size) {}

private:
	byte m_data[k_size];
};
#endif //__ALLOCATOR_H__
