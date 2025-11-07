#ifndef __INPUT_SYSTEM_H__
#define __INPUT_SYSTEM_H__
#pragma once

#include "engine/engine_system.h"
#include <types/types.h>
#include <time/time.h>
#include "input_map.h"

typedef uint16 t_message_id;
typedef uint64 t_param;

const real64 k_input_keydown_allowance_seconds = 0.01f;

enum e_input_event_type : byte
{
	_input_event_type_none,
	_input_event_type_key_down,
	_input_event_type_key_up,
};

enum e_input_event_key_type : byte
{
	_input_event_key_type_none,
	_input_event_key_type_num,
	_input_event_key_type_char,
	_input_event_key_type_special,
};

enum e_input_event_key_special : byte
{
	_input_event_key_special_shift,
	_input_event_key_special_control,
	_input_event_key_special_alt,
	_input_event_key_special_esc,
};

struct s_input_queued_event
{
	t_message_id message_id;
	t_param param;
	t_timestamp timestamp;
};

class c_key_state
{
public:
	c_key_state() :
		m_is_down(false),
		m_last_changed_timestamp(0),
		m_key_type(_input_event_key_type_none),
		u_keycode_num(0)
	{
	}

	bool is_down() const 
	{ 
		if (m_is_down) return true;

		auto span = get_time_since(m_last_changed_timestamp);
		real64 duration_seconds = span.get_duration_seconds();

		return  (duration_seconds < k_input_keydown_allowance_seconds);
	}
	
	t_timestamp get_last_changed_timestamp() const { return m_last_changed_timestamp; }

	void set_key_state(bool is_down, t_timestamp timestamp)
	{
		if (is_down != m_is_down)
		{
			m_is_down = is_down;
			m_last_changed_timestamp = timestamp;
		}
	}

private:
	bool m_is_down;
	t_timestamp m_last_changed_timestamp;

	e_input_event_key_type m_key_type;
	union
	{
		uint8 u_keycode_num;
		char u_keycode_char;
		e_input_event_key_special r_keycode_special;
	};
};

class c_input_system : public c_engine_system
{
public:
	virtual void init() override;
	virtual void term() override;
	virtual void update() override;

};

bool input_system_queue_message(t_message_id message_id, t_param param);
const c_key_state* input_system_get_key_state(e_input_keycode key);
#endif//__INPUT_SYSTEM_H__