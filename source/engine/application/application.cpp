#include "application.h"
#include "debug/logging.h"
#include "engine/engine_system.h"
#include "engine/input/input_system.h"
#include "events/delegates.h"
#include "game_api.h"
#include "memory/allocator.h"
#include "memory/memory_system.h"
#include "platform/platform.h"
#include "platform/platform_assert.h"
#include "platform/platform_process.h"
#include "time/time.h"
#include "platform/platform_window.h"
#include "rendering/render_system.h"
#include "perf/perf.h"
#include "platform/platform_file.h"

const real32 k_max_fps = 60.0f;
const real32 k_max_frame_interval_seconds = 1 / k_max_fps;
const real32 k_max_frame_interval_ms = 1000 / k_max_fps;


const int32 k_default_window_width = 1440;
const int32 k_default_window_height = 720;

// todo: should we have the game dll expose a function to get it's dll name instead of hardcoding "game" ?
const c_file_path k_built_dll_path("game_" CONFIG_NAME "_" PLATFORM_NAME ".dll");
const char* k_runtime_dll_name_base = "game_" CONFIG_NAME "_" PLATFORM_NAME "_temp_";

struct s_game_info_internal
{
	s_game_engine_context game_engine_context;
	f_game_init init;
	f_game_update update;
#ifdef HOT_RELOAD
	f_game_reload reload;
#endif // HOT_RELOAD

	uint64 last_dll_write_time;
	int32 dll_reload_count;
	c_platform_handle library;
};

static_global void load_game();
static_global void unload_game();

#ifdef HOT_RELOAD
static_global bool check_game_dll_and_reload_if_newer();
static_global void handle_game_reload(bool down);
static_global void cleanup_temp_game_dlls();
#endif //HOT_RELOAD

s_game_info_internal g_game_info;
bool g_interrupt_signalled = false;
real32 g_last_dt;

#ifdef CONFIG_DEBUG
void debug_render_framerate_widget(real64 work_duration_ms, real64 frame_duration_ms);
#endif //CONFIG_DEBUG

void c_application::init()
{
	ASSERT(m_instance == nullptr);
	m_instance = this;
	
	// not sure if there's a better spot for this but we want it set very early so anything can assert safely
	g_assert_handler = assert_internal;

	m_running = false;

	engine_systems_init();

	m_window.init(k_default_window_width, k_default_window_height);

	// TODO: get the MAKE_DELEGATE macro working for this pointers
	m_window.set_event_handler(
		c_delegate<t_event_callback>::bind<c_application, &c_application::handle_window_event>(this));

	/*input_system_add_key_combo_callback(
		c_delegate<t_key_combo_callback>::bind<c_application, &c_application::handle_escape_key>(this),
		input_key_special_esc);*/

	input_system_add_key_combo_callback(
		MAKE_DELEGATE_STATIC(t_key_combo_callback, handle_game_reload),
		input_key_special_control, input_key_special_shift, input_key_char_r);

	zero_object(g_game_info);
	load_game();
	g_game_info.game_engine_context.engine = g_engine_ptr;
	g_game_info.game_engine_context.assert_hook = g_assert_handler;
	
	// bad/temp
	int32 game_state_memory_size = 10 * k_byte_mb;
	g_game_info.game_engine_context.memory.data = static_cast<void*>(c_memory_system::get().allocate(game_state_memory_size, alignof(byte), memory_arena_game_state));
	g_game_info.game_engine_context.memory.size = game_state_memory_size;

	g_game_info.init(g_game_info.game_engine_context);
}

void c_application::term()
{
	engine_systems_term();
	m_window.term();
	g_game_info.library.invalidate();
	unload_game();
	cleanup_temp_game_dlls();
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

		{
			PERF_MEASURE_SECTION("application loop");

			engine_systems_pregame_update();
			g_game_info.update(
				input_system_get_current_input_state(),
				g_last_dt);
			engine_systems_postgame_update();
		}

#ifdef HOT_RELOAD
		if (check_game_dll_and_reload_if_newer())
		{
			// todo: make a specific reload function for this so we don't have to worry
			// about actual init-specific logic getting rerun on reload.
			g_game_info.reload(g_game_info.game_engine_context);
		}
#endif //HOT_RELOAD

		timer.stop();
		real64 frame_work_time_ms = timer.get_time_span().get_duration_milliseconds();
		real64 sleep_time_ms = (k_max_frame_interval_ms - frame_work_time_ms);
		const real64 frame_sleep_pad_ms = 2.0f;
		if (sleep_time_ms > frame_sleep_pad_ms)
		{
			// if we sleep the full sleep time, innacuracies in sleep precision can cause us to wake
			// after the max frame interval. 
			thread_sleep_for_milliseconds(real64_to_uint32(sleep_time_ms - frame_sleep_pad_ms));
		}
		else
		{
			log_message(warning, "Long Frame Time: {f.2} milliseconds", frame_work_time_ms);
		}

		c_engine_time_span span = loop_timer.get_loop_time_span_and_continue();
		g_last_dt = span.get_duration_seconds();

#ifdef CONFIG_DEBUG
		debug_render_framerate_widget(frame_work_time_ms, span.get_duration_milliseconds());
#endif //CONFIG_DEBUG
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
	/*if (down)
	{
		log_message(verbose, "c_application: escape pressed, begin shutdown");
		m_running = false;
	}*/
}

void c_application::request_exit()
{
	log_message(verbose, "c_application: exit reqeusted, begin shutdown");
	m_running = false;
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
	c_render_system::get().resize(width, height);
}

static_global void load_game()
{
	bool success = false;
	for (int32 retries = 10; !success && retries >= 0; retries--)
	{
		if (file_exists(k_built_dll_path))
		{
			t_string_128 dll_name = k_runtime_dll_name_base;
			dll_name.appendf("{i}.dll", g_game_info.dll_reload_count);
			c_file_path runtime_dll_path = c_file_path(dll_name.get_const_char());

			if (file_copy(k_built_dll_path, runtime_dll_path, true))
			{
				if (file_exists(runtime_dll_path))
				{
					g_game_info.library = platform_process_load_library(runtime_dll_path);
					ASSERT(g_game_info.library.is_valid());

					t_string_128 game_init_function_name("game_init");
					t_string_128 game_update_function_name("game_update");
					t_string_128 game_reload_function_name("game_reload");

					g_game_info.init = reinterpret_cast<f_game_init>(platform_process_get_library_function_address(
						g_game_info.library,
						game_init_function_name));

					g_game_info.update = reinterpret_cast<f_game_update>(platform_process_get_library_function_address(
						g_game_info.library,
						game_update_function_name));

					g_game_info.reload = reinterpret_cast<f_game_reload>(platform_process_get_library_function_address(
						g_game_info.library,
						game_reload_function_name));

					ASSERT(g_game_info.init != nullptr);
					ASSERT(g_game_info.update != nullptr);
					ASSERT(g_game_info.reload != nullptr);

					g_game_info.last_dll_write_time = get_file_info(k_built_dll_path).write_time;
					
					success = true;
					g_game_info.dll_reload_count++;
					
					log_message(verbose, "Game Dll Loaded: {s}", runtime_dll_path.get_full_path());
				}
				else
				{
					log_message(warning, "Couldn't find runtime dll: {s}", runtime_dll_path.get_full_path());
				}
			}
			else
			{
				log_message(warning, "Failed to copy dll src: {s} dest: {s}", k_built_dll_path.get_full_path(), runtime_dll_path.get_full_path());
			}
		}
		else
		{
			log_message(warning, "Couldn't find built dll: {s}", k_built_dll_path.get_full_path());
		}
	}

	ASSERT(success);
}

static_global void unload_game()
{
	if (g_game_info.library.is_valid())
	{
		ASSERT(platform_process_unload_library(g_game_info.library));
		g_game_info.library.invalidate();
	}
}

#ifdef HOT_RELOAD
bool check_game_dll_and_reload_if_newer()
{
	bool reloaded = false;

	s_file_info info = get_file_info(k_built_dll_path);
	
	if (info.exists) 
	{
		if (info.write_time > g_game_info.last_dll_write_time)
		{
			log_message(verbose, "detected dll change!");

			// replace this with a wait until the file becomes readable
			thread_sleep_for_milliseconds(100);
			unload_game();
			load_game();
			reloaded = true;
		}
	}
	else
	{
		log_message(critical, "Couldn't find built dll! {s}", k_built_dll_path.get_full_path());
	}

	return reloaded;
}

static_global void handle_game_reload(bool down)
{
	if (down)
	{
		log_message(verbose, "Handling Reload Command");

		// i suppose this should probably be moved to windows specific code, but since we may not support hot reloading on
		// other platforms it may not matter.
		c_file_path cmd_path("C:\\Windows\\System32\\cmd.exe");
		t_string_128 command("/c cmake --build ../project --target game --config Debug");

		if (platform_process_start_process_and_wait(cmd_path, command))
		{
			log_message(verbose, "Reload build succeeded, triggering DLL update");
		}
		else
		{
			log_message(error, "Failed to launch build command");
		}
	}
}

static_global void cleanup_temp_game_dlls()
{
	c_file_path directory("C:\\Users\\RJ\\git\\simm_engine\\build");
	c_static_stack<c_file_path, 128> files;
	platform_file_directory_get_files(directory, files);

	const t_string_128 temp_string("temp");

	for (auto& file : files)
	{
		if (file.contains(temp_string))
		{
			platform_file_delete(file);
		}
	}
}
#endif //HOT_RELOAD

#ifdef CONFIG_DEBUG
void debug_render_framerate_widget(real64 work_duration_ms, real64 frame_duration_ms)
{
	t_string_128 string;
	string.printf("W: {f.2}ms| F: {f.2}ms", work_duration_ms, frame_duration_ms);
	render_system_draw_string(string, 5, 5, 1, k_color_white, render_layer_debug);

	const int32 full_frame_width = 170;
	t_render_shape_rect full_frame_rect(5, 15, full_frame_width, 10);
	render_system_draw_rect(full_frame_rect, k_color_white, render_layer_debug);

	const int32 actual_frame_width = frame_duration_ms / k_max_frame_interval_ms * full_frame_width;
	t_render_shape_rect actual_frame_rect(5, 15, actual_frame_width, 10);
	c_color actual_color = frame_duration_ms > k_max_frame_interval_ms ?
		k_color_red :
		k_color_grey_dark;
	render_system_draw_rect(actual_frame_rect, actual_color, render_layer_debug);

	const int32 work_width = work_duration_ms / k_max_frame_interval_ms * full_frame_width;
	t_render_shape_rect work_rect(5, 15, work_width, 10);
	c_color work_color = work_duration_ms > k_max_frame_interval_ms ?
		k_color_red :
		k_color_blue;
	render_system_draw_rect(work_rect, work_color, render_layer_debug);
}
#endif //CONFIG_DEBUG