#include "pch.h"
#include "types/types.h"
#include "structures/array.h"

TEST(C_ARRAY, C_ARRAY_COPY_CONSTRUCTOR)
{
	c_static_array<int32, 5> array1;
	for (int32 i = 0; i < 5; ++i)
	{
		array1[i] = i * 2;
	}

	c_static_array<int32, 5> array2;
	array2.copy_from(array1);

	for (int32 i = 0; i < 5; ++i)
	{
		EXPECT_EQ(array1[i], array2[i]);
	}
}

TEST(C_ARRAY, C_ARRAY_COPY)
{
	c_static_array<int32, 5> array1;
	for (int32 i = 0; i < 5; ++i)
	{
		array1[i] = i * 2;
	}

	c_static_array<int32, 5> array2;

	array2 = array1;
	for (int32 i = 0; i < 5; ++i)
	{
		EXPECT_EQ(array1[i], array2[i]);
	}
}

TEST(C_ARRAY, C_ARRAY_SIZE)
{
	const int32 size = 10;
	c_static_array<real32, size> array;
	EXPECT_EQ(size, array.capacity());
}

TEST(C_ARRAY, C_ARRAY_ITERATORS)
{
	const int32 size = 5;
	c_static_array<int32, size> array;
	for (int32 i = 0; i < size; ++i)
	{
		array[i] = i * 10;
	}

	int32 index = 0;

	// check pre increment
	for (auto it = array.begin(); it != array.end(); ++it)
	{
		EXPECT_EQ(array[index], *it);
		index++;
	}

	index = 0;
	// check post increment
	for (auto it = array.begin(); it != array.end(); it++)
	{
		EXPECT_EQ(array[index], *it);
		index++;
	}
}

TEST(C_ARRAY, C_ARRAY_ASSERTS)
{
	const int32 size = 10;
	c_static_array<real32, size> array;
	EXPECT_DEATH({ array[12] = 5.0f; }, ".*");
}

TEST(ARRAY, COPY_FROM_SAME_TYPE)
{
	c_static_array<int32, 10> array1 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

	c_static_array<int32, 10> array2;

	array2.copy_from_range(array1, 0, 10);

	for (int32 i = 0; i < 10; i++)
	{
		EXPECT_EQ(array2[i], i + 1);
	}
}

TEST(ARRAY, COPY_FROM_DIFF_TYPE)
{
	c_static_array<int32, 10> array1 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

	c_static_array<int32, 5> array2;

	array2.copy_from_range(array1, 0, 5);

	for (int32 i = 0; i < 5; i++)
	{
		EXPECT_EQ(array2[i], i + 1);
	}
}

TEST(ARRAY, COPY_FROM_REF)
{
	c_static_array<int32, 10> array1 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	c_static_array<int32, 5> array2;

	array2.copy_from_range(array1, 0, 5);

	for (int32 i = 0; i < 5; i++)
	{
		EXPECT_EQ(array2[i], i + 1);
	}
}

TEST(ARRAY, COPY_FROM_OFFSET)
{
	c_static_array<int32, 10> array1 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

	c_static_array<int32, 5> array2;

	array2.copy_from_range(array1, 2, 7);

	for (int32 i = 0; i < 5; i++)
	{
		EXPECT_EQ(array2[i], i + 3);
	}
}

TEST(ARRAY, COPY_FROM_BAD_RANGE)
{
	c_static_array<int32, 10> array1 = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

	c_static_array<int32, 5> array2;

	EXPECT_DEATH(array2.copy_from_range(array1, 2, 8), ".*");
}

TEST(ARRAY, COPY_FROM_RANGE_OFFSET)
{
	c_static_array<int32, 10> array1 = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	c_static_array<int32, 10> array2;
	array2.zero_data();
	//memory_zero(array2.data(), sizeof(int32) * array2.capacity());

	const int32 start = 2;
	const int32 end = 5;
	const int32 offset = 2;
	c_static_array<int32, 10> expected = { 0, 0, 2, 3, 4, 0, 0, 0, 0, 0 };

	array2.copy_from_range_offset(array1, start, end, offset);

	for (int32 i = 0; i < array2.capacity(); i++)
	{
		EXPECT_EQ(array2[i], expected[i]);
	}
}

TEST(ARRAY, EQUALS_OPERATOR)
{
	const c_static_array<int32, 10> array_match_1 = { 1, 2, 3, 4 , 5, 6, 7, 8, 9, 10 };
	const c_static_array<int32, 10> array_match_2 = { 1, 2, 3, 4 , 5, 6, 7, 8, 9, 10 };
	const c_static_array<int32, 10> array_no_match = { 1, 2, 3, 4 , 5, 6, 7, 8, 4, 10};

	EXPECT_EQ(array_match_1, array_match_2);
	EXPECT_NE(array_match_1, array_no_match);
}

TEST(C_STACK, C_STACK_PUSH_POP_TOP)
{
	const int32 size = 3;
	c_static_stack<real32, size> stack;

	EXPECT_TRUE(stack.empty());

	stack.push(7.0f);
	EXPECT_FALSE(stack.empty());
	EXPECT_EQ(stack.top(), 7.0f);

	stack.push(1.0f);
	stack.push(3.0f);

	EXPECT_EQ(stack.used(), 3);
	EXPECT_TRUE(stack.full());

	stack.pop();
	stack.pop();
	stack.pop();
	EXPECT_TRUE(stack.empty());
}

TEST(C_STACK, PUSH_FROM)
{
	c_static_stack<int32, 5> stack;
	c_static_array<int32, 3> data1 = { 1,2,3 };

	EXPECT_TRUE(stack.empty());
	stack.push_from(data1);

	EXPECT_EQ(stack.used(), data1.capacity());
}

TEST(C_STACK, C_STACK_ITERATORS)
{
	const int32 size = 10;
	c_static_stack<int32, size> stack;
	stack.push(10);
	stack.push(20);
	stack.push(30);
	int32 expected_values[] = { 10, 20, 30 };
	int32 index = 0;
	for (auto it = stack.begin(); it != stack.end(); ++it)
	{
		EXPECT_EQ(expected_values[index], *it);
		index++;
	}
	EXPECT_EQ(index, 3);
}

TEST(C_STACK, C_STACK_ASSERTS)
{
	const int32 size = 2;
	c_static_stack<real32, size> stack;

	EXPECT_DEATH(stack.top(), ".*");
	EXPECT_DEATH(stack.pop(), ".*");

	stack.push(0);
	stack.push(0);

	EXPECT_DEATH(stack.push(0), ".*");
}

TEST(C_STACK, ASSIGNMENT_PRESERVES_TOP)
{
	int32 source_buffer[8];
	int32 target_buffer[8];
	c_stack<int32> source(source_buffer, 8);
	c_stack<int32> target(target_buffer, 8);

	source.push(10);
	source.push(20);
	source.push(30);

	target = source;

	EXPECT_EQ(target.used(), 3);
	EXPECT_EQ(target.top(), 30);
	target.pop();
	EXPECT_EQ(target.top(), 20);
	target.pop();
	EXPECT_EQ(target.top(), 10);
	target.pop();
	EXPECT_TRUE(target.empty());
}

TEST(C_STATIC_STACK, TO_C_STACK_PRESERVES_TOP)
{
	c_static_stack<int32, 8> source;
	source.push(1);
	source.push(2);
	source.push(3);

	c_stack<int32> target = source;

	EXPECT_EQ(target.used(), source.used());
	EXPECT_FALSE(target.empty());
	EXPECT_EQ(target.top(), 3);
	target.pop();
	EXPECT_EQ(target.top(), 2);
	target.pop();
	EXPECT_EQ(target.top(), 1);
	target.pop();
	EXPECT_TRUE(target.empty());
}

TEST(C_STACK, COPY_BETWEEN_TYPES)
{
	c_static_stack<int32, 8> source;
	source.push(100);
	source.push(200);
	source.push(300);

	int32 target_buffer[8];
	c_stack<int32> target(target_buffer, 8);
	target.copy_from(source);

	EXPECT_EQ(target.used(), source.used());
	EXPECT_EQ(target.top(), 300);
	target.pop();
	EXPECT_EQ(target.top(), 200);
	target.pop();
	EXPECT_EQ(target.top(), 100);
}

TEST(C_STACK, PUSH_FROM_ARRAY)
{
	c_static_array<int32, 3> source(10, 20, 30);
	c_static_stack<int32, 8> target;

	target.push_from(source);

	EXPECT_EQ(target.used(), 3);
	EXPECT_EQ(target.top(), 30);
	target.pop();
	EXPECT_EQ(target.top(), 20);
	target.pop();
	EXPECT_EQ(target.top(), 10);
}

TEST(C_STACK, PUSH_FROM_APPEND)
{
	c_static_array<int32, 3> source(4, 5, 6);
	c_static_stack<int32, 8> target;

	target.push(1);
	target.push(2);
	target.push(3);
	target.push_from(source);

	EXPECT_EQ(target.used(), 6);
	EXPECT_EQ(target.top(), 6);
	target.pop();
	EXPECT_EQ(target.top(), 5);
	target.pop();
	EXPECT_EQ(target.top(), 4);
	target.pop();
	EXPECT_EQ(target.top(), 3);
}

TEST(C_STACK, COPY_FROM_RANGE)
{
	c_static_stack<int32, 8> source;
	source.push(10);
	source.push(20);
	source.push(30);
	source.push(40);
	source.push(50);

	int32 target_buffer[8];
	c_stack<int32> target(target_buffer, 8);
	target.copy_from_range(source, 1, 4);

	EXPECT_EQ(target.used(), 3);
	EXPECT_EQ(target[0], 20);
	EXPECT_EQ(target[1], 30);
	EXPECT_EQ(target.top(), 40);
}

TEST(C_BIT_ARRAY, SET_TEST)
{
	enum e_test_enum
	{
		test_enum_val_1,
		test_enum_val_2,
		test_enum_val_3,
		test_enum_val_4,

		k_test_enum_count
	};
	typedef c_bit_array<k_test_enum_count> t_test_flags;
	t_test_flags flags;

	EXPECT_TRUE(flags.none());

	flags.set(test_enum_val_2, true);

	EXPECT_TRUE(flags.test(test_enum_val_2));
	EXPECT_FALSE(flags.test(test_enum_val_1));

	flags.clear();
	EXPECT_FALSE(flags.any());
}

TEST(C_BIT_ARRAY, TEST_ALL)
{
	enum e_test_enum
	{
		test_enum_val_1,
		test_enum_val_2,
		test_enum_val_3,
		test_enum_val_4,

		k_test_enum_count
	};
	typedef c_bit_array<k_test_enum_count> t_test_flags;
	t_test_flags flags;
	flags.set(test_enum_val_1, true);
	flags.set(test_enum_val_2, true);
	flags.set(test_enum_val_3, true);
	flags.set(test_enum_val_4, true);

	EXPECT_TRUE(flags.all());

	flags.set_all(false);
	EXPECT_TRUE(flags.none());

	flags.set_all(true);
	EXPECT_TRUE(flags.all());
}

TEST(ARRAY, ARRAY_HAS_NON_ZERO_VALUES)
{
	c_static_array<int32, 256> array;
	array.zero_data();
	//memory_zero(array.data(), sizeof(int32) * array.capacity());

	EXPECT_FALSE(array_has_non_zero_data(array));

	array[200] = -253;
	EXPECT_TRUE(array_has_non_zero_data(array));
}


TEST(FLAG, FLAG_VALUES)
{
	enum e_test_enum
	{
		test_enum_value_0,
		test_enum_value_1,
		test_enum_value_2,
		test_enum_value_3,
		test_enum_value_4,

		k_test_enum_value_count
	};

	using t_test_flags = c_flags<k_test_enum_value_count>;

	t_test_flags test;
	test.clear();

	test.set(test_enum_value_3, true);
	EXPECT_TRUE(test.any());
	EXPECT_FALSE(test.test(test_enum_value_0));
	EXPECT_FALSE(test.test(test_enum_value_1));
	EXPECT_FALSE(test.test(test_enum_value_2));
	EXPECT_TRUE(test.test(test_enum_value_3));
	EXPECT_FALSE(test.test(test_enum_value_4));
}

TEST(FLAG, INITIAL_VALUES)
{
	enum e_test_enum
	{
		test_enum_value_0,
		test_enum_value_1,
		test_enum_value_2,
		test_enum_value_3,
		test_enum_value_4,

		k_test_enum_value_count
	};

	using t_test_flags = c_flags<k_test_enum_value_count>;

	t_test_flags test(test_enum_value_1,  test_enum_value_3);
	EXPECT_TRUE(test.any());

	EXPECT_FALSE(test.test(test_enum_value_0));
	EXPECT_TRUE(test.test(test_enum_value_1));
	EXPECT_FALSE(test.test(test_enum_value_2));
	EXPECT_TRUE(test.test(test_enum_value_3));
	EXPECT_FALSE(test.test(test_enum_value_4));
}