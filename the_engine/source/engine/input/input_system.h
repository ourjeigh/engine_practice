#ifndef __INPUT_SYSTEM_H__
#define __INPUT_SYSTEM_H__
#pragma once

#include "engine/engine_system.h"
#include <types/types.h>
#include <time/time.h>

typedef uint16 t_message_id;
typedef uint64 t_param;

struct s_input_queued_event
{
	t_message_id message_id;
	t_param param;
	t_timestamp timestamp;
};

class c_input_system : public c_engine_system
{
public:
	virtual void init() override;
	virtual void term() override;
	virtual void update() override;

};

bool input_system_queue_message(t_message_id message_id, t_param param);
#endif//__INPUT_SYSTEM_H__