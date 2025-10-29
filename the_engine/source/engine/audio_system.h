#ifndef __AUDIO_SYSTEM_H__
#define __AUDIO_SYSTEM_H__
#pragma once

#include "engine/engine_system.h"

class c_audio_system : public c_engine_system
{
public:
	virtual void init() override;
	virtual void term() override;
	virtual void update() override;
};

#endif//__AUDIO_SYSTEM_H__