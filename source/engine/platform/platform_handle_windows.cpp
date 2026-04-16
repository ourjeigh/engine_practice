#include "platform_handle.h"
#include "platform_handle_windows.h"

c_platform_handle::~c_platform_handle()
{
	if (is_valid() && reinterpret_cast<HANDLE>(m_handle) != INVALID_HANDLE_VALUE)
	{
		CloseHandle(reinterpret_cast<HANDLE>(m_handle));
	}
}

c_platform_handle::c_platform_handle(c_platform_handle&& other)
{
	m_handle = other.m_handle;
	other.invalidate();
}

c_platform_handle& c_platform_handle::operator=(c_platform_handle&& other)
{
	if (this != &other)
	{
		m_handle = other.m_handle;
		other.invalidate();
	}

	return *this;
}

bool c_platform_handle::is_valid() const
{
	return m_handle != k_invalid && reinterpret_cast<HANDLE>(m_handle) != INVALID_HANDLE_VALUE;
}

void* c_platform_handle::get_native_handle()
{
	return reinterpret_cast<void*>(m_handle);
}

const void* c_platform_handle::get_native_handle_const() const
{
	return reinterpret_cast<const void*>(m_handle);
}
