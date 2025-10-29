#ifndef __STRING_H__
#define __STRING_H__
#pragma once

#include "structures/array.h"
//#include "asserts.h"
#include <types/types.h>
#include <stdio.h>
#include <cstdarg>

const char k_null_char = '\0';

int32 expand_args_string(char* buffer, int32 size, const char* format, va_list args)
{
	 //doesn't count terminating '\0' even tho it will write one
	int32 length = _vscprintf(format, args) + 1;
	assert(length <= size);

	vsprintf_s(buffer, length, format, args);

	return length;
}

template<int32 k_max_size>
class c_string : public c_stack<char, k_max_size>
{
public:
	void print_va(const char* format, va_list args)
	{
		int length = expand_args_string(this->m_data, k_max_size, format, args);
		this->m_top = length - 1;
		terminate();
	}

	void print(const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		print_va(format, args);
		va_end(args);
	}

	void append_va(const char* format, va_list args)
	{
		assert(this->top() == k_null_char);

		// overwrite existing terminator
		int32 start = this->m_top;
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

	const char* get_const_char()
	{
		assert(this->top() == k_null_char);

		return &this->m_data[0];
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

#endif __STRING_H__