#ifndef __ASSERTS_H__
#define __ASSERTS_H__
#pragma once

#if defined(ENGINE_SOURCE) && !defined(GAME_SOURCE)

#include "platform/platform_assert.h"
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

#else // ENGINE_SOURCE

#include "engine_api.h"
#define ASSERT(condition) \
	do { \
		if (!(condition)) { \
			engine_assert(#condition, __FILE__, __LINE__); \
		} \
	} while (0)

#define COMPILE_ASSERT(condition) static_assert(condition)

#define HALT(message) \
	do { \
		engine_halt(message,  __FILE__, __LINE__); \
	} while (0)
#endif // ENGINE_SOURCE

#define COMPILE_ASSERT(condition) static_assert(condition)
#define HALT_UNIMPLEMENTED() HALT("Unimplemented")

#endif //__ASSERTS_H__