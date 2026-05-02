#ifndef __PLATFORM_HANDLE_H__
#define __PLATFORM_HANDLE_H__
#pragma once

#include <types/types.h>

class c_platform_handle_factory;

class c_platform_handle
{
public:
	c_platform_handle() { invalidate(); }
	~c_platform_handle();

	c_platform_handle(c_platform_handle&& other)
	{
		m_handle = other.m_handle;
		m_cookie = other.m_cookie;
		other.invalidate();
	}

	c_platform_handle& operator=(c_platform_handle&& other)
	{
		if (this != &other)
		{
			m_handle = other.m_handle;
			m_cookie = other.m_cookie;
			other.invalidate();
		}

		return *this;
	}

	bool is_valid() const;
	void invalidate() { m_handle = k_invalid; }

private:
	friend class c_platform_handle_factory;

	c_platform_handle(void* handle, uint64 cookie) : m_handle(reinterpret_cast<uint64>(handle)), m_cookie(cookie) {}

	// no copying
	c_platform_handle(const c_platform_handle&) = delete;
	c_platform_handle& operator=(const c_platform_handle&) = delete;

	uint64 m_handle;
	uint64 m_cookie;
};

#endif //__PLATFORM_HANDLE_H__
