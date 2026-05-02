
#include "rendering/render_system.h"
#include "engine/input/input_system.h"
#include "memory/memory.h"
#include "memory/allocator.h"
#include "memory/memory.h"
#include "types/types.h"
#include "perf/perf.h"


enum e_render_layer
{
	render_layer_invalid = k_invalid,

	render_layer_background,
	render_layer_main,
	render_layer_ui,
	render_layer_debug,

	k_render_layer_count
};

enum e_render_message_type
{
	render_message_type_invalid = k_invalid,

	render_message_type_fill_screen,
	render_message_type_draw_rect,
	render_message_type_draw_line,
	render_message_type_draw_circle,

	k_render_message_type_count
};

struct s_render_message_data_fill_screen
{
	uint32 color;
};

struct s_render_message_data_draw_rect
{
	s_render_shape_rect rect;
	uint32 fill_color;
	uint32 outline_color; // TODO;
};

struct s_render_message_data_draw_line
{
	s_render_shape_point start;
	s_render_shape_point end;
	uint32 color;
};

struct s_render_message_data_draw_circle
{
	s_render_shape_circle circle;
	uint32 color;
	bool fill;
};

struct s_render_message
{
	e_render_message_type type;
	void* data;
};

void process_fill_screen_message_internal(const s_render_message_data_fill_screen const_ptr message, s_backbuffer const_ptr buffer);
void process_draw_rect_message_internal(const s_render_message_data_draw_rect const_ptr message, s_backbuffer const_ptr buffer);
void process_draw_line_message_internal(const s_render_message_data_draw_line const_ptr message, s_backbuffer const_ptr buffer);
void process_draw_circle_message_internal(const s_render_message_data_draw_circle const_ptr message, s_backbuffer const_ptr buffer);
void process_draw_circle_outline_message_internal(const s_render_message_data_draw_circle const_ptr message, s_backbuffer const_ptr buffer);

inline void draw_pixel_to_buffer_internal(int32 x, int32 y, uint32 color, s_backbuffer const_ptr buffer);
void draw_horizontal_line_internal(int32 start_x, int32 end_x, int32 y, uint32 color, s_backbuffer const_ptr buffer);

static_global c_static_stack_allocator<k_byte_kib> g_render_commands_allocator;

// if render system moves of the main thread it needs to be made safe
// TBD if each layer needs the same number (thinking about _background)
static_global c_static_array<c_static_stack<s_render_message, 128>, k_render_layer_count> g_render_messages;

void c_render_system::init()
{
	m_buffers[0].width = 1440;
	m_buffers[1].width = 1440;
	m_buffers[0].height = 720;
	m_buffers[1].height = 720;
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
	for (int32 layer_index = 0; layer_index < g_render_messages.capacity(); layer_index++)
	{
		c_stack<s_render_message>& layer_messages = g_render_messages[layer_index];

		while (!layer_messages.empty())
		{
			const s_render_message& message = layer_messages.top();
			layer_messages.pop();

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
			default:
				NOP();
			}
		}
	}

	m_write_buffer_index.store(!write_index);
	g_render_commands_allocator.clear();
}

void c_render_system::fill_screen(const uint32 color)
{
	s_render_message_data_fill_screen* message_data = ALLOCATE_NEW(s_render_message_data_fill_screen, g_render_commands_allocator);

	ASSERT(message_data != nullptr);

	message_data->color = color;

	s_render_message& new_message = g_render_messages[render_layer_background].push();

	new_message.type = render_message_type_fill_screen;
	new_message.data = message_data;
}

void c_render_system::draw_rect(const s_render_shape_rect rect, const uint32 color)
{
	s_render_message_data_draw_rect* message_data = ALLOCATE_NEW(s_render_message_data_draw_rect, g_render_commands_allocator);

	ASSERT(message_data != nullptr);

	message_data->rect = rect;
	message_data->fill_color = color;

	s_render_message& new_message = g_render_messages[render_layer_main].push();
	new_message.type = render_message_type_draw_rect;
	new_message.data = message_data;
}

void c_render_system::draw_line(const s_render_shape_point start, const s_render_shape_point end, const uint32 color)
{
	s_render_message_data_draw_line* message_data = ALLOCATE_NEW(s_render_message_data_draw_line, g_render_commands_allocator);
	
	ASSERT(message_data != nullptr);

	message_data->start = start;
	message_data->end = end;
	message_data->color = color;

	s_render_message& new_message = g_render_messages[render_layer_main].push();
	new_message.type = render_message_type_draw_line;
	new_message.data = message_data;
}

void c_render_system::draw_circle(const s_render_shape_circle circle, uint32 color, bool fill)
{
	s_render_message_data_draw_circle* message_data = ALLOCATE_NEW(s_render_message_data_draw_circle, g_render_commands_allocator);
	ASSERT(message_data != nullptr);

	message_data->circle = circle;
	message_data->color = color;
	message_data->color = color;
	message_data->fill = fill;

	s_render_message& new_messge = g_render_messages[render_layer_main].push();
	new_messge.type = render_message_type_draw_circle;
	new_messge.data = message_data;
}


const s_backbuffer* c_render_system::get_backbuffer()
{
	const int32 write_index = m_write_buffer_index.load();
	return &m_buffers[!write_index];
}

// BUG: this gets called too early, before c_render_system's global initializes, which resets it back to 0 :(
void c_render_system::resize(int32 width, int32 height)
{
	m_buffers[0].width = width;
	m_buffers[0].height = height;
	m_buffers[1].width = width;
	m_buffers[1].height = height;
}

s_render_shape_point c_render_system::get_screen_center() const
{
	return s_render_shape_point(m_buffers[0].width / 2, m_buffers[0].height / 2);
}

void process_fill_screen_message_internal(const s_render_message_data_fill_screen const_ptr message, s_backbuffer const_ptr buffer)
{
	PERF_MEASURE_FUNCTION();
	ASSERT(buffer->width && buffer->height);

	uint64 pixel_count = buffer->height * buffer->width;
	ASSERT(pixel_count % 2 == 0);

	// pack 2 pixels together for a faster memset
	uint64 packed_color = (static_cast<uint64>(message->color) << 32) | message->color;
	memory_set(buffer->memory.data(), packed_color, sizeof(packed_color) * pixel_count * 0.5f);
}

void process_draw_rect_message_internal(const s_render_message_data_draw_rect const_ptr message, s_backbuffer const_ptr buffer)
{
	PERF_MEASURE_FUNCTION();
	ASSERT(buffer->width && buffer->height);

	const int32 start_x = math_max(k_int32_zero, message->rect.x);
	const int32 end_x = math_min(buffer->width - 1, message->rect.x + message->rect.width);
	const int32 start_y = math_max(k_int32_zero, message->rect.y);
	const int32 end_y = math_min(buffer->height - 1, message->rect.y + message->rect.width);

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

void move_point_within_buffer_space(s_render_shape_point& point, const s_backbuffer const_ptr buffer)
{
	point.x = math_pin(k_int32_zero, buffer->width - 1, point.x);
	point.y = math_pin(k_int32_zero, buffer->height - 1, point.y);
}

void process_draw_line_message_internal(const s_render_message_data_draw_line const_ptr message, s_backbuffer const_ptr buffer)
{
	PERF_MEASURE_FUNCTION();
	ASSERT(buffer->width && buffer->height);
	
	// Bresenham's line algorithm
	// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

	s_render_shape_point start = message->start;
	s_render_shape_point end= message->end;

	move_point_within_buffer_space(start, buffer);
	move_point_within_buffer_space(end, buffer);

	if (math_abs(end.y - start.y) < math_abs(end.x - start.x))
	{
		if (start.x > end.x)
		{
			memory_swap(&start, &end);
		}

		int32 delta_x = end.x - start.x;
		int32 delta_y = end.y - start.y;
		int32 y_direction = 1;

		if (delta_y < 0)
		{
			y_direction = -1;
			delta_y = -delta_y;
		}

		int32 difference = (2 * delta_y) - delta_x;
		int32 y = start.y;

		for (int32 x = start.x; x <= end.x; x++)
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
		if (start.y > end.y)
		{
			memory_swap(&start, &end);
		}

		int32 delta_x = end.x - start.x;
		int32 delta_y = end.y - start.y;
		int32 x_direction = 1;

		if (delta_x < 0)
		{
			x_direction = -1;
			delta_x = -delta_x;
		}

		int32 difference = (2 * delta_x) - delta_y;
		int32 x = start.x;

		for (int32 y = start.y; y <= end.y; y++)
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
	const real32 degree_delta = 0.005f;
	const real32 radius = message->circle.radius;
	
	for (real32 degree = 0; degree <= k_math_real64_two_pi; degree += degree_delta)
	{
		int32 x = math_round_real32_to_int32(radius * math_cos(degree)) + message->circle.center.x;
		int32 y = math_round_real32_to_int32(radius * math_sin(degree)) + message->circle.center.y;
		
		if (in_range(k_int32_zero, buffer->width, x) && in_range(k_int32_zero, buffer->height, y))
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
	const int32 center_x = message->circle.center.x;
	const int32 center_y = message->circle.center.y;

	int32 r_squared = radius * radius;
	
	// Iterate through all y values in the circle's range
	for (int32 y = -radius; y <= radius; ++y)
	{
		int32 y_pos = center_y + y;
		if (in_range(k_int32_zero, buffer->height, y_pos))
		{
			int32 y_squared = y * y;

			// Calculate the maximum horizontal distance (dx) for this y using the circle equation
			// x^2 + y^2 = r^2  => x^2 = r^2 - y^2
			int32 delta_x = static_cast<int>(math_sqrt(r_squared - y_squared));

			int32 x_start = math_pin(k_int32_zero, buffer->width - 1, center_x - delta_x);
			int32 x_end = math_pin(k_int32_zero, buffer->width - 1, center_x + delta_x);

			draw_horizontal_line_internal(x_start, x_end, y_pos, message->color, buffer);
		}
	}
}

inline void draw_pixel_to_buffer_internal(int32 x, int32 y, uint32 color, s_backbuffer const_ptr buffer)
{
	ASSERT(in_range(k_int32_zero, buffer->width, x) && in_range(k_int32_zero, buffer->height, y));
	buffer->memory[y * buffer->width + x] = color;
}

void draw_horizontal_line_internal(int32 start_x, int32 end_x, int32 y, uint32 color, s_backbuffer const_ptr buffer)
{
	ASSERT(in_range(k_int32_zero, buffer->width, start_x));
	ASSERT(in_range(k_int32_zero, buffer->width, end_x));
	ASSERT(in_range(k_int32_zero, buffer->height, y));

	if (start_x > end_x)
	{
		memory_swap(&start_x, &end_x);
	}
	
	// start by copying sets of 2 pixes (64 bits)
	const int32 count = end_x - start_x;
	const int32 first_chunk_count = count * 0.5f;
	const uint64 packed_color = (static_cast<uint64>(color) << 32) | color;

	memory_set(&buffer->memory[y * buffer->width + start_x], packed_color, sizeof(packed_color) * first_chunk_count);

	// if it was an odd number, set the last pixel manually
	if (2 * first_chunk_count != count)
	{
		ASSERT((count - (2 * first_chunk_count)) == 1);
		draw_pixel_to_buffer_internal(end_x, y, color, buffer);
	}
}