#ifndef __COLLISION_H__
#define __COLLISION_H__
#pragma once

#include "types/types.h"
#include "structures/vector.h"
#include "structures/shapes.h"



struct s_collision_info
{
	real32 t;
	t_vector_4d_real32 position;
	t_vector_4d_real32 normal;
};

// 2d
bool ray_aabb_intersect_test_2d(
	const t_vector_2d_real32& origin,
	const t_vector_2d_real32& direction,
	const t_aabb_2d_real32& static_aabb,
	s_collision_info& out_collision_info);

bool aabb_intersect_aabb_test_2d(
	const t_vector_2d_real32& origin,
	const t_vector_2d_real32& direction,
	const t_aabb_2d_real32& moving_aabb,
	const t_aabb_2d_real32& static_aabb,
	s_collision_info& out_collision_info);

// 3d
bool ray_intersect_aabb_test_3d(
	const t_vector_4d_real32& origin,
	const t_vector_4d_real32& direction,
	const t_aabb_3d_real32& static_aabb,
	s_collision_info& out_collision_info);

bool aabb_intersect_aabb_test_3d(
	const t_vector_4d_real32& origin,
	const t_vector_4d_real32& direction,
	const t_aabb_3d_real32& moving_aabb,
	const t_aabb_3d_real32& static_aabb,
	s_collision_info& out_collision_info);

bool ray_intersect_plane_test_3d(
	const t_vector_4d_real32& origin,
	const t_vector_4d_real32& direction,
	const t_plane_3d_real32& plane,
	s_collision_info& out_collision_info);

bool aabb_intersect_plane_test_3d(
	const t_vector_4d_real32& origin,
	const t_vector_4d_real32& direction,
	const t_aabb_3d_real32& aabb,
	const t_plane_3d_real32& plane,
	s_collision_info& out_collision_info);
#endif // !__COLLISION_H__
