#ifndef __ASSERTS_H__
#define __ASSERTS_H__
#pragma once
#include <intrin.h>

#define ASSERT(condition) \
	do { \
		if (!(condition)) { \
			assert_internal(#condition, __FILE__, __LINE__); \
		} \
	} while (0)

#define HALT(message) \
	do { \
		halt_internal(message,  __FILE__, __LINE__); \
	} while (0)

#define NOP() __nop()

void assert_internal(const char* condition, const char *file, const long line);
void halt_internal(const char* message, const char* file, const long line);
#endif //__ASSERTS_H__