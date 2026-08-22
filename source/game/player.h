#ifndef __PLAYER_H__
#define __PLAYER_H__
#pragma once

#include "structures/string/string_id.h"
#include "transform.h"
#include "structures/shapes.h"

class c_object
{
public:
	void move_to(t_vector_4d_real32& new_position)
	{
		m_transform.position = new_position;
	}

	void apply_move_delta(real32 dt)
	{
		m_transform.position += m_velocity * dt;
	}

	t_vector_4d_real32 get_move_delta(real32 dt)
	{
		return m_transform.position + m_velocity * dt;
	}

	t_rect_2d_real32 get_collision_rect_2d() const
	{
		t_rect_2d_real32 out;
		out.x = m_transform.position.x() - (m_transform.scale.x() * 0.5f);
		out.y = m_transform.position.y() - (m_transform.scale.y() * 0.5f);
		out.width = m_transform.scale.x();
		out.height = m_transform.scale.y();

		ASSERT(out.center().is_equal(m_transform.position.xy()));
		return out;
	}

	t_rect_3d_real32 get_collision_rect_3d() const
	{
		t_rect_3d_real32 out;
		out.x = m_transform.position.x() - (m_transform.scale.x() * 0.5f);
		out.y = m_transform.position.y() - (m_transform.scale.y() * 0.5f);
		out.z = m_transform.position.z() - (m_transform.scale.z() * 0.5f);
		out.width = m_transform.scale.x();
		out.height = m_transform.scale.y();
		out.depth = m_transform.scale.z();

		ASSERT(out.center().is_equal(m_transform.position));
		return out;
	}
	
	t_vector_4d_real32 get_velocity() const
	{
		return m_velocity;
	}

	void set_velocity(const t_vector_4d_real32& velocity)
	{
		m_velocity = velocity;
	}

	void apply_force(const t_vector_4d_real32& force)
	{
		const real32 hack_drag = 0.90f;
		m_velocity = (force * (1.0f - hack_drag)) + (m_velocity * hack_drag);
		//t_vector_4d_real32 new_acceleration = force / m_mass;
		//m_acceleration += new_acceleration;
	}

	s_transform m_transform;

#ifdef CONFIG_DEBUG
	void set_name(const c_string& name)
	{
		// hacks
		d_name = *(new t_string_128());
		d_name.copy_from(name);
	}

	c_string_const get_name() const
	{
		return d_name.as_string_const();
	}

#endif //CONFIG_DEBUG
private:
	t_vector_4d_real32 m_velocity;
	t_vector_4d_real32 m_acceleration;
	const real32 m_mass = 1.0f;

#ifdef CONFIG_DEBUG
	t_string_128 d_name;
#endif //CONFIG_DEBUG
};

class c_player : public c_object
{
public:
	c_string_id m_bitmap_asset_id;
};
#endif // !__PLAYER_H__