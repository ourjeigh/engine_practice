#ifndef __APPLICATION_H__
#define __APPLICATION_H__
#pragma once

#include "window.h"

class c_application
{
public:
	void init();
	void term();
	void run();
	void handle_window_event(s_event& event);
	void handle_escape_key(bool down);
private:
	void handle_window_close();
	void handle_window_focus(bool is_in_focus);
	void handle_window_resize(int32 height, int32 width);
	
	// make this something like an i_graphics_endpoint so that we can
	// swap it out with a screen recorder instead of a window
	c_window_thread m_window;
	bool m_running;
};

#endif //__APPLICATION_H__
