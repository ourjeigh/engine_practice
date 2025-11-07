#include "pch.h"
#include "asserts.cpp"
#include "threads/threads.cpp"
#include "logging/logging.cpp"
#include <time/time.h>

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
	bool created = test_thread.create(test_function_loop, WIDE("TEST THREAD"));
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
