#ifndef __STATE_MACHINE_H__
#define __STATE_MACHINE_H__
#pragma once

#include "memory/memory.h"
#include "structures/string/string_id.h"
#include "types/types.h"
#include "engine_api.h"

class c_game_state_machine_state_base;

const int32 k_state_machine_state_data_size_bytes = k_byte_mib;

enum e_state_machine_phase
{
	state_machine_phase_pre_enter,
	state_machine_phase_enter,
	state_machine_phase_in_state,
	state_machine_phase_exit,
	state_machine_phase_post_exit,
	state_machine_phase_finished,

	k_state_machine_phase_count
};

struct s_game_flow_state_machine_data
{
	c_string_id current_state_id;
	e_state_machine_phase phase;
	real32 pre_post_wait;
	byte current_state_data[k_state_machine_state_data_size_bytes];
};

// state machine state
class c_game_state_machine_state_base
{
public:
	virtual real32 pre_enter_wait_seconds() { return 0.0f; };
	virtual real32 post_exit_wait_seconds() { return 0.0f; };
	virtual void enter(byte* state_data, real32 dt, bool& out_continue) = 0;
	virtual void update(byte* state_data, real32 dt, bool& out_continue) = 0;
	virtual void exit(byte* state_data, real32 dt, bool& out_continue) = 0;

	virtual c_string_id state_id() const = 0;
	virtual const char* debug_state_name() const = 0;

	template<typename t_child_type>
	bool is_type() const
	{
		return dynamic_cast<const t_child_type*>(this) != nullptr;
	}
};

template<typename t_state_data_type>
class c_game_state_machine_state : public c_game_state_machine_state_base
{
	COMPILE_ASSERT(sizeof(t_state_data_type) <= k_state_machine_state_data_size_bytes);

	void enter(byte* state_data, real32 dt, bool& out_continue) final
	{
		on_enter(make_type_data(state_data), dt, out_continue);
	}

	void update(byte* state_data, real32 dt, bool& out_continue) final
	{
		on_update(make_type_data(state_data), dt, out_continue);
	}

	void exit(byte* state_data, real32 dt, bool& out_continue) final
	{
		on_exit(make_type_data(state_data), dt, out_continue);
	}

protected:
	virtual void on_enter(t_state_data_type* state_data, real32 dt, bool& out_continue) = 0;
	virtual void on_update(t_state_data_type* state_data, real32 dt, bool& out_continue) = 0;
	virtual void on_exit(t_state_data_type* state_data, real32 dt, bool& out_continue) = 0;

private:
	t_state_data_type* make_type_data(byte* state_data)
	{
		return reinterpret_cast<t_state_data_type*>(state_data);
	}
};

// state machine
class c_state_machine_fsm
{
public:
	void init(s_game_flow_state_machine_data* data)
	{
		data->phase = state_machine_phase_pre_enter;
		data->pre_post_wait = 0.0f;
		data->current_state_id = get_next_state_id(data);
	}

	void init_with_state(s_game_flow_state_machine_data* data, c_string_id state_id)
	{
		init(data);
		set_state_id(data, state_id);
	}

	void update(s_game_flow_state_machine_data* data, real32 dt, bool& out_continue)
	{
		bool state_continue = true;

		c_string_id last_state_id = "";
		c_game_state_machine_state_base* current_state = get_state(data->current_state_id);

		if (current_state)
		{
			last_state_id = current_state->state_id();

			switch (data->phase)
			{
			case state_machine_phase_pre_enter:
			{
				handle_wait_internal(
					data,
					current_state->pre_enter_wait_seconds(),
					dt,
					state_machine_phase_enter); 
				break;
			}
			case state_machine_phase_enter:
			{
				current_state->enter(data->current_state_data, dt, state_continue);
				if (!state_continue)
				{
					data->phase = state_machine_phase_in_state;
				}
				break;
			}
			case state_machine_phase_in_state:
			{
				current_state->update(data->current_state_data, dt, state_continue);
				if (!state_continue)
				{
					data->phase = state_machine_phase_exit;
				}
				break;
			}
			case state_machine_phase_exit:
			{
				current_state->exit(data->current_state_data, dt, state_continue);
				if (!state_continue)
				{
					data->phase = state_machine_phase_post_exit;
				}
				break;
			}
			case state_machine_phase_post_exit:
			{
				handle_wait_internal(
					data,
					current_state->post_exit_wait_seconds(),
					dt,
					state_machine_phase_finished);
				break;
			}
			case state_machine_phase_finished:
			{
				data->current_state_id = get_next_state_id(data);
				data->phase = state_machine_phase_pre_enter;
				break;
			}
			}
		}
		else
		{
			out_continue = false;
		}

		if (last_state_id != data->current_state_id)
		{
			zero_object(data->current_state_data);
			engine_log_verbose("game_flow: state changed from {s} to {s}",
				last_state_id.get_debug_string(),
				current_state->debug_state_name());
		}
	}

	void term()
	{
	}

	virtual void set_state_id(s_game_flow_state_machine_data* data, c_string_id state_id) = 0;
	virtual c_string_id get_next_state_id(const s_game_flow_state_machine_data* data) const = 0;
	virtual c_game_state_machine_state_base* get_state(const c_string_id& state_id) = 0;

protected:

private:
	void handle_wait_internal(
		s_game_flow_state_machine_data* data,
		real32 wait_time_seconds,
		real32 dt,
		e_state_machine_phase wait_complete_phase)
	{
		if (wait_time_seconds > 0)
		{
			real32 delta = dt / wait_time_seconds;
			data->pre_post_wait += delta;
		}

		if (data->pre_post_wait >= wait_time_seconds)
		{
			data->pre_post_wait = 0.0f;
			data->phase = wait_complete_phase;
		}
	}
};
#endif // !__STATE_MACHINE_H__
