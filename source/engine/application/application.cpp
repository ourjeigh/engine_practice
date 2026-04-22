#include "application.h"
#include "window.h"
#include <events/delegates.h>
#include <time/time.h>
#include <engine/engine_system.h>
#include <debug/logging.h>
#include <engine/input/input_system.h>
#include "memory/allocator.h"
#include "platform/platform_assert.h"

const real32 k_max_fps = 60.0f;
const real32 k_max_frame_interval_seconds = 1 / k_max_fps;
const real32 k_max_frame_interval_ms = 1000 / k_max_fps;

const uint32 k_global_memory_bytes = k_byte_mb;

const int32 k_default_window_width = 1440;
const int32 k_default_window_height = 720;

bool g_interrupt_signalled = false;
c_static_stack_allocator<k_global_memory_bytes> g_global_stack_allocator;

#include "windows.h"
#include "libloaderapi.h"
struct s_game_info
{
	c_game_state game_state;
	f_game_init init;
	f_game_update update;
	uint64 HACK_dll_write_time = 0;
	HMODULE library;
};

// todo: create a unit test that greps c++ files and fails on "HACK" :P
s_game_info HACK_g_game_info;

bool HACK_check_dll();
void load_game();
void unload_game();

void c_application::init()
{
	// not sure if there's a better spot for this but we want it set very early so anything can assert safely
	g_assert_handler = assert_internal;

	m_running = false;

	engine_systems_init();

	m_window.init(k_default_window_width, k_default_window_height);

	// TODO: get the MAKE_DELEGATE macro working for this pointers
	m_window.set_event_handler(
		c_delegate<t_event_callback>::bind<c_application, &c_application::handle_window_event>(this));

	input_system_add_key_combo_callback(
		c_delegate<t_key_combo_callback>::bind<c_application, &c_application::handle_escape_key>(this),
		input_key_special_esc);

	zero_object(HACK_g_game_info);
	load_game();
	HACK_g_game_info.game_state.engine = g_engine_ptr;
	HACK_g_game_info.game_state.assert_hook = g_assert_handler;

	(HACK_g_game_info.init)(HACK_g_game_info.game_state);
}

void c_application::term()
{
	engine_systems_term();
	m_window.term();
}

void c_application::run()
{
	m_running = true;
	
	c_loop_timer loop_timer;
	loop_timer.start();

	while (m_running)
	{
		c_timer timer;
		timer.start();
			
		engine_systems_pregame_update();
		HACK_g_game_info.update();
		engine_systems_postgame_update();

		if (HACK_check_dll())
		{
			HACK_g_game_info.init(HACK_g_game_info.game_state);
		}

		timer.stop();
		real64 frame_work_time_ms = timer.get_time_span()->get_duration_milliseconds();
		real64 sleep_time_ms = (k_max_frame_interval_ms - frame_work_time_ms);

		if (sleep_time_ms > 0.0f)
		{
			sleep_for_milliseconds(real64_to_uint32(sleep_time_ms));
		}
		else
		{
			log_message(warning, "Long Frame Time: {f.2} milliseconds", frame_work_time_ms);
		}

		// TODO: track loop time somewhere
		c_time_span span = loop_timer.get_loop_time_span_and_continue();
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



// todo: should we have the game dll expose a function to get it's dll name instead of hardcoding "game" ?
const c_file_path built_dll_path("game_" CONFIG_NAME "_" PLATFORM_NAME ".dll");
const c_file_path runtime_dll_path("game_" CONFIG_NAME "_" PLATFORM_NAME "_temp.dll");

void load_game()
{
	ASSERT(file_exists(built_dll_path));
	ASSERT(file_copy(built_dll_path, runtime_dll_path, true));
	ASSERT(file_exists(runtime_dll_path));

	HMODULE game_dll = nullptr;
	game_dll = LoadLibraryA(runtime_dll_path.get_full_path());
	ASSERT(game_dll != nullptr);

	HACK_g_game_info.library = game_dll;
	HACK_g_game_info.init = reinterpret_cast<f_game_init>(GetProcAddress(game_dll, "game_init"));
	HACK_g_game_info.update = reinterpret_cast<f_game_update>(GetProcAddress(game_dll, "game_update"));
	HACK_g_game_info.HACK_dll_write_time = get_file_info(built_dll_path).write_time;
}

void unload_game()
{
	log_message(verbose, "detected dll change!");

	HMODULE game_dll = GetModuleHandleA(built_dll_path.get_full_path());

	if (game_dll == nullptr)
	{
		game_dll = GetModuleHandleA(runtime_dll_path.get_full_path());
	}

	if (game_dll != nullptr)
	{
		ASSERT(FreeLibrary(game_dll));
	}
}

bool HACK_check_dll()
{
	bool reloaded = false;

	s_file_info info = get_file_info(built_dll_path);
	ASSERT(info.exists);

	if (info.write_time > HACK_g_game_info.HACK_dll_write_time)
	{
		unload_game();
		load_game();
		reloaded = true;
	}

	return reloaded;
}
