#include "engine_system.h"
#include <types/types.h>
#include "structures/array.h"
#include "audio_system.h"

const int32 k_max_game_systems = 12;

static c_audio_system g_audio_system;

void engine_systems_init()
{
	// initialize all engine systems here
	g_audio_system.init();
}

void engine_systems_term()
{
	// terminate all engine systems here
	g_audio_system.term();
}

void engine_systems_update()
{
	// update all engine systems here
	g_audio_system.update();
}