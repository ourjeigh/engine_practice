#include "mmath.h"

template<typename t_type>
void c_audio_buffer<t_type>::zero()
{
	zero(0, size());
}

template<typename t_type>
void c_audio_buffer<t_type>::zero(int32 begin, int32 end)
{
	ASSERT(0 <= begin && begin < size());
	ASSERT(0 <= end && end <= size());
	ASSERT(begin < end);

	int32 sample_count = end - begin;
	for (int32 channel_index = 0; channel_index < channel_count(); channel_index++)
	{
		memory_zero(&get_channel(channel_index)[begin], sample_count * sizeof(t_type));
	}
}

template<typename t_type>
void c_audio_buffer<t_type>::copy_from(const c_audio_buffer<t_type>& other)
{
	ASSERT(other.channel_count() <= channel_count());

	copy_from(other, size());
}

template<typename t_type>
void c_audio_buffer<t_type>::copy_from(const c_audio_buffer<t_type>& other, int32 sample_count)
{
	ASSERT(other.channel_count() <= channel_count());
	ASSERT(other.size() >= sample_count);

	for (int32 channel_index = 0; channel_index < channel_count(); channel_index++)
	{
		memory_copy(get_channel(channel_index), other.get_channel_const(channel_index), sample_count * sizeof(t_type));
	}
}

// returns actual samples written
template<typename t_type, int32 k_channel_count, int32 k_size>
int32 c_audio_ring_buffer<t_type, k_channel_count, k_size>::write(const c_audio_buffer<t_type>* in_buffer, int32 sample_count)
{
	int32 writable_samples = m_read_position == m_write_position ? k_size : (m_read_position - m_write_position - 1 + k_size) % k_size;
	int32 samples_to_write = math_min(sample_count, writable_samples);
	int32 first_block = math_min(k_size - m_write_position, samples_to_write);

	for (int32 channel_index = 0; channel_index < in_buffer->channel_count(); channel_index++)
	{
		memory_copy((m_buffer.get_channel(channel_index) + m_write_position), (in_buffer->get_channel_const(channel_index)), sizeof(t_type) * first_block);
	}

	if (first_block < samples_to_write)
	{
		int32 second_block = samples_to_write - first_block;

		for (int32 channel_index = 0; channel_index < in_buffer->channel_count(); channel_index++)
		{
			memory_copy(m_buffer.get_channel(channel_index), (in_buffer->get_channel_const(channel_index) + first_block), sizeof(t_type) * second_block);
		}
	}

	m_write_position = (m_write_position + samples_to_write) % k_size;

	return samples_to_write;
}

// returns actual samples read
template<typename t_type, int32 k_channel_count, int32 k_size>
int32 c_audio_ring_buffer<t_type, k_channel_count, k_size>::read(c_audio_buffer<t_type>* out_buffer, int32 sample_count)
{
	int32 readable_samples = (m_write_position - m_read_position + k_size) % k_size;
	int32 samples_to_read = math_min(sample_count, readable_samples);
	int32 first_block = math_min(k_size - m_read_position, samples_to_read);

	for (int32 channel_index = 0; channel_index < out_buffer->channel_count(); channel_index++)
	{
		memory_copy(&(out_buffer->get_channel(channel_index))[0], &m_buffer.get_channel(channel_index)[m_read_position], sizeof(t_type) * first_block);
	}

	if (first_block < samples_to_read)
	{
		int32 second_block = samples_to_read - first_block;
		for (int32 channel_index = 0; channel_index < out_buffer->channel_count(); channel_index++)
		{
			memory_copy(&out_buffer->get_channel(channel_index)[first_block], &m_buffer.get_channel(channel_index)[0], sizeof(t_type) * second_block);
		}
	}

	m_read_position = (m_read_position + samples_to_read) % k_size;

	return samples_to_read;
}

template<typename t_type, int32 k_channel_count, int32 k_size>
int32 c_audio_threadsafe_ring_buffer<t_type, k_channel_count, k_size>::write(const c_audio_buffer<t_type>* in_buffer, int32 sample_count)
{
	int32 read_position = m_read_position.load(atomic_memory_order_relaxed);
	int32 write_position = m_write_position.load(atomic_memory_order_acquire);

	int32 writable_samples = read_position == write_position ? k_size : (read_position - write_position - 1 + k_size) % k_size;
	int32 samples_to_write = math_min(sample_count, writable_samples);
	int32 first_block = math_min(k_size - write_position, samples_to_write);

	for (int32 channel_index = 0; channel_index < in_buffer->channel_count(); channel_index++)
	{
		memory_copy(&m_buffer.get_channel(channel_index)[write_position], &(in_buffer->get_channel_const(channel_index))[0], sizeof(t_type) * first_block);
	}

	if (first_block < samples_to_write)
	{
		int32 second_block = samples_to_write - first_block;

		for (int32 channel_index = 0; channel_index < in_buffer->channel_count(); channel_index++)
		{
			memory_copy(&m_buffer.get_channel(channel_index)[0], &in_buffer->get_channel_const(channel_index)[first_block], sizeof(t_type) * second_block);
		}
	}

	m_write_position.store((write_position + samples_to_write) % k_size, atomic_memory_order_release);
	return samples_to_write;
}

// returns actual samples read
template<typename t_type, int32 k_channel_count, int32 k_size>
int32 c_audio_threadsafe_ring_buffer<t_type, k_channel_count, k_size>::read(c_audio_buffer<t_type>* out_buffer, int32 sample_count)
{
	int32 read_position = m_read_position.load(atomic_memory_order_acquire);
	int32 write_position = m_write_position.load(atomic_memory_order_relaxed);

	int32 readable_samples = (write_position - read_position + k_size) % k_size;
	int32 samples_to_read = math_min(sample_count, readable_samples);
	int32 first_block = math_min(k_size - read_position, samples_to_read);

	for (int32 channel_index = 0; channel_index < out_buffer->channel_count(); channel_index++)
	{
		memory_copy(&(out_buffer->get_channel(channel_index))[0], &m_buffer.get_channel(channel_index)[read_position], sizeof(t_type) * first_block);
	}

	if (first_block < samples_to_read)
	{
		int32 second_block = samples_to_read - first_block;
		for (int32 channel_index = 0; channel_index < out_buffer->channel_count(); channel_index++)
		{
			memory_copy(&out_buffer->get_channel(channel_index)[first_block], &m_buffer.get_channel(channel_index)[0], sizeof(t_type) * second_block);
		}
	}

	m_read_position.store((read_position + samples_to_read) % k_size, atomic_memory_order_release);
	return samples_to_read;
}

template<typename t_type, int32 k_channel_count, int32 k_size>
int32 c_audio_threadsafe_ring_buffer<t_type, k_channel_count, k_size>::read_interleaved(t_type* out_buffer, int32 sample_count)
{
	int32 read_position = m_read_position.load(atomic_memory_order_acquire);
	int32 write_position = m_write_position.load(atomic_memory_order_relaxed);

	int32 readable_samples = (write_position - read_position + k_size) % k_size;
	int32 samples_to_read = math_min(sample_count, readable_samples);
	int32 first_block = math_min(k_size - read_position, samples_to_read);

	for (int32 sample_index = 0; sample_index < first_block; sample_index++)
	{
		for (int32 channel_index = 0; channel_index < k_channel_count; channel_index++)
		{
			*out_buffer++ = m_buffer.get_channel(channel_index)[read_position + sample_index];
		}
	}

	if (first_block < samples_to_read)
	{
		int32 second_block = samples_to_read - first_block;
		for (int32 sample_index = 0; sample_index < second_block; sample_index++)
		{
			for (int32 channel_index = 0; channel_index < k_channel_count; channel_index++)
			{
				*out_buffer++ = m_buffer.get_channel(channel_index)[sample_index];
			}
		}
	}

	m_read_position.store((read_position + samples_to_read) % k_size, atomic_memory_order_release);
	return samples_to_read;
}


template<typename t_type, int32 k_channel_count, int32 k_size>
int32 c_audio_threadsafe_ring_buffer<t_type, k_channel_count, k_size>::free_sample_count()
{
	int32 read_position = m_read_position.load(atomic_memory_order_relaxed);
	int32 write_position = m_write_position.load(atomic_memory_order_relaxed);

	int32 writable_samples = read_position == write_position ? k_size : (read_position - write_position - 1 + k_size) % k_size;
	return writable_samples;
}
