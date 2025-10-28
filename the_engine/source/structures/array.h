#ifndef __ARRAY_H__
#define __ARRAY_H__
#pragma once

#include "types/types.h"

template<class t_type, int32 k_max_size>
class c_array
{
public:
	int32 size() { return k_max_size; }
	t_type& operator[](int32 index) 
	{ 
		assert_valid_index(index);
		return m_data[index]; 
	}

protected:
	void assert_valid_index(int32 index)
	{
		assert(index >= 0);
		assert(index < k_max_size);
	}

	t_type m_data[k_max_size];
	// how can we track usage in debug builds?
};

template<class t_type, int32 k_max_size>
class c_stack : c_array<t_type, k_max_size>
{
public:
	c_stack() { m_top = -1; }

	void push(t_type item)
	{
		assert(!full());
		m_data[++m_top] = item;
	}

	void pop()
	{
		assert(!empty());
		m_top--;
	}

	t_type top()
	{
		assert(!empty());
		return m_data[m_top];
	}

	int32 used() { return m_top; }
	bool empty() { return m_top == -1; }
	bool full() { return m_top == k_max_size - 1; }
private:
	int32 m_top;

	//debug only
	int32 d_max_used;
};

#include "structures/array.inl"

#endif //__ARRAY_H__