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

	virtual bool load_asset(const s_asset_definition* asset_def);

	virtual void render_fill_screen(const uint32 color) = 0;
	virtual void render_draw_rect(const s_render_shape_rect rect, const uint32 color) = 0;
	virtual void render_draw_line(const s_render_shape_point start, const s_render_shape_point end, const uint32 color) = 0;
	virtual void render_draw_circle(const s_render_shape_circle circle, uint32 color, bool fill) = 0;
	virtual s_render_shape_point get_screen_center() = 0;

	virtual t_sound_playback_id play_sound(s_sound_info& info) = 0;
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

inline bool engine_load_asset(const s_asset_definition* asset_def) { return g_engine_ptr->load_asset(asset_def); }


inline void engine_render_fill_screen(const uint32 color) { g_engine_ptr->render_fill_screen(color); }
inline void engine_render_draw_rect(const s_render_shape_rect rect, const uint32 color) { g_engine_ptr->render_draw_rect(rect, color); }
inline void engine_render_draw_line(const s_render_shape_point start, const s_render_shape_point end, const uint32 color) { g_engine_ptr->render_draw_line(start, end, color); }
inline void engine_render_draw_circle(const s_render_shape_circle circle, uint32 color, bool fill) { g_engine_ptr->render_draw_circle(circle, color, fill); }
inline s_render_shape_point engine_get_screen_center() { return g_engine_ptr->get_screen_center(); }
inline t_sound_playback_id engine_audio_play_sound(s_sound_info& info) { return g_engine_ptr->play_sound(info); }

#endif // !__ENGINE_API_H__
