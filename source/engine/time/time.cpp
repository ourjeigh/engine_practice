#include "time.h"

#include "platform/platform.h"
#include "platform/platform_time.h"

const real64 k_milliseconds_in_second = 1000.0f;
const real64 k_microseconds_in_second = 1000000.0f;



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
	platform_time_sleep_for_milliseconds(seconds * k_milliseconds_in_second);
}

void sleep_for_milliseconds(uint32 milliseconds)
{
	platform_time_sleep_for_milliseconds(milliseconds);
}

const c_session_time* get_session_time()
{
	return &g_session_time;
}