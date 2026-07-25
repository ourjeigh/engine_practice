#include "assets/asset_system.h"
#include "debug/logging.h"
#include "memory/memory_system.h"
#include "platform/platform.h"
#include "platform/platform_handle.h"
#include "structures/string/string_id.h"
#include "threads/threads.h"
#include "structures/hash_set.h"
#include "threads/threadsafe_queue.h"
#include "engine/audio/audio_system.h"
#include "time/time.h"

// temp: build signalling into thread class
#include "platform/platform_thread.h"

const int32 k_max_asset_load_requests = 32;
const int32 k_max_active_assets = 128;

// TODO: Streamable assets
// - long wavs should only load & prep the "next" chunk needed.
// - it would be cool to have a very large bmp and only load requested chunks of it at a time
//		- this could let us have a continuous background that we just load and unload from a single file in chunks
//		- it would need to track individual chunks non-contiguously (so that we could unload chunk 1 and load chunk 4
//		- caller declares chunk size (eg rect pixels) and max concurrent chunks needed, for allocation and indexing into file
//		- the logic would be basically the same for wavs, wavs would just presumably only need 2 chunks, this and next.
//		- need a way to access the chunk in a way that the existing render_bitmap can handle (or rework)
//		- would need to be able to render a portion of multiple chunks (eg you're in the middle of chunks 2 & 3), and be able to clip the offscreen portion
// - asset_system_load_streamed_asset(chunk_size, chunk_count)
// - asset_system_load_streamed_asset_chunk(chunk_index)
// - asset_system_unload_streamed_asset_chunk(chunk_index)

struct s_load_asset_request
{
	s_asset_definition asset_definition;
	void* object;
	f_asset_loaded_callback* callback;
};

struct s_asset_internal
{
	c_string_id asset_id;
	s_asset* asset;
	int32 ref_count;
	c_array<byte> memory;//remove
};

struct s_asset_internal_comparitor
{
	bool operator()(const uint64& lhs, const uint64& rhs)
	{
		return lhs == rhs;
	}
};

struct s_asset_internal_hasher
{
	uint32 operator()(const uint64& value)
	{
		// the asset handle is just a hash of the filepath so hashing is a NOP
		return value;
	}
};

class c_asset_loader_thread : public c_thread
{
public:
	void init();
	void term();

private:
	static_member_function void asset_loader_thread_entry_point(c_asset_loader_thread* thread);
	void process_asset_loads();

	bool m_is_running;
};

void process_new_asset(s_load_asset_request& request, s_asset_internal& asset);
void process_bitmap_asset(c_array<byte> const_ptr asset_data, s_bitmap_asset& out_bitmap_info);
void process_wav_asset(c_array<byte> const_ptr asset_data, s_wav_asset& out_wav_asset);

using t_asset_request_stack = c_static_spsc_queue<s_load_asset_request, k_max_asset_load_requests>;
t_asset_request_stack* g_asset_load_requests;

using t_asset_hash_map = c_hash_map<uint64, s_asset_internal, k_max_active_assets, s_asset_internal_hasher, s_asset_internal_comparitor>;
t_asset_hash_map* g_active_assets;

c_asset_loader_thread g_asset_loader_thread;
c_platform_handle g_asset_loader_thread_wake_event;

void c_asset_system::init()
{
	g_asset_load_requests = ALLOCATE_NEW_GLOBAL(t_asset_request_stack, memory_arena_system);
	g_active_assets = ALLOCATE_NEW_GLOBAL(t_asset_hash_map, memory_arena_system);

	ASSERT(g_asset_load_requests != nullptr);
	ASSERT(g_active_assets != nullptr);

	g_asset_load_requests->clear();
	g_active_assets->clear();

	g_asset_loader_thread_wake_event = platform_thread_create_event(false, false, t_string_128("process assets event"));
	ASSERT(g_asset_loader_thread_wake_event.is_valid());

	g_asset_loader_thread.init();
}

void c_asset_system::term()
{
	ASSERT(platform_thread_signal_event(g_asset_loader_thread_wake_event));
	g_asset_loader_thread_wake_event.close();
	g_asset_loader_thread.term();
}


void c_asset_system::update()
{
	if (g_asset_load_requests != nullptr && !g_asset_load_requests->empty())
	{
		ASSERT(platform_thread_signal_event(g_asset_loader_thread_wake_event));
	}
}

// todo: f_asset_loaded_callback should include a result so that we can tell the caller if the asset failed to load for some reason.
bool c_asset_system::load_asset(const s_asset_definition* asset_def, f_asset_loaded_callback* callback, void* object)
{
	ASSERT(g_asset_load_requests != nullptr);
	
	bool success = false;

	s_load_asset_request request;
	request.asset_definition = *asset_def;
	request.object = object;
	request.callback = callback;
	
	if (g_asset_load_requests->push_back(request))
	{
		success = true;
	}

	return success;
}

bool c_asset_system::load_asset_list(const c_array<s_asset_definition>& asset_list, f_asset_loaded_callback* callback, void* object)
{
	HALT_UNIMPLEMENTED();
	// we'll need a mechanism to track a group of asset requests together so that we only send the callback when all are loaded.
	return false;
}

const s_asset* c_asset_system::get_asset(c_string_id asset_id)
{
	ASSERT(get_current_thread_id() != g_asset_loader_thread.get_thread_id());

	s_asset_internal* asset;
	
	if (g_active_assets->try_find(asset_id.get_id(), &asset))
	{
		if (asset->memory.is_valid())
		{
			return asset->asset;
		}
	}

	return nullptr;
}

void c_asset_system::unload_asset(c_string_id asset_id)
{
	// find asset
	// release memory
	// remove asset
}

void c_asset_loader_thread::init()
{
	m_is_running = true;

	s_thread_properties properties;
	properties.function = THREAD_FUNCTION(asset_loader_thread_entry_point);
	properties.param = THREAD_ARGS(this);
	properties.name = WIDE("Asset Loader Thread");
	properties.priority = thread_priority_normal;

	create(properties);
	start();
}

void c_asset_loader_thread::term()
{
	m_is_running = false;
	join();
}

void c_asset_loader_thread::asset_loader_thread_entry_point(c_asset_loader_thread* thread)
{
	while (thread->m_is_running)
	{
		if (platform_thread_wait_for_signalled_object(g_asset_loader_thread_wake_event) == signalled_object_result_signalled)
		{
			thread->process_asset_loads();
		}
	}
}

void c_asset_loader_thread::process_asset_loads()
{

	while (!g_asset_load_requests->empty())
	{
		s_load_asset_request request;
		g_asset_load_requests->pop_front(request);
		c_string_id asset_id = request.asset_definition.id;

		// first check if we have the asset loaded already
		bool found;
		s_asset_internal& asset = g_active_assets->find_or_insert(asset_id.get_id(), found);

		if (found)
		{
			ASSERT(asset.ref_count > 0);
			ASSERT(asset.memory.is_valid());
			asset.ref_count++;

			if (request.callback != nullptr)
			{
				request.callback(asset.asset_id, asset.asset, request.object);
			}

			log_message(verbose, "asset_system: existing asset already loaded [asset: {s}, file: {s}]",
				request.asset_definition.id.get_debug_string(),
				request.asset_definition.path.get_full_path());
		}
		else
		{
			ASSERT(!asset.memory.is_valid());
			t_string_256 file_path;
			request.asset_definition.path.get_path_string(file_path);
			s_file_info file_info = get_file_info(file_path);
			if (file_info.exists)
			{
				t_file_open_mode_flags flags(file_open_mode_read);

				c_file file;
				if (file.open(request.asset_definition.path, flags))
				{
					// we may want a separate arena for assets, or just have asset system hold it's own allocator
					void* data = c_memory_system::allocate(file_info.size_bytes, alignof(byte), memory_arena_frame);
					ASSERT(data != nullptr);
					
					// make temp buffer
					asset.memory = c_array<byte>(static_cast<byte*>(data), uint64_to_int32(file_info.size_bytes));

					int32 bytes_read = file.read_bytes(0, asset.memory.capacity(), asset.memory);
					ASSERT(bytes_read == file_info.size_bytes);
					file.close();

					asset.ref_count++;
					asset.asset_id = asset_id;

					switch (request.asset_definition.type)
					{
					case asset_type_bitmap:
					{
						s_bitmap_asset* new_asset = ALLOCATE_GLOBAL_NO_CONSTRUCTOR(s_bitmap_asset, memory_arena_system);
						ASSERT(new_asset != nullptr);
						process_bitmap_asset(&asset.memory, *new_asset);
						asset.asset = new_asset;
						break;
					}
					case asset_type_wav:
					{
						s_wav_asset* new_asset = ALLOCATE_GLOBAL_NO_CONSTRUCTOR(s_wav_asset, memory_arena_system);
						ASSERT(new_asset != nullptr);

						process_wav_asset(&asset.memory, *new_asset);
						asset.asset = new_asset;
						break;
					}
					default:
						HALT_UNIMPLEMENTED();
						break;
					}

					if (request.callback != nullptr)
					{
						request.callback(asset.asset_id, asset.asset, request.object);
					}

					log_message(verbose, "asset_system: loaded new asset [asset: {s}, file: {s}, size: {i}]",
						request.asset_definition.id.get_debug_string(),
						request.asset_definition.path.get_full_path(),
						bytes_read);
				}
				else
				{
					g_active_assets->remove(asset_id.get_id());
					log_message(critical, "asset_system: failed to open asset file! [asset: {s}, file: {s}]",
						request.asset_definition.id.get_debug_string(),
						request.asset_definition.path.get_full_path());
				}
			}
			else
			{
				ASSERT(g_active_assets->remove(asset_id.get_id()));
				log_message(warning, "asset_system: file not found [asset: {s}, file: {s}]",
					request.asset_definition.id.get_debug_string(),
					request.asset_definition.path.get_full_path());
			}
		}
	}
}

// private

//  move this to bitmap_asset.h?
#pragma pack(push, 1)
struct s_bitmap_file_header
{
	char bfType[2];
	uint32 bfSize;
	uint16 bfReserved1;
	uint16 bfReserved2;
	uint32 bfOffBits;
};

struct s_bitmap_info_header
{
	uint32 biSize;
	int32 width;
	int32 height;
	uint16 biPlanes;
	uint16 bits_per_pixel;
	uint32 compression; // 3 == no compression
	uint32 image_size_bytes; // in bytes
	int32 biXPelsPerMeter;
	int32 biYPelsPerMeter;
	uint32 biClrUsed;
	uint32 biClrImportant;

	uint32 red_mask;
	uint32 green_mask;
	uint32 blue_mask;
	uint32 alpha_mask;
};
#pragma pack(pop)

uint32 bit_scan_forward(uint32 value)
{
	// TODO: use intrinsic if available
	uint32 out = k_invalid;

	for (int32 i = 0; i < 32; i++)
	{
		if (value & (1 << i))
		{
			out = i;
			break;
		}
	}

	return out;
}

void process_bitmap_asset(c_array<byte> const_ptr asset_data, s_bitmap_asset& out_bitmap_asset)
{
	const s_bitmap_file_header* header = reinterpret_cast<const s_bitmap_file_header*>(asset_data->data());
	ASSERT(header != nullptr);
	ASSERT(memory_compare(header->bfType, "BM", sizeof(char) * 2) == 0);
	ASSERT(header->bfType[0] == 'B' && header->bfType[1] == 'M'); //remove
	const s_bitmap_info_header* core = reinterpret_cast<const s_bitmap_info_header*> (asset_data->data() + sizeof(s_bitmap_file_header));

	ASSERT(core->bits_per_pixel == sizeof(uint32) * 8);

	uint32 red_mask = core->red_mask;
	uint32 green_mask = core->green_mask;
	uint32 blue_mask = core->blue_mask;
	uint32 alpha_mask = core->alpha_mask;

	out_bitmap_asset.height = core->height;
	out_bitmap_asset.width = core->width;

	uint32 red_shift = bit_scan_forward(red_mask);
	uint32 green_shift = bit_scan_forward(green_mask);
	uint32 blue_shift = bit_scan_forward(blue_mask);
	uint32 alpha_shift = bit_scan_forward(alpha_mask);

	int32 pixel_count = core->height * core->width;
	ASSERT(pixel_count * sizeof(uint32) == core->image_size_bytes);

	uint32* pixels = static_cast<uint32*>(c_memory_system::allocate(sizeof(uint32) * pixel_count, alignof(uint32), memory_arena_system));
	out_bitmap_asset.pixels = c_array<uint32>(pixels, pixel_count);

	c_array<uint32> temp_pixels(reinterpret_cast<uint32*>(asset_data->data() + header->bfOffBits), pixel_count);
	ASSERT(temp_pixels.capacity() == out_bitmap_asset.pixels.capacity());
	
	// we are a top down render system, bitmap is bottom up. flip the rows
	// while processing the initial asset to keep runtime logic consistent.
	int32 dest_y = 0;
	for (int32 source_y = (out_bitmap_asset.height - 1); source_y >= 0; source_y--, dest_y++)
	{
		int32 dest_x = 0;
		for (int32 source_x = 0; source_x < out_bitmap_asset.width; source_x++, dest_x++)
		{
			uint32 source_pixel = temp_pixels[source_y * out_bitmap_asset.width + source_x];
			uint32& dest_pixel = out_bitmap_asset.pixels[dest_y * out_bitmap_asset.width + dest_x];
			
			// we store pixels as 0xAARRGGBB, convert any different ordering up front
			// to keep runtime logic consistent
			dest_pixel =
				(((source_pixel >> alpha_shift) & 0xFF) << 24) |
				(((source_pixel >> red_shift) & 0xFF) << 16) |
				(((source_pixel >> green_shift) & 0xFF) << 8) |
				(((source_pixel >> blue_shift) & 0xFF) << 0);
		}
	}
}

// move this to wav_asset.h

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

void convert_samples_to_real32(
	c_array<byte> samples,
	e_audio_sample_type sample_type,
	int32 channel_count,
	int32 block_align,
	t_audio_buffer_real32 const_ptr out_buffer)
{
	switch (sample_type)
	{
	case audio_sample_type_int8:
	{
		HALT_UNIMPLEMENTED();
		break;
	}
	case audio_sample_type_int16:
	{
		constexpr uint32 max = math_pow<uint32>(2, 15);
		const real32 divisor = 1 / static_cast<real32>(max);
		const int32 bytes_per_sample = 2;
		
		int32 sample_index = 0;
		for (int32 block_index = 0; block_index < samples.capacity(); block_index += block_align)
		{
			for (int32 channel_index = 0; channel_index < channel_count; channel_index++)
			{
				const int32 byte_index = block_index + (bytes_per_sample * channel_index);
				const byte* sample = samples.get_item(byte_index);

				out_buffer->get_channel(channel_index)[sample_index] =
					static_cast<int16>(sample[0] | sample[1] << 8) * divisor;
			}

			sample_index++;
		}

		ASSERT(sample_index == out_buffer->size());
		break;
	}
	case audio_sample_type_int24:
	{
		// note for (confused) future self: we shift these bits into the upper 3 bytes
		// of a 4 byte integer so that we preserve the sign of the 24bit value. if we
		// only shifted them up 3 (0, 8, 16) we would lose the sign and all negative
		// values would be incorrect. to account for this, our divisor has to be based
		// off uint32_max
		constexpr uint32 max = math_pow<uint32>(2, 31);
		const real32 divisor = 1.0f / static_cast<real32>(max);
		const int32 bytes_per_sample = 3;

		int32 sample_index = 0;
		for (int32 block_index = 0; block_index < samples.capacity(); block_index += block_align)
		{
			for (int32 channel_index = 0; channel_index < channel_count; channel_index++)
			{
				const int32 byte_index = block_index + (bytes_per_sample * channel_index);
				const byte* sample = samples.get_item(byte_index);

				out_buffer->get_channel(channel_index)[sample_index] = 
					((sample[0] << 8) | (sample[1] << 16) | (sample[2] << 24)) * divisor;
			}

			sample_index++;
		}

		ASSERT(sample_index == out_buffer->size());
		break;
	}
	case audio_sample_type_real32:
	{
		// needs testing with real sample
		HALT_UNIMPLEMENTED(); 

		const real32 divisor = 1.0f / int64_to_real32((int32_to_int64(k_int32_max) + 1));
		const int32 bytes_per_sample = 4;

		int32 sample_index = 0;
		for (int32 block_index = 0; block_index < samples.capacity(); block_index += block_align)
		{
			for (int32 channel_index = 0; channel_index < channel_count; channel_index++)
			{
				const int32 byte_index = block_index + (bytes_per_sample * channel_index);
				const byte* in_sample = samples.get_item(byte_index);

				real32 sample = (*reinterpret_cast<const int32*>(in_sample)) * divisor;

				out_buffer->get_channel(channel_index)[sample_index] = sample;
			}

			sample_index++;
		}

		ASSERT(sample_index == out_buffer->size());
		break;
	}
	default:
	{
		HALT_UNIMPLEMENTED();
		break;
	}
	}
}

void convert_and_resample_to_real32(
	c_array<byte> samples,
	e_audio_sample_type sample_type,
	int32 channel_count,
	int32 block_align,
	int32 source_sample_rate,
	int32 target_sample_rate,
	t_audio_buffer_real32 const_ptr out_buffer)
{
	//todo
}

void process_wav_asset(c_array<byte> const_ptr asset_data, s_wav_asset& out_wav_asset)
{
	uint32 index = 0;
	s_audio_wav_header_riff riff_chunk;
	memory_copy(&riff_chunk, &asset_data->data()[index], sizeof(s_audio_wav_header_riff));

	index += sizeof(s_audio_wav_header_riff);

	s_audio_wav_header_format format_chunk;
	memory_copy(&format_chunk, &asset_data->data()[index], sizeof(s_audio_wav_header_format));

	e_audio_sample_type sample_type = get_sample_type_from_bits_per_sample(format_chunk.bits_per_sample);
	int32 channel_count = format_chunk.channel_count;

	index += sizeof(s_audio_wav_header_format);

	// now we're in uncharted territory. look for the "data" chunk
	s_audio_wav_header_chunk chunk;
	bool data_chunk_found = false;

	while (!data_chunk_found)
	{
		if (index + sizeof(s_audio_wav_header_chunk) >= asset_data->capacity())
		{
			HALT_UNIMPLEMENTED();
		}

		memory_copy(&chunk, &asset_data->data()[index], sizeof(s_audio_wav_header_chunk));

		data_chunk_found = memory_compare(chunk.name, "data", sizeof(chunk.name)) == 0;

		if (!data_chunk_found)
		{
			// the next chunk will start chunk_size bytes away
			index += sizeof(s_audio_wav_header_chunk) + chunk.chunk_size;
		}
	}

	int32 data_position = index + sizeof(s_audio_wav_header_chunk);
	uint32 sample_count = chunk.chunk_size / format_chunk.block_align;

	ASSERT(data_position > (sizeof(s_audio_wav_header_riff) + sizeof(s_audio_wav_header_format) + sizeof(s_audio_wav_header_chunk)));
	ASSERT(sample_count > 0);

	// todo: generalize, recombine
	if (format_chunk.sample_rate != audio_system_get_sample_rate())
	{
		real64 source_rate = format_chunk.sample_rate;
		real64 target_rate = audio_system_get_sample_rate();
		
		real64 ratio = source_rate / target_rate;
		int32 target_samples = sample_count * ratio;

		real32* samples = static_cast<real32*>(c_memory_system::allocate(sizeof(real32) * target_samples * channel_count, alignof(real32), memory_arena_system));
		out_wav_asset.buffer = t_audio_buffer_real32(channel_count, target_samples);
		out_wav_asset.buffer.set_data(samples);

		const int32 bytes_per_sample = format_chunk.bits_per_sample / 8;

		ASSERT(asset_data->capacity() - data_position == sample_count * format_chunk.block_align);
		ASSERT(format_chunk.block_align == bytes_per_sample * channel_count);

		int32 start_index = data_position;

		// linear resampling
		for (int32 target_sample = 0; target_sample < target_samples; target_sample++)
		{
			real64 source_index = (static_cast<real32>(target_sample) / target_samples) * sample_count;
			int32 source_index_lo = source_index;
			int32 source_index_hi = source_index_lo + 1;

#ifndef CONFIG_RELEASE
			if (target_sample == target_samples - 1)
			{
				ASSERT(source_index_hi == sample_count - 1);
			}
#endif

			real32 weight_hi = source_index - source_index_lo;
			real32 weight_lo = 1.0f - weight_hi;

			if (source_index_hi >= sample_count)
			{
				weight_hi = 0.0f;
			}

			for (int32 channel_index = 0; channel_index < channel_count; channel_index++)
			{
				int32 byte_index_lo = start_index + (format_chunk.block_align * source_index_lo) + (bytes_per_sample * channel_index);
				int32 byte_index_hi = byte_index_lo + format_chunk.block_align;

				const byte* sample_bytes_lo = asset_data->get_item(byte_index_lo);
				const byte* sample_bytes_hi = asset_data->get_item(byte_index_hi);

				real32 sample_lo = 0;
				real32 sample_hi = 0;

				switch (sample_type)
				{
				case audio_sample_type_int16:
				{
					constexpr uint32 max = math_pow<uint32>(2, 15);
					const real32 divisor = 1 / static_cast<real32>(max);

					sample_lo = static_cast<int16>(sample_bytes_lo[0] | sample_bytes_lo[1] << 8) * divisor;
					sample_hi = static_cast<int16>(sample_bytes_hi[0] | sample_bytes_hi[1] << 8) * divisor;
					break;
				}
				case audio_sample_type_int24:
				{
					constexpr uint32 max = math_pow<uint32>(2, 31);
					const real32 divisor = 1.0f / static_cast<real32>(max);

					sample_lo = ((sample_bytes_lo[0] << 8) | (sample_bytes_lo[1] << 16) | (sample_bytes_lo[2] << 24)) * divisor;
					sample_hi = ((sample_bytes_hi[0] << 8) | (sample_bytes_hi[1] << 16) | (sample_bytes_hi[2] << 24)) * divisor;
					break;
				}
				default:
					HALT_UNIMPLEMENTED();
					break;
				}

				real32 sample = (sample_lo * weight_lo) + (sample_hi * weight_hi);
				out_wav_asset.buffer.get_channel(channel_index)[target_sample] = sample;
			}
		}
	}
	else
	{
		real32* samples = static_cast<real32*>(c_memory_system::allocate(sizeof(real32) * sample_count * channel_count, alignof(real32), memory_arena_system));
		out_wav_asset.buffer = t_audio_buffer_real32(channel_count, sample_count);
		out_wav_asset.buffer.set_data(samples);

		int32 bytes_to_read = sample_count * format_chunk.block_align;
		const int32 bytes_per_sample = format_chunk.bits_per_sample / 8;

		ASSERT(asset_data->capacity() - data_position == bytes_to_read);
		ASSERT(format_chunk.block_align == bytes_per_sample * channel_count);


		int32 start_index = data_position;
		int32 end_index = data_position + bytes_to_read - 1;
		ASSERT(end_index == asset_data->capacity() - 1);
	
		convert_samples_to_real32(
			asset_data->make_sub_array(start_index, end_index),
			sample_type,
			channel_count,
			format_chunk.block_align,
			&out_wav_asset.buffer);
	}

}