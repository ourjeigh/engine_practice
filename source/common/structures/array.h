#ifndef __ARRAY_H__
#define __ARRAY_H__
#pragma once

#include "debug/asserts.h"
#include "types/types.h"
#include "memory/memory.h"

template<class t_type>
class c_array
{
public:
	struct iterator
	{
		iterator(t_type* ptr) : m_ptr(ptr) {}
		t_type& operator*() const { return *m_ptr; }
		t_type* operator->() { return m_ptr; }
		iterator& operator++() { ++m_ptr; return *this; }
		iterator operator++(int) { iterator temp = *this; ++(*this); return temp; }
		iterator& operator--() { --m_ptr; return *this; }
		iterator operator--(int) { iterator temp = *this; --(*this); return temp; }

		bool operator== (const iterator& other) const { return m_ptr == other.m_ptr; }
		bool operator!= (const iterator& other) const { return !(*this == other); }

	private:
		t_type* m_ptr;
	};

	struct const_iterator
	{
		const_iterator(const t_type* ptr) : m_ptr(ptr) {}
		const t_type& operator*() const { return *m_ptr; }
		const t_type* operator->() { return m_ptr; }
		const_iterator& operator++() { ++m_ptr; return *this; }
		const_iterator operator++(int) { const_iterator temp = *this; ++(*this); return temp; }
		const_iterator& operator--() { --m_ptr; return *this; }
		const_iterator operator--(int) { const_iterator temp = *this; --(*this); return temp; }

		bool operator== (const const_iterator& other) const { return m_ptr == other.m_ptr; }
		bool operator!= (const const_iterator& other) const { return !(*this == other); }

	private:
		const t_type* m_ptr;
	};

	c_array()
	{ 
		invalidate();
	}

	c_array(t_type* data, int32 size) : m_data_ref(data), m_size(size) {}

	c_array(const c_array& other) : m_data_ref(other.m_data_ref), m_size(other.m_size) 
	{
		ASSERT(is_valid());
	}

	~c_array() 
	{
		invalidate();
		ASSERT(!is_valid());
	}

	c_array& operator=(const c_array& other)
	{
		ASSERT(other.is_valid());

		m_data_ref = other.m_data_ref;
		m_size = other.m_size;
		return *this;
	}

	t_type& operator[](int32 index)
	{
		assert_valid_index(index);
		return data()[index];
	}

	const t_type& operator[](int32 index) const
	{
		assert_valid_index(index);
		return data()[index];
	}

	bool operator==(const c_array<t_type>& other) const
	{
		bool equal = false;
		if (capacity() == other.capacity())
		{
			equal = true;

			for (int32 i = 0; equal && i < capacity(); i++)
			{
				equal = data()[i] == other.data()[i];
			}
		}

		return equal;
	}

	bool operator!=(const c_array<t_type>& other) const
	{
		return !(operator==(other));
	}

	iterator begin() { return iterator(&data()[0]); }
	iterator end() { return iterator(&data()[capacity()]); }
	iterator begin_reverse() { return iterator(&data()[capacity() - 1]); }
	iterator end_reverse() { return iterator(&data()[-1]); }
	const_iterator begin_const() const { return const_iterator(data()); }
	const_iterator end_const() const { return const_iterator(&data()[capacity()]); }
	const_iterator begin_reverse_const() const { return const_iterator(&data()[capacity() - 1]); }
	const_iterator end_reverse_const() const { return const_iterator(&data()[-1]); }
	
	int32 capacity() const
	{
		return m_size;
	}
	
	t_type* get_item(int32 index)
	{
		ASSERT(in_range_inclusive(k_int32_zero, capacity(), index));
		return &data()[index];
	}

	const t_type* get_item_const(int32 index) const
	{
		ASSERT(in_range_inclusive(k_int32_zero, capacity(), index));
		return &data()[index];
	}

	t_type* data()
	{
		return m_data_ref;
	}

	const t_type* data() const
	{
		return m_data_ref;
	}

	void copy_from(const c_array<t_type>& other)
	{
		int32 count = math_min(capacity(), other.capacity());
		memory_copy(data(), other.data(), sizeof(t_type) * count);
	}

	void copy_from_range(const c_array<t_type>& other, int32 start, int32 end)
	{
		ASSERT(start >= 0);
		ASSERT(end <= other.capacity());
		ASSERT(start < end);
		
		int32 count = (end - start);
		assert_valid_index(count - 1);

		memory_copy(data(), &other.data()[start], sizeof(t_type) * count);
	}

	void copy_from_range_offset(const c_array<t_type>& other, int32 start, int32 end, int32 offset)
	{
		ASSERT(start >= 0);
		ASSERT(end <= other.capacity());
		ASSERT(start < end);

		int32 count = (end - start);

		assert_valid_index(offset + count - 1);

		memory_copy(data() + offset, &other.data()[start], sizeof(t_type) * count);
	}

	bool is_valid() const
	{
		return data() != nullptr && capacity() != k_invalid;
	}

	void invalidate()
	{
		m_data_ref = nullptr;
		m_size = k_invalid;
	}

	void zero_data()
	{
		memory_zero(data(), sizeof(t_type) * m_size);
	}

	c_array<t_type> make_sub_array(int32 start, int32 end)
	{
		assert_valid_index(start);
		assert_valid_index(end);
		ASSERT(end > start);

		const int32 length = end - start;
		return c_array<t_type>(&data()[start], length);
	}

	const c_array<const t_type> make_sub_array_const(int32 start, int32 end) const
	{
		assert_valid_index(start);
		assert_valid_index(end);
		ASSERT(end > start);

		const int32 length = end - start;
		return c_array<const t_type>(&data()[start], length);
	}

protected:

	void assert_valid_index(int32 index) const
	{
		ASSERT(index >= 0);
		ASSERT(index < capacity());
	}

protected:
	t_type* m_data_ref;

	// would like this to be const, but it breaks copy assignment
	int32 m_size;
};

template<class t_type, int32 k_max_size>
class c_static_array : public c_array<t_type>
{
public:
	constexpr c_static_array<t_type, k_max_size>() : c_array<t_type>(&m_data[0], k_max_size) {}
	explicit c_static_array<t_type, k_max_size>(const c_static_array<t_type, k_max_size>& other) : c_array<t_type>(&m_data[0], k_max_size)
	{
		for (int32 i = 0; i < k_max_size; ++i)
		{
			m_data[i] = other.m_data[i];
		}
	}

	template<typename... args>
	constexpr c_static_array<t_type, k_max_size>(args... list) : c_array<t_type>(&m_data[0], k_max_size)
	{
		COMPILE_ASSERT(k_max_size == sizeof...(list));

		int32 i = 0;
		((m_data[i++] = list), ...);
	}

	~c_static_array<t_type, k_max_size>() {}

	void HACK_init()
	{
		this->m_data_ref = m_data;
		this->m_size = k_max_size;
	}

	c_static_array<t_type, k_max_size>& operator=(const c_static_array<t_type, k_max_size>& other)
	{
		if (this != &other)
		{
			for (int32 i = 0; i < k_max_size; ++i)
			{
				m_data[i] = other.m_data[i];
			}
		}

		return *this;
	}

	int32 capacity() const 
	{
		return k_max_size; 
	}

	t_type* data() { return m_data; }
	const t_type* const_data() const { return m_data; }

protected:
	t_type m_data[k_max_size];
};

template<class t_type>
class c_stack : public c_array<t_type>
{
public:
	using typename c_array<t_type>::iterator;

	c_stack() : m_top(k_invalid) {}
	explicit c_stack(t_type* data, int32 size) : c_array<t_type>(data, size), m_top(k_invalid) {}

	void push(t_type item)
	{
		ASSERT(!full());
		this->data()[++m_top] = item;
	}

	t_type& push()
	{
		ASSERT(!full());
		return this->data()[++m_top];
	}

	void push_from(const c_array<t_type> other)
	{
		ASSERT(free() >= other.capacity());
		memory_copy(&this->data()[++m_top], other.data(), sizeof(t_type) * other.capacity());

		m_top += other.capacity() - 1;
		ASSERT(m_top <= this->capacity());
	}

	void pop()
	{
		ASSERT(!empty());
		m_top--;
	}

	t_type& top()
	{
		ASSERT(!empty());
		return this->data()[m_top];
	}
	
	const t_type& top() const
	{
		ASSERT(!empty());
		return this->data()[m_top];
	}

	t_type* get_item(int32 index)
	{
		ASSERT(index <= m_top);
		return c_array<t_type>::get_item(index);
	}

	const t_type* get_item_const(int32 index) const
	{
		ASSERT(index <= m_top);
		return c_array<t_type>::get_item_const(index);
	}

	int32 used() const { return m_top + 1; }
	int32 free() const { return this->capacity() - used(); }
	bool empty() const { return m_top == -1; }
	bool full() const { return m_top == this->capacity() - 1; }
	void clear() { m_top = -1; }

	iterator end() { return iterator(&this->data()[m_top + 1]); }
	iterator begin_reverse() { return iterator(&this->data()[m_top]); }

	void copy_from(const c_stack<t_type>& other)
	{
		c_array<t_type>::copy_from(other);
		m_top = other.m_top;
	}

	void copy_from_range(const c_stack<t_type>& other, int32 start, int32 end)
	{
		c_array<t_type>::copy_from_range(other, start, end);
		m_top = end - start;
	}

	c_array<t_type> make_array()
	{
		return c_array<t_type>(this->data(), used());
	}

protected:
	int32 m_top;
};

template<class t_type, int32 k_max_size>
class c_static_stack : public c_stack<t_type>
{
public:
	c_static_stack() : c_stack<t_type>(&m_data[0], k_max_size) {}

private:
	t_type m_data[k_max_size];
};

template <size_t k_size>
class c_bit_array
{
public:
	c_bit_array() { clear(); }

	void clear()
	{
		set_all(false);
	}

	void set(int32 index, bool value)
	{
		if (value)
		{
			m_data[get_data_index(index)] |= get_index_mask(index);
		}
		else
		{
			m_data[get_data_index(index)] &= ~get_index_mask(index);
		}
	}

	void set_all(bool value)
	{
		for (int32 i = 0; i < m_data.capacity(); i++)
		{
			char val = value ? k_char_max : 0;
			m_data[i] = val;
		}
	}

	void flip(int32 index)
	{
		m_data[get_data_index(index)] ^= get_index_mask(index);
	}

	bool test(uint32 index)
	{
		return m_data[get_data_index(index)] & get_index_mask(index);
	}

	bool any()
	{
		for (int32 i = 0; i < m_data.capacity(); i++)
		{
			if (m_data[i] != 0)
			{
				return true;
			}
		}

		return false;
	}

	bool all()
	{
		for (int32 i = 0; i < m_data.capacity() - 1; i++)
		{
			if (m_data[i] != k_char_max)
			{
				return false;
			}
		}

		// this could be faster
		int32 last_set = k_size % 8;
		for (int32 i = k_size - last_set; i < k_size; i++)
		{
			if (!test(i))
			{
				return false;
			}
		}

		return true;
	}

	bool none()
	{
		return !any();
	}

	int32 count()
	{
		int out_count = 0;

		for (int32 i = 0; i < m_data.capacity(); i++)
		{
			// shift left to compare with storage bit
			// then shift back right since the comparison
			// will only be the tested bit, either 1 or 0.
			out_count += (m_data[i] & (1 << 0)) >> 0;
			out_count += (m_data[i] & (1 << 1)) >> 1;
			out_count += (m_data[i] & (1 << 2)) >> 2;
			out_count += (m_data[i] & (1 << 3)) >> 3;
			out_count += (m_data[i] & (1 << 4)) >> 4;
			out_count += (m_data[i] & (1 << 5)) >> 5;
			out_count += (m_data[i] & (1 << 6)) >> 6;
			out_count += (m_data[i] & (1 << 7)) >> 7;
		}

		return out_count;
	}

private:
	// size of 8 needs 1 char, size of 9 needs 2
	static_member_data const int32 num_chars = (k_size / 9) + 1;

	// we use a char because that causes the least wasted
	// storage for cases where the size is not perfectly
	// divisible (eg size=9 would waste 6 bytes if m_data were an int)
	c_static_array<char, num_chars> m_data;

	char get_index_mask(int index) { return 1 << (index % 8); }
	int32 get_data_index(int index) { return index / 8; }
};

template<size_t k_size>
class c_flags : public c_bit_array<k_size>
{
	// TODO: make a constructor that can take initial values
};

template<typename t_type>
bool array_has_non_zero_data(c_array<t_type>& array)
{
	for (auto it =  array.begin();
		it != array.end();
		++it)
	{
		byte value = static_cast<byte>(*it);
		if (value != 0x0)
		{
			return true;
		}
	}

	return false;
}
#endif //__ARRAY_H__