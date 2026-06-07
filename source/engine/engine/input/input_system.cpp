#include "input_system.h"
#include "debug/logging.h"
#include "platform/platform_input.h"
#include "debug/asserts.h"
#include "time/time.h"
#include "structures/array.h"
#include "memory/memory.h"
#include "memory/allocator.h"
#include "platform/platform.h"

// definitions
struct s_input_queued_event
{
	e_event_type type;
	t_timestamp timestamp;

	union 
	{
		s_input_event_mouse_data mouse_data;
		s_input_event_key_data key_data;
		s_input_event_controller_data controller_data;
	};
};

// prototypes
void process_input_event_queue_internal();

// internal data

// called and written from the message pump, processed in the input system update
// todo this needs to be thread safe
c_static_stack<s_input_queued_event, 256> g_input_event_queue;

s_input_state g_input_state;

// these let us calculate timespans for key state without needing to maintain
// a separate internal key state struct
c_static_array<t_timestamp, k_input_key_count> g_key_timestamps;

// public methods
void c_input_system::init()
{
	g_input_event_queue.clear();
	g_input_state.clear();

	for (auto& timestamp : g_key_timestamps)
	{
		timestamp = k_invalid;
	}

	log_message(verbose, "Input System Initialized");
}

void c_input_system::term()
{
	g_input_state.clear();
	log_message(verbose, "Input System Terminated");
}

void c_input_system::update()
{
	process_input_event_queue_internal();

	for (auto& combo : m_key_combo_callbacks)
	{
		bool new_down = true;

		// todo: we should generate a bitmask of keystate above, each combo could actually just store it's own
		// mask and we just compare them instead of iterating each key in the combo
		for (auto& key : combo.combo.keys)
		{
			new_down &= g_input_state.get_key_state(key).is_down;
		}

		if (new_down != combo.down)
		{
			combo.callback(new_down);
			combo.down = new_down;
		}
	}
}

s_key_state input_system_get_key_state(e_input_keycode key)
{
	return g_input_state.key_states[key];
}

const s_mouse_state* input_system_get_mouse_state()
{
	return &g_input_state.mouse_state;
}

const s_input_state* input_system_get_current_input_state()
{
	return &g_input_state;
}

void input_system_handle_event(s_event& event)
{
	ASSERT(event.get_category() == window_event_type_input);

	s_input_queued_event& new_queue = g_input_event_queue.push();
	new_queue.timestamp = get_high_precision_timestamp();
	new_queue.type = event.get_type();

	switch (new_queue.type)
	{
	case event_type_input_key:
	{
		s_input_key_event& key_event = static_cast<s_input_key_event&>(event);
		new_queue.key_data = key_event.data;
		break;
	}
	case event_type_input_mouse:
	{
		// this only coveres mouse position, not buttons or scroll...
		s_input_mouse_event& mouse_event = static_cast<s_input_mouse_event&>(event);
		new_queue.mouse_data = mouse_event.data;
		break;
	}
	case event_type_input_controller:
	{
		s_input_controller_event& controller_event = static_cast<s_input_controller_event&>(event);
		new_queue.controller_data = controller_event.data;
		break;
	}
	default:
		HALT_UNIMPLEMENTED();
	}
}

// private methods

void process_input_event_queue_internal()
{
	t_timestamp current_time = get_high_precision_timestamp();

	// todo: record input to file if setting enabled, to allow for automated playback
	while(!g_input_event_queue.empty())
	{
		s_input_queued_event event = g_input_event_queue.top();
		g_input_event_queue.pop();

		switch (event.type)
		{
		case event_type_input_key:
		{
			s_input_event_key_data& data = event.key_data;
			g_input_state.key_states[data.key].is_down = data.down;
			g_input_state.key_states[data.key].time_in_state = c_engine_time_span::to_time_span(g_key_timestamps[data.key], current_time);

			log_message(verbose, "input system: key:{i} {s} repeat:{i}",
				data.key, 
				data.down ? "down" : "up", 
				data.repeat_count);
			// todo, handle repeat
			break;
		}
		case event_type_input_mouse:
		{
			s_input_event_mouse_data& data = event.mouse_data;
			g_input_state.mouse_state.position.x =  event.mouse_data.x;
			g_input_state.mouse_state.position.y = event.mouse_data.y;
			g_input_state.mouse_state.position.last_changedtimestamp = event.timestamp;
			//log_message(verbose, "input system: mouse position x:{i} y:{i}", data.x, data.y);
			break;
		}
		case event_type_input_controller:
			HALT_UNIMPLEMENTED();
		}
	}

	g_input_event_queue.clear();
}