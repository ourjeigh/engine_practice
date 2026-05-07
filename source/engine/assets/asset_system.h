#ifndef __ASSET_SYSTEM_H__
#define __ASSET_SYSTEM_H__
#pragma once

#include "engine/engine_system.h"

using t_asset_handle = uint64;
using f_asset_loaded_callback = void(t_asset_handle, void*);

class c_asset_system : public c_engine_system<c_asset_system>
{
public:
	void init();
	void term();
	void update();

	static_function bool load_asset(c_file_path& file_path, void* object, f_asset_loaded_callback* callback);
	static_function const c_array<byte>* get_asset_data(t_asset_handle handle);
	static_function void unload_asset(t_asset_handle handle);
};

#endif //__ASSET_SYSTEM_H__