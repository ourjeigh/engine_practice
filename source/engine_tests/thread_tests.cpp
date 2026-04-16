#include "pch.h"
#include <time/time.h>
#include <platform/platform.h>

#include <thread>
#include <threads/atomic.h>
#include <threads/threads.h>

// enable this to test the std wrapped version of c_atomic
//#define USING_STD_ATOMIC

const int32 k_test_value_1 = 42;
const int32 k_test_value_2 = 100;

void test_function_nop()
{
	NOP();
}

void test_function_loop()
{
	int sum = 0;
	for (int i = 0; i < 1000000; ++i)
	{
		sum += i;
	}
}

TEST(THREADS, CREATE_AND_START_THREAD)
{
	c_thread test_thread;
	bool created = test_thread.create(test_function_loop, nullptr, WIDE("TEST THREAD"));
	EXPECT_TRUE(created);

	bool started = test_thread.start();
	EXPECT_TRUE(started);
	
	// if this fails the thread will not finish
	test_thread.join();
	NOP();
}

// awful test
// start a thread with a supplied arg pointer, with a value of 1
// verify the thread sees the first value
// while that thread is running, change the value
// verify that the thread sees the changed value

void test_function_with_args(int32* param)
{
	assert(*param == k_test_value_1);
	for (int32 i = 0; i < *param; ++i)
	{
		sleep_for_seconds(0.01f);
	}
	assert(*param != k_test_value_1);
}

TEST(THREADS, CREATE_THREAD_WITH_ARGS)
{
	c_thread test_thread;

	int32 loop_count = k_test_value_1;
	test_thread.create(test_function_with_args, THREAD_ARGS(&loop_count), WIDE("TEST THREAD WITH ARGS"));
	test_thread.start();

	sleep_for_seconds(0.1f);
	loop_count = k_test_value_2; // change after starting to ensure arg is passed by pointer
	NOP();
	test_thread.join();
}

TEST(ATOMIC, LOAD_STORE)
{
	c_atomic<int32> atomic_test(0);

	EXPECT_EQ(atomic_test.load(), 0);

	atomic_test.store(5);

	EXPECT_EQ(atomic_test.load(), 5);
}

TEST(ATOMIC, CAS)
{
	c_atomic<int32> atomic_test(10);
	EXPECT_EQ(atomic_test.load(), 10);

	// Correct expected, exchange succeeds.
	int32 expected = 10;
	EXPECT_TRUE(atomic_test.compare_exchange(expected, 20));
	EXPECT_EQ(expected, 10);
	EXPECT_EQ(atomic_test.load(), 20);

	// Incorrect expected, exchange fails
	EXPECT_FALSE(atomic_test.compare_exchange(expected, 30));
	EXPECT_EQ(expected, 20);
	EXPECT_EQ(atomic_test.load(), 20);
}

const int32 k_thread_count = 40;
const int32 k_increment_count = 100000;

TEST(ATOMIC, NON_ATONIC_SANITY_CHECK)
{
	volatile int32 counter = 0;

	std::vector<std::thread> threads;

	for (int t = 0; t < k_thread_count; ++t)
	{
		threads.emplace_back([&counter]
			{
				for (int i = 0; i < k_increment_count; ++i)
				{
					counter++;
				}
			});
	}

	for (auto it = threads.begin(); it != threads.end(); it++)
	{
		it->join();
	}

	const int32 expected = k_thread_count * k_increment_count;
	EXPECT_FALSE(counter == (expected));
}

TEST(ATOMIC, CONCURRENT_INCREMENT)
{
	c_atomic<int32> counter(0);

	std::vector<std::thread> threads;

	for (int t = 0; t < k_thread_count; ++t)
	{
		threads.emplace_back([&counter]
			{
				for (int i = 0; i < k_increment_count; ++i)
				{
					int32 old;
					do {
						old = counter.load();
					} while (!counter.compare_exchange(old, old + 1));
				}
			});
	}

	for (auto it = threads.begin(); it != threads.end(); it++)
	{
		it->join();
	}

	const int32 expected = k_thread_count * k_increment_count;
	EXPECT_EQ(counter.load(), (expected));
}