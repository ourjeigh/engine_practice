#include "engine.h"
#include "logging/logging.h"
#include <types/types.h>
#include <asserts.h>

void c_engine::init()
{
	s_log_config log_settings;
	log_settings.log_to_console = true;
	log_system_init(log_settings);
	int32 time = 7;
	log(verbose, "hellow world %i", time);

	bool is_poopy = false;
	ASSERT(is_poopy);
}

void c_engine::term()
{

}