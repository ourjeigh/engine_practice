#ifndef __ENGINE_API_H__
#define __ENGINE_API_H__
#pragma once

#include "types/types.h"
#include "types/asset_types.h"
#include "types/audio_types.h"
#include "types/input_types.h"
#include "types/render_types.h"
#include "structures/string/string.h"

struct i_engine
{
	virtual void log_verbose(c_string message) = 0;
	virtual void log_warning(c_string message) = 0;
	virtual void log_error(c_string message) = 0;
	virtual void log_critical(c_string message) = 0;

	virtual s_key_state input_get_key_state(e_input_keycode key) = 0;
	virtual const s_mouse_state* input_get_mouse_state() = 0;

	virtual bool load_asset(const s_asset_definition* asset_def, f_asset_loaded_callback* callback, void* object) = 0;
	virtual bool load_asset_list(const c_array<s_asset_definition>& asset_list, f_asset_loaded_callback* callback, void* object) = 0;
	virtual const s_asset* get_asset(c_string_id asset_id) = 0;

	virtual void render_fill_screen(const c_color color) = 0;
	virtual void render_draw_rect(const t_render_shape_rect rect, const c_color color) = 0;
	virtual void render_draw_line(const t_render_shape_point start, const t_render_shape_point end, const c_color color, e_render_layer layer) = 0;
	virtual void render_draw_circle(const s_render_shape_circle circle, c_color color, bool fill) = 0;
	virtual void render_draw_bitmap(const s_bitmap_asset& bitmap, const t_render_shape_rect& rect, e_render_layer layer) = 0;
	virtual void render_draw_string(const c_string string, int32 x, int32 y, int32 scale, c_color color) = 0;
	
	virtual s_screen_dimensions get_screen_dimensions() = 0;
	virtual t_render_shape_point get_screen_center() = 0;

	virtual t_sound_playback_id play_sound(s_sound_info& info) = 0;
	virtual t_sound_playback_id play_sound(const s_wav_asset& asset) = 0;
	virtual void stop_sound(t_sound_playback_id playback_id) = 0;
	virtual t_sound_playback_id play_debug_pip() = 0;

	virtual void request_exit() = 0;
};

inline extern i_engine* g_engine_ptr = nullptr;

template<typename... t_args>
inline void engine_log_verbose(const char* message, t_args... args) 
{
	t_string_512 log_msg;
	log_msg.printf(message, args...);
	g_engine_ptr->log_verbose(log_msg);
}

template<typename... t_args>
inline void engine_log_warning(const char* message, t_args... args)
{
	t_string_512 log_msg;
	log_msg.printf(message, args...);
	g_engine_ptr->log_warning(log_msg);
}

template<typename... t_args>
inline void engine_log_error(const char* message, t_args... args)
{
	t_string_512 log_msg;
	log_msg.printf(message, args...);
	g_engine_ptr->log_error(log_msg);
}

template<typename... t_args>
inline void engine_log_critical(const char* message, t_args... args)
{
	t_string_512 log_msg;
	log_msg.printf(message, args...);
	g_engine_ptr->log_critical(log_msg);
}

inline s_key_state engine_input_get_key_state(e_input_keycode key) { return g_engine_ptr->input_get_key_state(key); }
inline const s_mouse_state* engine_input_get_mouse_state() { return g_engine_ptr->input_get_mouse_state(); }

inline bool engine_load_asset(const s_asset_definition* asset_def, f_asset_loaded_callback* callback, void* object) { return g_engine_ptr->load_asset(asset_def, callback, object); }
inline bool engine_load_asset_list(const c_array<s_asset_definition>& asset_list, f_asset_loaded_callback* callback, void* object) { return g_engine_ptr->load_asset_list(asset_list, callback, object); }
inline const s_asset* engine_get_asset(c_string_id asset_id) { return g_engine_ptr->get_asset(asset_id); }

inline void engine_render_fill_screen(const c_color color) { g_engine_ptr->render_fill_screen(color); }
inline void engine_render_draw_rect(const t_render_shape_rect rect, const c_color color) { g_engine_ptr->render_draw_rect(rect, color); }
inline void engine_render_draw_line(const t_render_shape_point start, const t_render_shape_point end, const c_color color, e_render_layer layer) { g_engine_ptr->render_draw_line(start, end, color, layer); }
inline void engine_render_draw_circle(const s_render_shape_circle circle, c_color color, bool fill) { g_engine_ptr->render_draw_circle(circle, color, fill); }
inline void engine_render_bitmap(const s_bitmap_asset& bitmap, const t_render_shape_rect& rect, e_render_layer layer) { g_engine_ptr->render_draw_bitmap(bitmap, rect, layer); }
inline void engine_render_draw_string(const c_string string, int32 x, int32 y, int32 scale, c_color color) { g_engine_ptr->render_draw_string(string, x, y, scale, color); }

inline s_screen_dimensions engine_get_screen_dimensions() { return g_engine_ptr->get_screen_dimensions(); }
inline t_render_shape_point engine_get_screen_center() { return g_engine_ptr->get_screen_center(); }

inline t_sound_playback_id engine_audio_play_sound(const s_wav_asset& asset) { return g_engine_ptr->play_sound(asset); }
inline t_sound_playback_id engine_audio_play_sound(s_sound_info& info) { return g_engine_ptr->play_sound(info); }
inline void engine_audio_stop_sound(t_sound_playback_id playback_id) { return g_engine_ptr->stop_sound(playback_id); }
inline t_sound_playback_id engine_audio_play_debug_pip() { return g_engine_ptr->play_debug_pip(); }

inline void engine_request_exit() { return g_engine_ptr->request_exit(); }
#endif // !__ENGINE_API_H__
