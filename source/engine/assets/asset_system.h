#ifndef __ASSET_SYSTEM_H__
#define __ASSET_SYSTEM_H__
#pragma once

#include "engine/engine_system.h"

//using t_asset_handle = uint64;
using f_asset_loaded_callback = void(c_string_id, void*);

class c_asset_system : public c_engine_system<c_asset_system>
{
public:
	void init();
	void term();
	void update();

	static_function bool load_asset(const s_asset_definition* asset_def, void* object, f_asset_loaded_callback* callback);
	static_function c_array<byte>* get_asset_data(c_string_id asset_id);
	static_function void unload_asset(c_string_id asset_id);
};

#endif //__ASSET_SYSTEM_H__