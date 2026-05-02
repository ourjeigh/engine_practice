#include "application.h"
#include "window.h"
#include <events/delegates.h>
#include <time/time.h>
#include <engine/engine_system.h>
#include <debug/logging.h>
#include <engine/input/input_system.h>
#include "memory/allocator.h"
#include "platform/platform_assert.h"
#include "platform/platform_process.h"

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
	HMODULE DELETE_library;
	c_platform_handle library;
};

// todo: create a unit test that greps c++ files and fails on "HACK" :P
s_game_info HACK_g_game_info;

bool HACK_check_dll();
void load_game();
void unload_game();

static void handle_game_reload(bool down);

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

	input_system_add_key_combo_callback(
		MAKE_DELEGATE_STATIC(t_key_combo_callback, handle_game_reload),
		input_key_special_control, input_key_special_shift, input_key_char_r);

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
	HACK_g_game_info.library.invalidate();
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
c_file_path g_runtime_dll_path("game_" CONFIG_NAME "_" PLATFORM_NAME "_temp.dll");

const char* runtime_dll_name_base = "game_" CONFIG_NAME "_" PLATFORM_NAME "_temp_";

int32 g_dll_count = 0;

void load_game()
{
	bool success = false;
	for (int32 retries = 10; !success && retries >= 0; retries--)
	{
		if (file_exists(built_dll_path))
		{
			t_string_128 dll_name = runtime_dll_name_base;
			dll_name.appendf("{i}.dll", g_dll_count);

			g_runtime_dll_path = c_file_path(dll_name.get_const_char());
			if (file_copy(built_dll_path, g_runtime_dll_path, true))
			{
				if (file_exists(g_runtime_dll_path))
				{
					HACK_g_game_info.library = platform_process_load_library(g_runtime_dll_path);
					ASSERT(HACK_g_game_info.library.is_valid());

					t_string_128 game_init_function_name("game_init");
					t_string_128 game_update_function_name("game_update");

					HACK_g_game_info.init = reinterpret_cast<f_game_init>(platform_process_get_library_function_address(
						HACK_g_game_info.library,
						game_init_function_name));

					HACK_g_game_info.update = reinterpret_cast<f_game_update>(platform_process_get_library_function_address(
						HACK_g_game_info.library,
						game_update_function_name));

					ASSERT(HACK_g_game_info.init != nullptr);
					ASSERT(HACK_g_game_info.update != nullptr);
					/*HMODULE game_dll = LoadLibraryA(g_runtime_dll_path.get_full_path());
					ASSERT(game_dll != nullptr);

					HACK_g_game_info.DELETE_library = game_dll;
					HACK_g_game_info.init = reinterpret_cast<f_game_init>(GetProcAddress(game_dll, "game_init"));
					HACK_g_game_info.update = reinterpret_cast<f_game_update>(GetProcAddress(game_dll, "game_update"));*/
					HACK_g_game_info.HACK_dll_write_time = get_file_info(built_dll_path).write_time;
					
					success = true;
					g_dll_count++;
					
					log_message(verbose, "Game Dll Loaded: {s}", g_runtime_dll_path.get_full_path());
				}
				else
				{
					log_message(warning, "Couldn't find runtime dll: {s}", g_runtime_dll_path.get_full_path());
				}
			}
			else
			{
				// remove once we're more confident in copy
				DWORD error = GetLastError();
				LPSTR messageBuffer = nullptr;;

				size_t size = FormatMessageA(
					FORMAT_MESSAGE_ALLOCATE_BUFFER | // Let the system allocate memory
					FORMAT_MESSAGE_FROM_SYSTEM |    // Search system message tables
					FORMAT_MESSAGE_IGNORE_INSERTS,  // Ignore placeholder inserts
					NULL,
					error,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Use default language
					(LPSTR)&messageBuffer,
					0,
					NULL
				);

				log_message(critical, "Copy Error: {s}", messageBuffer);
				log_message(warning, "Failed to copy dll src: {s} dest: {s}", built_dll_path.get_full_path(), g_runtime_dll_path.get_full_path());
			}
		}
		else
		{
			log_message(warning, "Couldn't find built dll: {s}", built_dll_path.get_full_path());
		}
	}

	ASSERT(success);
}

void unload_game()
{
	if (HACK_g_game_info.DELETE_library != nullptr)
	{
		ASSERT(FreeLibrary(HACK_g_game_info.DELETE_library));
		HACK_g_game_info.DELETE_library = nullptr;
	}
}

bool HACK_check_dll()
{
	bool reloaded = false;

	s_file_info info = get_file_info(built_dll_path);
	
	if (info.exists) 
	{
		if (info.write_time > HACK_g_game_info.HACK_dll_write_time)
		{
			log_message(verbose, "detected dll change!");

			// replace this with a wait until the file becomes readable
			sleep_for_milliseconds(100);
			unload_game();
			load_game();
			reloaded = true;
		}
	}
	else
	{
		log_message(critical, "Couldn't find built dll! {s}", built_dll_path.get_full_path());
	}

	return reloaded;
}

bool reload_handled = false;

static void handle_game_reload(bool down)
{
	if (down && !reload_handled)
	{
		log_message(verbose, "Handling Reload Command");

		c_file_path cmd_path("C:\\Windows\\System32\\cmd.exe");
		t_string_128 command("/c cmake --build ../project --target game --config Debug");

		if (platform_process_start_process_and_wait(cmd_path, command))
		{
			reload_handled = true;

			log_message(verbose, "Reload build succeeded, triggering DLL update");
		}
		else
		{
			log_message(error, "Failed to launch build command");
		}
	}
	
	if (!down)
	{
		reload_handled = false;
	}
}
