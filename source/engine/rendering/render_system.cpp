
#include "rendering/render_system.h"
#include "memory/memory.h"
#include "memory/allocator.h"
#include "memory/memory_system.h"
#include "types/types.h"
#include "perf/perf.h"
#include "platform/platform.h"
#include "debug/debug_letters.h"

enum e_render_message_type
{
	render_message_type_invalid = k_invalid,

	render_message_type_fill_screen,
	render_message_type_draw_rect,
	render_message_type_draw_line,
	render_message_type_draw_circle,
	render_message_type_draw_bitmap,
	render_message_type_draw_string,

	k_render_message_type_count
};

struct s_render_message_data
{
};

struct s_render_message_data_fill_screen : s_render_message_data
{
	uint32 color;
};

struct s_render_message_data_draw_rect : s_render_message_data
{
	t_render_shape_rect rect;
	uint32 fill_color;
	uint32 outline_color; // TODO;
};

struct s_render_message_data_draw_line : s_render_message_data
{
	t_render_shape_point start;
	t_render_shape_point end;
	uint32 color;
};

struct s_render_message_data_draw_circle : s_render_message_data
{
	s_render_shape_circle circle;
	uint32 color;
	bool fill;
};

struct s_render_message_data_draw_bitmap : s_render_message_data
{
	s_bitmap_asset bitmap;
	t_render_shape_rect rect;
};

struct s_render_message_data_draw_string : s_render_message_data
{
	t_string_256 string;
	int32 x;
	int32 y;
	int32 scale;
	c_color color;
};

struct s_render_message
{
	e_render_message_type type;
	s_render_message_data* data;
};

void process_fill_screen_message_internal(const s_render_message_data_fill_screen const_ptr message, s_backbuffer const_ptr buffer);
void process_draw_rect_message_internal(const s_render_message_data_draw_rect const_ptr message, s_backbuffer const_ptr buffer);
void process_draw_line_message_internal(const s_render_message_data_draw_line const_ptr message, s_backbuffer const_ptr buffer);
void process_draw_circle_message_internal(const s_render_message_data_draw_circle const_ptr message, s_backbuffer const_ptr buffer);
void process_draw_circle_outline_message_internal(const s_render_message_data_draw_circle const_ptr message, s_backbuffer const_ptr buffer);
void process_draw_bitmap_message_internal(const s_render_message_data_draw_bitmap const_ptr message, s_backbuffer const_ptr buffer);
void process_draw_string_message_internal(const s_render_message_data_draw_string const_ptr message, s_backbuffer const_ptr buffer);

inline void draw_pixel_to_buffer_internal(int32 x, int32 y, uint32 color, s_backbuffer const_ptr buffer);
void draw_horizontal_line_internal(int32 start_x, int32 end_x, int32 y, uint32 color, s_backbuffer const_ptr buffer);

const int32 k_render_commands_size_kb = k_byte_mib;
static_global c_static_stack_allocator<k_render_commands_size_kb>* g_render_commands_allocator;

// if render system moves off the main thread it needs to be made safe
// TBD if each layer needs the same number (thinking about _background)
using t_render_message_layer_stack = c_static_stack<s_render_message, 256>;
using t_render_message_stack_array = c_static_array<t_render_message_layer_stack, k_render_layer_count>;
static_global t_render_message_stack_array* g_render_messages;

void c_render_system::init()
{
	m_buffers[0].dimensions.width = 1440;
	m_buffers[1].dimensions.width = 1440;
	m_buffers[0].dimensions.height = 720;
	m_buffers[1].dimensions.height = 720;

	g_render_commands_allocator = ALLOCATE_NEW_GLOBAL(c_static_stack_allocator<k_render_commands_size_kb>, memory_arena_system);
	g_render_messages = ALLOCATE_NEW_GLOBAL(t_render_message_stack_array, memory_arena_system);
}

void c_render_system::term()
{
}


void c_render_system::update()
{
	PERF_MEASURE_SECTION("c_render_system::update");
	
	const int32 write_index = m_write_buffer_index.load();
	s_backbuffer& current_backbuffer = m_buffers[write_index];

	// process the messages starting from the bottom layer so that we draw higher layers on top of lower
	for (int32 layer_index = 0; layer_index < g_render_messages->capacity(); layer_index++)
	{
		t_render_message_layer_stack* layer_messages = g_render_messages->get_item(layer_index);

		while (!layer_messages->empty())
		{
			const s_render_message& message = layer_messages->top();
			layer_messages->pop();

			switch (message.type)
			{
			case render_message_type_fill_screen:
			{
				const s_render_message_data_fill_screen* message_data = static_cast<s_render_message_data_fill_screen*>(message.data);
				ASSERT(message_data != nullptr);
				process_fill_screen_message_internal(message_data, &current_backbuffer);
				break;
			}
			case render_message_type_draw_rect:
			{
				const s_render_message_data_draw_rect* message_data = static_cast<s_render_message_data_draw_rect*>(message.data);
				ASSERT(message_data != nullptr);
				process_draw_rect_message_internal(message_data, &current_backbuffer);
				break;
			}
			case render_message_type_draw_line:
			{
				const s_render_message_data_draw_line* message_data = static_cast<s_render_message_data_draw_line*>(message.data);
				ASSERT(message_data != nullptr);
				process_draw_line_message_internal(message_data, &current_backbuffer);
				break;
			}
			case render_message_type_draw_circle:
			{
				const s_render_message_data_draw_circle* message_data = static_cast<s_render_message_data_draw_circle*>(message.data);
				ASSERT(message_data != nullptr);
				if (message_data->fill)
				{
					process_draw_circle_message_internal(message_data, &current_backbuffer);
				}
				else
				{
					process_draw_circle_outline_message_internal(message_data, &current_backbuffer);
				}
				break;
			}
			case render_message_type_draw_bitmap:
			{
				const s_render_message_data_draw_bitmap* message_data = static_cast<s_render_message_data_draw_bitmap*>(message.data);
				ASSERT(message_data != nullptr);
				process_draw_bitmap_message_internal(message_data, &current_backbuffer);
				break;
			}
#ifdef CONFIG_DEBUG
			case render_message_type_draw_string:
			{
				const s_render_message_data_draw_string* message_data = static_cast<s_render_message_data_draw_string*>(message.data);
				ASSERT(message_data != nullptr);
				process_draw_string_message_internal(message_data, &current_backbuffer);
				break;
			}
#endif //CONFIG_DEBUG
			default:
				HALT_UNIMPLEMENTED();
			}
		}
	}

	m_write_buffer_index.store(!write_index);
	g_render_commands_allocator->clear();
}


void c_render_system::fill_screen(const uint32 color)
{
	s_render_message_data_fill_screen* message_data = ALLOCATE_NEW(s_render_message_data_fill_screen, *g_render_commands_allocator);

	ASSERT(message_data != nullptr);

	message_data->color = color;

	s_render_message& new_message = g_render_messages->get_item(render_layer_background)->push();

	new_message.type = render_message_type_fill_screen;
	new_message.data = message_data;
}

void c_render_system::draw_rect(const t_render_shape_rect rect, const uint32 color)
{
	s_render_message_data_draw_rect* message_data = ALLOCATE_NEW(s_render_message_data_draw_rect, *g_render_commands_allocator);

	ASSERT(message_data != nullptr);

	message_data->rect = rect;
	message_data->fill_color = color;

	s_render_message& new_message = g_render_messages->get_item(render_layer_main)->push();
	new_message.type = render_message_type_draw_rect;
	new_message.data = message_data;
}

void c_render_system::draw_line(const t_render_shape_point start, const t_render_shape_point end, const uint32 color, e_render_layer layer)
{
	s_render_message_data_draw_line* message_data = ALLOCATE_NEW(s_render_message_data_draw_line, *g_render_commands_allocator);
	
	ASSERT(message_data != nullptr);

	message_data->start = start;
	message_data->end = end;
	message_data->color = color;

	s_render_message& new_message = g_render_messages->get_item(layer)->push();
	new_message.type = render_message_type_draw_line;
	new_message.data = message_data;
}

void c_render_system::draw_circle(const s_render_shape_circle circle, uint32 color, bool fill)
{
	s_render_message_data_draw_circle* message_data = ALLOCATE_NEW(s_render_message_data_draw_circle, *g_render_commands_allocator);
	ASSERT(message_data != nullptr);

	message_data->circle = circle;
	message_data->color = color;
	message_data->color = color;
	message_data->fill = fill;

	s_render_message& new_message = g_render_messages->get_item(render_layer_main)->push();
	new_message.type = render_message_type_draw_circle;
	new_message.data = message_data;
}

void c_render_system::draw_bitmap(const s_bitmap_asset& bitmap, const t_render_shape_rect& rect, e_render_layer layer)
{
	s_render_message_data_draw_bitmap* message_data = ALLOCATE_NO_CONSTRUCTOR(s_render_message_data_draw_bitmap, *g_render_commands_allocator);
	ASSERT(message_data != nullptr);

	message_data->bitmap = bitmap;
	message_data->rect = rect;

	s_render_message& new_message = g_render_messages->get_item(layer)->push();
	new_message.type = render_message_type_draw_bitmap;
	new_message.data = message_data;
}

void c_render_system::draw_string(const c_string string, int32 x, int32 y, int32 scale, c_color color)
{
	s_render_message_data_draw_string* message_data = ALLOCATE_NEW(s_render_message_data_draw_string, *g_render_commands_allocator);
	ASSERT(message_data != nullptr);

	message_data->string.copy_from(string);
	message_data->x = x;
	message_data->y = y;
	message_data->scale = scale;
	message_data->color = color;

	s_render_message& new_message = g_render_messages->get_item(render_layer_debug)->push();
	new_message.type = render_message_type_draw_string;
	new_message.data = message_data;
}

const s_backbuffer* c_render_system::get_backbuffer()
{
	const int32 write_index = m_write_buffer_index.load();
	return &m_buffers[!write_index];
}

// BUG: this gets called too early, before c_render_system's global initializes, which resets it back to 0 :(
void c_render_system::resize(int32 width, int32 height)
{
	m_buffers[0].dimensions.width = width;
	m_buffers[0].dimensions.height = height;
	m_buffers[1].dimensions.width = width;
	m_buffers[1].dimensions.height = height;
}

s_screen_dimensions c_render_system::get_screen_dimensions() const
{
	s_screen_dimensions out = { m_buffers[0].dimensions.width, m_buffers[0].dimensions.height };
	return out;
}

t_render_shape_point c_render_system::get_screen_center() const
{
	return t_render_shape_point(m_buffers[0].dimensions.width / 2, m_buffers[0].dimensions.height / 2);
}

void process_fill_screen_message_internal(const s_render_message_data_fill_screen const_ptr message, s_backbuffer const_ptr buffer)
{
	PERF_MEASURE_FUNCTION();
	ASSERT(buffer->dimensions.width && buffer->dimensions.height);

	uint64 pixel_count = buffer->dimensions.height * buffer->dimensions.width;
	ASSERT(pixel_count % 2 == 0);

	// pack 2 pixels together for a faster memset
	uint64 packed_color = (static_cast<uint64>(message->color) << 32) | message->color;
	memory_set(buffer->memory.data(), packed_color, sizeof(packed_color) * pixel_count * 0.5f);
}

void process_draw_rect_message_internal(const s_render_message_data_draw_rect const_ptr message, s_backbuffer const_ptr buffer)
{
	PERF_MEASURE_FUNCTION();
	ASSERT(buffer->dimensions.width && buffer->dimensions.height);

	const int32 start_x = math_max(k_int32_zero, message->rect.x);
	const int32 end_x = math_min(buffer->dimensions.width - 1, message->rect.x + message->rect.width);
	const int32 start_y = math_max(k_int32_zero, message->rect.y);
	const int32 end_y = math_min(buffer->dimensions.height - 1, message->rect.y + message->rect.width);

	for (int32 y = start_y;	y <= end_y; y++)
	{
		draw_horizontal_line_internal(
			start_x,
			end_x,
			y,
			message->fill_color,
			buffer);
	}
}

void move_point_within_buffer_space(t_render_shape_point& point, const s_backbuffer const_ptr buffer)
{
	point.x() = math_pin(k_int32_zero, buffer->dimensions.width - 1, point.x());
	point.y() = math_pin(k_int32_zero, buffer->dimensions.height - 1, point.y());
}

void process_draw_line_message_internal(const s_render_message_data_draw_line const_ptr message, s_backbuffer const_ptr buffer)
{
	PERF_MEASURE_FUNCTION();
	ASSERT(buffer->dimensions.width && buffer->dimensions.height);
	
	// Bresenham's line algorithm
	// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

	t_render_shape_point start = message->start;
	t_render_shape_point end= message->end;

	move_point_within_buffer_space(start, buffer);
	move_point_within_buffer_space(end, buffer);

	if (math_abs(end.y() - start.y()) < math_abs(end.x() - start.x()))
	{
		if (start.x() > end.x())
		{
			memory_swap(&start, &end);
		}

		int32 delta_x = end.x() - start.x();
		int32 delta_y = end.y() - start.y();
		int32 y_direction = 1;

		if (delta_y < 0)
		{
			y_direction = -1;
			delta_y = -delta_y;
		}

		int32 difference = (2 * delta_y) - delta_x;
		int32 y = start.y();

		for (int32 x = start.x(); x <= end.x(); x++)
		{
			draw_pixel_to_buffer_internal(x, y, message->color, buffer);

			if (difference > 0)
			{
				y = y + y_direction;
				difference = difference + (2 * (delta_y - delta_x));
			}
			else
			{
				difference = difference + 2 * delta_y;
			}
		}
	}
	else
	{
		if (start.y() > end.y())
		{
			memory_swap(&start, &end);
		}

		int32 delta_x = end.x() - start.x();
		int32 delta_y = end.y() - start.y();
		int32 x_direction = 1;

		if (delta_x < 0)
		{
			x_direction = -1;
			delta_x = -delta_x;
		}

		int32 difference = (2 * delta_x) - delta_y;
		int32 x = start.x();

		for (int32 y = start.y(); y <= end.y(); y++)
		{
			draw_pixel_to_buffer_internal(x, y, message->color, buffer);

			if (difference > 0)
			{
				x = x + x_direction;
				difference = difference + (2 * (delta_x - delta_y));
			}
			else
			{
				difference = difference + 2 * delta_x;
			}
		}
	}
}

void process_draw_circle_outline_message_internal(const s_render_message_data_draw_circle const_ptr message, s_backbuffer const_ptr buffer)
{
	PERF_MEASURE_FUNCTION();
	const real32 degree_delta = 0.005f;
	const real32 radius = message->circle.radius;
	
	for (real32 degree = 0; degree <= k_math_real64_two_pi; degree += degree_delta)
	{
		int32 x = math_round_real32_to_int32(radius * math_cos(degree)) + message->circle.center.x();
		int32 y = math_round_real32_to_int32(radius * math_sin(degree)) + message->circle.center.y();
		
		if (in_range_inclusive(k_int32_zero, buffer->dimensions.width, x) && in_range_inclusive(k_int32_zero, buffer->dimensions.height, y))
		{
			draw_pixel_to_buffer_internal(x, y, message->color, buffer);
		}
	}
}

void process_draw_circle_message_internal(const s_render_message_data_draw_circle const_ptr message, s_backbuffer const_ptr buffer)
{
	PERF_MEASURE_FUNCTION();
	//https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
	
	const int32 radius = message->circle.radius;
	const int32 center_x = message->circle.center.x();
	const int32 center_y = message->circle.center.y();

	int32 r_squared = radius * radius;
	
	// Iterate through all y values in the circle's range
	for (int32 y = -radius; y <= radius; ++y)
	{
		int32 y_pos = center_y + y;
		if (in_range_inclusive(k_int32_zero, buffer->dimensions.height, y_pos))
		{
			int32 y_squared = y * y;

			// Calculate the maximum horizontal distance (dx) for this y using the circle equation
			// x^2 + y^2 = r^2  => x^2 = r^2 - y^2
			int32 delta_x = static_cast<int>(math_sqrt(r_squared - y_squared));

			int32 x_start = math_pin(k_int32_zero, buffer->dimensions.width - 1, center_x - delta_x);
			int32 x_end = math_pin(k_int32_zero, buffer->dimensions.width - 1, center_x + delta_x);

			draw_horizontal_line_internal(x_start, x_end, y_pos, message->color, buffer);
		}
	}
}

inline void process_draw_bitmap_message_internal(const s_render_message_data_draw_bitmap const_ptr message, s_backbuffer const_ptr buffer)
{
	PERF_MEASURE_FUNCTION();

	const real32 width_scale = static_cast<real32>(message->bitmap.width) / message->rect.width;
	const real32 height_scale = static_cast<real32>(message->bitmap.height) / message->rect.height;
	
	const int32 x_start = message->rect.x;
	const int32 y_start = message->rect.y;
	const int32 x_end = x_start + message->rect.width;
	const int32 y_end = y_start + message->rect.height;

	for (int32 dest_y = y_start; dest_y < y_end; dest_y++)
	{
		if (in_range_inclusive_int32(0, buffer->dimensions.height - 1, dest_y))
		{
			int32 source_y = (dest_y - message->rect.y) * height_scale;

			for (int32 dest_x = x_start; dest_x < x_end; dest_x++)
			{
				if (in_range_inclusive_int32(0, buffer->dimensions.width - 1, dest_x))
				{
					int32 source_x = (dest_x - message->rect.x) * width_scale;
					
					uint32 index = source_y * message->bitmap.width + source_x;
					uint32 pixel = message->bitmap.pixels[index];

					draw_pixel_to_buffer_internal(dest_x, dest_y, pixel, buffer);
				}
			}
		}
	}
}

inline void process_draw_string_message_internal(const s_render_message_data_draw_string const_ptr message, s_backbuffer const_ptr buffer)
{
	const int32 scale = message->scale;
	int32 current_x = message->x;
	int32 current_y = message->y;
	
	for (auto it = message->string.begin_const(); it != message->string.end_const(); ++it)
	{
		if (*it == '\n' || *it == '\r')
		{
			current_y += 10;
			current_x = message->x;
			continue;
		}

		// TODO: update string iterators to stop before null term
		if (*it == k_null_char) break;

		t_debug_render_char_array letter_pixels = get_debug_char_array(*it);

		int i = 0;
		int32 start_x = current_x;
		int32 start_y = current_y;
		for (int y = start_y; y < start_y + k_debug_char_pixel_height * scale; y+=scale)
		{
			for (int x = start_x; x < start_x + k_debug_char_pixel_width * scale; x+=scale)
			{
				// we need a 64bit float to handle the precision loss in multiplication
				real64 letter_weight = *letter_pixels.get_item(i);
				uint32 pixel = message->color.to_uint32() * letter_weight;

				for (int32 sy = y; sy < y + scale; sy++)
				{
					if (in_range_inclusive_int32(0, buffer->dimensions.height - 1, sy))
					{
						for (int32 sx = x; sx < x + scale; sx++)
						{
							if (in_range_inclusive_int32(0, buffer->dimensions.width - 1, sx))
							{
								draw_pixel_to_buffer_internal(sx, sy, pixel, buffer);
							}
						}
					}
				}

				i++;
			}
		}

		current_x += k_debug_char_pixel_width * scale;
	}
}

inline void draw_pixel_to_buffer_internal(int32 x, int32 y, uint32 color, s_backbuffer const_ptr buffer)
{
	ASSERT(in_range_inclusive(k_int32_zero, buffer->dimensions.width, x) && in_range_inclusive(k_int32_zero, buffer->dimensions.height, y));

	uint32 alpha = (color >> 24) & 0xFF;
	if (alpha == 0xFF)
	{
		buffer->memory[y * buffer->dimensions.width + x] = color;
	}
	else if (alpha > 0)
	{
			uint32 buffer_pixel = buffer->memory[y * buffer->dimensions.width + x];
			real32 buffer_red = ((buffer_pixel >> 16) & 0xFF) / 255.0f;
			real32 buffer_green = ((buffer_pixel >> 8) & 0xFF) / 255.0f;
			real32 buffer_blue = ((buffer_pixel >> 0) & 0xFF) / 255.0f;
			
			real32 color_red = ((color >> 16) & 0xFF) / 255.0f;
			real32 color_green = ((color >> 8) & 0xFF) / 255.0f;
			real32 color_blue = ((color >> 0) & 0xFF) / 255.0f;

			real32 falpha = alpha / 255.0f;
			real32 invalpha = 1.0f - falpha;

			real32 fred = (color_red * falpha) + (buffer_red * invalpha);
			real32 fgreen = (color_green * falpha) + (buffer_green * invalpha);
			real32 fblue = (color_blue * falpha) + (buffer_blue * invalpha);

			uint32 red = fred * 255;
			uint32 green = fgreen * 255;
			uint32 blue = fblue * 255;

			uint32 final_pixel = (red << 16) | (green << 8) | (blue << 0);
			buffer->memory[y * buffer->dimensions.width + x] = final_pixel;
			NOP();
	}
	else
	{
		// fully transparent, leave underlying buffer as-is.
		NOP();
	}
}

void draw_horizontal_line_internal(int32 start_x, int32 end_x, int32 y, uint32 color, s_backbuffer const_ptr buffer)
{
	ASSERT(in_range_inclusive(k_int32_zero, buffer->dimensions.width, start_x));
	ASSERT(in_range_inclusive(k_int32_zero, buffer->dimensions.width, end_x));
	ASSERT(in_range_inclusive(k_int32_zero, buffer->dimensions.height, y));

	if (start_x > end_x)
	{
		memory_swap(&start_x, &end_x);
	}
	
	// start by copying sets of 2 pixes (64 bits)
	const int32 count = end_x - start_x;
	const int32 first_chunk_count = count * 0.5f;
	const uint64 packed_color = (static_cast<uint64>(color) << 32) | color;

	memory_set(&buffer->memory[y * buffer->dimensions.width + start_x], packed_color, sizeof(packed_color) * first_chunk_count);

	// if it was an odd number, set the last pixel manually
	if (2 * first_chunk_count != count)
	{
		ASSERT((count - (2 * first_chunk_count)) == 1);
		draw_pixel_to_buffer_internal(end_x, y, color, buffer);
	}
}