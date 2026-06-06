#ifndef __TIME_TYPES_H__
#define __TIME_TYPES_H__
#pragma once

#include "types.h"

typedef uint64 t_timestamp;

const real64 k_milliseconds_in_second = 1000.0f;
const real64 k_microseconds_in_second = 1000000.0f;

struct s_time_span
{
	real64 get_duration_seconds() { return m_span_seconds; }
	// todo: move time conversions into common and add get_durration_milliseconds here

	real64 m_span_seconds;
};

#endif // !__TIME_TYPES_H__