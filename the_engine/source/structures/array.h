#ifndef __ARRAY_H__
#define __ARRAY_H__
#pragma once

#include "types/types.h"
#include "asserts.h"
#include "memory/memory.h"

template<class t_derived_array, class t_type>
class i_array
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

	bool operator==(const i_array<t_derived_array, t_type>& other) const
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

	bool operator!=(const i_array<t_derived_array, t_type>& other) const
	{
		return !(operator==(other));
	}

	iterator begin() { return iterator(&data()[0]); }
	iterator end() { return iterator(&data()[capacity()]); }

	int32 capacity() const
	{
		return static_cast<const t_derived_array*>(this)->capacity();
	}
	
	t_type* get_item(int32 index)
	{
		ASSERT(in_range(0, capacity(), index));
		return &data()[index];
	}

	const t_type* get_item_const(int32 index)
	{
		return get_item(index);
	}

	t_type* data()
	{
		return static_cast<t_derived_array*>(this)->data();
	}

	const t_type* data() const
	{
		return static_cast<const t_derived_array*>(this)->data();
	}

	template<class t_other_derived_type>
	void copy_from_range(const i_array<t_other_derived_type, t_type>& other, int32 start, int32 end)
	{
		ASSERT(start >= 0);
		ASSERT(end <= other.capacity());
		ASSERT(start < end);
		
		int32 count = (end - start);
		assert_valid_index(count - 1);

		memory_copy(data(), &other.data()[start], sizeof(t_type) * count);
	}

	template<class t_other_derived_type>
	void copy_from_range_offset(const i_array<t_other_derived_type, t_type>& other, int32 start, int32 end, int32 offset)
	{
		ASSERT(start >= 0);
		ASSERT(end <= other.capacity());
		ASSERT(start < end);

		int32 count = (end - start);

		assert_valid_index(offset + count - 1);

		memory_copy(data() + offset, &other.data()[start], sizeof(t_type) * count);
	}

protected:
	void assert_valid_index(int32 index) const
	{
		ASSERT(index >= 0);
		ASSERT(index < capacity());
	}

private:

};

template<class t_type>
class c_array_reference : public i_array<c_array_reference<t_type>, t_type>
{
public:
	c_array_reference() : m_data_ref(nullptr), m_capacity(0) {}

	explicit c_array_reference(t_type* data, int32 capacity) : m_data_ref(data), m_capacity(capacity) {}

	c_array_reference(const c_array_reference& other) : m_data_ref(other.m_data_ref), m_capacity(other.m_capacity) {}

	c_array_reference& operator=(const c_array_reference& other)
	{
		m_data_ref = other.m_data_ref;
		m_capacity = other.m_capacity;
		return *this;
	}

	bool is_valid() { return m_data_ref != nullptr; }
	int32 capacity() const { return m_capacity; }
	t_type* data() { return m_data_ref; }
	const t_type* data() const { return m_data_ref; }

protected:

private:
	friend class i_array<c_array_reference<t_type>, t_type>;
	
	t_type* m_data_ref;
	int32 m_capacity;
};

template<class t_type, int32 k_max_size>
class c_array : public i_array<c_array<t_type, k_max_size>, t_type>
{
public:

	c_array<t_type, k_max_size>() { zero_object(m_data); }
	~c_array<t_type, k_max_size>() {}
	c_array<t_type, k_max_size>(const c_array<t_type, k_max_size>& other)
	{
		for (int32 i = 0; i < k_max_size; ++i)
		{
			m_data[i] = other.m_data[i];
		}
	}

	template<typename... args>
	constexpr c_array<t_type, k_max_size>(args... list)
	{
		COMPILE_ASSERT(k_max_size == sizeof...(list));

		int32 i = 0;
		for (t_type item : {list...})
		{
			m_data[i] = item;
			i++;
		}
	}

	c_array<t_type, k_max_size>& operator=(const c_array<t_type, k_max_size>& other)
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

	int32 capacity() const { return k_max_size; }

	virtual c_array_reference<t_type> make_reference()
	{
		return c_array_reference<t_type>(m_data, k_max_size);
	}

	virtual c_array_reference<const t_type> make_reference() const
	{
		return c_array_reference<const t_type>(m_data, k_max_size);
	}

	virtual const c_array_reference<const t_type> make_reference_const() const
	{
		return c_array_reference<const t_type>( m_data, k_max_size );
	}

	t_type* data() { return m_data; }
	const t_type* data() const { return m_data; }

protected:
	friend class i_array<c_array<t_type, k_max_size>, t_type>;

	t_type m_data[k_max_size];
};

template<class t_type, int32 k_max_size>
class c_stack : public c_array<t_type, k_max_size>
{
public:
	using typename c_array<t_type, k_max_size>::iterator;

	c_stack<t_type, k_max_size>() : m_top(k_invalid) {}

	void push(t_type item)
	{
		ASSERT(!full());
		this->m_data[++m_top] = item;
	}

	t_type& push()
	{
		ASSERT(!full());
		return this->m_data[++m_top];
	}

	void pop()
	{
		ASSERT(!empty());
		m_top--;
	}

	t_type top()
	{
		ASSERT(!empty());
		return this->m_data[m_top];
	}

	t_type* get_item(int32 index)
	{
		ASSERT(index <= m_top);
		return i_array<c_stack, t_type>::get_item(index);
	}

	const t_type* get_item_const(int32 index)
	{
		return get_item(index);
	}

	int32 used() const { return m_top + 1; }
	bool empty() const { return m_top == -1; }
	bool full() const { return m_top == k_max_size - 1; }
	void clear() { m_top = -1; }

	iterator begin() { return c_array<t_type, k_max_size>::begin(); }
	iterator end() { return iterator(&this->m_data[m_top + 1]); }

	void copy_from_range(const c_stack<t_type, k_max_size>& other, int32 start, int32 end)
	{
		c_array<t_type, k_max_size>::copy_from_range(other, start, end);
		m_top = end - start;
	}

	// hmm
	const c_array_reference<const t_type> make_reference_const() const override
	{
		return c_array_reference<const t_type>(this->m_data, used());
	}

protected:
	int32 m_top;
};


template <size_t k_size>
class c_bit_array
{
public:
	c_bit_array() { clear(); }

	void clear()
	{
		zero_object(m_data);
	}

	void set(uint32 index, bool value)
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

	void flip(uint32 index)
	{
		m_data[get_data_index(index)] ^= get_index_mask(index);
	}

	bool test(uint32 index)
	{
		return m_data[get_data_index(index)] & get_index_mask(index);
	}

	bool any()
	{
		for (uint32 i = 0; i < num_chars; i++)
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
		for (uint32 i = 0; i < num_chars - 1; i++)
		{
			if (m_data[i] != k_char_max)
			{
				return false;
			}
		}

		// this could be faster
		uint32 last_set = k_size % 8;
		for (uint32 i = k_size - last_set; i < k_size; i++)
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

	uint32 count()
	{
		int out_count = 0;

		for (uint32 i = 0; i < num_chars; i++)
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

	void print()
	{
		//t_string_128 temp_string;
		//for (int i = 0; i < num_chars; i++)
		//{
		//	// shift left to compare with storage bit
		//	// then shift back right since the comparison
		//	// will only be the tested bit, either 1 or 0.
		//	temp_string.append("%i %i %i %i %i %i %i %i",
		//		((m_data[i] & (1 << 0)) >> 0),
		//		((m_data[i] & (1 << 1)) >> 1),
		//		((m_data[i] & (1 << 2)) >> 2),
		//		((m_data[i] & (1 << 3)) >> 3),
		//		((m_data[i] & (1 << 4)) >> 4),
		//		((m_data[i] & (1 << 5)) >> 5),
		//		((m_data[i] & (1 << 6)) >> 6),
		//		((m_data[i] & (1 << 7)) >> 7));
		//}
	}
private:
	// size of 8 needs 1 char, size of 9 needs 2
	static const uint32 num_chars = (k_size / 9) + 1;

	// we use a char because that causes the least wasted
	// storage for cases where the size is not perfectly
	// divisible (eg size=9 would waste 6 bytes if m_data were an int)
	char m_data[num_chars];

	char get_index_mask(int index) { return 1 << (index % 8); }
	uint32 get_data_index(int index) { return index / 8; }
};

template<size_t k_size>
class c_flags : public c_bit_array<k_size>
{
	// TODO: make a constructor that can take initial values
};

template<typename t_type>
bool array_has_non_zero_data(c_array_reference<t_type> array)
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