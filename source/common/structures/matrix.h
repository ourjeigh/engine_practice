#ifndef __MATRIX_H__
#define __MATRIX_H__
#pragma once

#include "types/types.h"

struct s_matrix_3x3_real32
{
	static_member_data const int32 k_row_count = 3;
	static_member_data const int32 k_col_count = 3;

	real32 data[k_row_count][k_col_count];

	real32* operator[](int32 row)
	{
		return data[row];
	}

	// AxB
	// | A00 A01 A02|   | B00 B01 B02 |   | AR0•BC0 AR0•BC1 AR0•BC2 |
	// | A10 A11 A12| x | B10 B11 B12 | = | AR1•BC0 AR1•BC1 AR1•BC2 | = ...
	// | A20 A21 A22|   | B20 B21 B22 |   | AR2•BC0 AR2•BC1 AR2•BC2 |
	// 
	// | (A00*B00 + A01*B10 + A02*B20) (A00*B01 + A01*B11 + A02*B21) (A00*B02 + A01*B12 + A02*B22) |
	// | (A10*B00 + A11*B10 + A12*B20) (A10*B01 + A11*B11 + A12*B21) (A10*B02 + A11*B12 + A12*B22) |
	// | (A20*B00 + A21*B10 + A22*B20) (A20*B01 + A21*B11 + A22*B21) (A20*B02 + A21*B12 + A22*B22) |
	s_matrix_3x3_real32 operator*(const s_matrix_3x3_real32& other) const
	{
		const real32(&a)[k_row_count][k_col_count] = data;
		const real32(&b)[k_row_count][k_col_count] = other.data;

		s_matrix_3x3_real32 out
		{
			{
				{(a[0][0] * b[0][0] + a[0][1] * b[1][0] + a[0][2] * b[2][0]), (a[0][0] * b[0][1] + a[0][1] * b[1][1] + a[0][2] * b[2][1]), (a[0][0] * b[0][2] + a[0][1] * b[1][2] + a[0][2] * b[2][2])},
				{(a[1][0] * b[0][0] + a[1][1] * b[1][0] + a[1][2] * b[2][0]), (a[1][0] * b[0][1] + a[1][1] * b[1][1] + a[1][2] * b[2][1]), (a[1][0] * b[0][2] + a[1][1] * b[1][2] + a[1][2] * b[2][2])},
				{(a[2][0] * b[0][0] + a[2][1] * b[1][0] + a[2][2] * b[2][0]), (a[2][0] * b[0][1] + a[2][1] * b[1][1] + a[2][2] * b[2][1]), (a[2][0] * b[0][2] + a[2][1] * b[1][2] + a[2][2] * b[2][2])}
			}
		};

		return out;
	}

	// we ignore the w() component
	// vectors are 1xN row matrices!
	t_vector_4d_real32 operator*(const t_vector_4d_real32& vector) const
	{
		// 1x3 * 3x3
		t_vector_4d_real32 out;
		out.x() = vector.x() * data[0][0] + vector.y() * data[1][0] + vector.z() * data[2][0];
		out.y() = vector.x() * data[0][1] + vector.y() * data[1][1] + vector.z() * data[2][1];
		out.z() = vector.x() * data[0][2] + vector.y() * data[1][2] + vector.z() * data[2][2];
		return out;
	}
};

struct s_matrix_4x4_real32
{
	real32 data[4][4];
};

const s_matrix_3x3_real32 k_matrix_3x3_identity =
{
	{
		{1, 0, 0 },
		{0, 1, 0 },
		{0, 0, 1 },
	}
};

const s_matrix_4x4_real32 k_matrix_4x4_identity =
{
	{
		{1, 0, 0, 0 },
		{0, 1, 0, 0 },
		{0, 0, 1, 0 },
		{0, 0, 0, 1 },
	}
};

#endif //__MATRIX_H__