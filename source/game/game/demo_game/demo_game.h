#ifndef __DEMO_GAME_H__
#define __DEMO_GAME_H__
#pragma once

#include "game/game.h"

class c_demo_game : public c_game
{
public:
	void init(const s_game_memory& game_memory);
	void update(const s_input_state const_ptr input_state, real32 dt);

#ifdef HOT_RELOAD
	void reload(const s_game_memory& game_memory);
#endif // HOT_RELOAD
};
#endif // !__DEMO_GAME_H__
