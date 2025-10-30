#include "input_system.h"
#include <logging/logging.h>
#include "input_map.h"
#include <asserts.h>
#include <time/time.h>
#include <structures/array.h>

c_stack<s_input_queued_event, 256> g_input_event_queue;

void process_input_event_internal(const s_input_queued_event& event);

void c_input_system::init()
{
	log(verbose, "Input System Initialized");
}

void c_input_system::term()
{
	log(verbose, "Input System Terminated");
}

void c_input_system::update()
{
	//log(verbose, "Input System Update");

	for (auto it = g_input_event_queue.begin(); it != g_input_event_queue.end(); ++it)
	{
		process_input_event_internal(*it);
	}

	g_input_event_queue.clear();
}

s_input_queued_event create_event(t_message_id message_id, t_param param)
{
	s_input_queued_event event;
	event.message_id = message_id;
	event.param = param;
	event.timestamp = get_high_precision_timestamp();
	return event;
}

bool input_system_queue_message(t_message_id message_id, t_param param)
{
	bool handled = false;
	switch (message_id)
	{
	case k_input_id_key_down:
	case k_input_id_key_up:
		s_input_queued_event new_event = create_event(message_id, param);
		//log(verbose, "Key Down Event: keycode=%llu", param);
		g_input_event_queue.push(new_event);
		handled = true;
		break;
	}
	return handled;
}

enum e_input_event_type : byte
{
	_input_event_type_key_down,
	_input_event_type_key_up,
};

enum e_input_event_key_type : byte
{
	_input_event_key_type_num,
	_input_event_key_type_char,
	_input_event_key_type_special,
};

enum e_input_event_key_special : byte
{
	_input_event_key_special_shift,
	_input_event_key_special_control,
	_input_event_key_special_alt,
};

struct s_input_event
{
	e_input_event_type event_type;
	e_input_event_key_type key_type;
	union
	{
		uint8 keycode_num;
		char keycode_char;
		e_input_event_key_special keycode_special;
	};
};

void process_input_event_internal(const s_input_queued_event& event)
{ 
	c_time_span span(event.timestamp, get_high_precision_timestamp());
	
	s_input_event processed_event;

	if (event.message_id == k_input_id_key_down)
	{
		processed_event.event_type = _input_event_type_key_down;
	}
	else if (event.message_id == k_input_id_key_up)
	{
		processed_event.event_type = _input_event_type_key_up;
	}

	if (k_input_id_kbd_0 <= event.param && event.param <= k_input_id_kbd_9)
	{
		processed_event.key_type = _input_event_key_type_num;
		processed_event.keycode_num = static_cast<uint8>(event.param - k_input_id_kbd_0);
		log(verbose, "Processed Keyboard %s Event: %i , delay=%.4fs", 
			processed_event.event_type == _input_event_type_key_down ? "Down" : "Up",
			processed_event.keycode_num, event.param, span.get_duration_seconds());
	}
	else if (k_input_id_kbd_a <= event.param && event.param <= k_input_id_kbd_z)
	{
		processed_event.key_type = _input_event_key_type_char;
		processed_event.keycode_char = static_cast<char>(event.param);
		log(verbose, "Processed Keyboard %s Event: %c , delay=%.4fs",
			processed_event.event_type == _input_event_type_key_down ? "Down" : "Up",
			processed_event.keycode_char, span.get_duration_seconds());
	}
	else if (event.param == k_input_id_kbd_shift)
	{
		processed_event.key_type = _input_event_key_type_special;
		processed_event.keycode_special = _input_event_key_special_shift;
		log(verbose, "Processed Keyboard %s Event: SHIFT , delay=%.4fs", 
			processed_event.event_type == _input_event_type_key_down ? "Down" : "Up",
			span.get_duration_seconds());
	}
	else if (event.param == k_input_id_kbd_control)
	{
		processed_event.key_type = _input_event_key_type_special;
		processed_event.keycode_special = _input_event_key_special_control;
		log(verbose, "Processed Keyboard %s Event: CONTROL , delay=%.4fs", 
			processed_event.event_type == _input_event_type_key_down ? "Down" : "Up",
			span.get_duration_seconds());
	}
	else if (event.param == k_input_id_kbd_menu)
	{
		processed_event.key_type = _input_event_key_type_special;
		processed_event.keycode_special = _input_event_key_special_alt;
		log(verbose, "Processed Keyboard %s Event: ALT , delay=%.4fs", 
			processed_event.event_type == _input_event_type_key_down ? "Down" : "Up",
			span.get_duration_seconds());
	}
	else if (event.param == k_input_id_kbd_esc)
	{
		processed_event.key_type = _input_event_key_type_special;
		log(verbose, "Processed Keyboard %s Event: ESCAPE, delay=%.4fs",
			processed_event.event_type == _input_event_type_key_down ? "Down" : "Up",
			span.get_duration_seconds());
	}
	else
	{
		log(warning, "Unrecognized key event: message_id=%llu, param=%llu", event.message_id, event.param);
	}
}
