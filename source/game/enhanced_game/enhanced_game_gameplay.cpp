#include "enhanced_game_state_machine.h"
#include "debug/debug_helpers.h"
#include "input/input_utilities.h"
#include "collision/collision.h"

const t_plane_3d_real32 k_ground_plane = t_plane_3d_real32::from_point_and_normal(t_vector_4d_real32::zero(), k_vector_4d_direction_up);
//const t_plane_3d_real32 k_ramp_plane = t_plane_3d_real32::from_point_and_normal({ 4,0,0,1 }, {1,1,0,0});

void c_game_flow_state_gameplay::on_enter(s_flow_state_gameplay* state_data, real32 dt, bool& out_continue)
{
	state_data->scene_objects.clear();
	state_data->player.m_transform.reset();
	state_data->player.m_transform.position.y() = 2;
	state_data->player.m_transform.scale *= 0.5f;
	{
		c_object& dummy = state_data->scene_objects.push();
		dummy.m_transform.reset();
		dummy.m_transform.position.set(1, 0.5f, 0, 1);
		t_string_128 name("dummy 1");
		dummy.set_name(name);
	}
	{
		c_object& dummy = state_data->scene_objects.push();
		dummy.m_transform.reset();
		dummy.m_transform.position.set(-1, 0.5f, 0, 1);
		t_string_128 name("dummy 2");
		dummy.set_name(name);
	}

	state_data->camera.set_transform(s_transform::default_values());
	state_data->camera.set_position({ 0, 2, 0, 1 });
	state_data->camera.set_rotation(k_vector_4d_rotation_forward);
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

	const s_input_state* input_state = engine_input_get_input_state();
	t_vector_4d_real32 player_velocity;
	get_arrow_key_move_delta(input_state, player_velocity);

	// update camera
	{
		t_vector_4d_real32 camera_rotation = state_data->camera.get_transform().rotation;
		if (try_update_camera_direction_from_input(input_state, camera_rotation))
		{
			state_data->camera.set_rotation(camera_rotation);
		}
	}

	const real32 player_speed_meters_per_second = 5;
	player_velocity *= player_speed_meters_per_second;
	state_data->player.apply_force(player_velocity);
	t_vector_4d_real32 new_player_position = state_data->player.get_move_delta(dt);

	s_collision_info collision_info;
	bool collides = false;

	t_vector_4d_real32 origin = { 0, 2, 0, 1 };
	t_vector_4d_real32 end = new_player_position;

	for (/*const*/ auto& object : state_data->scene_objects)
	{
		bool object_collides = aabb_intersect_aabb_test_3d(
			state_data->player.m_transform.position,
			new_player_position - state_data->player.m_transform.position,
			state_data->player.get_collision_rect_3d().to_aabb(),
			object.get_collision_rect_3d().to_aabb(),
			collision_info);

		if (object_collides)
		{
			engine_log_verbose("player collision: {s}", object.get_name().get_const_char());
			collides = true;
		}
	}

	/*collides |= aabb_intersect_plane_test_3d(
		state_data->player.m_transform.position,
		new_player_position - state_data->player.m_transform.position,
		state_data->player.get_collision_rect_3d().to_aabb(),
		k_ground_plane,
		collision_info);*/

	collides |= aabb_intersect_plane_test_3d(
		state_data->player.m_transform.position,
		new_player_position - state_data->player.m_transform.position,
		state_data->player.get_collision_rect_3d().to_aabb(),
		k_ground_plane,
		collision_info);

	/*collides |= aabb_intersect_plane_test_3d(
		state_data->player.m_transform.position,
		new_player_position - state_data->player.m_transform.position,
		state_data->player.get_collision_rect_3d().to_aabb(),
		k_ramp_plane,
		collision_info);*/

	/*engine_render_draw_line(
		state_data->camera.world_position_to_screen_space(origin),
		state_data->camera.world_position_to_screen_space(new_player_position),
		collides ? k_color_blue : k_color_white,
		render_layer_debug);*/

	if (collides)
	{
		s_render_shape_circle hit_mark;
		hit_mark.center = state_data->camera.world_position_to_screen_space(collision_info.position);
		hit_mark.radius = 15;
		engine_render_draw_circle(hit_mark, k_color_white, true, render_layer_debug);

		s_render_shape_line hit_normal;
		hit_normal.p1 = state_data->camera.world_position_to_screen_space(collision_info.position);
		hit_normal.p2 = state_data->camera.world_position_to_screen_space(collision_info.position + collision_info.normal);
		engine_render_draw_line(hit_normal.p1, hit_normal.p2, k_color_white, render_layer_debug);
		state_data->player.set_velocity(t_vector_4d_real32::zero());
		new_player_position = state_data->player.m_transform.position + (player_velocity * dt *(collision_info.t - 0.1f));
		//state_data->player.m_transform.position = new_player_position;
	}
	else
	{
		state_data->player.apply_move_delta(dt);
	}

	s_render_shape_line ground_line = state_data->camera.world_plane_to_screen_space(k_ground_plane);
	engine_render_draw_line(ground_line.p1, ground_line.p2, k_color_red, render_layer_main);

	/*s_render_shape_line ramp_line = state_data->camera.world_plane_to_screen_space(k_ramp_plane);
	engine_render_draw_line(ramp_line.p1, ramp_line.p2, k_color_red, render_layer_main);*/

#ifdef CONFIG_DEBUG
	t_render_shape_rect player_rect = state_data->camera.world_rect_to_screen_space(state_data->player.get_collision_rect_3d());
	engine_render_draw_rect(
		player_rect,
		k_color_red,
		false,
		render_layer_debug);

	t_vector_2d_real32 player_center = state_data->player.m_transform.position.xy();
	t_aabb_3d_real32 player_aabb = state_data->player.get_collision_rect_3d().to_aabb();
	for (const auto& object : state_data->scene_objects)
	{
		c_color color = k_color_blue;
		auto aabb = object.get_collision_rect_3d().to_aabb();

		if (aabb.is_valid())
		{
			if (aabb.overlaps_other(player_aabb))
			{
				color = k_color_green;
			}

			engine_render_draw_rect(
				state_data->camera.world_rect_to_screen_space(object.get_collision_rect_3d()),
				color,
				false,
				render_layer_debug);
		}
	}

	//draw_debug_world_grid(20, state_data->camera);

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
		t_string_256 camera_viewport_string;
		state_data->camera.get_viewport_debug_string(camera_viewport_string);
		engine_render_draw_string(camera_viewport_string, view_align_x, 35, 1, k_color_white, render_layer_debug);
	}
#endif // CONFIG_DEBUG
}

void c_game_flow_state_gameplay::on_exit(s_flow_state_gameplay* state_data, real32 dt, bool& out_continue)
{
	out_continue = false;
}