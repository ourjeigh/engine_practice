#ifndef __SOFTWARE_RENDERER_H__
#define __SOFTWARE_RENDERER_H__
#pragma once

#include "engine/engine_system.h"
#include "structures/array.h"
#include "threads/atomic.h"
#include "types/render_types.h"

const int32 k_window_max_height = 2400;
const int32 k_window_max_width = 3840;
const int32 k_window_buffer_size = k_window_max_height * k_window_max_width;

struct s_backbuffer
{
	s_screen_dimensions dimensions;
	c_static_array<uint32, k_window_buffer_size> memory;
};

class c_render_system : public c_engine_system<c_render_system>
{
public:
	void init();
	void term();
	void update();

	void fill_screen(const c_color color);
	void draw_rect(const t_render_shape_rect rect, const c_color color, e_render_layer layer);
	void draw_line(const t_render_shape_point start, const t_render_shape_point end, const c_color color, e_render_layer layer);
	void draw_circle(const s_render_shape_circle circle, c_color color, bool fill, e_render_layer layer);
	void draw_bitmap(const s_bitmap_asset& bitmap, const t_render_shape_rect& rect, e_render_layer layer);
	void draw_string(const c_string string, int32 x, int32 y, int32 scale, c_color color, e_render_layer layer);

	const s_backbuffer* get_backbuffer();

	void resize(int32 width, int32 height);

	t_render_shape_point get_screen_center() const;
	s_screen_dimensions get_screen_dimensions() const;

private:
	c_static_array<s_backbuffer, 2> m_buffers;
	c_atomic<int32> m_write_buffer_index;
};

inline void render_system_fill_screen(const c_color color) { c_render_system::get().fill_screen(color); }
inline void render_system_draw_rect(const t_render_shape_rect rect, const c_color color, e_render_layer layer) { c_render_system::get().draw_rect(rect, color, layer); }
inline void render_system_draw_line(const t_render_shape_point start, const t_render_shape_point end, const c_color color, e_render_layer layer) { c_render_system::get().draw_line(start, end, color, layer); }
inline void render_system_draw_circle(const s_render_shape_circle circle, c_color color, bool fill, e_render_layer layer) { c_render_system::get().draw_circle(circle, color, fill, layer); }
inline void render_system_draw_bitmap(const s_bitmap_asset& bitmap, const t_render_shape_rect& rect, e_render_layer layer) { c_render_system::get().draw_bitmap(bitmap, rect, layer); }
inline void render_system_draw_string(const c_string string, int32 x, int32 y, int32 scale, c_color color, e_render_layer layer) { c_render_system::get().draw_string(string, x, y, scale, color, layer); }

inline s_screen_dimensions render_system_get_screen_dimensions() { return c_render_system::get().get_screen_dimensions(); }
inline t_render_shape_point render_system_get_screen_center() { return c_render_system::get().get_screen_center(); }
#endif //__SOFTWARE_RENDERER_H__
