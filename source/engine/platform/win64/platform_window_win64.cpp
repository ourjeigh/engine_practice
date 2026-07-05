#include "platform/platform.h"
#ifdef PLATFORM_WIN64

#include "platform/platform_window.h"

#include "engine/input/input_system.h"
#include "debug/logging.h"
#include "platform/platform.h"
#include "structures/array.h"
#include "threads/threads.h"
#include "mmath.h"
#include "rendering/render_system.h"
#include "platform/win64/win64_includes.h"

//remove
const char* k_application_name = "SiMM Engine";

struct s_window_info
{
	int32 width;
	int32 height;
	BITMAPINFO bmi;
};

static_global HWND g_hwnd; 
static_global s_window_info g_window_info = {};

LRESULT CALLBACK process_message_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void c_window_thread::init(int32 width, int32 height)
{
	g_window_info.width = width;
	g_window_info.height = height;
	create(THREAD_FUNCTION(c_window_thread::window_thread_entry_point), THREAD_ARGS(this), WIDE("Window Thread"));
	start();
}

void c_window_thread::term()
{
	m_running = false;
	join();
}

void c_window_thread::window_thread_entry_point(c_window_thread const_ptr thread)
{
	if (thread->setup_window())
	{
		thread->m_running = true;

		while (thread->m_running)
		{
			thread->message_pump();
			thread->render();
		}
	}

	zero_object(g_hwnd);
	zero_object(g_window_info);
}

bool c_window_thread::setup_window()
{
	// Register the window class.
	const char* CLASS_NAME = k_application_name;

	HINSTANCE instance = GetModuleHandle(nullptr);

	WNDCLASS window_class = {};

	window_class.style = CS_OWNDC;
	window_class.lpfnWndProc = process_message_callback;
	window_class.hInstance = instance;
	window_class.lpszClassName = CLASS_NAME;

	RegisterClass(&window_class);

	ASSERT(g_window_info.width > 0);
	ASSERT(g_window_info.height > 0);

	// Create the window.
	g_hwnd = CreateWindowEx(
		0,								// Optional window styles.
		CLASS_NAME,						// Window class
		k_application_name,				// Window text
		WS_OVERLAPPEDWINDOW,			// Window style
		CW_USEDEFAULT, CW_USEDEFAULT,	// Position (X Y)
		g_window_info.width, g_window_info.height,
		NULL,							// Parent window
		NULL,							// Menu
		instance,						// Instance handle
		NULL							// Additional application data
	);

	if (g_hwnd == NULL)
	{
		log_message(error, "c_window_thread: failed to create window");
		return false;
	}

	SetLastError(0);

	LONG_PTR result = SetWindowLongPtr(
		g_hwnd,
		GWLP_USERDATA,
		reinterpret_cast<LONG_PTR>(this));

	ShowWindow(g_hwnd, SW_SHOWDEFAULT);

	resize(g_window_info.width, g_window_info.height);

	return true;
}

void c_window_thread::message_pump()
{
	MSG msg;
	zero_object(msg);

	while (PeekMessageA(&msg, g_hwnd, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void c_window_thread::render()
{
	const s_backbuffer* backbuffer = c_render_system::get().get_backbuffer();

	HDC hdc = GetDC(g_hwnd);

	t_render_shape_rect source_rect(0, 0, backbuffer->dimensions.width, backbuffer->dimensions.height);
	t_render_shape_rect dest_rect(0, 0, g_window_info.width, g_window_info.height);

	StretchDIBits(
		hdc,
		dest_rect.x, dest_rect.y, dest_rect.width, dest_rect.height, // dest
		source_rect.x, source_rect.y, source_rect.width, source_rect.height, // src
		backbuffer->memory.data(),
		&g_window_info.bmi,
		DIB_RGB_COLORS,
		SRCCOPY
	);

	//ReleaseDC(0, hdc);
	ReleaseDC(g_hwnd, hdc);
}

void c_window_thread::resize(int width, int height)
{
	ASSERT((width * height) % 2 == 0);
	zero_object(g_window_info);

	g_window_info.width = width;
	g_window_info.height = height;

	g_window_info.bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	g_window_info.bmi.bmiHeader.biWidth = width;
	g_window_info.bmi.bmiHeader.biHeight = -height; // top-down
	g_window_info.bmi.bmiHeader.biPlanes = 1;
	g_window_info.bmi.bmiHeader.biBitCount = 32;
	g_window_info.bmi.bmiHeader.biCompression = BI_RGB;
}

struct s_window_rect
{
	int32 height;
	int32 width;
};

// returns true if rect was modified
// TODO: this should check against some list of supported rects and pick the closest
bool verify_window_size(s_window_rect& in_out_rect)
{
	bool modified = false;
	if (in_out_rect.height % 2 != 0)
	{
		in_out_rect.height--;
		modified = true;
	}

	if (in_out_rect.width % 2 != 0)
	{
		in_out_rect.width--;
		modified = true;
	}

	return modified;
}

LRESULT CALLBACK process_message_callback(HWND hwnd, UINT msg, WPARAM param, LPARAM lParam)
{
	LONG_PTR user_data = GetWindowLongPtr(hwnd, GWLP_USERDATA);
	c_window_thread* window = reinterpret_cast<c_window_thread*>(user_data);
	
	if (window != nullptr)
	{
		if (msg == WM_CLOSE || msg == WM_DESTROY || msg == WM_QUIT)
		{
			s_window_event_close event;
			window->send_window_event(event);
			//window->term();
			PostQuitMessage(0);
			return 0;
		}

		if (msg == WM_SIZING)
		{
			RECT* rect = reinterpret_cast<RECT*>(lParam);

			s_window_rect requested;
			requested.height = rect->bottom - rect->top;
			requested.width = rect->right - rect->left;

			verify_window_size(requested);
			window->resize(requested.width, requested.height);

			s_window_event_resize event;
			event.height = requested.height;
			event.width = requested.width;
			window->send_window_event(event);

			return 0;
		}

		if (msg == WM_WINDOWPOSCHANGING)
		{
			WINDOWPOS* window_position = reinterpret_cast<WINDOWPOS*>(lParam);

			if (!(window_position->flags & SWP_NOSIZE))
			{
				s_window_rect requested;
				requested.width = window_position->cx;
				requested.height = window_position->cy;

				verify_window_size(requested);
				window->resize(requested.width, requested.height);

				s_window_event_resize event;
				event.height = requested.height;
				event.width = requested.width;
				window->send_window_event(event);
			}

			return 0;
		}

		if (msg == WM_SETFOCUS || msg == WM_KILLFOCUS)
		{
			s_window_event_focus event;
			event.is_in_focus = msg == WM_SETFOCUS;
			window->send_window_event(event);

			return 0;
		}

		if (msg == WM_SETCURSOR)
		{
			// make our own??
			SetCursor(0);
			
			return 0;
		}

		if (msg == WM_KEYDOWN || msg == WM_KEYUP)
		{
			uint16 repeat_count = 0;
			
			bool is_repeat = (HIWORD(lParam) & KF_REPEAT) == KF_REPEAT;

			if (is_repeat)
			{
				repeat_count = LOWORD(lParam);
			}

			s_input_key_event event;
			event.data.key = get_key_code_from_platform_key(param);
			event.data.repeat_count = repeat_count;
			event.data.down = msg == WM_KEYDOWN;
			window->send_window_event(event);

			return 0;
		}

		if (msg >= WM_MOUSEFIRST && msg <= WM_MOUSELAST)
		{
			if (msg == WM_MOUSEMOVE)
			{
				s_input_mouse_event event;
				event.data.x = GET_X_LPARAM(lParam);
				event.data.y = GET_Y_LPARAM(lParam);
				window->send_window_event(event);
			}
			else
			{
				s_input_key_event event;
				event.data.key = input_key_invalid;
				event.data.repeat_count = 0; // ?

				switch (msg)
				{
				case WM_LBUTTONDOWN:
					event.data.key = input_mouse_left;
					event.data.down = true;
					break;
				case WM_LBUTTONUP:
					event.data.key = input_mouse_left;
					event.data.down = false;
					break;
				case WM_MBUTTONDOWN:
					event.data.key = input_mouse_middle;
					event.data.down = true;
					break;
				case WM_MBUTTONUP:
					event.data.key = input_mouse_middle;
					event.data.down = false;
					break;
				case WM_RBUTTONDOWN:
					event.data.key = input_mouse_right;
					event.data.down = true;
					break;
				case WM_RBUTTONUP:
					event.data.key = input_mouse_right;
					event.data.down = false;
					break;
				default:
					// handle repeats here??
					NOP();
					break;
				}

				if (event.data.key != input_key_invalid)
				{
					window->send_window_event(event);
				}
			}

			return 0;
		}

		if (msg == WM_PAINT)
		{
			ValidateRect(hwnd, nullptr);
			return 0;
		}
	}

	return DefWindowProc(hwnd, msg, param, lParam);
}

#endif //PLATFORM_WINDOWS