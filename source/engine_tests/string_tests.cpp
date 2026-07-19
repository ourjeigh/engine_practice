#include "pch.h"
#include "types/types.h"
#include "structures/string/string.h"
#include "structures/string/string_id.h"

const char* k_int64_max_string =   "9223372036854775807";
const char* k_int64_min_string =  "-9223372036854775808";
const char* k_uint64_max_string = "18446744073709551615";

TEST(STRING, STR_COMPARE)
{
	const char* string1 = "hello world";
	const char* string2 = "hello world";

	EXPECT_EQ(str_compare(string1, string2), 0);
}

TEST(STRING_TEST, STRING_TEST_PRINT)
{
	const char* test_string = "hey this is a test string";
	t_string_128 string;
	string.print(test_string);

	EXPECT_EQ(str_compare(test_string, string.get_const_char()), 0);
}

TEST(STRING_FORMAT, ATOI_VALID_INPUTS)
{
	EXPECT_EQ(atoi(t_string_128("0").make_array()), 0);
	EXPECT_EQ(atoi(t_string_128("1").make_array()), 1);
	EXPECT_EQ(atoi(t_string_128("   \t   -4\r").make_array()), -4);
	EXPECT_EQ(atoi(t_string_128("12345").make_array()), 12345);
	EXPECT_EQ(atoi(t_string_128("-5283451235632").make_array()), -5283451235632);

	EXPECT_EQ(atoi(t_string_128(k_int64_min_string).make_array()), k_int64_min);
	EXPECT_EQ(atoi(t_string_128(k_int64_max_string).make_array()), k_int64_max);

	EXPECT_EQ(atoi(t_string_128("9223372036854775806").make_array()), k_int64_max -1);
	EXPECT_EQ(atoi(t_string_128("-9223372036854775807").make_array()), k_int64_min + 1);
}

TEST(STRING_FORMAT, ATOI_INVALID_INPUTS)
{
	EXPECT_EQ(atoi(t_string_128("poop").make_array()), 0);

	EXPECT_EQ(atoi(t_string_128("9223372036854775808").make_array()), k_int64_max);
	EXPECT_EQ(atoi(t_string_128("-9223372036854775809").make_array()), k_int64_min);

	EXPECT_EQ(atoi(t_string_128(k_uint64_max_string).make_array()), k_int64_max);
}

TEST(STRING_FORMAT, CHAR_CHECKS)
{
	EXPECT_TRUE(is_uppercase('A'));
	EXPECT_TRUE(is_uppercase('P'));
	EXPECT_TRUE(is_uppercase('Z'));

	EXPECT_TRUE(is_lowercase('a'));
	EXPECT_TRUE(is_lowercase('p'));
	EXPECT_TRUE(is_lowercase('z'));

	EXPECT_TRUE(is_letter('a'));
	EXPECT_TRUE(is_letter('Q'));
	EXPECT_TRUE(is_letter('r'));
	EXPECT_TRUE(is_letter('Z'));

	EXPECT_TRUE(is_digit('0'));
	EXPECT_TRUE(is_digit('1'));
	EXPECT_TRUE(is_digit('8'));
	EXPECT_TRUE(is_digit('9'));

	EXPECT_TRUE(is_whitespace(' '));
	EXPECT_TRUE(is_whitespace('	'));
	EXPECT_TRUE(is_whitespace('\n'));
	EXPECT_TRUE(is_whitespace('\r'));

	EXPECT_FALSE(is_uppercase('a'));
	EXPECT_FALSE(is_uppercase('8'));
	EXPECT_FALSE(is_uppercase('	'));

	EXPECT_FALSE(is_lowercase('M'));
	EXPECT_FALSE(is_lowercase('Z'));
	EXPECT_FALSE(is_lowercase('7'));

	EXPECT_FALSE(is_letter(')'));
	EXPECT_FALSE(is_letter('+'));
	EXPECT_FALSE(is_letter('5'));
}

TEST(STRING_FORMAT, SWITCH_CASE)
{
	EXPECT_EQ('a', to_lower_char('A'));
	EXPECT_EQ('p', to_lower_char('P'));
	EXPECT_EQ('a', to_lower_char('a'));
	EXPECT_EQ('-', to_lower_char('-'));

	EXPECT_EQ('A', to_upper_char('a'));
	EXPECT_EQ('M', to_upper_char('m'));
	EXPECT_EQ('Z', to_upper_char('z'));
	EXPECT_EQ('R', to_upper_char('R'));
	EXPECT_EQ(']', to_upper_char(']'));
}

TEST(STRING_FORMAT, PARSE_FORMAT)
{
	t_arg_format_buffer format;
	parse_format_buffer("i2.1", format);

	EXPECT_EQ(format.used(), 4);
}

TEST(STRING_FORMAT, PARSE_SPEC)
{
	t_arg_format_buffer format;
	parse_format_buffer("i2.1", format);
	
	s_format_spec spec = parse_spec(format);

	EXPECT_EQ(spec.type, 'i');
	EXPECT_EQ(spec.width, 2);
	EXPECT_EQ(spec.precision, 1);
}

TEST(STRING_TEST, STRING_TEST_PRINTF_STRING)
{
	const int32 k_max_size = 64;
	c_static_string<k_max_size> test_string;
	test_string.printf("Hello {s}!", "World");

	int32 used = test_string.used();
	EXPECT_EQ(used, 13);
	EXPECT_TRUE(test_string.top() == '\0');
	EXPECT_STREQ("Hello World!", test_string.get_const_char());
}

TEST(STRING_TEST, STRING_TEST_PRINTF_INT)
{
	const int32 k_max_size = 64;
	c_static_string<k_max_size> test_string;
	test_string.printf("The answer is {i}.", 42L);
	EXPECT_STREQ("The answer is 42.", test_string.get_const_char());
}

TEST(STRING_TEST, STRING_TEST_PRINTF_APPEND)
{
	const int32 k_max_size = 64;
	c_static_string<k_max_size> test_string;
	test_string.printf("Hello {s}!", "World");
	//test_string.printf("Hello %s!", "World");

	// "Hello World!\0" = 13
	int32 used = test_string.used();
	EXPECT_EQ(used, 13);
	EXPECT_TRUE(test_string.top() == '\0');
	EXPECT_STREQ("Hello World!", test_string.get_const_char());
	//test_string.appendf(" The answer is %d.", 42);
	test_string.appendf(" The answer is {i}.", 42L);
	EXPECT_STREQ("Hello World! The answer is 42.", test_string.get_const_char());
}

TEST(STRING_TEST, STRING_TEST_ASSERTS)
{
	const int32 k_max_size = 16;
	c_static_string<k_max_size> test_string;

	// should fit
	test_string.printf("Hello {s}!", "Bob");
	//test_string.printf("Hello %s!", "Bob");

	// should not fit
	//EXPECT_DEATH(test_string.printf("This string is way too long %d", 12345), ".*");
	EXPECT_DEATH(test_string.printf("This string is way too long {d}", 12345), ".*");
}

TEST(STRING, STRING_PRINT_FIXED_LENGTH)
{
	t_string_128 test_string;
	test_string.printf("Hello {s5}.", "Bob");
	
	t_string_128 expected_string = "Hello   Bob.";

	EXPECT_TRUE(test_string == expected_string);
}

TEST(STRING_TEST, STRING_COMPARE_EQUALS)
{
	const char* test1 = "This is a test string";
	const char* test2 = "This is a test string";

	EXPECT_EQ(str_compare(test1, test2), 0);
}

TEST(STRING_TEST, STRING_COMPARE_UNEQUAL)
{
	const char* test1 = "This is test string 1";
	const char* test2 = "This is test string 2";

	EXPECT_EQ(str_compare(test1, test2), 1);
}

TEST(STRING_TEST, STRING_COMPARE_SHORTER)
{
	const char* test1 = "This is test string 1";
	const char* test2 = "This is test";

	EXPECT_EQ(str_compare(test1, test2), -1);
}

TEST(STRING_TEST, STRING_COMPARE_LONGER)
{
	const char* test1 = "This is test string 1";
	const char* test2 = "This is test string 11";

	EXPECT_EQ(str_compare(test1, test2), 1);
}

TEST(STRING_TEST, STRING_COMPARE_NULL)
{
	const char* test1 = "This is test string 1";
	const char* test2 = "This is test string 11";

	EXPECT_EQ(str_compare(nullptr, nullptr), 0);
	EXPECT_GE(str_compare(nullptr, test1), 1);
}

TEST(STRING, STRING_COMPARE_TOO_BIG)
{
	char* long_string1 = new char[k_int32_max];
	char* long_string2 = new char[k_int32_max];

	for (uint64 i = 0; i < k_uint16_max + 10; i++)
	{
		char new_char = i % k_char_max;
		if (new_char == k_null_char)
		{
			new_char++;
		}

		long_string1[i] = new_char;
		long_string2[i] = new_char;
	}

	EXPECT_DEATH(str_compare(long_string1, long_string2), ".*");

	delete[] long_string1;
	delete[] long_string2;
}

TEST(STRING, STRING_LENGTH)
{
	EXPECT_DEATH(str_length(nullptr), ".*");
	EXPECT_EQ(str_length("hello"), 5);
	EXPECT_EQ(str_length("hello world"), 11);
	EXPECT_NE(str_length("hello world"), 3);

	COMPILE_ASSERT(str_length("hello") == 5);
}

TEST(STRING, STRING_CONTAINS)
{
	t_string_128 test("hello madam i'm adam");
	t_string_128 match("mad");
	t_string_128 miss("madan");

	EXPECT_TRUE(test.contains(match));
	EXPECT_FALSE(test.contains(miss));
}

TEST(STRING, STRING_ENDS_WITH)
{
	t_string_128 test("hello madam i'm adam");
	t_string_128 match("adam");
	t_string_128 miss("madan");

	EXPECT_TRUE(test.ends_with(match));
	EXPECT_FALSE(test.ends_with(miss));
}

TEST(STRING_ID, STRING_ID_COMPILED)
{
	DECLARE_STRING_ID(test, "test string");
	COMPILE_ASSERT(test.get_id() != -1);

#ifdef _DEBUG
	EXPECT_EQ(str_compare(test.get_debug_string(), "test string"), 0);
#endif //_DEBUG
}

TEST(STRING_ID, EQUALS)
{
	DECLARE_STRING_ID(test_string1, "hello world");
	DECLARE_STRING_ID(test_string2, "hello world");
	DECLARE_STRING_ID(test_string3, "hello world!!!");

	COMPILE_ASSERT(test_string1 == test_string2);
	COMPILE_ASSERT(test_string2 != test_string3);

	EXPECT_EQ(test_string1, test_string2);
	EXPECT_NE(test_string1, test_string3);
}

TEST(STRING_FORMAT, BOOL)
{
	t_string_128 string;

	string_format_from_bool(true, 0, string);
	string.terminate();
	EXPECT_TRUE(str_equal("true", string.get_const_char()));
	string.clear();

	string_format_from_bool(false, 0, string);
	string.terminate();
	EXPECT_TRUE(str_equal("false", string.get_const_char()));
	string.clear();

	string_format_from_bool(true, 1, string);
	string.terminate();
	EXPECT_TRUE(str_equal("t", string.get_const_char()));
	string.clear();

	string_format_from_bool(false, 1, string);
	string.terminate();
	EXPECT_TRUE(str_equal("f", string.get_const_char()));
	string.clear();
}

TEST(STRING_FORMAT, INT)
{
	t_string_128 string;

	string_format_from_int(0, 0, string);
	string.terminate();
	EXPECT_TRUE(str_equal("0", string.get_const_char()));
	string.clear();

	string_format_from_int(256, 0, string);
	string.terminate();
	EXPECT_TRUE(str_equal("256", string.get_const_char()));
	string.clear();

	string_format_from_int(-256, 0, string);
	string.terminate();
	EXPECT_TRUE(str_equal("-256", string.get_const_char()));
	string.clear();

	string_format_from_int(k_int64_max, 0, string);
	string.terminate();
	EXPECT_TRUE(str_equal("9223372036854775807", string.get_const_char()));
	string.clear();

	string_format_from_int(k_int64_min, 0, string);
	string.terminate();
	EXPECT_TRUE(str_equal("-9223372036854775808", string.get_const_char()));
	string.clear();

	string_format_from_int(k_uint64_max, 0, string);
	string.terminate();
	EXPECT_TRUE(str_equal("18446744073709551615", string.get_const_char()));
}

TEST(STRING_FORMAT, REAL64)
{
	real64 test = -234512.320004;
	t_string_128 string;
	string_format_from_real(test, 0, 6, string);
	string.terminate();

	EXPECT_TRUE(str_equal("-234512.320004", string.get_const_char()));
}

TEST(STRING_FORMAT, HEX)
{
	t_string_128 string;

	{
		int8 input = -1;
		string_format_to_hex(input, 0, string);
		string.terminate();
		EXPECT_TRUE(str_equal("FF", string.get_const_char()));
		string.clear();
	}

	{
		int16 input = -1;
		string_format_to_hex(input, 0, string);
		string.terminate();
		EXPECT_TRUE(str_equal("FFFF", string.get_const_char()));
		string.clear();

		input = 757;
		string_format_to_hex(input, 0, string);
		string.terminate();
		EXPECT_TRUE(str_equal("02F5", string.get_const_char()));
		string.clear();

		input = -4870;
		string_format_to_hex(input, 0, string);
		string.terminate();
		EXPECT_TRUE(str_equal("ECFA", string.get_const_char()));
		string.clear();
	}

	{
		int32 input = -1;
		string_format_to_hex(input, 0, string);
		string.terminate();
		EXPECT_TRUE(str_equal("FFFFFFFF", string.get_const_char()));
		string.clear();

		input = -4870;
		string_format_to_hex(input, 0, string);
		string.terminate();
		EXPECT_TRUE(str_equal("FFFFECFA", string.get_const_char()));
		string.clear();

		input = 10;
		string_format_to_hex(input, 0, string);
		string.terminate();
		EXPECT_TRUE(str_equal("0000000A", string.get_const_char()));
		string.clear();

		input = 256;
		string_format_to_hex(input, 0, string);
		string.terminate();
		EXPECT_TRUE(str_equal("00000100", string.get_const_char()));
		string.clear();

		input = -487294710;
		string_format_to_hex(input, 0, string);
		string.terminate();
		EXPECT_TRUE(str_equal("E2F4790A", string.get_const_char()));
		string.clear();
	}
}