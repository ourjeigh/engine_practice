#ifndef __RENDER_TYPES_H__
#define __RENDER_TYPES_H__
#pragma once

#include "types/types.h"
#include "structures/vector.h"
#include "structures/shapes.h"

const real32 k_one_over_uint8_max = 1 / static_cast<real32>(k_uint8_max);

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
	c_color() : m_data(k_invalid) DEBUG_ONLY_PARAM_LEFT_COMMA(d_name("")) {}
	constexpr c_color(uint32 data DEBUG_ONLY_PARAM_LEFT_COMMA(const char* name = "")) :
		m_data(data) 
		DEBUG_ONLY_PARAM_LEFT_COMMA(d_name(name)) {}
	
	constexpr c_color(real32 r, real32 g, real32 b, real32 a DEBUG_ONLY_PARAM_LEFT_COMMA(const char* name = ""))
	{
		set(r, g, b, a);
		d_name = name;
	}

	inline real32 alpha()	const { return ((m_data & 0xFF000000) >> 24) * k_one_over_uint8_max; }
	inline real32 red()		const { return ((m_data & 0x00FF0000) >> 16) * k_one_over_uint8_max; }
	inline real32 green()	const { return ((m_data & 0x0000FF00) >> 8) * k_one_over_uint8_max; }
	inline real32 blue()	const { return ((m_data & 0x000000FF) >> 0) * k_one_over_uint8_max; }

	constexpr void set(real32 r, real32 g, real32 b, real32 a)
	{
		m_data =
			(static_cast<uint32>(a * k_uint8_max) << 24) |
			(static_cast<uint32>(r * k_uint8_max) << 16) |
			(static_cast<uint32>(g * k_uint8_max) << 8) |
			(static_cast<uint32>(b * k_uint8_max) << 0);
	}
	
	void set_alpha(real32 alpha)
	{
		set(red(), green(), blue(), alpha);
	}

	constexpr uint32 to_uint32() const
	{
		return m_data;
	}

	static_member_function c_color from_uint32(uint32 color)
	{
		c_color new_color(color);
		return new_color;
	}

	static_member_function c_color from_rgba(real32 r, real32 g, real32 b, real32 a)
	{
		c_color new_color(r,g,b,a);
		return new_color;
	}

private:
	uint32 m_data;
	IF_DEBUG(const char* d_name;)
};

constexpr c_color k_color_red(1.0f, 0.0f, 0.0f, 1.0f DEBUG_ONLY_PARAM_LEFT_COMMA("red"));
constexpr c_color k_color_green(0.0f, 1.0f, 0.0f, 1.0f DEBUG_ONLY_PARAM_LEFT_COMMA("green"));
constexpr c_color k_color_blue(0.0f, 0.0f, 1.0f, 1.0f DEBUG_ONLY_PARAM_LEFT_COMMA("blue"));
constexpr c_color k_color_black(0.0f, 0.0f, 0.0f, 1.0f DEBUG_ONLY_PARAM_LEFT_COMMA("black"));
constexpr c_color k_color_white(1.0f, 1.0f, 1.0f, 1.0f DEBUG_ONLY_PARAM_LEFT_COMMA("white"));

using t_render_shape_point = t_vector_2d_int32;
using t_render_shape_rect = t_rect_2d_int32;

struct s_screen_dimensions
{
	int32 width;
	int32 height;
};

struct s_render_shape_circle
{
	t_render_shape_point center;
	int32 radius;
};

#endif // !__RENDER_TYPES_H__