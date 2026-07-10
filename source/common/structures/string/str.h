#ifndef __STR_H__
#define __STR_H__
#pragma once

#include "types/types.h"
#include "debug/asserts.h"

const char k_null_char = '\0';

/// <summary>
/// </summary>
/// <param name="left"></param>
/// <param name="right"></param>
/// <returns>
/// 0	They compare equal
/// <0	Either the value of the first character that does not match is lower in the compared string, or all compared characters match but the compared string is shorter.
/// >0	Either the value of the first character that does not match is greater in the compared string, or all compared characters match but the compared string is longer.
/// </returns>
constexpr int32 str_compare(const char* left, const char* right)
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

constexpr bool str_equal(const char* left, const char* right)
{
	return str_compare(left, right) == 0;
}

constexpr int32 str_length(const char* string)
{
	ASSERT(string != nullptr);

	int32 out_length = 0;
	while (string[out_length] != k_null_char && out_length < k_int32_max)
	{
		out_length++;
	}

	return out_length;
}

#endif //__STR_H__