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
	
	void move(real32 dt)
	{
		m_transform.position += m_velocity * dt;
	}

	t_rect_2d_real32 get_move_collision(real32 dt) const
	{
		t_vector_4d_real32 new_position = m_transform.position + m_velocity * dt;
		
		t_rect_2d_real32 out;
		out.x = new_position.x() - (m_transform.scale.x() * 0.5f);
		out.y = new_position.y() - (m_transform.scale.y() * 0.5f);
		out.width = m_transform.scale.x();
		out.height = m_transform.scale.y();

		return out;
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

	void set_velocity(const t_vector_4d_real32& new_velocity)
	{
		const real32 hack_drag = 0.95f;
		m_velocity = (new_velocity * (1.0f - hack_drag)) + (m_velocity * hack_drag);
	}

	s_transform m_transform;

private:
	t_vector_4d_real32 m_velocity;
};

class c_player : public c_object
{
public:
	c_string_id m_bitmap_asset_id;
};
#endif // !__PLAYER_H__