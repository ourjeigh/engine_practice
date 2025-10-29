#include "audio_system.h"
#include <logging/logging.h>


void c_audio_system::init()
{
	log(verbose, "Audio System Initialized");
}

void c_audio_system::term()
{
	log(verbose, "Audio System Terminated");
}

void c_audio_system::update()
{
	log(verbose, "Audio System Update");
}