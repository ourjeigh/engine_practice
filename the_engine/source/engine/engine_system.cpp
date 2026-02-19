#include "engine_system.h"
#include "audio/audio_system.h"
#include "input/input_system.h"
#include <debug/logging.h>

static c_input_system g_input_system;
static c_audio_system g_audio_system;
static c_logging_system g_logging_system;

void engine_systems_init()
{
	s_log_config log_settings;
	log_settings.log_to_console = true;
	log_settings.log_to_file = true;

	// initialize all engine systems here
	g_logging_system.init(log_settings);
	g_input_system.init();
	g_audio_system.init();
}

void engine_systems_term()
{
	log_message(verbose, "Begin Engine System Term");
	// terminate all engine systems here
	g_input_system.term();
	g_audio_system.term();

	log_message(verbose, "Engine System Term Complete");
	g_logging_system.term();
}

void engine_systems_update()
{
	// update all engine systems here
	g_input_system.update();
	g_audio_system.update();
	g_logging_system.update();
}
