#include "replay.h"
#ifdef FEATURE_REPLAY

#include "engine/input/input_system.h"
#include "logging.h"
#include "memory/memory_system.h"
#include "rendering/render_system.h"

const e_input_keycode k_record_key = input_key_special_home;
const e_input_keycode k_replay_key = input_key_special_end;
const c_file_path k_replay_file_path = "replay.reverse";

struct s_replay_system_state
{
	e_replay_state state;
	c_file replay_file;
	int32 file_position; // both read and write
	int32 frame;
	s_replay_info replay_info;
};

static_global void set_replay_state_internal(e_replay_state new_state);
static_global void update_debug_internal();

// not going thru allocator since that memory is what we'll be manipulating with replay
s_replay_system_state g_replay_system_state;

void c_replay_system::init()
{
	g_replay_system_state.state = replay_state_none;
}

void c_replay_system::term()
{
}

void c_replay_system::update()
{
	// check inputs for state change
	if (input_system_get_key_state(k_record_key).is_down)
	{
		switch (get_replay_state())
		{
		case replay_state_none:
			start_record_internal();
			break;
		case replay_state_recording:
			stop_record_internal();
			break;
		case replay_state_playback:
			log_message(warning, "c_replay_system: record triggered while replay active, ignoring.");
			break;
		default:
			HALT_UNIMPLEMENTED();
		}

		input_system_consume_key_state(k_record_key);
	}
	else if (input_system_get_key_state(k_replay_key).is_down)
	{
		switch (get_replay_state())
		{
		case replay_state_none:
			start_replay_internal();
			break;
		case replay_state_recording:
			log_message(warning, "c_replay_system: replay triggered while recordin active, ignoring.");
			break;
		case replay_state_playback:
			stop_replay_internal();
			break;
		default:
			HALT_UNIMPLEMENTED();
		}

		input_system_consume_key_state(k_replay_key);
	}

	// process current state
	switch (get_replay_state())
	{
	case replay_state_recording:
		update_record_internal();
		break;
	case replay_state_playback:
		update_replay_internal();
		break;
	case replay_state_none:
	default:
		break;
	}

	update_debug_internal();
}

e_replay_state c_replay_system::get_replay_state()
{
	return g_replay_system_state.state;
}

// private
void set_replay_state_internal(e_replay_state new_state)
{
	if (g_replay_system_state.state != new_state)
	{
		g_replay_system_state.state = new_state;
	}
}

void c_replay_system::start_record_internal()
{
	log_message(verbose, "c_replay_system: start record");

	ASSERT(!g_replay_system_state.replay_file.is_open());

	uint64& memory_size = g_replay_system_state.replay_info.memory_size;
	const void* memory_block = c_memory_system::get_memory_block_for_replay_read(memory_size);

	t_file_open_mode_flags flags(file_open_mode_write, file_open_mode_replace);
	g_replay_system_state.replay_file.open(k_replay_file_path, flags);

	// reserve block to write replay info at the front
	g_replay_system_state.file_position = sizeof(s_replay_info);
	
	g_replay_system_state.replay_file.write_memory(
		g_replay_system_state.file_position, 
		memory_block, 
		memory_size);

	g_replay_system_state.file_position += memory_size;
	g_replay_system_state.frame = 0;

	set_replay_state_internal(replay_state_recording);
}

void c_replay_system::stop_record_internal()
{
	log_message(verbose, "c_replay_system: stop record");
	ASSERT(g_replay_system_state.replay_file.is_open());
	
	g_replay_system_state.replay_info.frame_count = g_replay_system_state.frame;

	// write the replay to the beginning of the file so we can easily grab it before playback
	g_replay_system_state.replay_file.write_memory(
		0,
		&g_replay_system_state.replay_info,
		sizeof(s_replay_info));

	g_replay_system_state.replay_file.close();

	zero_object(g_replay_system_state.replay_info);
	g_replay_system_state.frame = 0;

	set_replay_state_internal(replay_state_none);
}

void c_replay_system::update_record_internal()
{
	ASSERT(g_replay_system_state.replay_file.is_open());

	s_input_state input_state = *input_system_get_current_input_state();

	// we don't want the record/replay toggle keys getting recorded since it would
	// interrupt the replay/record functionality itself
	input_state.key_states.get_item(k_record_key)->is_down = false;
	input_state.key_states.get_item(k_replay_key)->is_down = false;

	g_replay_system_state.replay_file.write_memory(
		g_replay_system_state.file_position,
		&input_state,
		sizeof(s_input_state));

	g_replay_system_state.file_position += sizeof(s_input_state);

	//g_replay_system_state.replay_info.frame_count++;
	g_replay_system_state.frame++;
}

void c_replay_system::start_replay_internal()
{
	log_message(verbose, "c_replay_system: start replay");
	ASSERT(!g_replay_system_state.replay_file.is_open());
	
	t_file_open_mode_flags flags(file_open_mode_read);
	g_replay_system_state.replay_file.open(k_replay_file_path, flags);

	g_replay_system_state.replay_file.read_memory(
		0,
		sizeof(s_replay_info),
		&g_replay_system_state.replay_info);

	g_replay_system_state.file_position = sizeof(s_replay_info);

	const uint64 memory_size = g_replay_system_state.replay_info.memory_size;
	g_replay_system_state.replay_file.read_memory(
		g_replay_system_state.file_position,
		memory_size,
		c_memory_system::get_memory_block_for_replay_write(memory_size));

	g_replay_system_state.file_position += memory_size;

	g_replay_system_state.frame = 0;

	set_replay_state_internal(replay_state_playback);
}

void c_replay_system::stop_replay_internal()
{
	log_message(verbose, "c_replay_system: stop replay");
	ASSERT(g_replay_system_state.replay_file.is_open());

	g_replay_system_state.replay_file.close();
	set_replay_state_internal(replay_state_none);
}

void c_replay_system::update_replay_internal()
{
	ASSERT(g_replay_system_state.replay_file.is_open());
	ASSERT(g_replay_system_state.file_position < g_replay_system_state.replay_file.file_size());

	g_replay_system_state.replay_file.read_memory(
		g_replay_system_state.file_position,
		sizeof(s_input_state),
		c_input_system::get_input_state_for_write());

	g_replay_system_state.file_position += sizeof(s_input_state);

	if (++g_replay_system_state.frame == g_replay_system_state.replay_info.frame_count)
	{
		stop_replay_internal();
		start_replay_internal();
	}
}

void update_debug_internal()
{
	const int32 debug_position_x = 240;
	const int32 debug_position_y = 5;

	t_string_256 debug_string;

	switch (c_replay_system::get_replay_state())
	{
	case replay_state_recording:
		debug_string.printf("Recording: frame {i}", g_replay_system_state.frame);
		break;
	case replay_state_playback:
		debug_string.printf("Replay: frame {i}/{i}",
			g_replay_system_state.frame,
			g_replay_system_state.replay_info.frame_count);
		break;
	case replay_state_none:
	default:
		break;
	}

	if (!debug_string.empty())
	{
		render_system_draw_string(
			debug_string,
			debug_position_x,
			debug_position_y,
			1.0f,
			k_color_white,
			render_layer_debug);
	}
}

#endif //FEATURE_REPLAY