#include "threads.h"
#include "windows.h"
#include <logging/logging.h>

bool c_thread::create(void* function, void* param, const wchar* name)
{
	m_function = function;
	m_param = param;
	m_thread_name = name;
	return true;
}

bool c_thread::start()
{
	LPSECURITY_ATTRIBUTES lpThreadAttributes = nullptr;
	uint64 dwStackSize= 0;
	DWORD dwCreationFlags = 0;
	bool success = true;

	HANDLE h = CreateThread(
		lpThreadAttributes,
		dwStackSize,
		(LPTHREAD_START_ROUTINE)m_function,
		m_param,
		dwCreationFlags,
		&m_thread_id);

	if (h != nullptr )
	{
		if (m_thread_name != nullptr && SetThreadDescription(h, m_thread_name) != S_OK)
		{
			// if we create the thread but fail to name it, we'll still call that a success
			log(warning, "Failed to set thread description!");
		}
	}
	else
	{
		log(error, "Failed to create thread!");
		success = false;
	}

	return success;
}

void c_thread::join()
{
	HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, m_thread_id);
	if (hThread != nullptr)
	{
		WaitForSingleObject(hThread, INFINITE);
		CloseHandle(hThread);
	}
	else
	{
		log(error, "Failed to open thread for joining!");
		return;
	}

}
