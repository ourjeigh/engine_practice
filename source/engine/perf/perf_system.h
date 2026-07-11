#ifndef __PERF_SYSTEM_H__
#define __PERF_SYSTEM_H__
#pragma once

#include "config.h"
#ifdef FEATURE_PERF_MEASUREMENT

#include "perf/perf.h"
#include "engine/engine_system.h"
#include "time/time.h"
#include "structures/string/string_id.h"

class c_perf_system : public c_engine_system<c_perf_system>
{
public:
	void init();
	void term();
	void update();

	void set_capture_state(bool active);

	void generate_report();
	static_member_function void report_perf_message(const s_perf_measurement& measurement);

private:
	bool m_is_capture_active;
};

#endif //FEATURE_PERF_MEASUREMENT
#endif // __PERF_SYSTEM_H__
