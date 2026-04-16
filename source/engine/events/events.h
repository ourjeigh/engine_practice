#ifndef __EVENTS_H__
#define __EVENTS_H__
#pragma once

enum e_event_type : byte
{
	event_type_none,
	event_type_window_close,
	event_type_window_focus,
	event_type_window_resize,
	event_type_input_key,
	event_type_input_mouse,
	event_type_input_controller,

	k_event_type_count
};

enum e_event_category
{
	window_event_type_none,
	window_event_type_window,
	window_event_type_input,

	k_window_event_type_count
};

struct s_event
{
	virtual e_event_type get_type() = 0;
	virtual e_event_category get_category() = 0;
	bool is_type(e_event_type other) { return other == get_type(); }
};


#endif //__EVENTS_H__