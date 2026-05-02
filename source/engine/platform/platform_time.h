#ifndef __PLATFORM_TIME_H__
#define __PLATFORM_TIME_H__
#pragma once

int64 platform_time_get_performance_frequency();
int64 platform_time_get_performance_counter();
void platform_time_sleep_for_milliseconds(uint32 milliseconds);

#endif // __PLATFORM_TIME_H__