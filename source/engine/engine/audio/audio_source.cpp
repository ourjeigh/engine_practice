#include "audio_source.h"
#include "mmath.h"
#include "types/audio_types.h"
#include "audio_buffer.h"
#include "audio_system.h"

// these types correspond to the predefined wave file chunks.
// they must be sized (and ordered) to spec so we can read them 
// directly from files.
// see: http://soundfile.sapp.org/doc/WaveFormat/
struct s_audio_wav_header_riff
{
	char riff[4];
	uint32 chunk_size;
	char wave[4];
};
COMPILE_ASSERT(sizeof(s_audio_wav_header_riff) == 12);

struct s_audio_wav_header_format
{
	char format[4];
	uint32 chunk_size;
	uint16 audio_format;
	uint16 channel_count;
	uint32 sample_rate;
	uint32 bytes_per_second;
	uint16 block_align;
	uint16 bits_per_sample;
};
COMPILE_ASSERT(sizeof(s_audio_wav_header_format) == 24);

struct s_audio_wav_header_chunk
{
	char name[4];
	uint32 chunk_size;
};
COMPILE_ASSERT(sizeof(s_audio_wav_header_chunk) == 8);

inline real32 convert_sample_to_real32(byte* in, e_audio_sample_type sample_type);


void c_audio_source_sine::get_samples(t_audio_buffer_real32& out_buffer)
{
	int32 sample_count = out_buffer.size();
	real32* buffer = out_buffer.get_channel(0);
	ASSERT(buffer != nullptr);
	const real32 sample_rate = audio_system_get_sample_rate();

	while (sample_count > 0)
	{
		real32 val = math_sin((k_math_real32_two_pi * m_frequency) * (static_cast<real32>(m_sample_position++) / sample_rate));
		*buffer++ = val;
		sample_count--;
	}
}

void c_audio_source_noise::get_samples(t_audio_buffer_real32& out_buffer)
{
	int32 sample_count = out_buffer.size();
	real32* buffer = out_buffer.get_channel(0);
	ASSERT(buffer != nullptr);

	while (sample_count > 0)
	{
		g_x1 ^= g_x2;
		*buffer++ = g_x2 * g_level;
		g_x2 += g_x1;
		sample_count--;
	}
}


e_audio_sample_type get_sample_type_from_bits_per_sample(uint16 bits_per_sample)
{
	e_audio_sample_type out_type = audio_sample_type_unknown;
	switch (bits_per_sample)
	{
	case 16:
		out_type = audio_sample_type_int16;
		break;
	case 24:
		out_type = audio_sample_type_int24;
		break;
	default:
		break;
	}

	return out_type;
}

e_audio_compression_format get_compression_format_from_wave_format(uint16 format)
{
	e_audio_compression_format out_format = audio_compression_format_unknown;

	switch (format)
	{
	case 1:
		out_format = audio_compression_format_pcm;
		break;
	default:
		break;
	}

	return out_format;
}

s_audio_file_format get_file_format(c_array<byte> file_bytes)
{
	s_audio_file_format out_format;

	uint32 index = 0;
	s_audio_wav_header_riff riff_chunk;
	memory_copy(&riff_chunk, &file_bytes.data()[index], sizeof(s_audio_wav_header_riff));

	index += sizeof(s_audio_wav_header_riff);

	s_audio_wav_header_format format_chunk;
	memory_copy(&format_chunk, &file_bytes.data()[index], sizeof(s_audio_wav_header_format));

	out_format.sample_type = get_sample_type_from_bits_per_sample(format_chunk.bits_per_sample);
	out_format.channel_count = format_chunk.channel_count;
	out_format.sample_rate = format_chunk.sample_rate;
	out_format.bits_per_sample = format_chunk.bits_per_sample;
	out_format.block_align = format_chunk.block_align;

	ASSERT(out_format.sample_type != audio_sample_type_unknown);
	ASSERT(out_format.channel_count > 0);
	ASSERT(out_format.sample_rate > 0);
	ASSERT(out_format.bits_per_sample > 0);
	ASSERT(out_format.block_align > 0);

	index += sizeof(s_audio_wav_header_format);

	// now we're in uncharted territory. look for the "data" chunk
	s_audio_wav_header_chunk chunk;
	bool data_chunk_found = false;

	while (!data_chunk_found)
	{
		if (index + sizeof(s_audio_wav_header_chunk) >= file_bytes.capacity())
		{
			HALT_UNIMPLEMENTED();
		}

		memory_copy(&chunk, &file_bytes.data()[index], sizeof(s_audio_wav_header_chunk));

		data_chunk_found = memory_compare(chunk.name, "data", sizeof(chunk.name)) == 0;

		if (!data_chunk_found)
		{
			// the next chunk will start chunk_size bytes away
			index += sizeof(s_audio_wav_header_chunk) + chunk.chunk_size;
		}
	}

	out_format.sample_count = chunk.chunk_size / format_chunk.block_align;
	out_format.data_position = index + sizeof(s_audio_wav_header_chunk);

	ASSERT(out_format.sample_count > 0);
	ASSERT(out_format.data_position > (sizeof(s_audio_wav_header_riff) + sizeof(s_audio_wav_header_format) + sizeof(s_audio_wav_header_chunk)));

	return out_format;
}

c_audio_source_file::c_audio_source_file()
{
}

c_audio_source_file::~c_audio_source_file()
{
}

void c_audio_source_file::set_memory(const c_array<byte>* memory)
{
	ASSERT(memory != nullptr);
	ASSERT(memory->is_valid());
	m_memory = *memory;

	m_format = get_file_format(m_memory);
	m_position = 0;
}

void c_audio_source_file::get_samples(t_audio_buffer_real32& out_buffer)
{
	const int32 sample_count = out_buffer.size();
	const int32 bytes_per_sample = m_format.bits_per_sample / 8;

	const int32 samples_left = m_format.sample_count - m_position;
	const int32 first_read = math_min(sample_count, samples_left);

	c_static_audio_buffer<real32, 2, 1024> audio_buffer;

	for (int32 sample_index = 0 ;
		sample_index < first_read;
		sample_index++)
	{
		int32 byte_index = (m_position + sample_index) * bytes_per_sample;
		
		for (int32 channel_index = 0; channel_index < m_format.channel_count; channel_index++)
		{
			// TODO: this involves way too much switch logic. rework to just do the switch once
			real32 sample = convert_sample_to_real32(&m_memory.data()[byte_index + (bytes_per_sample * channel_index)], m_format.sample_type);
			out_buffer.get_channel(channel_index)[sample_index] = sample;
		}
	}

	//out_buffer.copy_from(audio_buffer);

	m_position += first_read;

	if (first_read < sample_count)
	{
		ASSERT(m_position == m_format.sample_count);

		if (m_looping)
		{
			int32 second_read = sample_count - first_read;

			for (int32 sample_index = first_read;
				sample_index < sample_count;
				sample_index++)
			{
				int32 byte_index = (m_position + sample_index) * bytes_per_sample;

				for (int32 channel_index = 0; channel_index < m_format.channel_count; channel_index++)
				{
					// TODO: this involves way too much switch logic. rework to just do the switch once
					real32 sample = convert_sample_to_real32(&m_memory.data()[byte_index + (bytes_per_sample * channel_index)], m_format.sample_type);
					out_buffer.get_channel(channel_index)[sample_index] = sample;
				}
			}

			m_position = second_read;
		}
		else
		{
			m_HACK_finished = true;
			int32 begin = sample_count - first_read;
			int32 end = sample_count;

			if (begin != end)
			{
				out_buffer.zero(begin, end);
			}
		}
	}
}

c_audio_source_file_streamed::c_audio_source_file_streamed() :
	m_file(),
	m_format(),
	m_position(k_invalid),
	m_looping(false),
	m_HACK_finished(false),
	c_audio_source() 
{
}

c_audio_source_file_streamed::~c_audio_source_file_streamed()
{
	m_file.close();
}

void c_audio_source_file_streamed::set_file(c_file_path file_path)
{
	zero_object(m_format);

	t_file_open_mode_flags flags;
	flags.set(file_open_mode_read, true);
	m_file.open(file_path, flags);

	c_static_array<byte, 2048> header_buffer;
	m_file.read_bytes_unbuffered(0, header_buffer.capacity(), header_buffer);

	m_format = get_file_format(header_buffer);

	m_file.set_read_position(m_format.data_position);
	m_position = 0;
}

inline real32 convert_sample_to_real32(byte* in, e_audio_sample_type sample_type)
{
	real32 out = 0.0f;

	switch (sample_type)
	{
	case audio_sample_type_int8:
	{
		const uint8 k_audio_int8_zero = 128;
		const real32 divizor = 1 / k_audio_int8_zero;
		out = (static_cast<uint8>(*in) - k_audio_int8_zero) * divizor;
		HALT_UNIMPLEMENTED(); // needs testing
		break;
	}
	case audio_sample_type_int16:
	{
		const real32 divizor = 1 / (k_int16_max + 1);
		out = (*reinterpret_cast<int16*>(in)) * divizor;
		//HALT_UNIMPLEMENTED(); // needs testing
		break;
	}
	case audio_sample_type_int24:
	{
		constexpr uint32 int24_max = math_pow<uint32>(2, 31);
		const real32 divisor = 1.0f / int24_max;
		out = ((in[0] << 8)| (in[1] << 16) | (in[2] << 24)) * divisor;
		break;
	}
	case audio_sample_type_int32:
	{
		const real32 divizor = 1 / int64_to_real32((int32_to_int64(k_int32_max) + 1));
		out = (*reinterpret_cast<int32*>(in)) * divizor;
		HALT_UNIMPLEMENTED(); // needs testing
		break;
	}
	case audio_sample_type_float32:
	{
		out = *reinterpret_cast<real32*>(in);
		break;
	}
	default:
		HALT("Unknown sample type");
	}

	return out;
}

void c_audio_source_file_streamed::get_samples(t_audio_buffer_real32& out_buffer)
{
	const int32 sample_count = out_buffer.size();

	c_static_array<byte, 4096> temp_buffer;

	const int32 bytes_to_read = sample_count * m_format.block_align;
	ASSERT(bytes_to_read < temp_buffer.capacity());

	int32 bytes_read = m_file.read_bytes(bytes_to_read, temp_buffer);

	c_static_audio_buffer<real32, 2, 1024> audio_buffer;

	int32 sample_index = 0;
	const int32 bytes_per_sample = m_format.bits_per_sample / 8;
	for (int32 byte_index = 0; byte_index < bytes_read; byte_index += m_format.block_align)
	{
		for (int32 channenl_index = 0; channenl_index < m_format.channel_count; channenl_index++)
		{
			// TODO: this involves way too much switch logic. rework to just do the switch once
			real32 sample = convert_sample_to_real32(&temp_buffer.data()[byte_index + (bytes_per_sample * channenl_index)], m_format.sample_type);
			audio_buffer.get_channel(channenl_index)[sample_index] = sample;
		}

		sample_index++;
	}

	out_buffer.copy_from(audio_buffer);

	m_position += sample_index;

	if (bytes_read < bytes_to_read)
	{
		ASSERT(m_position == m_format.sample_count);

		if (m_looping)
		{
			m_position = 0;
			int32 second_pass_bytes_to_read = bytes_to_read - bytes_read;

			m_file.set_read_position(m_format.data_position);
			int32 second_pass = m_file.read_bytes(second_pass_bytes_to_read, temp_buffer);
			ASSERT(second_pass == bytes_to_read - bytes_read);

			sample_index = 0;
			for (int32 byte_index = 0; byte_index < second_pass; byte_index += m_format.block_align)
			{
				for (int32 channenl_index = 0; channenl_index < m_format.channel_count; channenl_index++)
				{
					// TODO: this involves way too much switch logic. rework to just do the switch once
					real32 sample = convert_sample_to_real32(&temp_buffer.data()[byte_index + (bytes_per_sample * channenl_index)], m_format.sample_type);
					audio_buffer.get_channel(channenl_index)[sample_index] = sample;
				}

				sample_index++;
			}

			m_position += sample_index;

			out_buffer.copy_from(audio_buffer);
		}
		else
		{
			m_HACK_finished = true;
			int32 begin = sample_count - sample_index;
			int32 end = sample_count;

			if (begin != end)
			{
				out_buffer.zero(begin, end);
			}
		}
	}
}
