#include "engine_system.h"
#include "audio/audio_system.h"
#include "input/input_system.h"
#include "debug/logging.h"
#include "perf/perf_system.h"
#include "rendering/render_system.h"
#include "engine.h"

// TODO: make dev only
static_global c_logging_system g_logging_system;
static_global c_perf_system g_perf_system;

static_global c_input_system g_input_system;
static_global c_render_system g_render_system;
static_global c_audio_system g_audio_system;

static_global c_engine g_engine;

void engine_systems_init()
{
	// TODO: temp, move
	s_log_config log_settings;
	log_settings.log_to_console = true;
	log_settings.log_to_file = true;

	// init logging first so it's available for all other systems to use
	g_logging_system.init(log_settings);
	g_perf_system.init();

	g_input_system.init();
	g_render_system.init();
	g_audio_system.init();

	// TEMP
	g_engine_ptr = &g_engine;
}

void engine_systems_term()
{
	log_message(verbose, "Begin Engine System Term");
	g_audio_system.term();
	g_render_system.term();
	g_input_system.term();
	log_message(verbose, "Engine System Term Complete");
	
	g_perf_system.term();

	// term logging last so it's available for all other systems to use
	g_logging_system.term();
}

void engine_systems_pregame_update()
{
	// anything that will be needed for the game to update
	g_input_system.update();
}

void engine_systems_postgame_update()
{
	// anything needed to respond to the game's update
	g_audio_system.update();
	g_render_system.update();
	
	g_perf_system.update();
	g_logging_system.update();
}
