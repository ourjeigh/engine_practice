#include "replay.h"
#ifdef FEATURE_REPLAY

#include "engine/input/input_system.h"
#include "logging.h"
#include <memory/memory_system.h>
#include <platform/platform_memory.h>

struct s_replay_system_state
{
	e_replay_state state;
	c_file replay_file;
	int32 file_position; // both read and write
	uint64 memory_size;
	int32 frame;
	s_replay_info replay_info;
};

void set_replay_state_internal(e_replay_state new_state);
void start_record_internal();
void stop_record_internal();
void start_replay_internal();
void stop_replay_internal();

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
	if (input_system_get_key_state(input_key_special_home).is_down)
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

		input_system_consume_key_state(input_key_special_home);
	}
	else if (input_system_get_key_state(input_key_special_end).is_down)
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

		input_system_consume_key_state(input_key_special_end);
	}

	// process current state
	switch (get_replay_state())
	{
	case replay_state_recording:
		// todo: get current input state and write to file
		g_replay_system_state.replay_info.frame_count++;
		break;
	case replay_state_playback:
		// todo: read next input state from file and send to input system
		break;
	case replay_state_none:
	default:
		break;
	}
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

void start_record_internal()
{
	log_message(verbose, "c_replay_system: start record");

	ASSERT(!g_replay_system_state.replay_file.is_open());

	const void* memory_block;
	c_memory_system::get_memory_block_for_replay(&memory_block, g_replay_system_state.memory_size);

	c_file_path file_path("replay.reverse"); // make const
	
	t_file_open_mode_flags flags(file_open_mode_write, file_open_mode_replace);
	g_replay_system_state.replay_file.open(file_path, flags);

	// reserve block to write replay info at the front
	g_replay_system_state.file_position = sizeof(s_replay_info);
	
	g_replay_system_state.replay_file.write_memory(
		g_replay_system_state.file_position, 
		memory_block, 
		g_replay_system_state.memory_size);

	g_replay_system_state.file_position += g_replay_system_state.memory_size;

	g_replay_system_state.replay_file.write_memory(
		g_replay_system_state.file_position,
		memory_block, 
		g_replay_system_state.memory_size);

	set_replay_state_internal(replay_state_recording);
}

void stop_record_internal()
{
	log_message(verbose, "c_replay_system: stop record");
	ASSERT(g_replay_system_state.replay_file.is_open());
	
	// write the replay to the beginning of the file so we can easily grab it before playback
	g_replay_system_state.replay_file.write_memory(
		0,
		&g_replay_system_state.replay_info,
		sizeof(s_replay_info));

	g_replay_system_state.replay_file.close();

	zero_object(g_replay_system_state.replay_info);

	set_replay_state_internal(replay_state_none);
}

void start_replay_internal()
{
	log_message(verbose, "c_replay_system: start replay");
	ASSERT(!g_replay_system_state.replay_file.is_open());
	
	c_file_path file_path("replay.reverse"); // make const
	t_file_open_mode_flags flags(file_open_mode_read);
	g_replay_system_state.replay_file.open(file_path, flags);

	g_replay_system_state.replay_file.read_memory(
		0,
		sizeof(s_replay_info),
		&g_replay_system_state.replay_info);

	g_replay_system_state.file_position = sizeof(s_replay_info);

	// temp/hack - probably better to obtain the actual arena memory and write directly to it from the file
	const uint64 size = 20 * k_byte_mb;
	void* mem = platform_memory_allocate(size);
	c_array<byte> bytes(static_cast<byte*>(mem), size);

	g_replay_system_state.replay_file.read_bytes(g_replay_system_state.file_position, size, bytes);

	c_memory_system::set_memory_block_for_replay(bytes.data(), size);

	g_replay_system_state.frame = 0;

	set_replay_state_internal(replay_state_playback);
}

void stop_replay_internal()
{
	log_message(verbose, "c_replay_system: stop replay");
	ASSERT(g_replay_system_state.replay_file.is_open());

	g_replay_system_state.replay_file.close();
	set_replay_state_internal(replay_state_none);
}

#endif //FEATURE_REPLAY