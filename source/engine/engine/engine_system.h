#ifndef __ENGINE_SYSTEM_H__
#define __ENGINE_SYSTEM_H__
#pragma once

#include "types/types.h"
#include "engine_api.h"
#include "debug/asserts.h"

template<typename t_system>
pure_virtual_class c_engine_system
{
public:
	virtual void init() = 0;
	virtual void term() = 0;
	virtual void update() = 0;

	static_member_function t_system& get()
	{
		ASSERT(m_instance);
		return *m_instance;
	}

	static_member_function const t_system& get_const()
	{
		ASSERT(m_instance);
		return *m_instance;
	}

	static_member_function bool is_valid()
	{
		return m_instance != nullptr;
	}

protected:
	c_engine_system()
	{
		ASSERT(m_instance == nullptr);
		m_instance = static_cast<t_system*>(this);
	}

	~c_engine_system()
	{
		m_instance = nullptr;
	}

private:
	inline static_member_data t_system* m_instance = nullptr;
};

void engine_systems_init();
void engine_systems_term();
void engine_systems_pregame_update();
void engine_systems_postgame_update();

#endif //__ENGINE_SYSTEM_H__