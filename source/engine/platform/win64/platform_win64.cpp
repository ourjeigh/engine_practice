#include "platform/platform.h"

#ifdef PLATFORM_WINDOWS
#include "structures/string/string.h"

IGNORE_WINDOWS_WARNINGS_PUSH
#include "windows.h"
#include "intrin.h"
#include "debugapi.h"
IGNORE_WINDOWS_WARNINGS_PUSH

void platform_nop()
{
	__nop();
}

void platform_log_to_console(const c_string& message)
{
	OutputDebugString(message.get_const_char());
}

#endif // PLATFORM_WINDOWS