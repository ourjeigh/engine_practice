#include "engine.h"
#include "audio/audio_system.h"
#include "debug/logging.h"
#include "platform/platform_assert.h"
#include "input/input_system.h"
#include "rendering/render_system.h"
#include "structures/string/string.h"
#include "assets/asset_system.h"

void c_engine::log_verbose(c_string message)
{
	c_logging_system::get().log(verbose, message);
}

void c_engine::log_warning(c_string message)
{
	c_logging_system::get().log(warning, message);
}

void c_engine::log_error(c_string message)
{
	c_logging_system::get().log(error, message);
}

void c_engine::log_critical(c_string message)
{
	c_logging_system::get().log(critical, message);
}

void c_engine::assert(const char* condition, const char* file, const long line)
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

const s_mouse_state* c_engine::input_get_mouse_state()
{
	return input_system_get_mouse_state();
}

bool c_engine::load_asset(const s_asset_definition* asset_def, f_asset_loaded_callback* callback, void* object)
{
	return c_asset_system::get().load_asset(asset_def, callback, object);
}

const s_asset* c_engine::get_asset(c_string_id asset_id)
{
	return c_asset_system::get().get_asset(asset_id);
};

void c_engine::render_fill_screen(const uint32 color)
{
	render_system_fill_screen(color);
}

void c_engine::render_draw_rect(const s_render_shape_rect rect, const uint32 color)
{
	render_system_draw_rect(rect, color);
}

void c_engine::render_draw_line(const t_render_shape_point start, const t_render_shape_point end, const uint32 color)
{
	render_system_draw_line(start, end, color);
}

void c_engine::render_draw_circle(const s_render_shape_circle circle, uint32 color, bool fill)
{
	render_system_draw_circle(circle, color, fill);
}

void c_engine::render_draw_bitmap(const s_bitmap_asset bitmap, int32 x, int32 y, e_render_layer layer)
{
	render_system_draw_bitmap(bitmap, x, y, layer);
}

t_render_shape_point c_engine::get_screen_center()
{
	return render_system_get_screen_center();
}

t_sound_playback_id c_engine::play_sound(s_sound_info& info)
{
	return audio_system_play_sound(info);
}

t_sound_playback_id c_engine::play_sound(const s_wav_asset& asset)
{
	return audio_system_play_sound(asset);
}