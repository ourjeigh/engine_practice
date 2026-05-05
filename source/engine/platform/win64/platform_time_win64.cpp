#include "platform/platform.h"
#ifdef PLATFORM_WIN64

#include "platform/platform_time.h"
#include "platform/win64/win64_includes.h"

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

#endif //PLATFORM_WIN64
