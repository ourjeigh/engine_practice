#include "time.h"
#include <windows.h>
#include <profileapi.h>

const real64 k_microseconds_in_second = 1000000.0f;
const real64 k_microseconds_in_millisecond = 1000.0f;
const real64 k_seconds_to_microseconds = 1 / k_microseconds_in_second;
const real64 k_milliseconds_to_microseconds = 1 / k_microseconds_in_millisecond;

real64 c_time_span::get_duration_microseconds()
{
	LARGE_INTEGER frequency;

	QueryPerformanceFrequency(&frequency);
	real64 span = static_cast<real64>(m_end - m_start);
	span *= 1000000;
	span /= frequency.QuadPart;
	return span;
}

real64 c_time_span::get_duration_milliseconds()
{
	return get_duration_microseconds() * k_milliseconds_to_microseconds;
}

real64 c_time_span::get_duration_seconds()
{
	return get_duration_microseconds() * k_seconds_to_microseconds;
}

uint64 get_high_precision_time()
{
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);

	return static_cast<uint64>(counter.QuadPart);
}

void sleep_for_seconds(real32 seconds)
{
	Sleep(static_cast<DWORD>(seconds * 1000.0f));
}