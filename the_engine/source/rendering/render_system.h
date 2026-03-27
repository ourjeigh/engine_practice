#ifndef __SOFTWARE_RENDERER_H__
#define __SOFTWARE_RENDERER_H__
#pragma once

#include "engine/engine_system.h"
#include "structures/array.h"
#include "threads/atomic.h"

const int32 k_window_max_height = 2400;
const int32 k_window_max_width = 3840;
const int32 k_window_buffer_size = k_window_max_height * k_window_max_width;

struct s_backbuffer
{
	int32 width;
	int32 height;
	c_static_array<uint32, k_window_buffer_size> memory;
};

//static_global s_backbuffer m_buffer;



// TODO: move
struct s_rect
{
	int32 x;
	int32 y;
	int32 width;
	int32 height;
};

class c_render_system : public c_engine_system<c_render_system>
{
public:
	void init();
	void term();

	void update();

	void fill_screen(const uint32 color);
	void draw_rect(const s_rect rect, const uint32 color);

	void fill_screen_OLD(const uint32 color, s_backbuffer const_ptr buffer);

	const s_backbuffer* get_backbuffer();

	// BUG: this gets called too early, before c_render_system's global initializes, which resets it back to 0 :(
	void resize(int32 width, int32 height);

private:
	c_static_array<s_backbuffer, 2> m_buffers;
	c_atomic<int32> m_write_buffer_index;

};

#endif //__SOFTWARE_RENDERER_H__
