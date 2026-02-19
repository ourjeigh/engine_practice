#ifndef __STRING_H__
#define __STRING_H__
#pragma once

#include "structures/array.h"
#include <types/types.h>

#include "platform/platform.h"

IGNORE_WINDOWS_WARNINGS_PUSH
// TODO: remove
#include <stdio.h>
#include <cstdarg>
IGNORE_WINDOWS_WARNINGS_POP

const char k_null_char = '\0';

inline int32 expand_args_string(char* buffer, int32 size, const char* format, va_list args)
{
	 //doesn't count terminating '\0' even tho it will write one
	int32 length = _vscprintf(format, args) + 1;
	ASSERT(length <= size);

	vsprintf_s(buffer, length, format, args);

	return length;
}

// TODO make this c_static_string and make it have a c_stack<char, k_max_size>
// then make the base c_string have all the functionality and take in a data member

template<int32 k_max_size>
class c_string : public c_stack<char, k_max_size>
{
public:
	c_string() { this->clear(); }

	constexpr c_string(const char* string)
	{
		this->clear();
		print(string);
	}

	void print_va(const char* format, va_list args)
	{
		int length = expand_args_string(this->m_data, k_max_size, format, args);
		this->m_top = length - 1;
		terminate();
	}

	void print(const char* string)
	{
		if (string != nullptr)
		{
			while (*string != k_null_char)
			{
				this->push(*string++);
			}
		}
	}

	void printf(const char* format, ...)
	{
		// TODO: write custom va_args
		va_list args;
		va_start(args, format);
		print_va(format, args);
		va_end(args);
	}

	void append_va(const char* format, va_list args)
	{
		ASSERT(this->empty() || this->top() == k_null_char);

		int32 start = this->empty() ? 0 : this->m_top;
		int32 size_left = k_max_size - start;
		int lenth = expand_args_string(&this->m_data[start], size_left, format, args);
		this->m_top += lenth - 1;
		terminate();
	}

	void append(const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		append_va(format, args);
		va_end(args);
	}

	const char* get_const_char() const
	{
		if (this->empty())
		{
			return nullptr;
		}

		return this->m_data;
	}

	void HACK_post_copy_fixup(int32 chars_written)
	{
		if (chars_written == k_max_size)
		{
			chars_written--;
		}

		this->assert_valid_index(chars_written);
		this->m_top = chars_written;
		terminate();
	}

	void assert_valid()
	{
		assert_valid_index(this->m_top);
		ASSERT(this->top() == k_null_char);
	}

private:
	void terminate()
	{
		if (this->top() != k_null_char)
		{
			this->push(k_null_char);
		}
	}
};

typedef c_string<128> t_string_128;
typedef c_string<256> t_string_256;
typedef c_string<512> t_string_512;
typedef c_string<1024> t_string_1024;

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