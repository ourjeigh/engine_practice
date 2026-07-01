#include "time.h"
#include "types/time_types.h"
#include "platform/platform.h"
#include "platform/platform_time.h"


static_global int64 g_time_performance_frequency = platform_time_get_performance_frequency();
c_session_time g_session_time;

c_session_time::c_session_time()
{
	m_initial_timestamp = get_high_precision_timestamp();
}

c_engine_time_span c_session_time::get_time_since_start() const
{
	t_timestamp current_timestamp = get_high_precision_timestamp();
	return c_engine_time_span(m_initial_timestamp, current_timestamp);
}

c_engine_time_span c_session_time::time_since_start(t_timestamp time_stamp) const
{
	return c_engine_time_span(m_initial_timestamp, time_stamp);
}

real64 c_engine_time_span::get_duration_seconds() const
{
	// start with the span in ticks
	real64 span = static_cast<real64>(get_delta_raw());

	// divide by ticks per second to get seconds
	span /= g_time_performance_frequency;
	return span;
}

real64 c_engine_time_span::get_duration_milliseconds() const
{
	return get_duration_seconds() * k_milliseconds_in_second;
}

real64 c_engine_time_span::get_duration_microseconds() const
{
	return get_duration_seconds() * k_microseconds_in_second;
}

uint64 c_engine_time_span::get_delta_raw() const
{
	return m_span;
}

c_timer::c_timer() : m_start(k_invalid), m_end(k_invalid)
{
}

void c_timer::start()
{
	m_start = get_high_precision_timestamp();
}

void c_timer::stop()
{
	ASSERT(m_start != k_invalid);
	m_end = get_high_precision_timestamp();
}

c_engine_time_span c_loop_timer::get_loop_time_span_and_continue()
{
	stop();
	c_engine_time_span span = c_engine_time_span(m_start, m_end);
	start();
	return span;
}


uint64 get_high_precision_timestamp()
{
	return platform_time_get_performance_counter();
}

c_engine_time_span get_time_since(t_timestamp since)
{
	return c_engine_time_span(since, get_high_precision_timestamp());
}

const c_session_time* get_session_time()
{
	return &g_session_time;
}