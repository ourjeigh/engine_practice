#include "assets/asset_system.h"
#include "debug/logging.h"
#include "memory/memory_system.h"
#include "platform/platform_file.h"
#include "platform/platform_handle.h"
#include "structures/string/string_id.h"
#include "threads/threads.h"
#include "structures/hash_set.h"
#include "threads/threadsafe_queue.h"

const int32 k_max_asset_load_requests = 32;
const int32 k_max_active_assets = 128;

struct s_load_asset_request
{
	c_file_path file_path;
	void* object;
	f_asset_loaded_callback* callback;
};

struct s_asset_internal
{
	t_asset_handle asset_handle;
	int32 ref_count;
	c_array<byte> memory;
};

struct s_asset_internal_comparitor
{
	bool operator()(const t_asset_handle& lhs, const t_asset_handle& rhs)
	{
		return lhs == rhs;
	}
};

struct s_asset_internal_hasher
{
	uint32 operator()(const t_asset_handle& value)
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

using t_asset_request_stack = c_static_spsc_queue<s_load_asset_request, k_max_asset_load_requests>;
t_asset_request_stack* g_asset_load_requests;

using t_asset_hash_set = c_hash_map<t_asset_handle, s_asset_internal, k_max_active_assets, s_asset_internal_hasher, s_asset_internal_comparitor>;
t_asset_hash_set* g_active_assets;

c_asset_loader_thread g_asset_loader_thread;

void c_asset_system::init()
{
	g_asset_load_requests = ALLOCATE_NEW_GLOBAL(t_asset_request_stack, memory_arena_system);
	g_active_assets = ALLOCATE_NEW_GLOBAL(t_asset_hash_set, memory_arena_system);

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

bool c_asset_system::load_asset(c_file_path& file_path, void* object, f_asset_loaded_callback* callback)
{
	ASSERT(g_asset_load_requests != nullptr);
	
	bool success = false;

	s_load_asset_request request;
	request.file_path = file_path;
	request.object = object;
	request.callback = callback;
	
	if (g_asset_load_requests->push_back(request))
	{
		success = true;
	}

	return success;
}

const c_array<byte>* c_asset_system::get_asset_data(t_asset_handle handle)
{
	ASSERT(get_current_thread_id() != g_asset_loader_thread.get_thread_id());

	s_asset_internal asset;
	
	if (g_active_assets->try_find_or_insert(handle, &asset))
	{
		return &asset.memory;
	}

	return nullptr;
}

void c_asset_system::unload_asset(t_asset_handle handle)
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
	int32 processed = 0;
	s_load_asset_request request;

	while (g_asset_load_requests->pop_front(request))
	{
		processed++;
		// first check if we have the asset loaded already
		t_string_256 file_path;
		request.file_path.get_path_string(file_path);
		t_asset_handle hash = fnv1a_hash_64(file_path.get_const_char(), sizeof(char) * file_path.used());

		s_asset_internal asset;


		if (g_active_assets->try_find_or_insert(hash, &asset))
		{
			request.callback(asset.asset_handle, request.object);

			log_message(verbose, "asset_system: existing asset already loaded[file: {s}, handle: {u}]",
				request.file_path.get_full_path(),
				asset.asset_handle);
		}
		else
		{
			s_file_info file_info = get_file_info(file_path);
			if (file_info.exists)
			{
				t_file_open_mode_flags flags;
				flags.set(file_open_mode_read, true);

				c_file file;
				if (file.open(request.file_path, flags))
				{
					// we may want a separate arena for assets, or just have asset system hold it's own allocator
					void* data = c_memory_system::allocate(file_info.size_bytes, alignof(byte), memory_arena_system);
					ASSERT(data != nullptr);
					asset.memory = c_array<byte>(static_cast<byte*>(data), uint64_to_int32(file_info.size_bytes));

					int32 bytes_read = file.read_bytes(0, asset.memory.capacity(), asset.memory);
					ASSERT(bytes_read == file_info.size_bytes);
					file.close();

					asset.ref_count = 1;
					request.callback(asset.asset_handle, request.object);
					
					log_message(verbose, "asset_system: loaded new asset [file: {s}, handle: {u}, size: {i}]",
						request.file_path.get_full_path(),
						asset.asset_handle,
						bytes_read);
				}
				else
				{
					g_active_assets->remove(asset.asset_handle);
					log_message(critical, "asset_system: failed to open asset file! [file:{s}]", request.file_path.get_full_path());
				}
			}
			else
			{
				log_message(warning, "asset_system: file not found at: {s}", request.file_path.get_full_path());
			}
		}
	}

	ASSERT(g_asset_load_requests->empty());
}