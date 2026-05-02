#ifndef __PLATFORM_HANDLE_WINDOWS_H__
#define __PLATFORM_HANDLE_WINDOWS_H__
#pragma once

#include "platform.h"
#include "platform_handle.h"

IGNORE_WINDOWS_WARNINGS_PUSH
#include <Windows.h> 
IGNORE_WINDOWS_WARNINGS_POP

template<typename t_type>
uint64 platform_handle_get_cookie_from_native_handle()
{
}

template<>
inline uint64 platform_handle_get_cookie_from_native_handle<HANDLE>()
{
	return 0;
}

template<>
inline uint64 platform_handle_get_cookie_from_native_handle<HMODULE>()
{
	return 1;
}

// make template type
class c_platform_handle_factory
{
public:
	template<typename t_type>
	inline static t_type get_native_handle_from_platform_handle(c_platform_handle& handle)
	{
		return reinterpret_cast<t_type>(handle.m_handle);
	}

	template<typename t_type>
	inline static c_platform_handle get_platform_handle_from_native_handle(t_type handle)
	{
		return c_platform_handle(handle, platform_handle_get_cookie_from_native_handle<t_type>());
	}
};

#endif //__PLATFORM_HANDLE_WINDOWS_H__
