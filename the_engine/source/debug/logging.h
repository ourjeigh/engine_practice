#ifndef __LOGGING_H__
#define __LOGGING_H__
#pragma once

#include "config.h"
#include "engine/engine_system.h"
#include <engine/file_system/file.h>

const int32 k_log_file_buffer_size = 10 * k_byte_kib;

struct s_log_config
{
	bool log_to_file;
	bool log_to_console;
	bool log_to_screen;
};

enum e_log_level
{
	verbose,
	warning,
	error,
	critical
};

class c_logging_system : public c_engine_system<c_logging_system>
{
public:
	void init(s_log_config config);
	void term() override;
	void update() override;

	void log(e_log_level level, const char* text, ...);

private:
	void init() override {};
	void process_log_events();

	s_log_config m_config;
	c_file_static_buffered<k_log_file_buffer_size> m_file;
};

#ifdef FEATURE_LOGGING
#define log_message(level, text, ...) if (c_logging_system::is_valid()) c_logging_system::get().log(level, text __VA_OPT__(,) __VA_ARGS__)
#else
#define log_message(leve, text, ...)
#endif

#endif //__LOGGING_H__
