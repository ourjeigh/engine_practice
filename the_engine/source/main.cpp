#include <engine/engine.h>
#include <windows.h>
#include <asserts.h>
#include <engine/engine_system.h>
#include <engine/input/input_system.h>
#include <engine/input/input_map.h>
#include <memory.h>
#include <threads/threads.h>
#include <logging/logging.h>

const char* k_application_name = "SiMM Engine";

LRESULT CALLBACK process_message_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void create_window(HINSTANCE instance);

int main()
{
	engine_init();

	return 0;
}

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd)
{
	c_thread window_thread;
	window_thread.create(THREAD_FUNCTION(create_window), THREAD_ARGS(hInstance), WIDE("Window Thread"));
	window_thread.start();

	engine_init();
	
	return 0;
}

void create_window(HINSTANCE instance)
{
	// Register the window class.
	const char* CLASS_NAME = "SiMM Window";

	WNDCLASS window_class = { };

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
		return;
	}

	ShowWindow(hwnd, SW_SHOWDEFAULT);

	MSG msg;
	zero_object(msg);

	bool quit = false;
	while (GetMessage(&msg, hwnd, 0, 0) != 0 && !quit)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		// this should be moved to engine code and signalled to the message thread
		const c_key_state* escape_key = input_system_get_key_state(_input_key_special_esc);
		if (escape_key != nullptr && escape_key->is_down())
		{
			quit = true;
		}
	}

	engine_term();
}

LRESULT CALLBACK process_message_callback(HWND hwnd, UINT msg, WPARAM param, LPARAM lParam)
{
	if (msg == WM_DESTROY)
	{
		PostQuitMessage(0);
		return 0;
	}
	
	// move to some kind of render?
	if (msg == WM_PAINT)
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		// All painting occurs here, between BeginPaint and EndPaint.
		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
		EndPaint(hwnd, &ps);
		return 0;
	}

	if (input_system_queue_message(msg, param))
	{
		return 0;
	}

	return DefWindowProc(hwnd, msg, param, lParam);
}