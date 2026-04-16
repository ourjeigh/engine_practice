#include "time.h"

#include "platform/platform.h"

const real64 k_milliseconds_in_second = 1000.0f;
const real64 k_microseconds_in_second = 1000000.0f;

// -----------------------
// move to platform_time...
IGNORE_WINDOWS_WARNINGS_PUSH
#include <windows.h>
#include <timeapi.h>
#include <profileapi.h>
IGNORE_WINDOWS_WARNINGS_PUSH

int64 platform_time_get_performance_frequency()
{
	// A pointer to a variable that receives the current performance-counter frequency, in counts per second. 
	// On systems that run Windows XP or later, the function will always succeed and will thus never return zero.
	LARGE_INTEGER ticks_per_second;
	QueryPerformanceFrequency(&ticks_per_second);
	return ticks_per_second.QuadPart;
}

int64 platform_time_get_performance_counter()
{
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return counter.QuadPart;
}

void platform_time_sleep_for_seconds(real32 seconds)
{
	// anything under 20ms needs 1ms precision
	bool needs_precision = seconds < 0.02f;

	if (needs_precision)
	{
		timeBeginPeriod(1);
	}

	Sleep(static_cast<uint32>(seconds * k_milliseconds_in_second));

	if (needs_precision)
	{
		timeEndPeriod(1);
	}
}
// ...move to platform_time
// -----------------------

static_global int64 g_time_performance_frequency = platform_time_get_performance_frequency();
c_session_time g_session_time;

c_session_time::c_session_time()
{
	m_initial_timestamp = get_high_precision_timestamp();
}

c_time_span c_session_time::get_time_since_start() const
{
	t_timestamp current_timestamp = get_high_precision_timestamp();
	return c_time_span(m_initial_timestamp, current_timestamp);
}

c_time_span c_session_time::time_since_start(t_timestamp time_stamp) const
{
	return c_time_span(m_initial_timestamp, time_stamp);
}

real64 c_time_span::get_duration_seconds() const
{
	// start with the span in ticks
	real64 span = static_cast<real64>(get_delta_raw());

	// divide by ticks per second to get seconds
	span /= g_time_performance_frequency;
	return span;
}

real64 c_time_span::get_duration_milliseconds() const
{
	return get_duration_seconds() * k_milliseconds_in_second;
}

real64 c_time_span::get_duration_microseconds() const
{
	return get_duration_seconds() * k_microseconds_in_second;
}

uint64 c_time_span::get_delta_raw() const
{
	return m_end - m_start;
}

c_timer::c_timer()
{
	m_span = c_time_span();
}

void c_timer::start()
{
	m_span.set_start(get_high_precision_timestamp());
}

void c_timer::stop()
{
	m_span.set_end(get_high_precision_timestamp());
}

c_time_span c_loop_timer::get_loop_time_span_and_continue()
{
	stop();
	c_time_span span = this->m_span;
	start();
	return span;
}


uint64 get_high_precision_timestamp()
{
	return platform_time_get_performance_counter();
}

c_time_span get_time_since(t_timestamp since)
{
	return c_time_span(since, get_high_precision_timestamp());
}

void sleep_for_seconds(real32 seconds)
{
	platform_time_sleep_for_seconds(seconds);
}

void sleep_for_milliseconds(uint32 milliseconds)
{
	timeBeginPeriod(1);
	Sleep(milliseconds);
	timeEndPeriod(1);
}

const c_session_time* get_session_time()
{
	return &g_session_time;
}