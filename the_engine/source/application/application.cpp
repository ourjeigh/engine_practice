#include "application.h"
#include <engine/engine.h>
#include "window.h"
#include <events/delegates.h>
#include <time/time.h>
#include <engine/engine_system.h>
#include <debug/logging.h>
#include <engine/input/input_system.h>


const real32 k_max_fps = 60.0f;
const real32 k_max_frame_interval_seconds = 1 / k_max_fps;

const uint32 k_global_memory_bytes = k_byte_mb;

bool g_interrupt_signalled = false;
c_static_stack_allocator<k_global_memory_bytes> g_global_stack_allocator;

void c_application::init()
{
	m_running = false;

	m_window.init();

	// TODO: get the MAKE_DELEGATE macro working for this pointers
	m_window.set_event_handler(
		c_delegate<t_event_callback>::bind<c_application, &c_application::handle_window_event>(this));

	engine_systems_init();

	input_system_add_key_combo_callback(
		c_delegate<t_key_combo_callback>::bind<c_application, &c_application::handle_escape_key>(this),
		input_key_special_esc);
}

void c_application::term()
{
	engine_systems_term();
	m_window.term();
}

void c_application::run()
{
	m_running = true;
	
	while (m_running)
	{
		c_timer timer;
		timer.start();
			
		engine_systems_update();
		//m_window.update(); todo

		timer.stop();

		// clean this up, don't use both
		real64 span_millis = timer.get_time_span()->get_duration_milliseconds();
		real64 span_seconds = timer.get_time_span()->get_duration_seconds();

		real32 sleep_time = (k_max_frame_interval_seconds - static_cast<real32>(span_seconds));
		if (sleep_time > 0.0f)
		{
			sleep_for_seconds(sleep_time);
		}
		else
		{
			log_message(warning, "Long Frame Time: %.2f milliseconds", span_millis);
		}
	}
}

void c_application::handle_window_event(s_event& event)
{
	if (event.get_category() == window_event_type_window)
	{
		switch (event.get_type())
		{
		case event_type_window_close:
			handle_window_close();
			break;
		case event_type_window_resize:
		{
			s_window_event_resize& resize_event = static_cast<s_window_event_resize&>(event);
			handle_window_resize(resize_event.height, resize_event.width);
			break;
		}
		case event_type_window_focus:
		{
			s_window_event_focus& focus_event = static_cast<s_window_event_focus&>(event);
			handle_window_focus(focus_event.is_in_focus);
			break;
		}
		default:
			HALT_UNIMPLEMENTED();
		}
	}
	else if (event.get_category() == window_event_type_input)
	{
		//input_system_queue_message()
		input_system_handle_event(event);
	}
	else
	{
		HALT_UNIMPLEMENTED();
	}
}

void c_application::handle_escape_key(bool down)
{
	if (down)
	{
		log_message(verbose, "c_application: escape pressed, begin shutdown");
		m_running = false;
	}
}

void c_application::handle_window_close()
{
	log_message(verbose, "c_application: window close called, begin shutdown");
	m_running = false;

	// tbd if we want to explicitly call term here...
}

void c_application::handle_window_focus(bool is_in_focus)
{
	NOP();
}

void c_application::handle_window_resize(int32 height, int32 width)
{
	NOP();
}