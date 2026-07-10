#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__
#pragma once

#include "structures/vector.h"

struct s_transform
{
	t_vector_4d_real32 position;
	t_vector_4d_real32 rotation;
	t_vector_4d_real32 scale;

	void set(
		real32 position_x, real32 position_y, real32 position_z,
		real32 rotation_x, real32 rotation_y, real32 rotation_z,
		real32 scale_x, real32 scale_y, real32 scale_z)
	{
		// not ready to support negative scales
		ASSERT(scale_x > 0.0f && scale_y > 0.0f && scale_z > 0.0f);

		position.set(position_x, position_y, position_z, 1);
		rotation.set(rotation_x, rotation_y, rotation_z, 0);
		scale.set(scale_x, scale_y, scale_z, 0);
	}
	
	void reset()
	{
		position.set(0, 0, 0, 1);
		rotation.set_zero();
		scale.set(1, 1, 1, 0);
	}

	static_member_function s_transform default_values()
	{
		s_transform out;
		out.reset();
		return out;
	}
};


#endif //__TRANSFORM_H__