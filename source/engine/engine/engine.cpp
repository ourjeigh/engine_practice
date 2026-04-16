//#include "engine.h"
//#include "debug/logging.h"
//#include <types/types.h>
//#include <asserts.h>
//#include "engine_system.h"
//#include "time.h"
//#include <time/time.h>
//#include <threads/threads.h>
//
//static bool init_internal();
//static void term_internal();
//static void update_internal();
//static void main_loop_entry_point();
//
//// maybe when we have something to show we'll speed up
//const real32 k_max_fps = 30.0f;
//const real32 k_max_frame_interval_seconds = 1 / k_max_fps;
//
//const uint32 k_global_memory_bytes = k_byte_mb;
//
//bool g_interrupt_signalled = false;
//c_static_stack_allocator<k_global_memory_bytes> g_global_stack_allocator;
//
////void engine_init()
////{
////	g_engine.init();
////}
////
////void engine_term()
////{
////	g_interrupt_signalled = true;
////}
////
////void engine_run();
//
//c_stack_allocator* get_global_allocator()
//{
//	return &g_global_stack_allocator;
//}
//
//void c_engine::init()
//{
//	if (init_internal())
//	{
//		main_loop_entry_point();
//	}
//	else
//	{
//		log(critical, "Engine failed to initialize!");
//		term_internal();
//	}
//}
//
//void c_engine::term()
//{
//	g_interrupt_signalled = true;
//	term_internal();
//}
//
//void c_engine::update()
//{
//
//}
//
//static bool init_internal()
//{
//	s_log_config log_settings;
//	log_settings.log_to_console = true;
//	log_system_init(log_settings);
//
//	engine_systems_init();
//
//	return true;
//}
//
//static void term_internal()
//{
//	engine_systems_term();
//}
//
//static void main_loop_entry_point()
//{
//	while (!g_interrupt_signalled)
//	{
//		c_timer timer;
//		timer.start();
//		engine_systems_update();
//		timer.stop();
//
//		real64 span_millis = timer.get_time_span()->get_duration_milliseconds();
//		real64 span_seconds = timer.get_time_span()->get_duration_seconds();
//
//		real32 sleep_time = (k_max_frame_interval_seconds - static_cast<real32>(span_seconds));
//		if (sleep_time > 0.0f)
//		{
//			//log(verbose, "Frame Time: %.2f microseconds. Sleeping for %.6f seconds", span_micros, sleep_time);
//			sleep_for_seconds(sleep_time);
//		}
//		else
//		{
//			log(warning, "Long Frame Time: %.2f milliseconds", span_millis);
//		}
//	}
//
//	term_internal();
//}
//
//static void update_internal()
//{
//	engine_systems_update();
//}