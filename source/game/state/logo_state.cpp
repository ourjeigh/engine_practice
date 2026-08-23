#include "logo_state.h"
#include "engine_api.h"

const s_asset_definition k_simm_logo_bmp_asset_def = { "logo_bmp", asset_scope_global, asset_type_bitmap, R"(..\assets\simm_logo.bmp)" };
const s_asset_definition k_simm_logo_wav_asset_def = { "logo_wav", asset_scope_global, asset_type_wav, R"(..\assets\logo_24_48k.wav)" };
/////////////////////////////////////////////////////
// TEMP MOVE

// this is cool but i'm not sure how practical it will be. it combines what would otherwise be const data (asset) with state (loaded), so there's
// not a great place in the game layer to use it. Keeping it as a global would break hot-reload, but storing the entire thing in game state could be wasteful.
template<int32 asset_count>
struct s_asset_definition_list
{
	static_member_function void asset_loaded_callback(c_string_id id, s_asset* asset, void* cookie)
	{
		s_asset_definition_list* asset_list = static_cast<s_asset_definition_list*>(cookie);
		ASSERT(asset_list != nullptr);

		for (int32 i = 0; i < asset_list->assets.capacity(); i++)
		{
			if (asset_list->assets[i].id == id)
			{
				asset_list->loaded.set(i, true);
				return;
			}
		}

		HALT("didn't find asset");
	}

	void load_all()
	{
		for (s_asset_definition& asset : assets)
		{
			engine_load_asset(&asset, asset_loaded_callback, this);
		}
	}

	bool all_assets_loaded()
	{
		return loaded.all();
	}

	c_static_array<s_asset_definition, asset_count> assets;
	c_bit_array<asset_count> loaded;
};

template<int32 asset_count>
constexpr auto make_asset_definition_list(s_asset_definition(&& list)[asset_count])
{
	s_asset_definition_list<asset_count> result;
	for (int32 i = 0; i < asset_count; i++)
	{
		result.assets[i] = list[i];
	}

	result.loaded.clear();

	return static_cast<s_asset_definition_list<asset_count>>(result);
}

#define MAKE_ASSET_DEF_LIST(name, ...) \
	auto name = make_asset_definition_list({__VA_ARGS__})


MAKE_ASSET_DEF_LIST(k_logo_asset_list,
	{ "logo_bmp", asset_scope_global, asset_type_bitmap, R"(..\assets\simm_logo.bmp)" },
	{ "logo_wav", asset_scope_global, asset_type_wav, R"(..\assets\logo_24_48k.wav)" });

inline void render_full_screen_fade(real32 fade_value)
{
	s_screen_dimensions dimensions = engine_get_screen_dimensions();
	t_render_shape_rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.height = dimensions.height;
	rect.width = dimensions.width;

	c_color color = k_color_black;
	real32 alpha = math_pin(0.0f, 1.0f, (1.0f - fade_value));
	color.set_alpha(alpha);

	engine_render_draw_rect(rect, color, true, render_layer_ui);
}
// //TEMP MOVE
/////////////////////////////////////////////////////

void asset_loaded_callback(c_string_id asset_id, s_asset* asset, void* cookie)
{
	bool* assets_loaded = static_cast<bool*>(cookie);
	ASSERT(assets_loaded != nullptr);
	*assets_loaded = true;
}

void c_game_flow_state_logo::on_enter(s_flow_state_logo_data* state_data, real32 dt, bool& out_continue)
{
	if (!k_logo_asset_list.all_assets_loaded())
	//if (!state_data->assets_loaded)
	{
		state_data->fade_in_value = 0.0f;
		state_data->is_fading_in = true;
		//engine_load_asset(&k_simm_logo_bmp_asset_def, asset_loaded_callback, &state_data->assets_loaded);
		k_logo_asset_list.load_all();
		out_continue = true;
	}
	else
	{
		out_continue = false;
	}
}

void c_game_flow_state_logo::on_update(s_flow_state_logo_data* state_data, real32 dt, bool& out_continue)
{
	// let us skip
	if (engine_input_get_key_state(input_key_special_return).is_down)
	{
		out_continue = false;
		return;
	}

	//static_local_variable t_sound_playback_id HACK_id = k_invalid;
	//if (state_data->fade_in_value == 0 && state_data->is_fading_in)
	//{
	//	HACK_id = engine_audio_play_debug_pip();
	//}
	//else if (HACK_id != k_invalid)
	//{
	//	engine_audio_stop_sound(HACK_id);
	//	HACK_id = k_invalid;
	//}

	if (state_data->is_fading_in && state_data->fade_in_value == 0.0f)
	{
		const s_wav_asset* logo_wav_asset = static_cast<const s_wav_asset * >(engine_get_asset(k_simm_logo_wav_asset_def.id));
		if (logo_wav_asset != nullptr)
		{
			engine_audio_play_sound(*logo_wav_asset);
		}
	}

	const s_asset* asset = engine_get_asset(k_simm_logo_bmp_asset_def.id);
	ASSERT(asset != nullptr);

	const real32 fade_in_time_seconds = 2.5f;
	const real32 fade_out_time_seconds = 1.3f;

	real32 fade_time_seonds = state_data->is_fading_in ? fade_in_time_seconds : -fade_out_time_seconds;
	real32 delta = dt / fade_time_seonds;
	state_data->fade_in_value += delta;

	const s_bitmap_asset* bitmap = static_cast<const s_bitmap_asset*>(asset);
	ASSERT(bitmap != nullptr);

	s_screen_dimensions dimensions = engine_get_screen_dimensions();
	t_render_shape_rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.height = dimensions.height;
	rect.width = dimensions.width;
	engine_render_bitmap(*bitmap, rect, render_layer_background);

	render_full_screen_fade(state_data->fade_in_value);

	if (state_data->fade_in_value >= 1.0f)
	{
		state_data->is_fading_in = false;
	}
	else if (state_data->fade_in_value < 0.0f && !state_data->is_fading_in)
	{
		out_continue = false;
	}
}

void c_game_flow_state_logo::on_exit(s_flow_state_logo_data* state_data, real32 dt, bool& out_continue)
{
	out_continue = false;
	// unload asset
}