#include "perf.h"
#ifdef FEATURE_PERF_MEASUREMENT

#include "perf_system.h"

s_stack_perf_measurer::s_stack_perf_measurer(c_string_id id) : id(id)
{
	timer.start();
}

s_stack_perf_measurer::~s_stack_perf_measurer()
{
	timer.stop();
	s_perf_measurement measurement(
		id,
		*timer.get_time_span(),
		get_current_thread_id());

	c_perf_system::report_perf_message(measurement);
}
#endif //FEATURE_PERF_MEASUREMENT
