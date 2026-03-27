#include "engine_system.h"
#include "audio/audio_system.h"
#include "input/input_system.h"
#include <debug/logging.h>
#include <rendering/render_system.h>

static_global c_logging_system g_logging_system;
static_global c_input_system g_input_system;
static_global c_render_system g_render_system;
static_global c_audio_system g_audio_system;

void engine_systems_init()
{
	// TODO: temp, move
	s_log_config log_settings;
	log_settings.log_to_console = true;
	log_settings.log_to_file = true;

	// init logging first so it's available for all other systems to use
	g_logging_system.init(log_settings);
	
	g_input_system.init();
	g_render_system.init();
	g_audio_system.init();
}

void engine_systems_term()
{
	log_message(verbose, "Begin Engine System Term");
	g_audio_system.term();
	g_render_system.term();
	g_input_system.term();
	log_message(verbose, "Engine System Term Complete");
	
	// term logging last so it's available for all other systems to use
	g_logging_system.term();
}

void engine_systems_update()
{
	// update all engine systems here
	g_input_system.update();
	g_render_system.update();
	g_audio_system.update();
	
	g_logging_system.update();
}
