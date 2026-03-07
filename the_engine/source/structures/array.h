#ifndef __ARRAY_H__
#define __ARRAY_H__
#pragma once

#include "types/types.h"
#include "asserts.h"
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

	c_array()
	{ 
		invalidate();
	}

	explicit c_array(const c_array& other) : m_data_ref(other.m_data_ref), m_size(other.m_size) 
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

	int32 capacity() const
	{
		return m_size;
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

	// it'd be good to make this protected
	explicit c_array(t_type* data, int32 size) : m_data_ref(data), m_size(size) {}
protected:

	void assert_valid_index(int32 index) const
	{
		ASSERT(index >= 0);
		ASSERT(index < capacity());
	}

private:
	t_type* m_data_ref;

	// would like this to be const, but it breaks copy assignment
	int32 m_size;
};

//template<class t_type>
//class c_array : public i_array<c_array<t_type>, t_type>
//{
//public:
//	c_array() : m_data_ref(nullptr), m_capacity(0) {}
//
//	explicit c_array(t_type* data, int32 capacity) : m_data_ref(data), m_capacity(capacity) {}
//
//	c_array(const c_array& other) : m_data_ref(other.m_data_ref), m_capacity(other.m_capacity) {}
//
//	c_array& operator=(const c_array& other)
//	{
//		m_data_ref = other.m_data_ref;
//		m_capacity = other.m_capacity;
//		return *this;
//	}
//
//	bool is_valid() { return m_data_ref != nullptr; }
//	int32 capacity() const { return m_capacity; }
//	t_type* data() { return m_data_ref; }
//	const t_type* data() const { return m_data_ref; }
//
//protected:
//
//private:
//	friend class i_array<c_array<t_type>, t_type>;
//	
//	t_type* m_data_ref;
//	int32 m_capacity;
//};

template<class t_type, int32 k_max_size>
class c_static_array : public c_array<t_type>
{
public:
	constexpr c_static_array<t_type, k_max_size>() : c_array<t_type>(&m_data[0], k_max_size) {/* zero_object(m_data);*/ }
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
		for (t_type item : {list...})
		{
			m_data[i] = item;
			i++;
		}
	}

	~c_static_array<t_type, k_max_size>() {}

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

	int32 capacity() const { return k_max_size; }

	/*c_array<t_type> make_reference()
	{
		return c_array<t_type>(m_data, k_max_size);
	}

	c_array<const t_type> make_reference() const
	{
		return c_array<const t_type>(m_data, k_max_size);
	}

	const c_array<const t_type> make_reference_const() const
	{
		return c_array<const t_type>( m_data, k_max_size );
	}*/

	t_type* data() { return m_data; }
	const t_type* const_data() const { return m_data; }

protected:
	//friend class i_array<c_static_array<t_type, k_max_size>, t_type>;

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

	const t_type* get_item_const(int32 index)
	{
		return get_item(index);
	}

	int32 used() const { return m_top + 1; }
	bool empty() const { return m_top == -1; }
	bool full() const { return m_top == this->capacity() - 1; }
	void clear() { m_top = -1; }

	iterator end() { return iterator(&this->data()[m_top + 1]); }

	void copy_from(const c_stack<t_type>& other)
	{
		//todo: remove this and properly handle the min capacity case
		ASSERT(this->capacity() == other.capacity());
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


// -------------------------------------------------------------------------
// v2

template<typename t_type>
class c_array_v2
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

	c_array_v2(t_type* data, int32 size) : m_data_ref(data), m_size(size) {};

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

	bool operator==(const c_array_v2<t_type>& other) const
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

	bool operator!=(const c_array_v2<t_type>& other) const
	{
		return !(operator==(other));
	}

	iterator begin() { return iterator(m_data_ref); }
	iterator end() { return iterator(m_data_ref[capacity()]); }

	int32 capacity() const
	{
		return m_size;
	}

	t_type* get_item(int32 index)
	{
		ASSERT(in_range(0, capacity(), index));
		return m_data_ref[index];
	}

	const t_type* get_item_const(int32 index)
	{
		return get_item(index);
	}

	t_type* data()
	{
		return m_data_ref;
	}

	const t_type* data() const
	{
		return m_data_ref;
	}

	bool is_valid() const 
	{ 
		return m_data_ref != nullptr;
	}

protected:
	void assert_valid_index(int32 index) const
	{
		ASSERT(in_range(0, capacity() - 1));
		//ASSERT(index >= 0);
		//ASSERT(index < capacity());
	}

	t_type* m_data_ref;
	int32 m_size;
};

template<typename t_type, int32 k_size>
class c_array_static_v2 : public c_array_v2<t_type>
{
public:
	c_array_static_v2() : c_array_v2<t_type>(&m_data, k_size) {}
private:
	t_type m_data[k_size];
};
#endif //__ARRAY_H__