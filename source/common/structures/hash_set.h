#ifndef __HASH_SET_H__
#define __HASH_SET_H__
#pragma once

#include "types/types.h"
#include "array.h"
#include "algorithms/algorithms.h"

const real32 k_load_factor_threshold = 0.7f;

template<typename t_key, typename t_value>
struct s_key_value_pair
{
	s_key_value_pair() : key(), value() {}
	s_key_value_pair(t_key key, t_value value) : key(key), value(value) {}
	
	t_key key;
	t_value value;
};

template<typename t_type>
struct s_default_comparitor
{
	bool operator()(const t_type& lhs, const t_type& rhs)
	{
		return lhs == rhs;
	}
};

template<typename t_type>
struct s_default_hasher
{
	uint32 operator()(const t_type& value)
	{
		return fnv1a_hash_32(&value, sizeof(value));
	}
};

// base implementation for hash set and hash map, you should probably just use one of those.
// note: we actually reserve a larger array to ensure efficient operation
// f_get_key is a functor that must provide const and non-const methods for retrieving the t_key from the t_type.
template<
	typename t_type,
	typename t_key,
	int32 k_max_size,
	typename f_get_key,
	typename f_hasher = s_default_hasher<t_type>, 
	typename f_comparator = s_default_comparitor<t_type>>
class c_linear_hash_table_base
{
public:
	struct iterator
	{
	public:
		iterator(c_linear_hash_table_base* table, int32 index) : table(table), index(index) 
		{
			if (index < table->m_data.capacity() && table->m_data[index].state != cell_state_occupied) 
			{
				try_advance();
			}
		}

		t_type* operator->() { return &table->m_data[index].data; }
		t_type& operator*() { return table->m_data[index].data; }

		iterator& operator++() 
		{
			index++;
			try_advance();
			return *this;
		}

		bool operator==(const iterator& other) const { return index == other.index; }
		bool operator!=(const iterator& other) const { return !(*this == other); }

	protected:
		void try_advance()
		{
			while (index < table->m_data.capacity() && table->m_data[index].state != cell_state_occupied)
			{
				index++;
			}
		}

		c_linear_hash_table_base* table;
		int32 index;
	};

	iterator begin() { return iterator(this, 0); }
	iterator end() { return iterator(this, m_data.capacity()); }
	
	c_linear_hash_table_base() : m_used(0), m_data() {}

	t_type& find(const t_key& key)
	{
		bool found;
		int32 index = find_index(key, found);
		ASSERT(found && m_data[index].state == cell_state_occupied);

		return m_data[index].data;
	}

	const t_type& find_const(const t_key& key) const
	{
		bool found;
		int32 index = find_index(key, found);
		ASSERT(found && m_data[index].state == cell_state_occupied);

		return m_data[index].data;
	}

	t_type& insert(const t_key& key)
	{
		ASSERT(!full());

		bool found;
		int32 index = find_index(key, found);
		ASSERT(!found && m_data[index].state != cell_state_occupied);

		insert_at_index_unsafe(index, key);
		return m_data[index].data;
	}

	t_type& find_or_insert(const t_key& key, bool& out_found)
	{
		int32 index = find_index(key, out_found);

		if (out_found)
		{
			ASSERT(m_data[index].state == cell_state_occupied);
			return m_data[index].data;
		}

		// didn't find, try to insert
		ASSERT(!full());

		insert_at_index_unsafe(index, key);
		return m_data[index].data;
	}

	// returns true if found, out_value will be nullptr if couldn't find or insert
	bool try_find_or_insert(const t_key& key, t_type** out_value)
	{
		bool found;
		int32 index = find_index(key, found);

		if (found)
		{
			ASSERT(m_data[index].state == cell_state_occupied);
			*out_value = &m_data[index].data;
			return true;
		}

		// didn't find, try to insert
		if (full())
		{
			*out_value = nullptr;
			return false;
		}
	
		insert_at_index_unsafe(index, key);
		*out_value = &m_data[index].data;

		return false;
	}

	// returns true if removed
	bool remove(const t_key& key)
	{
		bool found;
		int32 index = find_index(key, found);

		if (found)
		{
			ASSERT(m_data[index].state == cell_state_occupied);

			m_data[index].state = cell_state_deleted;
			m_used--;
			return true;
		}

		return false;
	}

	bool contains(const t_key& key) const
	{
		bool found;
		int32 index = find_index(key, found);
		ASSERT(!found || m_data[index].state == cell_state_occupied);
		return found;
	}

	void clear()
	{
		for (auto it = m_data.begin(); it != m_data.end(); ++it)
		{
			it->state = cell_state_empty;
		}
	
		m_used = 0;
	}

	int32 usabale_capacity() { return k_max_size; }
	int32 used() { return m_used; }
	int32 full() { return m_used == k_max_size; }

private:
	enum e_cell_state : uint8
	{
		cell_state_empty,
		cell_state_occupied,
		cell_state_deleted,

		k_cell_state_count
	};

	struct s_cell
	{
		s_cell() : state(cell_state_empty) {}

		t_type data;
		e_cell_state state;
	};

	// ie "probe"
	int32 find_index(const t_key& key, bool& found) const
	{
		found = false;
		uint32 hash_value = f_hasher{}(key);
		int32 index = hash_value % m_data.capacity();
		int32 first_deleted = k_invalid;

		for (int32 visited = 0;
			(m_data[index].state != cell_state_empty) && (visited < m_data.capacity());
			visited++)
		{
			if (m_data[index].state == cell_state_deleted && first_deleted == k_invalid)
			{
				first_deleted = index;
			}
			else if (m_data[index].state == cell_state_occupied &&
					 f_comparator{}(f_get_key{}(m_data[index].data), key))
			{
				found = true;
				return index;
			}

			index = ++index % m_data.capacity();
		}

		return first_deleted != k_invalid ? first_deleted : index;
	}

	void set_key(t_type& data, const t_key& key)
	{
		f_get_key{}(data) = key;
	}

	void insert_at_index_unsafe(int32 index, const t_key& key)
	{
		set_key(m_data[index].data, key);
		m_data[index].state = cell_state_occupied;
		m_used++;

#ifdef CONFIG_DEBUG
		for (int32 i1 = 0; i1 < m_data.capacity(); i1++)
		{
			if (m_data[i1].state == cell_state_occupied)
			{
				for (int32 i2 = i1 + 1; i2 < m_data.capacity(); i2++)
				{
					if (i1 != i2 && m_data[i1].state == cell_state_occupied)
					{
						ASSERT(!f_comparator{}(f_get_key{}(m_data[i1].data), f_get_key{}(m_data[i2].data)));
					}
				}
			}
		}
#endif // CONFIG_DEBUG
	}

	static const int32 k_array_size_actual = real32_to_int32(0.5f + (k_max_size * 1 / k_load_factor_threshold));
	c_static_array<s_cell, k_array_size_actual> m_data;
	int32 m_used;
};

// note: we actually reserve a larger array to ensure efficient operation
template<
	typename t_type,
	int32 k_max_size,
	typename f_hasher = s_default_hasher<t_type>,
	typename f_comparator = s_default_comparitor<t_type>>
class c_hash_set
{
public:
	class s_get_key;
	using t_table = c_linear_hash_table_base<t_type, t_type, k_max_size, s_get_key, f_hasher, f_comparator>;
	using iterator = t_table::iterator;

	iterator begin() { return m_table.begin(); }
	iterator end() { return m_table.end(); }

	t_type& insert(const t_type& key)
	{
		return m_table.insert(key);
	}

	t_type& find_or_insert(t_type& key, bool& out_found)
	{
		return m_table.find_or_insert(key, out_found);
	}

	// returns true if found, out_value will be nullptr if couldn't find or insert
	bool try_find_or_insert(t_type& key, t_type** out_value)
	{
		m_table.try_find_or_insert(key, out_value);
	}

	bool remove(const t_type& key)
	{
		return m_table.remove(key);
	}

	bool contains(const t_type& key)
	{
		return m_table.contains(key);
	}

	void clear()
	{
		m_table.clear();
	}

	int32 usabale_capacity() { return m_table.usabale_capacity(); }
	int32 used() { return m_table.used(); }
	int32 full() { return m_table.full(); }

private:
	struct s_get_key
	{
		t_type& operator()(t_type& key) { return key; }
		const t_type& operator()(const t_type& key) { return key; }
	};

	t_table m_table;
};

template<
	typename t_key,
	typename t_value,
	int32 k_max_size,
	typename f_hasher = s_default_hasher<t_key>,
	typename f_comparitor = s_default_comparitor<t_key>>
class c_hash_map
{
public:
	class s_get_key;
	using t_table = c_linear_hash_table_base<s_key_value_pair<t_key, t_value>, t_key, k_max_size, s_get_key, f_hasher, f_comparitor>;
	using iterator = t_table::iterator;
	
	iterator begin() { return m_table.begin(); }
	iterator end() { return m_table.end(); }

	t_value& insert(const t_key& key)
	{
		return m_table.insert(key).value;
	}

	t_value& find_or_insert(const t_key& key, bool& out_found)
	{
		return m_table.find_or_insert(key, out_found).value;
	}

	// returns true if found, out_value will be nullptr if couldn't find or insert
	bool try_find_or_insert(const t_key& key, t_value** out_value)
	{
		t_kvp* out_kvp;
		bool result = m_table.try_find_or_insert(key, &out_kvp);
		if (result)
		{
			*out_value = &out_kvp->value;
		}

		return result;
	}

	t_value& find(const t_key& key)
	{
		return m_table.find(key).value;
	}

	bool contains(const t_key& key)
	{
		return m_table.contains(key);
	}

	bool remove(const t_key& key)
	{
		return m_table.remove(key);
	}

	void clear()
	{
		m_table.clear();
	}

	t_value& operator[](const t_key& key)
	{
		bool found;
		return find_or_insert(key, found);
	}

	int32 usabale_capacity() { return m_table.usabale_capacity(); }
	int32 used() { return m_table.used(); }
	int32 full() { return m_table.full(); }

private:
	using t_kvp = s_key_value_pair<t_key, t_value>;

	struct s_get_key
	{
		t_key& operator()(t_kvp& kvp) { return kvp.key; }
		const t_key& operator()(const t_kvp& kvp) { return kvp.key; }
	};

	t_table m_table;
};

#endif // __HASH_SET_H__
