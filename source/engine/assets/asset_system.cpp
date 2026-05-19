#include "assets/asset_system.h"
#include "debug/logging.h"
#include "memory/memory_system.h"
#include "platform/platform.h"
//#include "platform/platform_file.h"
#include "platform/platform_handle.h"
#include "structures/string/string_id.h"
#include "threads/threads.h"
#include "structures/hash_set.h"
#include "threads/threadsafe_queue.h"

const int32 k_max_asset_load_requests = 32;
const int32 k_max_active_assets = 128;

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
	static_function void asset_loader_thread_entry_point(c_asset_loader_thread* thread);
	void process_asset_loads();

	bool m_is_running;
};

void process_new_asset(s_load_asset_request& request, s_asset_internal& asset);
void process_bitmap_asset(c_array<byte> const_ptr asset_data, s_bitmap_asset& out_bitmap_info);

using t_asset_request_stack = c_static_spsc_queue<s_load_asset_request, k_max_asset_load_requests>;
t_asset_request_stack* g_asset_load_requests;

using t_asset_hash_map = c_hash_map<uint64, s_asset_internal, k_max_active_assets, s_asset_internal_hasher, s_asset_internal_comparitor>;
t_asset_hash_map* g_active_assets;

c_asset_loader_thread g_asset_loader_thread;

void c_asset_system::init()
{
	g_asset_load_requests = ALLOCATE_NEW_GLOBAL(t_asset_request_stack, memory_arena_system);
	g_active_assets = ALLOCATE_NEW_GLOBAL(t_asset_hash_map, memory_arena_system);

	ASSERT(g_asset_load_requests != nullptr);
	ASSERT(g_active_assets != nullptr);

	g_asset_load_requests->clear();
	g_active_assets->clear();

	g_asset_loader_thread.init();
}

void c_asset_system::term()
{
	g_asset_loader_thread.term();
}


void c_asset_system::update()
{
}

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

// returning a non-const pointer to the memory so that things like
// bitmap loading can re-arrange pixel masks if needed. In the long-term
// maybe the asset system is responsible for processing things like that
// so we can go back to just returning read-only memory
c_array<byte>* c_asset_system::get_asset_data(c_string_id asset_id)
{
	ASSERT(get_current_thread_id() != g_asset_loader_thread.get_thread_id());

	s_asset_internal& asset = g_active_assets->find(asset_id.get_id());
	
	if (asset.memory.is_valid())
	{
		return &asset.memory;
	}

	return nullptr;
}

s_asset* c_asset_system::get_asset(c_string_id asset_id)
{
	// we'll need to actually be storing the asset itself instead of just the raw memory
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
		thread->process_asset_loads();
	}
}

void c_asset_loader_thread::process_asset_loads()
{
	s_load_asset_request request;

	while (g_asset_load_requests->pop_front(request))
	{
		// first check if we have the asset loaded already
		
		c_string_id asset_id = request.asset_definition.id;

		s_asset_internal& asset = g_active_assets->find_or_insert(asset_id.get_id());

		if (asset.ref_count > 0)
		{
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
				t_file_open_mode_flags flags;
				flags.set(file_open_mode_read, true);

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

					asset.ref_count = 1;
					asset.asset_id = asset_id;

					switch (request.asset_definition.type)
					{
					case asset_type_bitmap:
					{
						s_bitmap_asset* new_asset = ALLOCATE_GLOBAL_NO_CONSTRUCTOR(s_bitmap_asset, memory_arena_system);

						process_bitmap_asset(&asset.memory, *new_asset);
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
					g_active_assets->remove(asset.asset_id.get_id());
					log_message(critical, "asset_system: failed to open asset file! [asset: {s}, file: {s}]",
						request.asset_definition.id.get_debug_string(),
						request.asset_definition.path.get_full_path());
				}
			}
			else
			{
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
	ASSERT(header->bfType[0] == 'B' && header->bfType[1] == 'M');
	const s_bitmap_info_header* core = reinterpret_cast<const s_bitmap_info_header*> (asset_data->data() + sizeof(s_bitmap_file_header));

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