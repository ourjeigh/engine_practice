#ifndef __GAME_H__
#define __GAME_H__
#pragma once

#include "game_api.h"

class c_game
{
public:
	virtual void init(const s_game_memory& game_memory) = 0;
	virtual void update(const s_input_state const_ptr input_state, real32 dt) = 0;

#ifdef HOT_RELOAD
	virtual void reload(const s_game_memory& game_memory) = 0;
#endif // HOT_RELOAD
};
#endif // !__GAME_H__

