#ifndef __GAME_API_H__
#define __GAME_API_H__
#pragma once

class c_game_base
{
public:
	virtual void init() = 0;
	virtual void term() = 0;
	virtual void update() = 0;
};

#endif // !__GAME_API_H__