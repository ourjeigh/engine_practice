#include "engine.h"
#include "debug/logging.h"
#include "platform/platform_assert.h"
#include "input/input_system.h"
#include "rendering/render_system.h"

void c_engine::log()
{
	log_message(verbose, "c_engine::log");
}

void c_engine::cassert(const char* condition, const char* file, const long line)
{
	assert_internal(condition, file, line);
}

void c_engine::halt(const char* message, const char* file, const long line)
{
	halt_internal(message, file, line);
}

s_key_state c_engine::input_get_key_state(e_input_keycode key)
{
	return input_system_get_key_state(key);
}

void c_engine::render_fill_screen(const uint32 color)
{
	render_system_fill_screen(color);
}

void c_engine::render_draw_rect(const s_render_shape_rect rect, const uint32 color)
{
	render_system_draw_rect(rect, color);
}

void c_engine::render_draw_line(const s_render_shape_point start, const s_render_shape_point end, const uint32 color)
{
	render_system_draw_line(start, end, color);
}

void c_engine::render_draw_circle(const s_render_shape_circle circle, uint32 color, bool fill)
{
	render_system_draw_circle(circle, color, fill);
}

s_render_shape_point c_engine::get_screen_center()
{
	return render_system_get_screen_center();
}
