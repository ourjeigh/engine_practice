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

	void close();

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

#endif //__PLATFORM_HANDLE_H__
