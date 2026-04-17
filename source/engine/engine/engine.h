#ifndef __ENGINE_H__
#define __ENGINE_H__
#pragma once

#include "engine_api.h"

class c_engine : public i_engine
{
public:
	void log();
	void cassert(const char* condition, const char* file, const long line);
	void halt(const char* message, const char* file, const long line);

	s_key_state input_get_key_state(e_input_keycode key);

	void render_fill_screen(const uint32 color);
	void render_draw_rect(const s_render_shape_rect rect, const uint32 color);
	void render_draw_line(const s_render_shape_point start, const s_render_shape_point end, const uint32 color);
	void render_draw_circle(const s_render_shape_circle circle, uint32 color, bool fill);
	s_render_shape_point get_screen_center();
};

#endif //__ENGINE_H__