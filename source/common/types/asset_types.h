#ifndef __ASSET_TYPES_H__
#define __ASSET_TYPES_H__
#pragma once

#include "file/file_path.h"
#include "structures/string/string_id.h"

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
	// get c_string_id into common and make this a c_string_id
	c_string_id id;
	e_asset_scope scope;
	c_file_path path;
};

#endif // !__ASSET_TYPES_H__
