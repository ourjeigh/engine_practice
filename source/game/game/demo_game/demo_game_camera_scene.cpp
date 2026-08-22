#include "game/demo_game/demo_game_state_machine.h"
#include "input/input_utilities.h"
#include "debug/debug_helpers.h"
#include "collision/collision.h"

// menu (MOVE)
void c_demo_game_flow_menu_scene::on_enter(s_demo_game_state_menu_scene* state_data, real32 dt, bool& out_continue)
{
	out_continue = false;
}

void c_demo_game_flow_menu_scene::on_update(s_demo_game_state_menu_scene* state_data, real32 dt, bool& out_continue)
{
	out_continue = true;

	engine_render_fill_screen(k_color_black.to_uint32());

	t_string_128 title("TITLE");
	engine_render_draw_string(title, 200, 200, 4, k_color_white, render_layer_main);

	auto menu_items = make_static_array<t_string_128>({
		"CAMERA",
		"COLLISION",
		"EXIT" });

	menu_items[state_data->selection].append("-");

	if (engine_input_get_key_state(input_key_arrow_up).is_down)
	{
		state_data->selection--;
		engine_input_consume_key_state(input_key_arrow_up);
	}
	else if (engine_input_get_key_state(input_key_arrow_down).is_down)
	{
		state_data->selection++;
		engine_input_consume_key_state(input_key_arrow_down);
	}

	if (state_data->selection < 0)
	{
		state_data->selection = demo_game_menu_selection_camera_scene;
	}
	else if (state_data->selection >= k_demo_game_menu_selection_count)
	{
		state_data->selection = demo_game_menu_selection_exit;
	}

	if (engine_input_get_key_state(input_key_special_return).is_down)
	{
		engine_input_consume_key_state(input_key_special_return);
		switch (state_data->selection)
		{
		case demo_game_menu_selection_camera_scene:
		case demo_game_menu_selection_collision_scene:
			out_continue = false;
			break;
		case demo_game_menu_selection_exit:
			engine_request_exit();
			break;
		}
	}

	const int32 x = 225;
	int32 y = 300;
	for (int32 item_index = 0; item_index < menu_items.capacity(); item_index++)
	{
		engine_render_draw_string(menu_items[item_index], x, y, 3, k_color_white, render_layer_main);
		y += 30;
	}
}

void c_demo_game_flow_menu_scene::on_exit(s_demo_game_state_menu_scene* state_data, real32 dt, bool& out_continue) 
{
	out_continue = false;
}

// camera scene

const s_asset_definition k_test_bmp_asset_def = { "test_bmp", asset_scope_global, asset_type_bitmap, R"(C:\Users\RJ\git\simm_engine\assets\test\dude.bmp)" };

void c_demo_game_flow_camera_scene::on_enter(s_demo_game_state_camera_scene* state_data, real32 dt, bool& out_continue)
{
	if (engine_get_asset(k_test_bmp_asset_def.id) == nullptr)
	{
		engine_load_asset(&k_test_bmp_asset_def, nullptr, nullptr);
	}
	else
	{
		state_data->player.m_transform.reset();
		state_data->player.m_bitmap_asset_id = k_test_bmp_asset_def.id;
		state_data->camera.set_transform(s_transform::default_values());
		state_data->camera.set_zoom(1.0f);
		state_data->camera.set_width(10.0f);
		state_data->camera.set_screen_dimensions(engine_get_screen_dimensions());

		out_continue = false;
	}
}

void c_demo_game_flow_camera_scene::on_update(s_demo_game_state_camera_scene* state_data, real32 dt, bool& out_continue)
{
	engine_render_fill_screen(k_color_black);
	const s_input_state* input_state = engine_input_get_input_state();

	// update camera
	{
		real32 zoom = state_data->camera.get_zoom();
		if (zoom < 1.0f)
		{
			zoom *= 1.05f;
			state_data->camera.set_zoom(zoom);
		}

		t_vector_4d_real32 move_delta;
		get_wads_key_move_delta(input_state, move_delta);
		const real32 camera_speed_meters_per_second = 1.5f;
		move_delta *= camera_speed_meters_per_second * dt;
		s_transform camera_transfom = state_data->camera.get_transform();
		camera_transfom.position += move_delta;
		state_data->camera.set_transform(camera_transfom);

		// TODO: bring rotation update over
	}

	// update player
	{
		t_vector_4d_real32 move_delta;
		get_arrow_key_move_delta(input_state, move_delta);
		const real32 player_speed_meters_per_second = 2.0f;
		move_delta *= player_speed_meters_per_second * dt;
		state_data->player.m_transform.position += move_delta;
	}

	const s_bitmap_asset* player_bitmap = static_cast<const s_bitmap_asset*>(engine_get_asset(state_data->player.m_bitmap_asset_id));
	if (player_bitmap != nullptr)
	{
		// todo: make position a well defined type with w hardcoded to 1
		state_data->player.m_transform.position.w() = 1.0f;

		t_render_shape_point render_pos = state_data->camera.world_position_to_screen_space(state_data->player.m_transform.position);
		t_render_shape_rect rect;
		rect.x = render_pos.x() - player_bitmap->width * state_data->camera.get_zoom() / 2;
		rect.y = render_pos.y() - player_bitmap->height * state_data->camera.get_zoom() / 2;
		rect.width = player_bitmap->width * state_data->camera.get_zoom();
		rect.height = player_bitmap->height * state_data->camera.get_zoom();
		engine_render_bitmap(
			*player_bitmap,
			rect,
			render_layer_main);
	}

#ifdef CONFIG_DEBUG
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

void c_demo_game_flow_camera_scene::on_exit(s_demo_game_state_camera_scene* state_data, real32 dt, bool& out_continue)
{
	out_continue = false;
}


// collision scene (MOVE)
void c_demo_game_flow_collision_scene::on_enter(s_demo_game_state_collision_scene* state_data, real32 dt, bool& out_continue)
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
	{
		c_object& ground = state_data->scene_objects.push();
		ground.m_transform.reset();
		ground.m_transform.scale.set(100, 0.01f, 0.01f, 0);
		t_string_128 name("ground");
		ground.set_name(name);
	}

	state_data->camera.set_transform(s_transform::default_values());
	state_data->camera.set_position({ 0, 2, 0, 1 });
	state_data->camera.set_rotation(k_vector_4d_rotation_forward);
	state_data->camera.set_zoom(1.0f);
	state_data->camera.set_width(10.0f);
	state_data->camera.set_screen_dimensions(engine_get_screen_dimensions());

	out_continue = false;
}

void resolve_collision(c_object& in_out_object, s_collision_info& collision, real32 dt)
{
	t_vector_4d_real32 position = in_out_object.m_transform.position;
	t_vector_4d_real32 velocity = in_out_object.get_velocity();

	// first move the position to the collision position with a slight offset
	position += (velocity * dt * (collision.t - 0.1f));

	// then remove the hit normal from the current velocity
	real32 t_remainder = dt * (1.0f - (collision.t));
	t_vector_4d_real32 reflect_velocity = collision.normal * velocity.dot(collision.normal);
	velocity -= reflect_velocity;
	//position += velocity * t_remainder;
	in_out_object.set_velocity(velocity);
	in_out_object.m_transform.position = position;
}

void c_demo_game_flow_collision_scene::on_update(s_demo_game_state_collision_scene* state_data, real32 dt, bool& out_continue)
{
	engine_render_fill_screen(k_color_black);

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
	state_data->player.set_velocity(player_velocity);
	//state_data->player.apply_force(player_velocity);
	//t_vector_4d_real32 new_player_position = state_data->player.get_move_delta(dt);

	s_collision_info collision_info;
	bool collides = false;

	int32 passes_remaining = 4;
	real32 dt_remaining = 1.0f;
	t_vector_4d_real32 new_player_position = state_data->player.get_move_delta(dt);

	while (passes_remaining-- > 0 && dt_remaining > 0.0f)
	{
		for (const auto& object : state_data->scene_objects)
		{
			// velocity and positions may be updated by previous collisions in this loop so we
			// always get the latest move
			real32 dt_actual = dt * dt_remaining;
			new_player_position = state_data->player.get_move_delta(dt_actual);
			engine_log_verbose("player collision: checking {s}. remaining: {f.3} velocity: " VECTOR_4D_REAL32_FORMAT,
				object.get_name().get_const_char(),
				dt_remaining,
				VECTOR_4D_REAL32_ARGS(state_data->player.get_velocity()));

			bool object_collides = aabb_intersect_aabb_test_3d(
				state_data->player.m_transform.position,
				new_player_position - state_data->player.m_transform.position,
				state_data->player.get_collision_rect_3d().to_aabb(),
				object.get_collision_rect_3d().to_aabb(),
				collision_info);

			if (object_collides)
			{
				collides = true;
				passes_remaining--;
				dt_remaining -= collision_info.t;
				resolve_collision(state_data->player, collision_info, dt_actual);

				engine_log_verbose("player collision detected: {s}. t: {f.3} remaining: {f.3} new velocity: " VECTOR_4D_REAL32_FORMAT,
					object.get_name().get_const_char(),
					collision_info.t,
					dt_remaining,
					VECTOR_4D_REAL32_ARGS(state_data->player.get_velocity()));

#ifdef CONFIG_DEBUG
				s_render_shape_circle hit_mark;
				hit_mark.center = state_data->camera.world_position_to_screen_space(collision_info.position);
				hit_mark.radius = 15;
				engine_render_draw_circle(hit_mark, k_color_white, true, render_layer_debug);

				s_render_shape_line hit_normal;
				hit_normal.p1 = state_data->camera.world_position_to_screen_space(collision_info.position);
				hit_normal.p2 = state_data->camera.world_position_to_screen_space(collision_info.position + collision_info.normal);
				engine_render_draw_line(hit_normal.p1, hit_normal.p2, k_color_white, render_layer_debug);
#endif //CONFIG_DEBUG
			}
		}

		if (!collides)
		{
			passes_remaining = 0;
		}
	}

	state_data->player.m_transform.position = new_player_position;

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

void c_demo_game_flow_collision_scene::on_exit(s_demo_game_state_collision_scene* state_data, real32 dt, bool& out_continue)
{
	out_continue = false;
}