#include "platform/platform.h"

#ifdef PLATFORM_WINDOWS

#include "platform/platform_process.h"
#include "debug/asserts.h"
#include "engine/file_system/file.h"
#include "platform/win64/platform_handle_win64.h"

IGNORE_WINDOWS_WARNINGS_PUSH
#include "windows.h"
#include "libloaderapi.h"
IGNORE_WINDOWS_WARNINGS_POP

c_platform_handle platform_process_load_library(c_file_path& library_path)
{
	ASSERT(file_exists(library_path));
	HMODULE library_handle = LoadLibraryA(library_path.get_full_path());
	return c_platform_handle_factory::get_platform_handle_from_native_handle<HMODULE>(library_handle);
}

bool platform_process_unload_library(c_platform_handle& library)
{
	HMODULE library_handle = c_platform_handle_factory::get_native_handle_from_platform_handle<HMODULE>(library);
	return FreeLibrary(library_handle) != 0;
}

void* platform_process_get_library_function_address(c_platform_handle& library, c_string& function)
{
	HMODULE library_handle = c_platform_handle_factory::get_native_handle_from_platform_handle<HMODULE>(library);
	return GetProcAddress(library_handle, function.get_const_char());
}

bool platform_process_start_process_and_wait(c_file_path& process_path, c_string& command)
{
	bool success = false;

	STARTUPINFO startup_info = { sizeof(startup_info) };
	PROCESS_INFORMATION proc_info = { 0 };

	char cmd[256];
	memory_copy(&cmd[0], command.data(), sizeof(char) * command.used());

	LPCSTR lpApplicationName = process_path.get_full_path();
	LPSTR lpCommandLine = &cmd[0];
	LPSECURITY_ATTRIBUTES lpProcessAttributes = {};
	LPSECURITY_ATTRIBUTES lpThreadAttributes = {};
	BOOL bInheritHandles = false;
	DWORD dwCreationFlags = CREATE_NEW_CONSOLE;
	LPVOID lpEnvironment = nullptr;
	LPCSTR lpCurrentDirectory = nullptr;
	LPSTARTUPINFOA lpStartupInfo = &startup_info;
	LPPROCESS_INFORMATION lpProcessInformation = &proc_info;

	if (CreateProcessA(
		lpApplicationName,
		lpCommandLine,
		lpProcessAttributes,
		lpThreadAttributes,
		bInheritHandles,
		dwCreationFlags,
		lpEnvironment,
		lpCurrentDirectory,
		lpStartupInfo,
		lpProcessInformation))
	{
		// Wait until child process exits.
		WaitForSingleObject(proc_info.hProcess, INFINITE);

		// Close process and thread handles. 
		CloseHandle(proc_info.hProcess);
		CloseHandle(proc_info.hThread);
		
		success = true;
	}
	else
	{
		DWORD error = GetLastError();
		NOP();
	}

	return success;
}
#endif //PLATFORM_WINDOWS