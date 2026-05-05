#include "platform/platform.h"
#ifdef PLATFORM_WIN64

#include "platform/platform_thread.h"
#include "platform/win64/win64_includes.h"


int32 get_thread_priority(e_thread_priority priority);

uint32 platform_thread_create(s_thread_properties& properties)
{
	LPSECURITY_ATTRIBUTES lpThreadAttributes = nullptr;
	uint64 dwStackSize = 0;
	DWORD dwCreationFlags = 0;
	bool success = true;

	uint32 out_thread_id = k_invalid;
	
	HANDLE h = CreateThread(
		lpThreadAttributes,
		dwStackSize,
		(LPTHREAD_START_ROUTINE)properties.function,
		properties.param,
		dwCreationFlags,
		&out_thread_id);

	if (h != nullptr)
	{
		if (properties.name != nullptr)
		{
			HRESULT hr = SetThreadDescription(h, properties.name);
			if (FAILED(hr))
			{
				// if we create the thread but fail to name it, we'll still call that a success
				//log_message(warning, "Failed to set thread description! [{s}]", _com_error(hr).ErrorMessage());
			}
		}

		int priority = get_thread_priority(properties.priority);

		if (!SetThreadPriority(h, priority))
		{
			//log_message(warning, "Failed to set thread priority!");
		}
	}
	else
	{
		//log_message(error, "Failed to create thread!");
		success = false;
	}

	

	return out_thread_id;
}

void platform_thread_join(uint32 thread_id)
{
	HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, thread_id);
	if (hThread != nullptr)
	{
		WaitForSingleObject(hThread, INFINITE);
		CloseHandle(hThread);
	}
	else
	{
		//log_message(error, "Failed to open thread for joining!");
		return;
	}
}

uint32 platform_thread_get_current_thread_id()
{
	return GetCurrentThreadId();
}

// private
int32 get_thread_priority(e_thread_priority priority)
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
#endif // PLATFORM_WIN64