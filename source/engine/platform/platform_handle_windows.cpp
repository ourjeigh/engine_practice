#include "debug/asserts.h"
#include "platform_handle.h"
#include "platform_handle_windows.h"

bool c_platform_handle::is_valid() const
{
	return m_handle != k_invalid && reinterpret_cast<HANDLE>(m_handle) != INVALID_HANDLE_VALUE;
}