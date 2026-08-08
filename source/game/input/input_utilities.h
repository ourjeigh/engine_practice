#ifndef __INPUT_UTILITIES_H
#define __INPUT_UTILITIES_H
#pragma once

#include "types/input_types.h"
#include "structures/vector.h"

inline void get_arrow_key_move_delta(const s_input_state const_ptr input_state, t_vector_4d_real32& out_move_delta)
{
	out_move_delta.set_zero();

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

	if (input_state->get_key_state(input_mouse_left).is_down)
	{
		out_move_delta.z() += 1;
	}
	else if (input_state->get_key_state(input_mouse_right).is_down)
	{
		out_move_delta.z() -= 1;
	}

	out_move_delta.normalize();
}

inline void get_wads_key_move_delta(const s_input_state const_ptr input_state, t_vector_4d_real32& out_move_delta)
{
	out_move_delta.set_zero();

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

/// <summary>
/// Shift + Num 1-6 sets rotation.
/// 1 - Forward | 2 - Back | 3 - Up | 4 - Down | 5 - Left | 6 - Right
/// </summary>
inline bool try_update_camera_direction_from_input(const s_input_state const_ptr input_state, t_vector_4d_real32& in_out)
{
	bool updated = false;
	if (input_state->get_key_state(input_key_special_shift).is_down)
	{
		if (input_state->get_key_state(input_key_num_1).is_down)
		{
			in_out = k_vector_4d_rotation_forward;
			updated = true;
		}
		else if (input_state->get_key_state(input_key_num_2).is_down)
		{
			in_out = k_vector_4d_rotation_backward;
			updated = true;
		}
		else if (input_state->get_key_state(input_key_num_3).is_down)
		{
			in_out = k_vector_4d_rotation_up;
			updated = true;
		}
		else if (input_state->get_key_state(input_key_num_4).is_down)
		{
			in_out = k_vector_4d_rotation_down;
			updated = true;
		}
		else if (input_state->get_key_state(input_key_num_5).is_down)
		{
			in_out = k_vector_4d_rotation_left;
			updated = true;
		}
		else if (input_state->get_key_state(input_key_num_6).is_down)
		{
			in_out = k_vector_4d_rotation_right;
			updated = true;
		}
	}

	return updated;
}

#endif // !__INPUT_UTILITIES_H
