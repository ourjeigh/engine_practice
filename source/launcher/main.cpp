#include "config.h"
#include "application/application.h"

#ifdef PLATFORM_WINDOWS

IGNORE_WINDOWS_WARNINGS_PUSH
#include <windows.h>
IGNORE_WINDOWS_WARNINGS_POP

// TEMP
#include "../game/game.cpp"

c_application g_application;

int main(
	int argc, 
	char** argv)
{
	c_game_test test;
	g_application.init(&test);
	g_application.run();
	g_application.term();
	return 0;
}

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd)
{
	c_game_test game;
	g_application.init(&game);
	g_application.run();
	g_application.term();
	return 0;
}

#endif // PLATFORM_WINDOWS