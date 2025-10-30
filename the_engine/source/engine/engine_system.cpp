#include "engine_system.h"
#include <types/types.h>
#include "audio/audio_system.h"
#include "input/input_system.h"
#include "asserts.h"

const int32 k_max_game_systems = 12;

static c_input_system g_input_system;
static c_audio_system g_audio_system;

void engine_systems_init()
{
	// initialize all engine systems here
	g_input_system.init();
	g_audio_system.init();
}

void engine_systems_term()
{
	// terminate all engine systems here
	g_input_system.term();
	g_audio_system.term();
}

void engine_systems_update()
{
	// update all engine systems here
	g_input_system.update();
	g_audio_system.update();
}
