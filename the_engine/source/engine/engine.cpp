#include "engine.h"
#include "logging/logging.h"
#include <types/types.h>
#include <asserts.h>
#include "engine_system.h"
#include "time.h"
#include <time/time.h>
#include <threads/threads.h>

static bool init_internal();
static void term_internal();
static void update_internal();
static void loop_begin_internal();

// maybe when we have something to show we'll speed up
const real64 k_max_fps = 30.0f;
const real64 k_max_frame_interval_seconds = 1 / k_max_fps;

bool g_interrupt_signalled = false;
c_engine g_engine;

void engine_init()
{
	g_engine.init();
}

void engine_term()
{
	g_interrupt_signalled = true;
}

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
		c_timer timer;
		timer.start();
		engine_systems_update();
		timer.stop();

		real64 span_millis = timer.get_time_span()->get_duration_milliseconds();
		real64 span_seconds = timer.get_time_span()->get_duration_seconds();

		real64 sleep_time = (k_max_frame_interval_seconds - span_seconds);
		if (sleep_time > 0.0f)
		{
			//log(verbose, "Frame Time: %.2f microseconds. Sleeping for %.6f seconds", span_micros, sleep_time);
			sleep_for_seconds(sleep_time);
		}
		else
		{
			log(warning, "Long Frame Time: %.2f milliseconds", span_millis);
		}
	}

	term_internal();
}

static void update_internal()
{
	engine_systems_update();
}