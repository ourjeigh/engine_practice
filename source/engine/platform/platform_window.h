#ifndef __WINDOW_H__
#define __WINDOW_H__
#pragma once

#include "events/delegates.h"
#include "types/types.h"	
#include "events/events.h"
#include "threads/threads.h"

using t_event_callback = void(s_event&);

struct s_window_event : s_event
{
	e_event_category get_category() override { return window_event_type_window; }
};

struct s_window_event_close : s_window_event
{
	e_event_type get_type() override { return event_type_window_close; }
};

struct s_window_event_resize : s_window_event
{
	e_event_category get_category() override { return window_event_type_window; }
	e_event_type get_type() override { return event_type_window_resize; }
	int32 width;
	int32 height;
};

struct s_window_event_focus : s_window_event
{
	e_event_type get_type() override { return event_type_window_focus; }
	bool is_in_focus;
};

class c_window_thread : c_thread
{
public:
	void init(int32 width, int32 height);
	void term();

	void set_event_handler(c_delegate<t_event_callback> callback)
	{
		m_event_callback = callback;
	}

	// make private
	void send_window_event(s_event& event)
	{
		if (m_event_callback.is_valid())
		{
			m_event_callback(event);
		}
	}

	void resize(int width, int height);

private:
	static_member_function void window_thread_entry_point(c_window_thread const_ptr thread);
	
	bool setup_window();
	void message_pump();
	void render();

	c_delegate<t_event_callback> m_event_callback;
	bool m_running;
};
#endif //__WINDOW_H__
