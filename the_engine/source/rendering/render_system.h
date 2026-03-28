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

// TODO: move
struct s_color
{
	s_color() : red(k_invalid), green(k_invalid), blue(k_invalid), alpha(k_invalid) {}
	constexpr s_color(real32 r, real32 g, real32 b, real32 a) : red(r), green(g), blue(b), alpha(a) {}
	
	real32 red;
	real32 green;
	real32 blue;
	real32 alpha;

	constexpr uint32 to_uint32() const
	{
		return
			(math_round_real32_to_uint32(alpha * k_uint8_max) << 24) |
			(math_round_real32_to_uint32(red * k_uint8_max) << 16) |
			(math_round_real32_to_uint32(green * k_uint8_max) << 8) |
			(math_round_real32_to_uint32(blue * k_uint8_max) << 0);
	}

	static s_color from_uint32(uint32 color)
	{
		s_color new_color;
		new_color.alpha = ((color & 0xFF000000) >> 24) / static_cast<real32>(k_uint8_max);
		new_color.red = ((color & 0x00FF0000) >> 16) / static_cast<real32>(k_uint8_max);
		new_color.green = ((color & 0x0000FF00) >> 8) / static_cast<real32>(k_uint8_max);
		new_color.blue = ((color & 0x000000FF) >> 0)  / static_cast<real32>(k_uint8_max);
		return new_color;
	}
};

constexpr s_color k_color_red(1.0f, 0.0f, 0.0f, 1.0f);
constexpr s_color k_color_green(0.0f, 1.0f, 0.0f, 1.0f);
constexpr s_color k_color_blue(0.0f, 0.0f, 1.0f, 1.0f);

constexpr uint32 k_color_red_uint32 = k_color_red.to_uint32();
constexpr uint32 k_color_blue_uint32 = k_color_blue.to_uint32();
constexpr uint32 k_color_green_uint32 = k_color_green.to_uint32();

// TODO: move
struct s_render_shape_point
{
	int32 x;
	int32 y;
};

struct s_render_shape_rect
{
	int32 x;
	int32 y;
	int32 width;
	int32 height;
};

struct s_render_shape_circle
{
	s_render_shape_point center;
	int32 radius;
};

class c_render_system : public c_engine_system<c_render_system>
{
public:
	void init();
	void term();
	void update();

	void fill_screen(const uint32 color);
	void draw_rect(const s_render_shape_rect rect, const uint32 color);
	void draw_line(const s_render_shape_point start, const s_render_shape_point end, const uint32 color);
	void draw_circle(const s_render_shape_circle circle, uint32 color);
	const s_backbuffer* get_backbuffer();

	// BUG: this gets called too early, before c_render_system's global initializes, which resets it back to 0 :(
	void resize(int32 width, int32 height);

	s_render_shape_point get_screen_center() const;

private:
	c_static_array<s_backbuffer, 2> m_buffers;
	c_atomic<int32> m_write_buffer_index;

};

#endif //__SOFTWARE_RENDERER_H__
