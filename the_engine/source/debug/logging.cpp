#include "logging.h"

#include <types/types.h>
#include <structures/string.h>
#include "asserts.h"
#include <time/time.h>
#include "threads/threads.h"
#include "platform/platform.h"

IGNORE_WINDOWS_WARNINGS_PUSH
#include "windows.h"
#include <cstdarg>
#include <debugapi.h>
IGNORE_WINDOWS_WARNINGS_POP

// characters in a log string
const int32 k_max_log_string_length = 512;

// log events in stack
const int32 k_max_log_count = 1024;

struct s_log_event
{
	t_timestamp timestamp;
	// todo: make this date/time
	real64 time_seconds;
	uint32 thread_id;
	e_log_level level;
	c_static_string<k_max_log_string_length> message;
};

// todo: this needs to be thread safe
c_static_stack<s_log_event, k_max_log_count> g_log_stack;

const char* get_log_level_string(e_log_level level);

void c_logging_system::init(s_log_config config)
{
	m_config = config;

	// temp
	c_file_path path("C:\\Users\\RJ\\Desktop\\test\\test_log_file.txt");
	t_file_open_mode_flags flags;
	flags.set(file_open_mode_write, true);
	flags.set(file_open_mode_replace, true);
	m_file.open(path, flags);

	t_string_128 header = "Timestamp\tTime\tThread\tLevel\tMessage\n";
	header.pop();
	m_file.write_string(k_invalid, header.make_array());
}

void c_logging_system::term()
{
	// catch any shutdown logging
	process_log_events();
	m_file.close();
}

void c_logging_system::update()
{
	process_log_events();
}

void c_logging_system::log(e_log_level level, const char* format, ...)  
{  
	// todo: get thread number & name  
	t_timestamp current_time = get_high_precision_timestamp();  
	c_time_span time_since_start = get_session_time()->time_since_start(current_time);  

	c_static_string<k_max_log_string_length> output;
	c_static_string<k_max_log_string_length> message;
	message.clear();

	output.printf("%llu %2.3f %s: ", current_time, time_since_start.get_duration_seconds(), get_log_level_string(level));  

	va_list args;  
	va_start(args, format);  
	message.append_va(format, args);  
	va_end(args);  

	output.append(message.get_const_char());
	output.append("\n");

	if (m_config.log_to_console)  
	{  
		OutputDebugString(output.get_const_char());  
	}  

	if (m_config.log_to_file)
	{  
		s_log_event& new_event = g_log_stack.push();
		new_event.timestamp = current_time;
		new_event.time_seconds = time_since_start.get_duration_seconds();
		new_event.thread_id = get_current_thread_id();
		new_event.level = level;
		new_event.message.copy_from(message);
	}
}

void c_logging_system::process_log_events()
{
	if (m_file.is_open())
	{
		for (auto it = g_log_stack.begin(); it != g_log_stack.end(); ++it)
		{
			const s_log_event& evt = *it;
			c_static_string<k_max_log_string_length> output;
			output.printf("%llu\t%06.3f\t0x%x\t%s\t%s\n",
				evt.timestamp,
				evt.time_seconds,
				evt.thread_id,
				get_log_level_string(evt.level),
				evt.message.get_const_char());

			// hack, should c_string.make_reference remove the trailing null term?
			// we don't want to write it to file...
			output.pop(); 
			m_file.write_string(k_invalid, output.make_array());
		}

		g_log_stack.clear();
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
