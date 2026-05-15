#ifndef __ATOMIC_H__
#define __ATOMIC_H__
#pragma once

#include "types/types.h"
#include "debug/asserts.h"

#ifdef USING_STD_ATOMIC
#include "atomic"
#endif

// based off of https://en.cppreference.com/w/cpp/atomic/memory_order.html
enum e_atomic_memory_order
{
	atomic_memory_order_relaxed,
	atomic_memory_order_consume,
	atomic_memory_order_acquire,
	atomic_memory_order_release,
	atomic_memory_order_aquire_release,
	atomic_memory_order_sequential,

	k_atomic_memory_order_count
};

template<typename t_type>
class c_atomic
{
public:
	c_atomic() = default;
	c_atomic(t_type value) { store(value); }

	t_type load(e_atomic_memory_order order = atomic_memory_order_sequential);
	void store(t_type value, e_atomic_memory_order order = atomic_memory_order_sequential);
	t_type exchange(t_type value, e_atomic_memory_order order = atomic_memory_order_sequential);
	bool compare_exchange(t_type& expected, t_type desired, e_atomic_memory_order order = atomic_memory_order_sequential);
	
	t_type operator=(const t_type& other) = delete;
	bool operator==(const t_type& other) = delete;
	bool operator!=(const t_type& other) = delete;

private:
#ifdef USING_STD_ATOMIC
	std::atomic<t_type> m_data;
#else
	volatile t_type m_data;
#endif
	COMPILE_ASSERT(sizeof(t_type) == 4 || sizeof(t_type) == 8);
};

#include "atomic.inl"
#endif // __ATOMIC_H__