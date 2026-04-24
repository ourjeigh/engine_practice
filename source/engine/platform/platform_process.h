#ifndef __PLATFORM_PROCESS_H__
#define __PLATFORM_PROCESS_H__
#pragma once

#include "file/file_path.h"
#include "platform_handle.h"

c_platform_handle platform_process_load_library(c_file_path& library_path);
void* platform_process_get_library_function_address(c_platform_handle& library, c_string& function);
bool platform_process_start_process_and_wait(c_file_path& process_path, c_string& command);
#endif // !__PLATFORM_PROCESS_H__

