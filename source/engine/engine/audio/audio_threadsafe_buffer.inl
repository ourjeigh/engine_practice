#include "mmath.h"

template<typename t_type>
int32 c_audio_threadsafe_ring_buffer<t_type>::write(const c_audio_buffer<t_type>* in_buffer, int32 sample_count)
{
	int32 read_position = m_read_position.load(atomic_memory_order_relaxed);
	int32 write_position = m_write_position.load(atomic_memory_order_acquire);
	int32 writable_samples = read_position == write_position ? size() : (read_position - write_position - 1 + size()) % size();
	int32 samples_to_write = math_min(sample_count, writable_samples);
	int32 first_block = math_min(size() - write_position, samples_to_write);

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

	m_write_position.store((write_position + samples_to_write) % size(), atomic_memory_order_release);
	return samples_to_write;
}

// returns actual samples read
template<typename t_type>
int32 c_audio_threadsafe_ring_buffer<t_type>::read(c_audio_buffer<t_type>* out_buffer, int32 sample_count)
{
	int32 read_position = m_read_position.load(atomic_memory_order_acquire);
	int32 write_position = m_write_position.load(atomic_memory_order_relaxed);

	int32 readable_samples = (write_position - read_position + size()) % size();
	int32 samples_to_read = math_min(sample_count, readable_samples);
	int32 first_block = math_min(size() - read_position, samples_to_read);

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

	m_read_position.store((read_position + samples_to_read) % size(), atomic_memory_order_release);
	return samples_to_read;
}

template<typename t_type>
int32 c_audio_threadsafe_ring_buffer<t_type>::read_interleaved(t_type* out_buffer, int32 sample_count)
{
	int32 read_position = m_read_position.load(atomic_memory_order_acquire);
	int32 write_position = m_write_position.load(atomic_memory_order_relaxed);

	int32 readable_samples = (write_position - read_position + size()) % size();
	int32 samples_to_read = math_min(sample_count, readable_samples);
	int32 first_block = math_min(size() - read_position, samples_to_read);
	int32 channels = channel_count();

	for (int32 sample_index = 0; sample_index < first_block; sample_index++)
	{
		for (int32 channel_index = 0; channel_index < channels; channel_index++)
		{
			*out_buffer++ = m_buffer.get_channel(channel_index)[read_position + sample_index];
		}
	}

	if (first_block < samples_to_read)
	{
		int32 second_block = samples_to_read - first_block;
		for (int32 sample_index = 0; sample_index < second_block; sample_index++)
		{
			for (int32 channel_index = 0; channel_index < channels; channel_index++)
			{
				*out_buffer++ = m_buffer.get_channel(channel_index)[sample_index];
			}
		}
	}

	m_read_position.store((read_position + samples_to_read) % size(), atomic_memory_order_release);
	return samples_to_read;
}


template<typename t_type>
int32 c_audio_threadsafe_ring_buffer<t_type>::free_sample_count()
{
	int32 read_position = m_read_position.load(atomic_memory_order_relaxed);
	int32 write_position = m_write_position.load(atomic_memory_order_relaxed);

	int32 sz = size();
	int32 writable_samples = read_position == write_position ? sz : (read_position - write_position - 1 + sz) % sz;
	return writable_samples;
}

template<typename t_type>
bool c_audio_threadsafe_ring_buffer<t_type>::is_initialized() const
{
	int32 read_position = m_read_position.load(atomic_memory_order_relaxed);
	int32 write_position = m_write_position.load(atomic_memory_order_relaxed);

	return read_position != k_invalid && write_position != k_invalid;
}
