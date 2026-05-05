#include "platform/platform.h"
#ifdef PLATFORM_WIN64

#include "structures/string/string.h"
#include "platform/win64/win64_includes.h"

void platform_nop()
{
	__nop();
}

void platform_log_to_console(const c_string& message)
{
	OutputDebugString(message.get_const_char());
}
#endif // PLATFORM_WINDOWS