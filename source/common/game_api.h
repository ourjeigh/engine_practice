#ifndef __GAME_API_H__
#define __GAME_API_H__
#pragma once

#include "debug/assert_handler.h"

#ifdef HOT_RELOAD
#define GAME_API __declspec(dllexport)
#else
#define GAME_API
#endif //HOT_RELOAD

class i_engine;

struct c_game_state
{
	i_engine* engine;
	f_assert_handler assert_hook;
};

typedef void(*f_game_init)(c_game_state& game_state);
typedef void(*f_game_update)();

#endif // !__GAME_API_H__