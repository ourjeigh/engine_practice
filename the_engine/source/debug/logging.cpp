#include "logging.h"

#include <types/types.h>
#include <structures/string.h>
#include "asserts.h"
#include <time/time.h>

#include "platform/platform.h"

IGNORE_WINDOWS_WARNINGS_PUSH
#include "windows.h"
#include <cstdarg>
#include <debugapi.h>
IGNORE_WINDOWS_WARNINGS_POP

// characters in a log string
const int32 k_max_log_string_length = 256;

// log events in stack
const int32 k_max_log_count = 1024;


c_stack<c_string<k_max_log_string_length>, k_max_log_count> g_log_stack;

const char* get_log_level_string(e_log_level level);

void c_logging_system::init(s_log_config config)
{
	m_config = config;
}

void c_logging_system::term()
{
}

void c_logging_system::update()
{
}

void c_logging_system::log(e_log_level level, const char* format, ...)  
{  
// todo: get thread number & name  
t_timestamp current_time = get_high_precision_timestamp();  
c_time_span time_since_start = get_session_time()->time_since_start(current_time);  

c_string<k_max_log_string_length> output;  

output.printf("%llu %2.3f %s: ", current_time, time_since_start.get_duration_seconds(), get_log_level_string(level));  

va_list args;  
va_start(args, format);  
output.append_va(format, args);  
va_end(args);  

if (m_config.log_to_console)  
{  
	puts(output.get_const_char());  
	OutputDebugString(output.get_const_char());  
	OutputDebugString("\n");  
}  

if (m_config.log_to_file)
{  
	// todo: build a log_event type with all the info split out still so that we can format it better for the file  
	g_log_stack.push(output);  
}  
}

//void log(e_log_level level, const char* format, ...)  
//{  
//   va_list args;  
//   va_start(args, format);  
//   c_logging_system::get().log(level, format, args);  
//   va_end(args);  
//}

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
