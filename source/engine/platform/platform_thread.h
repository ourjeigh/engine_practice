#ifndef __PLATFORM_THREAD_H__
#define __PLATFORM_THREAD_H__
#pragma once

#include "types/types.h"
#include "threads/threads.h"

uint32 platform_thread_create(s_thread_properties& properties);
void platform_thread_join(uint32 thread_id);
uint32 platform_thread_get_current_thread_id();

#endif // !__PLATFORM_THREAD_H__

