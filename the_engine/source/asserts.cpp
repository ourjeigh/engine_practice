#include "asserts.h"
#include <debug/logging.h>
#include "platform/platform.h"

IGNORE_WINDOWS_WARNINGS_PUSH
#include <windows.h>
IGNORE_WINDOWS_WARNINGS_POP

void assert_internal(const char* condition, const char* file, const long line)
{
	log_message(critical, "Assertion Failed: %s [%s:%i]", condition, file, line);

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
	log_message(critical, "Halt: %s [%s:%i]", message, file, line);

	if (IsDebuggerPresent())
	{
		DebugBreak();
	}
	else
	{
		abort();
	}
}