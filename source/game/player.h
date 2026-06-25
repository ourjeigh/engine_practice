#ifndef __PLAYER_H__
#define __PLAYER_H__
#pragma once

#include "transform.h"
#include "structures/string/string_id.h"

class c_object
{
public:
	s_transform m_transform;
};

class c_player : public c_object
{
public:
	c_string_id m_bitmap_asset_id;
};
#endif // !__PLAYER_H__