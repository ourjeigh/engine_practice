#include "pch.h"
#include "memory.h"
#include <types/types.h>

TEST(MEMORY, CLEAR_STRUCT)
{
	struct s_test_struct
	{
		int32 a;
		real32 b;
	};

	s_test_struct test_struct;
	test_struct.a = 42;
	test_struct.b = 3.14f;
	
	clear_struct(test_struct);
	
	EXPECT_EQ(test_struct.a, 0);
	EXPECT_EQ(test_struct.b, 0.0f);
}