#include "platform/platform.h"
#ifdef PLATFORM_WIN64

#include "debug/logging.h"
#include "platform/win64/win64_includes.h"


void assert_internal(const char* condition, const char* file, const long line)
{
	log_message(critical, "{s} [{s}:{i}]", condition, file, line);
	
	// todo: make this a "flush"
	c_logging_system::get().update();
	
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