#ifndef __INPUT_SYSTEM_H__
#define __INPUT_SYSTEM_H__
#pragma once

#include "engine/engine_system.h"
#include "types/types.h"
#include "types/input_types.h"
#include "time/time.h"
#include "platform/platform_input.h"
#include "events/events.h"
#include "events/delegates.h"
#include "structures/array.h"

typedef uint16 t_message_id;
typedef uint64 t_param;

const real64 k_input_keydown_allowance_seconds = 0.01f;

struct s_input_event_mouse_data
{
	// does mouse data need to track button down?
	// it might make more sense to treat buttons as keys
	int32 x;
	int32 y;
};

struct s_input_event_key_data
{
	e_input_keycode key;
	uint16 repeat_count;
	bool down;
};

struct s_input_event_controller_data
{
	// literally who knows
};

struct s_input_event : s_event
{
	e_event_category get_category() { return window_event_type_input; }
};

struct s_input_key_event : s_input_event
{
	e_event_type get_type() { return event_type_input_key; }

	s_input_event_key_data data;
};

struct s_input_mouse_event : s_input_event
{
	e_event_type get_type() { return event_type_input_mouse; }

	s_input_event_mouse_data data;
};

struct s_input_controller_event : s_input_event
{
	e_event_type get_type() { return event_type_input_controller; }

	s_input_event_controller_data data;
};

enum e_input_event_key_type : byte
{
	input_event_key_type_none,
	input_event_key_type_num,
	input_event_key_type_char,
	input_event_key_type_special,
};

enum e_input_event_key_special : byte
{
	input_event_key_special_shift,
	input_event_key_special_control,
	input_event_key_special_alt,
	input_event_key_special_esc,
};

struct s_key_combo
{
	// todo, make this a t_key_code_flags
	c_static_stack<e_input_keycode, 8> keys;
};

using t_key_combo_callback = void(bool);

struct s_key_combo_delegate
{
	bool down;
	s_key_combo combo;
	c_delegate<t_key_combo_callback> callback;
};

/// <summary>
/// key combo thoughts:
/// - a system can subscribe to a key combo to be notified when all keys in the combo are either up or down
/// - if a system wants to subscribe to a key combo, it probably wants to know both when the key combo is fully down
/// as well as when it's fully up.
/// - we almost certainly just want edge triggering
/// </summary>

#ifdef CONFIG_DEBUG
enum e_input_replay_state
{
	replay_state_none,
	replay_state_recording,
	replay_state_playback,

	k_input_replay_state_count
};
#endif // CONFIG_DEBUG


class c_input_system : public c_engine_system<c_input_system>
{
public:
	virtual void init() override;
	virtual void term() override;
	virtual void update() override;

	template<typename... t_type>
	static_member_function void add_key_combo_callback(c_delegate<t_key_combo_callback> callback, t_type... keys)
	{
		ASSERT(!get().m_key_combo_callbacks.full());

		s_key_combo_delegate& new_combo = get().m_key_combo_callbacks.push();
		new_combo.callback = callback;
		new_combo.down = false;
		new_combo.combo.keys.clear();

		(new_combo.combo.keys.push(keys), ...);
	}

private:
	c_static_stack<s_key_combo_delegate, 32> m_key_combo_callbacks;
};

void input_system_handle_event(s_event& event);

s_key_state input_system_get_key_state(e_input_keycode key);
void input_system_consume_key_state(e_input_keycode key);
const s_mouse_state* input_system_get_mouse_state();

const s_input_state* input_system_get_current_input_state();

template<typename... e_input_keycode>
inline void input_system_add_key_combo_callback(c_delegate<t_key_combo_callback> callback, e_input_keycode... keys)
{
	c_input_system::get().add_key_combo_callback(callback, keys...);
}
#endif//__INPUT_SYSTEM_H__