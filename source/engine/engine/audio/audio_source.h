#ifndef __AUDIO_SOURCE_H__
#define __AUDIO_SOURCE_H__
#pragma once

#include "structures/audio/audio_buffer.h"
#include "types/audio_types.h"
#include "types/types.h"
#include "engine/file_system/file.h"

class c_audio_source
{
public:
	c_audio_source() {}
	~c_audio_source() {}

	virtual void get_samples(t_audio_buffer_real32& out_buffer) = 0;

	// this probably wants to be included in the result of get_samples since that's
	// likely the only time we need that info.
	virtual bool HACK_finished() const = 0;
};

class c_audio_source_sine : public c_audio_source
{
public:
	c_audio_source_sine() :
		m_sample_position(0),
		m_frequency(0.0f)
	{
	}

	c_audio_source_sine(real32 frequency) :
		m_sample_position(0),
		m_frequency(frequency)
	{
	}

	~c_audio_source_sine() {}

	void get_samples(t_audio_buffer_real32& out_buffer);
	bool HACK_finished() const { return false; }

private:
	uint32 m_sample_position;
	real32 m_frequency;
};

class c_audio_source_noise : public c_audio_source
{
public:
	c_audio_source_noise() : c_audio_source() {}

	void get_samples(t_audio_buffer_real32& out_buffer);
	bool HACK_finished() const { return false; }

private:

	float g_level = 2.0f / 0xffffffff;
	int g_x1 = 0x67452301;
	int g_x2 = 0xefcdab89;
};

class c_audio_source_file : public c_audio_source
{
public:
	c_audio_source_file();
	~c_audio_source_file();

	// remove
	void set_memory(const c_array<byte>* memory);

	void set_buffer(const t_audio_buffer_real32& buffer)
	{
		m_buffer = buffer;
	}

	void get_samples(t_audio_buffer_real32& out_buffer);
	bool HACK_finished() const { return m_HACK_finished; }

private:
	c_array<byte> m_memory;
	s_audio_file_format m_format;
	uint32 m_position;
	bool m_looping;
	bool m_HACK_finished;

	t_audio_buffer_real32 m_buffer;
};

class c_audio_source_file_streamed : public c_audio_source
{
public:
	c_audio_source_file_streamed();
	~c_audio_source_file_streamed();

	void set_file(c_file_path file_path);

	void get_samples(t_audio_buffer_real32& out_buffer);
	bool HACK_finished() const { return m_HACK_finished; }

private:
	// todo: either make this a templatized size or allow the buffer to be dynamically allocated
	c_file_static_buffered<30 * k_byte_kb> m_file;
	s_audio_file_format m_format;
	uint32 m_position;
	bool m_looping;
	bool m_HACK_finished;
};

#endif // __AUDIO_SOURCE_H__
