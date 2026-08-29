#include "memory/memory_system.h"
#include "memory/allocator.h"
#include "platform/platform_memory.h"

const uint32 k_global_memory_bytes = k_byte_mb;

using t_arenas_array = c_static_array<c_stack_allocator, k_memory_arena_count>;

static_global t_arenas_array g_arenas;

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
	g_arenas[memory_arena_frame].clear();
}

void* c_memory_system::allocate(uint64 size, uint64 align, e_memory_arena arena)
{
	// we may want to only allow global allocations during initializations (instead of update/term etc)
	return g_arenas[arena].allocate(size, align);
}

#ifdef FEATURE_REPLAY
void c_memory_system::get_memory_block_for_replay(const void** out_block, uint64& out_size)
{
	*out_block = g_arenas.get_item(memory_arena_engine_state)->get_base_const();

	out_size =
		g_arenas.get_item(memory_arena_engine_state)->get_capacity() +
		g_arenas.get_item(memory_arena_game_state)->get_capacity();
}

void c_memory_system::set_memory_block_for_replay(const void* block, uint64 size)
{
	ASSERT(size == k_arena_sizes[memory_arena_engine_state] + k_arena_sizes[memory_arena_game_state]);

	memory_copy(
		g_arenas.get_item(memory_arena_engine_state)->get_base(),
		block,
		k_arena_sizes[memory_arena_engine_state]);

	memory_copy(
		g_arenas.get_item(memory_arena_game_state)->get_base(),
		static_cast<const void*>(static_cast<const byte*>(block) + k_arena_sizes[memory_arena_engine_state]),
		k_arena_sizes[memory_arena_game_state]);
}
#endif //FEATURE_REPLAY