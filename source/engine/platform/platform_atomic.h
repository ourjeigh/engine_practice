#ifndef __PLATFORM_ATOMIC_H__
#define __PLATFORM_ATOMIC_H__
#pragma once

#include "types/types.h"

int32 atomic_load_32(const volatile int32* dest);
int64 atomic_load_64(const volatile int64* dest);
void atomic_store_32(volatile int32* dest, int32 value);
void atomic_store_64(volatile int64* dest, int64 value);
int32 atomic_exchange_32(volatile int32* dest, int32 exchange);
int64 atomic_exchange_64(volatile int64* dest, int64 exchange);
int32 atomic_compare_exchange_32(volatile int32* dest, int32 exchange, int32 compare);
int64 atomic_compare_exchange_64(volatile int64* dest, int64 exchange, int64 compare);

// these don't necessarily belong here, move/rename once they are used by something other than atomic
void atomic_memory_fence();
void atomic_memory_read_barrier();
void atomic_memory_write_barrier();
void atomic_memory_read_write_barrier();

#endif //__PLATFORM_ATOMIC_H__