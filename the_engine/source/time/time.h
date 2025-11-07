#ifndef __TIME_H__
#define __TIME_H__
#pragma once

#include <types/types.h>

typedef uint64 t_timestamp;

class c_time_span
{
public:
	c_time_span() { clear(); }
	c_time_span(t_timestamp start) : m_start(start), m_end(-1) {}
	c_time_span(t_timestamp start, t_timestamp end) : m_start(start), m_end(end) {}
	
	void clear() { m_start = -1; m_end = -1; }
	void set_start(t_timestamp start) { m_start = start; }
	void set_end(t_timestamp end) { m_end = end; }

	real64 get_duration_microseconds() const;
	real64 get_duration_milliseconds() const;
	real64 get_duration_seconds() const;

private:
	t_timestamp m_start;
	t_timestamp m_end;
};

class c_timer
{
public:
	c_timer();
	void start();
	void stop();
	const c_time_span* get_time_span() { return &m_span; }
	void reset() { m_span.clear(); }

private:
	c_time_span m_span;
};

t_timestamp get_high_precision_timestamp();
c_time_span get_time_since(t_timestamp since);

void sleep_for_seconds(real32 seconds);
void sleep_for_milliseconds(uint32 milliseconds);

#endif //__TIME_H__
