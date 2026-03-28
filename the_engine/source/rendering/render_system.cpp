
#include "rendering/render_system.h"
#include <engine/input/input_system.h>
#include <memory/memory.h>
#include "memory/allocator.h"
#include "memory/memory.h"
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

enum e_render_message_type
{
	render_message_type_invalid = k_invalid,

	render_message_type_fill_screen,
	render_message_type_draw_rect,
	render_message_type_draw_line,
	render_message_type_draw_circle,

	k_render_message_type_count
};

struct s_render_message_data
{
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
	uint32 fill_color;
	uint32 outline_color;
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

inline void draw_pixel_to_buffer(int32 x, int32 y, uint32 color, s_backbuffer const_ptr buffer);

static_global c_static_stack_allocator<k_byte_kib> g_render_commands_allocator;

// if render system moves of the main thread it needs to be made safe
// TBD if each layer needs the same number (thinking about _background)
static_global c_static_array<c_static_stack<s_render_message, 128>, k_render_layer_count> g_render_messages;

void c_render_system::init()
{
	/*m_buffers[0].width = 1424;
	m_buffers[0].height = 779;
	m_buffers[1].width = 1424;
	m_buffers[1].height = 779;*/
}

void c_render_system::term()
{
}

void c_render_system::update()
{
	const int32 write_index = m_write_buffer_index.load();
	s_backbuffer& current_backbuffer = m_buffers[write_index];
	
	// HACK TEMP
	//if (false)
	{
		g_render_commands_allocator.clear();

		// test code that would ultimately done by real code outside this loop

		fill_screen(0xFF202020);

		const c_mouse_state* mouse_state = input_system_get_mouse_state();
		int32 mouse_x = mouse_state->position.x;
		int32 mouse_y = mouse_state->position.y;

		const uint32 color = input_system_get_key_state(input_mouse_left)->is_down() ?
			k_color_red_uint32 :
			k_color_blue_uint32;

		s_render_shape_rect mouse_box(mouse_x - 25, mouse_y - 25, 50, 50);
		draw_line(get_screen_center(), s_render_shape_point(mouse_x, mouse_y), k_color_green_uint32);

		draw_rect(mouse_box, color);

		s_render_shape_circle circle;
		circle.center = s_render_shape_point(mouse_x, mouse_y);
		circle.radius = 38;
		draw_circle(circle, color);
	}

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
				process_draw_circle_message_internal(message_data, &current_backbuffer);
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

void c_render_system::draw_circle(const s_render_shape_circle circle, uint32 color)
{
	s_render_message_data_draw_circle* message_data = ALLOCATE_NEW(s_render_message_data_draw_circle, g_render_commands_allocator);
	ASSERT(message_data != nullptr);

	message_data->circle = circle;
	message_data->fill_color = color;

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
	ASSERT(buffer->width && buffer->height);

	for (int32 y = 0; y < buffer->height; y++)
	{
		for (int32 x = 0; x < buffer->width; x++)
		{
			draw_pixel_to_buffer(x, y, message->color, buffer);
		}
	}
}

void process_draw_rect_message_internal(const s_render_message_data_draw_rect const_ptr message, s_backbuffer const_ptr buffer)
{
	ASSERT(buffer->width && buffer->height);

	for (int32 x = message->rect.x; x < message->rect.x + message->rect.width; x++)
	{
		for (int32 y = message->rect.y; y < message->rect.y + message->rect.width; y++)
		{
			draw_pixel_to_buffer(x, y, message->fill_color, buffer);
		}
	}
}

void process_draw_line_message_internal(const s_render_message_data_draw_line const_ptr message, s_backbuffer const_ptr buffer)
{
	ASSERT(buffer->width && buffer->height);
	
	// Bresenham's line algorithm
	// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

	s_render_shape_point start = message->start;
	s_render_shape_point end= message->end;

	if (math_abs(end.y - start.y) < math_abs(end.x - start.x))
	{
		if (start.x > end.x)
		{
			memory_swap(&start, &end);
		}

		int32 dx = end.x - start.x;
		int32 dy = end.y - start.y;
		int32 yi = 1;

		if (dy < 0)
		{
			yi = -1;
			dy = -dy;
		}

		int32 D = (2 * dy) - dx;
		int32 y = start.y;

		for (int32 x = start.x; x <= end.x; x++)
		{
			draw_pixel_to_buffer(x, y, message->color, buffer);

			if (D > 0)
			{
				y = y + yi;
				D = D + (2 * (dy - dx));
			}
			else
			{
				D = D + 2 * dy;
			}
		}
	}
	else
	{
		if (start.y > end.y)
		{
			memory_swap(&start, &end);
		}

		int32 dx = end.x - start.x;
		int32 dy = end.y - start.y;
		int32 xi = 1;

		if (dx < 0)
		{
			xi = -1;
			dx = -dx;
		}

		int32 D = (2 * dx) - dy;
		int32 x = start.x;

		for (int32 y = start.y; y <= end.y; y++)
		{
			draw_pixel_to_buffer(x, y, message->color, buffer);

			if (D > 0)
			{
				x = x + xi;
				D = D + (2 * (dx - dy));
			}
			else
			{
				D = D + 2 * dx;
			}
		}
	}
}

void process_draw_circle_message_internal(const s_render_message_data_draw_circle const_ptr message, s_backbuffer const_ptr buffer)
{
	//https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
	
	// this is just an outline, save for later
	/*real32 rad = message->circle.radius;
	float deg = 0;
	
	do {
		int32 X = math_round_real32_to_int32(rad * math_cos(deg));
		int32 Y = math_round_real32_to_int32(rad * math_sin(deg));
		draw_pixel_to_buffer(X + message->circle.center.x, Y + message->circle.center.y, message->fill_color, buffer);
		deg += 0.005;
	} while (deg <= 6.4);*/
	
	const int32 radius = message->circle.radius;
	const int32 center_x = message->circle.center.x;
	const int32 center_y = message->circle.center.y;

	int32 r_squared = radius * radius;
	
	// Iterate through all y values in the circle's range
	for (int y = -radius; y <= radius; ++y) {
		int y_squared = y * y;
		// Calculate the maximum horizontal distance (dx) for this y using the circle equation
		// x^2 + y^2 = r^2  => x^2 = r^2 - y^2
		int dx = static_cast<int>(math_sqrt(r_squared - y_squared));

		// Calculate the start and end points of the horizontal line
		int x_start = center_x - dx;
		int x_end = center_x + dx;
		int y_pos = center_y + y;

		// Draw the horizontal line
		// Need to make a draw_horizontal_line method that can use memcopy
		s_render_message_data_draw_line hack;
		hack.start.x = x_start;
		hack.start.y = y_pos;
		hack.end.x = x_end;
		hack.end.y = y_pos;
		hack.color = message->fill_color;
		process_draw_line_message_internal(&hack, buffer);
	}
}


inline void draw_pixel_to_buffer(int32 x, int32 y, uint32 color, s_backbuffer const_ptr buffer)
{
	if (in_range(k_int32_zero, buffer->width, x) && in_range(k_int32_zero, buffer->height, y))
	{
		buffer->memory[y * buffer->width + x] = color;
	}
}