#include "config.h"
#ifdef PLATFORM_WIN64

#include "platform/platform_audio_sink.h"

#include "debug/logging.h"
#include "debug/asserts.h"
#include "platform/platform.h"

IGNORE_WINDOWS_WARNINGS_PUSH
#include "windows.h"
#include "comdef.h"
#include "combaseapi.h"
#include "mmdeviceapi.h"
#include "audioclient.h"
IGNORE_WINDOWS_WARNINGS_POP

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);

//// TODO move this to something like platform_windows.h
#define CALL_WIN_FUNC_LOG_CRITICAL_AND_RETURN_FALSE_ON_ERROR(x) do \
{ \
	HRESULT hres = x; \
	if (FAILED(hres)) \
	{ \
		log_message(critical, "Windows Call: %s returned non S_OK: [%s]", #x, _com_error(hres).ErrorMessage()); \
		return false; \
	} \
} while (0)

#define CALL_WIN_FUNC_LOG_CRITICAL_AND_RETURN_ON_ERROR(x) do \
{ \
	HRESULT hres = x; \
	if (FAILED(hres)) \
	{ \
		log_message(critical, "Windows Call: %s returned non S_OK: [%s]", #x, _com_error(hres).ErrorMessage()); \
		return; \
	} \
} while (0)

IAudioClient* g_audio_client;
IAudioRenderClient* g_audio_render_client;

c_platform_audio_sink::c_platform_audio_sink()
{
	m_device_period_ms = 0;
	m_render_active = false;
}

bool c_platform_audio_sink::register_sink(s_audio_device_format& inout_audio_format)
{
	//https://learn.microsoft.com/en-us/windows/win32/coreaudio/rendering-a-stream?source=recommendations

	IMMDeviceEnumerator* device_enumerator = nullptr;
	IMMDevice* audio_device = nullptr;
	WAVEFORMATEX* mix_format = nullptr;

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
		NULL, (void**)&g_audio_client));

	CALL_WIN_FUNC_LOG_CRITICAL_AND_RETURN_FALSE_ON_ERROR(g_audio_client->GetMixFormat(&mix_format));

	int64 device_period_default;
	int64 device_period_min;
	CALL_WIN_FUNC_LOG_CRITICAL_AND_RETURN_FALSE_ON_ERROR(g_audio_client->GetDevicePeriod(&device_period_default, &device_period_min));

	DWORD stream_flags = 0;

	CALL_WIN_FUNC_LOG_CRITICAL_AND_RETURN_FALSE_ON_ERROR(g_audio_client->Initialize(
		AUDCLNT_SHAREMODE_SHARED,
		stream_flags,
		device_period_min,
		device_period_min,
		mix_format,
		NULL));

	UINT32 buffer_size = 0;
	CALL_WIN_FUNC_LOG_CRITICAL_AND_RETURN_FALSE_ON_ERROR(g_audio_client->GetBufferSize(&buffer_size));
	inout_audio_format.buffer_size = buffer_size;

	CALL_WIN_FUNC_LOG_CRITICAL_AND_RETURN_FALSE_ON_ERROR(g_audio_client->Start());

	CALL_WIN_FUNC_LOG_CRITICAL_AND_RETURN_FALSE_ON_ERROR(g_audio_client->GetService(IID_IAudioRenderClient, (void**)&g_audio_render_client));

	// the buffer needs to be filled twice every device period, so we'll track that value
	const real32 reftimes_per_millisecond = 10000.0f;
	m_device_period_ms = static_cast<uint32>(device_period_min / reftimes_per_millisecond / 2.0f);

	inout_audio_format.channel_count = mix_format->nChannels;
	inout_audio_format.sample_rate = mix_format->nSamplesPerSec;

	switch (mix_format->wBitsPerSample)
	{
	case 16:
		inout_audio_format.sample_type = audio_sample_type_int16;
		break;
	case 32:
		inout_audio_format.sample_type = audio_sample_type_float32;
		break;
	default:
		inout_audio_format.sample_type = audio_sample_type_unknown;
		break;
	}

	audio_device->Release();
	device_enumerator->Release();

	return true;
}

bool c_platform_audio_sink::unregister_sink()
{
	// unregister all the stuff

	if (g_audio_render_client != nullptr)
	{
		g_audio_render_client->Release();
		g_audio_render_client = nullptr;
	}

	if (g_audio_client != nullptr)
	{
		g_audio_client->Stop();
		g_audio_client->Release();
		g_audio_client = nullptr;
	}

	return true;
}

bool c_platform_audio_sink::begin_render(real32*& out_buffer, uint32& out_available_samples)
{
	ASSERT(!m_render_active);

	bool success = false;
	out_buffer = nullptr;
	UINT32 buffer_frame_count = 0;

	g_audio_client->GetBufferSize(&buffer_frame_count);

	UINT32 numFramesAvailable;
	CALL_WIN_FUNC_LOG_CRITICAL_AND_RETURN_FALSE_ON_ERROR(g_audio_client->GetCurrentPadding(&numFramesAvailable));
	buffer_frame_count -= numFramesAvailable;

	out_available_samples = buffer_frame_count;

	if (buffer_frame_count > 0)
	{
		BYTE* data = nullptr;

		CALL_WIN_FUNC_LOG_CRITICAL_AND_RETURN_FALSE_ON_ERROR(g_audio_render_client->GetBuffer(buffer_frame_count, &data));
		ASSERT(data != nullptr);
		out_buffer = reinterpret_cast<real32*>(data);

		success = true;
		m_render_active = true;
	}

	return success;
}

void c_platform_audio_sink::render_complete(uint32 samples_filled)
{
	ASSERT(m_render_active);

	CALL_WIN_FUNC_LOG_CRITICAL_AND_RETURN_ON_ERROR(g_audio_render_client->ReleaseBuffer(samples_filled, 0));
	m_render_active = false;

}
#endif // PLATFORM_WINDOWS