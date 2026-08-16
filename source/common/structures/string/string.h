#ifndef __STRING_H__
#define __STRING_H__
#pragma once

/*
custom string class and related functionality. for const char* functions, see str.h
*/

#include "debug/asserts.h"
#include "structures/array.h"
#include "structures/string/string_format.h"
#include "types/types.h"

template<class t_derived>
class c_string_base : public c_stack_base<char, t_derived>
{
public:
	using t_stack_base = c_stack_base<char, t_derived>;

	using t_stack_base::empty;

	void clear()
	{
		t_stack_base::clear();
	}

	int32 used() const 
	{ 
		return t_stack_base::used();
	}

	template<typename t_other>
	void copy_from(const c_string_base<t_other>& other)
	{
		ASSERT(other.is_terminated());
		t_stack_base::copy_from(other);
		ASSERT(is_terminated());
	}

	template<typename t_other>
	void copy_from_range(const c_string_base<t_other>& other, int32 start, int32 end)
	{
		ASSERT(other.is_terminated());
		t_stack_base::copy_from_range(other, start, end);
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
			t_stack_base::pop();
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
			t_stack_base::pop();
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

	void assert_valid() const
	{
		this->assert_valid_index(this->top_index());
		ASSERT(is_terminated());
	}

	void terminate()
	{
		if (!is_terminated())
		{
			this->push(k_null_char);
		}
	}
	
	template<typename t_other>
	bool contains(const c_string_base<t_other>& substring) const
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
			if (*t_stack_base::get_item_const(string_index) == *substring.get_item_const(substring_index))
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

	template<typename t_other>
	bool ends_with(const c_string_base<t_other>& substring) const
	{
		if (empty()) return false;

		int32 string_index = used() - (is_terminated() ? 2 : 1);
		int32 substring_index = substring.used() - (substring.is_terminated() ? 2 : 1);

		while (string_index >= 0 && substring_index >= 0)
		{
			if (*t_stack_base::get_item_const(string_index) != *substring.get_item_const(substring_index))
			{
				return false;
			}

			string_index--;
			substring_index--;
		}

		return true;
	}

	bool is_terminated() const
	{
		return t_stack_base::top() == k_null_char;
	}
	
	c_stack<char> as_stack() { return c_stack<char>(this->data(), this->capacity(), &this->top_index()); }
	operator c_stack<char>() { return as_stack(); }

private:
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
		format_arg(arg_format_buffer, first, this->as_stack());
		
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
			t_stack_base::push(*format++);
		}
	}

	template<typename t_type>
	inline void format_arg(t_arg_format_buffer format, const t_type& value, t_char_stack out)
	{
		const s_format_spec spec = parse_spec(format);
		s_string_formatter<t_type>::format(spec, value, out);
	}

};

class c_string_const : public c_string_base<c_string_const>
{
public:
	c_string_const() : m_data_reference(nullptr), m_size(k_invalid), m_top_reference(nullptr) {}
	explicit c_string_const(const char* data, int32 size, const int32* top) : m_data_reference(data), m_size(size), m_top_reference(top) {}

	const int32& top_index() const { return *m_top_reference; }

	// unusuable non-const c_string_base methods
	void clear() = delete;
	void print(const char* string) = delete;
	void append(const char* string) = delete;
	template<typename... t_args>
	void printf(const char* format, t_args... args) = delete;
	template<typename... t_args>
	void appendf(const char* format, t_args... args) = delete;
	template<typename t_other>
	void copy_from(const c_string_base<t_other>& other) = delete;
	template<typename t_other>
	void copy_from_range(const c_string_base<t_other>& other, int32 start, int32 end) = delete;
	void terminate() = delete;

private:
	const int32* m_top_reference;

	using t_type = char;
	ARRAY_DECLARE_REFERENCE_MEMBERS_CONST
};

class c_string : public c_string_base<c_string>
{
public:
	c_string() : m_data_reference(nullptr), m_size(k_invalid), m_top_reference(nullptr) {}
	explicit c_string(char* data, int32 size, int32* top) : m_data_reference(data), m_size(size), m_top_reference(top) {}

	int32& top_index() { return *m_top_reference; }
	const int32& top_index() const { return *m_top_reference; }

private:
	int32* m_top_reference;

	using t_type = char;
	ARRAY_DECLARE_REFERENCE_MEMBERS
};

template<int32 k_max_size>
class c_static_string : public c_string_base<c_static_string<k_max_size>>
{
public:
	using t_string_base = c_string_base<c_static_string<k_max_size>>;

	c_static_string() { this->clear(); }

	constexpr c_static_string(const char* string)
	{
		this->clear();
		this->print(string);
	}

	c_string as_string() { return c_string(this->data(), this->capacity(), &this->top_index()); }
	operator c_string() { return as_string(); }

	c_string_const as_string_const() const { return c_string_const(this->data(), this->capacity(), &this->top_index()); }
	operator c_string_const() const { return as_string_const(); }

	int32& top_index() { return m_top; }
	const int32& top_index() const { return m_top; }

private:
	int32 m_top;

	using t_type = char;
	ARRAY_DECLARE_STORAGE_MEMBERS
};

typedef c_static_string<128> t_string_128;
typedef c_static_string<256> t_string_256;
typedef c_static_string<512> t_string_512;
typedef c_static_string<1024> t_string_1024;

#endif __STRING_H__