#include "assets/asset_system.h"
#include "audio_source.h"
#include "audio_system.h"
#include "debug/asserts.h"
#include "debug/logging.h"
#include "engine/audio/audio_threadsafe_buffer.h"
#include "memory/allocator.h"
#include "memory/memory_system.h"
#include "platform/platform_thread.h"
#include "rendering/render_system.h"
#include "structures/hash_set.h"
#include "threads/threads.h"
#include "time/time.h"


// move to audio helper
constexpr real32 db_to_linear_amplitude(real32 db)
{
	return math_pow(10, db / 20);
}

const int32 k_audio_engine_buffer_size = 512;
const int32 k_audio_output_buffer_size = k_audio_engine_buffer_size * 16;
const int32 k_audio_engine_sample_rate = 48000;

const real32 k_max_output_level_db = 24;
constexpr real32 k_max_output_level_linear = db_to_linear_amplitude(k_max_output_level_db);

struct s_audio_playback
{
	s_audio_playback() : id(k_invalid), source(nullptr) {}

	t_sound_playback_id id;
	c_audio_source* source;
};

// private prototypes
void stop_audio_playback_internal(s_audio_playback* playback);

// data
c_audio_engine_thread g_audio_engine_thread;

// TODO: move this, maybe to application, that's what owns the graphics renderer
c_audio_render_thread g_audio_render_thread;
s_audio_device_format g_audio_format;

// TODO: move this all to an allocated state
c_audio_threadsafe_ring_buffer<real32>* g_audio_output_ring_buffer;

t_sound_playback_id g_sound_id_top = 0;

c_static_array<s_audio_playback, 64> g_audio_playbacks;
c_hash_map<t_sound_playback_id, c_audio_source_file, 128> g_audio_playback_source_file_map;
c_hash_map<t_sound_playback_id, c_audio_source_sine, 8> g_audio_playback_source_sine_map;

// public methods
void c_audio_system::init()
{
	zero_object(g_audio_format);

	g_audio_render_thread.init();

	// HACK? we need to wait for the audio render thread to setup the audio device and fill in the format
	// replace with a flag checking for audio sync setup
	while (g_audio_format.sample_rate == 0)
	{
		thread_sleep_for_milliseconds(100);
	}
	
	{
		uint64 size = sizeof(real32) * k_audio_output_buffer_size * g_audio_format.channel_count;
		real32* buffer_data = reinterpret_cast<real32*>(c_memory_system::allocate(size, alignof(real32), memory_arena_system));
		g_audio_output_ring_buffer = ALLOCATE_GLOBAL_NO_CONSTRUCTOR(c_audio_threadsafe_ring_buffer<real32>, memory_arena_system);

		g_audio_output_ring_buffer->init(g_audio_format.channel_count, k_audio_output_buffer_size, buffer_data);
	}

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
		render_system_draw_string(string, 5, 5, 1, k_color_white, render_layer_debug);
#endif //CONFIG_DEBUG
	}
}

t_sound_playback_id c_audio_system::play_sound(const s_wav_asset& asset)
{
	// todo: This either needs to go faster than iterating thru g_audio_playbacks, or (preferrably) it should just add
	// the playrequest to a queue that we'll process in update()

	for (auto it = g_audio_playbacks.begin(); it != g_audio_playbacks.end(); ++it)
	{
		if (it->id == k_invalid)
		{
			if (!g_audio_playback_source_file_map.full())
			{
				t_sound_playback_id new_id = g_sound_id_top++;
				bool found = false;
				c_audio_source_file& source = g_audio_playback_source_file_map.find_or_insert(new_id, found);
				source.set_buffer(asset.buffer);
				
				it->id = new_id;
				it->source = &source;

				// TODO: this should have a human-readable name to output
				log_message(verbose, "audio_system: play_sound: [id:{u}]", it->id);

				return it->id;
			}
			else
			{
				log_message(error, "audio_system: could not create new playback source file, map full ({d} sources)",
					g_audio_playback_source_file_map.used());
				return k_invalid;
			}
		}
	}
	
	log_message(warning, "audio_system: could not start playback, playbacks list is full!");
	return k_invalid;
}

// todo: this should take in a s_sound_properties, so we can start playback with correct gain, etc;
t_sound_playback_id c_audio_system::play_sound(s_sound_info& info)
{
	const s_wav_asset* asset = static_cast<const s_wav_asset*>(c_asset_system::get_asset(info.asset_id));

	if (asset != nullptr)
	{
		return play_sound(*asset);
	}
	else
	{
		log_message(warning, "audio_system: play_sound: could not get asset data for sound [id {u}], name: {s}", info.asset_id.get_id(), info.asset_id.get_debug_string());
		return k_invalid;
	}

	return k_invalid;
}

t_sound_playback_id c_audio_system::play_debug_pip()
{
	for (auto it = g_audio_playbacks.begin(); it != g_audio_playbacks.end(); ++it)
	{
		if (it->id == k_invalid)
		{
			if (!g_audio_playback_source_sine_map.full())
			{
				t_sound_playback_id new_id = g_sound_id_top++;
				bool found = false;
				c_audio_source_sine& source = g_audio_playback_source_sine_map.find_or_insert(new_id, found);
				source.set_frequency(1000);
				it->id = new_id;
				it->source = &source;

				log_message(verbose, "audio_system: play_pip: [id:{u}]", it->id);

				return it->id;
			}
			else
			{
				log_message(error, "audio_system: could not create new playback source sine, map full ({d} sources)",
					g_audio_playback_source_sine_map.used());
				return k_invalid;
			}
		}
	}

	log_message(warning, "audio_system: could not start playback, playbacks list is full!");
	return k_invalid;
}

void c_audio_system::update_sound(t_sound_playback_id playback_id, s_sound_properties& properties)
{
	// TODO
}

void c_audio_system::stop_sound(t_sound_playback_id playback_id)
{
	for (auto it = g_audio_playbacks.begin(); it != g_audio_playbacks.end(); ++it)
	{
		if (it->id == playback_id)
		{
			stop_audio_playback_internal(it.get_item());

			return;
		}
	}

	log_message(verbose, "audio_system:stop_sound couldn't find playback id: [id:{u}]", playback_id);
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

	c_platform_handle timer_handle = platform_thread_create_waitable_timer(false, true, t_string_128("unused"));
	ASSERT(timer_handle.is_valid());

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
			real64 wait_duration_ms = update_period_ms - time_span_ms - sleep_padding_ms;
			ASSERT(platform_thread_start_waitable_timer(timer_handle, real64_to_uint32(wait_duration_ms), 0));
			platform_thread_wait_for_signalled_object(timer_handle);
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
			playbacks_processed++;
			c_static_audio_buffer<real32, 2, k_audio_engine_buffer_size> temp_buffer;
			temp_buffer.zero();

			it->source->get_samples(temp_buffer);
			
			if (it->source->HACK_finished() ||
				// temp - stop a pip after a single buffer
				it->source->type() == audio_source_type_sine)
			{
				stop_audio_playback_internal(it.get_item());
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

	if (playbacks_processed > 0)
	{
		const real32 inv_source_count = unsafe_divide(1.0f, playbacks_processed);

		for (int32 channel_index = 0; channel_index < mix_buffer.channel_count(); channel_index++)
		{
			real32* mix_channel = mix_buffer.get_channel(channel_index);

			for (int32 sample_index = 0; sample_index < k_audio_engine_buffer_size; sample_index++)
			{
				mix_channel[sample_index] *= inv_source_count;
			}
		}
	}

#ifdef CONFIG_DEBUG
	// catch various issues with the audio signal immediately that can be hard to track down otherwise:
	// - very loud signals
	// - uninitialized samples
	// - divide by zero samples (-inf)
	for (int32 channel = 0; channel < mix_buffer.channel_count(); channel++) 
	{
		const real32* channel_buffer = mix_buffer.get_channel_const(channel);
		for (int32 sample = 0; sample < mix_buffer.size(); sample++)
		{
			ASSERT(math_abs(channel_buffer[sample]) < k_max_output_level_linear);
		}
	}
#endif // CONFIG_DEBUG

	// we need to be able to write the full mix_buffer, so wait until there's room.
	// TODO: we could allow the sink thread to alert this thread when there is
	// enough space to consume, and eliminate the polling here completely.
	while (g_audio_output_ring_buffer->free_sample_count() < mix_buffer.size())
	{
		c_platform_handle timer_handle = platform_thread_create_waitable_timer(true, true, t_string_128("unused"));
		ASSERT(timer_handle.is_valid());
		platform_thread_start_waitable_timer(timer_handle, 1, 0);
		platform_thread_wait_for_signalled_object(timer_handle);
		timer_handle.close();
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

	if (m_sink.register_sink(requested_format, m_render_event_handle))
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
			platform_thread_wait_for_signalled_object(thread->m_render_event_handle, k_wait_time_infinite);

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

// private
void stop_audio_playback_internal(s_audio_playback* playback)
{
	log_message(verbose, "audio_system:stop_sound stopped sound: [id:{u}]", playback->id);

	switch (playback->source->type())
	{
	case audio_source_type_file:
		g_audio_playback_source_file_map.remove(playback->id);
		break;
	case audio_source_type_sine:
		g_audio_playback_source_sine_map.remove(playback->id);
		break;
	default:
		HALT_UNIMPLEMENTED();
	}

	playback->id = k_invalid;
	playback->source = nullptr;
}