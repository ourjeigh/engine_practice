#ifndef __ENGINE_H__
#define __ENGINE_H__
#pragma once

class c_engine
{
public:
	void init();
	void term();
	// hack, remove
	//void update();
};

void engine_init();
void engine_term();
#endif //__ENGINE_H__