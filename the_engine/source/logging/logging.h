#ifndef __LOGGING_H__
#define __LOGGING_H__
#pragma once

struct s_log_config
{
	bool log_to_file;
	bool log_to_console;
};

enum e_log_level
{
	verbose,
	warning,
	error,
	critical
};

void log_system_init(s_log_config config);
void log_system_term();
void log(e_log_level level, const char *text, ...);

#endif //__LOGGING_H__
