// OH NO A MATH TEST! 
#include "pch.h"
#include "mmath.h"

TEST(MATH, MIN)
{
	EXPECT_EQ(math_min(3, 5), 3);
}

TEST(MATH, MAX)
{
	EXPECT_EQ(math_max(3, 5), 5);
}

TEST(MATH, PIN)
{
	EXPECT_EQ(math_pin(2, 5, 8), 5);
}

TEST(MATH, ABS)
{
	EXPECT_EQ(math_abs(-5), 5);
}

TEST(MATH, POW)
{
	EXPECT_EQ(math_pow(5, 3), 125);
	EXPECT_NEAR(math_pow(2.63f, 8), 2289.00f, 0.01f);
}

TEST(MATH, FACT)
{
	EXPECT_EQ(math_fact(0), 0);
	EXPECT_EQ(math_fact(1), 1);
	EXPECT_EQ(math_fact(6), 720);
}

TEST(MATH, FLOOR)
{
	EXPECT_EQ(math_floor(1.5f), 1.0f);
}

TEST(MATH, CEIL)
{
	EXPECT_EQ(math_ceil(1.5f), 2.0f);
}

TEST(MATH, SIN)
{
	const real32 tolerance = 0.001f; // 0.1%
	const int32 stress_iterations = 1000000;

	for (int32 i = 0; i < 1; i++)
	{
		EXPECT_NEAR(math_sin(0.0f), 0.0f, tolerance);
		EXPECT_NEAR(math_sin(k_math_real32_sixth_pi),						0.5f, tolerance);
		EXPECT_NEAR(math_sin(k_math_real32_quarter_pi),						0.707f, tolerance); // 1/rt(2)
		EXPECT_NEAR(math_sin(k_math_real32_third_pi),						0.866f, tolerance); // rt(3)/2 
		EXPECT_NEAR(math_sin(k_math_real32_half_pi),						1.0f, tolerance);
		EXPECT_NEAR(math_sin(k_math_real32_two_pi + k_math_real32_half_pi), 1.0f, tolerance);
		EXPECT_NEAR(math_sin(k_math_real32_pi),								0.0f, tolerance);
		EXPECT_NEAR(math_sin(((3*k_math_real32_pi)/2)),						-1.0f, tolerance);
		EXPECT_NEAR(math_sin(k_math_real32_two_pi), 0.0f, tolerance);
		EXPECT_NEAR(math_sin(k_math_real32_two_pi * 20),							0.0f, tolerance);
	}
}

TEST(MATH, COS)
{
	const real32 tolerance = 0.001f; // 0.1%

	EXPECT_NEAR(math_cos(0.0f),											1.0f, tolerance);
	EXPECT_NEAR(math_cos(k_math_real32_sixth_pi),						0.866f, tolerance); // rt(3)/2 
	EXPECT_NEAR(math_cos(k_math_real32_quarter_pi),						0.707f, tolerance); // 1/rt(2)
	EXPECT_NEAR(math_cos(k_math_real32_third_pi),						0.5f, tolerance); 
	EXPECT_NEAR(math_cos(k_math_real32_half_pi),						0.0f, tolerance);
	EXPECT_NEAR(math_cos(k_math_real32_two_pi + k_math_real32_half_pi), 0.0f, tolerance);
	EXPECT_NEAR(math_cos(k_math_real32_pi),								-1.0f, tolerance);
	EXPECT_NEAR(math_cos(((3 * k_math_real32_pi) / 2)),					0.0f, tolerance);
	EXPECT_NEAR(math_cos(k_math_real32_two_pi),							1.0f, tolerance);
}

TEST(MATH, NUM_DIGITS)
{
	EXPECT_EQ(math_digit_count(0), 0);
	EXPECT_EQ(math_digit_count(1), 1);
	EXPECT_EQ(math_digit_count(1234), 4);
	EXPECT_EQ(math_digit_count(k_int32_max), 10);
}

#define ENUM_RANGE(type, count) \
	bool in_range_enum(const type& value) \
	{ \
		return in_range_inclusive<type>(static_cast<type>(0), count - 1, value); \
	} 

enum e_test_dummy
{
	test_dummy_0,
	test_dummy_1,
	test_dummy_2,
	test_dummy_3,

	k_test_dummy_count
};
ENUM_MATH(e_test_dummy, k_test_dummy_count);

TEST(MATH, ENUM_ADD)
{
	e_test_dummy test = test_dummy_0;
	test = test + 1;
	EXPECT_EQ(test, test_dummy_1);
}

TEST(MATH, ENUM_SUBTRACT)
{
	e_test_dummy test = test_dummy_0;
	test = test + 1;
	EXPECT_EQ(test, test_dummy_1);
}

TEST(MATH, ENUM_INCREMENT)
{
	{
		// pre-fix
		e_test_dummy test = test_dummy_0;
		++test;
		EXPECT_EQ(test, test_dummy_1);
	}
	{
		// post-fix
		e_test_dummy test = test_dummy_0;
		test++;
		EXPECT_EQ(test, test_dummy_1);
	}
	{
		// pre-fix mid-use
		e_test_dummy test = test_dummy_0;
		EXPECT_EQ(++test, test_dummy_1);
		EXPECT_EQ(test, test_dummy_1);
	}
	{
		// post-fix mid-use
		e_test_dummy test = test_dummy_0;
		EXPECT_EQ(test++, test_dummy_0);
		EXPECT_EQ(test, test_dummy_1);
	}
}

TEST(MATH, ENUM_DECREMENT)
{
	{
		// pre-fix
		e_test_dummy test = test_dummy_1;
		--test;
		EXPECT_EQ(test, test_dummy_0);
	}
	{
		// post-fix
		e_test_dummy test = test_dummy_1;
		test--;
		EXPECT_EQ(test, test_dummy_0);
	}
	{
		// pre-fix mid-use
		e_test_dummy test = test_dummy_1;
		EXPECT_EQ(--test, test_dummy_0);
		EXPECT_EQ(test, test_dummy_0);
	}
	{
		// post-fix mid-use
		e_test_dummy test = test_dummy_1;
		EXPECT_EQ(test--, test_dummy_1);
		EXPECT_EQ(test, test_dummy_0);
	}
}

TEST(MATH, ENUM_RANGE)
{
	{
		e_test_dummy test = test_dummy_0;
		EXPECT_TRUE(in_range_enum(test));
		test--;
		EXPECT_FALSE(in_range_enum(test));
	}
	{
		e_test_dummy test = test_dummy_3;
		EXPECT_TRUE(in_range_enum(test));
		test++;
		EXPECT_FALSE(in_range_enum(test));
	}
}