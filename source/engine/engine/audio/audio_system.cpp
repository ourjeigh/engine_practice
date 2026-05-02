#include "audio_system.h"
#include <debug/logging.h>
#include <debug/asserts.h>
#include <threads/threads.h>
#include <time/time.h>
#include <platform/platform.h>

const int32 k_audio_engine_buffer_size = 512;
const int32 k_audio_output_buffer_size = k_audio_engine_buffer_size * 16;

c_audio_engine_thread g_audio_engine_thread;
c_audio_render_thread g_audio_render_thread;
s_audio_device_format g_audio_format;

c_audio_threadsafe_ring_buffer<real32, 2, k_audio_output_buffer_size> g_audio_output_ring_buffer;

// public methods
void c_audio_system::init()
{
	zero_object(g_audio_format);

	g_audio_render_thread.init();

	// HACK? we need to wait for the audio render thread to setup the audio device and fill in the format
	while (g_audio_format.sample_rate == 0)
	{
		sleep_for_milliseconds(10);
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
	//log(verbose, "Audio System Update");

	// update audio sources and mix
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

	// TEMP
	{
		thread->m_HACK_test_noise = c_audio_source_noise(g_audio_format.sample_rate);
		thread->m_HACK_test_sine = c_audio_source_sine(g_audio_format.sample_rate, 440.0f);
		//thread->m_HACK_test_file.set_file(c_file_path("C:\\Users\\RJ\\Desktop\\pelican_mono.wav"));
	}

	const real64 update_period_ms = static_cast<real32>(g_audio_format.buffer_size) / g_audio_format.sample_rate * 1000.0f;

	while (thread->m_is_running)
	{
		c_timer timer;
		timer.start();

		thread->process_audio();

		timer.stop();
		real64 time_span_ms = timer.get_time_span()->get_duration_milliseconds();

		const real64 sleep_padding_ms = 1.0f;
		if (update_period_ms - time_span_ms > sleep_padding_ms)
		{
			real64 sleep_duration_milliseconds = update_period_ms - time_span_ms - sleep_padding_ms;
			sleep_for_milliseconds(real64_to_uint32(sleep_duration_milliseconds));
		}

		while (update_period_ms - timer.get_time_span()->get_duration_milliseconds() > 0.5f)
		{
			NOP();
		}
	}
}

void c_audio_engine_thread::process_audio()
{
	c_static_audio_buffer<real32, 2, k_audio_engine_buffer_size> mix_buffer;
	
	//m_HACK_test_sine.get_samples(mix_buffer);
	//m_HACK_test_noise.get_samples(mix_buffer);
	//m_HACK_test_file.get_samples(mix_buffer);

	// temp, make stereo
	memory_copy(mix_buffer.get_channel(1), mix_buffer.get_channel(0), sizeof(real32) * mix_buffer.size());

	// we need to be able to write the full mix_buffer, so wait until there's room.
	while (g_audio_output_ring_buffer.free_sample_count() < mix_buffer.size())
	{
		NOP();
	}

	int32 samples_written = g_audio_output_ring_buffer.write(&mix_buffer, mix_buffer.size());

	ASSERT(samples_written == mix_buffer.size());
}

bool c_audio_render_thread::setup_audio_sink()
{
	bool result = false;
	ASSERT(get_current_thread_id() == this->get_thread_id());

	if (m_sink.register_sink(g_audio_format))
	{
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
			thread->render_audio();
			sleep_for_milliseconds(thread->m_device_period_ms);
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
	real32* buffer = nullptr;
	uint32 buffer_size;

	if (m_sink.begin_render(buffer, buffer_size))
	{
		ASSERT(buffer != nullptr);
		ASSERT(g_audio_output_ring_buffer.channel_count() == g_audio_format.channel_count);
		IF_DEBUG(int32 read_samples = ) g_audio_output_ring_buffer.read_interleaved(buffer, buffer_size);

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

uint32 audio_get_sample_rate()
{
	return g_audio_format.sample_rate;
}

const s_audio_device_format& audio_get_format()
{
	return g_audio_format;
}
