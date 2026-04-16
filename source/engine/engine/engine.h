#ifndef __ENGINE_H__
#define __ENGINE_H__
#pragma once

#include <memory/memory.h>
#include <memory/allocator.h>


//class c_engine
//{
//public:
//	void init();
//	void term();
//	void update();
//};

//void engine_init();
//void engine_term();

// TBD if this is the best place for it...
// Note: once you get memory, you can try to give it back, but it probably won't work.
//c_stack_allocator* get_global_allocator();

// TODO: some kind of macro ALLOCATE_GLOBAL_MEMORY that takes in the object being created, and captures the file/function allocating it
// will require some additional debug args in the allocator, plust tracking of course.
// eg:
//#define ALLOCATE_GLOBAL_MEMORY(x, area) get_global_allocator()->allocate(sizeof(x), alignof(x), area, __func__, __FILE__)

#endif //__ENGINE_H__