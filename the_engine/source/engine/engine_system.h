#ifndef __ENGINE_SYSTEM_H__
#define __ENGINE_SYSTEM_H__
#pragma once

// inherit from this to make an engine
class c_engine_system
{
public:

	virtual void init() = 0;
	virtual void term() = 0;
	virtual void update() = 0;

protected:
	c_engine_system() {}
	~c_engine_system() {}
};

void engine_systems_init();
void engine_systems_term();
void engine_systems_update();

#endif //__ENGINE_SYSTEM_H__