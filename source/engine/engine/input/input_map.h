#ifndef __INPUT_MAP_H__
#define __INPUT_MAP_H__
#pragma once

#include "types/types.h"

enum e_input_keycode : int16
{
	input_key_invalid = k_invalid,

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
	input_key_special_alt,
	input_key_special_delete,
	input_key_special_backspace,
	input_key_special_tab,
	input_key_special_esc,

	input_mouse_left,
	input_mouse_middle,
	input_mouse_right,

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

e_input_keycode get_key_code_from_platform_key(uint64 platform_key);

#endif //__INPUT_MAP_H__