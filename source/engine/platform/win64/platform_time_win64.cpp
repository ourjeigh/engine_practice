#include "platform/platform.h"
#include "platform/platform_time.h"

IGNORE_WINDOWS_WARNINGS_PUSH
#include "windows.h"
#include "timeapi.h"
#include "profileapi.h"
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

void platform_time_sleep_for_milliseconds(uint32 milliseconds)
{
	// anything under 20ms needs 1ms precision
	bool needs_precision = milliseconds < 20;

	if (needs_precision)
	{
		timeBeginPeriod(1);
	}

	Sleep(milliseconds);

	if (needs_precision)
	{
		timeEndPeriod(1);
	}
}
