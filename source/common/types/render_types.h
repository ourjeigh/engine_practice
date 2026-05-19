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
			(/*math_round_real32_to_uint32*/static_cast<uint32>(alpha * k_uint8_max) << 24) |
			(/*math_round_real32_to_uint32*/static_cast<uint32>(red * k_uint8_max) << 16) |
			(/*math_round_real32_to_uint32*/static_cast<uint32>(green * k_uint8_max) << 8) |
			(/*math_round_real32_to_uint32*/static_cast<uint32>(blue * k_uint8_max) << 0);
	}

	static s_color from_uint32(uint32 color)
	{
		s_color new_color;
		new_color.alpha = ((color & 0xFF000000) >> 24) / static_cast<real32>(k_uint8_max);
		new_color.red = ((color & 0x00FF0000) >> 16) / static_cast<real32>(k_uint8_max);
		new_color.green = ((color & 0x0000FF00) >> 8) / static_cast<real32>(k_uint8_max);
		new_color.blue = ((color & 0x000000FF) >> 0) / static_cast<real32>(k_uint8_max);
		return new_color;
	}
};

constexpr s_color k_color_red(1.0f, 0.0f, 0.0f, 1.0f);
constexpr s_color k_color_green(0.0f, 1.0f, 0.0f, 1.0f);
constexpr s_color k_color_blue(0.0f, 0.0f, 1.0f, 1.0f);

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