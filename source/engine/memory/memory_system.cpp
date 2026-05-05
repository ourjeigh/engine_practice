#include "memory/memory_system.h"
#include "memory/allocator.h"
#include "platform/platform_memory.h"

const uint32 k_global_memory_bytes = k_byte_mb;

c_static_stack<c_stack_allocator, k_memory_arena_count> g_arenas;

void c_memory_system::init()
{
	uint64 total_memory = 0;
	for (auto it = k_arena_sizes.begin_const(); it != k_arena_sizes.end_const(); ++it)
	{
		total_memory += *it;
	}

	void* base = platform_memory_allocate(total_memory);
	uint64 current_arena_address = reinterpret_cast<uint64>(base);

	for (int32 arena_index = 0; arena_index < g_arenas.capacity(); arena_index++)
	{
		const uint64 arena_size = k_arena_sizes[arena_index];
		g_arenas[arena_index].set_memory(reinterpret_cast<void*>(current_arena_address), arena_size);
		current_arena_address += arena_size;
	}

	ASSERT(current_arena_address - reinterpret_cast<uint64>(base) == total_memory);
}

void c_memory_system::term()
{
	for (auto it = g_arenas.begin(); it != g_arenas.end(); ++it)
	{
		it->clear();
	}
}

void c_memory_system::update()
{
}

void* c_memory_system::allocate(uint64 size, uint64 align, e_memory_arena arena)
{
	// we may want to only allow global allocations during initializations (instead of update/term etc)
	return g_arenas[arena].allocate(size, align);
}
