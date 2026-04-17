#ifndef __TIME_TYPES_H__
#define __TIME_TYPES_H__
#pragma once

#include "types.h"

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
	uint64 get_delta_raw() const;

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
	const c_time_span* get_time_span() { stop(); return &m_span; }
	void reset() { m_span.clear(); }

protected:
	c_time_span m_span;
};

class c_loop_timer : public c_timer
{
public:
	c_time_span get_loop_time_span_and_continue();
};

class c_session_time
{
public:
	c_session_time();

	c_time_span get_time_since_start() const;
	c_time_span time_since_start(t_timestamp time_stamp) const;

private:
	t_timestamp m_initial_timestamp;
};

#endif // !__TIME_TYPES_H__