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
#endif //PLATFORM_WIN64
