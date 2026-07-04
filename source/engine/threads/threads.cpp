#include "threads.h"
#include "debug/logging.h"

#include "platform/platform.h"
#include "platform/platform_thread.h"

bool c_thread::create(void* function, void* param, const wchar* name)
{
	m_thread_properties.name = name;
	m_thread_properties.function = function;
	m_thread_properties.param = param;

	return true;
}

bool c_thread::create(s_thread_properties& properties)
{
	m_thread_properties = properties;
	return true;
}

bool c_thread::start()
{
	m_thread_id = platform_thread_create(m_thread_properties);

	if (m_thread_id == k_invalid)
	{
		log_message(error, "c_thread: failed to create thread [id: 0x{x}, name: {s}]",
			m_thread_id,
			m_thread_properties.name); // need to convert from wchar
	}
	else
	{
		log_message(verbose, "c_thread: thread started [id: 0x{x}, name: {s}]",
			m_thread_id,
			m_thread_properties.name); // need to convert from wchar
	}

	return m_thread_id != k_invalid;
}

void c_thread::join()
{
	platform_thread_join(m_thread_id);
}

uint32 get_current_thread_id()
{
	return platform_thread_get_current_thread_id();
}

void thread_sleep_for_milliseconds(uint32 milliseconds)
{
	platform_thread_sleep_for_milliseconds(milliseconds);
}
