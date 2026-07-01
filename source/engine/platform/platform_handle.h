#ifndef __PLATFORM_HANDLE_H__
#define __PLATFORM_HANDLE_H__
#pragma once

#include "types/types.h"

class c_platform_handle_factory;

class c_platform_handle
{
public:
	c_platform_handle() { invalidate(); }
	~c_platform_handle() { }

	c_platform_handle(c_platform_handle&& other)
	{
		m_handle = other.m_handle;
		other.invalidate();
	}

	c_platform_handle& operator=(c_platform_handle&& other)
	{
		if (this != &other)
		{
			m_handle = other.m_handle;
			other.invalidate();
		}

		return *this;
	}

	bool is_valid() const;
	void invalidate() { m_handle = k_invalid; }
	static_member_function c_platform_handle invalid()
	{
		c_platform_handle out;
		out.invalidate();
		return out;
	}

private:
	friend class c_platform_handle_factory;

	c_platform_handle(void* handle) : m_handle(reinterpret_cast<uint64>(handle)) {}

	// no copying
	c_platform_handle(const c_platform_handle&) = delete;
	c_platform_handle& operator=(const c_platform_handle&) = delete;

	uint64 m_handle;
};

#endif //__PLATFORM_HANDLE_H__
