#ifndef __MEMORY_SYSTEM_H__
#define __MEMORY_SYSTEM_H__
#pragma once

#include "config.h"
#include "engine/engine_system.h"
#include <new>

#define ALLOCATE_NEW_GLOBAL(type, arena, ...) \
	new (c_memory_system::allocate(sizeof(type), alignof(type), arena)) type(__VA_ARGS__)

#define ALLOCATE_GLOBAL_NO_CONSTRUCTOR(type, arena) \
	static_cast<type*>(c_memory_system::allocate(sizeof(type), alignof(type), arena))

enum e_memory_arena
{
	// memory used by a system but not needed for state
	// eg: render command buffer
	memory_arena_system,

	// memory needed to be tracked over game lifetime
	// eg: transforms
	// eg: audio playbacks
	// eg: game state
	memory_arena_state,

	// memory needed for a single frame
	// eg: audio mix buffers
	memory_arena_frame,

	DEBUG_ONLY_PARAM_RIGHT_COMMA(memory_arena_debug)

	k_memory_arena_count
};

// would be nice to have these in something that makes the total easier to get.
const c_static_array<int32, k_memory_arena_count> k_arena_sizes =
{
	10 * k_byte_mb,									// memory_arena_system
	01 * k_byte_mb,									// memory_arena_state
	01 * k_byte_mb,									// memory_arena_frame
	DEBUG_ONLY_PARAM_RIGHT_COMMA(01 * k_byte_mb)	// memory_arena_debug
};

class c_memory_system : public c_engine_system<c_memory_system>
{
public:
	void init();
	void term();
	void update();

	static_function void* allocate(uint64 size, uint64 align, e_memory_arena arena);
};

#endif //__MEMORY_SYSTEM_H__