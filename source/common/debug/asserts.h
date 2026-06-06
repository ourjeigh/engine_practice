#ifndef __ASSERTS_H__
#define __ASSERTS_H__
#pragma once

#include "debug/assert_handler.h"

#define ASSERT(condition) \
	do { \
		if (!(condition))\
		{\
			if (g_assert_handler) \
			{ \
				g_assert_handler("Assertion Failed: " #condition, __FILE__, __LINE__); \
			} \
			else \
			{ \
				int x = *(int*)0; \
			} \
		} \
	} while (0)


#define HALT(message) \
	do { \
		if (g_assert_handler) { \
			g_assert_handler("HALT: " message,  __FILE__, __LINE__); \
		} \
	} while (0)

#define COMPILE_ASSERT(condition) static_assert(condition)
#define HALT_UNIMPLEMENTED() HALT("Unimplemented")

#endif //__ASSERTS_H__