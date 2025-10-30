#include <engine/engine.h>

#define WIDE(x) L##x

const char* k_application_name = "SiMM Engine";

int main()
{
	c_engine engine;
	engine.init();

	return 0;
}

#include <windows.h>
#include <asserts.h>
#include <engine/engine_system.h>
#include <engine/input/input_system.h>

LRESULT CALLBACK process_message_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
)
{

	// Register the window class.
	const char* CLASS_NAME = "SiMM Window";

	WNDCLASS window_class = { };

	window_class.lpfnWndProc = process_message_callback;
	window_class.hInstance = hInstance;
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
		hInstance,						// Instance handle
		NULL							// Additional application data
	);

	if (hwnd == NULL)
	{
		return 0;
	}

	ShowWindow(hwnd, nShowCmd);

	c_engine engine;
	engine.init();

	// Run the message loop.
	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0) != 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		engine.update();
	}

	return 0;
}

LRESULT CALLBACK process_message_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_DESTROY)
	{
		PostQuitMessage(0);
		return 0;
	}
	
	// move to some kind of render?
	if (uMsg == WM_PAINT)
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		// All painting occurs here, between BeginPaint and EndPaint.
		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
		EndPaint(hwnd, &ps);
		return 0;
	}

	if (input_system_queue_message(uMsg, wParam))
	{
		return 0;
	}
	

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}