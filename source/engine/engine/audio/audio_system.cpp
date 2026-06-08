#include "audio_system.h"
#include "assets/asset_system.h"
#include "debug/logging.h"
#include "debug/asserts.h"
#include "threads/threads.h"
#include "time/time.h"
#include "platform/platform.h"
#include "memory/allocator.h"
#include "memory/memory_system.h"
#include "engine/audio/audio_threadsafe_buffer.h"
#include "rendering/render_system.h"

const int32 k_audio_engine_buffer_size = 512;
const int32 k_audio_output_buffer_size = k_audio_engine_buffer_size * 16;
const int32 k_audio_engine_sample_rate = 48000;

c_audio_engine_thread g_audio_engine_thread;

// TODO: move this, maybe to application, that's what owns the graphics renderer
c_audio_render_thread g_audio_render_thread;
s_audio_device_format g_audio_format;

//c_audio_threadsafe_static_ring_buffer<real32, 2, k_audio_output_buffer_size> g_audio_output_ring_buffer;
c_audio_threadsafe_ring_buffer<real32>* g_audio_output_ring_buffer;

c_static_stack_allocator<k_byte_mb>* g_audio_source_allocator;
t_sound_playback_id g_sound_id_top = 0;

struct s_sound_playback
{
	s_sound_playback() : id(k_invalid), source(nullptr) {}

	t_sound_playback_id id;
	c_audio_source* source;
};

c_static_array<s_sound_playback, 64> g_audio_playbacks;

// public methods
void c_audio_system::init()
{
	zero_object(g_audio_format);

	g_audio_render_thread.init();

	// HACK? we need to wait for the audio render thread to setup the audio device and fill in the format
	// replace with a flag checking for audio sync setup
	while (g_audio_format.sample_rate == 0)
	{
		sleep_for_milliseconds(10);
	}
	
	{
		uint64 size = sizeof(real32) * k_audio_output_buffer_size * g_audio_format.channel_count;
		real32* buffer_data = reinterpret_cast<real32*>(c_memory_system::allocate(size, alignof(real32), memory_arena_system));
		g_audio_output_ring_buffer = ALLOCATE_GLOBAL_NO_CONSTRUCTOR(c_audio_threadsafe_ring_buffer<real32>, memory_arena_system);

		g_audio_output_ring_buffer->init(g_audio_format.channel_count, k_audio_output_buffer_size, buffer_data);
	}

	g_audio_source_allocator = ALLOCATE_NEW_GLOBAL(c_static_stack_allocator<k_byte_mb>, memory_arena_system);

	g_audio_engine_thread.init();

	log_message(verbose, "Audio System Initialized");
}

void c_audio_system::term()
{
	g_audio_engine_thread.term();
	g_audio_render_thread.term();

	log_message(verbose, "Audio System Terminated");
}

void c_audio_system::update()
{
	// update audio sources and mix
	
	if (false)
	{
		int32 active_sounds = 0;
		for (auto it = g_audio_playbacks.begin(); it != g_audio_playbacks.end(); ++it)
		{
			if (it->id != k_invalid)
			{
				active_sounds++;
			}
		}

#ifdef CONFIG_DEBUG
		t_string_128 string;
		string.printf("Active Sounds: {i}", active_sounds);
		render_system_draw_debug_string(string, 5, 5, 1, k_color_white);
#endif //CONFIG_DEBUG
	}
}

t_sound_playback_id c_audio_system::play_sound(const s_wav_asset& asset)
{
	c_audio_source_file* source = ALLOCATE_NEW(c_audio_source_file, *g_audio_source_allocator);
	source->set_buffer(asset.buffer);

	for (auto it = g_audio_playbacks.begin(); it != g_audio_playbacks.end(); ++it)
	{
		if (it->id == k_invalid)
		{
			it->id = g_sound_id_top++;
			it->source = source;

			//log_message(verbose, "audio_system: play_sound: [id:{u} name:{s}]", it->id, info.asset_id.get_debug_string()); // make this the string when it's a string_id

			return it->id;
		}
	}
	return k_invalid;
}

// todo: this should take in a s_sound_properties, so we can start playback with correct gain, etc;
t_sound_playback_id c_audio_system::play_sound(s_sound_info& info)
{
	// todo: This either needs to go faster than iterating thru g_audio_playbacks, or (preferrably) it should just add
	// the playrequest to a queue that we'll process in update()
	c_audio_source_file* source =  ALLOCATE_NEW(c_audio_source_file, *g_audio_source_allocator);
	
	const c_array<byte>* sound_data = c_asset_system::get_asset_data(info.asset_id);
	
	if (sound_data != nullptr && sound_data->is_valid())
	{
		source->set_memory(sound_data);
	
		for (auto it = g_audio_playbacks.begin(); it != g_audio_playbacks.end(); ++it)
		{
			if (it->id == k_invalid)
			{
				it->id = g_sound_id_top++;
				it->source = source;

				log_message(verbose, "audio_system: play_sound: [id:{u} name:{s}]", it->id, info.asset_id.get_debug_string());

				return it->id;
			}
		}
	}
	else
	{
		log_message(warning, "audio_system: play_sound: could not get asset data for sound [id {u}], name: {s}", info.asset_id.get_id(), info.asset_id.get_debug_string()); // make this string
		return k_invalid;
	}

	log_message(warning, "audio_system: could not start playback, playbacks list is full! [file:{s}]", info.asset_id.get_debug_string());
	return k_invalid;
}

void c_audio_system::update_sound(t_sound_playback_id playback_id, s_sound_properties& properties)
{
	// TODO
}

void c_audio_engine_thread::init()
{
	// start render loop
	m_is_running = true;

	s_thread_properties properties;
	properties.function = THREAD_FUNCTION(audio_engine_thread_entry_point);
	properties.param = THREAD_ARGS(this);
	properties.name = WIDE("Audio Engine Thread");
	properties.priority = thread_priority_time_critical;

	create(properties);
	start();
}

void c_audio_engine_thread::term()
{
	m_is_running = false;
	join();
}

void c_audio_engine_thread::audio_engine_thread_entry_point(c_audio_engine_thread* thread)
{
	g_audio_format.buffer_size = k_audio_engine_buffer_size;

	const real64 update_period_ms = static_cast<real32>(g_audio_format.buffer_size) / g_audio_format.sample_rate * 1000.0f;

	while (thread->m_is_running)
	{
		c_timer timer;
		timer.start();

		thread->process_audio();

		timer.stop();
		real64 time_span_ms = timer.get_time_span().get_duration_milliseconds();

		const real64 sleep_padding_ms = 1.0f;
		if (update_period_ms - time_span_ms > sleep_padding_ms)
		{
			real64 sleep_duration_milliseconds = update_period_ms - time_span_ms - sleep_padding_ms;
			sleep_for_milliseconds(real64_to_uint32(sleep_duration_milliseconds));
		}

		while (update_period_ms - timer.get_time_span().get_duration_milliseconds() > 0.5f)
		{
			NOP();
		}
	}
}

void c_audio_engine_thread::process_audio()
{
	c_static_audio_buffer<real32, 2, k_audio_engine_buffer_size> mix_buffer;
	mix_buffer.zero();
	
	real32 playbacks_processed = 0.0f;
	for (auto it = g_audio_playbacks.begin(); it != g_audio_playbacks.end(); ++it)
	{
		if (it->id != k_invalid)
		{
			//log_message(verbose, "audio_system: processing sound: {u}", it->id);

			playbacks_processed++;
			c_static_audio_buffer<real32, 2, k_audio_engine_buffer_size> temp_buffer;
			it->source->get_samples(temp_buffer);
			
			if (it->source->HACK_finished())
			{
				it->source = nullptr;
				it->id = k_invalid;
			}

			// this should be turned into a helper add_a_into_b()
			for (int32 channel_index = 0; channel_index < temp_buffer.channel_count(); channel_index++)
			{
				real32* mix_channel = mix_buffer.get_channel(channel_index);
				real32* temp_channel = temp_buffer.get_channel(channel_index);

				for (int32 sample_index = 0; sample_index < k_audio_engine_buffer_size; sample_index++)
				{
					mix_channel[sample_index] += temp_channel[sample_index];
				}
			}
		}
	}

	const real32 inv_source_count = 1 / playbacks_processed;

	for (int32 channel_index = 0; channel_index < mix_buffer.channel_count(); channel_index++)
	{
		real32* mix_channel = mix_buffer.get_channel(channel_index);

		for (int32 sample_index = 0; sample_index < k_audio_engine_buffer_size; sample_index++)
		{
			mix_channel[sample_index] *= inv_source_count;
		}
	}

	// temp, make stereo
	//memory_copy(mix_buffer.get_channel(1), mix_buffer.get_channel(0), sizeof(real32) * mix_buffer.size());
	//memory_zero(mix_buffer.get_channel(1), sizeof(real32) * mix_buffer.size());
	// we need to be able to write the full mix_buffer, so wait until there's room.
	while (g_audio_output_ring_buffer->free_sample_count() < mix_buffer.size())
	{
		NOP();
	}

	int32 samples_written = g_audio_output_ring_buffer->write(&mix_buffer, mix_buffer.size());

	ASSERT(samples_written == mix_buffer.size());
}

bool c_audio_render_thread::setup_audio_sink()
{
	bool result = false;
	ASSERT(get_current_thread_id() == this->get_thread_id());

	// temp: move to settings
	s_audio_device_format requested_format;
	
	// inherit system channel count
	requested_format.channel_count = k_invalid;
	requested_format.sample_rate = k_audio_engine_sample_rate;
	requested_format.sample_type = audio_sample_type_real32;

	if (m_sink.register_sink(requested_format))
	{
		g_audio_format = requested_format;

		result = true;
		log_message(verbose, "Audio Sink registration succeeded");
	}
	else
	{
		log_message(critical, "Audio Sink registration failed");
	}

	return result;
}

void c_audio_render_thread::init()
{
	// start render loop
	m_is_running = true;
	create(THREAD_FUNCTION(audio_render_thread_entry_point), THREAD_ARGS(this), WIDE("Audio Render Thread"));
	start();
}

void c_audio_render_thread::term()
{
	m_is_running = false;
	join();
}

// private methods
void c_audio_render_thread::audio_render_thread_entry_point(c_audio_render_thread* thread)
{
	if (thread->setup_audio_sink())
	{
		while (thread->m_is_running)
		{
			sleep_for_milliseconds(thread->m_device_period_ms);
			thread->render_audio();
		}
	}
	
	thread->shutdown_audio_sink();
}

void c_audio_render_thread::shutdown_audio_sink()
{
	m_sink.unregister_sink();
}

void c_audio_render_thread::render_audio()
{
	// replace this with a flag check to verify audio sink is started
	if (g_audio_output_ring_buffer == nullptr || !g_audio_output_ring_buffer->is_initialized()) return;

	real32* buffer = nullptr;
	uint32 buffer_size;

	if (m_sink.begin_render(buffer, buffer_size))
	{
		ASSERT(buffer != nullptr);
		ASSERT(g_audio_output_ring_buffer->channel_count() == g_audio_format.channel_count);
		IF_DEBUG(int32 read_samples = ) g_audio_output_ring_buffer->read_interleaved(buffer, buffer_size);

#ifdef CONFIG_DEBUG
		// once we have begun reading, we should always be able to read the full amount
		// anything else will result in an audible pop which we never want.
		if (read_samples > 0)
		{
			d_read_initialized = true;
		}
		
		// not reliable enough
		if (d_read_initialized && read_samples != buffer_size)
		{
			log_message(warning, "c_audio_render_thread::render_audio: unable to read full output buffer for rendering [needed: {u}, got: {i}]",
				buffer_size, read_samples);
		}
#endif

		m_sink.render_complete(buffer_size);
	}
}

uint32 audio_system_get_sample_rate()
{
	return g_audio_format.sample_rate;
}

const s_audio_device_format& audio_get_format()
{
	return g_audio_format;
}
