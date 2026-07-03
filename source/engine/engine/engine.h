#ifndef __ENGINE_H__
#define __ENGINE_H__
#pragma once

#include "engine_api.h"

class c_engine : public i_engine
{
public:
	void log_verbose(c_string message);
	void log_warning(c_string message);
	void log_error(c_string message);
	void log_critical(c_string message);
	void assert(const char* condition, const char* file, const long line);
	void halt(const char* message, const char* file, const long line);

	s_key_state input_get_key_state(e_input_keycode key);
	void input_consume_key_state(e_input_keycode key);
	const s_mouse_state* input_get_mouse_state();

	bool load_asset(const s_asset_definition* asset_def, f_asset_loaded_callback* callback, void* object);
	bool load_asset_list(const c_array<s_asset_definition>& asset_list, f_asset_loaded_callback* callback, void* object);
	const s_asset* get_asset(c_string_id asset_id);

	void render_fill_screen(const c_color color);
	void render_draw_rect(const t_render_shape_rect rect, const c_color color, bool fill, e_render_layer layer);
	void render_draw_line(const t_render_shape_point start, const t_render_shape_point end, const c_color color, e_render_layer layer);
	void render_draw_circle(const s_render_shape_circle circle, c_color color, bool fill, e_render_layer layer);
	void render_draw_bitmap(const s_bitmap_asset& bitmap, const t_render_shape_rect& rect, e_render_layer layer);
	void render_draw_string(const c_string string, int32 x, int32 y, int32 scale, c_color color, e_render_layer layer);

	s_screen_dimensions get_screen_dimensions();
	t_render_shape_point get_screen_center();

	t_sound_playback_id play_sound(s_sound_info& info);
	t_sound_playback_id play_sound(const s_wav_asset& asset);
	t_sound_playback_id play_debug_pip();
	void stop_sound(t_sound_playback_id playback_id);

	void request_exit();
};

#endif //__ENGINE_H__