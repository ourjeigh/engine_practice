#ifndef __PLAYER_H__
#define __PLAYER_H__
#pragma once

#include "structures/string/string_id.h"
#include "transform.h"
#include "structures/shapes.h"

class c_object
{
public:
	void move(const t_vector_4d_real32& move_delta)
	{
		m_transform.position += move_delta;
	}

	t_rect_2d_real32 get_collision_rect() const
	{
		t_rect_2d_real32 out;
		out.x = m_transform.position.x() - (m_transform.scale.x() * 0.5f);
		out.y = m_transform.position.y() - (m_transform.scale.y() * 0.5f);
		out.width = m_transform.scale.x();
		out.height = m_transform.scale.y();

		ASSERT(out.center().is_equal(m_transform.position.xy()));
		return out;
	}

	s_transform m_transform;
};

class c_player : public c_object
{
public:
	c_string_id m_bitmap_asset_id;
};
#endif // !__PLAYER_H__