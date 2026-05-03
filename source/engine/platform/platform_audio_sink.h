#ifndef __PLATFORM_AUDIO_SINK_H__
#define __PLATFORM_AUDIO_SINK_H__
#pragma once

#include "types/audio_types.h"

class c_platform_audio_sink
{
public:
	c_platform_audio_sink();
	~c_platform_audio_sink() {}

	bool register_sink(s_audio_device_format& inout_audio_format);
	bool unregister_sink();

	// maybe change this to take an actual c_audio_buffer and just have the sink set the data pointer
	bool begin_render(real32*& out_buffer, uint32& out_available_samples);
	void render_complete(uint32 samples_filled);

private:
	uint32 m_device_period_ms;
	bool m_render_active;
};

#endif // __PLATFORM_AUDIO_SINK_H__