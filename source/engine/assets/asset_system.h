#ifndef __ASSET_SYSTEM_H__
#define __ASSET_SYSTEM_H__
#pragma once

#include "engine/engine_system.h"

//using t_asset_handle = uint64;



class c_asset_system : public c_engine_system<c_asset_system>
{
public:
	void init();
	void term();
	void update();

	static_member_function bool load_asset(const s_asset_definition* asset_def, f_asset_loaded_callback* callback, void* object);
	static_member_function const s_asset* get_asset(c_string_id asset_id);
	static_member_function void unload_asset(c_string_id asset_id);
};

#endif //__ASSET_SYSTEM_H__