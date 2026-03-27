
#include "rendering/render_system.h"
#include <engine/input/input_system.h>
#include <memory/memory.h>
#include "memory/allocator.h"
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
	s_rect rect;
	uint32 color;
};

struct s_render_message
{
	e_render_message_type type;
	void* data;
};

void process_draw_rect_message_internal(const s_render_message_data_draw_rect const_ptr message, s_backbuffer const_ptr buffer);
void process_fill_screen_message_internal(const s_render_message_data_fill_screen const_ptr message, s_backbuffer const_ptr buffer);

static_global c_static_stack_allocator<k_byte_kb> g_render_commands_allocator;

// if render system moves of the main thread it needs to be made safe
// TBD if each layer needs the same number (thinking about _background)
static_global c_static_array<c_static_stack<s_render_message, 128>, k_render_layer_count> g_render_messages;

void c_render_system::init()
{
	m_buffers[0].width = 1424;
	m_buffers[0].height = 779;
	m_buffers[1].width = 1424;
	m_buffers[1].height = 779;
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
		int32 mouse_x = math_pin_int32(25, current_backbuffer.width - 26, mouse_state->position.x);
		int32 mouse_y = math_pin_int32(25, current_backbuffer.height - 26, mouse_state->position.y);

		const uint32 color = input_system_get_key_state(input_mouse_left)->is_down() ?
			0xFFFF0000 : // red
			0x0000FFFF; // blue


		s_rect mouse_box(mouse_x - 25, mouse_y - 25, 50, 50);
		draw_rect(mouse_box, color);

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
	s_render_message_data_fill_screen* message_data =
		static_cast<s_render_message_data_fill_screen*>(
			g_render_commands_allocator.allocate(
				sizeof(s_render_message_data_fill_screen),
				alignof(s_render_message_data_fill_screen)));

	ASSERT(message_data != nullptr);

	message_data->color = color;

	s_render_message& new_message = g_render_messages[render_layer_background].push();

	new_message.type = render_message_type_fill_screen;
	new_message.data = message_data;
}

void c_render_system::draw_rect(const s_rect rect, const uint32 color)
{
	s_render_message_data_draw_rect* message_data = 
		static_cast<s_render_message_data_draw_rect*>(
			g_render_commands_allocator.allocate(
				sizeof(s_render_message_data_draw_rect),
				alignof(s_render_message_data_draw_rect)));

	ASSERT(message_data != nullptr);

	message_data->rect = rect;
	message_data->color = color;

	s_render_message& new_message = g_render_messages[render_layer_main].push();

	new_message.type = render_message_type_draw_rect;
	new_message.data = message_data;
}

void c_render_system::fill_screen_OLD(const uint32 color, s_backbuffer const_ptr buffer)
{
	return;
	// temp;
	if (!buffer->width || !buffer->height) return;

	for (int32 y = 0; y < buffer->height; y++)
	{
		for (int32 x = 0; x < buffer->width; x++)
		{
			buffer->memory[y * buffer->width + x] = color;
		}
	}
}

const s_backbuffer* c_render_system::get_backbuffer()
{
	const int32 write_index = m_write_buffer_index.load();
	return &m_buffers[!write_index];
}

// BUG: this gets called too early, before c_render_system's global initializes, which resets it back to 0 :(
void c_render_system::resize(int32 width, int32 height)
{
	//g_buffer.width = width;
	//g_buffer.height = height;
}


void process_draw_rect_message_internal(const s_render_message_data_draw_rect const_ptr message, s_backbuffer const_ptr buffer)
{
	ASSERT(buffer->width && buffer->height);
	//if (!buffer->width || !buffer->height) return;

	for (int32 x_index = message->rect.x; x_index < message->rect.x + message->rect.width; x_index++)
	{
		for (int32 y_index = message->rect.y; y_index < message->rect.y + message->rect.width; y_index++)
		{
			buffer->memory[y_index * buffer->width + x_index] = message->color;
		}
	}
}

void process_fill_screen_message_internal(const s_render_message_data_fill_screen const_ptr message, s_backbuffer const_ptr buffer)
{
	ASSERT(buffer->width && buffer->height);

	for (int32 y = 0; y < buffer->height; y++)
	{
		for (int32 x = 0; x < buffer->width; x++)
		{
			buffer->memory[y * buffer->width + x] = message->color;
		}
	}
}