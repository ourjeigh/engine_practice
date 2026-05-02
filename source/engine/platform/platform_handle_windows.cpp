#include "debug/asserts.h"
#include "platform_handle.h"
#include "platform_handle_windows.h"

c_platform_handle::~c_platform_handle()
{
	if (is_valid())
	{
		switch (m_cookie)
		{
		case 0:
			CloseHandle(reinterpret_cast<HANDLE>(m_handle));
			break;
		case 1:
			CloseHandle(reinterpret_cast<HMODULE>(m_handle));
			break;
		default:
			HALT_UNIMPLEMENTED();
		}
	}
}

bool c_platform_handle::is_valid() const
{
	return m_handle != k_invalid && reinterpret_cast<HANDLE>(m_handle) != INVALID_HANDLE_VALUE;
}