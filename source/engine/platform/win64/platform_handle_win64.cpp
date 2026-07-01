#include "platform/platform.h"
#ifdef PLATFORM_WIN64

#include "platform/win64/platform_handle_win64.h"
#include "debug/asserts.h"

bool c_platform_handle::is_valid() const
{
	return m_handle != k_invalid && reinterpret_cast<HANDLE>(m_handle) != INVALID_HANDLE_VALUE;
}

void c_platform_handle::close()
{
	CloseHandle(c_platform_handle_factory::get_native_handle_from_platform_handle<HANDLE>(*this));
}
#endif // PLATFORM_WIN64