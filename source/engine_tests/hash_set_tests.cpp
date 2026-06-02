#include "pch.h"
#include "types/types.h"
#include "structures/hash_set.h"

TEST(HASH_TABLES, HASH_TABLE_BASE)
{
	struct s_get_key
	{
		int32& operator()(int32& key) { return key; }
		const int32& operator()(const int32& key) { return key; }
	};

	c_linear_hash_table_base<int32, int32, 128, s_get_key> test_table;

	bool found;
	EXPECT_EQ(test_table.find_or_insert(256, found), 256);
	EXPECT_TRUE(test_table.contains(256));

	const int32 find = test_table.find_const(256);
	EXPECT_EQ(find, 256);

	EXPECT_FALSE(test_table.contains(25));

	test_table.remove(256);
	EXPECT_FALSE(test_table.contains(256));
}

TEST(HASH_TABLES, HASH_SET)
{
	c_hash_set<int32, 128> set;

	// Test inserting a value
	EXPECT_TRUE(set.insert(42));
	EXPECT_TRUE(set.contains(42));

	// Test inserting a duplicate value
	EXPECT_FALSE(set.insert(42));

	// Test removing a value
	EXPECT_TRUE(set.remove(42));
	EXPECT_FALSE(set.contains(42));
	EXPECT_EQ(set.used(), 0);

	// Test removing a non-existent value
	EXPECT_FALSE(set.remove(99));
	EXPECT_EQ(set.used(), 0);

	// Test inserting multiple values
	EXPECT_TRUE(set.insert(1));
	EXPECT_TRUE(set.insert(2));
	EXPECT_TRUE(set.insert(3));
	EXPECT_TRUE(set.contains(1));
	EXPECT_TRUE(set.contains(2));
	EXPECT_TRUE(set.contains(3));
	EXPECT_EQ(set.used(), 3);
	EXPECT_TRUE(set.remove(3));
	EXPECT_EQ(set.used(), 2);

	// Test clearing the set
	set.clear();
	EXPECT_FALSE(set.contains(1));
	EXPECT_FALSE(set.contains(2));
	EXPECT_FALSE(set.contains(3));
	EXPECT_EQ(set.used(), 0);

	// Test the size of the set
	set.insert(10);
	set.insert(20);
	EXPECT_EQ(set.used(), 2);

	set.clear();

	EXPECT_FALSE(set.full());

	for (int32 i = 0; i < 128; i++)
	{
		EXPECT_FALSE(set.full());
		EXPECT_TRUE(set.insert(i));
	}

	EXPECT_TRUE(set.full());
	EXPECT_FALSE(set.insert(129));
}

TEST(HASH_TABLES, HASH_MAP)
{
	c_hash_map<int32, real32, 128> map;

	// inserting
	map.insert(1) = 1.5f;
	EXPECT_TRUE(map.contains(1));
	EXPECT_EQ(map.find(1), 1.5f);

	// updating an existing key
	map[1] = 2.5f;
	EXPECT_EQ(map.find(1), 2.5f);

	// inserting multiple
	map.insert(2) = 3.5f;
	map.insert(3) = 4.5f;
	EXPECT_TRUE(map.contains(2));
	EXPECT_TRUE(map.contains(3));
	EXPECT_EQ(map.find(2), 3.5f);
	EXPECT_EQ(map.find(3), 4.5f);

	// removing
	EXPECT_TRUE(map.remove(2));
	EXPECT_FALSE(map.contains(2));
	EXPECT_EQ(map.used(), 2);

	// removing a non-existent key
	EXPECT_FALSE(map.remove(99));
	EXPECT_EQ(map.used(), 2);

	// clearing
	map.clear();
	EXPECT_FALSE(map.contains(1));
	EXPECT_FALSE(map.contains(3));
	EXPECT_EQ(map.used(), 0);

	// size 
	map[10] = 5.5f;
	map[20] = 6.5f;
	EXPECT_EQ(map.used(), 2);

	// capacity
	map.clear();
	EXPECT_FALSE(map.full());
	for (int32 i = 0; i < 128; i++)
	{
		EXPECT_FALSE(map.full());
		map[i] = static_cast<real32>(i) + 0.5f;
	}

	EXPECT_TRUE(map.full());

	// inserting when false fails
	real32* new_val;
	EXPECT_FALSE(map.try_find_or_insert(129, &new_val));

	// Verify all inserted values
	for (int32 i = 0; i < 128; i++)
	{
		EXPECT_TRUE(map.contains(i));
		EXPECT_EQ(map.find(i), static_cast<real32>(i) + 0.5f);
	}
}

TEST(HASH_TABLES, RECLAIMS_DELETED_ITEMS)
{
	const int32 capacity = 10;
	c_hash_set<int32, capacity> set;

	// Fill it up to the limit
	for (int32 i = 0; i < capacity; ++i)
	{
		EXPECT_TRUE(set.insert(i));
	}

	EXPECT_TRUE(set.full());

	// Remove everything
	for (int32 i = 0; i < capacity; ++i)
	{
		EXPECT_TRUE(set.remove(i));
	}
	EXPECT_EQ(set.used(), 0);

	// 3. Try to fill it again now that all slots will be in deleted state
	for (int32 i = 0; i < capacity; ++i)
	{
		EXPECT_TRUE(set.insert(i + 100));
	}
	EXPECT_EQ(set.used(), capacity);
}

TEST(HASH_TABLES, FAILED_SEARCH_WHEN_FULL)
{
	const int32 capacity = 5;
	c_hash_set<int32, capacity> mySet;

	// Fill to capacity
	for (int32 i = 0; i < capacity; ++i)
	{
		mySet.insert(i);
	}

	mySet.remove(0);
	mySet.remove(1);

	// if we don't properly exit the find_index this would loop forever
	EXPECT_FALSE(mySet.contains(999));
}
