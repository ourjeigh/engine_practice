#include "threads.h"
#include <debug/logging.h>

#include "platform/platform.h"

IGNORE_WINDOWS_WARNINGS_PUSH
#include "windows.h"
#include <comdef.h>
IGNORE_WINDOWS_WARNINGS_POP

int get_thread_priority(e_thread_priority priority)
{
	switch (priority)
	{
	case thread_priority_lowest:
		return THREAD_PRIORITY_LOWEST;
	case thread_priority_below_normal:
		return THREAD_PRIORITY_BELOW_NORMAL;
	case thread_priority_normal:
		return THREAD_PRIORITY_NORMAL;
	case thread_priority_above_normal:
		return THREAD_PRIORITY_ABOVE_NORMAL;
	case thread_priority_highest:
		return THREAD_PRIORITY_HIGHEST;
	case thread_priority_time_critical:
		return THREAD_PRIORITY_TIME_CRITICAL;
	default:
		return THREAD_PRIORITY_NORMAL;
	}
}

bool c_thread::create(void* function, void* param, const wchar* name)
{
	m_thread_properties.name = name;
	m_thread_properties.function = function;
	m_thread_properties.param = param;

	//m_function = function;
	//m_param = param;
	//m_thread_name = name;
	return true;
}

bool c_thread::create(s_thread_properties& properties)
{
	m_thread_properties = properties;
	return true;
}

bool c_thread::start()
{
	LPSECURITY_ATTRIBUTES lpThreadAttributes = nullptr;
	uint64 dwStackSize= 0;
	DWORD dwCreationFlags = 0;
	bool success = true;

	HANDLE h = CreateThread(
		lpThreadAttributes,
		dwStackSize,
		(LPTHREAD_START_ROUTINE)m_thread_properties.function,
		m_thread_properties.param,
		dwCreationFlags,
		&m_thread_id);

	if (h != nullptr )
	{
		if (m_thread_properties.name != nullptr) 
		{
			HRESULT hr = SetThreadDescription(h, m_thread_properties.name);
			if (FAILED(hr))
			{
				// if we create the thread but fail to name it, we'll still call that a success
				log_message(warning, "Failed to set thread description! [{s}]", _com_error(hr).ErrorMessage());
			}
		}

		int priority = get_thread_priority(m_thread_properties.priority);

		if (!SetThreadPriority(h, priority))
		{
			log_message(warning, "Failed to set thread priority!");
		}
	}
	else
	{
		log_message(error, "Failed to create thread!");
		success = false;
	}

	log_message(verbose, "c_thread: started thread [id: 0x{x}, name: {s}]",
		m_thread_id,
		m_thread_properties.name); // need to convert from wchar

	return success;
}

void c_thread::join()
{
	HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, m_thread_id);
	if (hThread != nullptr)
	{
		WaitForSingleObject(hThread, INFINITE);
		CloseHandle(hThread);
	}
	else
	{
		log_message(error, "Failed to open thread for joining!");
		return;
	}

}

uint32 get_current_thread_id()
{
	return GetCurrentThreadId();
}
