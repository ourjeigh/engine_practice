#include "pch.h"
#include "structures/shapes.h"

TEST(SHAPE, AABB_3D_IS_VALID)
{
	t_aabb_3d_real32 test;
	test.min = { 0.0f, 0.0f, 0.0f, 1.0f };
	test.max = { 1.0f, 1.0f, 1.0f, 1.0f };
	EXPECT_TRUE(test.is_valid());

	// min == max
	test.min = { 0.0f, 0.0f, 0.0f, 1.0f };
	test.max = { 0.0f, 0.0f, 0.0f, 1.0f };
	EXPECT_FALSE(test.is_valid());

	// negative depth
	test.min = { 0.0f, 0.0f, 0.0f, 1.0f };
	test.max = { 0.0f, 0.0f, -1.0f, 1.0f };
	EXPECT_FALSE(test.is_valid());

	// bad w
	test.min = { 0.0f, 0.0f, 0.0f, 1.0f };
	test.max = { 1.0f, 1.0f, 1.0f, 0.0f };
	EXPECT_FALSE(test.is_valid());
}


TEST(SHAPE, AABB_3D_IS_VALID_Z_EQUAL_ALLOWED)
{
	// z is allowed to be strictly equal (2d shapes riding the 3d type),
	t_aabb_3d_int32 test;
	test.min = { 0, 0, 5, 1 };
	test.max = { 1, 1, 5, 1 };
	EXPECT_TRUE(test.is_valid());

	// but x collapsing to equal is still invalid
	test.min = { 0, 0, 0, 1 };
	test.max = { 0, 1, 1, 1 };
	EXPECT_FALSE(test.is_valid());

	// and y collapsing to equal is still invalid
	test.min = { 0, 0, 0, 1 };
	test.max = { 1, 0, 1, 1 };
	EXPECT_FALSE(test.is_valid());
}

TEST(SHAPE, AABB_3D_WIDTH_HEIGHT_DEPTH)
{
	t_aabb_3d_real32 test;
	test.min = { -1.0f, -2.0f, -3.0f, 1.0f };
	test.max = { 4.0f, 5.0f, 6.0f, 1.0f };
	EXPECT_FLOAT_EQ(test.width(), 5.0f);
	EXPECT_FLOAT_EQ(test.height(), 7.0f);
	EXPECT_FLOAT_EQ(test.depth(), 9.0f);
}

TEST(SHAPE, AABB_CENTER)
{
	t_aabb_3d_real32 aabb
	{
		{ 0, 0, 0, 1 },
		{ 2, 2, 2, 1 }
	};

	t_vector_4d_real32 expected_center = { 1,1,1,1 };

	EXPECT_EQ(expected_center, aabb.center());
}

TEST(SHAPE, AABB_3D_CONTAINS_POINT)
{
	t_aabb_3d_real32 test;
	test.min = { 0.0f, 0.0f, 0.0f, 1.0f };
	test.max = { 10.0f, 10.0f, 10.0f, 1.0f };

	// interior point
	EXPECT_TRUE(test.contains_point({ 5.0f, 5.0f, 5.0f, 1.0f }));

	// on min corner (inclusive)
	EXPECT_TRUE(test.contains_point({ 0.0f, 0.0f, 0.0f, 1.0f }));

	// on max corner (inclusive)
	EXPECT_TRUE(test.contains_point({ 10.0f, 10.0f, 10.0f, 1.0f }));

	// on a face (inclusive)
	EXPECT_TRUE(test.contains_point({ 5.0f, 5.0f, 10.0f, 1.0f }));

	// outside on each axis independently
	EXPECT_FALSE(test.contains_point({ -0.01f, 5.0f, 5.0f, 1.0f }));
	EXPECT_FALSE(test.contains_point({ 5.0f, 10.01f, 5.0f, 1.0f }));
	EXPECT_FALSE(test.contains_point({ 5.0f, 5.0f, -0.01f, 1.0f }));
}

TEST(SHAPE, AABB_3D_CONTAINS_POINT_ASSERTS_ON_INVALID_AABB)
{
	t_aabb_3d_real32 test;
	test.min = { 0.0f, 0.0f, 0.0f, 1.0f };
	test.max = { 0.0f, 0.0f, 0.0f, 1.0f }; // collapsed, invalid
	EXPECT_DEATH(test.contains_point({ 0.0f, 0.0f, 0.0f, 1.0f }), "");
}

TEST(SHAPE, AABB_3D_OVERLAPS_PLANE_XY_AT_Z)
{
	t_aabb_3d_real32 test;
	test.min = { 0.0f, 0.0f, 0.0f, 1.0f };
	test.max = { 10.0f, 10.0f, 10.0f, 1.0f };

	EXPECT_TRUE(test.overlaps_plane_xy_at_z(5.0f));
	EXPECT_TRUE(test.overlaps_plane_xy_at_z(0.0f));  // inclusive at min
	EXPECT_TRUE(test.overlaps_plane_xy_at_z(10.0f)); // inclusive at max
	EXPECT_FALSE(test.overlaps_plane_xy_at_z(-0.01f));
	EXPECT_FALSE(test.overlaps_plane_xy_at_z(10.01f));
}

TEST(SHAPE, AABB_3D_OVERLAPS_PLANE_YZ_AT_X)
{
	t_aabb_3d_real32 test;
	test.min = { 0.0f, 0.0f, 0.0f, 1.0f };
	test.max = { 10.0f, 10.0f, 10.0f, 1.0f };

	EXPECT_TRUE(test.overlaps_plane_yz_at_x(5.0f));
	EXPECT_TRUE(test.overlaps_plane_yz_at_x(0.0f));
	EXPECT_TRUE(test.overlaps_plane_yz_at_x(10.0f));
	EXPECT_FALSE(test.overlaps_plane_yz_at_x(-0.01f));
	EXPECT_FALSE(test.overlaps_plane_yz_at_x(10.01f));
}

TEST(SHAPE, AABB_3D_OVERLAPS_PLANE_XZ_AT_Y)
{
	t_aabb_3d_real32 test;
	test.min = { 0.0f, 0.0f, 0.0f, 1.0f };
	test.max = { 10.0f, 10.0f, 10.0f, 1.0f };

	EXPECT_TRUE(test.overlaps_plane_xz_at_y(5.0f));
	EXPECT_TRUE(test.overlaps_plane_xz_at_y(0.0f));
	EXPECT_TRUE(test.overlaps_plane_xz_at_y(10.0f));
	EXPECT_FALSE(test.overlaps_plane_xz_at_y(-0.01f));
	EXPECT_FALSE(test.overlaps_plane_xz_at_y(10.01f));
}

TEST(SHAPE, AABB_3D_OVERLAPS_PLANE_ASSERTS_ON_INVALID_AABB)
{
	t_aabb_3d_real32 test;
	test.min = { 0.0f, 0.0f, 0.0f, 1.0f };
	test.max = { 0.0f, 0.0f, 0.0f, 1.0f };
	EXPECT_DEATH(test.overlaps_plane_xy_at_z(0.0f), "");
}

TEST(SHAPE, AABB_3D_OVERLAPS_OTHER)
{
	t_aabb_3d_real32 a;
	a.min = { 0.0f, 0.0f, 0.0f, 1.0f };
	a.max = { 10.0f, 10.0f, 10.0f, 1.0f };

	// full overlap (self)
	EXPECT_TRUE(a.overlaps_other(a));

	// partial overlap, straddling one corner
	t_aabb_3d_real32 b;
	b.min = { 5.0f, 5.0f, 5.0f, 1.0f };
	b.max = { 15.0f, 15.0f, 15.0f, 1.0f };
	EXPECT_TRUE(a.overlaps_other(b));
	EXPECT_TRUE(b.overlaps_other(a));

	// touching exactly at a face (inclusive)
	t_aabb_3d_real32 c;
	c.min = { 10.0f, 0.0f, 0.0f, 1.0f };
	c.max = { 20.0f, 10.0f, 10.0f, 1.0f };
	EXPECT_TRUE(a.overlaps_other(c));

	// fully separate on one axis only is enough to not overlap
	t_aabb_3d_real32 d;
	d.min = { 10.01f, 0.0f, 0.0f, 1.0f };
	d.max = { 20.0f, 10.0f, 10.0f, 1.0f };
	EXPECT_FALSE(a.overlaps_other(d));

	// b fully contains a
	t_aabb_3d_real32 e;
	e.min = { -5.0f, -5.0f, -5.0f, 1.0f };
	e.max = { 15.0f, 15.0f, 15.0f, 1.0f };
	EXPECT_TRUE(a.overlaps_other(e));
}

TEST(SHAPE, AABB_3D_OVERLAPS_OTHER_ASSERTS_ON_INVALID_AABB)
{
	t_aabb_3d_real32 valid;
	valid.min = { 0.0f, 0.0f, 0.0f, 1.0f };
	valid.max = { 10.0f, 10.0f, 10.0f, 1.0f };

	t_aabb_3d_real32 invalid;
	invalid.min = { 0.0f, 0.0f, 0.0f, 1.0f };
	invalid.max = { 0.0f, 0.0f, 0.0f, 1.0f };

	// asserts whether the invalid one is *this or the argument
	EXPECT_DEATH(invalid.overlaps_other(valid), "");
	EXPECT_DEATH(valid.overlaps_other(invalid), "");
}

TEST(SHAPE, AABB_3D_GROW_BY_OTHER)
{
	t_aabb_3d_real32 test;
	test.min = { 0.0f, 0.0f, 0.0f, 1.0f };
	test.max = { 10.0f, 10.0f, 10.0f, 1.0f };

	// other's extent (4 x 6 x 2) determines the pad; other's position is a no-op
	t_aabb_3d_real32 other;
	other.min = { 100.0f, 100.0f, 100.0f, 1.0f }; // deliberately far away
	other.max = { 104.0f, 106.0f, 102.0f, 1.0f };

	t_aabb_3d_real32 result = test.grow_by_other(other);
	EXPECT_FLOAT_EQ(result.min.x(), -2.0f); // 0 - 4*0.5
	EXPECT_FLOAT_EQ(result.min.y(), -3.0f); // 0 - 6*0.5
	EXPECT_FLOAT_EQ(result.min.z(), -1.0f); // 0 - 2*0.5
	EXPECT_FLOAT_EQ(result.max.x(), 12.0f); // 10 + 4*0.5
	EXPECT_FLOAT_EQ(result.max.y(), 13.0f); // 10 + 6*0.5
	EXPECT_FLOAT_EQ(result.max.z(), 11.0f); // 10 + 2*0.5
	EXPECT_TRUE(result.is_valid());
}

TEST(SHAPE, AABB_3D_GROW_BY_OTHER_IGNORES_OTHER_POSITION)
{
	t_aabb_3d_real32 test;
	test.min = { 0.0f, 0.0f, 0.0f, 1.0f };
	test.max = { 10.0f, 10.0f, 10.0f, 1.0f };

	// same extent (2 x 2 x 2), two wildly different positions -> identical result
	t_aabb_3d_real32 other_near;
	other_near.min = { 0.0f, 0.0f, 0.0f, 1.0f };
	other_near.max = { 2.0f, 2.0f, 2.0f, 1.0f };

	t_aabb_3d_real32 other_far;
	other_far.min = { -500.0f, 500.0f, -500.0f, 1.0f };
	other_far.max = { -498.0f, 502.0f, -498.0f, 1.0f };

	t_aabb_3d_real32 result_near = test.grow_by_other(other_near);
	t_aabb_3d_real32 result_far = test.grow_by_other(other_far);

	EXPECT_FLOAT_EQ(result_near.min.x(), result_far.min.x());
	EXPECT_FLOAT_EQ(result_near.min.y(), result_far.min.y());
	EXPECT_FLOAT_EQ(result_near.min.z(), result_far.min.z());
	EXPECT_FLOAT_EQ(result_near.max.x(), result_far.max.x());
	EXPECT_FLOAT_EQ(result_near.max.y(), result_far.max.y());
	EXPECT_FLOAT_EQ(result_near.max.z(), result_far.max.z());
}

TEST(SHAPE, AABB_3D_GROW_BY_OTHER_ASSERTS_ON_INVALID_RESULT)
{
	// starting box has zero width; growing by a zero-extent other doesn't
	// rescue it, so the result is still invalid and should assert.
	t_aabb_3d_real32 test;
	test.min = { 0.0f, 0.0f, 0.0f, 1.0f };
	test.max = { 0.0f, 10.0f, 10.0f, 1.0f };

	t_aabb_3d_real32 zero_extent_other;
	zero_extent_other.min = { 0.0f, 0.0f, 0.0f, 1.0f };
	zero_extent_other.max = { 0.0f, 0.0f, 0.0f, 1.0f };

	EXPECT_DEATH(test.grow_by_other(zero_extent_other), "");
}

TEST(SHAPE, AABB_3D_MINKOWSKI_SUM)
{
	t_aabb_3d_real32 a;
	a.min = { 0.0f, 0.0f, 0.0f, 1.0f };
	a.max = { 2.0f, 3.0f, 4.0f, 1.0f };

	t_aabb_3d_real32 b;
	b.min = { 10.0f, 10.0f, 10.0f, 1.0f };
	b.max = { 15.0f, 16.0f, 17.0f, 1.0f };

	t_aabb_3d_real32 result = a.minkowski_sum(b);
	EXPECT_FLOAT_EQ(result.min.x(), 10.0f); // 0 + 10
	EXPECT_FLOAT_EQ(result.min.y(), 10.0f); // 0 + 10
	EXPECT_FLOAT_EQ(result.min.z(), 10.0f); // 0 + 10
	EXPECT_FLOAT_EQ(result.max.x(), 17.0f); // 2 + 15
	EXPECT_FLOAT_EQ(result.max.y(), 19.0f); // 3 + 16
	EXPECT_FLOAT_EQ(result.max.z(), 21.0f); // 4 + 17
	EXPECT_FLOAT_EQ(result.min.w(), 1.0f);
	EXPECT_FLOAT_EQ(result.max.w(), 1.0f);
	EXPECT_TRUE(result.is_valid());
}

TEST(SHAPE, AABB_3D_MINKOWSKI_DIFFERENCE)
{
	t_aabb_3d_real32 a;
	a.min = { 0.0f, 0.0f, 0.0f, 1.0f };
	a.max = { 10.0f, 10.0f, 10.0f, 1.0f };

	t_aabb_3d_real32 b;
	b.min = { 2.0f, 2.0f, 2.0f, 1.0f };
	b.max = { 4.0f, 4.0f, 4.0f, 1.0f };

	// min = a.min - b.max, max = a.max - b.min
	t_aabb_3d_real32 result = a.minkowski_difference(b);
	EXPECT_FLOAT_EQ(result.min.x(), -4.0f); // 0 - 4
	EXPECT_FLOAT_EQ(result.min.y(), -4.0f);
	EXPECT_FLOAT_EQ(result.min.z(), -4.0f);
	EXPECT_FLOAT_EQ(result.max.x(), 8.0f);  // 10 - 2
	EXPECT_FLOAT_EQ(result.max.y(), 8.0f);
	EXPECT_FLOAT_EQ(result.max.z(), 8.0f);
	EXPECT_FLOAT_EQ(result.min.w(), 1.0f);
	EXPECT_FLOAT_EQ(result.max.w(), 1.0f);
	EXPECT_TRUE(result.is_valid());
}

TEST(SHAPE, AABB_3D_MINKOWSKI_DIFFERENCE_ORIGIN_CONTAINMENT_MATCHES_OVERLAP)
{
	// the property this operation exists for: a and b overlap iff the
	// origin lies inside their minkowski difference.
	t_aabb_3d_real32 a;
	a.min = { 0.0f, 0.0f, 0.0f, 1.0f };
	a.max = { 10.0f, 10.0f, 10.0f, 1.0f };

	t_aabb_3d_real32 overlapping;
	overlapping.min = { 5.0f, 5.0f, 5.0f, 1.0f };
	overlapping.max = { 15.0f, 15.0f, 15.0f, 1.0f };

	t_aabb_3d_real32 separate;
	separate.min = { 20.0f, 20.0f, 20.0f, 1.0f };
	separate.max = { 30.0f, 30.0f, 30.0f, 1.0f };

	t_vector_4d_real32 origin = { 0.0f, 0.0f, 0.0f, 1.0f };

	t_aabb_3d_real32 diff_overlapping = a.minkowski_difference(overlapping);
	EXPECT_TRUE(a.overlaps_other(overlapping));
	EXPECT_TRUE(diff_overlapping.contains_point(origin));

	t_aabb_3d_real32 diff_separate = a.minkowski_difference(separate);
	EXPECT_FALSE(a.overlaps_other(separate));
	EXPECT_FALSE(diff_separate.contains_point(origin));
}

TEST(SHAPE, RECT_3D_SET)
{
	t_rect_3d_real32 test;
	test.set(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f);
	EXPECT_FLOAT_EQ(test.x, 1.0f);
	EXPECT_FLOAT_EQ(test.y, 2.0f);
	EXPECT_FLOAT_EQ(test.z, 3.0f);
	EXPECT_FLOAT_EQ(test.width, 4.0f);
	EXPECT_FLOAT_EQ(test.height, 5.0f);
	EXPECT_FLOAT_EQ(test.depth, 6.0f);
}

TEST(SHAPE, RECT_3D_XYZ)
{
	t_rect_3d_real32 test;
	test.set(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f);
	t_vector_4d_real32 xyz = test.xyz();
	EXPECT_FLOAT_EQ(xyz.x(), 1.0f);
	EXPECT_FLOAT_EQ(xyz.y(), 2.0f);
	EXPECT_FLOAT_EQ(xyz.z(), 3.0f);
	EXPECT_FLOAT_EQ(xyz.w(), 1.0f); // point, not vector
}

TEST(SHAPE, RECT_3D_WHD)
{
	t_rect_3d_real32 test;
	test.set(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f);
	t_vector_4d_real32 whd = test.whd();
	EXPECT_FLOAT_EQ(whd.x(), 4.0f);
	EXPECT_FLOAT_EQ(whd.y(), 5.0f);
	EXPECT_FLOAT_EQ(whd.z(), 6.0f);
	EXPECT_FLOAT_EQ(whd.w(), 0.0f); // vector (extent), not a point
}

TEST(SHAPE, RECT_3D_CENTER)
{
	t_rect_3d_real32 test;
	test.set(0.0f, 0.0f, 0.0f, 4.0f, 6.0f, 2.0f); // deliberately non-cube
	t_vector_4d_real32 center = test.center();
	EXPECT_FLOAT_EQ(center.x(), 2.0f); // 0 + 4*0.5
	EXPECT_FLOAT_EQ(center.y(), 3.0f); // 0 + 6*0.5
	EXPECT_FLOAT_EQ(center.z(), 1.0f); // 0 + 2*0.5 (regression check: was using height here)
	EXPECT_FLOAT_EQ(center.w(), 1.0f);
}

TEST(SHAPE, RECT_3D_CENTER_WITH_NONZERO_ORIGIN)
{
	t_rect_3d_real32 test;
	test.set(10.0f, -5.0f, 3.0f, 4.0f, 6.0f, 2.0f);
	t_vector_4d_real32 center = test.center();
	EXPECT_FLOAT_EQ(center.x(), 12.0f); // 10 + 4*0.5
	EXPECT_FLOAT_EQ(center.y(), -2.0f); // -5 + 6*0.5
	EXPECT_FLOAT_EQ(center.z(), 4.0f);  // 3 + 2*0.5
}

TEST(SHAPE, RECT_3D_TO_AABB)
{
	t_rect_3d_real32 test;
	test.set(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f);
	t_aabb_3d_real32 result = test.to_aabb();
	EXPECT_FLOAT_EQ(result.min.x(), 1.0f);
	EXPECT_FLOAT_EQ(result.min.y(), 2.0f);
	EXPECT_FLOAT_EQ(result.min.z(), 3.0f);
	EXPECT_FLOAT_EQ(result.max.x(), 5.0f);  // 1 + 4
	EXPECT_FLOAT_EQ(result.max.y(), 7.0f);  // 2 + 5
	EXPECT_FLOAT_EQ(result.max.z(), 9.0f);  // 3 + 6
	EXPECT_FLOAT_EQ(result.min.w(), 1.0f);
	EXPECT_FLOAT_EQ(result.max.w(), 1.0f);
	EXPECT_TRUE(result.is_valid());
}

TEST(SHAPE, RECT_3D_TO_AABB_ZERO_DEPTH_IS_VALID)
{
	// 2d-in-3d case: zero depth should still produce a valid aabb (z: >=).
	t_rect_3d_real32 test;
	test.set(0.0f, 0.0f, 5.0f, 4.0f, 5.0f, 0.0f);
	t_aabb_3d_real32 result = test.to_aabb();
	EXPECT_FLOAT_EQ(result.min.z(), 5.0f);
	EXPECT_FLOAT_EQ(result.max.z(), 5.0f);
	EXPECT_TRUE(result.is_valid());
}

TEST(SHAPE, RECT_3D_TO_AABB_ASSERTS_ON_ZERO_WIDTH_OR_HEIGHT)
{
	// unlike depth, width/height collapsing to zero is never valid (x/y use >).
	t_rect_3d_real32 zero_width;
	zero_width.set(0.0f, 0.0f, 0.0f, 0.0f, 5.0f, 5.0f);
	EXPECT_DEATH(zero_width.to_aabb(), "");

	t_rect_3d_real32 zero_height;
	zero_height.set(0.0f, 0.0f, 0.0f, 5.0f, 0.0f, 5.0f);
	EXPECT_DEATH(zero_height.to_aabb(), "");
}

TEST(SHAPE, RECT_3D_TO_AABB_ASSERTS_ON_NEGATIVE_EXTENT)
{
	// negative width/height produces an inverted (max < min) aabb on x/y,
	// where is_valid uses a strict >.
	t_rect_3d_real32 test;
	test.set(0.0f, 0.0f, 0.0f, -4.0f, 5.0f, 5.0f);
	EXPECT_DEATH(test.to_aabb(), "");
}

TEST(SHAPE, RECT_3D_TO_AABB_ASSERTS_ON_NEGATIVE_DEPTH)
{
	// depth uses is_valid's non-strict z >= check, so zero depth is valid
	// (see RECT_3D_TO_AABB_ZERO_DEPTH_IS_VALID) but negative depth still
	// inverts max.z() < min.z() and must assert.
	t_rect_3d_real32 test;
	test.set(0.0f, 0.0f, 0.0f, 4.0f, 5.0f, -1.0f);
	EXPECT_DEATH(test.to_aabb(), "");
}


TEST(SHAPE, SPHERE_FROM_AABB)
{
	t_aabb_3d_real32 aabb
	{
		{0,0,0,1},
		{2,2,2,1}
	};

	t_sphere_3d_real32 sphere = t_sphere_3d_real32::from_aabb(aabb);

	EXPECT_EQ(sphere.center, t_vector_4d_real32(1,1,1,1));
	EXPECT_EQ(sphere.radius, 1);
}