#ifndef __AUDIO_TYPES_H__
#define __AUDIO_TYPES_H__
#pragma once

#include "types/types.h"
#include "debug/asserts.h"
#include "structures/string/string_id.h"

using t_sound_playback_id = uint64;

// bad name
enum e_sound_source_playback_type
{
	sound_source_playback_type_streamed,
	sound_source_playback_type_memory,

	k_sound_source_playback_type_count
};

struct s_sound_info
{
	/*s_sound_info(uint64 asset_id, e_sound_source_playback_type playback_type) :
		asset_id(asset_id),
		playback_type(playback_type) {}*/

	c_string_id asset_id;
	e_sound_source_playback_type playback_type;
};

struct s_sound_properties
{
	real32 gain;
};

// none of this stuff actually needs to be in common...
enum e_audio_sample_type
{
	audio_sample_type_unknown = k_invalid,
	audio_sample_type_int8,
	audio_sample_type_int16,
	audio_sample_type_int24,
	audio_sample_type_int32,
	audio_sample_type_real32,

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



#endif // __AUDIO_TYPES_H__