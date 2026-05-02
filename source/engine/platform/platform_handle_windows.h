#ifndef __PLATFORM_HANDLE_WINDOWS_H__
#define __PLATFORM_HANDLE_WINDOWS_H__
#pragma once

#include "platform.h"
#include "platform_handle.h"

IGNORE_WINDOWS_WARNINGS_PUSH
#include <Windows.h> 
IGNORE_WINDOWS_WARNINGS_POP

// make template type
class c_platform_handle_factory
{
public:
	inline static HANDLE get_native_handle_from_platform_handle(c_platform_handle& handle)
	{
		return reinterpret_cast<HANDLE>(handle.m_handle);
	}

	inline static c_platform_handle get_platform_handle_from_native_handle(HANDLE handle)
	{
		return c_platform_handle(handle);
	}
};

#endif //__PLATFORM_HANDLE_WINDOWS_H__
