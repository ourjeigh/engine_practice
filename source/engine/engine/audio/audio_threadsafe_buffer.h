#ifndef __AUDIO_THREADSAFE_BUFFER_H__
#define __AUDIO_THREADSAFE_BUFFER_H__
#pragma once

#include "types/types.h"
#include "structures/array.h"
#include "threads/atomic.h"

template<typename t_type>
class c_audio_threadsafe_ring_buffer
{
public:
	c_audio_threadsafe_ring_buffer() : m_write_position(0), m_read_position(0) {}
	~c_audio_threadsafe_ring_buffer() {}

	void init(int32 channel_count, int32 size, t_type* data)
	{
		m_buffer = c_audio_buffer<t_type>(channel_count, size, data);
	}

	// returns actual samples written
	int32 write(const c_audio_buffer<t_type>* in_buffer, int32 sample_count);

	// returns actual samples read
	int32 read(c_audio_buffer<t_type>* out_buffer, int32 sample_count);

	int32 read_interleaved(t_type* out_buffer, int32 sample_count);

	int32 free_sample_count();
	int32 channel_count() const { return m_buffer.channel_count(); }
	int32 size() const { return m_buffer.size(); }

protected:
	c_atomic<int32> m_write_position;
	c_atomic<int32> m_read_position;
	c_audio_buffer<t_type> m_buffer;
};

template<typename t_type, int32 k_channel_count, int32 k_size>
class c_audio_threadsafe_static_ring_buffer : public c_audio_threadsafe_ring_buffer<t_type>
{
public:
	constexpr c_audio_threadsafe_static_ring_buffer() : 
		c_audio_threadsafe_ring_buffer<t_type>()
	{
		this->m_buffer = m_data;
	}

private:
	c_static_audio_buffer<t_type, k_channel_count, k_size> m_data;
};

#include "audio_threadsafe_buffer.inl"
#endif // __AUDIO_THREADSAFE_BUFFER_H__
