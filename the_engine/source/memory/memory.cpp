#include "asserts.h"
#include "memory/memory.h"
#include <types/types.h>

//c_handle::c_handle(c_handle&& other)
//{
//}
//
//c_handle& c_handle::operator=(c_handle&& other)
//{
//	return *this;
//}
//
//void* c_handle::get_pointer()
//{
//	return nullptr;
//}
//
//const void* c_handle::get_pointer_const()
//{
//	return nullptr;
//}

void memory_zero(void* obj, size_t size)
{
	if (size % 8 == 0)
	{
		memory_set(obj, k_int64_zero, size);
	}
	else if (size % 4 == 0)
	{
		memory_set(obj, k_int32_zero, size);
	}
	else if (size % 2 == 0)
	{
		memory_set(obj, k_int16_zero, size);
	}
	else
	{
		memory_set(obj, k_int8_zero, size);
	}
}

void memory_copy(void* dest, void const* src, size_t size)
{
	ASSERT(dest != nullptr);
	ASSERT(src != nullptr);
	
	char* dst_ptr = static_cast<char*>(dest);
	const char* src_ptr = static_cast<const char*>(src);
	ASSERT(((dst_ptr + size) <= src_ptr) || ((src_ptr + size) <= dst_ptr));

	size_t size_remaining = size;

	while (size_remaining >= sizeof(uint64))
	{
		// I feel like we should be able to do this without the intermediary char cast, but I can't get it working yet
		*reinterpret_cast<uint64*>(&dst_ptr[size - size_remaining]) = *reinterpret_cast<const uint64*>(&src_ptr[size - size_remaining]);
		size_remaining -= sizeof(uint64);
	}

	while (size_remaining >= sizeof(uint32))
	{
		*reinterpret_cast<uint32*>(&dst_ptr[size - size_remaining]) = *reinterpret_cast<const uint32*>(&src_ptr[size - size_remaining]);
		size_remaining -= sizeof(uint32);
	}

	while (size_remaining >= sizeof(uint16))
	{
		*reinterpret_cast<uint16*>(&dst_ptr[size - size_remaining]) = *reinterpret_cast<const uint16*>(&src_ptr[size - size_remaining]);
		size_remaining -= sizeof(uint16);
	}

	while (size_remaining > 0)
	{
		*reinterpret_cast<uint8*>(&dst_ptr[size - size_remaining]) = *reinterpret_cast<const uint8*>(&src_ptr[size - size_remaining]);
		size_remaining -= sizeof(uint8);
	}
}

void memory_move(void* dest, void const* src, size_t size)
{
	ASSERT(dest != nullptr);
	ASSERT(src != nullptr);

	char* dst_ptr = static_cast<char*>(dest);
	const char* src_ptr = static_cast<const char*>(src);

	size_t size_remaining = size;

	if (dst_ptr < src_ptr)
	{
		while (size_remaining >= sizeof(uint64))
		{
			*reinterpret_cast<uint64*>(&dst_ptr[size - size_remaining]) = *reinterpret_cast<const uint64*>(&src_ptr[size - size_remaining]);
			size_remaining -= sizeof(uint64);
		}

		while (size_remaining >= sizeof(uint32))
		{
			*reinterpret_cast<uint32*>(&dst_ptr[size - size_remaining]) = *reinterpret_cast<const uint32*>(&src_ptr[size - size_remaining]);
			size_remaining -= sizeof(uint32);
		}

		while (size_remaining >= sizeof(uint16))
		{
			*reinterpret_cast<uint16*>(&dst_ptr[size - size_remaining]) = *reinterpret_cast<const uint16*>(&src_ptr[size - size_remaining]);
			size_remaining -= sizeof(uint16);
		}

		while (size_remaining > 0)
		{
			*reinterpret_cast<uint8*>(&dst_ptr[size - size_remaining]) = *reinterpret_cast<const uint8*>(&src_ptr[size - size_remaining]);
			size_remaining -= sizeof(uint8);
		}
	}
	else if (dst_ptr > src_ptr)
	{
		while (size_remaining >= sizeof(uint64))
		{
			*reinterpret_cast<uint64*>(&dst_ptr[size_remaining - sizeof(uint64)]) = *reinterpret_cast<const uint64*>(&src_ptr[size_remaining - sizeof(uint64)]);
			size_remaining -= sizeof(uint64);
		}

		while (size_remaining >= sizeof(uint32))
		{
			*reinterpret_cast<uint32*>(&dst_ptr[size_remaining - sizeof(uint32)]) = *reinterpret_cast<const uint32*>(&src_ptr[size_remaining - sizeof(uint32)]);
			size_remaining -= sizeof(uint32);
		}

		while (size_remaining >= sizeof(uint16))
		{
			*reinterpret_cast<uint16*>(&dst_ptr[size_remaining - sizeof(uint16)]) = *reinterpret_cast<const uint16*>(&src_ptr[size_remaining - sizeof(uint16)]);
			size_remaining -= sizeof(uint16);
		}

		while (size_remaining > 0)
		{
			*reinterpret_cast<uint8*>(&dst_ptr[size_remaining - sizeof(uint8)]) = *reinterpret_cast<const uint8*>(&src_ptr[size_remaining - sizeof(uint8)]);
			size_remaining -= sizeof(uint8);
		}
	}
}

int32 memory_compare(const void* left, const void* right, size_t size)
{
	while (size > 0)
	{
		byte left_byte = *reinterpret_cast<const byte*>(left);
		byte right_byte = *reinterpret_cast<const byte*>(right);
		if (left_byte < right_byte)
		{
			return -1;
		}
		else if (left_byte > right_byte)
		{
			return 1;
		}
		size--;
	}

	return 0;
}
