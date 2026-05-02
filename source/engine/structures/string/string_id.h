#ifndef __STRING_ID_H__
#define __STRING_ID_H__
#pragma once

#include "config.h"
#include "string.h"
#include "algorithms/algorithms.h"
#include "types/types.h"
#include "structures/string/string.h"
#include "memory/memory.h"

#define DECLARE_STRING_ID(variable, string) constexpr c_string_id variable = c_string_id(string)
//#define DECLARE_STRING_ID(variable, string) constexpr c_string_id variable = c_string_id(fnv1a_string_hash_64(string, string_length(string)), string)

class c_string_id
{
public:
	c_string_id() : m_hash(k_invalid) DEBUG_ONLY_PARAM_LEFT_COMMA(d_debug_string(nullptr)) {}
	
	constexpr c_string_id(const char* string) :
		m_hash(fnv1a_string_hash_64(string, string_length(string)))
		DEBUG_ONLY_PARAM_LEFT_COMMA(d_debug_string(string)) {}

	constexpr c_string_id(uint64 hash DEBUG_ONLY_PARAM_LEFT_COMMA(const char* string)) :
		m_hash(hash)
		DEBUG_ONLY_PARAM_LEFT_COMMA(d_debug_string(string)) {}

	c_string_id(const c_string_id& other) :
		m_hash(other.m_hash)
		DEBUG_ONLY_PARAM_LEFT_COMMA(d_debug_string(other.d_debug_string)) {}

	void copy_from(const c_string_id& other)
	{
		memory_copy(this, &other, sizeof(c_string_id));
	}

	bool is_valid() const
	{
		return m_hash != k_invalid;
	}

	constexpr bool operator==(const c_string_id& other) const
	{
		const bool equals = m_hash == other.m_hash;

#ifdef CONFIG_DEBUG
		if (equals)
		{
			ASSERT(string_compare(d_debug_string, other.d_debug_string) == 0);
		}
#endif // CONFIG_DEBUG

		return equals;
	}

	constexpr bool operator!=(const c_string_id& other) const
	{
		return!(*this == other);
	}

#ifdef CONFIG_DEBUG
	// not sure how this would be needed outside debug, so leaving out for now
	constexpr uint64 get_id() const { return m_hash; }
	
	const char* get_debug_string() const
	{
		return d_debug_string;
	}
#endif //CONFIG_DEBUG

private:

	// may want these some day, but for now keep it simple
	c_string_id(c_string_id&& other) = delete;
	c_string_id& operator=(const char* string) = delete;
	c_string_id& operator=(const c_string_id& other) = delete;
	c_string_id& operator=(const c_string_id&& other) = delete;

	const uint64 m_hash;

#ifdef CONFIG_DEBUG
	const char* d_debug_string;
#endif //CONFIG_DEBUG
};

#endif //__STRING_ID_H__
