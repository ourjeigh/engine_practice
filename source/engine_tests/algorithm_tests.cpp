#include "pch.h"

#include "algorithms/algorithms.h"
#include <types/types.h>
#include <asserts.h>

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