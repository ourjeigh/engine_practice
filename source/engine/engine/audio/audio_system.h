#ifndef __AUDIO_SYSTEM_H__
#define __AUDIO_SYSTEM_H__
#pragma once

#include "structures/audio/audio_buffer.h"
#include "types/audio_types.h"
#include "audio_source.h"
#include "config.h"
#include "debug/asserts.h"
#include "engine/engine_system.h"
#include "mmath.h"
#include "platform/platform_audio_sink.h"
#include "threads/threads.h"

class c_audio_mixer
{
public:
	// assumes interleaved.
	static_member_function void mix_a_into_b_interleaved(real32* in_buffer, real32* out_buffer, int32 num_samples, int32 num_channels)
	{
		while (num_samples)
		{
			real32 sample = *in_buffer++;

			for (int32 channel_index = 0; channel_index < num_channels; channel_index++)
			{
				*out_buffer++ = sample;
			}

			num_samples--;
		}
	}

	static_member_function void interleave(const real32** in_buffers, real32* out_buffer, int32 num_samples, int32 num_channels)
	{
		while (num_samples)
		{
			for (int32 channel_index = 0; channel_index < num_channels; channel_index++)
			{
				*out_buffer++ = *in_buffers[channel_index]++;
			}

			num_samples--;
		}
	}
};

class c_audio_system : public c_engine_system<c_audio_system>
{
public:
	void init() override;
	void term() override;
	void update() override;

	static_member_function t_sound_playback_id play_sound(const s_wav_asset& asset);
	static_member_function t_sound_playback_id play_sound(s_sound_info& info);
	static_member_function t_sound_playback_id play_debug_pip();

	static_member_function void update_sound(t_sound_playback_id playback_id, s_sound_properties& properties);
	static_member_function void stop_sound(t_sound_playback_id playback_id);
};

class c_audio_engine_thread : public c_thread
{
public:
	c_audio_engine_thread() : 
		m_is_running(false), 
		c_thread() {}

	~c_audio_engine_thread() {};

	void init();
	void term();

private:
	static_member_function void audio_engine_thread_entry_point(c_audio_engine_thread* thread);
	void process_audio();

	bool m_is_running;
};

class c_audio_render_thread : public c_thread
{
public:
	c_audio_render_thread() : 
		m_is_running(false),
		m_device_period_ms(0),
		m_buffer_size(0),
		m_sink(),
		DEBUG_ONLY_PARAM_RIGHT_COMMA(d_read_initialized(false))
		c_thread() {}
	~c_audio_render_thread() {}

	void init();
	void term();

private:
	static_member_function void audio_render_thread_entry_point(c_audio_render_thread* thread);
	void render_audio();
	bool setup_audio_sink();
	void shutdown_audio_sink();

	bool m_is_running;
	
	c_platform_audio_sink m_sink;

	uint32 m_device_period_ms;
	uint32 m_buffer_size;

	c_platform_handle m_render_event_handle;

	DEBUG_ONLY_MEMBER(bool d_read_initialized);
};

uint32 audio_system_get_sample_rate();
const s_audio_device_format& audio_get_format();

inline t_sound_playback_id audio_system_play_sound(s_sound_info& info)
{
	return c_audio_system::play_sound(info);
}

inline t_sound_playback_id audio_system_play_sound(const s_wav_asset& asset)
{
	return c_audio_system::play_sound(asset);
}

inline void audio_system_stop_sound(t_sound_playback_id playback_id)
{
	return c_audio_system::stop_sound(playback_id);
}

inline t_sound_playback_id audio_system_play_debug_pip()
{
	return c_audio_system::play_debug_pip();
}
#endif //__AUDIO_SYSTEM_H__