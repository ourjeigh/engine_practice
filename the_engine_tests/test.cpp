#include "pch.h"
#include "types/types.h"
#include "structures/array.h"

TEST(C_ARRAY, C_ARRAY_SIZE) 
{
	const int32 size = 10;
	c_array<real32, size> array;
	EXPECT_EQ(size, array.size());
}

TEST(C_ARRAY, C_ARRAY_ASSERTS)
{
	const int32 size = 10;
	c_array<real32, size> array;

	EXPECT_DEATH({ array[12] = 5.0f; }, "Assertion failed: index < k_max_size");
}

TEST(C_STACK, C_STACK_PUSH_POP_TOP)
{
	const int32 size = 3;
	c_stack<real32, size> stack;

	EXPECT_TRUE(stack.empty());

	stack.push(7.0f);
	EXPECT_FALSE(stack.empty());
	EXPECT_EQ(stack.top(), 7.0f);

	stack.push(1.0f);
	stack.push(3.0f);

	EXPECT_TRUE(stack.full());

	stack.pop();
	stack.pop();
	stack.pop();
	EXPECT_TRUE(stack.empty());
}

TEST(C_STACK, C_STACK_ASSERTS)
{
	const int32 size = 2;
	c_stack<real32, size> stack;

	EXPECT_DEATH(stack.top(), "Assertion failed: !empty");
	EXPECT_DEATH(stack.pop(), "Assertion failed: !empty");

	stack.push(0);
	stack.push(0);

	EXPECT_DEATH(stack.push(0), "Assertion failed: !full");
}