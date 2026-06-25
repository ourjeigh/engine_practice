#ifndef __MATRIX_H__
#define __MATRIX_H__
#pragma once

#include "types/types.h"
#include "structures/vector.h"

struct s_matrix_3x3_real32
{
	static_member_data const int32 k_row_count = 3;
	static_member_data const int32 k_col_count = 3;

	real32 data[k_row_count][k_col_count];

	real32* operator[](int32 row)
	{
		ASSERT(in_range_inclusive_int32(0, k_row_count - 1, row));
		return data[row];
	}
	
	const real32* operator[](int32 row) const
	{
		ASSERT(in_range_inclusive_int32(0, k_row_count - 1, row));
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
		{{
			{(a[0][0]*b[0][0] + a[0][1]*b[1][0] + a[0][2]*b[2][0]), (a[0][0]*b[0][1] + a[0][1]*b[1][1] + a[0][2]*b[2][1]), (a[0][0]*b[0][2] + a[0][1]*b[1][2] + a[0][2]*b[2][2])},
			{(a[1][0]*b[0][0] + a[1][1]*b[1][0] + a[1][2]*b[2][0]), (a[1][0]*b[0][1] + a[1][1]*b[1][1] + a[1][2]*b[2][1]), (a[1][0]*b[0][2] + a[1][1]*b[1][2] + a[1][2]*b[2][2])},
			{(a[2][0]*b[0][0] + a[2][1]*b[1][0] + a[2][2]*b[2][0]), (a[2][0]*b[0][1] + a[2][1]*b[1][1] + a[2][2]*b[2][1]), (a[2][0]*b[0][2] + a[2][1]*b[1][2] + a[2][2]*b[2][2])}
		}};

		return out;
	}

	// vectors are 1xN row matrices, you must perform multiplication as v*M, not M*v
	void operator*(const t_vector_4d_real32& vector) = delete;
};

// we ignore the w() component
// vectors are 1xN row matrices!
t_vector_4d_real32 operator*(const t_vector_4d_real32& vector, const s_matrix_3x3_real32& matrix)
{
	// 1x3*3x3
	t_vector_4d_real32 out;
	out.x() = vector.x()*matrix[0][0] + vector.y()*matrix[1][0] + vector.z()*matrix[2][0];
	out.y() = vector.x()*matrix[0][1] + vector.y()*matrix[1][1] + vector.z()*matrix[2][1];
	out.z() = vector.x()*matrix[0][2] + vector.y()*matrix[1][2] + vector.z()*matrix[2][2];
	return out;
}


struct s_matrix_4x4_real32
{
	static_member_data const int32 k_row_count = 4;
	static_member_data const int32 k_col_count = 4;

	real32 data[k_row_count][k_col_count];

	real32* operator[](int32 row)
	{
		ASSERT(in_range_inclusive_int32(0, k_row_count - 1, row));
		return data[row];
	}

	const real32* operator[](int32 row) const
	{
		ASSERT(in_range_inclusive_int32(0, k_row_count - 1, row));
		return data[row];
	}

	// AxB
	// | A00 A01 A02 A03|   | B00 B01 B02 B03|   | AR0•BC0 AR0•BC1 AR0•BC2 AR0•BC3 |
	// | A10 A11 A12 A13| x | B10 B11 B12 B13| = | AR1•BC0 AR1•BC1 AR1•BC2 AR1•BC3 | = ...
	// | A20 A21 A22 A23|   | B20 B21 B22 B23|   | AR2•BC0 AR2•BC1 AR2•BC2 AR2•BC3 |
	// | A30 A31 A32 A33|   | B30 B31 B32 B33|   | AR3•BC0 AR3•BC1 AR3•BC3 AR3•BC3 |
	// 
	// | (A00*B00 + A01*B10 + A02*B20 + A03*B30) (A00*B01 + A01*B11 + A02*B21 + A03*B31) (A00*B02 + A01*B12 + A02*B22 + A03*B32) (A00*B03 + A01*B13 + A02*B23 + A03*B33) |
	// | (A10*B00 + A11*B10 + A12*B20 + A13*B30) (A10*B01 + A11*B11 + A12*B21 + A13*B31) (A10*B02 + A11*B12 + A12*B22 + A13*B32) (A10*B03 + A11*B13 + A12*B23 + A13*B33) |
	// | (A20*B00 + A21*B10 + A22*B20 + A23*B30) (A20*B01 + A21*B11 + A22*B21 + A23*B31) (A20*B02 + A21*B12 + A22*B22 + A23*B32) (A20*B03 + A21*B13 + A22*B23 + A23*B33) |
	// | (A30*B00 + A31*B10 + A32*B20 + A33*B30) (A30*B01 + A31*B11 + A32*B21 + A33*B31) (A30*B02 + A31*B12 + A32*B22 + A33*B32) (A30*B03 + A31*B13 + A32*B23 + A33*B33) |
	s_matrix_4x4_real32 operator*(const s_matrix_4x4_real32& other) const
	{
		const real32(&a)[k_row_count][k_col_count] = data;
		const real32(&b)[k_row_count][k_col_count] = other.data;

		s_matrix_4x4_real32 out
		{{
			{(a[0][0]*b[0][0] + a[0][1]*b[1][0] + a[0][2]*b[2][0] + a[0][3]*b[3][0]), (a[0][0]*b[0][1] + a[0][1]*b[1][1] + a[0][2]*b[2][1] + a[0][3]*b[3][1]), (a[0][0]*b[0][2] + a[0][1]*b[1][2] + a[0][2]*b[2][2] + a[0][3]*b[3][2]), (a[0][0]*b[0][3] + a[0][1]*b[1][3] + a[0][2]*b[2][3] + a[0][3]*b[3][3])},
			{(a[1][0]*b[0][0] + a[1][1]*b[1][0] + a[1][2]*b[2][0] + a[1][3]*b[3][0]), (a[1][0]*b[0][1] + a[1][1]*b[1][1] + a[1][2]*b[2][1] + a[1][3]*b[3][1]), (a[1][0]*b[0][2] + a[1][1]*b[1][2] + a[1][2]*b[2][2] + a[1][3]*b[3][2]), (a[1][0]*b[0][3] + a[1][1]*b[1][3] + a[1][2]*b[2][3] + a[1][3]*b[3][3])},
			{(a[2][0]*b[0][0] + a[2][1]*b[1][0] + a[2][2]*b[2][0] + a[2][3]*b[3][0]), (a[2][0]*b[0][1] + a[2][1]*b[1][1] + a[2][2]*b[2][1] + a[2][3]*b[3][1]), (a[2][0]*b[0][2] + a[2][1]*b[1][2] + a[2][2]*b[2][2] + a[2][3]*b[3][2]), (a[2][0]*b[0][3] + a[2][1]*b[1][3] + a[2][2]*b[2][3] + a[2][3]*b[3][3])},
			{(a[3][0]*b[0][0] + a[3][1]*b[1][0] + a[3][2]*b[2][0] + a[3][3]*b[3][0]), (a[3][0]*b[0][1] + a[3][1]*b[1][1] + a[3][2]*b[2][1] + a[3][3]*b[3][1]), (a[3][0]*b[0][2] + a[3][1]*b[1][2] + a[3][2]*b[2][2] + a[3][3]*b[3][2]), (a[3][0]*b[0][3] + a[3][1]*b[1][3] + a[3][2]*b[2][3] + a[3][3]*b[3][3])}
		}};

		return out;
	}

	void operator*(const t_vector_4d_real32& vector) = delete;
};

t_vector_4d_real32 operator*(const t_vector_4d_real32& vector, const s_matrix_4x4_real32& matrix)
{
	// 1x4*4x4
	t_vector_4d_real32 out;
	out.x() = vector.x()*matrix[0][0] + vector.y()*matrix[1][0] + vector.z()*matrix[2][0] + vector.w()*matrix[3][0];
	out.y() = vector.x()*matrix[0][1] + vector.y()*matrix[1][1] + vector.z()*matrix[2][1] + vector.w()*matrix[3][1];
	out.z() = vector.x()*matrix[0][2] + vector.y()*matrix[1][2] + vector.z()*matrix[2][2] + vector.w()*matrix[3][2];
	out.w() = vector.x()*matrix[0][3] + vector.y()*matrix[1][3] + vector.z()*matrix[2][3] + vector.w()*matrix[3][3];
	return out;
}

const s_matrix_3x3_real32 k_matrix_3x3_identity =
{{
	{1, 0, 0 },
	{0, 1, 0 },
	{0, 0, 1 },
}};

const s_matrix_4x4_real32 k_matrix_4x4_identity =
{{
	{1, 0, 0, 0 },
	{0, 1, 0, 0 },
	{0, 0, 1, 0 },
	{0, 0, 0, 1 },
}};

#endif //__MATRIX_H__