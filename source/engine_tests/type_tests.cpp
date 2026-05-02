#include "pch.h"
#include "types/types.h"

TEST(TYPES, INVALID_INT32)
{
	const int32 test = k_invalid;
	EXPECT_TRUE(test == k_invalid);
}

TEST(TYPES, INVALID_UINT32)
{
	const uint32 test = k_invalid;
	EXPECT_TRUE(test == k_invalid);
}

// move
#include "rendering/render_system.h"
TEST(COLOR, FROM_UINT32)
{
	s_color known(0.4f, 1.0f, 0.1f, 0.35f);

	uint32 uint32_color = known.to_uint32();
	s_color color = s_color::from_uint32(uint32_color);

	const real32 tolerance = 0.002f;
	EXPECT_NEAR(color.alpha, known.alpha, tolerance);
	EXPECT_NEAR(color.red, known.red, tolerance);
	EXPECT_NEAR(color.green, known.green, tolerance);
	EXPECT_NEAR(color.blue, known.blue, tolerance);
}