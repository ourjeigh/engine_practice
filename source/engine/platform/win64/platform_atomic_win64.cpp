#include "platform/platform.h"
#ifdef PLATFORM_WIN64

#include "platform/win64/win64_includes.h"

int32 atomic_load_32(const volatile int32* dest)
{
	return __iso_volatile_load32(reinterpret_cast<const volatile int*>(dest));
}

int64 atomic_load_64(const volatile int64* dest)
{
	return __iso_volatile_load64(dest);
}

void atomic_store_32(volatile int32* dest, int32 value)
{
	__iso_volatile_store32(reinterpret_cast<volatile int*>(dest), value);
}

void atomic_store_64(volatile int64* dest, int64 value)
{
	__iso_volatile_store64(dest, value);
}

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

void atomic_memory_fence()
{
	_mm_mfence();
}

void atomic_memory_read_barrier()
{
	_ReadBarrier();
}

void atomic_memory_write_barrier()
{
	_WriteBarrier();
}

void atomic_memory_read_write_barrier()
{
	_ReadWriteBarrier();
}

#endif //PLATFORM_WIN64
