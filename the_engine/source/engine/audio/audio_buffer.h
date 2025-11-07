#ifndef __AUDIO_BUFFER_H__
#define __AUDIO_BUFFER_H__
#pragma once

#include <types/types.h>
#include "structures/array.h"

template<typename t_type, int32 k_channel_count, int32 k_size>
class c_audio_ring_buffer
{
public:
	c_audio_ring_buffer() : m_front(0), m_back(0) { clear_struct(m_data); }
private:
	int32 m_front;
	int32 m_back;
	c_array<c_stack<real32, k_size>, k_channel_count> m_data;
};

#endif // __AUDIO_BUFFER_H__
