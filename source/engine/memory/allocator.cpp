#include "allocator.h"

inline uint64 align_address(uint64 address, uint64 align)
{
	const uint64 mask = (align - 1);
	ASSERT((align & mask) == 0);
	return (address + mask) & ~mask;
}

inline void* align_pointer(void* pointer, uint64 align)
{
	const uint64 address = reinterpret_cast<uint64>(pointer);
	const uint64 aligned = align_address(address, align);
	return reinterpret_cast<void*>(aligned);
}

c_stack_allocator::c_stack_allocator(void* memory, uint64 size)
{
	m_base = reinterpret_cast<uint64>(memory);
	m_top = m_base;
	m_end = m_base + size;
}

void* c_stack_allocator::allocate(uint64 size, uint64 align)
{
	uint64 aligned_allocation = align_address(m_top, align);
	uint64 new_top = aligned_allocation + size;

	ASSERT(new_top <= m_end);

	m_top = new_top;

	return reinterpret_cast<void*>(aligned_allocation);
}

void* c_stack_allocator::allocate(uint64 size, uint64 align, t_stack_allocator_marker& out_marker)
{
	out_marker = m_top;
	return allocate(size, align);
}

bool c_stack_allocator::is_marker_valid(t_stack_allocator_marker marker) const
{
	// the top is not a valid marker, you cannot "free" to the top
	return m_base <= marker && marker < m_top;
}

void c_stack_allocator::free_to_marker(t_stack_allocator_marker marker)
{
	ASSERT(is_marker_valid(marker));
	m_top = marker;
}

void c_stack_allocator::clear()
{
	m_top = m_base;
}
