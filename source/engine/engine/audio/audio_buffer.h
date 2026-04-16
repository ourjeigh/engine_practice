#ifndef __AUDIO_BUFFER_H__
#define __AUDIO_BUFFER_H__
#pragma once

#include <types/types.h>
#include "structures/array.h"
#include "platform/platform.h"
#include "threads/atomic.h"

IGNORE_WINDOWS_WARNINGS_PUSH
//#include <atomic>
IGNORE_WINDOWS_WARNINGS_POP

template<typename t_type>
class c_audio_buffer
{
public:
	c_audio_buffer(int32 channel_count, int32 size) : 
		m_channel_count(channel_count),
		m_size(size),
		m_data(nullptr)
	{
		ASSERT(channel_count > 0);
		ASSERT(size > 0);
	}

	c_audio_buffer(int32 channel_count, int32 size, t_type* data) : m_channel_count(channel_count), m_size(size), m_data(data)
	{
		ASSERT(channel_count > 0);
		ASSERT(size > 0);
		ASSERT(data != nullptr);
	}

	~c_audio_buffer() {}

	void set_data(t_type* data)
	{
		ASSERT(data != nullptr);
		m_data = data;
	}

	int32 channel_count() const { return m_channel_count; }
	int32 size() const { return m_size; }

	t_type* get_channel(int32 channel_index)
	{
		ASSERT(0 <= channel_index && channel_index < m_channel_count);
		return &m_data[channel_index * m_size];
	}

	const t_type* get_channel_const(int32 channel_index) const
	{
		ASSERT(0 <= channel_index && channel_index < m_channel_count);
		return &m_data[channel_index * m_size];
	}

	void zero();

	// end exclusive
	void zero(int32 begin, int32 end);
	void copy_from(const c_audio_buffer<t_type>& other);
	void copy_from(const c_audio_buffer<t_type>& other, int32 sample_count);

protected:
	int32 m_channel_count;
	int32 m_size;
	t_type* m_data;

private:
	c_audio_buffer(const c_audio_buffer& other) = delete;
	c_audio_buffer& operator=(const c_audio_buffer& other) = delete;
};

typedef c_audio_buffer<real32> t_audio_buffer_real32;

template<typename t_type, int32 k_channel_count, int32 k_size>
class c_static_audio_buffer : public c_audio_buffer<t_type>
{
public:
	c_static_audio_buffer() : c_audio_buffer<t_type>(k_channel_count, k_size)
	{
		this->set_data(&m_storage[0]);
		//this->m_data = &m_storage[0][0];
		//zero_object(m_storage); 
	}

	c_static_audio_buffer(const c_static_audio_buffer& other) { m_storage = other.m_storage; }
	
	~c_static_audio_buffer() {}
	
	c_static_audio_buffer& operator=(const c_static_audio_buffer& other)
	{
		if (this != &other)
		{
			m_storage = other.m_storage;
		}
		return *this;
	}

	int32 channel_count() const { return k_channel_count; }

	t_type* get_channel(int32 channel_index)
	{
		ASSERT(0 <= channel_index && channel_index < k_channel_count);
		return &m_storage[channel_index * k_size];
		//return &m_storage[channel_index][0];
	}

	const t_type* get_channel_const(int32 channel_index) const
	{
		return get_channel(channel_index);
	}

	void get_interleaved(t_type* out_buffer, int32 sample_count)
	{
		ASSERT(out_buffer != nullptr);

		for (int32 sample_index = 0; sample_index < sample_count; sample_index++)
		{
			for (int32 channel_index = 0; channel_index < k_channel_count; channel_index++)
			{
				*out_buffer++ = m_storage[channel_index][sample_index];
			}
		}
	}

	//t_type* get_channel_raw(int32 channel_index)
	//{
	//	ASSERT(0 <= channel_index && channel_index < k_channel_count);
	//	return &m_storage[channel_index][0];
	//}
private:
	//c_static_array<c_static_array<t_type, k_size>, k_channel_count> m_storage;
	c_static_array<t_type, k_size * k_channel_count> m_storage;
};

template<typename t_type, int32 k_channel_count, int32 k_size>
class c_audio_ring_buffer
{
public:
	c_audio_ring_buffer() : m_write_position(0), m_read_position(0) { m_buffer.zero(); }
	~c_audio_ring_buffer() {}

	// returns actual samples written
	int32 write(const c_audio_buffer<t_type>* in_buffer, int32 sample_count);
	
	// returns actual samples read
	int32 read(c_audio_buffer<t_type>* out_buffer, int32 sample_count);


private:
	int32 m_write_position;
	int32 m_read_position;
	c_static_audio_buffer<t_type,k_channel_count, k_size> m_buffer;
};

template<typename t_type, int32 k_channel_count, int32 k_size>
class c_audio_threadsafe_ring_buffer
{
public:
	c_audio_threadsafe_ring_buffer() : m_write_position(0), m_read_position(0) { m_buffer.zero(); }
	~c_audio_threadsafe_ring_buffer() {}

	// returns actual samples written
	int32 write(const c_audio_buffer<t_type>* in_buffer, int32 sample_count);

	// returns actual samples read
	int32 read(c_audio_buffer<t_type>* out_buffer, int32 sample_count);

	int32 read_interleaved(t_type* out_buffer, int32 sample_count);

	int32 free_sample_count();
	int32 channel_count() const { return k_channel_count; }

private:
	c_atomic<int32> m_write_position;
	c_atomic<int32> m_read_position;
	c_static_audio_buffer<t_type, k_channel_count, k_size> m_buffer;
};


template<int32 k_size>
class c_audio_real32_mono_ring_buffer : public c_audio_ring_buffer<real32, 1, k_size>
{
};

#include "audio_buffer.inl"
#endif // __AUDIO_BUFFER_H__
