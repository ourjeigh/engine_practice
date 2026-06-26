#ifndef __INPUT_TYPES_H__
#define __INPUT_TYPES_H__
#pragma once

#include "types/time_types.h"
#include "structures/array.h"

enum e_input_keycode : int16
{
	input_key_invalid = k_invalid,
	
	input_key_arrow_up,
	input_key_arrow_down,
	input_key_arrow_left,
	input_key_arrow_right,

	input_key_num_0,
	input_key_num_1,
	input_key_num_2,
	input_key_num_3,
	input_key_num_4,
	input_key_num_5,
	input_key_num_6,
	input_key_num_7,
	input_key_num_8,
	input_key_num_9,

	input_key_char_a,
	input_key_char_b,
	input_key_char_c,
	input_key_char_d,
	input_key_char_e,
	input_key_char_f,
	input_key_char_g,
	input_key_char_h,
	input_key_char_i,
	input_key_char_j,
	input_key_char_k,
	input_key_char_l,
	input_key_char_m,
	input_key_char_n,
	input_key_char_o,
	input_key_char_p,
	input_key_char_q,
	input_key_char_r,
	input_key_char_s,
	input_key_char_t,
	input_key_char_u,
	input_key_char_v,
	input_key_char_w,
	input_key_char_x,
	input_key_char_y,
	input_key_char_z,

	input_key_f1,
	input_key_f2,
	input_key_f3,
	input_key_f4,
	input_key_f5,
	input_key_f6,
	input_key_f7,
	input_key_f8,
	input_key_f9,
	input_key_f10,
	input_key_f11,
	input_key_f12,

	input_key_special_shift,
	input_key_special_control,
	input_key_special_return,
	input_key_special_alt,
	input_key_special_delete,
	input_key_special_backspace,
	input_key_special_tab,
	input_key_special_esc,

	input_mouse_left,
	input_mouse_middle,
	input_mouse_right,

	k_input_key_first_arrow = input_key_arrow_up,
	k_input_key_last_arrow = input_key_arrow_right,

	k_input_key_first_num = input_key_num_0,
	k_input_key_last_num = input_key_num_9,

	k_input_key_first_char = input_key_char_a,
	k_input_key_last_char = input_key_char_z,

	k_input_key_first_function = input_key_f1,
	k_input_key_last_function = input_key_f12,

	k_input_key_first_special = input_key_special_shift,
	k_input_key_last_special = input_key_special_esc,

	k_input_first_mouse = input_mouse_left,
	k_input_last_mouse = input_mouse_right,

	k_input_key_count
};
// todo: ^^ make a t_key_code_flags for this ^^

struct s_key_state
{
	bool is_down;
	s_time_span time_in_state;
};

struct s_mouse_position_state
{
	int32 x;
	int32 y;
	t_timestamp last_changedtimestamp;
};

struct s_mouse_scroll_state
{
	int32 position;
	t_timestamp last_changedtimestamp;
};

// button state is in key state
struct s_mouse_state
{
	s_mouse_position_state position;
	s_mouse_scroll_state horizontal_scroll;
	s_mouse_scroll_state vertical_scroll;
};

struct s_input_state
{
	c_static_array<s_key_state, k_input_key_count> key_states;
	s_mouse_state mouse_state;

	const s_key_state get_key_state(e_input_keycode key) const
	{
		return key_states[key];
	}

	const s_mouse_state get_mouse_state() const
	{
		return mouse_state;
	}

	void clear()
	{
		key_states.zero_data();
		zero_object(mouse_state);
	}
};
#endif // !__INPUT_TYPES_H__