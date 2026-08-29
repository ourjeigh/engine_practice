#ifndef __REPLAY_H__
#define __REPLAY_H__
#pragma once

#include "config.h"
#ifdef FEATURE_REPLAY

#include "engine/engine_system.h"

enum e_replay_state
{
	replay_state_none,
	replay_state_recording,
	replay_state_playback,

	k_input_replay_state_count
};

struct s_replay_info
{
	int32 frame_count;
};

class c_replay_system : public c_engine_system<c_replay_system>
{
public:
	void init();
	void term();
	void update();

	static_member_function e_replay_state get_replay_state();
};
#endif //FEATURE_REPLAY
#endif // !__REPLAY_H__
