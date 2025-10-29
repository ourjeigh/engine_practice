#include "logging.h"
#include <cstdarg>
#include <iostream>
#include <types/types.h>
#include <structures/string.h>
#include "asserts.h"

const int32 k_max_log_length = 256;

s_log_config g_log_config;

const char* get_log_level_string(e_log_level level);

void log_system_init(s_log_config config)
{
	g_log_config = config;
}

void log_system_term()
{

}

void log(e_log_level level, const char* format, ...)
{
	c_string<k_max_log_length> output;

	output.print("%s:", get_log_level_string(level));

	va_list args;
	va_start(args, format);

	output.append_va(format, args);
	va_end(args);

	if (g_log_config.log_to_console)
	{
		puts(output.get_const_char());
	}

	if (g_log_config.log_to_file)
	{
		// todo
	}
}

const char* get_log_level_string(e_log_level level)
{
	switch (level)
	{	
	case verbose:
		return "verbose";
		break;
	case warning:
		return "warning";
		break;
	case error:
		return "error";
		break;
	case critical:
		return "critical";
		break;
	default:
		HALT("Unknown log level");
		break;
	}
	return "unknown";
}
