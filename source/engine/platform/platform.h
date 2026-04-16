#ifndef __PLATFORM_H__
#define __PLATFORM_H__
#pragma once

#include "config.h"

#ifdef PLATFORM_WINDOWS

#define IGNORE_WINDOWS_WARNINGS_PUSH	\
	_Pragma("warning(push)")			\
	_Pragma("warning(disable: 5105)")

#define IGNORE_WINDOWS_WARNINGS_POP		\
	_Pragma("warning(pop)")




#else

#endif //PLATFORM_WINDOWS

#define NOP() platform_nop()

void platform_nop();


#endif //__PLATFORM_H__