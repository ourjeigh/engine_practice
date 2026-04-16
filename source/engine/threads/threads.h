#ifndef __THREADS_H__
#define __THREADS_H__
#pragma once

#include <types/types.h>

#define THREAD_ARGS(x) reinterpret_cast<void*>(x)
#define THREAD_FUNCTION(x) reinterpret_cast<void*>(x)

enum e_thread_priority
{
	thread_priority_lowest,
	thread_priority_below_normal,
	thread_priority_normal,
	thread_priority_above_normal,
	thread_priority_highest,
	thread_priority_time_critical,
};

struct s_thread_properties
{
	const wchar* name;
	e_thread_priority priority;
	void* function;
	void* param;
};

class c_thread
{
public:
	// todo: remove this, use s_thread_properties instead
	bool create(void* function, void* param, const wchar* name = nullptr);
	bool create(s_thread_properties& properties);
	bool start();
	void join();
	
	uint32 get_thread_id() { return m_thread_id; }

private:
	//const wchar* m_thread_name;
	uint32 m_thread_id;
	//void* m_function;
	//void* m_param;

	s_thread_properties m_thread_properties;
};

uint32 get_current_thread_id();

#endif //__THREADS_H__