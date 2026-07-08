#ifndef __ARRAY_H__
#define __ARRAY_H__
#pragma once

#include "debug/asserts.h"
#include "types/types.h"
#include "memory/memory.h"

#define ARRAY_DECLARE_STORAGE_MEMBERS \
	public: \
		t_type* data() { return m_data; } \
		const t_type* data() const { return m_data; } \
		int32 capacity() const { return k_max_size; } \
	private: \
		t_type m_data[k_max_size];

#define ARRAY_DECLARE_REFERENCE_MEMBERS \
	public: \
		t_type* data() { return m_data_reference; } \
		const t_type* data() const { return m_data_reference; } \
		int32 capacity() const { return m_size; } \
	private: \
		t_type* m_data_reference; \
		int32 m_size; \

// forward declare
template<class t_type>
class c_array;

template<class t_type>
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

template<class t_type>
using const_iterator = iterator<const t_type>;

template<class t_type, class t_derived>
class c_array_base
{
public:
	c_array_base()
	{ 
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

	template<class t_other>
	bool operator==(const c_array_base<t_type, t_other>& other) const
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

	template<class t_other>
	bool operator!=(const c_array_base<t_type, t_other>& other) const
	{
		return !(operator==(other));
	}

	iterator<t_type> begin() { return iterator<t_type>(&data()[0]); }
	iterator<t_type> end() { return iterator<t_type>(&data()[capacity()]); }
	iterator<t_type> begin_reverse() { return iterator<t_type>(&data()[capacity() - 1]); }
	iterator<t_type> end_reverse() { return iterator<t_type>(&data()[-1]); }
	const_iterator<t_type> begin_const() const { return const_iterator<t_type>(data()); }
	const_iterator<t_type> end_const() const { return const_iterator<t_type>(&data()[capacity()]); }
	const_iterator<t_type> begin_reverse_const() const { return const_iterator<t_type>(&data()[capacity() - 1]); }
	const_iterator<t_type> end_reverse_const() const { return const_iterator<t_type>(&data()[-1]); }
	
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

	template<class t_other>
	void copy_from(const c_array_base<t_type, t_other>& other)
	{
		// we could support only copying what fits, but that could lead to sublte/silent errors
		ASSERT(capacity() >= other.capacity());
		memory_copy(data(), other.data(), sizeof(t_type) * other.capacity());
	}

	template<class t_other>
	void copy_from_range(const c_array_base<t_type, t_other>& other, int32 start, int32 end)
	{
		ASSERT(start >= 0);
		ASSERT(end <= other.capacity());
		ASSERT(start < end);

		int32 count = (end - start);
		
		// we could support only copying what fits, but that could lead to sublte/silent errors
		assert_valid_index(count - 1);

		memory_copy(data(), &other.data()[start], sizeof(t_type) * count);
	}

	template<class t_other>
	void copy_from_range_offset(const c_array_base<t_type, t_other>& other, int32 start, int32 end, int32 offset)
	{
		ASSERT(start >= 0);
		ASSERT(end <= other.capacity());
		ASSERT(start < end);

		int32 count = (end - start);

		// we could support only copying what fits, but that could lead to sublte/silent errors
		assert_valid_index(offset + count - 1);

		memory_copy(data() + offset, &other.data()[start], sizeof(t_type) * count);
	}

	bool is_valid() const
	{
		return data() != nullptr && capacity() != k_invalid;
	}

	void zero_data()
	{
		memory_zero(data(), sizeof(t_type) * capacity());
	}

	c_array<t_type> make_sub_array(int32 start, int32 end)
	{
		assert_valid_index(start);
		assert_valid_index(end);
		ASSERT(end > start);

		const int32 length = end - start;
		return c_array<t_type>(&data()[start], length);
	}

	c_array<const t_type> make_sub_array_const(int32 start, int32 end) const
	{
		assert_valid_index(start);
		assert_valid_index(end);
		ASSERT(end > start);

		const int32 length = end - start;
		return c_array<const t_type>(&data()[start], length);
	}

	t_type* data() { return self().data(); }
	const t_type* data() const { return self().data(); }
	int32 capacity() const { return self().capacity(); }

protected:

	void assert_valid_index(int32 index) const
	{
		ASSERT(index >= 0);
		ASSERT(index < capacity());
	}

	t_derived& self() { return *static_cast<t_derived*>(this); }
	const t_derived& self() const { return *static_cast<const t_derived*>(this); }
private:
};

template<class t_type>
class c_array : public c_array_base<t_type, c_array<t_type>>
{
public:
	c_array() : m_data_reference(nullptr), m_size(k_invalid) {}
	c_array(t_type* data, int32 size) : m_data_reference(data), m_size(size) {}

	c_array& operator=(const c_array& other)
	{
		ASSERT(other.is_valid());

		m_data_reference = other.m_data_reference;
		m_size = other.m_size;
		return *this;
	}

	ARRAY_DECLARE_REFERENCE_MEMBERS
};

template<class t_type, int32 k_max_size>
class c_static_array : public c_array_base<t_type, c_static_array<t_type, k_max_size>>
{
public:
	constexpr c_static_array<t_type, k_max_size>() {}
	explicit c_static_array<t_type, k_max_size>(const c_static_array<t_type, k_max_size>& other)
	{
		for (int32 i = 0; i < k_max_size; ++i)
		{
			this->data()[i] = other.data()[i];
		}
	}

	template<typename... args>
	constexpr c_static_array<t_type, k_max_size>(args... list)
	{
		COMPILE_ASSERT(k_max_size == sizeof...(list));

		int32 i = 0;
		((this->data()[i++] = list), ...);
	}

	template<int32 count>
	constexpr c_static_array(t_type(&&array)[count])
	{
		COMPILE_ASSERT(count == k_max_size);

		for (int32 i = 0; i < count; i++)
		{
			this->data()[i] = array[i];
		}
	}

	~c_static_array<t_type, k_max_size>() {}

	c_static_array<t_type, k_max_size>& operator=(const c_static_array<t_type, k_max_size>& other)
	{
		if (this != &other)
		{
			for (int32 i = 0; i < k_max_size; ++i)
			{
				this->data()[i] = other.data()[i];
			}
		}

		return *this;
	}

	c_array<t_type> as_array() { return c_array<t_type>(data(), capacity()); }
	operator c_array<t_type>() { return c_array<t_type>(data(), capacity()); }
	operator c_array<const t_type>() const { return c_array<const t_type>(data(), capacity()); }
	
	ARRAY_DECLARE_STORAGE_MEMBERS
};

template<typename t_type, int32 count>
constexpr auto make_static_array(t_type(&& array)[count])
{
	c_static_array<t_type, count> result;
	for (int32 i = 0; i < count; ++i)
	{
		result.data()[i] = array[i];
	}

	return c_static_array<t_type, count>(result);
}

template<class t_type, class t_derived>
class c_stack_base : public c_array_base<t_type, t_derived>
{
public:
	iterator<t_type> end() { return iterator<t_type>(&this->data()[this->top_index() + 1]); }
	iterator<t_type> begin_reverse() { return iterator<t_type>(&this->data()[this->top_index()]); }
	const_iterator<t_type> end_const() const { return const_iterator<t_type>(&this->data()[this->top_index() + 1]); }
	const_iterator<t_type> begin_reverse_const() const { return const_iterator<t_type>(&this->data()[this->top_index()]); }

	template<class t_other>
	bool operator==(const c_stack_base<t_type, t_other>& other) const
	{
		bool equal = false;
		if (used() == other.used())
		{
			equal = true;

			for (int32 i = 0; equal && i < used(); i++)
			{
				equal = this->data()[i] == other.data()[i];
			}
		}

		return equal;
	}

	template<class t_other>
	bool operator!=(const c_stack_base<t_type, t_other>& other) const
	{
		return !(*this == other);
	}

	void push(t_type item)
	{
		ASSERT(!full());
		this->data()[++this->top_index()] = item;
	}

	t_type& push()
	{
		ASSERT(!full());
		return this->data()[++this->top_index()];
	}

	template<class t_other>
	void push_from(const c_array_base<t_type, t_other>& other)
	{
		ASSERT(free() >= other.capacity());
		memory_copy(&this->data()[++this->top_index()], other.data(), sizeof(t_type) * other.capacity());

		this->top_index() += other.capacity() - 1;
		ASSERT(this->top_index() <= this->capacity());
	}

	void pop()
	{
		ASSERT(!empty());
		this->top_index()--;
	}

	t_type& top()
	{
		ASSERT(!empty());
		return this->data()[this->top_index()];
	}

	const t_type& top() const
	{
		ASSERT(!empty());
		return this->data()[this->top_index()];
	}

	t_type* get_item(int32 index)
	{
		ASSERT(index <= this->top_index());
		return c_array_base<t_type, t_derived>::get_item(index);
	}

	const t_type* get_item_const(int32 index) const
	{
		ASSERT(index <= this->top_index());
		return c_array_base<t_type, t_derived>::get_item_const(index);
	}

	int32 used() const { return this->top_index() + 1; }
	int32 free() const { return this->capacity() - used(); }
	bool empty() const { return this->top_index() == k_index_empty; }
	bool full() const { return this->top_index() == this->capacity() - 1; }
	void clear() { this->top_index() = k_index_empty; }

	operator c_array<t_type>() { return c_array<t_type>(this->data(), this->used()); }
	operator c_array<const t_type>() const { return c_array<const t_type>(this->data(), this->used()); }
	c_array<t_type> as_array_full() { return c_array<t_type>(this->data(), this->capacity()); }

	template<class t_other>
	void copy_from(const c_stack_base<t_type, t_other>& other)
	{
		ASSERT(!other.empty());
		ASSERT(this->capacity() >= other.used());
		c_array_base<t_type, t_derived>::copy_from_range(other, 0, other.used());
		this->top_index() = other.used() - 1;
	}

	template<class t_other>
	void copy_from_range(const c_stack_base<t_type, t_other>& other, int32 start, int32 end)
	{
		c_array_base<t_type, t_derived>::copy_from_range(other, start, end);
		this->top_index() = end - start - 1;
	}

	c_array<t_type> make_array()
	{
		return c_array<t_type>(this->data(), used());
	}

	int32& top_index() { return this->self().top_index(); }
	const int32& top_index() const { return this->self().top_index(); }
protected:
	//int32 m_top = k_index_empty;

private:
	static_member_data const int32 k_index_empty = -1;
};


template<class t_type>
class c_stack : public c_stack_base<t_type, c_stack<t_type>>
{
public:
	c_stack() : m_data_reference(nullptr), m_size(k_invalid), m_top_ref(nullptr) {}
	c_stack(t_type* data, int32 size, int32* top) : m_data_reference(data), m_size(size) 
	{
		m_top_ref = top;
	}

	c_stack& operator=(const c_stack& other)
	{
		ASSERT(other.is_valid());

		m_data_reference = other.m_data_reference;
		m_size = other.m_size;
		m_top_ref = other.m_top_ref;
		return *this;
	}

	int32& top_index() { return *m_top_ref; }
	const int32& top_index() const { return *m_top_ref; }

private:
	int32* m_top_ref;

	ARRAY_DECLARE_REFERENCE_MEMBERS
};

template<class t_type, int32 k_max_size>
class c_static_stack : public c_stack_base<t_type, c_static_stack<t_type, k_max_size>>
{
public:
	c_static_stack() { this->clear(); }

	c_stack<t_type> as_stack() { return c_stack<t_type>(this->data(), this->capacity(), &m_top); }
	operator c_stack<t_type>() { return c_stack<t_type>(this->data(), this->capacity(), &m_top); }

	int32& top_index() { return m_top; }
	const int32& top_index() const { return m_top; }

private:
	int32 m_top;

	ARRAY_DECLARE_STORAGE_MEMBERS
};

#undef ARRAY_DECLARE_STORAGE_MEMBERS
#undef ARRAY_DECLARE_REFERENCE_MEMBERS

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
		ASSERT(in_range_inclusive<int32>(0, k_size - 1, index));

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
public:
	c_flags() { this->clear(); }
	
	template<typename... t_args>
	c_flags(const t_args... args)
	{
		this->clear();
		(this->set(args, true), ...);
	}
};

template<typename t_type, typename t_derived>
bool array_has_non_zero_data(c_array_base<t_type, t_derived>& array)
{
	for (auto it =  array.begin_const();
		it != array.end_const();
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

template<typename t_type>
bool array_has_non_zero_data(c_array<t_type> array)
{
	return array_has_non_zero_data<t_type, c_array<t_type>>(array);
}
#endif //__ARRAY_H__