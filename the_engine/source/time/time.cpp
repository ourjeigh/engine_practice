#include "time.h"
#include <windows.h>
#include <profileapi.h>

const real64 k_millisecons_in_second = 1000.0f;
const real64 k_microseconds_in_second = 1000000.0f;

real64 c_time_span::get_duration_seconds() const
{
	// A pointer to a variable that receives the current performance-counter frequency, in counts per second. 
	// On systems that run Windows XP or later, the function will always succeed and will thus never return zero.
	LARGE_INTEGER ticks_per_second;
	QueryPerformanceFrequency(&ticks_per_second);

	// start with the span in ticks
	real64 span = static_cast<real64>(m_end - m_start);

	// divide by ticks per second to get seconds
	span /= ticks_per_second.QuadPart;
	return span;
}

real64 c_time_span::get_duration_milliseconds() const
{
	return get_duration_seconds() * k_millisecons_in_second;
}

real64 c_time_span::get_duration_microseconds() const
{
	return get_duration_seconds() * k_microseconds_in_second;
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

uint64 get_high_precision_timestamp()
{
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);

	return static_cast<uint64>(counter.QuadPart);
}

c_time_span get_time_since(t_timestamp since)
{
	return c_time_span(since, get_high_precision_timestamp());
}


void sleep_for_seconds(real32 seconds)
{
	Sleep(static_cast<uint32>(seconds * k_millisecons_in_second));
}

void sleep_for_milliseconds(uint32 milliseconds)
{
	Sleep(milliseconds);
}