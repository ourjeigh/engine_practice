#ifndef __TIME_H__
#define __TIME_H__
#pragma once

#include "types/types.h"
#include "types/time_types.h"

const uint64 k_time_nanoseconds_in_millisecond = 1000000ULL;

t_timestamp get_high_precision_timestamp();
c_time_span get_time_since(t_timestamp since);

void sleep_for_seconds(real32 seconds);
void sleep_for_milliseconds(uint32 milliseconds);

const c_session_time* get_session_time();
#endif //__TIME_H__
