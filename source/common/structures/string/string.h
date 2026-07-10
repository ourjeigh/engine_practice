#ifndef __STRING_H__
#define __STRING_H__
#pragma once

#include "debug/asserts.h"
#include "structures/array.h"
#include "structures/string/string_format.h"
#include "types/types.h"


// TODO: create a c_string_base which imherits c_stack_base and
// make c_string and c_satic_string inherit that
class c_string : public c_stack<char>
{
public:
	using c_stack<char>::empty;

	c_string() { this->clear(); }

	explicit c_string(char* data, int32 size, int32* top) : c_stack<char>(data, size, top) {}

	void clear()
	{
		c_stack::clear();
	}

	int32 used() const 
	{ 
		return c_stack::used(); 
	}

	void copy_from(const c_string& other)
	{
		ASSERT(other.is_terminated());
		c_stack<char>::copy_from(other);
		ASSERT(is_terminated());
	}

	void copy_from_range(const c_string& other, int32 start, int32 end)
	{
		ASSERT(other.is_terminated());
		c_stack::copy_from_range(other, start, end);
		terminate();
	}

	void print(const char* string)
	{
		clear();
		append(string);
	}

	void append(const char* string)
	{
		if (!empty() && is_terminated())
		{
			pop();
		}

		if (string != nullptr)
		{
			while (*string != k_null_char)
			{
				this->push(*string++);
			}
		}

		terminate();
	}

	template<typename... t_args>
	void printf(const char* format, t_args... args)
	{
		clear();
		appendf(format, args...);
		terminate();
	}

	template<typename... t_args>
	void appendf(const char* format, t_args... args)
	{
		ASSERT(format != nullptr);

		if (!empty() && is_terminated())
		{
			pop();
		}

		appendf_internal(format, args...);

		terminate();
	}

	const char* get_const_char() const
	{
		if (this->empty())
		{
			return nullptr;
		}
		
		ASSERT(is_terminated());

		return this->data();
	}

	void assert_valid()
	{
		assert_valid_index(this->top_index());
		ASSERT(is_terminated());
	}

	void terminate()
	{
		if (!is_terminated())
		{
			this->push(k_null_char);
		}
	}
	
	bool contains(c_string substring) const
	{
		bool matched = false;
		int32 string_index = 0;
		int32 substring_index = 0;
		int32 substring_length = substring.used();

		if (substring.is_terminated())
		{
			substring_length--;
		}

		while (!matched && string_index < this->used())
		{
			if (*get_item_const(string_index) == *substring.get_item_const(substring_index))
			{ 
				substring_index++;
				matched = substring_index == substring_length;
			}
			else
			{
				substring_index = 0;
			}

			string_index++;
		}

		return matched;
	}

	bool ends_with(const c_string substring) const
	{
		if (empty()) return false;

		int32 string_index = used() - (is_terminated() ? 2 : 1);
		int32 substring_index = substring.used() - (substring.is_terminated() ? 2 : 1);

		while (string_index >= 0 && substring_index >= 0)
		{
			if (*get_item_const(string_index) != *substring.get_item_const(substring_index))
			{
				return false;
			}

			string_index--;
			substring_index--;
		}

		return true;
	}

private:

	bool is_terminated() const
	{
		return top() == k_null_char;
	}

	void appendf_internal(const char*& format)
	{
		push_unformatted_string(format);
		ASSERT(*format == k_null_char);
	}

	template<typename t_type, typename... t_args>
	void appendf_internal(const char*& format, t_type first, t_args... args)
	{
		// process first
		push_unformatted_string(format);

		// we still have args to parse so we better have an arg format ready
		ASSERT(*format == k_string_format_char_begin);
		format++;

		t_arg_format_buffer arg_format_buffer;
		parse_format_buffer(format, arg_format_buffer);
		format_arg(arg_format_buffer, first, *this);
		
		format += arg_format_buffer.used();

		ASSERT(*format == k_string_format_char_end);
		format++;

		appendf_internal(format, args...);
	}

	void push_unformatted_string(const char*& format)
	{
		// todo: handle escaped (eg "{{blah}}") 
		while (*format != k_null_char && *format != k_string_format_char_begin)
		{
			push(*format++);
		}
	}

	template<typename t_type>
	inline void format_arg(t_arg_format_buffer format, const t_type& value, t_char_stack& out)
	{
		const s_format_spec spec = parse_spec(format);
		s_string_formatter<t_type>::format(spec, value, out);
	}
};

template<int32 k_max_size>
class c_static_string : public c_string
{
public:
	c_static_string() : c_string(m_data, k_max_size, &m_top) { this->clear(); }

	constexpr c_static_string(const char* string) : c_string(m_data, k_max_size, &m_top)
	{
		this->clear();
		print(string);
	}

private:
	char m_data[k_max_size];
	int32 m_top;
};

typedef c_static_string<128> t_string_128;
typedef c_static_string<256> t_string_256;
typedef c_static_string<512> t_string_512;
typedef c_static_string<1024> t_string_1024;



#endif __STRING_H__