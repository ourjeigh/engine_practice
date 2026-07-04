#include "platform/platform.h"
#ifdef PLATFORM_WIN64

#include "platform/platform_thread.h"
#include "platform/win64/win64_includes.h"
#include "platform/win64/platform_handle_win64.h"

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

void platform_thread_sleep_for_milliseconds(uint32 milliseconds)
{
	// if you need sub-20ms precision, use a waitable timer
	// platform_thread_create_waitable_timer
	// platform_thread_start_waitable_timer
	// platform_thread_wait_for_signalled_object
	ASSERT(milliseconds >= 20);
	
	// leaving here as an artifact, but we want to avoid timeBeginPeriod
	// because it's a bit heavyhanded.

	// anything under 20ms needs 1ms precision
	/*bool needs_precision = milliseconds < 20;
	if (needs_precision)
	{
		timeBeginPeriod(1);
	}*/

	Sleep(milliseconds);

	/*if (needs_precision)
	{
		timeEndPeriod(1);
	}*/
}

c_platform_handle platform_thread_create_event(bool manual_reset, bool start_signalled, c_string name)
{
	c_platform_handle out_handle = c_platform_handle::invalid();
	
	HANDLE event_handle = CreateEvent(nullptr, manual_reset, start_signalled, name.get_const_char());
	if (event_handle != nullptr)
	{
		out_handle = c_platform_handle_factory::get_platform_handle_from_native_handle<HANDLE>(event_handle);
	}

	return out_handle;
}

bool platform_thread_signal_event(c_platform_handle& event_handle)
{
	return SetEvent(c_platform_handle_factory::get_native_handle_from_platform_handle<HANDLE>(event_handle));
}

e_signalled_object_result platform_thread_wait_for_signalled_object(c_platform_handle& object, uint32 timeout_ms)
{
	e_signalled_object_result out_result = signalled_object_result_failed;

	if (timeout_ms == k_wait_time_infinite)
	{
		timeout_ms = INFINITE;
	}

	DWORD result = 	WaitForSingleObject(
		c_platform_handle_factory::get_native_handle_from_platform_handle<HANDLE>(object),
		timeout_ms);

	switch (result)
	{
	case WAIT_OBJECT_0:
		out_result = signalled_object_result_signalled;
		break;
	case WAIT_ABANDONED:
		out_result = signalled_object_result_abandoned;
		break;
	case WAIT_TIMEOUT:
		out_result = signalled_object_result_timed_out;
		break;
	case WAIT_FAILED:
		out_result = signalled_object_result_failed;
		break;
	default:
		HALT("unreachable");
	}

	return out_result;
}

c_platform_handle platform_thread_create_waitable_timer(bool manual_reset, bool high_resolution, c_string name)
{
	c_platform_handle out_handle = c_platform_handle::invalid();

	DWORD flags = 0;
	DWORD access = TIMER_ALL_ACCESS;
	if (manual_reset)
	{
		flags |= CREATE_WAITABLE_TIMER_MANUAL_RESET;
	}

	if (high_resolution)
	{
		flags |= CREATE_WAITABLE_TIMER_HIGH_RESOLUTION;
	}

	HANDLE handle = CreateWaitableTimerExW(nullptr, nullptr, flags, access);
	if (handle != nullptr)
	{
		out_handle = c_platform_handle_factory::get_platform_handle_from_native_handle<HANDLE>(handle);
	}

	return out_handle;
}

bool platform_thread_start_waitable_timer(c_platform_handle& timer_handle, int32 time_milliseconds, int32 period_milliseconds)
{
	HANDLE handle = c_platform_handle_factory::get_native_handle_from_platform_handle<HANDLE>(timer_handle);
	
	// positive indicates absolute time in future, for now we always want a relative time
	const int64 hundred_ns_per_ms = 10000;
	int64 time_nano = -time_milliseconds * hundred_ns_per_ms;

	LARGE_INTEGER due_time = {};
	due_time.QuadPart = time_nano;

	// REVIEW
	const bool resume = false;
	bool result =  SetWaitableTimer(handle, &due_time, period_milliseconds, nullptr, nullptr, resume);
	if (!result)
	{
		DWORD err = GetLastError();
		NOP();
	}
	return result;
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