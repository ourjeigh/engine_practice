#ifndef __RENDER_TYPES_H__
#define __RENDER_TYPES_H__
#pragma once

#include "types/types.h"

enum e_render_layer
{
	render_layer_invalid = k_invalid,

	render_layer_background,
	render_layer_main,
	render_layer_ui,
	render_layer_debug,

	k_render_layer_count
};



class c_color
{
public:
	c_color() : data(k_invalid) {}
	constexpr c_color(uint32 data) : data(data) {}
	
	constexpr c_color(real32 r, real32 g, real32 b, real32 a) 
	{
		data = 
			(static_cast<uint32>(a * k_uint8_max) << 24) |
			(static_cast<uint32>(r * k_uint8_max) << 16) |
			(static_cast<uint32>(g * k_uint8_max) << 8)  |
			(static_cast<uint32>(b * k_uint8_max) << 0);
	}

	real32 alpha() { return ((data & 0xFF000000) >> 24) / static_cast<real32>(k_uint8_max); }
	real32 red() { return ((data & 0x00FF0000) >> 16) / static_cast<real32>(k_uint8_max); }
	real32 green() { return ((data & 0x0000FF00) >> 8) / static_cast<real32>(k_uint8_max); }
	real32 blue() { return ((data & 0x000000FF) >> 0) / static_cast<real32>(k_uint8_max); }

	constexpr uint32 to_uint32() const
	{
		return data;
	}

	static c_color from_uint32(uint32 color)
	{
		c_color new_color(color);
		return new_color;
	}

	static c_color from_rgba(real32 r, real32 g, real32 b, real32 a)
	{
		c_color new_color(r,g,b,a);
		return new_color;
	}

private:
	uint32 data;
};

constexpr c_color k_color_red(1.0f, 0.0f, 0.0f, 1.0f);
constexpr c_color k_color_green(0.0f, 1.0f, 0.0f, 1.0f);
constexpr c_color k_color_blue(0.0f, 0.0f, 1.0f, 1.0f);
constexpr c_color k_color_black(1.0f, 1.0f, 1.0f, 1.0f);
constexpr c_color k_color_white(1.0f, 1.0f, 1.0f, 1.0f);

// remove
constexpr uint32 k_color_red_uint32 = k_color_red.to_uint32();
constexpr uint32 k_color_blue_uint32 = k_color_blue.to_uint32();
constexpr uint32 k_color_green_uint32 = k_color_green.to_uint32();

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

#endif // !__RENDER_TYPES_H__