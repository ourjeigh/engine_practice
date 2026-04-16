#ifndef __AUDIO_TYPES_H__
#define __AUDIO_TYPES_H__
#pragma once

#include <types/types.h>

enum e_audio_sample_type
{
	audio_sample_type_unknown = k_invalid,
	audio_sample_type_int8,
	audio_sample_type_int16,
	audio_sample_type_int24,
	audio_sample_type_int32,
	audio_sample_type_float32,

	k_audio_sample_type_count
};

enum e_audio_compression_format
{
	audio_compression_format_unknown = k_invalid,
	audio_compression_format_pcm,
	audio_compression_format_adpcm,

	k_audio_compression_format_count
};

struct s_audio_device_format
{
	e_audio_sample_type sample_type;
	uint16 channel_count;
	uint32 sample_rate;
	uint16 buffer_size;
};

struct s_audio_file_format
{
	e_audio_sample_type sample_type;
	uint16 channel_count;
	uint16 bits_per_sample;
	uint16 block_align;
	uint32 sample_rate;
	uint32 data_position;
	uint64 sample_count;
};

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

#endif // __AUDIO_TYPES_H__