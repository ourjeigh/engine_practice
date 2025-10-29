#include "asserts.h"
#include <stdlib.h>
#include <logging/logging.h>
#include <windows.h>

void assert_internal(const char* condition, const char* file, const long line)
{
	log(critical, "Assertion Failed : %s [%s:%i]", condition, file, line);

	if (IsDebuggerPresent())
	{
		DebugBreak();
	}
	else
	{
		abort();
	}
}

void halt(const char* message, const char* file, const long line)
{
	log(critical, "Halt: %s [%s:%i]", message, file, line);

	if (IsDebuggerPresent())
	{
		DebugBreak();
	}
	else
	{
		abort();
	}
}