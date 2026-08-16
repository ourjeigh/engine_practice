#ifndef __STRING_FORMAT_H__
#define __STRING_FORMAT_H__
#pragma once

#include "debug/asserts.h"
#include "mmath.h"
#include "types/types.h"
#include "structures/string/str.h"

inline extern const char k_null_char;
const char k_string_format_char_begin = '{';
const char k_string_format_char_end = '}';

typedef c_static_stack<char, 16> t_arg_format_buffer;
typedef c_stack<char> t_char_stack;

constexpr int32 k_upper_to_lowercase_distance = math_abs('A' - 'a');

struct s_format_spec
{
	char type;
	int8 width;
	int8 precision;
};

// char helpers
inline bool is_whitespace(const char in)
{
	return in == ' ' || in == '\t' || in == '\r' || in == '\n';
}

inline bool is_letter(const char in)
{
	return in_range_inclusive('A', 'z', in);
}

inline bool is_uppercase(const char in)
{
	return in_range_inclusive('A', 'Z', in);
}

inline bool is_lowercase(const char in)
{
	return in_range_inclusive('a', 'z', in);
}

inline bool is_digit(const char in)
{
	return in_range_inclusive('0', '9', in);
}

inline char to_upper_char(const char in)
{
	if (is_lowercase(in))
	{
		return in - k_upper_to_lowercase_distance;
	}

	return in;
}

inline char to_lower_char(const char in)
{
	if (is_uppercase(in))
	{
		return in + k_upper_to_lowercase_distance;
	}

	return in;
}

// string helpers
inline int64 atoi(c_array<char> string)
{
	int64 out = 0;
	bool positive = true;;

	auto iter = string.begin();

	while (is_whitespace(*iter))
	{
		++iter;
	}

	if (*iter == '-')
	{
		positive = false;
		++iter;
	}

	for (; *iter != k_null_char && iter != string.end(); ++iter)
	{
		const int64 digit_min = 0;
		const int64 digit_max = 9;
		const int64 int_max_div_ten = k_int64_max / 10;

		const int64 current = (*iter) - '0';

		if (!in_range_inclusive(digit_min, digit_max, current))
		{
			break;
		}

		if (out > int_max_div_ten ||
			(out == int_max_div_ten && current > 7))
		{
			out = positive ? k_int64_max : k_int64_min;
			break;
		}

		out = out * 10 + current;
	}

	return out * (positive ? 1 : -1);
}

// formatters
inline void string_format_from_bool(bool input, int8 width, t_char_stack out_buffer)
{
	if (width == 0)
	{
		width = input ? 4 : 5;
	};
	
	ASSERT((input && width <= 4) || (!input && width <= 5));

	const char true_string[] = "true";
	const char false_string[] = "false";

	const char* out_string = input ? &true_string[0] : &false_string[0];

	for (int32 index = 0; index < width; index++)
	{
		out_buffer.push(out_string[index]);
	}
}

template<typename t_type>
inline void string_format_from_int(const t_type input, int8 width, t_char_stack out_buffer)
{
	constexpr int32 max_digits = math_digit_count(k_uint64_max);
	c_static_stack<char, max_digits> temp;
	uint64 u_input = input;

	if (input < 0)
	{
		u_input = input * -1;
		out_buffer.push('-');
	}

	do
	{
		const int8 digit = u_input % 10;
		const char c = '0' + digit;
		temp.push(c);

		u_input /= 10;
	} while (u_input);

	while (temp.used() < width)
	{
		// I'm not sure if we eventually want an option for whether to put a space or 0 here, but trying space
		// for now since it feels a little easier to read.
		temp.push(' ');
		//temp.push('0');
	}

	ASSERT(out_buffer.free() > temp.used());

	for (auto it = temp.begin_reverse(); it != temp.end_reverse(); --it)
	{
		out_buffer.push(*it);
	}
}

inline void string_format_from_int(const int64 input, int8 width, t_char_stack out_buffer)
{
	return string_format_from_int<int64>(input, width, out_buffer);
}

inline void string_format_from_uint(const uint64 input, int8 width, t_char_stack out_buffer)
{
	return string_format_from_int<uint64>(input, width, out_buffer);
}

template<typename t_type>
inline void string_format_to_hex(t_type input, int8 width, t_char_stack out_buffer)
{
	ASSERT(sizeof(t_type) <= 8);

	if (width == 0)
	{
		width = sizeof(t_type) * 2;
	}

	c_static_stack<char, 16> temp;

	uint64 u_input = input;

	if (input < 0)
	{
		constexpr uint64 bits = sizeof(t_type) * 8;
		constexpr uint64 wrap = math_pow<uint64>(2, bits);
		u_input = wrap + input;
	}

	while (temp.used() < width)
	{
		if (u_input)
		{
			const char conversion[] = "0123456789ABCDEF";

			int32 remainder = u_input % 16;
			temp.push(conversion[remainder]);

			u_input /= 16;
		}
		else
		{
			temp.push('0');
		}
	}

	for (auto it = temp.begin_reverse(); it != temp.end_reverse(); --it)
	{
		out_buffer.push(*it);
	}
}

inline void string_format_from_real(
	real64 input, 
	int8 width,
	int8 precision,
	t_char_stack out_buffer)
{
	if (input < 0.0f)
	{
		input *= -1.0f;
		out_buffer.push('-');
	}

	uint32 integer_part = real64_to_uint32(input);
	real64 decimal_part = input - integer_part;

	//eg {f7.3} == 030.104
	string_format_from_int(integer_part, width - precision - 1, out_buffer);
	out_buffer.push('.');

	while (precision > 0)
	{
		decimal_part *= 10;
		uint32 digit = real64_to_uint32(decimal_part);
		decimal_part -= digit;

		if (precision == 1)
		{
			// check if we need to round last digit up
			if ((decimal_part * 10) >= 5 && digit < 9)
			{
				digit++;
			}
		}

		ASSERT(digit < 10);
		char c = '0' + static_cast<char>(digit);
		ASSERT(is_digit(c));
		out_buffer.push(c);
		
		precision--;
	}
}

inline void string_format_from_char(const char input, t_char_stack out_buffer)
{
	out_buffer.push(input);
}

inline void string_format_from_string(const char* input, int8 width, t_char_stack out_buffer)
{
	int32 length = str_length(input);

	int32 pad = math_max<int32>(0, width - length);
	while (pad-- > 0)
	{
		out_buffer.push(' ');
	}

	while (*input != k_null_char)
	{
		out_buffer.push(*input++);
	}
}

inline void string_format_from_string_wide(const wchar* input, t_char_stack out_buffer)
{
	while (*input != k_null_char)
	{
		out_buffer.push(wchar_to_char(*input++));
	}
}

inline const void* to_format_ptr(const void* in)
{
	return in;
}

template<typename t_type>
struct s_string_formatter
{
	static_member_function void format(const s_format_spec& spec, const t_type& value, t_char_stack out)
	{
		// it may be an enum, try to parse as an int
		if (spec.type == 'i')
		{
			string_format_from_int(static_cast<int64>(value), spec.width, out);
		}
		else if(spec.type == 'x')
		{
			string_format_to_hex(static_cast<int64>(value), spec.width, out);
		}
		else
		{
			HALT_UNIMPLEMENTED();
		}
	}
};

template<>
struct s_string_formatter<bool>
{
	static_member_function void format(const s_format_spec& spec, const bool value, t_char_stack out)
	{
		ASSERT(spec.type == 'b');
		string_format_from_bool(value, spec.width, out);
	}
};

template<>
struct s_string_formatter<const wchar*>
{
	static_member_function void format(const s_format_spec& spec, const wchar* value, t_char_stack out)
	{
		ASSERT(spec.type == 's');
		string_format_from_string_wide(value, out);
	}
};

template<>
struct s_string_formatter<const char*>
{
	static_member_function void format(const s_format_spec& spec, const char* value, t_char_stack out)
	{
		ASSERT(spec.type == 's');
		string_format_from_string(value, spec.width, out);
	}
};

template<>
struct s_string_formatter<char*>
{
	static_member_function void format(const s_format_spec& spec, char* value, t_char_stack out)
	{
		s_string_formatter<const char*>::format(spec, value, out);
	}
};

template<>
struct s_string_formatter<int32>
{
	static_member_function void format(const s_format_spec& spec, int32 value, t_char_stack out)
	{
		if (spec.type == 'i')
		{
			string_format_from_int(value, spec.width, out);
		}
		else if (spec.type == 'x')
		{
			string_format_to_hex(value, spec.width, out);
		}
		else
		{
			HALT("Not a valid integer format");
		}
	}
};

template<>
struct s_string_formatter<int64>
{
	static_member_function void format(const s_format_spec& spec, int64 value, t_char_stack out)
	{
		if (spec.type == 'i')
		{
			string_format_from_int(value, spec.width, out);
		}
		else if (spec.type == 'x')
		{
			string_format_to_hex(value, spec.width, out);
		}
		else
		{
			HALT("Not a valid integer format");
		}
	}
};

template<>
struct s_string_formatter<uint32>
{
	static_member_function void format(const s_format_spec& spec, uint64 value, t_char_stack out)
	{
		// if we format a uint with 'i' this will halt, do we care?
		if (spec.type == 'u')
		{
			string_format_from_int(value, spec.width, out);
		}
		else if (spec.type == 'x')
		{
			string_format_to_hex(value, spec.width, out);
		}
		else
		{
			HALT("Not a valid integer format");
		}
	}
};

template<>
struct s_string_formatter<uint64>
{
	static_member_function void format(const s_format_spec& spec, uint64 value, t_char_stack out)
	{
		// if we format a uint with 'i' this will halt, do we care?
		if (spec.type == 'u')
		{
			string_format_from_int(value, spec.width, out);
		}
		else if (spec.type == 'x')
		{
			string_format_to_hex(value, spec.width, out);
		}
		else
		{
			HALT("Not a valid integer format");
		}
	}
};

template<>
struct s_string_formatter<real32>
{
	static_member_function void format(const s_format_spec& spec, real32 value, t_char_stack out)
	{
		ASSERT(spec.type == 'f');
		string_format_from_real(value, spec.width, spec.precision, out);
	}
};

template<>
struct s_string_formatter<real64>
{
	static_member_function void format(const s_format_spec& spec, real64 value, t_char_stack out)
	{
		ASSERT(spec.type == 'f');
		string_format_from_real(value, spec.width, spec.precision, out);
	}
};

template<>
struct s_string_formatter<const void*>
{
	static_member_function void format(const s_format_spec& spec, const void* value, t_char_stack out)
	{
		ASSERT(spec.type == 'p');
		string_format_to_hex(reinterpret_cast<uint64>(value), spec.width, out);
	}
};

inline void parse_format_buffer(const char* format, t_arg_format_buffer& out_buffer)
{
	while (*format != k_null_char && *format != k_string_format_char_end)
	{
		out_buffer.push(*format++);
	}
}

inline bool is_valid_format_type(const char type)
{
	bool is_valid = false;

	switch (type)
	{
	case 's':
	case 'f':
	case 'i':
	case 'u':
	case 'b':
	case 'x':
	case 'p':
		is_valid = true;
		break;
	}

	return is_valid;
}

inline s_format_spec parse_spec(t_arg_format_buffer& format)
{
	s_format_spec spec;
	zero_object(spec);

	// we are expecting a valid format to look something like this
	// f2.1
	// i4
	// h12
	// b
	// p

	spec.type = format[0];
	ASSERT(is_valid_format_type(spec.type));
	
	if (format.used() > 1)
	{
		int32 decimal_index = 1;
		for (; decimal_index < format.used(); decimal_index++)
		{
			if (format[decimal_index] == '.')
			{
				break;
			}
		}

		if (decimal_index > 1)
		{
			spec.width = int64_to_int8(atoi(format.make_sub_array(1, decimal_index)));
		}

		if (decimal_index < format.used())
		{
			spec.precision = int64_to_int8(atoi(format.make_sub_array(decimal_index + 1, format.used())));
		}
	}

	return spec;

}
#endif //__STRING_FORMAT_H__