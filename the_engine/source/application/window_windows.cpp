#include "config.h"
#ifdef PLATFORM_WINDOWS

#include "window.h"

#include "engine/input/input_system.h"
#include "debug/logging.h"
#include "platform/platform.h"
#include "structures/array.h"
#include "threads/threads.h"
#include "mmath.h"

IGNORE_WINDOWS_WARNINGS_PUSH
#include "windows.h"
#include "windowsx.h"
IGNORE_WINDOWS_WARNINGS_POP

//remove
const char* k_application_name = "SiMM Engine";

LRESULT CALLBACK process_message_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void window_thread_entry_point(c_window* window);

void c_window::init()
{
	c_thread window_thread;

	window_thread.create(THREAD_FUNCTION(window_thread_entry_point), THREAD_ARGS(this), WIDE("Window Thread"));
	window_thread.start();
}

void c_window::term()
{
}


const int32 k_window_max_height = 2400;
const int32 k_window_max_width = 3840;
const int32 k_window_buffer_size = k_window_max_height * k_window_max_width;

struct s_backbuffer
{
	int32 width;
	int32 height;
	BITMAPINFO bmi;
	c_array<uint32, k_window_buffer_size> memory;
};

static s_backbuffer g_backbuffer = {};

void resize_backbuffer(HWND hwnd, int width, int height)
{
	g_backbuffer.width = width;
	g_backbuffer.height = height;

	g_backbuffer.bmi = {};
	g_backbuffer.bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	g_backbuffer.bmi.bmiHeader.biWidth = width;
	g_backbuffer.bmi.bmiHeader.biHeight = -height; // top-down
	g_backbuffer.bmi.bmiHeader.biPlanes = 1;
	g_backbuffer.bmi.bmiHeader.biBitCount = 32;
	g_backbuffer.bmi.bmiHeader.biCompression = BI_RGB;
}

void render()
{

	// Clear to dark gray
	for (int y = 0; y < g_backbuffer.height; y++)
	{
		for (int x = 0; x < g_backbuffer.width; x++)
		{
			g_backbuffer.memory[y * g_backbuffer.width + x] = 0xFF202020;
		}
	}

	const c_mouse_state* mouse_state = input_system_get_mouse_state();
	int32 mouse_x = math_pin_int32(25, g_backbuffer.width - 26, mouse_state->position.x);
	int32 mouse_y = math_pin_int32(25, g_backbuffer.height - 26, mouse_state->position.y);

	const uint32 color = input_system_get_key_state(input_mouse_left)->is_down() ?
		0xFFFF0000 : // red
		0x0000FFFF; // ?

	// Simple test square
	for (int y = mouse_y - 25; y < mouse_y + 25; y++)
	{
		for (int x = mouse_x - 25; x < mouse_x + 26; x++)
		{
			g_backbuffer.memory[y * g_backbuffer.width + x] = color;
		}
	}
}

void present(HWND hwnd)
{
	HDC hdc = GetDC(hwnd);

	StretchDIBits(
		hdc,
		0, 0, g_backbuffer.width, g_backbuffer.height,
		0, 0, g_backbuffer.width, g_backbuffer.height,
		g_backbuffer.memory.data(),
		&g_backbuffer.bmi,
		DIB_RGB_COLORS,
		SRCCOPY
	);

	ReleaseDC(hwnd, hdc);
}

void window_thread_entry_point(c_window* window)
{
	// Register the window class.
	const char* CLASS_NAME = k_application_name;

	HINSTANCE instance = GetModuleHandle(nullptr);

	WNDCLASS window_class = {};

	window_class.lpfnWndProc = process_message_callback;
	window_class.hInstance = instance;
	window_class.lpszClassName = CLASS_NAME;

	RegisterClass(&window_class);

	// Create the window.
	HWND hwnd = CreateWindowEx(
		0,								// Optional window styles.
		CLASS_NAME,						// Window class
		k_application_name,				// Window text
		WS_OVERLAPPEDWINDOW,			// Window style
		CW_USEDEFAULT, CW_USEDEFAULT,	// Position (X Y)
		CW_USEDEFAULT, CW_USEDEFAULT,	// Size (Width Height)
		NULL,							// Parent window
		NULL,							// Menu
		instance,						// Instance handle
		NULL							// Additional application data
	);

	if (hwnd == NULL)
	{
		log_message(error, "windows: failed to create window");
		return;
	}

	SetLastError(0);

	LONG_PTR result = SetWindowLongPtr(
		hwnd,
		GWLP_USERDATA,
		reinterpret_cast<LONG_PTR>(window));

	if (result == 0)
	{
		HRESULT result = HRESULT_FROM_WIN32 (GetLastError());
		NOP();
	}


	ShowWindow(hwnd, SW_SHOWDEFAULT);

	MSG msg;
	zero_object(msg);

	bool quit = false;
	while (GetMessage(&msg, hwnd, 0, 0) != 0 && !quit)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		render();
		present(hwnd);

		//// this should be moved to engine code and signalled to the message thread
		//const c_key_state* escape_key = input_system_get_key_state(_input_key_special_esc);
		//if (escape_key != nullptr && escape_key->is_down())
		//{
		//	quit = true;
		//}
	}
}

LRESULT CALLBACK process_message_callback(HWND hwnd, UINT msg, WPARAM param, LPARAM lParam)
{
	LONG_PTR user_data = GetWindowLongPtr(hwnd, GWLP_USERDATA);
	c_window* window = reinterpret_cast<c_window*>(user_data);
	
	if (window != nullptr)
	{

		if (msg == WM_CLOSE || msg == WM_DESTROY || msg == WM_QUIT)
		{
			s_window_event_close event;
			window->send_window_event(event);

			PostQuitMessage(0);
			return 0;
		}

		if (msg == WM_SIZE)
		{
			RECT rect;
			GetClientRect(hwnd, &rect);
			int32 height = rect.bottom - rect.top;
			int32 width = rect.right - rect.left;
			resize_backbuffer(hwnd, width, height);

			s_window_event_resize event;
			event.height = height;
			event.width = width;
			window->send_window_event(event);

			return 0;
		}

		if (msg == WM_SETFOCUS || msg == WM_KILLFOCUS)
		{
			s_window_event_focus event;
			event.is_in_focus = msg == WM_SETFOCUS;
			window->send_window_event(event);

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

		if (msg == WM_MOUSEMOVE)
		{
			s_input_mouse_event event;
			event.data.x = GET_X_LPARAM(lParam);
			event.data.y = GET_Y_LPARAM(lParam);
			window->send_window_event(event);
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