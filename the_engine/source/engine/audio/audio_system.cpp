#include "audio_system.h"
#include <logging/logging.h>
#include <threads/threads.h>
#include <time/time.h>
#include <asserts.h>

#include <Windows.h>
#include <comdef.h>
#include <combaseapi.h>
#include <mmdeviceapi.h>
#include <Audioclient.h>

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);

c_audio_render_thread g_audio_render_thread;
s_audio_format g_audio_format;


// public methods
void c_audio_system::init()
{
	g_audio_render_thread.init();
	log(verbose, "Audio System Initialized");
}

void c_audio_system::term()
{
	g_audio_render_thread.term();
	log(verbose, "Audio System Terminated");
}

void c_audio_system::update()
{
	//log(verbose, "Audio System Update");
}

// if we can't proceed past a failed call, log and abort
#define CALL_WIN_FUNC_LOG_CRITICAL_AND_RETURN_FALSE_ON_ERROR(x) do \
{ \
	HRESULT hres = x; \
	if (hres != S_OK) \
	{ \
		log(critical, "Windows Call: %s returned non S_OK: [%s]", #x, _com_error(hres).ErrorMessage()); \
		return false; \
	} \
} while (0)

#define CALL_WIN_FUNC_LOG_CRITICAL_AND_RETURN_ON_ERROR(x) do \
{ \
	HRESULT hres = x; \
	if (hres != S_OK) \
	{ \
		log(critical, "Windows Call: %s returned non S_OK: [%s]", #x, _com_error(hres).ErrorMessage()); \
		return; \
	} \
} while (0)

//https://learn.microsoft.com/en-us/windows/win32/coreaudio/rendering-a-stream?source=recommendations
bool c_audio_render_thread::setup_audio_device()
{
	ASSERT(GetCurrentThreadId() == this->get_thread_id());

	IMMDeviceEnumerator* device_enumerator = nullptr;
	IMMDevice* audio_device = nullptr;
	IAudioClient* audio_client = nullptr;
	IAudioRenderClient* render_client = nullptr;
	WAVEFORMATEX* mix_format =  nullptr;
	s_audio_format audio_format;

	CALL_WIN_FUNC_LOG_CRITICAL_AND_RETURN_FALSE_ON_ERROR(CoInitializeEx(
		NULL,
		COINIT_MULTITHREADED));

	CALL_WIN_FUNC_LOG_CRITICAL_AND_RETURN_FALSE_ON_ERROR(CoCreateInstance(
		CLSID_MMDeviceEnumerator, 
		NULL,
		CLSCTX_ALL, 
		IID_IMMDeviceEnumerator,
		(void**)&device_enumerator));

	CALL_WIN_FUNC_LOG_CRITICAL_AND_RETURN_FALSE_ON_ERROR(device_enumerator->GetDefaultAudioEndpoint(
		eRender, eConsole, &audio_device));

	CALL_WIN_FUNC_LOG_CRITICAL_AND_RETURN_FALSE_ON_ERROR(audio_device->Activate(
		IID_IAudioClient, CLSCTX_ALL,
		NULL, (void**)&audio_client));

	CALL_WIN_FUNC_LOG_CRITICAL_AND_RETURN_FALSE_ON_ERROR(audio_client->GetMixFormat(&mix_format));

	int64 device_period_default;
	int64 device_period_min;
	CALL_WIN_FUNC_LOG_CRITICAL_AND_RETURN_FALSE_ON_ERROR(audio_client->GetDevicePeriod(&device_period_default, &device_period_min));

	DWORD stream_flags = 0; //AUDCLNT_STREAMFLAGS_EVENTCALLBACK;

	CALL_WIN_FUNC_LOG_CRITICAL_AND_RETURN_FALSE_ON_ERROR(audio_client->Initialize(
		AUDCLNT_SHAREMODE_SHARED, // why can we not make this exclusive
		stream_flags,
		device_period_min,
		device_period_min,
		mix_format,
		NULL));

	UINT32 buffer_size = 0;
	CALL_WIN_FUNC_LOG_CRITICAL_AND_RETURN_FALSE_ON_ERROR(audio_client->GetBufferSize(&buffer_size));
	m_buffer_size = buffer_size;

	//m_callback_event_handle = CreateEvent(0, 0, 0, 0);
	//CALL_WIN_FUNC_LOG_CRITICAL_AND_RETURN_FALSE_ON_ERROR(audio_client->SetEventHandle(m_callback_event_handle));

	CALL_WIN_FUNC_LOG_CRITICAL_AND_RETURN_FALSE_ON_ERROR(audio_client->Start());

	m_audio_client = audio_client;

	CALL_WIN_FUNC_LOG_CRITICAL_AND_RETURN_FALSE_ON_ERROR(audio_client->GetService(IID_IAudioRenderClient, (void**)&m_audio_render_client));

	// the buffer needs to be filled twice every device period, so we'll track that value
	const uint64 reftimes_per_millisecond = 10000;
	m_device_period_ms = device_period_min / reftimes_per_millisecond / 2;

	// wasapi is always interleaved
	audio_format.interleaved = true;
	audio_format.num_channels = mix_format->nChannels;
	audio_format.sample_rate = mix_format->nSamplesPerSec;
	
	switch (mix_format->wBitsPerSample)
	{
	case 16:
		audio_format.sample_type = sample_type_int16;
		break;
	case 32:
		audio_format.sample_type = sample_type_float32;
		break;
	default:
		audio_format.sample_type = sample_type_unknown;
		break;
	}

	// make sure it's valid?
	g_audio_format = audio_format;

	audio_device->Release();
	device_enumerator->Release();

	// TEMP
	{
		m_HACK_test_noise = c_audio_source_noise(audio_format.sample_rate);
		m_HACK_test_sine = c_audio_source_sine(audio_format.sample_rate, 440.0f);
	}

	log(verbose, "Audio Device Setup success");
	return true;
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
	CloseHandle(m_callback_event_handle);
	// unregister all the stuff
	IAudioClient* audio_client = reinterpret_cast<IAudioClient*>(m_audio_client);
	IAudioRenderClient* render_client = reinterpret_cast<IAudioRenderClient*>(m_audio_render_client);

	if (render_client != nullptr)
	{
		render_client->Release();
		m_audio_render_client = nullptr;
	}

	if (audio_client != nullptr)
	{
		audio_client->Stop();
		audio_client->Release();
		m_audio_client = nullptr;
	}
}

// private methods
void c_audio_render_thread::audio_render_thread_entry_point(c_audio_render_thread* thread)
{
	if (thread->setup_audio_device())
	{
		while (thread->m_is_running)
		{
			//WaitForSingleObject(thread->m_callback_event_handle, INFINITE);

			thread->render_audio();
			sleep_for_milliseconds(thread->m_device_period_ms);
		}
	}

	thread->term();
}

void c_audio_render_thread::render_audio()
{
	IAudioClient* audio_client = reinterpret_cast<IAudioClient*>(m_audio_client);
	IAudioRenderClient* render_client = reinterpret_cast<IAudioRenderClient*>(m_audio_render_client);

	UINT32 buffer_frame_count = 0;
	audio_client->GetBufferSize(&buffer_frame_count);

	UINT32 numFramesAvailable;
	CALL_WIN_FUNC_LOG_CRITICAL_AND_RETURN_ON_ERROR(audio_client->GetCurrentPadding(&numFramesAvailable));
	buffer_frame_count = buffer_frame_count - numFramesAvailable;

	ASSERT(buffer_frame_count < 100000);

	if (buffer_frame_count > 0)
	{
		BYTE* data = nullptr;

		// TEMP
		real32 temp_buffer[2048];
		{
			m_HACK_test_sine.get_samples(&temp_buffer[0], buffer_frame_count);
		}

		CALL_WIN_FUNC_LOG_CRITICAL_AND_RETURN_ON_ERROR(render_client->GetBuffer(buffer_frame_count,	&data));
		ASSERT(data != nullptr);

		real32* realdata = reinterpret_cast<real32*>(data);

		c_audio_mixer::mix_a_into_b(&temp_buffer[0], realdata, buffer_frame_count, g_audio_format.num_channels);

		CALL_WIN_FUNC_LOG_CRITICAL_AND_RETURN_ON_ERROR(render_client->ReleaseBuffer(buffer_frame_count, 0));
	}
}