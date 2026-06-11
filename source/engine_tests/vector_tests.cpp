#include "pch.h"
#include "types/types.h"
#include "structures/vector.h"

TEST(VECTOR, VECTOR_2D_SET)
{
	const int32 x = 2;
	const int32 y = 7;

	t_vector_2d_int32 vector;
	vector.set(x, y);

	EXPECT_EQ(vector.x(), x);
	EXPECT_EQ(vector.y(), y);
}

TEST(VECTOR, ADD)
{
	t_vector_2d_int32 vector_1(2, 4);
	t_vector_2d_int32 vector_2(5, 6);

	vector_1.add(vector_2);

	EXPECT_EQ(vector_1.x(), 7);
	EXPECT_EQ(vector_1.y(), 10);

	t_vector_2d_int32 v3 = vector_1 + vector_2;
}

TEST(VECTOR, SUBTRACT)
{
	t_vector_2d_int32 vector_1(8, 4);
	t_vector_2d_int32 vector_2(5, 2);

	vector_1.subtract(vector_2);

	EXPECT_EQ(vector_1.x(), 3);
	EXPECT_EQ(vector_1.y(), 2);
}

TEST(VECTOR, MULTIPLY_SCALAR)
{
	t_vector_2d_int32 vector(3, 4);
	vector.multiply_scalar(2);

	EXPECT_EQ(vector.x(), 6);
	EXPECT_EQ(vector.y(), 8);
}

TEST(VECTOR, DIVIDE_SCALAR)
{
	t_vector_2d_int32 vector(8, 6);
	vector.divide_scalar(2);

	EXPECT_EQ(vector.x(), 4);
	EXPECT_EQ(vector.y(), 3);
}

TEST(VECTOR, FLIP)
{
	t_vector_2d_int32 vector(8, 6);
	vector.flip();

	EXPECT_EQ(vector.x(), -8);
	EXPECT_EQ(vector.y(), -6);
}