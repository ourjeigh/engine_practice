#include "pch.h"
#include "structures/audio/audio_buffer.h"

TEST(AUDIO, RING_BUFFER)
{
	c_audio_real32_mono_ring_buffer<8> ring_buffer;

	real32 data[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

	// this means that the data for each channel is not in contiguous memory...
	c_static_audio_buffer<real32, 1, 5> write_buffer;
	memory_copy(write_buffer.get_channel(0), &data[0], 5 * sizeof(real32));

	// bad
	ring_buffer.write(&write_buffer, write_buffer.size());

	c_static_audio_buffer<real32, 1, 5> output_buffer;
	ring_buffer.read(&output_buffer, output_buffer.size());

	for (int32 i = 0; i < 5; i++)
	{
		EXPECT_EQ(output_buffer.get_channel(0)[i], data[i]);
	}

	memory_copy(write_buffer.get_channel(0), &data[5], 5 * sizeof(real32));

	ring_buffer.write(&write_buffer, write_buffer.size());
	//zero_object(output_buffer);

	ring_buffer.read(&output_buffer, output_buffer.size());

	for (int32 i = 0; i < 5; i++)
	{
		EXPECT_EQ(output_buffer.get_channel(0)[i], data[i + 5]);
	}
}