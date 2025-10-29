#include "pch.h"
#include "types/types.h"
#include <structures/string.h>

TEST(STRING_TEST, STRING_TEST_PRINT_APPEND)
{
	const int32 k_max_size = 64;
	c_string<k_max_size> test_string;
	test_string.print("Hello %s!", "World");
	
	// "Hello World!\0" = 13
	int32 used = test_string.used();
	EXPECT_EQ(used, 13);
	EXPECT_TRUE(test_string.top() == '\0');
	EXPECT_STREQ("Hello World!", test_string.get_const_char());
	test_string.append(" The answer is %d.", 42);
	EXPECT_STREQ("Hello World! The answer is 42.", test_string.get_const_char());
}

TEST(STRING_TEST, STRING_TEST_ASSERTS)
{
	const int32 k_max_size = 16;
	c_string<k_max_size> test_string;

	// should fit
	test_string.print("Hello %s!", "Bob");

	// should not fit
	EXPECT_DEATH(test_string.print("This string is way too long %d", 12345), "Assertion failed: length <= size");
}