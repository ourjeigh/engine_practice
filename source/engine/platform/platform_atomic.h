#ifndef __PLATFORM_ATOMIC_H__
#define __PLATFORM_ATOMIC_H__
#pragma once

#include "types/types.h"

int32 atomic_exchange_32(volatile int32* dest, int32 exchange);
int64 atomic_exchange_64(volatile int64* dest, int64 exchange);
int32 atomic_compare_exchange_32(volatile int32* dest, int32 exchange, int32 compare);
int64 atomic_compare_exchange_64(volatile int64* dest, int64 exchange, int64 compare);

#endif //__PLATFORM_ATOMIC_H__