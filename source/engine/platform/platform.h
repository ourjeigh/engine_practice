#ifndef __PLATFORM_H__
#define __PLATFORM_H__
#pragma once

#include "config.h"
#include "structures/string/string.h"

#ifdef _WIN64
#define PLATFORM_NAME "win64"
#define PLATFORM_WIN64
#endif // _WIN64


#define NOP() platform_nop()

void platform_nop();
void platform_log_to_console(const c_string& message);

#endif //__PLATFORM_H__