#ifndef __ENGINE_API_H__
#define __ENGINE_API_H__
#pragma once

#include "types/types.h"
#include "types/input_types.h"
#include "types/render_types.h"

struct i_engine
{
	virtual void log() = 0;
	virtual void cassert(const char* condition, const char* file, const long line) = 0;
	virtual void halt(const char* message, const char* file, const long line) = 0;

	virtual s_key_state input_get_key_state(e_input_keycode key) = 0;


	virtual void render_fill_screen(const uint32 color) = 0;
	virtual void render_draw_rect(const s_render_shape_rect rect, const uint32 color) = 0;
	virtual void render_draw_line(const s_render_shape_point start, const s_render_shape_point end, const uint32 color) = 0;
	virtual void render_draw_circle(const s_render_shape_circle circle, uint32 color, bool fill) = 0;
	virtual s_render_shape_point get_screen_center() = 0;
};

inline extern i_engine* g_engine_ptr = nullptr;

inline void engine_log() { g_engine_ptr->log(); }
inline void engine_assert(const char* condition, const char* file, const long line) { g_engine_ptr->cassert(condition, file, line); }
inline void engine_halt(const char* message, const char* file, const long line) { g_engine_ptr->halt(message, file, line); }

inline s_key_state engine_input_get_key_state(e_input_keycode key) { g_engine_ptr->input_get_key_state(key); }

inline void engine_render_fill_screen(const uint32 color) { g_engine_ptr->render_fill_screen(color); }
inline void engine_render_draw_rect(const s_render_shape_rect rect, const uint32 color) { g_engine_ptr->render_draw_rect(rect, color); }
inline void engine_render_draw_line(const s_render_shape_point start, const s_render_shape_point end, const uint32 color) { g_engine_ptr->render_draw_line(start, end, color); }
inline void engine_render_draw_circle(const s_render_shape_circle circle, uint32 color, bool fill) { g_engine_ptr->render_draw_circle(circle, color, fill); }
inline s_render_shape_point engine_get_screen_center() { return g_engine_ptr->get_screen_center(); }
#endif // !__ENGINE_API_H__
