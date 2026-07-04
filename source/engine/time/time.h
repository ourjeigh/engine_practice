#ifndef __TIME_H__
#define __TIME_H__
#pragma once

#include "types/types.h"
#include "types/time_types.h"
#include "debug/asserts.h"

class c_engine_time_span
{
public:
	c_engine_time_span() { clear(); }
	c_engine_time_span(t_timestamp start, t_timestamp end) : m_span(end - start) {}

	void clear() { m_span = k_invalid; }

	real64 get_duration_microseconds() const;
	real64 get_duration_milliseconds() const;
	real64 get_duration_seconds() const;
	uint64 get_delta_raw() const;

	static_member_function s_time_span to_time_span(t_timestamp start, t_timestamp end)
	{
		ASSERT(start != k_invalid && end != k_invalid);
		c_engine_time_span span(start, end);
		return s_time_span(span.get_duration_seconds());
	}

private:
	uint64 m_span;
};

class c_timer
{
public:
	c_timer();
	void start();
	void stop();

	const c_engine_time_span get_time_span()
	{
		stop();
		return c_engine_time_span(m_start, m_end);
	}

	void reset()
	{
		m_start = k_invalid;
		m_end = k_invalid;
	}

protected:
	t_timestamp m_start;
	t_timestamp m_end;
};

class c_loop_timer : public c_timer
{
public:
	c_engine_time_span get_loop_time_span_and_continue();
};

class c_session_time
{
public:
	c_session_time();

	c_engine_time_span get_time_since_start() const;
	c_engine_time_span time_since_start(t_timestamp time_stamp) const;

private:
	t_timestamp m_initial_timestamp;
};

t_timestamp get_high_precision_timestamp();
c_engine_time_span get_time_since(t_timestamp since);

const c_session_time* get_session_time();
#endif //__TIME_H__
