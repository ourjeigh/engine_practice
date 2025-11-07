#ifndef __THREADS_H__
#define __THREADS_H__
#pragma once

#include <types/types.h>







#define THREAD_ARGS(x) reinterpret_cast<void*>(x)
#define THREAD_FUNCTION(x) reinterpret_cast<void*>(x)

class c_thread
{
public:
	// don't forget the param is a pointer, so whatever value it is could be changed
	// while the thread is running!
	bool create(void* function, void* param, const wchar* name = nullptr);
	bool start();
	void join();
	
	uint32 get_thread_id() { return m_thread_id; }

private:
	const wchar* m_thread_name;
	uint32 m_thread_id;
	void* m_function;
	void* m_param;
};

#endif //__THREADS_H__