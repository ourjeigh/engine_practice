#include "perf_system.h"

#ifdef FEATURE_PERF_MEASUREMENT
#include "debug/logging.h"
#include "structures/hash_set.h"

struct s_perf_measurement_set
{
	s_perf_measurement_set() :
		unique_id(),
		count(0),
		total_time_microseconds(0),
		min_duration(k_uint64_max),
		max_duration(0) {}

	// since a callsite could run on multiple threads, we want to track it's usage on each thread separately
	// we use the measurement id and thread id to form the key in the map storage.
	struct s_unique_id
	{
		s_unique_id() : measurement_id(), thread_id(k_invalid) {}

		c_string_id measurement_id;
		uint32 thread_id;

		s_unique_id operator=(const s_unique_id& other)
		{
			measurement_id.copy_from(other.measurement_id);
			thread_id = other.thread_id;
			return *this;
		}

		bool operator==(const s_unique_id& other) const
		{
			return measurement_id == other.measurement_id && thread_id == other.thread_id;
		}
	};

	s_unique_id unique_id;
	uint64 count;
	uint64 total_time_microseconds;
	uint64 min_duration;
	uint64 max_duration;
};

// we don't want to store perf data in allocated memory because it would get stomped by record
static_global c_static_stack<s_perf_measurement, 1024> g_measurement_messages;
static_global c_hash_map<s_perf_measurement_set::s_unique_id, s_perf_measurement_set, 1024> g_measurements;
uint64 g_ticks;

void c_perf_system::init() 
{
	// should start capturing on init?
	m_is_capture_active = true;
	g_ticks = 0;
}

void c_perf_system::term()
{
	m_is_capture_active = false;
	generate_report();
}

void c_perf_system::update()
{
	// check if capture state needs to change

	if (m_is_capture_active)
	{
		g_ticks++;

		for (auto it = g_measurement_messages.begin(); it != g_measurement_messages.end(); ++it)
		{
			uint64 duration = it->span.get_delta_raw();

			// a raw timestamp delta of 0 is not meaningful
			if (duration > 0)
			{
				s_perf_measurement_set::s_unique_id id;
				zero_object(id);

				id.measurement_id.copy_from(it->id);
				id.thread_id = it->thread_id;
				
				bool found;
				s_perf_measurement_set& set = g_measurements.find_or_insert(id, found);
				
				set.unique_id.measurement_id.copy_from(it->id);
				set.unique_id.thread_id = it->thread_id;
				set.count++;
				set.total_time_microseconds += duration;
				set.min_duration = math_min(set.min_duration, duration);
				set.max_duration = math_max(set.max_duration, duration);
			}
		}
	}

	g_measurement_messages.clear();
}

void c_perf_system::set_capture_state(bool active)
{
	m_is_capture_active = active;
}

bool compare_measurements(const void* l, const void* r)
{
	const s_perf_measurement_set* left = static_cast<const s_perf_measurement_set*>(l);
	const s_perf_measurement_set* right = static_cast<const s_perf_measurement_set*>(r);

	return (left->total_time_microseconds / left->count) > (right->total_time_microseconds / right->count);
}

void c_perf_system::generate_report()
{
	// dump to file...
	log_message(verbose, "::: PERF REPORT :::");

	c_static_array<s_perf_measurement_set, 1024> measurements;
	g_measurements.to_array(measurements);

	// why does this need as_array()?
	quick_sort(measurements.as_array(), compare_measurements, 0, g_measurements.used());

	for (int32 i = 0; i < g_measurements.used(); i++)
	{
		const s_perf_measurement_set& set = measurements[i];
		const char* name = set.unique_id.measurement_id.get_debug_string();

		// all times in microseconds
		real64 total_time = set.total_time_microseconds;
		real64 avg_time = total_time / set.count;

		real64 max_time = c_engine_time_span(0, set.max_duration).get_duration_microseconds();
		real64 min_time = c_engine_time_span(0, set.min_duration).get_duration_microseconds();

		int32 calls_per_tick = math_max<int32>(1, math_floor<real32>(set.count / static_cast<real32>(g_ticks)));
		real64 avg_time_per_tick = avg_time * calls_per_tick;

		log_message(verbose, "{s45}: avg time: {f6}(us)\tmin time: {f6}(us)\tmax time: {f6}(us)\tcalls per tick: {i5}\tavg time/tick:{f6}(us)",
			name,
			avg_time,
			min_time,
			max_time,
			calls_per_tick,
			avg_time_per_tick);
	}

	log_message(verbose, "::: END PERF REPORT :::");

}

void c_perf_system::report_perf_message(const s_perf_measurement& measurement)
{
	ASSERT(measurement.id.is_valid());
	s_perf_measurement& new_measurement = g_measurement_messages.push();

	memory_copy(&new_measurement, &measurement, sizeof(s_perf_measurement));
}

#endif //FEATURE_PERF_MEASUREMENT
