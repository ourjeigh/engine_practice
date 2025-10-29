#include "engine.h"
#include "logging/logging.h"
#include <types/types.h>
#include <asserts.h>
#include "engine_system.h"
#include "time.h"
#include <time/time.h>

static bool init_internal();
static void term_internal();
static void loop_begin_internal();

// maybe when we have something to show we'll speed up
const real64 k_target_fps = 3.0f;
const real64 k_target_frame_interval_seconds = 1 / k_target_fps;

bool g_interrupt_signalled = false;

void c_engine::init()
{
	if (init_internal())
	{
		loop_begin_internal();
	}
	else
	{
		log(critical, "Engine failed to initialize!");
		term_internal();
	}
}

void c_engine::term()
{
	g_interrupt_signalled = true;
	term_internal();
}

static bool init_internal()
{
	s_log_config log_settings;
	log_settings.log_to_console = true;
	log_system_init(log_settings);
	int32 time = 7;
	log(verbose, "hellow world %i", time);

	engine_systems_init();

	return true;
}

static void term_internal()
{
	engine_systems_term();
}

static void loop_begin_internal()
{
	while (!g_interrupt_signalled)
	{
		c_time_span time_span;
		time_span.start();
		engine_systems_update();
		time_span.stop();

		real64 span_micros = time_span.get_duration_microseconds();
		real64 span_seconds = time_span.get_duration_seconds();
		real32 sleep_time = (k_target_frame_interval_seconds - span_seconds);
		log(verbose, "Frame Time: %.2f microseconds. Sleeping for %.6f seconds", span_micros, sleep_time);
		
		sleep_for_seconds(sleep_time);
	}

	term_internal();
}