#ifndef __TIME_H__
#define __TIME_H__
#pragma once

#include <types/types.h>

class c_time_span
{
public:
	c_time_span() : m_start(-1), m_end(-1) {}
	c_time_span(uint64 start) : m_start(start), m_end(-1) {}
	c_time_span(uint64 start, uint64 end) : m_start(start), m_end(end) {}

	// these should probably be moved to something like c_timer which holds a time span
	void start() { m_start = get_high_precision_time(); }
	void stop() { m_end = get_high_precision_time(); }

	void set_start(uint64 start) { m_start = start; }
	void set_end(uint64 end) { m_end = end; }
	real64 get_duration_microseconds();
	real64 get_duration_milliseconds();
	real64 get_duration_seconds();

private:
	uint64 m_start;
	uint64 m_end;
};

uint64 get_high_precision_time();

void sleep_for_seconds(real32 seconds);

#endif //__TIME_H__
