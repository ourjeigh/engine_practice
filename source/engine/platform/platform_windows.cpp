#include "platform.h"

#ifdef PLATFORM_WINDOWS

IGNORE_WINDOWS_WARNINGS_PUSH
#include <intrin.h>
IGNORE_WINDOWS_WARNINGS_PUSH

void platform_nop()
{
	__nop();
}

#endif // PLATFORM_WINDOWS