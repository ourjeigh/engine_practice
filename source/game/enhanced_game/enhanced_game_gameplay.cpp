#include "enhanced_game_state_machine.h"
#include "debug/debug_helpers.h"
#include "input/input_utilities.h"

struct s_collision_info
{
	real32 t;
	t_vector_4d_real32 position;
};


bool ray_intersect_test(
	const t_vector_4d_real32& p1,
	const t_vector_4d_real32& p2,
	const t_aabb_2d_real32& static_aabb,
	s_collision_info& out_collision_info)
{
	t_vector_4d_real32 direction = p2 - p1;

	const real32 top_y = static_aabb.max_xy.y();
	const real32 bottom_y = static_aabb.min_xy.y();
	const real32 left_x = static_aabb.min_xy.x();
	const real32 right_x = static_aabb.max_xy.x();

	real32 t_min = 1.0f;

	{
		// top/bottom
		real32 t_top = safe_divide(top_y - p1.y(), direction.y(), k_real32_max);
		real32 t_bottom = safe_divide(bottom_y - p1.y(), direction.y(), k_real32_max);
		real32 t = math_min(t_top, t_bottom);
		if (in_range_inc_l_exc_r(0.0f, t_min, t))
		{
			t_vector_4d_real32 collision_position = p1 + (direction * t);
			if (in_range_inclusive(left_x, right_x, collision_position.x()))
			{
				t_min = t;
			}
		}
	}
	{
		// left/right
		real32 t_left = safe_divide(left_x - p1.x(), direction.x(), k_real32_max);
		real32 t_right = safe_divide(right_x - p1.x(), direction.x(), k_real32_max);
		real32 t = math_min(t_left, t_right);
		if (in_range_inc_l_exc_r(0.0f, t_min, t))
		{ 
			t_vector_4d_real32 collision_position = p1 + (direction * t);
			if (in_range_inclusive(bottom_y, top_y, collision_position.y()))
			{
				t_min = t;
			}
		}
	}

	bool collides = false;
	if (in_range_inc_l_exc_r(0.0f, 1.0f, t_min))
	{
		collides = true;
		out_collision_info.position = p1 + (direction * t_min);
		out_collision_info.t = t_min;
	}

	return collides;
}

bool aabb_intersect_test(
	const t_vector_4d_real32& p1,
	const t_vector_4d_real32& p2,
	const t_aabb_2d_real32& moving_aabb,
	const t_aabb_2d_real32& static_aabb,
	s_collision_info& out_collision_info)
{
	t_aabb_2d_real32 aabb_sum = static_aabb.add(moving_aabb);
	return ray_intersect_test(p1, p2, aabb_sum, out_collision_info);
}

void c_game_flow_state_gameplay::on_enter(s_flow_state_gameplay* state_data, real32 dt, bool& out_continue)
{
	state_data->scene_objects.clear();
	state_data->player.m_transform.reset();
	state_data->player.m_transform.scale *= 0.5f;
	{
		c_object& dummy = state_data->scene_objects.push();
		dummy.m_transform.reset();
		dummy.m_transform.position.set(1, 0, 0, 1);
	}
	{
		c_object& dummy = state_data->scene_objects.push();
		dummy.m_transform.reset();
		dummy.m_transform.position.set(-1, 0, 0, 1);
	}

	state_data->camera.set_transform(s_transform::default_values());
	state_data->camera.set_zoom(1.0f);
	state_data->camera.set_width(10.0f);
	state_data->camera.set_screen_dimensions(engine_get_screen_dimensions());
	out_continue = false;
}

void c_game_flow_state_gameplay::on_update(s_flow_state_gameplay* state_data, real32 dt, bool& out_continue)
{
	engine_render_fill_screen(k_color_black.to_uint32());

	t_string_128 title("Gameplay!");
	engine_render_draw_string(title, 600, 300, 5, k_color_white, render_layer_ui);

	auto input_state = engine_input_get_input_state();
	t_vector_4d_real32 player_velocity;
	get_arrow_key_move_delta(input_state, player_velocity);

	const real32 player_speed_meters_per_second = 5;
	player_velocity *= player_speed_meters_per_second;
	state_data->player.apply_force(player_velocity);
	t_vector_4d_real32 new_player_position = state_data->player.get_move_delta(dt);

	s_collision_info collision_info;
	bool collides = false;

	for (const auto& object : state_data->scene_objects)
	{
		collides |= aabb_intersect_test(
			state_data->player.m_transform.position,
			new_player_position,
			state_data->player.get_collision_rect().to_aabb(),
			object.get_collision_rect().to_aabb(),
			collision_info);
	}

	engine_render_draw_line(
		state_data->camera.world_position_to_screen_space(state_data->player.m_transform.position),
		state_data->camera.world_position_to_screen_space(new_player_position),
		k_color_white,
		render_layer_debug);

	if (collides)
	{
		s_render_shape_circle hit_mark;
		hit_mark.center = state_data->camera.world_position_to_screen_space(collision_info.position);
		hit_mark.radius = 15;
		engine_render_draw_circle(hit_mark, k_color_white, true, render_layer_debug);
		state_data->player.set_velocity(t_vector_4d_real32::zero());
		new_player_position = state_data->player.m_transform.position + (player_velocity * dt *(collision_info.t - 0.01f));
	}
	else
	{
		state_data->player.apply_move_delta(dt);
	}

#ifdef CONFIG_DEBUG
	engine_render_draw_rect(
		state_data->camera.world_rect_to_screen_space(state_data->player.get_collision_rect()),
		k_color_red,
		false,
		render_layer_debug);

	t_vector_2d_real32 player_center = state_data->player.m_transform.position.xy();
	t_aabb_2d_real32 player_aabb = state_data->player.get_collision_rect().to_aabb();
	for (const auto& object : state_data->scene_objects)
	{
		c_color color = k_color_blue;
		auto aabb = object.get_collision_rect().to_aabb();

		if (aabb.is_valid())
		{
			if (aabb.overlaps_other(player_aabb))
			{
				color = k_color_green;
			}

			engine_render_draw_rect(
				state_data->camera.world_rect_to_screen_space(object.get_collision_rect()),
				color,
				false,
				render_layer_debug);
		}
	}

	draw_debug_world_grid(20, state_data->camera);

	const int32 align_x = 1170;
	{
		t_string_128 player_position_string;
		player_position_string.printf("Player: {f6.2}, {f6.2}, {f6.2}",
			state_data->player.m_transform.position.x(),
			state_data->player.m_transform.position.y(),
			state_data->player.m_transform.position.z());

		engine_render_draw_string(player_position_string, align_x, 5, 1, k_color_white, render_layer_debug);
	}
	{
		t_string_128 camera_position_string;
		camera_position_string.printf("Camera: {f6.2}, {f6.2}, {f6.2}",
			state_data->camera.get_transform().position.x(),
			state_data->camera.get_transform().position.y(),
			state_data->camera.get_transform().position.z());

		engine_render_draw_string(camera_position_string, align_x, 15, 1, k_color_white, render_layer_debug);
	}
	{
		t_string_128 camera_viewport_title("Camera Viewport");
		engine_render_draw_string(camera_viewport_title, align_x, 25, 1, k_color_white, render_layer_debug);
		const int32 view_align_x = align_x + 65;
		t_string_128 camera_viewport_string;
		state_data->camera.get_viewport_debug_string(camera_viewport_string);
		engine_render_draw_string(camera_viewport_string, view_align_x, 35, 1, k_color_white, render_layer_debug);
	}
#endif // CONFIG_DEBUG
}

void c_game_flow_state_gameplay::on_exit(s_flow_state_gameplay* state_data, real32 dt, bool& out_continue)
{
	out_continue = false;
}