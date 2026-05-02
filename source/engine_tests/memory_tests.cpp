#include "pch.h"
#include "memory/memory.h"
#include "types/types.h"
#include "memory/allocator.h"
#include "structures/array.h"
#include "platform/platform.h"

TEST(MEMORY, MEM_SET)
{
	const int32 size = 10;
	uint32 test_array[size];
	memory_set(test_array, k_byte_max, sizeof(uint32) * size);

	for (int32 i = 0; i < size; i++)
	{
		EXPECT_EQ(test_array[i], k_uint32_max);
	}
}

TEST(MEMORY, MEMZERO)
{
	int32 data[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

	memory_zero(data, sizeof(int32) * 10);

	for (int32 i = 0; i < 10; i++)
	{
		EXPECT_EQ(data[i], 0);
	}
}

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
	
	zero_object(test_struct);
	
	EXPECT_EQ(test_struct.a, 0);
	EXPECT_EQ(test_struct.b, 0.0f);
}

TEST(MEMORY, MEMCOPY)
{
	for (int32 i = 0; i < 1000000; i++)
	{
		const int32 length = 10;
		int64 source[length] = { 1,3,5,7,9, 11, 13, 15, 17, 19 };
		int64 dest[length] = { 0 };
		memory_copy(dest, source, sizeof(int64) * length);

		for (int32 i = 0; i < length; i++)
		{
			EXPECT_EQ(dest[i], source[i]);
		}
	}
}

TEST(MEMORY, MEMCOPY_OVERLAPPING)
{
	int32 data[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	EXPECT_DEATH(memory_copy(&data[2], &data[0], sizeof(int32) * 8), ".*");

	EXPECT_DEATH(memory_copy(&data[0], &data[4], sizeof(int32) * 5), ".*");
}

TEST(MEMORY, MEMCOPY_NONOVERLAPPING)
{
	int32 data[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	int32 expected[] = { 1, 2, 3, 4, 5, 1, 2, 3, 4, 5 };

	memory_copy(&data[5], &data[0], sizeof(int32) * 5);

	for (int32 i = 0; i < 10; i++)
	{
		EXPECT_EQ(data[i], expected[i]);
	}
}

TEST(MEMORY, MEMMOVE)
{
	int32 data[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	int32 expected[] = { 1, 2, 1, 2, 3, 4, 5, 6, 7, 8 };

	memory_move(&data[2], &data[0], sizeof(int32) * 8);

	for (int32 i = 0; i < 10; i++)
	{
		EXPECT_EQ(data[i], expected[i]);
	}
}

TEST(MEMORY, MEMORY_COMPARE)
{
	byte data_array[4] = { 'd', 'a', 't', 'a' };
	const char* data_string = "data";

	EXPECT_EQ(memory_compare(data_array, data_string, sizeof(data_array)), 0);

	const char* data_neq = "dota";
	EXPECT_NE(memory_compare(data_string, data_neq, 4), 0);
}

TEST(ALLOCATOR, STACK_ALLOCATOR_ALLOC)
{
	byte test_memory[k_byte_kb];
	c_stack_allocator allocator(test_memory, sizeof(test_memory));

	void* int32_ptr = allocator.allocate(sizeof(int32), alignof(int32));
	EXPECT_FALSE(int32_ptr == nullptr);
}

TEST(ALLOCATOR, STACK_ALLOCATOR_MARKERS)
{
	byte test_memory[k_byte_kb];
	c_stack_allocator allocator(test_memory, sizeof(test_memory));

	uint64 first_marker;
	void* int32_ptr = allocator.allocate(sizeof(int32), alignof(int32), first_marker);
	EXPECT_TRUE(allocator.is_marker_valid(first_marker));

	uint64 second_marker;
	void* int64_ptr = allocator.allocate(sizeof(int64), alignof(int64), second_marker);
	EXPECT_TRUE(allocator.is_marker_valid(second_marker));

	allocator.free_to_marker(second_marker);
	EXPECT_FALSE(allocator.is_marker_valid(second_marker));
	EXPECT_TRUE(allocator.is_marker_valid(first_marker));
}

TEST(ALLOCATOR, STACK_ALLOCATOR_TOO_BIG)
{
	byte test_memory[k_byte_kb];
	c_stack_allocator allocator(test_memory, sizeof(test_memory));

	EXPECT_DEATH(allocator.allocate(sizeof(c_static_array<int32,1000>), alignof(c_static_array<int32, 1000>)), ".*");
}

TEST(ALLOCATOR, STACK_ALLOCATOR_JUST_RIGHT)
{
	const int32 buffer_size = 1000;
	byte test_memory[sizeof(c_static_array<byte, buffer_size >)];
	c_stack_allocator allocator(test_memory, sizeof(test_memory));
	
	void* ptr = nullptr;

	EXPECT_NO_FATAL_FAILURE(ptr = allocator.allocate(sizeof(c_static_array<byte, buffer_size >), alignof(c_static_array<byte, buffer_size>)));

	EXPECT_FALSE(ptr == nullptr);
}

TEST(ALLOCATOR, STATIC_STACK_ALLOCATOR)
{
	c_static_stack_allocator<k_byte_kb> allocator;

	void* int32_ptr = allocator.allocate(sizeof(int32), alignof(int32));
	EXPECT_FALSE(int32_ptr == nullptr);
}



