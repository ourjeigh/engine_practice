#ifndef __AUDIO_SYSTEM_H__
#define __AUDIO_SYSTEM_H__
#pragma once

#include "engine/engine_system.h"
#include <threads/threads.h>
#include <asserts.h>
#include <cmath>

const real32 k_math_real32_pi = 3.14159265358979323846f;
const real32 k_math_real32_2pi = 2 * k_math_real32_pi;

class c_audio_source_sine;

enum e_audio_sample_type
{
	sample_type_unknown = 0,
	sample_type_int16,
	sample_type_float32,
};

struct s_audio_format
{
	e_audio_sample_type sample_type;
	uint16 num_channels;
	uint32 sample_rate;
	bool interleaved;
};

class c_audio_source
{
public:
	c_audio_source() {}
	c_audio_source(int32 sample_rate) : m_sample_rate(sample_rate) {}
	~c_audio_source() {}

	virtual void get_samples(real32* buffer, uint32 num_samples) = 0;
protected:
	real32 m_sample_rate;
};

class c_audio_source_sine : public c_audio_source
{
public:
	c_audio_source_sine() : c_audio_source() {}
	c_audio_source_sine(int32 sample_rate, real32 frequency) :
		m_sample_position(0),
		m_frequency(frequency),
		c_audio_source(sample_rate)
	{
	}
	~c_audio_source_sine() {}

	void get_samples(real32* buffer, uint32 num_samples)
	{
		ASSERT(buffer);

		while (num_samples)
		{
			*buffer++ = sinf((k_math_real32_2pi * m_frequency) * (m_sample_position++ / this->m_sample_rate));
			num_samples--;
		}
	}
private:
	uint32 m_sample_position;
	real32 m_frequency;
};

class c_audio_source_noise : public c_audio_source
{
public:
	c_audio_source_noise() : c_audio_source() {}
	c_audio_source_noise(int32 sample_rate) : c_audio_source(sample_rate) {}

	void get_samples(real32* buffer, uint32 num_samples)
	{
		ASSERT(buffer);

		real32 _fLevel= g_fScale;

		while (num_samples--)
		{
			g_x1 ^= g_x2;
			*buffer++ = g_x2 * _fLevel;
			g_x2 += g_x1;
		}
	}


	void whitenoise(
		float* _fpDstBuffer, // Pointer to buffer
		unsigned int _uiBufferSize, // Size of buffer
		float _fLevel) // Noiselevel (0.0 ... 1.0)
	{
	}
private:

	float g_fScale = 2.0f / 0xffffffff;
	int g_x1 = 0x67452301;
	int g_x2 = 0xefcdab89;
};

class c_audio_mixer
{
public:

	// assumes interleaved.
	static void mix_a_into_b(real32* in_buffer, real32* out_buffer, int32 num_samples, int32 num_channels)
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
};

class c_audio_system : public c_engine_system
{
public:
	virtual void init() override;
	virtual void term() override;
	virtual void update() override;
};

class c_audio_render_thread : public c_thread
{
public:
	c_audio_render_thread() : c_thread() {}
	~c_audio_render_thread() {}
	void init();
	void term();

private:
	static void audio_render_thread_entry_point(c_audio_render_thread *thread);
	void render_audio();
	bool setup_audio_device();

	// make this thread safe
	bool m_is_running;
	void* m_callback_event_handle;
	void* m_audio_client;
	void* m_audio_render_client;
	uint64 m_device_period_ms;
	uint32 m_buffer_size;

	c_audio_source_noise m_HACK_test_noise;
	c_audio_source_sine m_HACK_test_sine;
};



#endif//__AUDIO_SYSTEM_H__