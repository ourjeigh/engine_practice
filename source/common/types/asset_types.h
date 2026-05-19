#ifndef __ASSET_TYPES_H__
#define __ASSET_TYPES_H__
#pragma once

#include "file/file_path.h"
#include "structures/string/string_id.h"

struct s_asset
{
};

struct s_bitmap_asset : s_asset
{
	int32 width;
	int32 height;

	c_array<uint32> pixels;

	int32 size() { return width * height; }
};

enum e_asset_type
{
	asset_type_wav,
	asset_type_bitmap,

	k_asset_type_count
};

enum e_asset_scope
{
	asset_scope_global,

	k_asset_scope_count
};

struct s_asset_definition
{
	c_string_id id;
	e_asset_scope scope;
	e_asset_type type;
	c_file_path path;
};

using f_asset_loaded_callback = void(c_string_id, s_asset*, void*);

#endif // !__ASSET_TYPES_H__
