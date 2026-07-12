#include "pch.h"

#include "algorithms/algorithms.h"
#include "types/types.h"
#include "debug/asserts.h"

TEST(ALGORITHMS, FNV1_32)
{
	const char string1[] = "Hello World!";
	const char string2[] = "wello horld?";

	// two different strings are not the same hash
	EXPECT_NE(fnv1_hash_32(string1, sizeof(string1)), fnv1_hash_32(string2, sizeof(string2)));

	const uint32 expected = fnv1_hash_32(string1, sizeof(string1));

	// the same strings are always the same hash
	for (int32 i = 0; i < 50; i++)
	{
		const uint32 actual = fnv1_hash_32(string1, sizeof(string1));
		EXPECT_EQ(expected, actual);
	}
}

TEST(ALGORITHMS, FNV1a_32)
{
	const char string1[] = "Hello World!";
	const char string2[] = "wello horld?";

	// two different strings are not the same hash
	EXPECT_NE(fnv1a_hash_32(string1, sizeof(string1)), fnv1a_hash_32(string2, sizeof(string2)));

	const uint32 expected = fnv1a_hash_32(string1, sizeof(string1));

	// the same strings are always the same hash
	for (int32 i = 0; i < 50; i++)
	{
		const uint32 actual = fnv1a_hash_32(string1, sizeof(string1));
		EXPECT_EQ(expected, actual);
	}
}

TEST(ALGORITHMS, FNV1_64)
{
	const char string1[] = "Hello World!";
	const char string2[] = "wello horld?";

	// two different strings are not the same hash
	EXPECT_NE(fnv1_hash_64(string1, sizeof(string1)), fnv1_hash_64(string2, sizeof(string2)));

	const uint64 expected = fnv1_hash_64(string1, sizeof(string1));

	// the same strings are always the same hash
	for (int32 i = 0; i < 50; i++)
	{
		const uint64 actual = fnv1_hash_64(string1, sizeof(string1));
		EXPECT_EQ(expected, actual);
	}
}

TEST(ALGORITHMS, FNV1a_64)
{
	const char string1[] = "Hello World!";
	const char string2[] = "wello horld?";
	
	// two different strings are not the same hash
	EXPECT_NE(fnv1a_hash_64(string1, sizeof(string1)), fnv1a_hash_64(string2, sizeof(string2)));

	const uint64 expected = fnv1a_hash_64(string1, sizeof(string1));
	//COMPILE_ASSERT(fnv1a_hash_64(string1, sizeof(string1)) == 11208736881023205110);

	// the same strings are always the same hash
	for (int32 i = 0; i < 50; i++)
	{
		const uint64 actual = fnv1a_hash_64(string1, sizeof(string1));
		EXPECT_EQ(expected, actual);
	}
}

TEST(ALGORITHMS, FVN1_COMPILE_TIME)
{
	COMPILE_ASSERT(MAKE_STRING_HASH("hello world") != 0);
}

bool compare_int32(const void* l, const void* r)
{
	int32 left = *(static_cast<const int32*>(l));
	int32 right = *(static_cast<const int32*>(r));

	return left < right;
}

TEST(ALGORITHMS, QUICK_SORT)
{
	c_static_array<int32, 10> test = { 8, 0, 3, 6, 4, 9, 1, 7, 2, 5 };

	quick_sort(test.as_array(), compare_int32, 0, test.capacity());

	for (int32 i = 0; i < test.capacity(); i++)
	{
		EXPECT_EQ(test[i], i);
	}
}