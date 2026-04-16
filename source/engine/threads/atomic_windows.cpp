#include "atomic.h"
#include "platform/platform.h"

#ifdef PLATFORM_WINDOWS

IGNORE_WINDOWS_WARNINGS_PUSH
#include "windows.h"
IGNORE_WINDOWS_WARNINGS_POP

int32 atomic_exchange_32(volatile int32* dest, int32 value)
{
	return InterlockedExchange(dest, value);
}

int64 atomic_exchange_64(volatile int64* dest, int64 value)
{
	return InterlockedExchange64(dest, value);
}

int32 atomic_compare_exchange_32(volatile int32* dest, int32 exchange, int32 compare)
{
	return InterlockedCompareExchange(dest, exchange, compare);
}

int64 atomic_compare_exchange_64(volatile int64* dest, int64 exchange, int64 compare)
{
	return InterlockedCompareExchange64(dest, exchange, compare);
}

#endif //PLATFORM_WINDOWS
