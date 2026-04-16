#include "pch.h"

// need to figure out why this isn't getting linked properly
#include "time/time.h"
#include <asserts.h>
#include <platform/platform.h>

// tbd if it's a good idea to be testing these low level operations, just need a sanity check
TEST(TIME, GET_HIGH_PRECISION_TIME)
{
	uint64 time1 = get_high_precision_timestamp();
	uint64 time2 = get_high_precision_timestamp();
	EXPECT_LE(time1, time2);
}

TEST(TIME, TIME_SPAN_DURATION)
{
	uint64 start = get_high_precision_timestamp();

	// waste some time to increase span time
	for (int i = 0; i < 1000000; ++i)
	{
		NOP();
	}

	uint64 end = get_high_precision_timestamp();
	c_time_span time_span(start, end);

	real64 duration_millis = time_span.get_duration_milliseconds();
	real64 duration_micros = time_span.get_duration_microseconds();
	real64 duration_seconds = time_span.get_duration_seconds();

	// this may not always match system tolerance
	const real64 tolerance = 1e-6f;

	EXPECT_NEAR(duration_millis * 1000.0f, duration_micros, tolerance);
	EXPECT_NEAR(duration_seconds * 1000.0f, duration_millis, tolerance);
}

TEST(TIME, SLEEP)
{
	c_timer timer;
	timer.start();

	const real32 sleep_duration_seconds = 0.1f;
	sleep_for_seconds(sleep_duration_seconds);
	timer.stop();

	// 25% tolerance since we can't guarantee sleep accuracy
	real64 tolerance = sleep_duration_seconds * 0.25f; 
	real64 diff = abs(timer.get_time_span()->get_duration_seconds() - sleep_duration_seconds);

	EXPECT_LE(diff, tolerance);
	//EXPECT_NEAR(timer.get_time_span()->get_duration_seconds(), sleep_duration_seconds, tolerance);
}