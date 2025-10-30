#ifndef __INPUT_MAP_H__
#define __INPUT_MAP_H__
#pragma once

#include "types/types.h";
#include "asserts.h"
#include <Windows.h>

const uint16 k_input_id_key_down =		WM_KEYDOWN;
const uint16 k_input_id_key_up =		WM_KEYUP;
const uint16 k_input_id_char =			WM_CHAR;
const uint16 k_input_id_dead_char =		WM_DEADCHAR; // a dead key that is pressed while holding down the ALT key.
const uint16 k_input_id_sys_key_down =	WM_SYSKEYDOWN;
const uint16 k_input_id_sys_key_up =	WM_SYSKEYUP;
const uint16 k_input_id_sys_char =		WM_SYSCHAR;
const uint16 k_input_id_sys_dead_char = WM_SYSDEADCHAR; // a dead key that is pressed while holding down the ALT key.

const uint16 k_input_id_kbd_0 = '0';
const uint16 k_input_id_kbd_1 = '1';
const uint16 k_input_id_kbd_2 = '2';
const uint16 k_input_id_kbd_3 = '3';
const uint16 k_input_id_kbd_4 = '4';
const uint16 k_input_id_kbd_5 = '5';
const uint16 k_input_id_kbd_6 = '6';
const uint16 k_input_id_kbd_7 = '7';
const uint16 k_input_id_kbd_8 = '8';
const uint16 k_input_id_kbd_9 = '9';

const uint16 k_input_id_kbd_a = 'A';
const uint16 k_input_id_kbd_b = 'B';
const uint16 k_input_id_kbd_c = 'C';
const uint16 k_input_id_kbd_d = 'D';
const uint16 k_input_id_kbd_e = 'E';
const uint16 k_input_id_kbd_f = 'F';
const uint16 k_input_id_kbd_g = 'G';
const uint16 k_input_id_kbd_h = 'H';
const uint16 k_input_id_kbd_i = 'I';
const uint16 k_input_id_kbd_j = 'J';
const uint16 k_input_id_kbd_k = 'K';
const uint16 k_input_id_kbd_l = 'L';
const uint16 k_input_id_kbd_m = 'M';
const uint16 k_input_id_kbd_n = 'N';
const uint16 k_input_id_kbd_o = 'O';
const uint16 k_input_id_kbd_p = 'P';
const uint16 k_input_id_kbd_q = 'Q';
const uint16 k_input_id_kbd_r = 'R';
const uint16 k_input_id_kbd_s = 'S';
const uint16 k_input_id_kbd_t = 'T';
const uint16 k_input_id_kbd_u = 'U';
const uint16 k_input_id_kbd_v = 'V';
const uint16 k_input_id_kbd_w = 'W';
const uint16 k_input_id_kbd_x = 'X';
const uint16 k_input_id_kbd_y = 'Y';
const uint16 k_input_id_kbd_z = 'Z';

const uint16 k_input_id_kbd_f1 =		VK_F1;
const uint16 k_input_id_kbd_f2 =		VK_F2;
const uint16 k_input_id_kbd_f3 =		VK_F3;
const uint16 k_input_id_kbd_f4 =		VK_F4;
const uint16 k_input_id_kbd_f5 =		VK_F5;
const uint16 k_input_id_kbd_f6 =		VK_F6;
const uint16 k_input_id_kbd_f7 =		VK_F7;
const uint16 k_input_id_kbd_f8 =		VK_F8;
const uint16 k_input_id_kbd_f9 =		VK_F9;
const uint16 k_input_id_kbd_f10 =		VK_F10;
const uint16 k_input_id_kbd_f11 =		VK_F11;
const uint16 k_input_id_kbd_f12 =		VK_F12;

const uint16 k_input_id_kbd_clear =		VK_CLEAR;
const uint16 k_input_id_kbd_return =	VK_RETURN;
const uint16 k_input_id_kbd_shift =		VK_SHIFT;
const uint16 k_input_id_kbd_control =	VK_CONTROL;
const uint16 k_input_id_kbd_menu =		VK_MENU;
const uint16 k_input_id_kbd_pause =		VK_PAUSE;
const uint16 k_input_id_kbd_capital =	VK_CAPITAL;
const uint16 k_input_id_kbd_esc =		VK_ESCAPE;
const uint16 k_input_id_kbd_space =		VK_SPACE;
const uint16 k_input_id_kbd_prior =		VK_PRIOR;
const uint16 k_input_id_kbd_njext =		VK_NEXT;
const uint16 k_input_id_kbd_end =		VK_END;
const uint16 k_input_id_kbd_home =		VK_HOME;
const uint16 k_input_id_kbd_left =		VK_LEFT;
const uint16 k_input_id_kbd_up =		VK_UP;
const uint16 k_input_id_kbd_right =		VK_RIGHT;
const uint16 k_input_id_kbd_down =		VK_DOWN;
const uint16 k_input_id_kbd_select =	VK_SELECT;
const uint16 k_input_id_kbd_prent =		VK_PRINT;
const uint16 k_input_id_kbd_execute =	VK_EXECUTE;
const uint16 k_input_id_kbd_snapshot =	VK_SNAPSHOT;
const uint16 k_input_id_kbd_insert =	VK_INSERT;
const uint16 k_input_id_kbd_delete =	VK_DELETE;
const uint16 k_input_id_kbd_help =		VK_HELP;


const uint16 k_input_id_gamepad_a =				VK_GAMEPAD_A;
const uint16 k_input_id_gamepad_b =				VK_GAMEPAD_B;
const uint16 k_input_id_gamepad_x =				VK_GAMEPAD_X;
const uint16 k_input_id_gamepad_y =				VK_GAMEPAD_Y;
const uint16 k_input_id_gamepad_r1 =			VK_GAMEPAD_RIGHT_SHOULDER;
const uint16 k_input_id_gamepad_l1 =			VK_GAMEPAD_LEFT_SHOULDER;
const uint16 k_input_id_gamepad_l2 =			VK_GAMEPAD_LEFT_TRIGGER;
const uint16 k_input_id_gamepad_r2 =			VK_GAMEPAD_RIGHT_TRIGGER;
const uint16 k_input_id_gamepad_d_up =			VK_GAMEPAD_DPAD_UP;
const uint16 k_input_id_gamepad_d_down =		VK_GAMEPAD_DPAD_DOWN;
const uint16 k_input_id_gamepad_d_left =		VK_GAMEPAD_DPAD_LEFT;
const uint16 k_input_id_gamepad_d_right =		VK_GAMEPAD_DPAD_RIGHT;
const uint16 k_input_id_gamepad_menu =			VK_GAMEPAD_MENU;
const uint16 k_input_id_gamepad_view =			VK_GAMEPAD_VIEW;
const uint16 k_input_id_gamepad_l3 =			VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON;
const uint16 k_input_id_gamepad_r3 =			VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON;
const uint16 k_input_id_gamepad_l_thumb_up =	VK_GAMEPAD_LEFT_THUMBSTICK_UP;
const uint16 k_input_id_gamepad_l_thum_down =	VK_GAMEPAD_LEFT_THUMBSTICK_DOWN;
const uint16 k_input_id_gamepad_l_thumb_right = VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT;
const uint16 k_input_id_gamepad_l_thumb_left =	VK_GAMEPAD_LEFT_THUMBSTICK_LEFT;
const uint16 k_input_id_gamepad_r_thumb_up =	VK_GAMEPAD_RIGHT_THUMBSTICK_UP;
const uint16 k_input_id_gamepad_r_thumb_down =	VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN;
const uint16 k_input_id_gamepad_r_thumb_right = VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT;
const uint16 k_input_id_gamepad_r_thumb_left =	VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT;


// unused
enum e_input_keycode : uint16
{
	_input_key_num_0,
	_input_key_num_1,
	_input_key_num_2,
	_input_key_num_3,
	_input_key_num_4,
	_input_key_num_5,
	_input_key_num_6,
	_input_key_num_7,
	_input_key_num_8,
	_input_key_num_9,
	_input_key_char_a,
	_input_key_char_b,
	_input_key_char_c,
	_input_key_char_d,
	_input_key_char_e,
	_input_key_char_f,
	_input_key_char_g,
	_input_key_char_h,
	_input_key_char_i,
	_input_key_char_j,
	_input_key_char_k,
	_input_key_char_l,
	_input_key_char_m,
	_input_key_char_n,
	_input_key_char_o,
	_input_key_char_p,
	_input_key_char_q,
	_input_key_char_r,
	_input_key_char_s,
	_input_key_char_t,
	_input_key_char_u,
	_input_key_char_v,
	_input_key_char_w,
	_input_key_char_x,
	_input_key_char_y,
	_input_key_char_z,
	_input_key_special_shift,
	_input_key_special_control,
	_input_key_special_alt,
	_input_key_special_esc,

	k_input_key_first_num = _input_key_num_0,
	k_input_key_last_num = _input_key_num_9,

	k_input_key_first_char = _input_key_char_a,
	k_input_key_last_char = _input_key_char_z,

	k_input_key_first_special = _input_key_special_shift,
	k_input_key_last_special = _input_key_special_esc,
};
#endif //__INPUT_MAP_H__