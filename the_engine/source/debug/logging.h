#ifndef __LOGGING_H__
#define __LOGGING_H__
#pragma once

#include "config.h"
#include "engine/engine_system.h"

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
	virtual void term() override;
	virtual void update() override;

	void log(e_log_level level, const char* text, ...);

private:
	virtual void init() override {};

	s_log_config m_config;
};

#ifdef FEATURE_LOGGING
#define log_message(level, text, ...) c_logging_system::get().log(level, text __VA_OPT__(,) __VA_ARGS__)
#else
#define log_message(leve, text, ...)
#endif

#endif //__LOGGING_H__
