#ifndef __GAME_API_H__
#define __GAME_API_H__
#pragma once

#include "types/types.h"
#include "debug/assert_handler.h"

#ifdef HOT_RELOAD
#define GAME_API __declspec(dllexport)
#else
#define GAME_API
#endif //HOT_RELOAD

class i_engine;

struct s_game_memory
{
	void* data;
	int32 size;
};

struct s_game_engine_context
{
	i_engine* engine;
	f_assert_handler assert_hook;
	s_game_memory memory;
};

typedef void(*f_game_init)(s_game_engine_context& engine_context);
typedef void(*f_game_update)();
#ifdef HOT_RELOAD
typedef void(*f_game_reload)(s_game_engine_context& engine_context);
#endif //HOT_RELOAD

#endif // !__GAME_API_H__