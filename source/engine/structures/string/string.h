#ifndef __STRING_H__
#define __STRING_H__
#pragma once

#include "debug/asserts.h"
#include "structures/array.h"
#include <types/types.h>
#include "structures/string/string_format.h"
#include "platform/platform.h"

IGNORE_WINDOWS_WARNINGS_PUSH
// TODO: remove
//#include <stdio.h>
//#include <cstdarg>
IGNORE_WINDOWS_WARNINGS_POP

const char k_null_char = '\0';

//inline int32 expand_args_string(char* buffer, int32 size, const char* format, va_list args)
//{
//	 //doesn't count terminating '\0' even tho it will write one
//	int32 length = _vscprintf(format, args) + 1;
//	ASSERT(length <= size);
//
//	//vsprintf_s(buffer, size, format, args);
//	vsprintf_s(buffer, length, format, args);
//
//	return length;
//}
template<int32 k_max_size>
class c_static_string;

class c_string : public c_stack<char>
{
public:
	c_string() { this->clear(); }

	explicit c_string(char* data, int32 size) : c_stack<char>(data, size)
	{ 
		memory_set(this->data(), k_null_char, this->capacity());
	}

	//void print_va(const char* format, va_list args)
	//{
	//	int length = expand_args_string(this->data(), this->capacity(), format, args);
	//	this->m_top = length - 1;
	//	terminate();
	//}

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
		assert_valid_index(this->m_top);
		ASSERT(this->top() == k_null_char);
	}

	void terminate()
	{
		if (this->top() != k_null_char)
		{
			this->push(k_null_char);
		}
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
	c_static_string() : c_string(m_data, k_max_size) {}

	constexpr c_static_string(const char* string) : c_string(m_data, k_max_size)
	{
		this->clear();
		print(string);
	}

private:
	char m_data[k_max_size];
};

typedef c_static_string<128> t_string_128;
typedef c_static_string<256> t_string_256;
typedef c_static_string<512> t_string_512;
typedef c_static_string<1024> t_string_1024;

/// <summary>
/// </summary>
/// <param name="left"></param>
/// <param name="right"></param>
/// <returns>
/// 0	They compare equal
/// <0	Either the value of the first character that does not match is lower in the compared string, or all compared characters match but the compared string is shorter.
/// >0	Either the value of the first character that does not match is greater in the compared string, or all compared characters match but the compared string is longer.
/// </returns>
constexpr int32 string_compare(const char* left, const char* right)
{
	if (left == nullptr || right == nullptr)
	{
		if (left != nullptr)
		{
			return -1;
		}
		else if (right != nullptr)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	const uint64 max_char_count = k_uint16_max;
	uint64 char_count = 0;

	while (*left != k_null_char && *right != k_null_char)
	{
		int32 diff = *right - *left;

		if (diff != 0)
		{
			return diff;
		}

		// that's a pretty long string buddy
		ASSERT(++char_count < max_char_count);

		left++;
		right++;
	}

	if (*left != k_null_char)
	{
		return -1;
	}

	if (*right != k_null_char)
	{
		return 1;
	}

	return 0;
}

constexpr bool are_strings_equal(const char* left, const char* right)
{
	return string_compare(left, right) == 0;
}

constexpr int32 string_length(const char* string)
{
	ASSERT(string != nullptr);

	int32 out_length = 0;
	while (string[out_length] != k_null_char && out_length < k_int32_max)
	{
		out_length++;
	}

	return out_length;
}

#endif __STRING_H__