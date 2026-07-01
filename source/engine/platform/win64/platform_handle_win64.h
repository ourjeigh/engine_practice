#ifndef __PLATFORM_HANDLE_WINDOWS_H__
#define __PLATFORM_HANDLE_WINDOWS_H__
#pragma once

#include "platform/platform.h"
#ifdef PLATFORM_WIN64

#include "platform/platform_handle.h"
#include "platform/win64/win64_includes.h"


// make template type
class c_platform_handle_factory
{
public:
	template<typename t_type>
	inline static_member_function t_type get_native_handle_from_platform_handle(c_platform_handle& handle)
	{
		return reinterpret_cast<t_type>(handle.m_handle);
	}

	template<typename t_type>
	inline static_member_function c_platform_handle get_platform_handle_from_native_handle(t_type handle)
	{
		return c_platform_handle(handle);
	}
};
#endif // PLATFORM_WIN64
#endif //__PLATFORM_HANDLE_WINDOWS_H__
