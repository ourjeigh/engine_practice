#include "platform/platform.h"
#ifdef PLATFORM_WIN64

#include "platform/platform_memory.h"
#include "platform/win64/win64_includes.h"


void* platform_memory_allocate(uint32 size)
{
    LPVOID base = 0;

    LPVOID out = VirtualAlloc(
        base,
        size,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE);
	
    return out;
}

#endif //PLATFORM_WIN64