#ifndef __PLATFORM_THREAD_H__
#define __PLATFORM_THREAD_H__
#pragma once

#include "types/types.h"
#include "threads/threads.h"
#include "platform/platform_handle.h"

const uint32 k_wait_time_infinite = k_invalid;

enum e_signalled_object_result
{
	signalled_object_result_signalled,
	signalled_object_result_abandoned,
	signalled_object_result_timed_out,
	signalled_object_result_failed,

	k_signalled_object_result_count
};

uint32 platform_thread_create(s_thread_properties& properties);
void platform_thread_join(uint32 thread_id);
uint32 platform_thread_get_current_thread_id();
void platform_thread_sleep_for_milliseconds(uint32 milliseconds);

c_platform_handle platform_thread_create_event(bool manual_reset, bool start_signalled, c_string name);
bool platform_thread_signal_event(c_platform_handle& event_handle);
e_signalled_object_result platform_thread_wait_for_signalled_object(c_platform_handle& object, uint32 timeout_ms = k_wait_time_infinite);
#endif // !__PLATFORM_THREAD_H__