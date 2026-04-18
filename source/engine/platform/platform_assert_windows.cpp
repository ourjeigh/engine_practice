#include "platform/platform.h"

#ifdef PLATFORM_WINDOWS
#include <debug/logging.h>

IGNORE_WINDOWS_WARNINGS_PUSH
#include <windows.h>
IGNORE_WINDOWS_WARNINGS_POP

void assert_internal(const char* condition, const char* file, const long line)
{
	log_message(critical, "{s} [{s}:{i}]", condition, file, line);
	//log_message(critical, "Assertion Failed: {s} [{s}:{i}]", condition, file, line);

	if (IsDebuggerPresent())
	{
		DebugBreak();
	}
	else
	{
		abort();
	}
}

void halt_internal(const char* message, const char* file, const long line)
{
	log_message(critical, "Halt: {s} [{s}:{i}]", message, file, line);

	if (IsDebuggerPresent())
	{
		DebugBreak();
	}
	else
	{
		abort();
	}
}

#endif // PLATFORM_WINDOWS