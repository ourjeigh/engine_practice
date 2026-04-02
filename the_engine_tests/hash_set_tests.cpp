#include "pch.h"
#include <types/types.h>
#include "structures/hash_set.h"

TEST(HASH_TABLES, HASH_TABLE_BASE)
{
	struct s_get_key
	{
		int32& operator()(int32& key) { return key; }
		const int32& operator()(const int32& key) { return key; }
	};

	c_linear_hash_table_base<int32, int32, 128, s_get_key> test_table;

	EXPECT_EQ(test_table.find_or_insert(256), 256);
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

	// Test inserting a key-value pair
	//EXPECT_TRUE(map.insert(1, 1.5f));
	map.insert(1) = 1.5f;
	EXPECT_TRUE(map.contains(1));
	EXPECT_EQ(map.find(1), 1.5f);

	// Test updating an existing key
	//EXPECT_TRUE(map.insert(1, 2.5f)); // Assuming insert updates the value
	map[1] = 2.5f;
	EXPECT_EQ(map.find(1), 2.5f);

	// Test inserting multiple key-value pairs
	//EXPECT_TRUE(map.insert(2, 3.5f));
	//EXPECT_TRUE(map.insert(3, 4.5f));
	map.insert(2) = 3.5f;
	map.insert(3) = 4.5f;
	EXPECT_TRUE(map.contains(2));
	EXPECT_TRUE(map.contains(3));
	EXPECT_EQ(map.find(2), 3.5f);
	EXPECT_EQ(map.find(3), 4.5f);

	// Test removing a key-value pair
	EXPECT_TRUE(map.remove(2));
	EXPECT_FALSE(map.contains(2));
	EXPECT_EQ(map.used(), 2);

	// Test removing a non-existent key
	EXPECT_FALSE(map.remove(99));
	EXPECT_EQ(map.used(), 2);

	// Test clearing the map
	map.clear();
	EXPECT_FALSE(map.contains(1));
	EXPECT_FALSE(map.contains(3));
	EXPECT_EQ(map.used(), 0);

	// Test the size of the map
	map[10] = 5.5f;
	map[20] = 6.5f;
	EXPECT_EQ(map.used(), 2);

	// Test map capacity
	map.clear();
	EXPECT_FALSE(map.full());
	for (int32 i = 0; i < 128; i++)
	{
		EXPECT_FALSE(map.full());
		map[i] = static_cast<real32>(i) + 0.5f;
	}

	EXPECT_TRUE(map.full());

	real32 new_val;
	EXPECT_FALSE(map.try_find_or_insert(129, new_val));

	// Verify all inserted values
	for (int32 i = 0; i < 128; i++)
	{
		EXPECT_TRUE(map.contains(i));
		EXPECT_EQ(map.find(i), static_cast<real32>(i) + 0.5f);
	}
}

TEST(HASH_TABLES, ReclaimsTombstones)
{
	const int32 capacity = 10;
	c_hash_set<int32, capacity> set;

	// 1. Fill it up to the limit
	for (int32 i = 0; i < capacity; ++i) 
	{
		EXPECT_TRUE(set.insert(i));
	}

	EXPECT_TRUE(set.full());

	// 2. Remove everything
	for (int32 i = 0; i < capacity; ++i) 
	{
		EXPECT_TRUE(set.remove(i));
	}
	EXPECT_EQ(set.used(), 0);

	// 3. Try to fill it again. 
	// If your find_index doesn't stop at DELETED slots for insertion,
	// this will likely trigger your ASSERT(!full()) or fail to find an EMPTY slot.
	for (int32 i = 0; i < capacity; ++i) 
	{
		EXPECT_TRUE(set.insert(i + 100));
	}
	EXPECT_EQ(set.used(), capacity);
}

TEST(LinearHashTableTest, HandlesSaturatedProbeChain) {
	const int32 capacity = 5;
	c_hash_set<int32, capacity> mySet;

	// Fill to capacity
	for (int32 i = 0; i < capacity; ++i) {
		mySet.insert(i);
	}

	// Now remove some to create DELETED states
	mySet.remove(0);
	mySet.remove(1);

	// If your loop only stops at EMPTY, and the rest are OCCUPIED/DELETED,
	// searching for a non-existent key might loop forever.
	// We use a timeout approach or just hope the ASSERTs catch it.
	EXPECT_FALSE(mySet.contains(999));
}

struct s_padded_struct {
	char a;
	// 3 bytes of padding usually live here
	int32 b;

	bool operator==(const s_padded_struct& other) const
	{
		return a == other.a && b == other.b;
	}
};

// move these into hash tests
TEST(LinearHashTableTest, IgnoresPaddingNoise) {
	// We must use the user's default hasher to catch the bug
	c_hash_set<s_padded_struct, 10> mySet;

	s_padded_struct s1;
	std::memset(&s1, 0x00, sizeof(s1)); // Zero out padding
	s1.a = 'A'; s1.b = 42;

	s_padded_struct s2;
	std::memset(&s2, 0xFF, sizeof(s2)); // Fill padding with "noise"
	s2.a = 'A'; s2.b = 42;

	mySet.insert(s1);

	// If the hasher uses sizeof(), it will hash the 0x00 vs 0xFF padding,
	// causing contains(s2) to return false even though the data is identical.
	EXPECT_TRUE(mySet.contains(s2)) << "Hasher is sensitive to padding garbage!";
}

TEST(LinearHashTableTest, HashesStringContentNotAddress) {
	c_hash_set<const char*, 10> mySet;

	const char* str1 = "hello";
	char str2_buf[] = "hello";
	const char* str2 = str2_buf;

	mySet.insert(str1);

	// str1 and str2 point to different memory addresses but have same content.
	// If the hasher uses sizeof(value), it hashes the 4 or 8-byte pointer address.
	EXPECT_TRUE(mySet.contains(str2)) << "Hasher is hashing pointer addresses, not content!";
}