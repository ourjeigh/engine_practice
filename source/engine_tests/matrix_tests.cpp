#include "pch.h"
#include "types/types.h"
#include "structures/matrix.h"

TEST(MATRIX, 3x3_MULTIPLY_WITH_IDENTITY)
{
	s_matrix_3x3_real32 matrix
	{{
		{1, 2, 3},
		{4, 5, 6},
		{7, 8, 9}
	}};

	s_matrix_3x3_real32 result = matrix * k_matrix_3x3_identity;

	for (int32 row = 0; row < 3; row++)
	{
		for (int32 col = 0; col < 3; col++)
		{
			EXPECT_FLOAT_EQ(result[row][col], matrix[row][col]);
		}
	}
}

TEST(MATRIX, 3x3_TRANSLATION_AND_SCALE)
{
	s_matrix_3x3_real32 translation
	{{
		{1, 0, 10},
		{0, 1, 20},
		{0, 0,  1}
	}};

	s_matrix_3x3_real32 scale
	{{
		{2, 0, 0},
		{0, 3, 0},
		{0, 0, 1}
	}};

	s_matrix_3x3_real32 result = translation * scale;

	EXPECT_FLOAT_EQ(result.data[0][0], 2.0f);
	EXPECT_FLOAT_EQ(result.data[0][2], 10.0f);
	EXPECT_FLOAT_EQ(result.data[1][1], 3.0f);
	EXPECT_FLOAT_EQ(result.data[1][2], 20.0f);
	EXPECT_FLOAT_EQ(result.data[2][2], 1.0f);

	// verify non-diagonal zeros are intact
	EXPECT_FLOAT_EQ(result.data[0][1], 0.0f);
	EXPECT_FLOAT_EQ(result.data[1][0], 0.0f);
	EXPECT_FLOAT_EQ(result.data[2][0], 0.0f);
	EXPECT_FLOAT_EQ(result.data[2][1], 0.0f);
}

TEST(MATRIX, 3x3_MULTIPLY_NOT_COMUTATIVE)
{
	s_matrix_3x3_real32 a
	{{
		{1, 2, 0},
		{0, 1, 0},
		{0, 0, 1}
	}};

	s_matrix_3x3_real32 b
	{{
		{1, 0, 0},
		{3, 1, 0},
		{0, 0, 1}
	}};

	s_matrix_3x3_real32 ab = a * b;
	s_matrix_3x3_real32 ba = b * a;

	// a*b and b*a should differ
	// a*b: row 0 col 0 = 1*1 + 2*3 = 7
	// b*a: row 0 col 0 = 1*1 + 0*3 = 1
	EXPECT_FLOAT_EQ(ab.data[0][0], 7.0f);
	EXPECT_FLOAT_EQ(ba.data[0][0], 1.0f);
	EXPECT_NE(ab.data[0][0], ba.data[0][0]);
}

// 4x4
TEST(MATRIX, 4x4_MULTIPLY_WITH_IDENTITY)
{
	s_matrix_4x4_real32 matrix
	{{
		{ 1,  2,  3,  4},
		{ 5,  6,  7,  8},
		{ 9, 10, 11, 12},
		{13, 14, 15, 16}
	}};

	s_matrix_4x4_real32 result = matrix * k_matrix_4x4_identity;

	for (int32 row = 0; row < 4; row++)
	{
		for (int32 col = 0; col < 4; col++)
		{
			EXPECT_FLOAT_EQ(result[row][col], matrix[row][col]);
		}
	}
}

TEST(MATRIX, 4x4_TRANSLATION_AND_SCALE)
{
	s_matrix_4x4_real32 translation
	{{
		{1, 0, 0, 10},
		{0, 1, 0, 20},
		{0, 0, 1, 30},
		{0, 0, 0,  1}
	}};

	s_matrix_4x4_real32 scale
	{{
		{2, 0, 0, 0},
		{0, 3, 0, 0},
		{0, 0, 4, 0},
		{0, 0, 0, 1}
	}};

	s_matrix_4x4_real32 result = translation * scale;

	EXPECT_FLOAT_EQ(result[0][0], 2.0f);
	EXPECT_FLOAT_EQ(result[0][3], 10.0f);
	EXPECT_FLOAT_EQ(result[1][1], 3.0f);
	EXPECT_FLOAT_EQ(result[1][3], 20.0f);
	EXPECT_FLOAT_EQ(result[2][2], 4.0f);
	EXPECT_FLOAT_EQ(result[2][3], 30.0f);
	EXPECT_FLOAT_EQ(result[3][3], 1.0f);

	// verify non-diagonal zeros are intact
	EXPECT_FLOAT_EQ(result[0][1], 0.0f);
	EXPECT_FLOAT_EQ(result[1][0], 0.0f);
	EXPECT_FLOAT_EQ(result[3][0], 0.0f);
	EXPECT_FLOAT_EQ(result[3][1], 0.0f);
	EXPECT_FLOAT_EQ(result[3][2], 0.0f);
}

TEST(MATRIX, 4x4_MULTIPLY_NOT_COMUTATIVE)
{
	s_matrix_4x4_real32 a
	{{
		{1, 2, 0, 0},
		{0, 1, 0, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 1}
	}};

	s_matrix_4x4_real32 b
	{{
		{1, 0, 0, 0},
		{3, 1, 0, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 1}
	}};

	s_matrix_4x4_real32 ab = a * b;
	s_matrix_4x4_real32 ba = b * a;

	// a*b and b*a should differ
	EXPECT_FLOAT_EQ(ab[0][0], 7.0f);   // 1*1 + 2*3 = 7
	EXPECT_FLOAT_EQ(ba[0][0], 1.0f);   // 1*1 + 0*3 = 1
	EXPECT_NE(ab[0][0], ba[0][0]);
}