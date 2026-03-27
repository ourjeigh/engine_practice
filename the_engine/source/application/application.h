#ifndef __APPLICATION_H__
#define __APPLICATION_H__
#pragma once

#include "window.h"
#include "engine/engine.h"
#include <events/events.h>

class c_application
{
public:
	void init();
	void term();
	void run();
	void handle_window_event(s_event& event);
	void handle_escape_key(bool down);
private:
	c_window_thread m_window;
	bool m_running;
	//c_engine m_engine;

	void handle_window_close();
	void handle_window_focus(bool is_in_focus);
	void handle_window_resize(int32 height, int32 width);
};

#endif //__APPLICATION_H__
