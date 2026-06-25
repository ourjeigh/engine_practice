#ifndef __INPUT_UTILITIES_H
#define __INPUT_UTILITIES_H
#pragma once
#include "types/input_types.h"
#include "structures/vector.h"

inline void get_arrow_key_move_delta(const s_input_state const_ptr input_state, t_vector_4d_real32& out_move_delta)
{
	out_move_delta.zero();

	if (input_state->get_key_state(input_key_arrow_up).is_down)
	{
		out_move_delta.y() += 1;
	}

	if (input_state->get_key_state(input_key_arrow_down).is_down)
	{
		out_move_delta.y() -= 1;
	}

	if (input_state->get_key_state(input_key_arrow_left).is_down)
	{
		out_move_delta.x() -= 1;
	}

	if (input_state->get_key_state(input_key_arrow_right).is_down)
	{
		out_move_delta.x() += 1;
	}

	out_move_delta.normalize();
}

inline void get_wads_key_move_delta(const s_input_state const_ptr input_state, t_vector_4d_real32& out_move_delta)
{
	out_move_delta.zero();

	if (input_state->get_key_state(input_key_char_w).is_down)
	{
		out_move_delta.y() += 1;
	}

	if (input_state->get_key_state(input_key_char_s).is_down)
	{
		out_move_delta.y() -= 1;
	}

	if (input_state->get_key_state(input_key_char_a).is_down)
	{
		out_move_delta.x() -= 1;
	}

	if (input_state->get_key_state(input_key_char_d).is_down)
	{
		out_move_delta.x() += 1;
	}

	out_move_delta.normalize();
}

#endif // !__INPUT_UTILITIES_H
