#ifndef __PERF_H__
#define __PERF_H__
#pragma once
#include "config.h"

#ifdef FEATURE_PERF_MEASUREMENT

#include "structures/string/string_id.h"
#include "time/time.h"
#include "threads/threads.h"

// these cause their id's constructors to get called at runtime, running the fnv1 hash the first time
// they're called. not ideal
#define PERF_MEASURE_FUNCTION() s_stack_perf_measurer measurer(c_string_id(__FUNCTION__))
#define PERF_MEASURE_SECTION(name) s_stack_perf_measurer measurer(c_string_id(name))

struct s_perf_measurement
{
	s_perf_measurement() {}

	s_perf_measurement(c_string_id id, c_time_span span, uint32 thread_id) :
		id(id),
		span(span),
		thread_id(thread_id) {
	}

	c_string_id id;
	c_time_span span;
	uint32 thread_id;
};

struct s_stack_perf_measurer
{
	s_stack_perf_measurer(c_string_id id);
	~s_stack_perf_measurer();

	c_string_id id;
	c_timer timer;
};

#else // FEATURE_PERF_MEASUREMENT

#define PERF_MEASURE_FUNCTION()
#define PERF_MEASURE_SECTION(name)

#endif // FEATURE_PERF_MEASUREMENT
#endif //__PERF_H__
